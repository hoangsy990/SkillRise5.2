$ErrorActionPreference = 'Stop'
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Run the companion CMD using Run as administrator. No automatic elevation is attempted.'
}
$expectedClient = 'D:\MU FICA Season 21\main.exe'
$candidates = @(Get-CimInstance Win32_Process -Filter "Name='main.exe'" |
    Where-Object { $_.ExecutablePath -eq $expectedClient })
if ($candidates.Count -ne 1) {
    throw "Expected exactly one S21 main.exe; found $($candidates.Count). Open S21 first."
}
$pythonExe = 'C:\Users\DELL\AppData\Local\Python\pythoncore-3.14-64\python.exe'
if (-not (Test-Path -LiteralPath $pythonExe -PathType Leaf)) {
    throw 'Verified Python installation is unavailable.'
}
$worktree = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$outputDir = Join-Path $worktree 'artifacts\grow_lancer\live_preflight'
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$logPath = Join-Path $outputDir ('preflight-' + [Guid]::NewGuid().ToString('N') + '.json')
$scriptPath = Join-Path $PSScriptRoot 'preflight_s21_live.py'
$result = & $pythonExe -B $scriptPath --pid $candidates[0].ProcessId
$resultCode = $LASTEXITCODE
$result | Set-Content -LiteralPath $logPath -Encoding UTF8
$result | Write-Output
Write-Host "Saved: $logPath"
Write-Host 'This checks read access and identity only. It does NOT record casts.'
exit $resultCode
