param(
    [string]$PegasusDataRoot = 'D:\Pegasus\Data'
)

$ErrorActionPreference = 'Stop'

function Assert-Equal {
    param([object]$Actual, [object]$Expected, [string]$Label)
    if ([string]$Actual -cne [string]$Expected) {
        throw "$Label drifted: expected '$Expected', got '$Actual'"
    }
}

function Get-Sha256 {
    param([string]$LiteralPath)
    $stream = [System.IO.File]::OpenRead($LiteralPath)
    try {
        $sha = New-Object System.Security.Cryptography.SHA256Managed
        try {
            return ([System.BitConverter]::ToString($sha.ComputeHash($stream))).Replace('-', '')
        }
        finally { $sha.Dispose() }
    }
    finally { $stream.Dispose() }
}

$skillPath = Join-Path $PegasusDataRoot 'Local\xml\Skill.xml'
$buffPath = Join-Path $PegasusDataRoot 'Local\xml\BuffEffect.xml'
if (-not (Test-Path -LiteralPath $skillPath -PathType Leaf)) { throw "Missing pinned Pegasus Skill.xml: $skillPath" }
if (-not (Test-Path -LiteralPath $buffPath -PathType Leaf)) { throw "Missing pinned Pegasus BuffEffect.xml: $buffPath" }

Assert-Equal (Get-Sha256 $skillPath) '4CD10CFA5BB310C2D89DE2305BEBB259A94BD3814FB64805669689A31657D032' 'Skill.xml SHA-256'
Assert-Equal (Get-Sha256 $buffPath) '1B12B27C6D56EAA721943AD0E15AAAD30F4D376B1F66B83947C17EA2E3BBBF76' 'BuffEffect.xml SHA-256'

[xml]$skillXml = Get-Content -LiteralPath $skillPath -Raw
[xml]$buffXml = Get-Content -LiteralPath $buffPath -Raw

$expectedSkills = @(
    @{ ID=288;  Name='Deathside';        Level=0;   Damage=75;  Mana=120; Gauge=20; Distance=6; Delay=0;    Energy=930;  Class='SUM'; ClassValue=1; Type=0; IsDamage=1; Icon=0 },
    @{ ID=289;  Name='Darkness';         Level=0;   Damage=0;   Mana=100; Gauge=50; Distance=5; Delay=5000; Energy=300;  Class='SUM'; ClassValue=1; Type=1; IsDamage=0; Icon=0 },
    @{ ID=2023; Name='Raining Arrow';    Level=160; Damage=65;  Mana=15;  Gauge=7;  Distance=7; Delay=0;    Energy=0;    Class='ELF'; ClassValue=3; Type=0; IsDamage=1; Icon=0 },
    @{ ID=2024; Name='Dex Booster';      Level=0;   Damage=0;   Mana=30;  Gauge=0;  Distance=0; Delay=0;    Energy=0;    Class='ELF'; ClassValue=3; Type=1; IsDamage=0; Icon=229 },
    @{ ID=2012; Name='Chaos Blade';      Level=160; Damage=350; Mana=19;  Gauge=14; Distance=3; Delay=0;    Energy=0;    Class='MG';  ClassValue=3; Type=0; IsDamage=1; Icon=0 },
    @{ ID=2013; Name='Havoc Spear';      Level=160; Damage=150; Mana=92;  Gauge=20; Distance=6; Delay=0;    Energy=1073; Class='MG';  ClassValue=3; Type=0; IsDamage=1; Icon=0 },
    @{ ID=2014; Name='Spiral Charge';    Level=120; Damage=0;   Mana=21;  Gauge=17; Distance=0; Delay=0;    Energy=0;    Class='MG';  ClassValue=3; Type=1; IsDamage=0; Icon=214 },
    @{ ID=2015; Name='Crusher Charge';   Level=120; Damage=0;   Mana=21;  Gauge=17; Distance=0; Delay=0;    Energy=0;    Class='MG';  ClassValue=3; Type=1; IsDamage=0; Icon=366 },
    @{ ID=2016; Name='Elemental Charge'; Level=120; Damage=0;   Mana=65;  Gauge=20; Distance=0; Delay=0;    Energy=0;    Class='MG';  ClassValue=3; Type=1; IsDamage=0; Icon=367 },
    @{ ID=1500; Name='Sword Wrath';      Level=0;   Damage=0;   Mana=21;  Gauge=17; Distance=0; Delay=0;    Energy=0;    Class='DK';  ClassValue=1; Type=1; IsDamage=0; Icon=213 },
    @{ ID=1501; Name='Sword Blow';       Level=0;   Damage=30;  Mana=19;  Gauge=17; Distance=5; Delay=0;    Energy=0;    Class='DK';  ClassValue=1; Type=0; IsDamage=1; Icon=0 },
    @{ ID=241;  Name='Shining Bird';     Level=80;  Damage=120; Mana=45;  Gauge=5;  Distance=6; Delay=0;    Energy=500;  Class='DW';  ClassValue=1; Type=0; IsDamage=1; Icon=0 },
    @{ ID=242;  Name='Dragon Violent';   Level=50;  Damage=140; Mana=30;  Gauge=5;  Distance=6; Delay=500;  Energy=680;  Class='RF';  ClassValue=1; Type=0; IsDamage=1; Icon=0 },
    @{ ID=243;  Name='Spearstorm';       Level=160; Damage=160; Mana=105; Gauge=5;  Distance=6; Delay=300;  Energy=1160; Class='DL';  ClassValue=1; Type=0; IsDamage=1; Icon=0 }
)

