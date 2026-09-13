@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
cl /nologo /EHsc /std:c++17 /W4 /WX "%~dp0PegasusCrusherChargeMathTest.cpp" /Fe:"%~dp0PegasusBuild\PegasusCrusherChargeMathTest.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\PegasusCrusherChargeMathTest.exe"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusCrusherChargeRouting.ps1"
exit /b %errorlevel%
