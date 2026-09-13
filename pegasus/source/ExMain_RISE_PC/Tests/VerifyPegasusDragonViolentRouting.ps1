$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusDragonViolentRuntime.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$switchStart = $runtime.IndexOf('void UpdateDragonViolent(')
$renderStart = $runtime.IndexOf('bool RenderDragonViolent(', $switchStart)
if ($switchStart -lt 0 -or $renderStart -lt 0) {
    throw 'Dragon Violent updater boundaries not found'
}
$update = $runtime.Substring($switchStart, $renderStart - $switchStart)
$endStart = $update.IndexOf('case kDragonViolentEndModel:')
if ($endStart -lt 0) {
    throw 'Dragon Violent target-impact branch not found'
}
$beforeEnd = $update.Substring(0, $endStart)
$endBranch = $update.Substring($endStart)
$failures = @()
foreach ($arguments in @(
    '(rand() % 2) + 1, &owner, -1, 242, 0, 0,',
    '0, &owner, -1, 242, 0, 0, 0.9f)',
    '7, crack.Owner, -1, 242, 0, 0, 0.64f)',
    'subtype, crack.Owner, -1, 242, 0, 0, 0.0f)',
    'light, index, &target, -1, static_cast<WORD>(skill), 0, 0, 0.0f)',
    '0, &target, -1, static_cast<WORD>(skill), 0, 0, 0.0f)')) {
    if (-not $runtime.Contains($arguments)) { $failures += "Dragon SkillIndex: $arguments" }
}
$headStart = $runtime.IndexOf('void CreateDragonHead(')
$headEnd = $runtime.IndexOf('void CreateDragonCrack(', $headStart)
$head = $runtime.Substring($headStart, $headEnd - $headStart)
if ($head -notmatch '(?s)rand\(\) % 360.*rand\(\) % 2') {
    $failures += '6639CE yaw random draw must precede head subtype draw'
}
if ($runtime -notmatch '(?s)if \(subtype == 1\)\s*Vector\(0\.0785f, 0\.314f, 0\.92f, light\);\s*else\s*Vector\(0\.4f, 0\.5f, 0\.92f, light\);') {
    $failures += '663BD2/663BDF CMOVNE: subtype1 uses B6B16C, others B6B184'
}
foreach ($token in @(
    'positions[index][0] = target.Position[0] + cosf(radians) * radius;',
    'positions[index][1] = target.Position[1] + sinf(radians) * radius;',
    'position[0] = effect.Position[0] + cosf(radians) * radius;',
    'position[1] = effect.Position[1] + sinf(radians) * radius;')) {
    if (-not $runtime.Contains($token)) {
        $failures += "verified Dragon radial trig ordering drifted: $token"
    }
}
$windStart = $runtime.IndexOf('if (effect.Type == kDragonViolentWindModel)', $renderStart)
$windEnd = $runtime.IndexOf('if (effect.Type == kDragonViolentLayerModel)', $windStart)
if ($windStart -lt 0 -or $windEnd -le $windStart) {
    throw 'Dragon wind renderer boundaries not found'
}
$wind = $runtime.Substring($windStart, $windEnd - $windStart)
$layerEnd = $runtime.IndexOf('if (!EnsureDragonViolentModel(effect.Type) || !Calc_RenderObject', $windEnd)
if ($layerEnd -le $windEnd) { throw 'Dragon layer renderer end not found' }
$layer = $runtime.Substring($windEnd, $layerEnd - $windEnd)
if (-not $layer.Contains('model.RenderMesh(0, RENDER_TEXTURE,') -or
    $layer.Contains('RENDER_BRIGHT')) {
    $failures += 'twilight layer pass must preserve original flags2 at 0x663762'
}
if (-not $wind.Contains('model.RenderMesh(0, RENDER_TEXTURE,') -or
    $wind.Contains('RENDER_BRIGHT')) {
    $failures += 'wind pass must preserve original flags 2 at 0x663871'
}
if ($beforeEnd.Contains('!effect.Owner->Live')) {
    $failures += 'radial children still use the target-impact live gate'
}
if (-not $endBranch.Contains('if (!effect.Owner || !effect.Owner->Live)')) {
    $failures += 'target-impact owner/live termination is missing'
}
if (-not $beforeEnd.Contains('if (!effect.Owner)')) {
    $failures += 'delayed base null-pointer guard is missing'
}
if (-not $runtime.Contains('for (int index = 0; index < 8; ++index)') -or
    -not $runtime.Contains('CreateDragonHead(positions[index], target)') -or
    -not $runtime.Contains('CreateDragonCrack(positions[index], target)') -or
    -not $runtime.Contains('CreateEffect(kDragonViolentBaseModel, positions[index + 4]')) {
    $failures += 'eight-point immediate/delayed radial chain is incomplete'
}
if (-not $runtime.Contains('BITMAP_JOINT_SPIRIT2') -or
    -not $runtime.Contains('kDragonViolentHeadJointSubtype') -or
    -not $runtime.Contains('CreateDragonWind(effect)') -or
    -not $runtime.Contains('CreateDragonLayers(effect)')) {
    $failures += 'head joint or crack-to-wind/layer child chain is incomplete'
}
if (-not $runtime.Contains('CreateEffect(kDragonViolentEndModel') -or
    -not $runtime.Contains('CreateParticle(kDragonViolentComboBitmap') -or
    -not $runtime.Contains('CreateParticle(kDragonViolentImpactBitmap')) {
    $failures += 'target-follow impact particle chain is incomplete'
}
if (-not $server.Contains('bool CSkillManager::SkillDragonViolent(') -or
    -not $server.Contains('this->GCPegasusTargetsSend(lpObj, lpSkill->m_index, targets, count);')) {
    $failures += 'authoritative multi-target damage/visual list route is missing'
}
if (-not $wsclient.Contains('void ReceiveAttackDamage(const BYTE* ReceiveBuffer)') -or
    -not $wsclient.Contains('SetPlayerShock(c, Damage);')) {
    $failures += 'native damage-packet target reaction is missing'
}
if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Dragon Violent ownership routing drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Dragon Violent radial/impact children, ownership, target list and native reaction are routed'
