@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
if not exist "%~dp0PegasusBuild\Server" mkdir "%~dp0PegasusBuild\Server"
if not exist "%~dp0PegasusBuild\Obj" mkdir "%~dp0PegasusBuild\Obj"
msbuild "%~dp0..\GameServer\GameServer.vcxproj" /m:2 /nologo /v:minimal /p:Configuration=Ex603 /p:Platform=Win32 /p:PegasusTestDefinitions=PEGASUS_ISOLATED_TEST /p:OutDir="%~dp0PegasusBuild/Server/" /p:IntDir="%~dp0PegasusBuild/Obj/" /p:TargetName=GameServer.PegasusImport
exit /b %errorlevel%