$skillNodes = @($skillXml.SkillTemplate.Skill)
foreach ($expected in $expectedSkills) {
    $node = @($skillNodes | Where-Object { [int]$_.ID -eq $expected.ID })
    if ($node.Count -ne 1) { throw "Skill $($expected.ID) must have exactly one pinned XML row; found $($node.Count)" }
    $node = $node[0]
    foreach ($field in @('Name','Level','Damage','Mana','Distance','Delay','Energy','TypeSkill','IsDamage','Magic_Icon')) {
        $expectedField = switch ($field) {
            'TypeSkill' { $expected.Type }
            'Magic_Icon' { $expected.Icon }
            default { $expected[$field] }
        }
        Assert-Equal $node.$field $expectedField "Skill $($expected.ID) $field"
    }
    Assert-Equal $node.AbilityGuage $expected.Gauge "Skill $($expected.ID) AbilityGuage"
    Assert-Equal $node.($expected.Class) $expected.ClassValue "Skill $($expected.ID) class $($expected.Class)"
}

$expectedBuffs = @(
    @{ Index=213; Group=128; Name='Sword Wrath';      Skill=1500; Icon=213; Description='Increase +1 Range and +1 Damage Radius for Death Stab and Sword Blow.' },
    @{ Index=214; Group=129; Name='Spiral Charge';    Skill=2014; Icon=214; Description='Changes into Two-Handed Sword Magic Gladiator./Increases Attack and Defense./Chaos Blade range increased to 6.' },
    @{ Index=215; Group=130; Name='Crusher Charge';   Skill=2015; Icon=366; Description='Changes into One-Handed Sword Magic Gladiator./Increases Attack Speed and Defense./Chaos Blade range increased to 5.' },
    @{ Index=227; Group=131; Name='Elemental Charge'; Skill=2016; Icon=367; Description='Converts to a Wizard-type Magic Gladiator./Havoc Spear range increased to 7./Every third Havoc Spear explodes a Nova.' },
    @{ Index=228; Group=132; Name='Darkness';         Skill=289;  Icon=0;   Description='Increases Curse damage and Defense.' },
    @{ Index=229; Group=133; Name='Dex Booster';      Skill=2024; Icon=229; Description='Increases attack speed and/attack success rate.' }
)

$buffNodes = @($buffXml.BuffEffectData.BuffEffect)
foreach ($expected in $expectedBuffs) {
    $node = @($buffNodes | Where-Object { [int]$_.Index -eq $expected.Index })
    if ($node.Count -ne 1) { throw "Buff $($expected.Index) must have exactly one pinned XML row; found $($node.Count)" }
    $node = $node[0]
    Assert-Equal $node.Group $expected.Group "Buff $($expected.Index) Group"
    Assert-Equal $node.Name $expected.Name "Buff $($expected.Index) Name"
    Assert-Equal $node.Description $expected.Description "Buff $($expected.Index) Description"
    Assert-Equal $node.Property.BuffType 1 "Buff $($expected.Index) BuffType"
    Assert-Equal $node.Property.NoticeType 1 "Buff $($expected.Index) NoticeType"
    Assert-Equal $node.Property.ClearType 1 "Buff $($expected.Index) ClearType"

    $skill = @($skillNodes | Where-Object { [int]$_.ID -eq $expected.Skill })[0]
    Assert-Equal $skill.Magic_Icon $expected.Icon "Skill $($expected.Skill) Magic_Icon namespace"
}

# The pinned metadata describes semantics only.  It does not authorize guessed
# GameServer stat magnitudes for these five buffs.
$forbiddenFormulaAttributes = @('Attack','AttackSpeed','AttackSuccessRate','Defense','DefenseSuccessRate','CurseDamage','Value','Value1','Value2','Percent')
foreach ($index in @(214,215,227,228,229)) {
    $node = @($buffNodes | Where-Object { [int]$_.Index -eq $index })[0]
    foreach ($attribute in $forbiddenFormulaAttributes) {
        if ($null -ne $node.Attributes[$attribute] -or $null -ne $node.Property.Attributes[$attribute]) {
            throw "Buff $index unexpectedly gained numeric formula attribute '$attribute'; audit server provenance before using it"
        }
    }
}

Write-Host 'PASS: pinned Pegasus Skill/Buff XML hashes, 14 skill rows, 6 buff rows, and icon namespaces'
