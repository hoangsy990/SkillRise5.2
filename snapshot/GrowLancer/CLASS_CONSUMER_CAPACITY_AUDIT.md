# Grow Lancer class-consumer capacity audit (SS6, reference only)

Status: `REFERENCE_ONLY` / `IN_PROCESS`

Current-worktree note (2026-09-15): GS metadata arrays reserve class index7
and the client has a private unevolved identity selector. The inventory below
describes remaining SS6 client/UI seven-column tables and class create/load,
rate/default/equipment/skill consumers that are not activated. A capacity
reservation is not full class ABI or gameplay acceptance.

Fresh S21 boundary check: `D:\GameServer S21\Data\CalcCharacter.ini` line13
says additional character defaults come from
`MuOnline.dbo.DefaultClassType`, not from its GL rate keys. The supplied S21
server tree has no original SQL row/dump/PDB; the `GrowLancer_QA` class112
default row is a local fixture, not verified S21 initial stat authority.
The current 32-bit `MuOnline` DSN targets `VIE`, so this task did not query
it or touch external/production data. Native unevolved class7/DB112 still
needs an original S21 default row, create/login/persistence and skill
authority. Owner excluded equipment and quests1–3 from this skill task;
that does not authorize a guessed base-class/default substitution.

This document records the SS6 class consumers that must be audited before a
Grow Lancer class can be activated. It is intentionally separate from the
skill renderer. The historical baseline audit did not itself change
`MAX_CLASS`, packet widths, class encoding, or existing class tables; the
later isolated GS capacity reservation is documented above.

## Current SS6 boundary

- Client `ExMain_RISE_PC/Main5.2_RISE/_define.h`: `MAX_CLASS == 7`.
- Server `ExGameServer/GameServer/DefaultClassInfo.h`: `MAX_CLASS == 7`.
- The SS6 source has no `CLASS_GL`, `DB_CLASS_GL`, `DB_CLASS_ML`, or
  `DB_CLASS_GL_4` symbols. The candidate wire/database values from the newer
  reference (`source class 7`, `DB 112..115`) therefore remain disabled.
- Client class encoding is still the native 3-bit base class plus second/third
  class bits in `CharacterManager.cpp`; server decode remains `DBClass / 16`
  and `ChangeUp = DBClass % 16` in `ObjectManager.cpp`.

## Consumer inventory

| Surface | SS6 consumer and evidence | Impact if Grow Lancer is activated |
|---|---|---|
| Client class selection | `CharMakeWin.cpp` uses `MAX_CLASS` loops, a seven-entry `_btn_classname` table, and `apszStat[MAX_CLASS][4]`. | Add a class entry, icon/text, stats, button, and creation path without overrunning fixed arrays. |
| Client body/model registration | `ZzzOpenData.cpp` registers first/second/third-class body models using `MAX_CLASS` offsets. | Add GL model slots and registration only after model IDs/skin mapping are proven. |
| Client skin index | `CharacterManager.cpp::GetSkinModelIndex` multiplies class stages by `MAX_CLASS`. | Extend the mapping locally; do not change global arithmetic until the full body table exists. |
| Client helper/UI arrays | `NewUIMuHelper.h` has four `class_character[MAX_CLASS]` arrays. | UI/helper class columns need a new capacity and packet contract. |
| Client item requirements | `_struct.h` and inventory code store `RequireClass[MAX_CLASS]`; `NewUIMyInventory.cpp` indexes by base class. | Item class columns and UI checks need coordinated migration. |
| Server default stats | `DefaultClassInfo.h` stores `m_DefaultClassInfo[MAX_CLASS]`; `ObjectManager.cpp` indexes it for life/mana/stat recomputation. | GL defaults and all stat formulas must be supplied before activation. |
| Server skill/master requirements | `SkillManager.h` and `MasterSkillTree.h` store `RequireClass[MAX_CLASS]`; `SkillManager.cpp` serializes exactly seven columns. | Skill learn/validation and master-tree packet/data widths must be extended together. |
| Server item/drop requirements | `ItemManager.h` and `ItemBagEx.h` use `RequireClass[MAX_CLASS]`. | ItemList/drop parsing and class checks must agree on the new column. |
| Server class-rate matrices | `ServerInfo.h` contains per-class and `[MAX_CLASS][MAX_CLASS]` PvP/elemental tables plus recovery/bonus arrays. | GL PvP/PvM/recovery rates and directed matrices require explicit data. |
| Server create/login/encoding | `Protocol.cpp` accepts only the seven legacy DB classes; `ObjectManager.cpp` decodes `DBClass`, `Class`, and `ChangeUp`; `User.h` stores all three. | Character creation, login, persistence, and packet compatibility are a single ABI gate. |
| Server class branches | `ObjectManager.cpp` and `SkillManager.cpp` have explicit branches for DW/DK/FE/MG/DL/SU/RF. | Every branch affecting stats, attack, defense, speed, reactions, and skill authority needs a GL rule. |

## Safe next action

Use `tools/grow_lancer/verify_class_consumer_capacity.py` as a read-only
anchor check. Porting the class remains blocked until the native create/login,
DB/wire encoding, body/model tables, class-rate data, and authoritative skill
validation are all evidenced. This is not permission to raise `MAX_CLASS`.
