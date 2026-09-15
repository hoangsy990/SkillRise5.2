# Add classes and items to Rise — read-only class7 comparison

Status: `REFERENCE_ONLY` as inspected 2026-09-15. The other task is active;
these source hashes are a snapshot, not an instruction to copy or merge it.

Source root read only:
`C:\Users\DELL\Documents\Codex\2026-09-14\d\work\Rise-CrossPlatform-S21-full-next`.

| File | SHA-256 | Relevant observed behavior |
|---|---|---|
| `Shared/RiseSeason21Class.h` | `26FE657C1D2D6D24B1E81B7948C44A604CFE93F83A1DC8B0A72852FD329EC634` | Defines sixteen classes and maps GL to RF's seven-column compatibility source. |
| `ExGameServer/GameServer/SkillManager.cpp` | `6AF676CE7A4E8C1BF5F2298A508A7D9DB1587571B978259F43D542E7FB049738` | Copies legacy class-column requirements through that compatibility mapping. |
| `ExGameServer/GameServer/ServerInfo.cpp` | `8B00745BF92A49ECE5EA098F3B73F6C849F1B8AE60ACD59664A8CDAC461E2FAD` | Copies RF rate/matrix entries into GL slots; source comments call these compatibility presets, not S21 final balance. |
| `ExGameServer/GameServer/ObjectManager.cpp` | `30EB21498BA91292672FF6D436B1D80033FF37BDA49C1E99FCE335EA9C526BB5` | Broadens DB class load validation; this cannot prove GL default/skill handlers. |

The Grow Lancer package cannot use RF/DK default rates or seven-column
requirements as S21 parity evidence. Its own SS6 readers consume seven source
tokens and zero the reserved eighth slot. Character learning/casting stays
closed until an explicit class7 package has verified defaults, requirements,
damage-rate matrix and server handlers. The other task's private clone was
not modified, built, launched, messaged or interrupted by this audit.

Positive reusable concept: distinguish the fixed legacy seven-column file
format from destination class capacity and keep a separate DB-class namespace.
No code was copied wholesale. The current task's `CLASS_GL=7` capacity and
unevolved client identity still require an authoritative S21 DB/create/evolution
and equipment contract before full activation.
