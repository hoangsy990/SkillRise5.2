$ErrorActionPreference = 'Stop'

$effects = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\ZzzEffect.cpp') -Raw
$required = @(
    'EnsureSwordBlowModel',
    'EnsureSwordWrathModel',
    'EnsureSpiralChargeModel',
    'EnsureCrusherChargeModel',
    'EnsureShiningBirdModel',
    'EnsureChaosBladeModel',
    'EnsureDragonViolentModel',
    'EnsureElementalChargeModel',
    'EnsureHavocSpearModel',
    'EnsureSpearstormModel',
    'EnsureRainingArrowModel',
    'EnsureDeathsideModels',
    'TraceRuntime("MODEL_ENSURE", Type, SubType, "FAIL")',
    'TraceRuntimeModelStateOnce("EFFECT_RENDER", o->Type'
)

$missing = @($required | Where-Object { -not $effects.Contains($_) })
if ($missing.Count -ne 0) {
    Write-Host 'FAIL: missing Pegasus model-load guards:'
    $missing | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: all Pegasus model families have a load guard and failed-load trace'
