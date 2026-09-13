$ErrorActionPreference = 'Stop'

$runtimeRoot = Join-Path $PSScriptRoot '..\Main5.2_RISE\RISE'
$effectSource = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\ZzzEffect.cpp') -Raw
$deathsideSource = Get-Content -LiteralPath (Join-Path $runtimeRoot 'PegasusDeathsideRuntime.cpp') -Raw
$elementalSource = Get-Content -LiteralPath (Join-Path $runtimeRoot 'PegasusElementalChargeRuntime.cpp') -Raw
$havocSource = Get-Content -LiteralPath (Join-Path $runtimeRoot 'PegasusHavocSpearRuntime.cpp') -Raw
$swordBlowSource = Get-Content -LiteralPath (Join-Path $runtimeRoot 'PegasusSwordBlowRuntime.cpp') -Raw
$swordWrathSource = Get-Content -LiteralPath (Join-Path $runtimeRoot 'PegasusSwordWrathRuntime.cpp') -Raw

$skills = @(
    @{ Name='Raining Arrow'; File='PegasusRainingArrowRuntime.cpp'; Init='InitializeRainingArrow'; Update='UpdateRainingArrow'; Render='RenderRainingArrow' },
    @{ Name='Dex Booster'; File='PegasusDexBoosterRuntime.cpp'; Init='InitializeDexBooster'; Update='UpdateDexBooster'; Render='RenderDexBooster' },
    @{ Name='Chaos Blade'; File='PegasusChaosBladeRuntime.cpp'; Init='InitializeChaosBlade'; Update='UpdateChaosBlade'; Render='RenderChaosBlade' },
    @{ Name='Havoc Spear'; File='PegasusHavocSpearRuntime.cpp'; Init='InitializeHavocSpear'; Update='UpdateHavocSpear'; Render='RenderHavocSpear' },
    @{ Name='Spiral Charge'; File='PegasusSpiralChargeRuntime.cpp'; Init='InitializeSpiralCharge'; Update='UpdateSpiralCharge'; Render='RenderSpiralCharge' },
    @{ Name='Crusher Charge'; File='PegasusCrusherChargeRuntime.cpp'; Init='InitializeCrusherCharge'; Update='UpdateCrusherCharge'; Render='RenderCrusherCharge' },
    @{ Name='Elemental Charge'; File='PegasusElementalChargeRuntime.cpp'; Init='InitializeElementalCharge'; Update='UpdateElementalCharge'; Render='RenderElementalCharge' },
    @{ Name='Sword Wrath'; File='PegasusSwordWrathRuntime.cpp'; Init='InitializeSwordWrath'; Update='UpdateSwordWrath'; Render='RenderSwordWrath' },
    @{ Name='Sword Blow'; File='PegasusSwordBlowRuntime.cpp'; Init='InitializeSwordBlowSmallSword'; Update='UpdateSwordBlowSmallSword'; Render='RenderSwordBlowModel' },
    @{ Name='Shining Bird'; File='PegasusShiningBirdRuntime.cpp'; Init='InitializeShiningBird'; Update='UpdateShiningBird'; Render='RenderShiningBird' },
    @{ Name='Dragon Violent'; File='PegasusDragonViolentRuntime.cpp'; Init='InitializeDragonViolent'; Update='UpdateDragonViolent'; Render='RenderDragonViolent' },
    @{ Name='Spearstorm'; File='PegasusSpearstormRuntime.cpp'; Init='InitializeSpearstorm'; Update='UpdateSpearstorm'; Render='RenderSpearstorm' }
)

$failures = @()
foreach ($skill in $skills) {
    $path = Join-Path $runtimeRoot $skill.File
    if (-not (Test-Path -LiteralPath $path)) {
        $failures += "$($skill.Name): missing $($skill.File)"
        continue
    }

    $source = Get-Content -LiteralPath $path -Raw
    $checks = @(
        @{ Gate='initializer'; Pattern=[regex]::Escape($skill.Init) },
        @{ Gate='update'; Pattern=[regex]::Escape($skill.Update) },
        @{ Gate='renderer'; Pattern=[regex]::Escape($skill.Render) },
        @{ Gate='lifetime'; Pattern='\bLifeTime\b' },
        @{ Gate='asset/load guard'; Pattern='Ensure[A-Za-z0-9_]*(Model|Bitmap|Bitmaps)|LoadImageFile|OpenModel' },
        @{ Gate='child or visible layer'; Pattern='CreateEffect|CreateParticle|CreateJoint|CreateSprite|RenderTerrainAlphaBitmap|Draw_RenderObject|RenderMesh' }
    )

    foreach ($check in $checks) {
        if ($source -notmatch $check.Pattern) {
            $failures += "$($skill.Name): missing static $($check.Gate) evidence"
        }
    }

    foreach ($route in @($skill.Init, $skill.Update, $skill.Render)) {
        if (-not $effectSource.Contains("rise::pegasus::$route")) {
            $failures += "$($skill.Name): ZzzEffect.cpp missing $route route"
        }
    }
}

