# Slayer port audit — `IN_PROCESS`

This audit separates recovered facts from work that still requires the actual
5.2 checkout and a controlled game/server session.

| Requirement | Evidence / implementation | State |
|---|---|---|
| Five skill IDs and names | `shared/SlayerSkillContractData.h` IDs 292..295,297 | PASS (static) |
| Slayer class profile | `shared/SlayerClassContractData.h` records class 9, creation/evolution names, Lorencia start and guide starting attributes | PASS (static; native class arrays open) |
| Config stats and resource costs | S21 `D:\GameServer S21\Data\Skills\SkillList.xml` (SHA-256 `3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0`) mirrored in shared/server records | PASS (static) |
| Merge-ready data rows | `data/SkillList.slayers.fragment.xml`, `SkillRequire.slayers.fragment.xml`, `BuffEffectManager.slayers.fragment.xml`, `SkillTreeData_3rd.slayers.fragment.xml`, `SkillSettings.slayers.fragment.ini` | PASS (static) |
| Public descriptions and Darkness/no-element attributes | Webzen guide 100; represented as flags/elements and event order | PASS (static) |
| Config/guide level differences | Separate `skillListLevel`, `guideLevel`, and bead override; no silent substitution | PASS (static) |
| Sword Inertia three swords / one hit | Three cast-tagged events, effect/damage sinks and a per-cast target ledger helper | PASS (focused; native collision binding open) |
| Bat Flock two hits and DOT | Two hit events, five-second duration, four one-second tick events | PASS (focused) |
| Pierce prerequisite and bonus | Bat Flock + ten mastery gate; target debuff controls four-hit branch | PASS (focused) |
| Detection minimap mark and cooldown | Self event, 5,000 ms cooldown, and Buff 316 one-minute result from hash-pinned `BuffEffectManager.xml` | PASS (static/focused) |
| Demolish self-buff | Self event, 60,000 ms recast/lifetime and ignore-defense formula boundary | PASS (focused; native class gate open) |
| Mastery tree | IDs 779, 780, 781, 782, 787, 788 and 794 mirrored from `SkillTreeData_3rd.xml` | PASS (static) |
| Skill bead/item mapping | Only known mapping group 12/index 479 is enabled from `SkillRequire.xml` | PARTIAL |
| Damage formulas | S21 `RegularSkillCalc.lua` (SHA-256 `78A1502C187D9EA248E682CCAA62EA8C4510D7119192510D1A966A18E6992E4D`) mirrored as double-precision helper and damage sink | PASS (focused) |
| Class column | S21 class number 9 recorded; runtime also rejects non-Slayer/disabled-class contexts; legacy 5.2 `MAX_CLASS == 7` blocks writes | PARTIAL (ABI) |
| Class migration checklist | `CLASS_MIGRATION.md` maps server/client/config/protocol/persistence/asset boundaries | PASS (audit) |
| Packet send/receive | Existing targeted/self envelope `0x19` is used for all five rows; no dedicated skill opcode is guessed | PARTIAL |
| Client effects, icons, sounds, minimap UI | IDs/icons are recorded; native binding and proprietary assets are not copied | OPEN |
| Server authority, persistence, reconnect/rollback | No live server or database mutation performed | OPEN |
| Native build and in-game QA | Isolated `Source_PC_Slayer` client and GameServer builds pass. A saved-credential `admin4` / `Slayer` session dispatched 292/293/294/295/297, logged all five native actions and particle submissions, and stayed responsive. The actor is still legacy class 1 under an explicit QA marker; remote GameServer authority and owner acceptance remain open | PARTIAL |

The Grow Lancer package is used as a process/reference source only. No Grow
Lancer hooks, addresses, layouts, or binaries are transplanted into Slayer.
