$ErrorActionPreference = 'Stop'

$client = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
$dispatch = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusHighSkillDispatch.h') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw
$effects = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\EffectManager.cpp') -Raw
$attack = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\..\ExGameServer\GameServer\Attack.cpp') -Raw

$requiredClient = @(
    'case rise::pegasus::kDarknessSkill:',
    'SetAction(so, rise::pegasus::kPegasusDarknessAction);',
    'BeginCasterActionTrace(*sc, MagicNumber,',
    'sc->AttackTime = 1;'
)
$failures = @($requiredClient | Where-Object { -not $client.Contains($_) })
if (-not $dispatch.Contains('constexpr int kPegasusDarknessAction = 145;')) {
    $failures += 'Darkness action 145 contract is missing'
}

$caseStart = $client.IndexOf('case rise::pegasus::kDarknessSkill:')
$nextCase = if ($caseStart -ge 0) {
    $client.IndexOf('case rise::pegasus::kRainingArrowSkill:', $caseStart)
} else { -1 }
if ($caseStart -ge 0 -and $nextCase -gt $caseStart) {
    $route = $client.Substring($caseStart, $nextCase - $caseStart)
    foreach ($invented in @('CreateEffect(', 'CreateParticle(', 'CreateJoint(', 'PlayBuffer(')) {
        if ($route.Contains($invented)) {
            $failures += "Darkness route invents unsupported visual/audio call: $invented"
        }
    }
} else {
    $failures += 'Darkness dispatch block could not be isolated'
}
if (-not $server.Contains('case SKILL_DARKNESS:') -or
    -not $server.Contains('return this->SkillPegasusSelfBuff(aIndex, lpSkill);') -or
    -not $server.Contains('NativeEffectForHighSkillBuff(lpSkill->m_index)')) {
    $failures += 'authoritative Darkness self-buff marker route is missing'
}
if (-not $effects.Contains('kDarknessNativeEffect, "Pegasus Darkness"')) {
    $failures += 'collision-safe Darkness effect seed is missing'
}
if (-not $attack.Contains('skill == SKILL_DEATHSIDE') -or
    -not $attack.Contains('GetAttackDamageCursed(lpObj, lpTarget')) {
    $failures += 'paired Deathside curse-damage family route is missing'
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Darkness source-only action/buff route drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: Darkness action/marker and paired curse damage are routed with no invented visuals'
