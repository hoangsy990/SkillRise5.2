@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64 >nul
if errorlevel 1 exit /b %errorlevel%
set "OUT=%~dp0PegasusBuild\Tests\PegasusSkillIconCatalogTest.exe"
if not exist "%~dp0PegasusBuild\Tests" mkdir "%~dp0PegasusBuild\Tests"
cl /nologo /std:c++17 /W4 /WX /EHsc /I"%~dp0..\Main5.2_RISE" "%~dp0PegasusSkillIconCatalogTest.cpp" /Fe:"%OUT%"
if errorlevel 1 exit /b %errorlevel%
"%OUT%"
exit /b %errorlevel%
