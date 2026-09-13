@echo off
setlocal
set "ROOT=%~dp0.."
set "OUT=%~dp0PegasusBuild\Tests"
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x64
if errorlevel 1 exit /b %errorlevel%
if not exist "%OUT%" mkdir "%OUT%"

cl /nologo /std:c++17 /EHsc /W4 /WX /I"%ROOT%\Main5.2_RISE" ^
  "%~dp0PegasusShaderModelCapacityTest.cpp" ^
  /Fe:"%OUT%\PegasusShaderModelCapacityTest.exe"
if errorlevel 1 exit /b %errorlevel%

"%OUT%\PegasusShaderModelCapacityTest.exe"
exit /b %errorlevel%
