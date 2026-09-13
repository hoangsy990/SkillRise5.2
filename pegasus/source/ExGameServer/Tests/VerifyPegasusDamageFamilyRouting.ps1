$ErrorActionPreference = 'Stop'

$attack = Get-Content -Raw (Join-Path $PSScriptRoot '..\GameServer\Attack.cpp')

$wizardCall = 'damage = this->GetAttackDamageWizard(lpObj, lpTarget, lpSkill, &effect, defense);'
$physicalCall = 'damage = this->GetAttackDamage(lpObj, lpTarget, lpSkill, &effect, defense);'
$wizardCallAt = $attack.IndexOf($wizardCall)
$physicalCallAt = $attack.IndexOf($physicalCall, $wizardCallAt)
if ($wizardCallAt -lt 0 -or $physicalCallAt -le $wizardCallAt) {
    throw 'Cannot isolate native physical/wizard damage-family selector'
}

$selectorStart = $attack.LastIndexOf('else if ((lpObj->Class == CLASS_DW', $wizardCallAt)
if ($selectorStart -lt 0) {
    throw 'Cannot find native wizard-family condition'
}
$selector = $attack.Substring($selectorStart, $wizardCallAt - $selectorStart)

if (-not $selector.Contains('skill != SKILL_CHAOS_BLADE')) {
    throw 'Chaos Blade must bypass the MG wizard-family fallback'
}
if ($selector.Contains('skill != SKILL_HAVOC_SPEAR')) {
    throw 'Havoc Spear must remain in the MG wizard-family fallback'
}

$curseCall = 'damage = this->GetAttackDamageCursed(lpObj, lpTarget, lpSkill, &effect, defense);'
$curseCallAt = $attack.IndexOf($curseCall)
$curseSelectorStart = $attack.LastIndexOf('else if (lpObj->Class == CLASS_SU', $curseCallAt)
if ($curseSelectorStart -lt 0 -or $curseCallAt -le $curseSelectorStart) {
    throw 'Cannot isolate native Summoner curse-family condition'
}
$curseSelector = $attack.Substring($curseSelectorStart, $curseCallAt - $curseSelectorStart)
if (-not $curseSelector.Contains('skill == SKILL_DEATHSIDE')) {
    throw 'Deathside must use the Summoner curse damage family'
}

$skillXml = 'D:\Pegasus\Data\Local\xml\Skill.xml'
if (-not (Test-Path -LiteralPath $skillXml)) {
    throw "Pinned Pegasus Skill.xml is missing: $skillXml"
}
$xml = [xml](Get-Content -Raw -LiteralPath $skillXml)
$chaos = $xml.SkillTemplate.Skill | Where-Object { [int]$_.ID -eq 2012 }
$havoc = $xml.SkillTemplate.Skill | Where-Object { [int]$_.ID -eq 2013 }
if ($null -eq $chaos -or $null -eq $havoc) {
    throw 'Pinned Pegasus Skill.xml lacks Chaos Blade or Havoc Spear'
}
if ([int]$chaos.Energy -ne 0 -or [int]$havoc.Energy -le 0) {
    throw 'Pinned Pegasus physical/wizard evidence drifted'
}

$buffXml = 'D:\Pegasus\Data\Local\xml\BuffEffect.xml'
if (-not (Test-Path -LiteralPath $buffXml)) {
    throw "Pinned Pegasus BuffEffect.xml is missing: $buffXml"
}
$buffs = [xml](Get-Content -Raw -LiteralPath $buffXml)
$darkness = $buffs.BuffEffectData.BuffEffect | Where-Object { [int]$_.Index -eq 228 }
if ($null -eq $darkness -or -not $darkness.Description.Contains('Curse damage')) {
    throw 'Pinned Pegasus Darkness-to-Curse evidence drifted'
}

Write-Host 'PASS: Deathside uses curse, Chaos Blade physical, and Havoc Spear wizard damage'
