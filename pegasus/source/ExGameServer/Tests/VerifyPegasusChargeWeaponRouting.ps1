$ErrorActionPreference = 'Stop'

$source = Get-Content -Raw (Join-Path $PSScriptRoot '..\GameServer\SkillManager.cpp')

$required = @(
    'index == SKILL_SPIRAL_CHARGE',
    'index == SKILL_CRUSHER_CHARGE',
    'index == SKILL_ELEMENTAL_CHARGE',
    'weapon->m_Index / MAX_ITEM_TYPE',
    'section == 0 ? rise::pegasus::PegasusChargeWeaponFamily::Sword',
    'section == 5 ? rise::pegasus::PegasusChargeWeaponFamily::Staff',
    'rise::pegasus::ChargeBuffWeaponAllowed',
    'CheckPacketSkillWeaponForIsolatedQA'
)

foreach ($needle in $required) {
    if (-not $source.Contains($needle)) {
        throw "Missing Pegasus charge weapon routing: $needle"
    }
}

$gateStart = $source.IndexOf('bool CSkillManager::CheckSkillRequireWeapon')
$gateEnd = $source.IndexOf('int CSkillManager::AddSkillWeapon', $gateStart)
if ($gateStart -lt 0 -or $gateEnd -le $gateStart) {
    throw 'Cannot isolate CheckSkillRequireWeapon'
}
$gate = $source.Substring($gateStart, $gateEnd - $gateStart)
if ($gate.IndexOf('ChargeBuffWeaponAllowed') -gt $gate.IndexOf('return 1;')) {
    throw 'Pegasus charge weapon gate is unreachable behind the native early return'
}

Write-Host 'PASS: Pegasus charge buffs reach the exact server equipment gate'
