$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusSwordWrathRuntime.cpp') -Raw
$joint = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectJoint.cpp') -Raw
$particle = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectParticle.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateSwordWrath(OBJECT&')
$renderStart = $runtime.IndexOf('bool RenderSwordWrath(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Sword Wrath updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$rootStart = $update.IndexOf('if (effect.Type == kSwordWrathRootModel)')
$starStart = $update.IndexOf('else if (effect.SubType == 2)', $rootStart)
$waveStart = $update.IndexOf('else if (effect.SubType == 3)', $starStart)
$followStart = $update.IndexOf("else`r`n    {", $waveStart)
if ($followStart -lt 0) {
    $followStart = $update.IndexOf("else`n    {", $waveStart)
}
if ($rootStart -lt 0 -or $starStart -lt 0 -or $waveStart -lt 0 -or
    $followStart -lt 0) {
    throw 'Sword Wrath per-subtype updater boundaries not found'
}
$prefix = $update.Substring(0, $rootStart)
$root = $update.Substring($rootStart, $starStart - $rootStart)
$star = $update.Substring($starStart, $waveStart - $starStart)
$wave = $update.Substring($waveStart, $followStart - $waveStart)
$follow = $update.Substring($followStart)
$copy = 'VectorCopy(effect.Owner->Position, effect.Position);'
$failures = @()

$render = $runtime.Substring($renderStart)
foreach ($requiredRenderToken in @(
    'effect.BlendMesh = 0;',
    'effect.BlendMeshLight = effect.Alpha;',
    'model.RenderBody(RENDER_TEXTURE, effect.Alpha, effect.BlendMesh,',
    'effect.BlendMeshTexCoordV, effect.HiddenMesh, -1);')) {
    if (-not $render.Contains($requiredRenderToken)) {
        $failures += "Sword Wrath native default material forwarding missing: $requiredRenderToken"
    }
}
if ($render.Contains('RENDER_BRIGHT') -or $render.Contains('model.RenderMesh(')) {
    $failures += 'Sword Wrath must not bypass native body blend-slot selection with a forced bright mesh loop'
}

if (-not $prefix.Contains('if (!effect.Owner || !effect.Owner->Live)')) {
    $failures += 'source-proven global owner/live gate is missing'
}
if (-not $root.Contains($copy)) {
    $failures += '0x27B3 root owner follow is missing'
}
if ($star.Contains($copy)) {
    $failures += 'subtype-2 ground star must remain at cast-time position'
}
if ($wave.Contains($copy)) {
    $failures += 'subtype-3 wave must remain at cast-time position'
}
if (-not $follow.Contains($copy)) {
    $failures += 'subtype-0/1 owner follow is missing'
}
$copyCount = ([regex]::Matches($update, [regex]::Escape($copy))).Count
if ($copyCount -ne 2) {
    $failures += 'only root and subtype-0/1 branches may follow owner'
}

$layersStart = $runtime.IndexOf('void CreateSwordWrathLayers(OBJECT&')
$layersEnd = $runtime.IndexOf('void UpdateSwordWrath(OBJECT&', $layersStart)
$accentStart = $runtime.IndexOf('void CreateSwordWrathAccentParticles(OBJECT&')
$accentEnd = $runtime.IndexOf("}`r`n}", $accentStart)
if ($accentEnd -lt 0) {
    $accentEnd = $runtime.IndexOf("}`n}", $accentStart)
}
if ($layersStart -lt 0 -or $layersEnd -lt 0 -or $accentStart -lt 0 -or
    $accentEnd -lt 0) {
    $failures += 'Sword Wrath layer/accent emitter boundaries not found'
}
else {
    $layers = $runtime.Substring($layersStart, $layersEnd - $layersStart)
    $accent = $runtime.Substring($accentStart, $accentEnd - $accentStart)
    if (-not $layers.Contains('for (int n = 0; n < 30; ++n)') -or
        -not $layers.Contains('CreateJoint(lineBitmap')) {
        $failures += 'source-proven thirty line-joint burst is missing'
    }
    if (-not $accent.Contains('GetSwordWrathBone17(effect, position);') -or
        -not $accent.Contains('CreateParticle(g_swordWrathSmokeBitmap') -or
        -not $accent.Contains('CreateParticle(g_swordWrathPinStarBitmap')) {
        $failures += 'bone-17 smoke/pin-star accent chain is missing'
    }
}

if (-not $update.Contains('ShouldEmitSwordWrathAccent(effect.Type == kSwordWrathRootModel')) {
    $failures += 'accent emission is no longer restricted to the root carrier'
}
if (-not $joint.Contains('IsSwordWrathLineBitmap(Type)') -or
    -not $joint.Contains('kSwordWrathLineLifetime') -or
    -not $joint.Contains('kSwordWrathLineVelocityFactor') -or
    -not $joint.Contains('SwordWrathLineLightFactor(o->LifeTime)')) {
    $failures += 'line-joint lifetime/movement/fade route is incomplete'
}
if (-not $particle.Contains('IsSwordWrathSmokeBitmap(Type)') -or
    -not $particle.Contains('kSwordWrathSmokeLifetime') -or
    -not $particle.Contains('IsSwordWrathPinStarBitmap(Type)') -or
    -not $particle.Contains('kSwordWrathPinStarLifetime') -or
    -not $particle.Contains('kSwordWrathPinStarRise')) {
    $failures += 'smoke/pin-star initialization or lifetime route is incomplete'
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Sword Wrath position ownership drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Sword Wrath ownership, 30 joints, bone-17 accents and child lifetimes are routed'
