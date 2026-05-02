param(
  [Parameter(Mandatory = $true)]
  [string]$MsiPath
)

$ErrorActionPreference = 'Stop'

$msi = Get-Item -LiteralPath $MsiPath
$installer = New-Object -ComObject WindowsInstaller.Installer
$database = $installer.GetType().InvokeMember(
  'OpenDatabase',
  'InvokeMethod',
  $null,
  $installer,
  @($msi.FullName, 0))

function Get-MsiProperty {
  param([Parameter(Mandatory = $true)][string]$Name)

  $query = "SELECT `Value` FROM `Property` WHERE `Property`='$Name'"
  $view = $database.GetType().InvokeMember(
    'OpenView',
    'InvokeMethod',
    $null,
    $database,
    @($query))
  $view.GetType().InvokeMember('Execute', 'InvokeMethod', $null, $view, $null) | Out-Null
  $record = $view.GetType().InvokeMember('Fetch', 'InvokeMethod', $null, $view, $null)
  if ($null -eq $record) {
    return $null
  }
  return $record.GetType().InvokeMember('StringData', 'GetProperty', $null, $record, 1)
}

[pscustomobject]@{
  Path = $msi.FullName
  ProductCode = Get-MsiProperty 'ProductCode'
  ProductVersion = Get-MsiProperty 'ProductVersion'
  ProductName = Get-MsiProperty 'ProductName'
  Manufacturer = Get-MsiProperty 'Manufacturer'
  UpgradeCode = Get-MsiProperty 'UpgradeCode'
}
