$ErrorActionPreference = 'Stop'

$client = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\WSclient.cpp') -Raw
$effects = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\ZzzEffect.cpp') -Raw
$server = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\..\ExGameServer\GameServer\SkillManager.cpp') -Raw

$routes = @(
    @{ Name='Raining Arrow'; Skill='kRainingArrowSkill'; Server='SKILL_RAINING_ARROW'; Create='CreateRainingArrowRoot'; Trace='kRainingArrowRootModel'; Init='InitializeRainingArrow'; Update='UpdateRainingArrow'; Render='RenderRainingArrow' },
    @{ Name='Dex Booster'; Skill='kDexBoosterSkill'; Server='SKILL_DEX_BOOSTER'; Create='CreateDexBooster'; Trace='kDexBoosterAuraModel'; Init='InitializeDexBooster'; Update='UpdateDexBooster'; Render='RenderDexBooster' },
    @{ Name='Chaos Blade'; Skill='kChaosBladeSkill'; Server='SKILL_CHAOS_BLADE'; Create='CreateChaosBladeRoot'; Trace='kChaosBladeRootModel'; Init='InitializeChaosBlade'; Update='UpdateChaosBlade'; Render='RenderChaosBlade' },
    @{ Name='Havoc Spear'; Skill='kHavocSpearSkill'; Server='SKILL_HAVOC_SPEAR'; Create='CreateHavocSpearRoot'; Trace='kHavocRootModel'; Init='InitializeHavocSpear'; Update='UpdateHavocSpear'; Render='RenderHavocSpear' },
    @{ Name='Spiral Charge'; Skill='kSpiralChargeSkill'; Server='SKILL_SPIRAL_CHARGE'; Create='CreateSpiralChargeRoot'; Trace='kSpiralChargeRootModel'; Init='InitializeSpiralCharge'; Update='UpdateSpiralCharge'; Render='RenderSpiralCharge' },
    @{ Name='Crusher Charge'; Skill='kCrusherChargeSkill'; Server='SKILL_CRUSHER_CHARGE'; Create='CreateCrusherChargeRoots'; Trace='kCrusherChargeCoreModel'; Init='InitializeCrusherCharge'; Update='UpdateCrusherCharge'; Render='RenderCrusherCharge' },
    @{ Name='Elemental Charge'; Skill='kElementalChargeSkill'; Server='SKILL_ELEMENTAL_CHARGE'; Create='CreateElementalCharge'; Trace='kElementalChargeModel'; Init='InitializeElementalCharge'; Update='UpdateElementalCharge'; Render='RenderElementalCharge' },
    @{ Name='Sword Wrath'; Skill='kSwordWrathSkill'; Server='SKILL_SWORD_WRATH'; Create='kSwordWrathRootModel'; Trace='kSwordWrathRootModel'; Init='InitializeSwordWrath'; Update='UpdateSwordWrath'; Render='RenderSwordWrath' },
    @{ Name='Sword Blow'; Skill='kSwordBlowSkill'; Server='SKILL_SWORD_BLOW'; Create='CreateSwordBlowCasterRoot'; Trace='MODEL_PEGASUS_SWORD_BLOW_BIG'; Init='InitializeSwordBlowSmallSword'; Update='UpdateSwordBlowSmallSword'; Render='RenderSwordBlowModel' },
    @{ Name='Shining Bird'; Skill='kShiningBirdSkill'; Server='SKILL_SHINING_BIRD'; Create='CreateShiningBird'; Trace='kShiningBirdModel'; Init='InitializeShiningBird'; Update='UpdateShiningBird'; Render='RenderShiningBird' },
    @{ Name='Dragon Violent'; Skill='kDragonViolentSkill'; Server='SKILL_DRAGON_VIOLENT'; Create='CreateDragonViolentRoots'; Trace='kDragonViolentHeadModel'; Init='InitializeDragonViolent'; Update='UpdateDragonViolent'; Render='RenderDragonViolent' },
    @{ Name='Spearstorm'; Skill='kSpearstormSkill'; Server='SKILL_SPEARSTORM'; Create='CreateSpearstormRoot'; Trace='kSpearstormRootModel'; Init='InitializeSpearstorm'; Update='UpdateSpearstorm'; Render='RenderSpearstorm' }
)

$missing = @()
$rootGatedSkills = @(
    'kDeathsideSkill',
    'kRainingArrowSkill',
    'kChaosBladeSkill',
    'kHavocSpearSkill',
    'kSwordBlowSkill',
    'kShiningBirdSkill',
    'kDragonViolentSkill',
    'kSpearstormSkill'
)
foreach ($route in $routes) {
    foreach ($check in @(
        @{ File='WSclient.cpp'; Text=$client; Token="case rise::pegasus::$($route.Skill):" },
        @{ File='SkillManager.cpp'; Text=$server; Token="case $($route.Server):" },
        @{ File='WSclient.cpp'; Text=$client; Token="rise::pegasus::$($route.Create)" },
        @{ File='WSclient.cpp'; Text=$client; Token=($route.Trace + ', "DISPATCHED"') },
        @{ File='ZzzEffect.cpp'; Text=$effects; Token="rise::pegasus::$($route.Init)" },
        @{ File='ZzzEffect.cpp'; Text=$effects; Token="rise::pegasus::$($route.Update)" },
        @{ File='ZzzEffect.cpp'; Text=$effects; Token="rise::pegasus::$($route.Render)" }
    )) {
        if (-not $check.Text.Contains($check.Token)) {
            $missing += "$($route.Name): $($check.File) missing $($check.Token)"
        }
    }
}

foreach ($skill in $rootGatedSkills) {
    $casePattern = "case rise::pegasus::${skill}:`r?`n(?s:.*?)break;"
    $caseMatch = [regex]::Match($client, $casePattern)
    if (-not $caseMatch.Success -or
        -not $caseMatch.Value.Contains('ShouldDispatchPegasusCastRoot(MagicNumber, SourceKey)')) {
        $missing += "${skill}: WSclient.cpp missing per-cast root duplicate gate"
    }
}

if (-not $client.Contains('Success != 0 ? "HIT" : "MISS"')) {
    $missing += 'WSclient.cpp CAST_ECHO must label the target success bit as HIT/MISS'
}

if ($missing.Count -ne 0) {
    Write-Host 'FAIL: incomplete Pegasus runtime routing:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host "PASS: all $($routes.Count) group-15 skills have server dispatch plus client create, initialize, update and render routing"
