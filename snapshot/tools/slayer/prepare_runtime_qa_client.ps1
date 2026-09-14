param(
    [string]$SourceClient = 'D:\RISE-CrossPlatform\Client',
    [string]$PrivateClient = 'D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild\Client',
    [string]$TargetClient = 'D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild\RuntimeQA\Client',
    [string]$BuiltEngine = 'D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild\RuntimeQA\Bin\Engine-Slayer S21.exe'
)

$ErrorActionPreference = 'Stop'
$expectedTarget = [IO.Path]::GetFullPath(
    'D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild\RuntimeQA\Client')
$resolvedTarget = [IO.Path]::GetFullPath($TargetClient)
if ($resolvedTarget -ne $expectedTarget) {
    throw "Refusing non-isolated target: $resolvedTarget"
}
foreach ($required in @($SourceClient, $PrivateClient)) {
    if (!(Test-Path -LiteralPath $required -PathType Container)) {
        throw "Required client directory is missing: $required"
    }
}
if (!(Test-Path -LiteralPath $BuiltEngine -PathType Leaf)) {
    throw "Runtime-QA Engine-Slayer S21.exe is missing: $BuiltEngine"
}

$privatePlayer = Join-Path $PrivateClient 'Data\Player\player.bmd'
if (!(Test-Path -LiteralPath $privatePlayer -PathType Leaf)) {
    throw 'Private Slayer Player\player.bmd is missing'
}
$expectedPlayerHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $privatePlayer).Hash

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
Copy-Item -LiteralPath $BuiltEngine -Destination (Join-Path $TargetClient 'Engine-Slayer S21.exe') -Force

$sourceScripts = Join-Path $SourceClient 'Scripts'
if (Test-Path -LiteralPath $sourceScripts -PathType Container) {
    Copy-Item -LiteralPath $sourceScripts -Destination $TargetClient -Recurse -Force
}

$targetData = Join-Path $TargetClient 'Data'
New-Item -ItemType Directory -Force -Path $targetData | Out-Null

# Keep the login key files at the Data root; directory junctions do not carry files.
Get-ChildItem -LiteralPath (Join-Path $SourceClient 'Data') -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $targetData $_.Name) -Force
}
Get-ChildItem -LiteralPath (Join-Path $SourceClient 'Data') -Directory | ForEach-Object {
    if ($_.Name -notin @('Player', 'RISE')) {
        $link = Join-Path $targetData $_.Name
        if (!(Test-Path -LiteralPath $link)) {
            New-Item -ItemType Junction -Path $link -Target $_.FullName | Out-Null
        }
    }
}

# Player must also be a complete private tree. The earlier stage copied only
# player.bmd, leaving cloth/body textures absent and crashing PhysicsManager
# before login. Copy the base Player tree first, then overlay the merged BMD.
$targetPlayerRoot = Join-Path $targetData 'Player'
New-Item -ItemType Directory -Force -Path $targetPlayerRoot | Out-Null
Get-ChildItem -LiteralPath (Join-Path $SourceClient 'Data\Player') -Force | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $targetPlayerRoot -Recurse -Force
}

# Copy the complete base RISE tree first so Config\Mix.bmd and login assets are
# present, then merge the private Slayer Player/RISE trees.
Copy-Item -LiteralPath (Join-Path $SourceClient 'Data\RISE') -Destination $targetData -Recurse -Force
Get-ChildItem -LiteralPath (Join-Path $PrivateClient 'Data\Player') -Force | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination $targetPlayerRoot -Recurse -Force
}
$privateRise = Join-Path $PrivateClient 'Data\RISE'
if (Test-Path -LiteralPath $privateRise -PathType Container) {
    Get-ChildItem -LiteralPath $privateRise -Force | ForEach-Object {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $targetData 'RISE') -Recurse -Force
    }
}

$targetPlayer = Join-Path $TargetClient 'Data\Player\player.bmd'
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $targetPlayer).Hash -ne $expectedPlayerHash) {
    throw 'Runtime-QA merged Player\player.bmd hash mismatch after staging'
}
$engineHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $TargetClient 'Engine-Slayer S21.exe')).Hash
$requiredRiseFile = Join-Path $TargetClient 'Data\RISE\Config\Mix.bmd'
if (!(Test-Path -LiteralPath $requiredRiseFile -PathType Leaf)) {
    throw 'Runtime-QA base RISE merge is incomplete: Config\Mix.bmd missing'
}
foreach ($keyFile in @('Enc1.dat', 'Dec2.dat')) {
    $sourceKey = Join-Path (Join-Path $SourceClient 'Data') $keyFile
    $targetKey = Join-Path $targetData $keyFile
    if (!(Test-Path -LiteralPath $targetKey -PathType Leaf) -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $sourceKey).Hash -ne
        (Get-FileHash -Algorithm SHA256 -LiteralPath $targetKey).Hash) {
        throw "Runtime-QA root Data file missing or mismatched: $keyFile"
    }
}
$junctionCount = @(Get-ChildItem -LiteralPath $targetData -Directory |
    Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint }).Count
Write-Output "PASS: Slayer runtime-QA client prepared at $TargetClient"
Write-Output "PASS: Engine-Slayer S21 SHA-256 $engineHash"
Write-Output "PASS: private Slayer Player SHA-256 $expectedPlayerHash"
Write-Output 'PASS: complete base RISE tree plus private Slayer overlay staged'
Write-Output 'PASS: root Data files including Enc1.dat and Dec2.dat staged'
Write-Output "PASS: $junctionCount source Data junctions; Player and RISE are private copies"
