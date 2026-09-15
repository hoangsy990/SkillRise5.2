param(
    [string]$SourceClient = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\Client',
    [string]$TargetClient = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client',
    [string]$BuiltEngine = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Bin\Engine-Port S21.exe'
)

$ErrorActionPreference = 'Stop'
$expectedTarget = [IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client')
$resolvedTarget = [IO.Path]::GetFullPath($TargetClient)
if ($resolvedTarget -ne $expectedTarget) {
    throw "Refusing non-isolated target: $resolvedTarget"
}
if (!(Test-Path -LiteralPath $SourceClient -PathType Container)) {
    throw "Source client is missing: $SourceClient"
}
if (!(Test-Path -LiteralPath $BuiltEngine -PathType Leaf)) {
    throw "Isolated Engine-Port S21.exe is missing: $BuiltEngine"
}

$mergedPlayer = Join-Path $TargetClient 'Data\Player\player.bmd'
if (!(Test-Path -LiteralPath $mergedPlayer -PathType Leaf)) {
    throw "Hash-pinned merged player is missing: $mergedPlayer"
}
$mergedHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $mergedPlayer).Hash
if ($mergedHash -ne '0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63') {
    throw "Merged player hash mismatch: $mergedHash"
}

New-Item -ItemType Directory -Force -Path $TargetClient | Out-Null
Get-ChildItem -LiteralPath $SourceClient -File | ForEach-Object {
    if ($_.Name -ne 'Engine.exe') {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $TargetClient $_.Name) -Force
    }
}
$legacyTarget = Join-Path $TargetClient 'Engine.exe'
if (Test-Path -LiteralPath $legacyTarget -PathType Leaf) {
    Remove-Item -LiteralPath $legacyTarget -Force
}
Copy-Item -LiteralPath $BuiltEngine -Destination (Join-Path $TargetClient 'Engine-Port S21.exe') -Force

$sourceScripts = Join-Path $SourceClient 'Scripts'
if (Test-Path -LiteralPath $sourceScripts -PathType Container) {
    Copy-Item -LiteralPath $sourceScripts -Destination $TargetClient -Recurse -Force
}

$targetData = Join-Path $TargetClient 'Data'
New-Item -ItemType Directory -Force -Path $targetData | Out-Null
Get-ChildItem -LiteralPath (Join-Path $SourceClient 'Data') -Directory | ForEach-Object {
    if ($_.Name -notin @('Player', 'RISE')) {
        $link = Join-Path $targetData $_.Name
        if (!(Test-Path -LiteralPath $link)) {
            New-Item -ItemType Junction -Path $link -Target $_.FullName | Out-Null
        }
    }
}

$sourcePlayer = Join-Path $SourceClient 'Data\Player'
$targetPlayer = Join-Path $TargetClient 'Data\Player'
New-Item -ItemType Directory -Force -Path $targetPlayer | Out-Null
Get-ChildItem -LiteralPath $sourcePlayer -File | ForEach-Object {
    if ($_.Name -ine 'player.bmd') {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $targetPlayer $_.Name) -Force
    }
}

if ((Get-FileHash -Algorithm SHA256 -LiteralPath $mergedPlayer).Hash -ne $mergedHash) {
    throw 'Merged player changed during isolated-client preparation'
}

$junctionCount = @(Get-ChildItem -LiteralPath $targetData -Directory |
    Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint }).Count
Write-Output "PASS: isolated client prepared at $TargetClient"
Write-Output "PASS: merged player SHA-256 $mergedHash"
Write-Output "PASS: $junctionCount frozen worktree Data junctions; Player and RISE remain isolated"
