@echo off
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusSwordBlowPreview.ps1"
exit /b %errorlevel%
