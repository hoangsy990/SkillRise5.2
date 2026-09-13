$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusRainingArrowRuntime.cpp') -Raw
$stage = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    'StagePegasusAssets.ps1') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateRainingArrow(OBJECT&')
$projectileStart = $runtime.IndexOf('void UpdateRainingArrowProjectile(', $updateStart)
if ($updateStart -lt 0 -or $projectileStart -lt 0) {
    throw 'Raining Arrow updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $projectileStart - $updateStart)

$requiredRuntime = @(
    'effect.AlphaTarget = RainingArrowPulse(effect.LifeTime - 30.0f, 20.0f);',
    'root.Light, 0, root.Owner',
    'RainingArrowTextureId(layer.textureSlot)',
    'RainingArrowTextureId(kTextureCratered)',
    '{kTexturePinStar, 1.0f',
    '{kTextureSpark,   1.0f',
    '{kTextureBowStar, effect.Gravity * 2.0f',
    '{kTextureDamageMono,effect.Gravity*3.5f',
    'CreateEffect(MODEL_STONE1 + rand() % 2',
    'joint.Target != &projectile',
    'VectorCopy(projectile.Angle, joint.Angle);',
    'effect.Owner->SubType = 1;',
    'UpdateRainingArrowProjectileJoints(effect, true);'
)
$requiredAssets = @(
    'Effect\empact01.OZJ',
    'Effect\enemy_ring02.OZJ',
    'Effect\lightmarks.ozj',
    'Effect\Shockwave2.OZJ',
    'Effect\wave.OZJ',
    'Effect\cratered.OZT',
    'Effect\pin_star03.OZJ',
    'Effect\Spark04.OZJ',
    'Effect\bostar3_R.OZJ',
    'Effect\damage01mono.OZJ'
)

$missing = @()
$sprites = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\zzzeffectsprite.cpp') -Raw
$atlasStart = $sprites.IndexOf('if (rise::pegasus::IsRainingArrowDamageBitmap(o->Type))')
$atlasEnd = $sprites.IndexOf('else if', $atlasStart)
if ($atlasStart -lt 0 -or $atlasEnd -le $atlasStart -or
    -not $sprites.Substring($atlasStart, $atlasEnd - $atlasStart).Contains(
        'Width * cell, Height * cell')) {
    $missing += 'damage atlas geometry must use cell dimensions, not the full sheet'
}
foreach ($token in $requiredRuntime) {
    if (-not $runtime.Contains($token)) {
        $missing += "Raining Arrow runtime missing: $token"
    }
}
foreach ($asset in $requiredAssets) {
    if (-not $stage.Contains($asset)) {
        $missing += "Raining Arrow staged asset missing: $asset"
    }
}
if ($runtime.Contains('RainingArrowPulse(effect.LifeTime, 30.0f)')) {
    $missing += 'Impact pulse regressed to the wrong lifetime window'
}
if ($runtime.Contains('root.Direction, 0, root.Owner')) {
    $missing += 'Ring08 light regressed from OBJECT+0xB0 to Direction'
}
# Pegasus 0x671052 and 0x6710E6 reject only a null owner for the two
# orchestrators. The launched visual children have no global owner/live gate.
if ($update.Contains('Owner->Live')) {
    $missing += 'non-Pegasus owner-live gate was added to the effect updater'
}
$nullOwnerCount = ([regex]::Matches($update,
    [regex]::Escape('if (!effect.Owner)'))).Count
if ($nullOwnerCount -ne 2) {
    $missing += 'root/follow null-owner gates drifted from the two Pegasus branches'
}
if (-not $server.Contains('case SKILL_RAINING_ARROW:') -or
    -not $server.Contains('return this->SkillDragonViolent(aIndex, bIndex, lpSkill, combo);') -or
    -not $server.Contains('this->GCPegasusTargetsSend(lpObj, lpSkill->m_index, targets, count);')) {
    $missing += 'authoritative multi-target damage/visual list route is missing'
}
if (-not $wsclient.Contains('void ReceiveAttackDamage(const BYTE* ReceiveBuffer)') -or
    -not $wsclient.Contains('SetPlayerShock(c, Damage);')) {
    $missing += 'native damage-packet target reaction is missing'
}

if ($missing.Count -ne 0) {
    Write-Host 'FAIL: Raining Arrow renderer/asset contract drifted:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: Raining Arrow volley/impact renderer, target list and native reaction match Pegasus'
