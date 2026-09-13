@echo off
setlocal
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusSkillLearningRoute.ps1"
exit /b %errorlevel%
