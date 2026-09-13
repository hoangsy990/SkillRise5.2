$ErrorActionPreference = 'Stop'

$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\ZzzOpenData.cpp') -Raw
$runtimeRoot = Join-Path $PSScriptRoot '..\Main5.2_RISE\RISE'
$required = @(
    'PreflightPegasusRuntimeAssets()',
    'EnsureSwordWrathModel',
    'EnsureSpiralChargeModel',
    'EnsureCrusherChargeModel',
    'EnsureShiningBirdModel',
    'EnsureDexBoosterBitmaps',
    'EnsureChaosBladeModel',
    'EnsureDragonViolentModel',
    'EnsureElementalChargeModel',
    'EnsureHavocSpearModel',
    'EnsureSpearstormModel',
    'EnsureRainingArrowModel',
    'EnsureDeathsideModels',
    'TraceRuntime("ASSET_PREFLIGHT"'
)

$missing = @($required | Where-Object { -not $source.Contains($_) })
if ($missing.Count -ne 0) {
    Write-Host 'FAIL: incomplete isolated Pegasus asset preflight:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

$openImages = $source.IndexOf("`tOpenImages();")
$preflightCall = $source.IndexOf("`tPreflightPegasusRuntimeAssets();", $openImages)
if ($openImages -lt 0 -or $preflightCall -le $openImages) {
    Write-Host 'FAIL: asset preflight must run after native OpenImages'
    exit 1
}

$runtimeSources = Get-ChildItem -LiteralPath $runtimeRoot -File -Filter 'Pegasus*Runtime.cpp' |
    ForEach-Object { Get-Content -LiteralPath $_.FullName -Raw }
if (($runtimeSources -join "`n") -match '\.OZJ"') {
    Write-Host 'FAIL: CGlobalBitmap requires logical .jpg names, not direct .OZJ paths'
    exit 1
}

$chaos = Get-Content -LiteralPath (Join-Path $runtimeRoot 'PegasusChaosBladeRuntime.cpp') -Raw
if (-not $chaos.Contains('model.NumBones > 0 && model.NumActions > 0')) {
    Write-Host 'FAIL: Chaos Blade animation-only carrier is still rejected as a no-mesh model'
    exit 1
}

Write-Host 'PASS: isolated startup preflights all 12 non-Darkness Pegasus asset families'
