$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

function Require([bool]$condition, [string]$message) {
    if (-not $condition) { throw "FAIL: $message" }
}

$skillDoc = [xml](Get-Content -Raw -LiteralPath (Join-Path $root 'data\SkillList.slayers.fragment.xml'))
$skills = @($skillDoc.SkillRows.Skill)
Require ($skills.Count -eq 5) 'five SkillList rows'
Require (($skills | ForEach-Object { [int]$_.Index }) -join ',' -eq '292,293,294,295,297') 'SkillList IDs 292..295,297'
Require ([int]$skills[1].ReqLevel -eq 150 -and [int]$skills[1].ReqStrength -eq 100 -and [int]$skills[1].ReqDexterity -eq 380) 'Bat Flock config requirements'
Require ([int]$skills[3].ReqLevel -eq 350 -and [int]$skills[3].ReqDexterity -eq 800 -and [int]$skills[3].Delay -eq 5000) 'Detection config requirements'
Require ([int]$skills[4].ReqLevel -eq 400 -and [int]$skills[4].ReqDexterity -eq 1450 -and [int]$skills[4].Delay -eq 60000) 'Demolish config requirements'

$requireDoc = [xml](Get-Content -Raw -LiteralPath (Join-Path $root 'data\SkillRequire.slayers.fragment.xml'))
$requireRow = $requireDoc.SkillRequireRows.Skill
Require ([int]$requireRow.Index -eq 294 -and [int]$requireRow.ItemGroup -eq 12 -and [int]$requireRow.ItemIndex -eq 479) 'Pierce bead mapping'
Require ([int]$requireRow.ReqIndex -eq 293 -and [int]$requireRow.ReqMasterSkillIndex -eq 782 -and [int]$requireRow.ReqMasterPoint -eq 10) 'Pierce prerequisite mapping'

$buffDoc = [xml](Get-Content -Raw -LiteralPath (Join-Path $root 'data\BuffEffectManager.slayers.fragment.xml'))
$buffs = @($buffDoc.BuffRows.Buff)
Require ($buffs.Count -eq 7) 'seven Slayer BuffEffectManager rows'
Require (($buffs | ForEach-Object { [int]$_.Index }) -join ',' -eq '315,316,317,318,319,320,321') 'Slayer buff indexes 315..321'
$detectionBuff = @($buffs | Where-Object { [int]$_.Index -eq 316 })[0]
Require ([int]$detectionBuff.EffectType -eq 298 -and $detectionBuff.Description -match '1 minute') 'Detection one-minute buff lifetime'
$demolishBuff = @($buffs | Where-Object { [int]$_.Index -eq 317 })[0]
Require ([int]$demolishBuff.EffectType -eq 299 -and [int]$demolishBuff.BuffOutputValue -eq 1) 'Demolish buff effect'

$treeDoc = [xml](Get-Content -Raw -LiteralPath (Join-Path $root 'data\SkillTreeData_3rd.slayers.fragment.xml'))
$magicNumbers = @($treeDoc.SkillTreeRows.Skill | ForEach-Object { [int]$_.MagicNumber })
Require (($magicNumbers -join ',') -eq '779,780,781,782,787,788,794') 'third-master magic numbers'

$settings = Get-Content -LiteralPath (Join-Path $root 'data\SkillSettings.slayers.fragment.ini')
Require (($settings -match '^DetectionSkillShowPlayers\s*=\s*1$').Count -eq 1) 'Detection setting'
Require (($settings -match '^BatFlockDotDamageTime\s*=\s*5$').Count -eq 1) 'Bat Flock DOT setting'

Write-Output 'PASS: Slayer data fragments are well-formed and match the recovered 5.2 rows'
