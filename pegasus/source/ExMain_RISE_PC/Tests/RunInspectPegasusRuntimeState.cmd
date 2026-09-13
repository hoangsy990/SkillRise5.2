@echo off
setlocal
if "%~1"=="" exit /b 2
if not exist "%~dp0PegasusBuild\RuntimeInspector" mkdir "%~dp0PegasusBuild\RuntimeInspector"
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0InspectPegasusRuntimeState.cpp" /Fo"%~dp0PegasusBuild\RuntimeInspector\state.obj" /Fe"%~dp0PegasusBuild\RuntimeInspector\state.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\RuntimeInspector\state.exe" %1
exit /b %errorlevel%
