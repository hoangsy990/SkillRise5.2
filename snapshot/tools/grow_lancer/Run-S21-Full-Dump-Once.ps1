$ErrorActionPreference = 'Stop'
$principal = [Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Use Run as administrator on the companion CMD. No automatic elevation.'
}
$candidates = @(Get-CimInstance Win32_Process -Filter "Name='main.exe'" |
    Where-Object { $_.ExecutablePath -eq 'D:\MU FICA Season 21\main.exe' })
if ($candidates.Count -ne 1) { throw 'Expected exactly one running S21 main.exe.' }
$pythonExe = 'C:\Users\DELL\AppData\Local\Python\pythoncore-3.14-64\python.exe'
Write-Host 'ONE full dump. Local only. May pause/crash S21. Do not run preflight again.'
& $pythonExe -B (Join-Path $PSScriptRoot 'capture_s21_full_once.py') --pid $candidates[0].ProcessId
exit $LASTEXITCODE
