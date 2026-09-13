$ErrorActionPreference = 'Stop'

$dexRuntime = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\RISE\PegasusDexBoosterRuntime.cpp') -Raw
if ($dexRuntime.Contains('static_cast<WORD>(skill)') -or
    -not $dexRuntime.Contains('3, &owner, -1, 0, 0)') -or
    -not $dexRuntime.Contains('0, &owner, -1, 0, 0)')) {
    throw '661C83..85/661CA9..AB: Dex cast objects require zero SkillIndex and Kind'
}
if (-not $dexRuntime.Contains('CreateParticleFpsChecked(kDexBoosterFlare01Bitmap, effect.Position,') -or
    $dexRuntime.Contains('CreateParticle(kDexBoosterFlare01Bitmap, effect.Position,')) {
    throw 'Dex flare emitter must preserve Pegasus7BB8A0 FPS-checked particle wrapper'
}

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusDexBoosterRuntime.cpp') -Raw
$joint = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffectJoint.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$shared = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\Shared\PegasusHighSkillCatalogData.h') -Raw
$required = @(
    'effect.Alpha = kDexBoosterGroundStartLight;',
    'ownerModel.TransformByObjectBone(bonePosition, orchestrator.Owner,',
    'rand() % ownerModel.NumBones);',
    'effect.Alpha -= kDexBoosterGroundAlphaStep * animationFactor;',
    'Vector(effect.Alpha, effect.Alpha * 0.5f, 0.0f, effect.Light);',
    'kDexBoosterGroundRotation10',
    'const float blend = effect.Alpha;',
    'static_cast<float>(WorldTime) * -kDexBoosterAuraRotation',
    'EnableAlphaBlend();',
    '-effect.Angle[2], 1.0f, 5.0f);'
    'kDexBoosterFlareJointScale, 2, kDexBoosterSkill);'
)
$missing = @($required | Where-Object { -not $runtime.Contains($_) })
$jointRequired = @(
    'SkillIndex == rise::pegasus::kDexBoosterSkill',
    'kDexBoosterFlareJointRuntimeScale',
    'kDexBoosterFlareJointMaxTails',
    'ComputeDexBoosterFlareJointPosition(WorldTime, iIndex,',
    'o->LifeTime -= FPS_ANIMATION_FACTOR;'
)
$missing += @($jointRequired | Where-Object { -not $joint.Contains($_) })
if ($runtime.Contains('effect.BlendMeshLight -= 0.02f') -or
    $runtime.Contains('effect.Angle[2] += ((effect.SubType == 9) ? 10.0f :') -or
    $runtime.Contains('EnableAlphaTest();')) {
    $missing += 'old guessed ground fade/rotation route remains'
}
if (-not $server.Contains('case SKILL_DEX_BOOSTER:') -or
    -not $server.Contains('return this->SkillPegasusSelfBuff(aIndex, lpSkill);') -or
    -not $server.Contains('HighSkillBuffDurationSeconds(lpSkill->m_index)')) {
    $missing += 'authoritative server marker/duration route is missing'
}
if (-not $shared.Contains('static constexpr int kDexBoosterDurationSeconds = 300;') -or
    -not $shared.Contains('skillId == kDexBoosterSkill ? kDexBoosterDurationSeconds')) {
    $missing += 'source-backed 300-second Dex Booster duration is missing'
}

if ($missing.Count -ne 0) {
    Write-Host 'FAIL: Dex Booster Pegasus routing drifted:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: Dex Booster visual chain plus authoritative 300-second marker route match Pegasus'
