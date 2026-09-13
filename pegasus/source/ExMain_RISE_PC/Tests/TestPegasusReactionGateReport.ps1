$ErrorActionPreference = 'Stop'
$source = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'AnalyzePegasusRuntimeQa.ps1') -Raw
$begin = $source.IndexOf('$damageContracts = @(')
$end = $source.IndexOf('# The five self-buffs', $begin)
if ($begin -lt 0 -or $end -le $begin) { throw 'Missing actual analyzer reaction block' }
$block = [scriptblock]::Create($source.Substring($begin, $end - $begin))
$run = @(
    [pscustomobject]@{Stage='TARGET_REACTION_GATE'; Value1=2013; Value2=1; Result='NATIVE_RANDOM_GATE'},
    [pscustomobject]@{Stage='TARGET_REACTION_GATE'; Value1=2013; Value2=1; Result='NATIVE_RANDOM_GATE'},
    [pscustomobject]@{Stage='TARGET_REACTION_GATE'; Value1=1501; Value2=1; Result='EXCLUDED_MONSTER_275'},
    [pscustomobject]@{Stage='TARGET_REACTION'; Value1=2012; Value2=1; Result='APPLIED'},
    [pscustomobject]@{Stage='TARGET_REACTION'; Value1=241; Value2=1; Result='NATIVE_CALL_WITHOUT_SHOCK_ACTION'}
)
$failures = @()
. $block
if ($reactionPassCount -ne 1) { throw 'Only observed APPLIED may pass reaction acceptance' }
if ($reactionGateSummary -notcontains 'REACTION_GATE skill=2013 applied=0 reasons=NATIVE_RANDOM_GATE:2') {
    throw 'Random gates were not counted per skill'
}
if ($reactionGateSummary -notcontains 'REACTION_GATE skill=1501 applied=0 reasons=EXCLUDED_MONSTER_275:1') {
    throw 'Excluded target reason was lost'
}
if ($reactionGateSummary -notcontains 'REACTION_GATE skill=241 applied=0 reasons=NONE_OBSERVED') {
    throw 'Missing gate evidence was inferred'
}
foreach ($name in @('Havoc Spear', 'Sword Blow', 'Shining Bird')) {
    if ($failures -notcontains "${name}: native SetPlayerShock reaction was not applied to the cast target") {
        throw "Gate diagnostics incorrectly waived acceptance: $name"
    }
}
Write-Host 'PASS: actual analyzer block reports gate reasons without waiving missing native reactions'
