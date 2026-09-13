$ErrorActionPreference = 'Stop'

$runtimeRoot = Join-Path $PSScriptRoot '..\Main5.2_RISE\RISE'
$files = @(
    'PegasusCrusherChargeRuntime.cpp',
    'PegasusDexBoosterRuntime.cpp',
    'PegasusDragonViolentRuntime.cpp',
    'PegasusSwordWrathRuntime.cpp'
)
$failures = @()
foreach ($file in $files) {
    $source = Get-Content -LiteralPath (Join-Path $runtimeRoot $file) -Raw
    $terrainCalls = ([regex]::Matches($source,
        'RenderTerrainAlphaBitmap\s*\(')).Count
    # Native RISE has two valid terrain passes: additive EnableAlphaBlend and
    # translucent EnableAlphaTest. Both restore through DisableAlphaBlend,
    # which clears GL_BLEND and GL_ALPHA_TEST and restores depth/cull state.
    $blendEnables = ([regex]::Matches($source,
        'EnableAlphaBlend\s*\(')).Count
    $testEnables = ([regex]::Matches($source,
        'EnableAlphaTest\s*\(')).Count
    $enables = $blendEnables + $testEnables
    $disables = ([regex]::Matches($source, 'DisableAlphaBlend\s*\(')).Count
    if ($terrainCalls -eq 0) {
        $failures += "${file}: no terrain-alpha route found"
    }
    if ($enables -eq 0 -or $enables -ne $disables) {
        $failures += "${file}: unbalanced terrain blend state ($enables enable, $disables disable)"
    }
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Pegasus terrain blend-state contract:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: every Pegasus terrain-alpha renderer brackets native additive/translucent state'
