$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusSpearstormRuntime.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateSpearstorm(')
$renderStart = $runtime.IndexOf('bool RenderSpearstorm(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Spearstorm updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$failures = @()
if (-not $runtime.Contains('0, &target, -1, static_cast<WORD>(skill), 0, 0, 0.9f)')) {
    $failures += '6755D3 root must pass243 in SkillIndex, not adjacent Skill'
}
$effectSource = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\ZzzEffect.cpp') -Raw
if ($effectSource -notmatch '(?s)case rise::pegasus::kSpearstormRootModel:\s*case rise::pegasus::kSpearstormBladeModel:\s*case rise::pegasus::kSpearstormCrackModel:\s*// UpdateSpearstorm owns[^;]*break;') {
    $failures += 'Spearstorm must bypass generic post-update MoveParticle'
}
$bladeStart = $runtime.IndexOf('if (effect.Type == kSpearstormBladeModel)', $renderStart)
$bladeEnd = $runtime.IndexOf('if (effect.Type == kSpearstormCrackModel)', $bladeStart)
if ($bladeStart -lt 0 -or $bladeEnd -le $bladeStart) {
    throw 'Spearstorm blade renderer boundaries not found'
}
$blade = $runtime.Substring($bladeStart, $bladeEnd - $bladeStart)
if ($blade.Contains('BITMAP_CRATER') -or $blade.Contains('effect.Scale =') -or
    -not $blade.Contains('Vector(1.0f, 1.0f, 1.0f, model.BodyLight);') -or
    -not $blade.Contains('model.RenderMesh(0, RENDER_TEXTURE, alpha, -1, 1.0f,')) {
    $failures += 'blade renderer must retain its own scale/material and white body light'
}
if (-not $runtime.Contains('layer.reverse ? 1.0f - progress : 1.0f;')) {
    $failures += 'non-reverse Combo layer must retain constant alpha 1'
}
if ($update.Contains('!effect.Owner') -or $update.Contains('effect.Owner->Live')) {
    $failures += 'non-Pegasus owner gate remains in Spearstorm updater'
}
if (-not $runtime.Contains('OBJECT* owner = root.Owner ? root.Owner : &root;')) {
    $failures += 'Pegasus root-owner fallback is missing'
}
if (-not $runtime.Contains('effect.LifeTime = kSpearstormRootLifetime') -or
    -not $runtime.Contains('CreateSpearstormBlade(effect)') -or
    -not $runtime.Contains('SpearstormShouldImpact(') -or
    -not $runtime.Contains('CreateSpearstormCrack(effect)')) {
    $failures += 'five-tick blade fan or forced-impact transition is missing'
}
if (-not $runtime.Contains('for (int trail = 0; trail < 2; ++trail)') -or
    -not $runtime.Contains('CreateJoint(kSpearstormTrailBitmap') -or
    -not $runtime.Contains('kSpearstormStoneChildCount') -or
    -not $runtime.Contains('kSpearstormImpactSmokeCount')) {
    $failures += 'blade trails or crack stone/smoke child fan is incomplete'
}
if (-not $runtime.Contains('SpearstormCrossesLifetime(') -or
    -not $runtime.Contains('PlayBuffer(kSpearstormHitSound')) {
    $failures += 'source-timed impact sound latch is missing'
}
if (-not $runtime.Contains('BITMAP_DAMAGE_01_MONO') -or
    -not $runtime.Contains('kSpearstormComboBitmap') -or
    -not $runtime.Contains('model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT')) {
    $failures += 'impact crack/expansion renderer layers are incomplete'
}
if (-not $server.Contains('bool CSkillManager::SkillSpearstorm(') -or
    -not $server.Contains('kSpearstormAdditionalDamageRate') -or
    -not $server.Contains('this->GCSkillAttackSend(lpObj, lpSkill->m_index, bIndex, 1);')) {
    $failures += 'authoritative damage and target-centred cast response are missing'
}
if (-not $wsclient.Contains('void ReceiveAttackDamage(const BYTE* ReceiveBuffer)') -or
    -not $wsclient.Contains('SetPlayerShock(c, Damage);')) {
    $failures += 'native damage-packet target reaction is missing'
}
if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Spearstorm lifetime ownership drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Spearstorm fan, forced impact, child layers, sound, damage and native reaction are routed'
