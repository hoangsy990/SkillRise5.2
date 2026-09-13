$ErrorActionPreference = 'Stop'

$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\ZzzOpenData.cpp') -Raw

$allocation = 'new BMD[MAX_MODELS + 1024 + rise::pegasus::kDynamicModelCapacity]'
$initialization = 'ZeroMemory(Models, rise::pegasus::RuntimeModelLimit(MAX_MODELS) * sizeof(BMD))'

if (-not $source.Contains($allocation)) {
    throw 'FAIL: dynamic Pegasus model tail is not part of the BMD allocation'
}
if (-not $source.Contains($initialization)) {
    throw 'FAIL: dynamic Pegasus model tail is not zero-initialized'
}
if ($source.Contains('ZeroMemory(Models, MAX_MODELS * sizeof(BMD))')) {
    throw 'FAIL: legacy-only BMD initialization boundary is still present'
}

Write-Host 'PASS: allocated and initialized BMD ranges both cover the isolated Pegasus model tail'
