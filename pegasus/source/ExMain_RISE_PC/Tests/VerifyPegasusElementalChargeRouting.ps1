$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusElementalChargeRuntime.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateElementalCharge(OBJECT&')
$renderStart = $runtime.IndexOf('bool RenderElementalCharge(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Elemental Charge root updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$failures = @()
if ($runtime.Contains('static_cast<WORD>(skill)') -or
    $runtime -notmatch 'kElementalCastSubtype, &target, -1, 0,\s*0, 0, 1\.0f\)') {
    $failures += '664D5B..664D5D: cast carrier requires zero SkillIndex and Kind'
}
$dispatch = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
if (-not $dispatch.Contains('CreateElementalCharge(*so, MagicNumber);') -or
    $dispatch.Contains('CreateElementalCharge(*to, MagicNumber);') -or
    -not $dispatch.Contains('CreateElementalChargeBuffAura(*o);')) {
    $failures += 'cast aura must follow animated caster; buff sync retains recipient ownership'
}
if ($update.Contains('g_isCharacterBuff') -or
    $update.Contains('99999940.0f')) {
    $failures += 'non-Pegasus delayed local buff expiration remains'
}
if (-not $update.Contains('if (!effect.Owner || !effect.Owner->Live)')) {
    $failures += 'source-proven owner/live gate is missing'
}
if (-not $runtime.Contains('DeleteElementalChargeAuras(OBJECT& target)')) {
    $failures += 'authoritative buff-removal route is missing'
}
if (-not $runtime.Contains('kElementalConstructorChildCount') -or
    -not $runtime.Contains('n < 3 ? kElementalFlare03Bitmap') -or
    -not $runtime.Contains('kElementalShockwaveBitmap')) {
    $failures += 'three-flare plus two-shockwave constructor chain is missing'
}
if (-not $runtime.Contains('model.PlayAnimation(') -or
    -not $runtime.Contains('for (int layer = 0; layer < kElementalLayerCount; ++layer)') -or
    -not $runtime.Contains('BoneTransform[kElementalBones[layer]]') -or
    -not $runtime.Contains('CreateSprite(kElementalBitmaps[layer]') -or
    -not $runtime.Contains('model.RenderMesh(layer, RENDER_TEXTURE | RENDER_BRIGHT')) {
    $failures += 'animated five-bone sprite/mesh renderer is incomplete'
}
if (-not $runtime.Contains('UpdateElementalChargeChild(OBJECT&') -or
    -not $runtime.Contains('CreateSprite(effect.Type, effect.Position, effect.Gravity')) {
    $failures += 'constructor-child growth/fade sprite route is missing'
}
if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Elemental Charge lifetime routing drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Elemental Charge ownership, five children, animated five-layer renderer and removal match Pegasus'
