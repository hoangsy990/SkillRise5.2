@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusDarknessRouting.ps1"
exit /b %errorlevel%
