param([switch]$Apply)
$ErrorActionPreference = 'Stop'
$taskRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$qaDir = Join-Path $taskRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client'
$binDir = Join-Path $taskRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Bin'
$candidate = Join-Path $binDir 'Engine-Port S21.exe'
$target = Join-Path $qaDir 'Engine-Port S21.exe'
$backup = Join-Path $binDir 'Engine-Port S21.before-body-conflict-954E11.exe'
$oldHash = '954E1111085888E7AF9DB96DB78098571FBADD947A6186A24AAE587E5FF71DAE'
$newHash = '7C30031DF6240329ED2688DBA159684EBFA10182236DB41CCE83CFB38D396F92'
function Assert-PrivatePath([string]$path) {
    $full = [IO.Path]::GetFullPath($path)
    if (!$full.StartsWith($taskRoot + '\',[StringComparison]::OrdinalIgnoreCase)) { throw "Outside worktree: $full" }
    $cursor = $full
    while ($cursor.Length -ge $taskRoot.Length) {
        if (Test-Path -LiteralPath $cursor) {
            if ((Get-Item -LiteralPath $cursor).Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Reparse path: $cursor" }
        }
        $cursor = [IO.Path]::GetDirectoryName($cursor)
    }
}
foreach ($path in @($candidate,$target,$backup)) { Assert-PrivatePath $path }
if ((Get-FileHash -LiteralPath $candidate).Hash -ne $newHash) { throw 'Candidate changed; review and repin before staging' }
$current = (Get-FileHash -LiteralPath $target).Hash
if ($current -ne $oldHash -and $current -ne $newHash) { throw 'Unexpected staged executable' }
if (Test-Path -LiteralPath $backup) {
    if ((Get-FileHash -LiteralPath $backup).Hash -ne $oldHash) { throw 'Backup conflict' }
}
function Test-Stage([string]$expected) {
    & python -c 'import sys; sys.path.insert(0, sys.argv[1]); import verify_runtime_qa_stage as v; v.main(sys.argv[2])' $PSScriptRoot $expected
    if ($LASTEXITCODE) { throw 'Full QA asset/executable preflight failed' }
}
Test-Stage $current
$running = @(Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and $_.ExecutablePath.Equals($target,[StringComparison]::OrdinalIgnoreCase)
})
if (!$Apply) {
    Write-Output "DRY RUN: candidate=$newHash current=$current runningPids=$($running.ProcessId -join ','); no writes or launch"
    return
}
if ($running.Count) { throw 'QA client is still running; no executable replaced' }
if ($current -eq $newHash) {
    if (!(Test-Path -LiteralPath $backup)) { throw 'Missing rollback backup' }
    Write-Output 'Already staged; no changes or launch'
    return
}
if (!(Test-Path -LiteralPath $backup)) { Copy-Item -LiteralPath $target -Destination $backup }
if ((Get-FileHash -LiteralPath $backup).Hash -ne $oldHash) { throw 'Backup verification failed' }
# Recheck immediately before replacement; another task must not launch this QA client.
if (Get-CimInstance Win32_Process | Where-Object { $_.ExecutablePath -and $_.ExecutablePath.Equals($target,[StringComparison]::OrdinalIgnoreCase) }) { throw 'QA client launched during preflight' }
if ((Get-FileHash -LiteralPath $target).Hash -ne $oldHash) { throw 'Target changed during preflight' }
if ((Get-FileHash -LiteralPath $candidate).Hash -ne $newHash) { throw 'Candidate changed during preflight' }
Copy-Item -LiteralPath $candidate -Destination $target
Test-Stage $newHash
Write-Output "PASS staged $newHash; rollback=$backup; no client launch or opt-in performed"
