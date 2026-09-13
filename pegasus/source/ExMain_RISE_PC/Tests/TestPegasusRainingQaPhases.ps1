$ErrorActionPreference = 'Stop'
$source = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'AnalyzePegasusRuntimeQa.ps1')
$start = $source.IndexOf('$arrowTerminations =')
$end = $source.IndexOf('$movementContracts =', $start)
if ($start -lt 0 -or $end -le $start) { throw 'Missing phase acceptance block' }
$contract = [scriptblock]::Create($source.Substring($start, $end - $start))
function Check-PhaseFixture([string]$Fault) {
    $reactionCastCount = 10
    $failures = @()
    $run = @(
        foreach ($phase in 10..20) {
            foreach ($cast in 1..10) {
                $ending = if ($phase -ge 16) { 'ABOVE_LIMIT' } else { 'TERRAIN' }
                [pscustomobject]@{ Stage='RAINING_ARROW_LAUNCH_PHASE'; Value1=$phase;
                    Result=$(if ($phase -ge 16) { 'ASCENDING' } else { 'FALLING_VOLLEY' }) }
                if (-not ($Fault -eq 'missing' -and $phase -eq 10 -and $cast -eq 1)) {
                    [pscustomobject]@{ Stage='RAINING_ARROW_TERMINATION';
                        Value1=$(if ($Fault -eq 'unknown' -and $phase -eq 10) { -1 } else { $phase });
                        Result=$(if ($Fault -eq 'wrong' -and $phase -eq 10) { 'ABOVE_LIMIT' } else { $ending }) }
                }
                [pscustomobject]@{ Stage='PROJECTILE_IMPACT'; Value1=2023; Result=$ending }
            }
        }
    )
    . $contract
    if (($Fault -eq '') -ne ($failures.Count -eq 0)) {
        throw "Incorrect acceptance for phase fixture '$Fault': $failures"
    }
}
foreach ($fault in @('', 'wrong', 'unknown', 'missing')) { Check-PhaseFixture $fault }
Write-Output 'PASS: actual Raining QA accepts proven phases and rejects wrong, unknown, missing endpoints'
