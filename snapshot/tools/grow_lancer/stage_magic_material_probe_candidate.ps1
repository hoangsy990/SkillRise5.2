param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$taskRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$qaRoot = Join-Path $taskRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-magic-material-141C2859.exe'
$oldHash = '141C2859677F2C4682491A6E0B83BB22B62087E988F8FE0081F008C53DA142B8'
$newHash = '8F544F139D46AD446427A7E0D3FC6322BB6C80CC191362572199D426BC625C67'

foreach ($path in @($candidate, $staged, $backup))
{
    $absolute = [IO.Path]::GetFullPath($path)
    if (!$absolute.StartsWith($qaRoot + '\', [StringComparison]::OrdinalIgnoreCase))
    {
        throw "Refusing path outside isolated RuntimeQA: $absolute"
    }
}
foreach ($path in @($candidate, $staged))
{
    $item = Get-Item -LiteralPath $path -ErrorAction Stop
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint)
    {
        throw "Refusing reparse executable: $path"
    }
}
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count -ne 0)
{
    throw 'Isolated QA executable is running; stage refused'
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $candidate).Hash -ne $newHash)
{
    throw 'Candidate hash mismatch'
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $oldHash)
{
    throw 'Staged executable hash mismatch'
}
if (Test-Path -LiteralPath $backup)
{
    $item = Get-Item -LiteralPath $backup -Force
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash)
    {
        throw 'Existing rollback backup conflict'
    }
}
Write-Output "PASS dry-run: staged=$oldHash candidate=$newHash rollback=$backup"
if (!$Apply) { return }

if (!(Test-Path -LiteralPath $backup))
{
    Copy-Item -LiteralPath $staged -Destination $backup
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash)
{
    throw 'Rollback backup hash mismatch'
}
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count -ne 0)
{
    throw 'Isolated QA executable started during preflight; stage refused'
}
Copy-Item -LiteralPath $candidate -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash)
{
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'Candidate stage hash mismatch; prior QA executable restored'
}
Write-Output "PASS staged private QA candidate $newHash; rollback $backup"
Write-Output 'No client was launched and no production executable was modified.'
