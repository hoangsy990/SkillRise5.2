@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild\CastRootGate" mkdir "%~dp0PegasusBuild\CastRootGate"
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusCastRootGateTest.cpp" /Fo"%~dp0PegasusBuild\CastRootGate\gate.obj" /Fe"%~dp0PegasusBuild\CastRootGate\gate.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\CastRootGate\gate.exe"
exit /b %errorlevel%
