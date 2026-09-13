# DK isolated launch checkpoint

Final class: new Grow Lancer; temporary visual QA actor: DK (owner decision).

Stage verifier PASS for build 7400C5B9, complete base/overlay/root files.
Desktop launch used exact RuntimeQA/Client/Engine-Port S21.exe path; resulting
PID 21060 exposed Error text: Data\RISE\Config\Mix.bmd - File not exist.
The file exists under that client and was verified. MixMgr.cpp::OpenRecipeFile
uses relative fopen(szFileName,"rb") and exits on failure. Working-directory
mismatch is the leading hypothesis, not a confirmed process-CWD observation.

The computer-use launcher has no documented working-directory parameter.
Screenshot capture failed with SetIsBorderRequired 0x80004002; accessibility
text worked. Clicking the indexed OK failed due unavailable coordinate geometry;
fresh text observation followed by Return dismissed it. Subsequent process
inspection confirmed the isolated executable no longer running. No restart loop.

Next: launch with verified client working directory or add a QA-only launcher
that pins it, then validate startup. Do not patch production files, overwrite
Data, or change S21 to solve this launch-context issue. Login is user-operated
under computer-use rules. Pegasus was observed running and left untouched.

Follow-up: added start_runtime_qa_client.ps1 with exact isolated path, duplicate
process guard, complete stage verification and explicit WorkingDirectory.
First launch PID 22628 reached WinMain breadcrumb, then exited with no new
dump. Source inspection found a second concrete defect: GetFileNameOfFilePath
truncates at space, so Engine-Port S21.exe becomes Engine-Port. OpenMainExe
uses that name with CreateFile and returns false when it cannot open it.
All three callers of that parser request the current executable. Under the
QA compile flag only, it now obtains the actual module path with
GetModuleFileNameA and keeps the complete basename including spaces.
Legacy non-QA parser is unchanged. Build/stage 935F3EE8 PASS; runtime recheck
is required before calling the launch repair complete.

Recheck: repaired build launched as PID 24216; process remained responsive
with MainWindowTitle Engine-Port S21. Session breadcrumb records WinMain and
QA crash-upload disabled. This proves the repaired build progressed beyond
the previous early exit, not login/character/cast acceptance. Await owner DK login.
