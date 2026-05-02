param(
  [Parameter(Mandatory = $true)]
  [string]$ExePath
)

$ErrorActionPreference = 'Stop'

$exe = Get-Item -LiteralPath $ExePath
$runPath = 'HKCU:\Software\Microsoft\Windows\CurrentVersion\Run'

function Invoke-ImeKeysForUS {
  param([Parameter(Mandatory = $true)][string]$Argument)

  $process = Start-Process -FilePath $exe.FullName -ArgumentList $Argument -Wait -PassThru
  if ($process.ExitCode -ne 0) {
    throw "ime-keys-for-us.exe $Argument failed with exit code $($process.ExitCode)."
  }
}

try {
  Invoke-ImeKeysForUS '--disable-startup'
  if (Get-ItemProperty -Path $runPath -Name ImeKeysForUS -ErrorAction SilentlyContinue) {
    throw 'Startup Run value should not exist after --disable-startup.'
  }

  Invoke-ImeKeysForUS '--enable-startup'
  $value = (Get-ItemProperty -Path $runPath -Name ImeKeysForUS -ErrorAction Stop).ImeKeysForUS
  if ($value -notmatch '^".*ime-keys-for-us\.exe" --startup$') {
    throw "Unexpected startup Run value: $value"
  }

  Invoke-ImeKeysForUS '--disable-startup'
  if (Get-ItemProperty -Path $runPath -Name ImeKeysForUS -ErrorAction SilentlyContinue) {
    throw 'Startup Run value should be removed after --disable-startup.'
  }
} finally {
  Start-Process -FilePath $exe.FullName -ArgumentList '--disable-startup' -Wait
}

Write-Host 'Startup registration smoke test passed.'
