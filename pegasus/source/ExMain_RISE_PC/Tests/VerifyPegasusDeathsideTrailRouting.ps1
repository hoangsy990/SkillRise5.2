$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusDeathsideRuntime.cpp') -Raw
$blurRenderer = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectBlurSpark.cpp') -Raw
if ($runtime -notmatch 'root->Angle, root->Light, 0, root, -1,\s*static_cast<WORD>\(skill\), 0, 0, root->Scale\)' -or
    -not $runtime.Contains('-1, static_cast<WORD>(skill), 0, 0, 1.0f)')) {
    throw '6605D2/660658: Deathside root and scythe require SkillIndex288, Kind0'
}
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$attack = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\Attack.cpp') -Raw
$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw

$requiredRuntime = @(
    'switch (caster.SafeZone ? -1 : caster.Helper.Type)',
    'CreateObjectBlur(&effect, start, end, light, g_deathsideBlur, false,',
    'kDeathsideTrailLifetime',
    'kDeathsideMirroredTailLifetime',
    'state.Loop = action != 0 && action != 2;',
    'state.PlaySpeed = DeathsideAttackPlaySpeed(state.NumAnimationKeys);',
    'model.Actions[0].PlaySpeed = 0.2f;'
    'effect.Angle, effect.HeadAngle, false, false);'
)
$requiredRenderer = @(
    'IsDeathsideBlurBitmap(Type)',
    'int nTexture = (deathsideBlur || IsChaosBladeBlur(Type)) ? Type : BITMAP_BLUR+Type;',
    'if (deathsideBlur)',
    'EnableAlphaBlendMinus();'
)

$missing = @()
$particleRenderer = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectParticle.cpp') -Raw
if ($particleRenderer -notmatch '(?s)if \(deathsideSmokeRender\)\s*\{[^}]*VectorScale\(o->Light, o->Alpha, deathsideRenderLight\);[^}]*EnableAlphaBlendMinus\(\);') {
    $missing += 'Deathside smoke must apply age alpha to temporary RGB and use subtractive blend'
}
if ($blurRenderer -notmatch '(?s)else if \(deathsideBlur\)\s*\{[^}]*EnableAlphaBlendMinus\(\);') {
    $missing += 'Deathside blur must use recovered GL_ZERO/GL_ONE_MINUS_SRC_COLOR, not alpha-test'
}
foreach ($token in $requiredRuntime) {
    if (-not $runtime.Contains($token)) {
        $missing += "Deathside runtime missing: $token"
    }
}
foreach ($token in $requiredRenderer) {
    if (-not $blurRenderer.Contains($token)) {
        $missing += "object-blur renderer missing: $token"
    }
}
if ($runtime.Contains('CreateJoint(g_deathsideBlur')) {
    $missing += 'Deathside blur regressed to the incompatible joint allocator'
}
foreach ($token in @(
    'const int variant = rand() % 3;',
    'PlayBuffer(kDeathsideAttack1Sound + variant',
    'SetAction(root, kDeathsideAttackActions[variant]',
    'CreateEffect(kDeathsideScythe01Model + variant',
    'effect.CurrentAction = kDeathsideDisappearAction;',
    'effect.LifeTime = kDeathsideDisappearLifetime;',
    'PlayBuffer(kDeathsideDisappearSound',
    'rand_fps_check(kDeathsideWaterfallCadence)',
    'rand_fps_check(kDeathsideGroundSmokeCadence)',
    'rand_fps_check(kDeathsideSmokeCadence)',
    'static const int meshes[] = {0, 1, 2, 4};'
)) {
    if (-not $runtime.Contains($token)) {
        $missing += "Deathside attack/dismiss/render chain missing: $token"
    }
}
if (-not $server.Contains('bool CSkillManager::SkillDeathside(') -or
    -not $server.Contains('this->BasicSkillAttack(aIndex, bIndex, lpSkill, combo)') -or
    -not $attack.Contains('skill == SKILL_DEATHSIDE') -or
    -not $attack.Contains('GetAttackDamageCursed(lpObj, lpTarget')) {
    $missing += 'authoritative cast or Summoner curse-damage family route is missing'
}
if (-not $wsclient.Contains('void ReceiveAttackDamage(const BYTE* ReceiveBuffer)') -or
    -not $wsclient.Contains('SetPlayerShock(c, Damage);')) {
    $missing += 'native damage-packet target reaction is missing'
}

if ($missing.Count -ne 0) {
    Write-Host 'FAIL: Deathside object-blur ABI/renderer route drifted:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: Deathside variants, Reaper lifecycle, trails, dismiss, curse damage and native reaction are routed'
