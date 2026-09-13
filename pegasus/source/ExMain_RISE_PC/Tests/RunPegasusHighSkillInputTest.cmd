@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild\HighSkillInput" mkdir "%~dp0PegasusBuild\HighSkillInput"
cl /nologo /std:c++17 /EHsc /W4 /WX /fp:precise "%~dp0PegasusHighSkillInputTest.cpp" /Fo"%~dp0PegasusBuild\HighSkillInput\input.obj" /Fe"%~dp0PegasusBuild\HighSkillInput\input.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\HighSkillInput\input.exe"
exit /b %errorlevel%
