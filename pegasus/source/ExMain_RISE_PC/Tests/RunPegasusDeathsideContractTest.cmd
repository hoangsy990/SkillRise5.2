@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild\DeathsideContract" mkdir "%~dp0PegasusBuild\DeathsideContract"
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusDeathsideContractTest.cpp" /Fo"%~dp0PegasusBuild\DeathsideContract\test.obj" /Fe"%~dp0PegasusBuild\DeathsideContract\test.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\DeathsideContract\test.exe"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusDeathsideTrailRouting.ps1"
exit /b %errorlevel%
