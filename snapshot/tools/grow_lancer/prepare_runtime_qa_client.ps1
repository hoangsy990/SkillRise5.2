param(
    [string]$FrozenDataClient = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\Client',
    [string]$RuntimeSupportClient = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client',
    [string]$PrivateClient = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client',
    [string]$TargetClient = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client',
    [string]$BuiltEngine = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Bin\Engine-Port S21.exe'
)

$ErrorActionPreference = 'Stop'
$expectedTarget = [IO.Path]::GetFullPath(
    'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client')
$resolvedTarget = [IO.Path]::GetFullPath($TargetClient)
if ($resolvedTarget -ne $expectedTarget) {
    throw "Refusing non-isolated target: $resolvedTarget"
}
foreach ($required in @($FrozenDataClient, $RuntimeSupportClient, $PrivateClient)) {
    if (!(Test-Path -LiteralPath $required -PathType Container)) {
        throw "Required client directory is missing: $required"
    }
}
if (!(Test-Path -LiteralPath $BuiltEngine -PathType Leaf)) {
    throw "Runtime-QA Engine-Port S21.exe is missing: $BuiltEngine"
}

$privatePlayer = Join-Path $PrivateClient 'Data\Player\player.bmd'
$expectedPlayerHash = '0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63'
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $privatePlayer).Hash -ne $expectedPlayerHash) {
    throw 'Private merged player hash mismatch'
}

New-Item -ItemType Directory -Force -Path $TargetClient | Out-Null
Get-ChildItem -LiteralPath $RuntimeSupportClient -File | ForEach-Object {
    # Runtime support files are frozen inside the Grow Lancer worktree.  Do
    # not copy Engine/linker/QA artifacts, logs or captures into the stage.
    if ($_.Extension -ieq '.dll' -or $_.Name -ieq 'RISE.ini') {
        Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $TargetClient $_.Name) -Force
    }
}
$legacyTarget = Join-Path $TargetClient 'Engine.exe'
if (Test-Path -LiteralPath $legacyTarget -PathType Leaf) {
    Remove-Item -LiteralPath $legacyTarget -Force
}
Copy-Item -LiteralPath $BuiltEngine -Destination (Join-Path $TargetClient 'Engine-Port S21.exe') -Force

$sourceScripts = Join-Path $RuntimeSupportClient 'Scripts'
if (Test-Path -LiteralPath $sourceScripts -PathType Container) {
    Copy-Item -LiteralPath $sourceScripts -Destination $TargetClient -Recurse -Force
}

$targetData = Join-Path $TargetClient 'Data'
New-Item -ItemType Directory -Force -Path $targetData | Out-Null

# Root-level Data files include the SimpleModulus keys used by the login
# packet path. The directory-junction loop below cannot carry these files.
Get-ChildItem -LiteralPath (Join-Path $FrozenDataClient 'Data') -File | ForEach-Object {
    Copy-Item -LiteralPath $_.FullName -Destination (Join-Path $targetData $_.Name) -Force
}
Get-ChildItem -LiteralPath (Join-Path $FrozenDataClient 'Data') -Directory | ForEach-Object {
    if ($_.Name -notin @('Player', 'RISE')) {
        $link = Join-Path $targetData $_.Name
        if (Test-Path -LiteralPath $link) {
            $existing = Get-Item -LiteralPath $link -Force
            if (!($existing.Attributes -band [IO.FileAttributes]::ReparsePoint)) {
                throw "Refusing to replace non-junction QA Data directory: $link"
            }
            $currentTarget = [IO.Path]::GetFullPath($existing.Target[0])
            $wantedTarget = [IO.Path]::GetFullPath($_.FullName)
            if ($currentTarget -ne $wantedTarget) {
                # Directory.Delete removes the reparse point itself and never
                # traverses into its target. PowerShell 7 Remove-Item can throw
                # a NullReferenceException for these legacy junctions.
                [IO.Directory]::Delete($link, $false)
            }
        }
        if (!(Test-Path -LiteralPath $link)) {
            New-Item -ItemType Junction -Path $link -Target $_.FullName | Out-Null
        }
    }
}

# Remove stale junctions left by older QA layouts.  Only delete the reparse
# point itself, never traverse into its target.  In particular, an obsolete
# Data\Map link once escaped to the mutable production client and made the
# reported junction count disagree with the verified frozen-source set.
$allowedJunctionNames = @(Get-ChildItem -LiteralPath (Join-Path $FrozenDataClient 'Data') -Directory |
    Where-Object { $_.Name -notin @('Player', 'RISE') } |
    ForEach-Object { $_.Name })
Get-ChildItem -LiteralPath $targetData -Directory | Where-Object {
    ($_.Attributes -band [IO.FileAttributes]::ReparsePoint) -and
    $_.Name -notin $allowedJunctionNames
} | ForEach-Object {
    if ([IO.Path]::GetFullPath($_.Parent.FullName) -ne [IO.Path]::GetFullPath($targetData)) {
        throw "Refusing stale junction outside QA Data root: $($_.FullName)"
    }
    [IO.Directory]::Delete($_.FullName, $false)
}

# RISE contains required base runtime files such as Config\Mix.bmd. It cannot
# be omitted or junctioned because the private Grow Lancer overlay must remain
# isolated. Copy the base RISE tree first, then merge the private overlay.
Copy-Item -LiteralPath (Join-Path $FrozenDataClient 'Data\RISE') -Destination $targetData -Recurse -Force
Copy-Item -LiteralPath (Join-Path $PrivateClient 'Data\Player') -Destination $targetData -Recurse -Force
Copy-Item -LiteralPath (Join-Path $PrivateClient 'Data\RISE') -Destination $targetData -Recurse -Force

$targetPlayer = Join-Path $TargetClient 'Data\Player\player.bmd'
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $targetPlayer).Hash -ne $expectedPlayerHash) {
    throw 'Runtime-QA merged player hash mismatch after staging'
}
$engineHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $TargetClient 'Engine-Port S21.exe')).Hash
$requiredRiseFile = Join-Path $TargetClient 'Data\RISE\Config\Mix.bmd'
if (!(Test-Path -LiteralPath $requiredRiseFile -PathType Leaf)) {
    throw 'Runtime-QA base RISE merge is incomplete: Config\Mix.bmd missing'
}
foreach ($keyFile in @('Enc1.dat', 'Dec2.dat')) {
    $sourceKey = Join-Path (Join-Path $FrozenDataClient 'Data') $keyFile
    $targetKey = Join-Path $targetData $keyFile
    if (!(Test-Path -LiteralPath $targetKey -PathType Leaf) -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $sourceKey).Hash -ne
        (Get-FileHash -Algorithm SHA256 -LiteralPath $targetKey).Hash) {
        throw "Runtime-QA root Data file missing or mismatched: $keyFile"
    }
}
$junctionCount = @(Get-ChildItem -LiteralPath $targetData -Directory |
    Where-Object { $_.Attributes -band [IO.FileAttributes]::ReparsePoint }).Count
Write-Output "PASS: runtime-QA client prepared at $TargetClient"
Write-Output "PASS: runtime-QA Engine SHA-256 $engineHash"
Write-Output "PASS: merged player SHA-256 $expectedPlayerHash"
Write-Output "PASS: base RISE tree plus private Grow Lancer overlay staged"
Write-Output "PASS: root Data files including Enc1.dat and Dec2.dat staged"
Write-Output "PASS: $junctionCount frozen worktree Data junctions; Player and RISE are private copies"
