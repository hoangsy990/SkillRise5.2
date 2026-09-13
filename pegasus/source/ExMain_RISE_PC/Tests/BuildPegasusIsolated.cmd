@echo off
setlocal
call "%ProgramFiles%\Microsoft Visual Studio\18\Insiders\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=amd64
if errorlevel 1 exit /b %errorlevel%
msbuild "%~dp0..\Main.vcxproj" /m:2 /nologo /v:minimal /p:Configuration="Global Release" /p:Platform=Win32 /p:PegasusIsolatedBuild=true /p:SolutionDir="%~dp0../" /p:OutDir="%~dp0PegasusBuild/Client/" /p:IntDir="%~dp0PegasusBuild/Obj/" /p:TargetName=Engine-Pegasus
exit /b %errorlevel%
