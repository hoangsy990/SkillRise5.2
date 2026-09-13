@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusTerrainBlendState.ps1"
exit /b %errorlevel%
