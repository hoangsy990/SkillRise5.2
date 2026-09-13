$ErrorActionPreference = 'Stop'
# Isolate each gate's CALL cursors/compiler environment. Not runtime acceptance.
$clientTests = @(
    'QaEvidence', 'HighSkillCatalog', 'PinnedMetadata', 'RuntimeId',
    'RuntimeTrace', 'SkillIconCatalog', 'PlayerActionBmd', 'CastRootGate',
    'LifecycleCoverage', 'AnimatedBmdRouting', 'RainingArrowMath',
    'DexBoosterContract', 'ChaosBladeRouting', 'HavocSpearMath',
    'SpiralChargeContract', 'CrusherChargeMath', 'ElementalChargeMath',
    'SwordWrathMath', 'SwordBlowMath', 'SwordBlowFlight', 'ShiningBirdMath',
    'DragonViolentContract', 'SpearstormMath', 'TerrainBlendState',
    'DeathsideContract', 'DarknessRouting', 'HighSkillInput', 'DynamicModelId',
    'ShaderModelCapacity', 'AutoQa'
)
$serverTests = @(
    'RunPegasusBuffLifecycleTest.cmd',
    'RunPegasusChargeRangeServerContractTest.cmd',
    'RunShiningBirdGeometryTest.cmd',
    'RunPegasusDamageFamilyRoutingTest.cmd',
    'RunPegasusQaCommandSafetyTest.cmd',
    'RunPegasusRuntimeQa15ContractTest.cmd',
    'RunPegasusSwordWrathServerContractTest.cmd',
    'RunPegasusSkillLearningRouteTest.cmd'
)
$gates = @($clientTests | ForEach-Object {
    Join-Path $PSScriptRoot "RunPegasus$($_)Test.cmd"
})
$gates += Join-Path $PSScriptRoot 'RunWindSoulContractTest.cmd'
$serverRoot = Join-Path $PSScriptRoot '../../ExGameServer/Tests'
$gates += @($serverTests | ForEach-Object { Join-Path $serverRoot $_ })
if (@($gates | Select-Object -Unique).Count -ne $gates.Count) {
    throw 'Duplicate gate in Pegasus suite'
}
foreach ($gate in $gates) {
    if (-not (Test-Path -LiteralPath $gate -PathType Leaf)) {
        throw "Missing required gate: $gate"
    }
}
$completed = 0
foreach ($gate in $gates) {
    Write-Host "[PEGASUS SUITE BEGIN $($completed + 1)/$($gates.Count)] $([IO.Path]::GetFileName($gate))"
    & $env:ComSpec /d /c "`"$gate`""
    if ($LASTEXITCODE -ne 0) {
        throw "Gate failed with exit $LASTEXITCODE : $gate"
    }
    $completed++
    Write-Host "[PEGASUS SUITE END $completed/$($gates.Count)] $([IO.Path]::GetFileName($gate))"
}
if ($completed -ne $gates.Count) { throw 'Incomplete Pegasus suite' }
Write-Host "PASS: $completed/$($gates.Count) source gates completed; 12 original skills + Deathside/Darkness/Wind Soul and separate Fly. Runtime acceptance remains IN_PROCESS."
