@echo off
setlocal
pushd "%~dp0"
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 goto :fail
cl /nologo /std:c++14 /W4 /WX /EHsc GrowLancerServerContractTest.cpp /Fe:GrowLancerServerContractTest.exe
if errorlevel 1 goto :fail
GrowLancerServerContractTest.exe
set result=%errorlevel%
popd
exit /b %result%

:fail
popd
exit /b 1
