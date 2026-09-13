# Supplied S21 server: offline evidence boundary

Read-only inventory under `D:\GameServer S21` found one executable and no
matching cpp/h/PDB/map/dmp files in that tree:
`GameServer\IGC.GameServer2_R.exe` (26,030,080 bytes).
SHA-256 `134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74`.

PE inspection: AMD64 (0x8664), image base 0x140000000, entry RVA 0x481B058.
Sections include .vm_sec, .winlice and .boot; entry lies in .boot. Import
descriptors expose one import each. ASCII Wrath, FormulaData, SkillList and
Character do not occur in raw bytes. These observations support a protected
on-disk image, not a recoverable ordinary source-function listing. They do not
prove every code path is impossible to recover or identify the protector version.

No server was executed, attached, dumped, patched or unpacked in this audit.
An available client main dump is not a dump of this AMD64 GameServer and cannot
prove authoritative Wrath duration, formula units, save policy or damage scheduling.

The existing acceptance gate still requires a matching server runtime dump/PDB
or an explicitly approved replacement behavior contract with differences recorded.
Neither has been established by finding formulas in XML. Continue independent
client work from its authorized pinned dump; do not repeatedly treat a formula
table as a server handler or introduce guessed timers merely to unblock builds.
