@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild" mkdir "%~dp0PegasusBuild"
set "OUT=%~dp0PegasusBuild\PegasusSpiralChargeContractTest.exe"
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusSpiralChargeContractTest.cpp" /Fe:"%OUT%"
if errorlevel 1 exit /b %errorlevel%
"%OUT%"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusSpiralChargeRouting.ps1"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusChargeAuraLiveGates.ps1"
exit /b %errorlevel%
