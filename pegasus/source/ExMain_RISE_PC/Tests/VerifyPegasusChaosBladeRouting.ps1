$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusChaosBladeRuntime.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$required = @(
    'position[0] += cosf(radians) * radius;',
    'position[1] += sinf(radians) * radius;',
    'effect.Alpha = 1.0f;',
    'const int firstFrame = static_cast<int>(effect.Timer) + 1;',
    'EmitChaosBladeRootFrame(effect, frame);',
    'EmitChaosBladeArcPulse(effect);',
    'EmitChaosBladeCrackBurst(effect);'
)
$missing = @($required | Where-Object { -not $runtime.Contains($_) })
# 6547C9/65483E/654DF5: original 2012 is SkillIndex, which native
# CreateEffect stores in OBJECT.Skill. Adjacent Skill is not that tag.
foreach ($arguments in @(
    'light, 2, &target, -1, static_cast<WORD>(skill), 0, 0, 0.9f)',
    'light, 19, &target, -1, static_cast<WORD>(skill), 0, 0, 2.0f)',
    '0, &target, -1, static_cast<WORD>(skill), 0, 0, 1.2f)')) {
    if (-not $runtime.Contains($arguments)) {
        $missing += "Chaos source SkillIndex slot missing: $arguments"
    }
}
if (-not $wsclient.Contains('CreateChaosBladeRoot(*so, MagicNumber);') -or
    $wsclient.Contains('CreateChaosBladeRoot(*to, MagicNumber);')) {
    $missing += 'Chaos cast root must belong to caster, not attack target'
}
if (-not $wsclient.Contains('Models[so->Type].NumActions > rise::pegasus::kPegasusChaosBladeExtendedAction') -or
    -not $wsclient.Contains('SetAction(so, rise::pegasus::kPegasusChaosBladeExtendedAction);')) {
    $missing += 'Chaos caster must prefer available extended action 285'
}
$blur = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectBlurSpark.cpp') -Raw
if (-not [regex]::IsMatch($blur,
    'if \(Type == rise::pegasus::kChaosBladeBlurBlackBitmap\)\s*\{[^}]*EnableAlphaBlendMinus\(\);')) {
    $missing += 'Chaos black blur must use recovered ZERO/ONE_MINUS_SRC_COLOR blend'
}
if ($runtime.Contains('if (!effect.Owner || !effect.Owner->Live)') -or
    $runtime.Contains('VectorCopy(effect.Owner->Position, effect.Position);')) {
    $missing += 'non-Pegasus owner gate/follow remains in Chaos Blade updater'
}
if ($runtime.Contains('PlayBuffer(kChaosBladeHit2Sound') -or
    $runtime.Contains('PlayBuffer(kChaosBladeExplosionSound')) {
    $missing += 'unproven Hit-2/Explosion sound was attached to Chaos Blade runtime'
}
if (-not $runtime.Contains('GetChaosBladeBone17(target, position)') -or
    -not $runtime.Contains('CreateParticle(kChaosBladeFlareBlueBitmap') -or
    -not $runtime.Contains('CreateParticle(kChaosBladePinStarBitmap') -or
    -not $runtime.Contains('CreateEffect(kChaosBladeArcModel') -or
    -not $runtime.Contains('CreateParticle(kChaosBladeSteamBitmap') -or
    -not $runtime.Contains('CreateEffect(kChaosBladeCrackModel')) {
    $missing += 'bone-17 five-layer target impact chain is incomplete'
}
if ($runtime.Contains('CreatePegasusRibbonSegment(') -or
    -not $runtime.Contains('kChaosBladeBlurBlackBitmap, true, 0, 8)') -or
    -not $runtime.Contains('kChaosBladeBlurLongBitmap, true, 1, 8)') -or
    -not $runtime.Contains('kChaosBladeBlurLongBitmap, true, 2, 8)') -or
    -not $runtime.Contains('CreateObjectBlur(&effect, start, end, ribbonLight,') -or
    -not $runtime.Contains('model.Animation(BoneTransform')) {
    $missing += 'meshless sword bone-sampled ribbon chain is incomplete'
}
if (-not $runtime.Contains('kChaosBladeMagicGroundBitmap') -or
    -not $runtime.Contains('model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT')) {
    $missing += 'ground-crack renderer override is missing'
}
if (-not $server.Contains('case SKILL_CHAOS_BLADE:') -or
    -not $server.Contains('return this->SkillDragonViolent(aIndex, bIndex, lpSkill, combo);') -or
    -not $server.Contains('this->GCPegasusTargetsSend(lpObj, lpSkill->m_index, targets, count);')) {
    $missing += 'authoritative multi-target damage/visual list route is missing'
}
if (-not $wsclient.Contains('void ReceiveAttackDamage(const BYTE* ReceiveBuffer)') -or
    -not $wsclient.Contains('SetPlayerShock(c, Damage);')) {
    $missing += 'native damage-packet target reaction is missing'
}

$particles = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectParticle.cpp') -Raw
$smokeStart = $particles.IndexOf('else if (o->Type == rise::pegasus::kChaosBladeSmokeLine03Bitmap)')
$smokeEnd = if ($smokeStart -ge 0) { $particles.IndexOf('else if (o->Type == rise::pegasus::kSpiralChargeSteamBitmap', $smokeStart) } else { -1 }
if ($smokeStart -lt 0 -or $smokeEnd -le $smokeStart) {
    $missing += 'Chaos smoke_line03 needs its dedicated render branch'
} else {
    $smokeRender = $particles.Substring($smokeStart, $smokeEnd - $smokeStart)
    if (-not $smokeRender.Contains('EnableAlphaBlendMinus();') -or
        -not $smokeRender.Contains('o->Light, o->Rotation);') -or
        $smokeRender.Contains('VectorScale(') -or $smokeRender.Contains('EnableAlphaBlend();')) {
        $missing += '7F3A must use ZERO/ONE_MINUS_SRC_COLOR with stored RGB, not additive or extra alpha'
    }
}

if ($missing.Count -ne 0) {
    Write-Host 'FAIL: Chaos Blade fixed-position lifecycle route drifted:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: Chaos Blade fixed cascade, bone ribbons, target list, sole sound and native reaction are routed'
