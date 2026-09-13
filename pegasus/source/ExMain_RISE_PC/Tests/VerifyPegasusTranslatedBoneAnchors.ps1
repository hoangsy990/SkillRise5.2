$ErrorActionPreference = 'Stop'
$root = Join-Path $PSScriptRoot '..\Main5.2_RISE\RISE'
$bird = Get-Content -LiteralPath (Join-Path $root 'PegasusShiningBirdRuntime.cpp') -Raw
$dragon = Get-Content -LiteralPath (Join-Path $root 'PegasusDragonViolentRuntime.cpp') -Raw
$elemental = Get-Content -LiteralPath (Join-Path $root 'PegasusElementalChargeRuntime.cpp') -Raw
if (-not $elemental.Contains('position, false); // 0x664B74') -or
    -not $elemental.Contains('&effect, 0.0f); // 0x664BB8')) {
    throw 'Elemental lights require translated bone extraction and carrier sprite owner'
}
if (-not $bird.Contains('model.TransformPosition(BoneTransform[1], zero, anchor, false);') -or
    -not $bird.Contains('birdPositions[boneIndex], false);') -or
    -not $dragon.Contains('position, false); // 0x6630AF/0x663119')) {
    throw 'Recovered world-space bone anchors must not add BodyOrigin twice'
}
Write-Output 'PASS: recovered bird/dragon bone-anchor translation contracts; runtime parity not implied'
