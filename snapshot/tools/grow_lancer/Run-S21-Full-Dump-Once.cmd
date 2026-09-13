@echo off
rem Owner-approved one attempt; run as administrator, no automatic elevation.
powershell.exe -NoProfile -File "%~dp0Run-S21-Full-Dump-Once.ps1"
pause
