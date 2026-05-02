param(
  [string]$BuildDir = 'build',
  [string]$Generator = '',
  [string]$Architecture = '',
  [switch]$IncludeInstallerSmoke
)

$ErrorActionPreference = 'Stop'

$repo = Resolve-Path (Join-Path $PSScriptRoot '..')
Push-Location $repo
try {
  git diff --check

  $configureArgs = @('-S', '.', '-B', $BuildDir)
  if ($Generator) {
    $configureArgs += @('-G', $Generator)
  }
  if ($Architecture) {
    $configureArgs += @('-A', $Architecture)
  }
  cmake @configureArgs
  cmake --build $BuildDir --config Release

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
  & $wix build installer\wix\Product.wxs -arch x64 -d SourceDir=$($exe.Directory.FullName) -d ProductVersion=$version -o $msi
  if ($LASTEXITCODE -ne 0) {
    throw "wix build failed with exit code $LASTEXITCODE."
  }

  if ($IncludeInstallerSmoke) {
    & (Join-Path $PSScriptRoot 'Test-Installer.ps1') -MsiPath $msi
  }

  Write-Host 'Release preflight passed.'
} finally {
  Pop-Location
}
