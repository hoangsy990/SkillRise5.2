@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusAnimatedBmdRouting.ps1"
exit /b %errorlevel%
