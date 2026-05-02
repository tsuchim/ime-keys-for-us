param(
  [string]$BuildDir = 'build',
  [string]$Generator = '',
  [string]$Architecture = '',
  [switch]$IncludeInstallerSmoke
)

$ErrorActionPreference = 'Stop'

$repo = Resolve-Path (Join-Path $PSScriptRoot '..')

function Invoke-Native {
  param(
    [Parameter(Mandatory = $true)][string]$FilePath,
    [string[]]$Arguments = @()
  )

  & $FilePath @Arguments
  if ($LASTEXITCODE -ne 0) {
    throw "$FilePath $($Arguments -join ' ') failed with exit code $LASTEXITCODE."
  }
}

Push-Location $repo
try {
  $diffArgs = @('diff', '--check')
  if ($env:GITHUB_BASE_REF) {
    $diffArgs += "origin/$env:GITHUB_BASE_REF...HEAD"
  }
  Invoke-Native -FilePath git -Arguments $diffArgs

  $configureArgs = @('-S', '.', '-B', $BuildDir)
  if ($Generator) {
    $configureArgs += @('-G', $Generator)
  }
  if ($Architecture) {
    $configureArgs += @('-A', $Architecture)
  }
  Invoke-Native -FilePath cmake -Arguments $configureArgs
  Invoke-Native -FilePath cmake -Arguments @('--build', $BuildDir, '--config', 'Release')

  $exe = Get-ChildItem $BuildDir -Recurse -Filter ime-keys-for-us.exe |
      Where-Object { $_.FullName -match '\\Release\\' } |
      Select-Object -First 1
  if (-not $exe) {
    throw 'Release executable not found.'
  }

  & (Join-Path $PSScriptRoot 'Test-StartupRegistration.ps1') -ExePath $exe.FullName

  $cmakeLists = Get-Content CMakeLists.txt -Raw
  if ($cmakeLists -notmatch 'project\(ImeKeysForUS VERSION ([0-9]+\.[0-9]+\.[0-9]+)') {
    throw 'Could not resolve project version from CMakeLists.txt.'
  }
  $version = $Matches[1]

  $wix = $null
  if (Test-Path (Join-Path $BuildDir 'wix4\wix.exe')) {
    $wix = (Get-Item (Join-Path $BuildDir 'wix4\wix.exe')).FullName
  }
  if (-not $wix) {
    $wix = (Get-Command wix -ErrorAction SilentlyContinue | Select-Object -First 1).Source
  }
  if (-not $wix) {
    throw 'WiX CLI was not found.'
  }

  $msi = Join-Path $BuildDir "IME-Keys-for-US-$version-x64-unsigned.msi"
  Invoke-Native -FilePath $wix -Arguments @(
    'build',
    'installer\wix\Product.wxs',
    '-arch',
    'x64',
    '-d',
    "SourceDir=$($exe.Directory.FullName)",
    '-d',
    "ProductVersion=$version",
    '-o',
    $msi
  )

  if ($IncludeInstallerSmoke) {
    & (Join-Path $PSScriptRoot 'Test-Installer.ps1') -MsiPath $msi
  }

  Write-Host 'Release preflight passed.'
} finally {
  Pop-Location
}
