$ErrorActionPreference = 'Stop'

$source = Get-Content -Raw -LiteralPath (
    Join-Path $PSScriptRoot '..\GameServer\SkillManager.cpp')

function Get-FunctionBody([string]$signature) {
    $start = $source.IndexOf($signature, [StringComparison]::Ordinal)
    if ($start -lt 0) { throw "Missing function: $signature" }
    $open = $source.IndexOf('{', $start)
    if ($open -lt 0) { throw "Missing function body: $signature" }
    $depth = 0
    for ($i = $open; $i -lt $source.Length; ++$i) {
        if ($source[$i] -eq '{') { ++$depth }
        elseif ($source[$i] -eq '}') {
            --$depth
            if ($depth -eq 0) { return $source.Substring($open, $i - $open + 1) }
        }
    }
    throw "Unterminated function body: $signature"
}

$targetFunctions = @(
	'bool CSkillManager::SkillDeathside(',
	'bool CSkillManager::SkillSwordBlow(',
    'bool CSkillManager::SkillDragonViolent(',
    'bool CSkillManager::SkillSpearstorm(',
    'bool CSkillManager::SkillShiningBird(',
    'bool CSkillManager::SkillSwordWrath(',
    'bool CSkillManager::SkillPegasusSelfBuff('
)

foreach ($signature in $targetFunctions) {
    $body = Get-FunctionBody $signature
    if ($body -notmatch 'GCSkillAttackSend\s*\(') {
        throw "Accepted Pegasus cast has no authoritative 0x19 echo: $signature"
    }
}

$dispatch = Get-FunctionBody 'bool CSkillManager::RunningSkill('
foreach ($skill in @(
	'SKILL_DEATHSIDE', 'SKILL_DARKNESS',
	'SKILL_RAINING_ARROW', 'SKILL_CHAOS_BLADE', 'SKILL_HAVOC_SPEAR',
    'SKILL_SHINING_BIRD', 'SKILL_DRAGON_VIOLENT', 'SKILL_SPEARSTORM',
    'SKILL_SWORD_BLOW', 'SKILL_SWORD_WRATH', 'SKILL_SPIRAL_CHARGE',
    'SKILL_CRUSHER_CHARGE', 'SKILL_ELEMENTAL_CHARGE', 'SKILL_DEX_BOOSTER')) {
    if ($dispatch -notmatch [regex]::Escape($skill)) {
        throw "RunningSkill is missing Pegasus dispatch: $skill"
    }
}

Write-Output 'PASS: all 14 Pegasus skill IDs have explicit server dispatch; accepted runtime routes own their 0x19 cast echoes.'
