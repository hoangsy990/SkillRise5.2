@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /EHsc /std:c++17 /W4 /WX "%~dp0PegasusShiningBirdMathTest.cpp" /Fe:"%~dp0PegasusBuild\PegasusShiningBirdMathTest.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\PegasusShiningBirdMathTest.exe"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0VerifyPegasusShiningBirdRouting.ps1"
exit /b %errorlevel%
