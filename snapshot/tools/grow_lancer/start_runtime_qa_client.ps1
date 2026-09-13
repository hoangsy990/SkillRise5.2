param([switch]$Launch)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$qaClient = Join-Path $qaRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client'
$qaExe = Join-Path $qaClient 'Engine-Port S21.exe'
if (!(Test-Path -LiteralPath $qaExe -PathType Leaf)) { throw 'Missing isolated QA executable' }
$resolvedExe = (Resolve-Path -LiteralPath $qaExe).ProviderPath
if ($resolvedExe -ne $qaExe) { throw 'Unexpected executable path' }
$running = @(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'")
if ($running.Count -gt 0) { throw 'Engine-Port S21 already running; refusing duplicate launch' }
Push-Location -LiteralPath $qaRoot
try {
    & python 'tools\grow_lancer\verify_runtime_qa_stage.py'
    if ($LASTEXITCODE -ne 0) { throw 'Stage verification failed; not launching' }
} finally { Pop-Location }
Write-Output "Verified executable: $qaExe"
Write-Output "Working directory: $qaClient"
if (!$Launch) { Write-Output 'DRY RUN: use -Launch to open the isolated client'; return }
# Interactive game explicitly requested by owner; do not hide the game window.
$qaProcess = Start-Process -FilePath $qaExe -WorkingDirectory $qaClient -PassThru
Write-Output "Started isolated QA PID: $($qaProcess.Id). Login and cast are not verified."
