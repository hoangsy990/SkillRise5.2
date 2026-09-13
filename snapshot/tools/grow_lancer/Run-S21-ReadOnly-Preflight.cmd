@echo off
rem Owner runs this file with Run as administrator. No automatic UAC or bypass.
powershell.exe -NoProfile -File "%~dp0Run-S21-ReadOnly-Preflight.ps1"
pause
