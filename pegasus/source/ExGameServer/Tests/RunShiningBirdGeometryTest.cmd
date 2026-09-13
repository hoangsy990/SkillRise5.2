@echo off
setlocal
python "%~dp0GenerateShiningBirdGeometryTest.py"
if errorlevel 1 exit /b %errorlevel%
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64 >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusBuild\ShiningBirdGeometry.generated.cpp" /Fo"%~dp0PegasusBuild\ShiningBirdGeometry.obj" /Fe"%~dp0PegasusBuild\ShiningBirdGeometry.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\ShiningBirdGeometry.exe"
exit /b %errorlevel%
