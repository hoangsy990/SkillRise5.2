@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusAutoQa.ps1"
exit /b %errorlevel%
