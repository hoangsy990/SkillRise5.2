@echo off
setlocal
if not exist "%~dp0PegasusBuild" mkdir "%~dp0PegasusBuild"
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusRuntimeQa15ContractTest.cpp" /Fo"%~dp0PegasusBuild\qa15_contract.obj" /Fe"%~dp0PegasusBuild\qa15_contract.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\qa15_contract.exe"
exit /b %errorlevel%
