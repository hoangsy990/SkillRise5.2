param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$worktree = [IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer')
$source = 'D:\MU FICA Season 21\Data\Effect\motion_blur.OZJ'
$expected = '934716E4C09EBCA25B43E62BC1F6B69B26663FA54306F5054FA12A354B6DC5AF'
$targets = @(
    'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\SpinStep\motion_blur.OZJ',
    'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client\Data\RISE\GrowLancer\SpinStep\motion_blur.OZJ'
)
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash -ne $expected) {
    throw 'Read-only S21 motion_blur source hash mismatch'
}
foreach ($target in $targets) {
    $absolute = [IO.Path]::GetFullPath($target)
    if (!$absolute.StartsWith($worktree + '\',[StringComparison]::OrdinalIgnoreCase)) {
        throw "Isolated staging path escaped worktree: $target"
    }
    $walk = Split-Path -Parent $absolute
    while ($walk.StartsWith($worktree + '\',[StringComparison]::OrdinalIgnoreCase)) {
        if (Test-Path -LiteralPath $walk) {
            if ((Get-Item -LiteralPath $walk -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) {
                throw "Reparse directory in staging path: $walk"
            }
        }
        $walk = Split-Path -Parent $walk
    }
    if (Test-Path -LiteralPath $absolute) {
        if ((Get-Item -LiteralPath $absolute -Force).Attributes -band [IO.FileAttributes]::ReparsePoint) {
            throw "Refusing reparse asset: $absolute"
        }
        if ((Get-FileHash -Algorithm SHA256 -LiteralPath $absolute).Hash -ne $expected) {
            throw "Existing staged asset conflicts: $absolute"
        }
    }
}
if ($Apply -and @(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count) {
    throw 'Private QA Engine is running; asset stage refused'
}
if (!$Apply) {
    Write-Output "PASS dry-run: source hash=$expected; two isolated targets, no overwrite"
    return
}
foreach ($target in $targets) {
    if (!(Test-Path -LiteralPath $target)) {
        New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
        Copy-Item -LiteralPath $source -Destination $target
    }
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $target).Hash -ne $expected) {
        throw "Staged Spin asset hash mismatch: $target"
    }
}
Write-Output "PASS: original S21 motion_blur staged only to private Grow Lancer package/QA roots; SHA256=$expected"
