$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusShiningBirdRuntime.cpp') -Raw
$client = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateShiningBird(')
$renderStart = $runtime.IndexOf('bool RenderShiningBird(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Shining Bird updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$failures = @()
if (-not $runtime.Contains('&owner, -1, static_cast<WORD>(skill), 0, 0, 1.3f)')) {
    $failures += '672B11 must pass241 in SkillIndex, not adjacent Skill'
}
foreach ($token in @(
    'effect.Position[0] += std::sin(heading) * kShiningBirdRootAdvance;',
    'effect.Position[1] -= std::cos(heading) * kShiningBirdRootAdvance;',
    'particle.Position[0] = anchor[0] + std::sin(angle) * 130.0f;',
    'particle.Position[1] = anchor[1] - std::cos(angle) * 130.0f;')) {
    if (-not $runtime.Contains($token)) {
        $failures += "verified SIN/COS callsite ordering drifted: $token"
    }
}
if ($runtime.Contains('CreateParticle(kShiningBirdPillarBitmap') -or
    -not $runtime.Contains('JOINT& particle = Joints[i]') -or
    ([regex]::Matches($runtime, 'CreateJoint\(kShiningBirdPillarBitmap')).Count -ne 3) {
    $failures += 'force-pillar children must be allocated and anchored in the joint pool'
}
if ($update.Contains('effect.Owner') -or $update.Contains('Owner->Live')) {
    $failures += 'non-Pegasus caster ownership gate remains'
}
if (-not $update.Contains('model.PlayAnimation(') -or
    -not $update.Contains('effect.Live = false;')) {
    $failures += 'non-looping BMD completion cleanup is missing'
}

$skillStart = $server.IndexOf('bool CSkillManager::SkillShiningBird(')
$skillEnd = $server.IndexOf('bool CSkillManager::SkillWindSoul(', $skillStart)
if ($skillStart -lt 0 -or $skillEnd -le $skillStart) {
    $failures += 'server Shining Bird route boundaries are missing'
}
else {
    $skillRoute = $server.Substring($skillStart, $skillEnd - $skillStart)
    foreach ($token in @(
        'GetSkillFrustrum(SkillFrustrumX, SkillFrustrumY, angle,',
        'if (OBJECT_RANGE(bIndex) == 0)',
        'angle = static_cast<BYTE>((this->GetSkillAngle(lpObj->X, lpObj->Y,',
        'gObj[bIndex].X, gObj[bIndex].Y) * 255) / 360)',
        'lpObj->X, lpObj->Y, 1.5f, 6.0f, 1.5f, 0.0f)',
        'CheckSkillFrustrum(SkillFrustrumX, SkillFrustrumY,',
        'gObjAddAttackProcMsgSendDelay(lpObj, 50, index, 200,')) {
        if (-not $skillRoute.Contains($token)) {
            $failures += "server directional hit/reaction timing is missing: $token"
        }
    }
}

$damageStart = $client.IndexOf('void ReceiveAttackDamage(')
$damageEnd = $client.IndexOf("`nvoid ", $damageStart + 10)
if ($damageStart -lt 0 -or $damageEnd -le $damageStart -or
    -not $client.Substring($damageStart, $damageEnd - $damageStart).Contains(
        'SetPlayerShock(c, Damage);')) {
    $failures += 'native authoritative damage packet no longer owns target reaction'
}
if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Shining Bird lifetime routing drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Shining Bird preserves launched BMD lifetime plus native directional hit/reaction routing'
