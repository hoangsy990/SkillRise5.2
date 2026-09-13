@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusDamageFamilyRouting.ps1"
exit /b %errorlevel%
