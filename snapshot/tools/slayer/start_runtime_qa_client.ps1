param([switch]$Launch)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source_PC_Slayer'
$qaClient = Join-Path $qaRoot 'ExMain_RISE_PC\Tests\SlayerBuild\RuntimeQA\Client'
$qaExe = Join-Path $qaClient 'Engine-Slayer S21.exe'
if (!(Test-Path -LiteralPath $qaExe -PathType Leaf)) { throw 'Missing isolated Slayer QA executable' }
$resolvedExe = (Resolve-Path -LiteralPath $qaExe).ProviderPath
if ($resolvedExe -ne $qaExe) { throw 'Unexpected executable path' }
$running = @(Get-CimInstance Win32_Process -Filter "Name='Engine-Slayer S21.exe'")
if ($running.Count -gt 0) { throw 'Engine-Slayer S21.exe already running; refusing duplicate launch' }
Push-Location -LiteralPath $qaRoot
try {
    & python 'tools\slayer\verify_runtime_qa_stage.py'
    if ($LASTEXITCODE -ne 0) { throw 'Slayer stage verification failed; not launching' }
} finally { Pop-Location }
Write-Output "Verified executable: $qaExe"
Write-Output "Working directory: $qaClient"
if (!$Launch) { Write-Output 'DRY RUN: use -Launch to open the isolated Slayer client'; return }
$autoQaMarker = Join-Path $qaClient 'SlayerAutoQA.enable'
New-Item -ItemType File -Force -Path $autoQaMarker | Out-Null
$startInfo = New-Object System.Diagnostics.ProcessStartInfo
$startInfo.FileName = $qaExe
$startInfo.WorkingDirectory = $qaClient
$startInfo.UseShellExecute = $false
# Reuse the Vulkan QA credential flow: the client reads only this explicitly
# named Windows Generic Credential and never receives a password on argv.
$startInfo.EnvironmentVariables['RISE_QA_CREDENTIAL'] = 'RISE_QA:SlayerSmoke'
$startInfo.EnvironmentVariables['RISE_SLAYER_QA_ACCOUNT'] = 'admin4'
$startInfo.EnvironmentVariables['RISE_SLAYER_QA_CHARACTER'] = 'Slayer'
# Keep this run on the isolated local Sub-1 stack.  ConnectIP.bmd remains
# hash-pinned production data; the QA-only build consumes this in-memory
# endpoint override before opening the login socket.
$startInfo.EnvironmentVariables['RISE_SLAYER_LOCAL_SERVER_IP'] = '127.0.0.2'
$startInfo.EnvironmentVariables['RISE_SLAYER_LOCAL_SERVER_PORT'] = '55901'
$qaProcess = [System.Diagnostics.Process]::Start($startInfo)
Write-Output "Started isolated Slayer QA PID: $($qaProcess.Id) with saved-credential auto-login marker. Login and cast are not verified."
