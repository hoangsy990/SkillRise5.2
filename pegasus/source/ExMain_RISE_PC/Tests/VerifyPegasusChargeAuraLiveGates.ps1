$ErrorActionPreference = 'Stop'
$runtime = Join-Path $PSScriptRoot '../Main5.2_RISE/RISE'
$contracts = @(
    @('PegasusSpiralChargeRuntime.cpp', 'CreateSpiralChargeAnchor', 'owner'),
    @('PegasusCrusherChargeRuntime.cpp', 'CreateCrusherChargeAura', 'owner'),
    @('PegasusElementalChargeRuntime.cpp', 'CreateElementalChargeBuffAura', 'target')
)
foreach ($contract in $contracts) {
    $source = Get-Content -LiteralPath (Join-Path $runtime $contract[0]) -Raw
    # Ignore explanatory comments, then require the live early-return as the
    # first operation. A late gate after DeleteEffect would not be equivalent.
    $source = [regex]::Replace($source, '//[^\r\n]*|/\*[\s\S]*?\*/', '')
    $pattern = 'void\s+' + $contract[1] + '\(OBJECT&\s+' + $contract[2] +
        '\)\s*\{\s*if\s*\(!' + $contract[2] + '\.Live\)\s*return;'
    if ($source -notmatch $pattern) { throw "Missing first-operation live gate: $($contract[1])" }
}
Write-Host 'PASS: three source-proven buff-add live gates precede aura search/removal/creation (static)'
