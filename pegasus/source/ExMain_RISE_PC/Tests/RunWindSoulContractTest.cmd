@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild\WindSoulContract" mkdir "%~dp0PegasusBuild\WindSoulContract"
cl /nologo /std:c++17 /EHsc /W4 /WX /fp:precise "%~dp0WindSoulContractTest.cpp" /Fo"%~dp0PegasusBuild\WindSoulContract\contract.obj" /Fe"%~dp0PegasusBuild\WindSoulContract\contract.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\WindSoulContract\contract.exe"
exit /b %errorlevel%
