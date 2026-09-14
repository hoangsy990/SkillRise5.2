# Slayer port audit — `IN_PROCESS`

This audit separates recovered facts from work that still requires the actual
5.2 checkout and a controlled game/server session.

| Requirement | Evidence / implementation | State |
|---|---|---|
| Four skill IDs and names | `shared/SlayerSkillContractData.h` IDs 292..295 | PASS (static) |
| Slayer class profile | `shared/SlayerClassContractData.h` records class 9, creation/evolution names, Lorencia start and guide starting attributes | PASS (static; native class arrays open) |
| Config stats and resource costs | `C:\Users\DELL\Desktop\Skills\\SkillList.xml` mirrored in shared/server records | PASS (static) |
| Merge-ready data rows | `data/SkillList.slayers.fragment.xml`, `SkillRequire.slayers.fragment.xml`, `SkillTreeData_3rd.slayers.fragment.xml`, `SkillSettings.slayers.fragment.ini` | PASS (static) |
| Public descriptions and Darkness/no-element attributes | Webzen guide 100; represented as flags/elements and event order | PASS (static) |
| Config/guide level differences | Separate `skillListLevel`, `guideLevel`, and bead override; no silent substitution | PASS (static) |
| Sword Inertia three swords / one hit | Three cast-tagged events, effect/damage sinks and a per-cast target ledger helper | PASS (focused; native collision binding open) |
| Bat Flock two hits and DOT | Two hit events, five-second duration, four one-second tick events | PASS (focused) |
| Pierce prerequisite and bonus | Bat Flock + ten mastery gate; target debuff controls four-hit branch | PASS (focused) |
| Detection minimap mark and cooldown | Self event and 5,000 ms cooldown; authoritative mark duration intentionally unset | PARTIAL |
| Mastery tree | IDs 779, 780, 781, 782 and 794 mirrored from `SkillTreeData_3rd.xml` | PASS (static) |
| Skill bead/item mapping | Only known mapping group 12/index 479 is enabled from `SkillRequire.xml` | PARTIAL |
| Damage formulas | `RegularSkillCalc.lua` formulas mirrored as double-precision helper and damage sink | PASS (focused) |
| Class column | S21 class number 9 recorded; runtime also rejects non-Slayer/disabled-class contexts; legacy 5.2 `MAX_CLASS == 7` blocks writes | PARTIAL (ABI) |
| Class migration checklist | `CLASS_MIGRATION.md` maps server/client/config/protocol/persistence/asset boundaries | PASS (audit) |
| Packet send/receive | Existing envelopes recovered (`0x19` target/self, `0x1E` duration/dash); skill-specific activation remains `-1` until handler/fan-out validation | PARTIAL |
| Client effects, icons, sounds, minimap UI | IDs/icons are recorded; native binding and proprietary assets are not copied | OPEN |
| Server authority, persistence, reconnect/rollback | No live server or database mutation performed | OPEN |
| Native build and in-game QA | Isolated `_PC_Slayer` Win32 QA build/stage passes with `Engine-Slayer S21.exe`; an actual `CuongDo` session recorded accepted adapter chains for 292/293/294/295 (log verifier passes). Native visual/effect, damage/server and owner acceptance remain open | PARTIAL |

The Grow Lancer package is used as a process/reference source only. No Grow
Lancer hooks, addresses, layouts, or binaries are transplanted into Slayer.
