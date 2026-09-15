param(
    [switch]$Launch,
    [switch]$AutoWrath,
    [switch]$MagicFixedTick,
    [switch]$MagicRasterProbe,
    [switch]$SpinCrossRasterProbe,
    [switch]$WrathPixelProbe,
    [switch]$LocalServer
)

$ErrorActionPreference = 'Stop'
if ($AutoWrath) { throw '-AutoWrath retired: QA buffs must be armed manually with F12 after login' }
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$qaClient = Join-Path $qaRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client'
$qaExe = Join-Path $qaClient 'Engine-Port S21.exe'
if ($Launch -and !$LocalServer) {
    throw 'Grow Lancer gameplay QA launch requires -LocalServer; refusing default/VPS endpoint'
}
if (!(Test-Path -LiteralPath $qaExe -PathType Leaf)) { throw 'Missing isolated QA executable' }
$resolvedExe = (Resolve-Path -LiteralPath $qaExe).ProviderPath
if ($resolvedExe -ne $qaExe) { throw 'Unexpected executable path' }
$running = @(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'")
if ($running.Count -gt 0) { throw 'Engine-Port S21 already running; refusing duplicate launch' }
if ($LocalServer) {
    $stackRoot = Join-Path $qaRoot 'ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke\'
    foreach ($port in @(44412,55910)) {
        $listener = @(Get-NetTCPConnection -LocalPort $port -State Listen -ErrorAction SilentlyContinue)
        if ($listener.Count -ne 1) { throw "Grow Lancer local QA port is not ready: $port" }
        $process = Get-CimInstance Win32_Process -Filter "ProcessId=$($listener[0].OwningProcess)"
        if (!$process -or !$process.ExecutablePath -or
            !$process.ExecutablePath.StartsWith($stackRoot,[StringComparison]::OrdinalIgnoreCase)) {
            throw "Local QA listener $port is not owned by Grow Lancer stack"
        }
    }
}
Push-Location -LiteralPath $qaRoot
try {
    & python 'tools\grow_lancer\verify_runtime_qa_stage.py'
    if ($LASTEXITCODE -ne 0) { throw 'Stage verification failed; not launching' }
} finally { Pop-Location }
Write-Output "Verified executable: $qaExe"
Write-Output "Working directory: $qaClient"
if ($LocalServer) { Write-Output 'ConnectServer override: 127.0.0.1:44412 (isolated Grow Lancer stack)' }
if (!$Launch) { Write-Output 'DRY RUN: use -Launch to open the isolated client'; return }
# Interactive game explicitly requested by owner; do not hide the game window.
# Both QA opt-ins are inherited by the child only; restore this shell even if
# Start-Process fails. Never persist user/machine environment values.
$hadAutoWrath = Test-Path Env:RISE_GL_AUTO_WRATH_QA
$previousAutoWrath = if ($hadAutoWrath) { $env:RISE_GL_AUTO_WRATH_QA } else { $null }
$hadMagicTick = Test-Path Env:RISE_GL_MAGIC_FIXED_TICK_QA
$previousMagicTick = if ($hadMagicTick) { $env:RISE_GL_MAGIC_FIXED_TICK_QA } else { $null }
$hadMagicRaster = Test-Path Env:RISE_GL_MAGIC_RASTER_QA
$previousMagicRaster = if ($hadMagicRaster) { $env:RISE_GL_MAGIC_RASTER_QA } else { $null }
$hadSpinCrossRaster = Test-Path Env:RISE_GL_SPIN_CROSS_RASTER_QA
$previousSpinCrossRaster = if ($hadSpinCrossRaster) { $env:RISE_GL_SPIN_CROSS_RASTER_QA } else { $null }
$hadWrathPixel = Test-Path Env:RISE_GL_WRATH_PIXEL_QA
$previousWrathPixel = if ($hadWrathPixel) { $env:RISE_GL_WRATH_PIXEL_QA } else { $null }
$hadLocalServer = Test-Path Env:RISE_GL_LOCAL_SERVER_QA
$previousLocalServer = if ($hadLocalServer) { $env:RISE_GL_LOCAL_SERVER_QA } else { $null }
try
{
    # Never auto-register a QA buff on login. The only fake buff fixture is
    # an explicit in-game F12 press; incoming server buffs remain separate.
    $env:RISE_GL_AUTO_WRATH_QA = '0'
    # Default remains OFF even if the parent shell happened to inherit an
    # old opt-in. Only -MagicFixedTick sets 1 for the new QA process.
    $env:RISE_GL_MAGIC_FIXED_TICK_QA = if ($MagicFixedTick) { '1' } else { '0' }
    $env:RISE_GL_MAGIC_RASTER_QA = if ($MagicRasterProbe) { '1' } else { '0' }
    $env:RISE_GL_SPIN_CROSS_RASTER_QA = if ($SpinCrossRasterProbe) { '1' } else { '0' }
    $env:RISE_GL_WRATH_PIXEL_QA = if ($WrathPixelProbe) { '1' } else { '0' }
    $env:RISE_GL_LOCAL_SERVER_QA = if ($LocalServer) { '1' } else { '0' }
    # Native Util_CheckOption searches slash-prefixed tokens with inline values.
    $arguments = if ($LocalServer) { '/u127.0.0.1 /p44412' } else { '' }
    $qaProcess = Start-Process -FilePath $qaExe -ArgumentList $arguments -WorkingDirectory $qaClient -PassThru
}
finally
{
    if ($hadAutoWrath) { $env:RISE_GL_AUTO_WRATH_QA = $previousAutoWrath }
    else { Remove-Item Env:RISE_GL_AUTO_WRATH_QA -ErrorAction SilentlyContinue }
    if ($hadMagicTick) { $env:RISE_GL_MAGIC_FIXED_TICK_QA = $previousMagicTick }
    else { Remove-Item Env:RISE_GL_MAGIC_FIXED_TICK_QA -ErrorAction SilentlyContinue }
    if ($hadMagicRaster) { $env:RISE_GL_MAGIC_RASTER_QA = $previousMagicRaster }
    else { Remove-Item Env:RISE_GL_MAGIC_RASTER_QA -ErrorAction SilentlyContinue }
    if ($hadSpinCrossRaster) { $env:RISE_GL_SPIN_CROSS_RASTER_QA = $previousSpinCrossRaster }
    else { Remove-Item Env:RISE_GL_SPIN_CROSS_RASTER_QA -ErrorAction SilentlyContinue }
    if ($hadWrathPixel) { $env:RISE_GL_WRATH_PIXEL_QA = $previousWrathPixel }
    else { Remove-Item Env:RISE_GL_WRATH_PIXEL_QA -ErrorAction SilentlyContinue }
    if ($hadLocalServer) { $env:RISE_GL_LOCAL_SERVER_QA = $previousLocalServer }
    else { Remove-Item Env:RISE_GL_LOCAL_SERVER_QA -ErrorAction SilentlyContinue }
}
if ($MagicFixedTick)
    { Write-Output 'Opt-in Magic Pin 40 ms QA timing enabled only in this child process; no visual PASS inferred.' }
if ($MagicRasterProbe)
    { Write-Output 'Opt-in peak-alpha Magic Pin GL sample query enabled only in this child; GPU samples are diagnostic, not visual PASS.' }
if ($SpinCrossRasterProbe)
    { Write-Output 'Opt-in bounded Spin cross gameplay GL sample query enabled only in this child; samples are diagnostic, not visual PASS.' }
if ($WrathPixelProbe)
    { Write-Output 'Opt-in bounded Wrath terrain before/after framebuffer probe enabled only in this child; owner visual remains OPEN.' }
Write-Output "Started isolated QA PID: $($qaProcess.Id). Login and cast are not verified."