foreach ($persistent in @(
    @{ Name='Crusher Charge'; File='PegasusCrusherChargeRuntime.cpp'; Delete='DeleteCrusherChargeAura' },
    @{ Name='Elemental Charge'; File='PegasusElementalChargeRuntime.cpp'; Delete='DeleteElementalChargeAuras' }
)) {
    $source = Get-Content -LiteralPath (Join-Path $runtimeRoot $persistent.File) -Raw
    # Pegasus removes these persistent auras through the authoritative
    # buff-remove callback. A local g_isCharacterBuff poll is not lifecycle
    # evidence and can race add/relog packet restoration.
    if (-not $source.Contains($persistent.Delete)) {
        $failures += "$($persistent.Name): packet-owned persistent aura deletion route is missing"
    }
    if (-not $source.Contains('BUFF_EFFECT_CLEAR')) {
        $failures += "$($persistent.Name): missing physical-effect cleanup telemetry"
    }
}

$spiralSource = Get-Content -LiteralPath (Join-Path $runtimeRoot `
    'PegasusSpiralChargeRuntime.cpp') -Raw
if (-not $spiralSource.Contains('g_isCharacterBuff(effect.Owner,') -or
    -not $spiralSource.Contains('kSpiralChargeNativeEffect')) {
    $failures += 'Spiral Charge: source-proven anchor-local buff gate is missing'
}

# These launched child chains explicitly have no owner-live truncation in
# their pinned Pegasus updaters. Keep that absence from being treated as a
# missing generic cleanup implementation.
$chaosSource = Get-Content -LiteralPath (Join-Path $runtimeRoot `
    'PegasusChaosBladeRuntime.cpp') -Raw
$chaosUpdateStart = $chaosSource.IndexOf('void UpdateChaosBlade(')
$chaosRenderStart = $chaosSource.IndexOf('bool RenderChaosBlade(', $chaosUpdateStart)
if ($chaosUpdateStart -lt 0 -or $chaosRenderStart -lt 0 -or
    $chaosSource.Substring($chaosUpdateStart,
        $chaosRenderStart - $chaosUpdateStart).Contains('Owner->Live')) {
    $failures += 'Chaos Blade: launched cascade lifetime drifted from owner-independent Pegasus updater'
}
$birdSource = Get-Content -LiteralPath (Join-Path $runtimeRoot `
    'PegasusShiningBirdRuntime.cpp') -Raw
$birdUpdateStart = $birdSource.IndexOf('void UpdateShiningBird(')
$birdRenderStart = $birdSource.IndexOf('bool RenderShiningBird(', $birdUpdateStart)
if ($birdUpdateStart -lt 0 -or $birdRenderStart -lt 0 -or
    $birdSource.Substring($birdUpdateStart,
        $birdRenderStart - $birdUpdateStart).Contains('Owner->Live')) {
    $failures += 'Shining Bird: launched BMD lifetime drifted from owner-independent Pegasus updater'
}

foreach ($gate in @(
    'rand_fps_check(kDeathsideWaterfallCadence)',
    'rand_fps_check(kDeathsideGroundSmokeCadence)',
    'rand_fps_check(kDeathsideSmokeCadence)'
)) {
    if (-not $deathsideSource.Contains($gate)) {
        $failures += "Deathside: missing native RISE FPS-normalized emission gate $gate"
    }
}

foreach ($gate in @(
    'model.PlayAnimation(&effect.AnimationFrame',
    'kDeathsideMinimumPlaySpeed',
    'effect.CurrentAction = kDeathsidePostAttackAction'
)) {
    if (-not $deathsideSource.Contains($gate)) {
        $failures += "Deathside: missing source-proven Reaper/scythe BMD animation gate $gate"
    }
}

if (-not $elementalSource.Contains('model.PlayAnimation(&effect.AnimationFrame')) {
    $failures += 'Elemental Charge: 05_buff_obj BMD frame is not advanced before rendering'
}

foreach ($gate in @(
    'kHavocLinePlaySpeed',
    'kHavocShockwavePlaySpeed',
    'model.PlayAnimation(&effect.AnimationFrame'
)) {
    if (-not $havocSource.Contains($gate)) {
        $failures += "Havoc Spear: missing source-proven line/shockwave BMD animation gate $gate"
    }
}

if (-not $swordBlowSource.Contains(
    'effect.Velocity / kSwordBlowModelPlayDivisor')) {
    $failures += 'Sword Blow: 0x27B5..0x27BD carrier BMD route lost velocity/6 animation'
}

if (-not $swordWrathSource.Contains(
    'Models[owner->Type].TransformByObjectBone(position, owner, 17, relative);')) {
    $failures += 'Sword Wrath: accent does not follow the player owner model bone 17'
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: incomplete Pegasus lifecycle coverage:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host "PASS: all $($skills.Count) requested Pegasus skills expose initializer, update, renderer, source-specific lifetime/cleanup, asset and visible-child coverage"
Write-Host 'NOTE: this is a static regression gate; cast/reaction/visual parity still require isolated runtime evidence'
