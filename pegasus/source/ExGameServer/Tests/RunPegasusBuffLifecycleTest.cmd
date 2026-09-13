@echo off
setlocal
python "%~dp0GeneratePegasusBuffLifecycleTest.py"
if errorlevel 1 exit /b %errorlevel%
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusBuild\PegasusBuffLifecycle.generated.cpp" /Fo"%~dp0PegasusBuild\buff_lifecycle.obj" /Fe"%~dp0PegasusBuild\buff_lifecycle.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\buff_lifecycle.exe"
exit /b %errorlevel%
