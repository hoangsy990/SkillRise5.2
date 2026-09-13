@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusRuntimeTrace.ps1"
exit /b %ERRORLEVEL%
