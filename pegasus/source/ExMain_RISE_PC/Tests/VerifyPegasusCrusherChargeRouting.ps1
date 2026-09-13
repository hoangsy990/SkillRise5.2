$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusCrusherChargeRuntime.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateCrusherCharge(OBJECT&')
$renderStart = $runtime.IndexOf('bool RenderCrusherCharge(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Crusher Charge updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$failures = @()
if (-not $runtime.Contains('0, &owner, -1, 0, 0, 0, 0.05f)') -or
    -not $runtime.Contains('0, &owner, -1, 0, 0)') -or
    $runtime.Contains('static_cast<WORD>(skill)')) {
    $failures += '65B051..65B079: cast carriers require zero SkillIndex and Kind'
}

# Pegasus 0x65A49A..0x65A4B9 initializes 0x27AE with Scale=0 and Alpha=0;
# updater 0x65A7EF dispatches only 0x27AF..0x27B1. It is an intentionally
# invisible root/carrier, not a missing renderer that should be made visible.
if (-not $runtime.Contains('case kCrusherChargeCoreModel:') -or
    -not $runtime.Contains('effect.Scale = 0.0f;') -or
    -not $runtime.Contains('effect.Alpha = 0.0f;')) {
    $failures += 'source-proven transparent core initialization drifted'
}
if ($update.Contains('kCrusherChargeCoreModel')) {
    $failures += 'invented Crusher core updater would make the transparent carrier visible'
}

# Pegasus initializer/updater 0x65A482..0x65A494 and 0x65A73A..0x65A74C
# apply one owner/live gate to all 0x27AE..0x27B1 effects.
if (-not $update.Contains('if (!effect.Owner || !effect.Owner->Live)')) {
    $failures += 'source-proven global owner/live gate is missing'
}

# The persistent 0x27B1 update only follows Owner::Position. Source buff D7
# is removed by the separate callback at 0x65B129, not polled in the updater.
if ($update.Contains('g_isCharacterBuff')) {
    $failures += 'non-Pegasus local buff expiration remains in updater'
}
if (-not $update.Contains('VectorCopy(effect.Owner->Position, effect.Position);')) {
    $failures += 'persistent aura owner follow is missing'
}
if (-not $runtime.Contains('void DeleteCrusherChargeAura(OBJECT& owner)')) {
    $failures += 'packet-owned aura deletion route is missing'
}

$wsclient = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
if (-not $wsclient.Contains('rise::pegasus::CreateCrusherChargeAura(*o);')) {
    $failures += 'buff-add aura route is missing'
}
if (-not $wsclient.Contains('rise::pegasus::DeleteCrusherChargeAura(*o);')) {
    $failures += 'buff-remove aura route is missing'
}
if (-not $runtime.Contains('CreateShard(ring, 5, 1.2f') -or
    -not $runtime.Contains('CreateShard(ring, 5, 1.4f') -or
    ([regex]::Matches($runtime, [regex]::Escape('CreateShard(ring, 4, 0.5f'))).Count -ne 2) {
    $failures += 'four-child ring cascade is incomplete'
}
if (-not $runtime.Contains('for (int layer = 0; layer < 2; ++layer)') -or
    -not $runtime.Contains('for (int index = 0; index < kCrusherChargeAuraBoneCount; ++index)') -or
    -not $runtime.Contains('CreateSprite(g_crusherFlareMonoBitmap')) {
    $failures += 'persistent two-ring/seventeen-bone aura renderer is incomplete'
}
if (-not $runtime.Contains('const float thresholds[] = {32.0f, 28.0f, 24.0f, 20.0f};') -or
    -not $runtime.Contains('model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT')) {
    $failures += 'timed ring cascade or exact mesh-0 renderer is missing'
}
if (-not $server.Contains('case SKILL_CRUSHER_CHARGE:') -or
    -not $server.Contains('return this->SkillPegasusSelfBuff(aIndex, lpSkill);')) {
    $failures += 'authoritative Crusher Charge server marker route is missing'
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Crusher Charge lifetime routing drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Crusher Charge transparent carrier, child cascade, aura and packet removal match Pegasus'
