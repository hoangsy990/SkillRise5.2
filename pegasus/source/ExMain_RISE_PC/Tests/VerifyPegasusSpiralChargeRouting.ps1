$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusSpiralChargeRuntime.cpp') -Raw
$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateSpiralCharge(OBJECT&')
if ($runtime.Contains('static_cast<WORD>(skill)') -or
    -not $runtime.Contains('0, &owner, -1, 0, 0)')) {
    throw '677012..677014: Spiral cast root requires zero SkillIndex and Kind'
}
$renderStart = $runtime.IndexOf('bool RenderSpiralCharge(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Spiral Charge updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$anchorStart = $update.IndexOf('if (effect.Type == kSpiralChargeAnchorModel)')
$shockwaveStart = $update.IndexOf('if (effect.Type == kSpiralChargeShockwaveModel)', $anchorStart)
if ($anchorStart -lt 0 -or $shockwaveStart -lt 0) {
    throw 'Spiral Charge anchor branch boundaries not found'
}
$prefix = $update.Substring(0, $anchorStart)
$anchor = $update.Substring($anchorStart, $shockwaveStart - $anchorStart)
$failures = @()
if (-not $update.Contains('SpiralChargeEmitterTick(effect.LifeTime, animationFactor)') -or
    $update.Contains('std::sin(effect.LifeTime')) {
    $failures += 'emitter cadence must use the recovered floor crossing, not a sine wave'
}

# Pegasus initializer 0x6770DA..0x6770F1 and updater 0x6773EC..0x6773FE
# reject every one of 0x27A8..0x27AD when its owner is absent or not live.
if (-not $prefix.Contains('if (!effect.Owner || !effect.Owner->Live)')) {
    $failures += 'source-proven global owner/live gate is missing'
}

# Only 0x27AD additionally checks source buff 214 (translated to native 169)
# and follows the actor. The five cast-time layers stay at their creation point.
if (-not $anchor.Contains('g_isCharacterBuff(effect.Owner,')) {
    $failures += 'persistent anchor buff gate is missing'
}
if (-not $anchor.Contains('static_cast<eBuffState>(kSpiralChargeNativeEffect)')) {
    $failures += 'source-214 to native-169 marker translation is missing'
}
if (-not $anchor.Contains('VectorCopy(effect.Owner->Position, effect.Position);')) {
    $failures += 'persistent anchor owner follow is missing'
}
$copyCount = ([regex]::Matches($update,
    [regex]::Escape('VectorCopy(effect.Owner->Position, effect.Position);'))).Count
if ($copyCount -ne 1) {
    $failures += 'only the persistent anchor may follow owner position'
}

$createStart = $runtime.IndexOf('void CreateSpiralChargeLayers(OBJECT&')
$updateBoundary = $runtime.IndexOf('void UpdateSpiralCharge(OBJECT&', $createStart)
if ($createStart -lt 0 -or $updateBoundary -lt 0) {
    throw 'Spiral Charge layer constructor boundaries not found'
}
$create = $runtime.Substring($createStart, $updateBoundary - $createStart)
if (-not $create.Contains('if (!root.Owner || !root.Owner->Live)')) {
    $failures += 'source-proven layer-construction owner/live gate is missing'
}
if (-not $create.Contains('CreateEffect(kSpiralChargeFlareModel') -or
    ([regex]::Matches($create, 'CreateLayer\(')).Count -lt 4 -or
    -not $create.Contains('for (int bone = 0; bone < kSpiralChargeRootJointCount; ++bone)')) {
    $failures += 'flare/four-child/nine-joint constructor chain is incomplete'
}
if (-not $update.Contains('model.PlayAnimation(') -or
    -not $update.Contains('CreateParticle(steam ? kSpiralChargeSteamBitmap')) {
    $failures += 'root BMD playback or steam/smoke emitters are missing'
}
if (-not $runtime.Contains('for (int index = 0; index < kSpiralChargeAnchorBoneCount; ++index)') -or
    -not $runtime.Contains('CreateSprite(BITMAP_LIGHT_RED, position')) {
    $failures += 'persistent seventeen-bone aura renderer is incomplete'
}
if (-not $wsclient.Contains('rise::pegasus::CreateSpiralChargeAnchor(*o);') -or
    -not $wsclient.Contains('DeleteEffect(rise::pegasus::kSpiralChargeAnchorModel, o);')) {
    $failures += 'packet-owned anchor add/remove route is missing'
}
if (-not $server.Contains('case SKILL_SPIRAL_CHARGE:') -or
    -not $server.Contains('return this->SkillPegasusSelfBuff(aIndex, lpSkill);')) {
    $failures += 'authoritative Spiral Charge server marker route is missing'
}

$particles = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectParticle.cpp') -Raw
$alphaStart = $particles.IndexOf('else if (o->Type == rise::pegasus::kSpiralChargeSteamBitmap')
$alphaEnd = if ($alphaStart -ge 0) { $particles.IndexOf('else switch (o->Type)', $alphaStart) } else { -1 }
if ($alphaStart -lt 0 -or $alphaEnd -le $alphaStart) {
    $failures += 'Spiral7F1B/7F1C particle render alpha branch is missing'
} else {
    $alphaRender = $particles.Substring($alphaStart, $alphaEnd - $alphaStart)
    if (-not $alphaRender.Contains('kSpiralChargeSmokeBitmap') -or
        -not $alphaRender.Contains('VectorScale(o->Light, o->Alpha, spiralRenderLight);') -or
        -not $alphaRender.Contains('spiralRenderLight, o->Rotation);') -or
        $alphaRender.Contains('EnableAlphaBlendMinus();')) {
        $failures += 'Spiral particles require temporary RGB*Alpha, preserving default sprite blend'
    }
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Spiral Charge ownership/buff routing drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Spiral Charge children, animation, particles, anchor aura and packet removal match Pegasus'
