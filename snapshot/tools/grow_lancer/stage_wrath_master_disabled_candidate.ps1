param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$isolatedRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client'
$source = 'D:\MU FICA Season 21\Data\Interface\new_Master_non_Icon.OZJ'
$target = Join-Path $isolatedRoot 'Data\RISE\GrowLancer\Master\new_Master_non_Icon.OZJ'
$expectedHash = '9FBD4B584C260A97B39E7C5FB93473879ECF96B8646396288BC4542DE36B9D5A'

$absolute = [IO.Path]::GetFullPath($target)
if (!$absolute.StartsWith($isolatedRoot + '\',[StringComparison]::OrdinalIgnoreCase)) {
    throw "Target escaped inactive isolated client: $target"
}
foreach ($path in @($source,$target)) {
    if (Test-Path -LiteralPath $path) {
        if ((Get-Item -LiteralPath $path -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) {
            throw "Refusing reparse file: $path"
        }
    }
}
if ((Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash -ne $expectedHash) {
    throw 'Read-only S21 disabled master atlas hash mismatch'
}
if (Test-Path -LiteralPath $target) {
    if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $expectedHash) {
        throw 'Inactive isolated disabled master atlas conflict'
    }
    Write-Output 'PASS: inactive Grow Lancer disabled master atlas already hash-pinned; no writes'
    return
}
$runningHere = @(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'" |
    Where-Object { $_.ExecutablePath -and $_.ExecutablePath.StartsWith($isolatedRoot + '\',[StringComparison]::OrdinalIgnoreCase) })
if ($runningHere.Count) { throw 'Inactive candidate client process is running; copy refused' }
Write-Output "PASS dry-run: source=$expectedHash target=$target"
if (!$Apply) { return }

New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
if (Test-Path -LiteralPath $target) { throw 'Target appeared during staging; refused' }
Copy-Item -LiteralPath $source -Destination $target
if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $expectedHash) {
    throw 'Inactive disabled master atlas copy verification failed'
}
Write-Output 'PASS: inactive Grow Lancer disabled master atlas staged; S21 source unchanged'
