$ErrorActionPreference = 'Stop'

$root = Join-Path $PSScriptRoot '..\Main5.2_RISE\RISE'
$routes = @(
    @{ Name='Sword Blow model carriers'; File='PegasusSwordBlowRuntime.cpp'; Function='bool RenderSwordBlowModel('; Token='model.PlayAnimation(' },
    @{ Name='Chaos Blade meshless sword'; File='PegasusChaosBladeRuntime.cpp'; Function='void EmitChaosBladeRootFrame('; Token='model.Animation(BoneTransform' },
    @{ Name='Shining Bird'; File='PegasusShiningBirdRuntime.cpp'; Function='void UpdateShiningBird('; Token='model.PlayAnimation(' },
    @{ Name='Elemental Charge'; File='PegasusElementalChargeRuntime.cpp'; Function='bool RenderElementalCharge('; Token='model.PlayAnimation(' },
    @{ Name='Spiral Charge anchor'; File='PegasusSpiralChargeRuntime.cpp'; Function='void UpdateSpiralCharge('; Token='model.PlayAnimation(' },
    @{ Name='Deathside Reaper'; File='PegasusDeathsideRuntime.cpp'; Function='void AdvanceDeathsideAnimation('; Token='model.PlayAnimation(' },
    @{ Name='Deathside scythe carriers'; File='PegasusDeathsideRuntime.cpp'; Function='void UpdateDeathsideScytheTrail('; Token='child.Animation(BoneTransform' },
    @{ Name='Havoc animated carriers'; File='PegasusHavocSpearRuntime.cpp'; Function='bool RenderHavocSpear('; Token='kHavocLinePlaySpeed' }
)

foreach ($route in $routes) {
    $source = Get-Content -Raw -LiteralPath (Join-Path $root $route.File)
    $start = $source.IndexOf($route.Function)
    if ($start -lt 0) {
        throw "$($route.Name): function not found: $($route.Function)"
    }
    $next = $source.IndexOf("`nbool ", $start + $route.Function.Length)
    $nextVoid = $source.IndexOf("`nvoid ", $start + $route.Function.Length)
    if ($next -lt 0 -or ($nextVoid -ge 0 -and $nextVoid -lt $next)) { $next = $nextVoid }
    if ($next -lt 0) { $next = $source.Length }
    $body = $source.Substring($start, $next - $start)
    if (-not $body.Contains($route.Token)) {
        throw "$($route.Name): authored BMD playback route is missing: $($route.Token)"
    }
}

$havoc = Get-Content -Raw -LiteralPath (Join-Path $root 'PegasusHavocSpearRuntime.cpp')
foreach ($token in @('kHavocLinePlaySpeed', 'kHavocShockwavePlaySpeed',
    'if (playSpeed > 0.0f)', 'model.PlayAnimation(')) {
    if (-not $havoc.Contains($token)) {
        throw "Havoc per-type animation contract is missing: $token"
    }
}

$deathside = Get-Content -Raw -LiteralPath (Join-Path $root 'PegasusDeathsideRuntime.cpp')
$deathsideHeader = Get-Content -Raw -LiteralPath (Join-Path $root 'PegasusDeathsideRuntime.h')
if (-not $deathside.Contains('ReaperHead.bmd') -or
    -not $deathsideHeader.Contains('kDeathsideHeadModel')) {
    throw 'ReaperHead must remain loadable/cleanup-addressable despite having no creation xref'
}

Write-Host 'PASS: every source-proven animated Pegasus BMD carrier has an explicit native playback route'
