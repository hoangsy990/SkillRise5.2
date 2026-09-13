@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild" mkdir "%~dp0PegasusBuild"
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusRainingArrowMathTest.cpp" /Fo"%~dp0PegasusBuild\raining_arrow_math.obj" /Fe"%~dp0PegasusBuild\raining_arrow_math.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\raining_arrow_math.exe"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusRainingArrowRenderer.ps1"
exit /b %errorlevel%
