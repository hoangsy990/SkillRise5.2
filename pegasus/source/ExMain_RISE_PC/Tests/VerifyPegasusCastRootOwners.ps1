$ErrorActionPreference = 'Stop'
$dispatch = Get-Content -LiteralPath (Join-Path $PSScriptRoot '..\Main5.2_RISE\WSclient.cpp') -Raw
$roots = @('CreateChaosBladeRoot', 'CreateHavocSpearRoot',
    'CreateDragonViolentRoots', 'CreateSpearstormRoot')
foreach ($root in $roots) {
    if (-not $dispatch.Contains("$root(*so, MagicNumber);") -or
        $dispatch.Contains("$root(*to, MagicNumber);")) {
        throw "$root must receive caster, not victim"
    }
}
Write-Output 'PASS: four recovered cast roots receive caster; source guard only, not runtime acceptance'
