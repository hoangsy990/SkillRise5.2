# Slayer port audit — `IN_PROCESS`

This audit separates recovered facts from work that still requires the actual
5.2 checkout and a controlled game/server session.

| Requirement | Evidence / implementation | State |
|---|---|---|
| Four skill IDs and names | `shared/SlayerSkillContractData.h` IDs 292..295 | PASS (static) |
| Config stats and resource costs | `C:\Users\DELL\Desktop\Skills\\SkillList.xml` mirrored in shared/server records | PASS (static) |
| Merge-ready data rows | `data/SkillList.slayers.fragment.xml`, `SkillRequire.slayers.fragment.xml`, `SkillTreeData_3rd.slayers.fragment.xml`, `SkillSettings.slayers.fragment.ini` | PASS (static) |
| Public descriptions and Darkness/no-element attributes | Webzen guide 100; represented as flags/elements and event order | PASS (static) |
| Config/guide level differences | Separate `skillListLevel`, `guideLevel`, and bead override; no silent substitution | PASS (static) |
| Sword Inertia three swords / one hit | Three events; collision consumer must keep a per-cast target ledger | PARTIAL |
| Bat Flock two hits and DOT | Two hit events, five-second duration, four one-second tick events | PASS (focused) |
| Pierce prerequisite and bonus | Bat Flock + ten mastery gate; target debuff controls four-hit branch | PASS (focused) |
| Detection minimap mark and cooldown | Self event and 5,000 ms cooldown; authoritative mark duration intentionally unset | PARTIAL |
| Mastery tree | IDs 779, 780, 781, 782 and 794 mirrored from `SkillTreeData_3rd.xml` | PASS (static) |
| Skill bead/item mapping | Only known mapping group 12/index 479 is enabled from `SkillRequire.xml` | PARTIAL |
| Damage formulas | `RegularSkillCalc.lua` formulas mirrored as double-precision helper | PASS (focused) |
| Class column | S21 class number 9 recorded; legacy 5.2 `MAX_CLASS == 7` blocks writes | BLOCKED (ABI) |
| Packet send/receive | Target/self/dash families classified; opcode remains `-1` until send/receive pair is recovered | BLOCKED (ABI) |
| Client effects, icons, sounds, minimap UI | IDs/icons are recorded; native binding and proprietary assets are not copied | OPEN |
| Server authority, persistence, reconnect/rollback | No live server or database mutation performed | OPEN |
| Native build and in-game QA | Focused adapter test only; no claim of full build or owner acceptance | OPEN |

The Grow Lancer package is used as a process/reference source only. No Grow
Lancer hooks, addresses, layouts, or binaries are transplanted into Slayer.
