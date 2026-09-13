$ErrorActionPreference = 'Stop'

$serverRoot = Split-Path -Parent $PSScriptRoot
$skillManager = Get-Content -Raw -LiteralPath (Join-Path $serverRoot 'GameServer\SkillManager.cpp')
$objectManager = Get-Content -Raw -LiteralPath (Join-Path $serverRoot 'GameServer\ObjectManager.cpp')
$catalog = Get-Content -Raw -LiteralPath (Join-Path (Split-Path -Parent $serverRoot) 'Shared\PegasusHighSkillCatalogData.h')

foreach ($token in @(
    'index >= GET_ITEM(15, 102) && index <= GET_ITEM(15, 113)',
    'FindHighSkillByItem(15, index % MAX_ITEM_TYPE)',
    'return (seed == 0) ? -1 : seed->skillId;'
)) {
    if (-not $skillManager.Contains($token)) {
        throw "Missing Pegasus scroll-to-skill mapping contract: $token"
    }
}

foreach ($token in @(
    'bool CObjectManager::CharacterUseScroll',
    'gItemManager.CheckItemRequireClass(lpObj, lpItem->m_Index)',
    'gSkillManager.GetSkillNumber(lpItem->m_Index, lpItem->m_Level)',
    'gSkillManager.AddSkill(lpObj, skill, lpItem->m_Level)',
    'gSkillManager.GCSkillAddSend(lpObj->Index, slot, skill'
)) {
    if (-not $objectManager.Contains($token)) {
        throw "Missing native Pegasus skill-learning route: $token"
    }
}

foreach ($index in 102..113) {
    if ($catalog -notmatch "\{15,$index,") {
        throw "Missing group-15 learning catalog row: 15:$index"
    }
}

Write-Host 'PASS: item 15:102..113 use the native requirement, add-skill and client-list learning route'
