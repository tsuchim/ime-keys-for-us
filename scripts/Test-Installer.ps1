param(
  [Parameter(Mandatory = $true)]
  [string]$MsiPath
)

$ErrorActionPreference = 'Stop'

function Test-IsAdministrator {
  $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
  $principal = [Security.Principal.WindowsPrincipal]::new($identity)
  return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

if (-not (Test-IsAdministrator)) {
  throw 'Installer smoke test requires an already elevated process. This script never triggers UAC elevation.'
}

$msi = Get-Item -LiteralPath $MsiPath
$runPath = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run'
$installedExe = 'C:\Program Files\ImeKeysForUS\ime-keys-for-us.exe'

function Invoke-Msi {
  param([Parameter(Mandatory = $true)][string[]]$Arguments)

  $process = Start-Process msiexec.exe -Wait -PassThru -ArgumentList $Arguments
  if ($process.ExitCode -ne 0) {
    throw "msiexec $($Arguments -join ' ') failed with exit code $($process.ExitCode)."
  }
}

try {
  Get-Process -Name ime-keys-for-us -ErrorAction SilentlyContinue | Stop-Process -Force

  Invoke-Msi @('/i', $msi.FullName, '/qn', '/norestart')
  if (-not (Test-Path $installedExe)) {
    throw "Installed executable was not found at $installedExe."
  }
  if (Get-ItemProperty -Path $runPath -Name ImeKeysForUS -ErrorAction SilentlyContinue) {
    throw 'MSI install should not create the current-user startup Run value.'
  }
} finally {
  Invoke-Msi @('/x', $msi.FullName, '/qn', '/norestart')
}

if (Test-Path $installedExe) {
  throw "Installed executable still exists after MSI uninstall: $installedExe"
}

Write-Host 'Installer smoke test passed.'
