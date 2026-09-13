@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
cl /nologo /EHsc /std:c++17 "%~dp0PegasusDynamicModelIdTest.cpp" /Fe:"%~dp0PegasusBuild\PegasusDynamicModelIdTest.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\PegasusDynamicModelIdTest.exe"
exit /b %errorlevel%
