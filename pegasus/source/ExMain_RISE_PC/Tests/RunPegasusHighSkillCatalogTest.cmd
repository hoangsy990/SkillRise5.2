@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild" mkdir "%~dp0PegasusBuild"
cl /nologo /std:c++17 /EHsc /W4 /WX "%~dp0PegasusHighSkillCatalogTest.cpp" /Fo"%~dp0PegasusBuild\catalog.obj" /Fe"%~dp0PegasusBuild\catalog.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\catalog.exe"
exit /b %errorlevel%
