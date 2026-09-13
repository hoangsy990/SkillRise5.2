@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars32.bat" >nul
if errorlevel 1 exit /b %errorlevel%
cl /nologo /EHsc /std:c++17 /W4 /WX "%~dp0PegasusQaEvidenceTest.cpp" /Fo:"%~dp0PegasusBuild\PegasusQaEvidenceTest.obj" /Fe:"%~dp0PegasusBuild\PegasusQaEvidenceTest.exe"
if errorlevel 1 exit /b %errorlevel%
"%~dp0PegasusBuild\PegasusQaEvidenceTest.exe"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0TestPegasusQaLogSharing.ps1"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0TestPegasusRainingQaPhases.ps1"
if errorlevel 1 exit /b %errorlevel%
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0TestPegasusReactionGateReport.ps1"
exit /b %errorlevel%
