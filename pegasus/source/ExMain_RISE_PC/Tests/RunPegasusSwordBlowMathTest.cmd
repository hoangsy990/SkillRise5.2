@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild" mkdir "%~dp0PegasusBuild"
cl /nologo /std:c++17 /EHsc /W4 /WX /fp:precise "%~dp0PegasusSwordBlowMathTest.cpp" /Fo"%~dp0PegasusBuild\math.obj" /Fe"%~dp0PegasusBuild\math.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\math.exe"
exit /b %errorlevel%
