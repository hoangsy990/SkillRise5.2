@echo off
setlocal
py -3 "%~dp0VerifyPegasusPlayerActionBmd.py"
exit /b %errorlevel%
