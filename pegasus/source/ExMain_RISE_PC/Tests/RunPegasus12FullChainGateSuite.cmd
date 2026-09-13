@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0RunPegasusFullChainGateSuite.ps1"
exit /b %errorlevel%
