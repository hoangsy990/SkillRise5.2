param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$taskRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$qaRoot = Join-Path $taskRoot 'ExGameServer\Tests\GrowLancerBuild'
$built = Join-Path $qaRoot 'Bin\GameServer.exe'
$staged = Join-Path $qaRoot 'ServerStackSmoke\4.MuServer\Sub-1\GameServer\GameServer-GrowLancer-S21-QA.exe'
$backup = Join-Path $qaRoot 'Bin\GameServer-GrowLancer.before-E10E0CDE.exe'
$oldHash = 'EE447339687920A5504DCEF52B7593A2CE7AFE683ED97D1CFE988CBC8EECD390'
$newHash = 'E10E0CDE8D91A00B495A5C34B05A207AD590E919ECFD6E77053BB5E1DACAE854'

foreach ($path in @($built, $staged, $backup))
{
    $absolute = [IO.Path]::GetFullPath($path)
    if (!$absolute.StartsWith($qaRoot + '\', [StringComparison]::OrdinalIgnoreCase))
    {
        throw "Refusing path outside isolated GrowLancerBuild: $absolute"
    }
}
foreach ($path in @($built, $staged))
{
    $item = Get-Item -LiteralPath $path -ErrorAction Stop
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint)
    {
        throw "Refusing reparse executable: $path"
    }
}
if (@(Get-CimInstance Win32_Process -Filter "Name='GameServer-GrowLancer-S21-QA.exe'").Count -ne 0)
{
    throw 'Grow Lancer isolated GameServer is running; stage refused'
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $built).Hash -ne $newHash -or
    (Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $oldHash)
{
    throw 'Built/staged hash preflight mismatch'
}
if (Test-Path -LiteralPath $backup)
{
    $item = Get-Item -LiteralPath $backup -Force
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash)
    {
        throw 'Rollback backup conflict'
    }
}
Write-Output "PASS dry-run: old=$oldHash built=$newHash backup=$backup"
if (!$Apply) { return }
if (!(Test-Path -LiteralPath $backup))
{
    Copy-Item -LiteralPath $staged -Destination $backup
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash)
{
    throw 'Rollback backup hash mismatch'
}
if (@(Get-CimInstance Win32_Process -Filter "Name='GameServer-GrowLancer-S21-QA.exe'").Count -ne 0)
{
    throw 'Grow Lancer isolated GameServer started during preflight'
}
Copy-Item -LiteralPath $built -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash)
{
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'Stage hash mismatch; old QA GameServer restored'
}
Write-Output "PASS staged Grow Lancer isolated GameServer $newHash; rollback $backup"
Write-Output 'No service was launched and no foreign/production server was touched.'
