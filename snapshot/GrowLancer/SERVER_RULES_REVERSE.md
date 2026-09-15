# Grow Lancer S21 server-contract evidence

Status: `IN_PROCESS`. This file records only contracts present in the accepted
S21 server tables under `D:\GameServer S21\Data\Skills` and facts verified in
the RISE 5.2 server source. It is not evidence that RISE currently implements
the Grow Lancer class or these skills.

2026-09-15 isolated local transport smoke: the current-user 32-bit ODBC DSN
connected specifically to `GrowLancer_QA`; the verified private CS/DS/JS/GS
test stack launched four hidden binaries inside the Grow Lancer worktree and
each owned its configured listener (44412/55964/55974/55910). CS and GS
accepted loopback TCP; the four checked task PIDs were stopped after smoke
to avoid idle load. This proves local startup/transport only. It does **not**
prove account login, class112 creation/persistence, learning/cast/damage,
Magic internal281 hit, or a matching S21 GameServer handler. The native RISE
`RunningSkill` still fail-closes IDs271..281 before state mutation.

Fresh supplied-S21 evidence scan (2026-09-15): the protected GS hash/RSDS
remains the same and no matching PDB, runtime dump or handler source has
appeared. `SkillAOETargetting.xml` has no 271..279 rows, so it cannot supply
their exact target/hit selection by absence; skill Lua/XML rows are formula
and catalog evidence only. `CalcCharacter.ini` explicitly defers additional
initial class defaults to the unsupplied S21
`MuOnline.dbo.DefaultClassType` table. Do not use the separate private QA
class112 seed or a newer-source RF preset as S21 class/handler authority.

2026-09-15 static server-binary check: the supplied read-only
`D:\GameServer S21\GameServer\IGC.GameServer2_R.exe` hashes to SHA-256
`134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74`.
Its PE is x64, with `.winlice` and `.boot` sections; exact ASCII strings
`CircleShieldDecreaseAG`, `CircleShieldTime`, `CircleShieldRate` and
`SkillSettings.ini` are absent from the on-disk image. The ini values below
are source configuration evidence, not executable handler or probability
formula evidence. No Circle Shield GS rule is activated from these values
alone; a verified runtime handler dump or equivalent source remains required.

## Item/scroll identity recovered from the native client table

`D:\MU FICA Season 21\Data\Local\item.bmd` has SHA-256
`9B9D62B7F3BA085D31B4AEFBCC9DE5DB505C48D45C336A18CAD424C93D3B02F6`.
Its unencrypted header declares 4,950 fixed records of 708 bytes followed by a
four-byte checksum. Applying the client-native repeating Bux XOR
`FC CF AB` independently to each record proves these identities:

- record 2891: group 12, index 271, `Spin Step Scroll`;
- record 2892: group 12, index 272, `Obsidian Scroll`;
- record 2893: group 12, index 273, `Magic Pin Scroll`;
- record 2894: group 12, index 274, `Harsh Strike Scroll`;
- record 2895: group 12, index 275, `Shining Peak Scroll`;
- record 2896: group 12, index 276, `Wrath Scroll`;
- record 2897: group 12, index 277, `Breche Scroll`.

The names correlate to base skill IDs 271, 273, 274 and 276..279; the item
index is not assumed to equal the skill ID. No decoded item-name record names
Circle Shield or Clash, so those mappings remain explicitly absent rather
than being filled from adjacent numbers.

The independent server `D:\GameServer S21\Data\Items\ItemList.xml` is pinned
to SHA-256
`61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78` and
corroborates the seven exact names, group-12 indices, GrowLancer class gate,
stat gates, drop levels and prices. The read-only check is
`tools/grow_lancer/verify_s21_itemlist_scroll_mapping.py`; it does not turn
the table's absent Circle Shield/Clash rows into guessed learn paths.

## Pinned inputs

| File | SHA-256 |
|---|---|
| `SkillList.xml` | `3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0` |
| `RegularSkillCalc.lua` | `F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847` |
| `MasterSkillCalc_3rd.lua` | `771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699` |
| `SkillSettings.ini` | `7CF1FF432D337EF3420B8037C2D26F95D768E317A0DFC6CA7E46C4EAF2F847C9` |
| `SkillAOETargetting.xml` | `CE5C501CB82D2179DB5ED5407DC89BEA4CB118446BA444D8FF95D7DE96F984D9` |
| `Skill_UseArea.xml` | `7C6B0C57A33F2E288AF106D8E19D43D9093279D01CE9DFB9C0E03EB62621BFC2` |
| `SkillRequire.xml` | `4E6B765FF7DCFB62E58AD9720F5E8C8E81DC8ECC5BA30C3BC3B842B390C9AC58` |
| `SkillElect.hit` | `809B8D5724DB063EB497C09722B894E1BEB6A66D7103FD075B9331858455BE10` |
| `SkillTreeData_3rd.xml` | `CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0` |

## Base rows

Every row has only `GrowLancer="1"`; all other class flags are zero.
`ItemSkill="0"` is common to all nine rows. That field alone does not identify
an item group/index and therefore is not accepted as scroll mapping evidence.

| ID | Name | Level | Damage | Mana | BP/AG | Range | Delay ms | Requirement | Buff |
|---:|---|---:|---:|---:|---:|---:|---:|---|---:|
| 271 | Spin Step | 83 | 100 | 12 | 0 | 2 | 500 | Dexterity 150 | 0 |
| 272 | Circle Shield | 220 | 0 | 100 | 50 | 0 | 0 | none in row | 216 |
| 273 | Obsidian | 74 | 0 | 50 | 50 | 0 | 0 | Energy 200 | 217 |
| 274 | Magic Pin | 30 | 80 | 5 | 3 | 2 | 500 | Strength 200, Dexterity 200 | 0 |
| 275 | Clash | 0 | 50 | 50 | 50 | 6 | 10000 | `Brand=75` | 0 |
| 276 | Harsh Strike | 74 | 100 | 12 | 0 | 3 | 500 | Strength 150 | 0 |
| 277 | Shining Peak | 92 | 50 | 8 | 5 | 4 | 0 | Strength 600 | 0 |
| 278 | Wrath | 66 | 0 | 40 | 30 | 0 | 0 | Strength 200, Dexterity 200 | 424 |
| 279 | Breche | 300 | 230 | 15 | 6 | 5 | 0 | Strength 300, Dexterity 300 | 0 |

`Brand=75` on Clash is preserved as raw table evidence. Its meaning is not
relabeled as an item, prerequisite or siege flag without loader/runtime proof.

## Proven regular formulas

- Spin Step primary: `(InDamage * 1.1) * (Dexterity / 10 + 97 + SkillTreeBonus) / 100`.
- Spin Step Explosion: `(InDamage * 0.7) * (Dexterity / 10 + 97 + SkillTreeBonus) / 100`.
- Harsh Strike hit 1 multiplier `1.0`; hit 2 multiplier `1.1`, using Strength.
- Magic Pin hit multipliers are `0.8`, `1.0`, `1.1`, using Dexterity.
- Breche multiplier is `0.5`, using Strength.
- Shining Peak combines Strength and Dexterity branches, multiplies by `0.8`,
  then divides the result by three. This proves a three-part damage formula but
  does not by itself prove packet timing or target selection.
- Obsidian returns effect value `Strength / 20` and duration `240` seconds.

2026-09-13 revalidation: verify_wrath_server_formula.py still pins Character
formulas44/46/47 and skill278/895 -> buff424/425, but not their handler semantics.
It now also pins BuffEffectManager.xml SHA256
201452C00D9D5CE0DB83820EF48EC4317A6654864C0BE9176A6389694A4AD6B0:
424/425 have qualitative attack/combat/defense descriptions. Legacy218/223/224
share EffectType123 yet describe50% damage/30% defense; those text values are
NOT accepted as formulas for424/425. The file contains unrelated malformed XML
at line414; the verifier parses only exact selected single-line records without
changing source. No matching PDB/map/dmp was found in the supplied GS tree during
this recheck. Server handler/duration evidence remains required for activation.

No regular formula for Circle Shield, Clash or Wrath is present in
`RegularSkillCalc.lua`. Their behavior must come from server handler/runtime
evidence rather than invented arithmetic.

## Internal and master mappings

- Skill 280 is `Explosion`, `Damage=50`, `Distance=2`, `Type=1`, `UseType=6`,
  class Grow Lancer. Client receive evidence ties it to Spin Step hit visuals.
- Skill 281 is `Magic Pin Explosion`, zero-cost internal skill with `Type=1`,
  `UseType=6`, class Grow Lancer. Client receive evidence ties it to Magic Pin
  contact visuals.
- The active third-master tree has 13 Grow Lancer rows. Exact `Brand`, tree
  parent, point gates, resource costs, buff IDs and formula coverage are pinned
  in `MASTER_SKILL_REVERSE.md` and the shared declarative contract.
- Shining Peak row 699 is deliberately not simplified into an inferred chain:
  `SkillList.xml` says `Brand=277`, while the active tree says
  `ParentSkill1=698`.
- Wrath rows 708..710 remain in `SkillList.xml`, but are absent from the active
  Grow Lancer tree; active row 895 brands to base 278. The legacy rows are not
  enabled.

All master variants remain outside runtime dispatch until their handler
semantics and recipient-class ABI are separately proven.

`SkillRequire.xml` contains enhancement requirements for 2036 and 2085, not a
base-row item mapping for IDs 271..279. Its commented 742 line is not executed
configuration.

## Global settings and exclusions

`SkillSettings.ini` proves the Circle Shield settings:

- `CircleShieldDecreaseAG = 10`
- `CircleShieldTime = 30`
- `CircleShieldRate = 20`

The file does not establish the exact probability equation or target validation
path; those remain open pending handler evidence.

None of IDs 271..279 is listed in `SkillAOETargetting.xml` or
`Skill_UseArea.xml`. Absence means those optional systems do not configure these
base skills; it does not prove single-target behavior or unrestricted usage.

`SkillElect.hit` is exactly 15,876 bytes. RISE's existing `CSkillHitBox` loader
defines the same size as `BYTE m_table[36][441]` and indexes it by direction and
relative 21x21 coordinates. It is a generic directional hit mask, not a
per-skill hit-count/damage table.

## RISE 5.2 compatibility facts

- `MAX_SKILL` is 629, so base IDs 271..279 fit the server lookup/delay arrays.
- `MAX_SKILL_LIST` is 60 and persistence stores 60 three-byte entries. This is
  character-owned skill capacity and must not be raised merely to accept these IDs.
- Attack, multi-attack and duration-attack receive packets store the skill in
  two bytes and decode it with `MAKE_NUMBERW`. Attack/duration send packets
  serialize high and low bytes explicitly; persisted/list entries store a
  `WORD skill`. IDs 271..281 therefore fit the existing protocol width without
  widening packet fields.
- The enum jumps from Phoenix Shot 270 to Blood Storm 344; the nine Grow Lancer
  IDs are unassigned and absent from `CSkillManager::SkillAttack` dispatch.
- The server defines only seven classes (`MAX_CLASS=7`, DW through RF). There is
  no Grow Lancer class slot, default-class record, item requirement column or
  native skeleton/equipment contract.

Therefore the base IDs and generic server handlers can be added in isolation,
but a truthful `Grow Lancer class validation` acceptance result cannot be
claimed until a recipient-class compatibility contract or a full class port is
explicitly selected and implemented.

## Local 5.2/newer-source comparison

The read-only tree `D:\MU-CrossPlatform\SRCMainGS\Source\GameServer` contains
a conditional newer class shell (`GAMESERVER_UPDATE>=601`) with `CLASS_GL=7`
and DB classes 112/114/115, plus generic stat and damage-rate branches. Its
`CSkillManager` source and matching local `GameServer.pdb` contain no named
Spin Step, Circle Shield, Obsidian, Magic Pin, Clash, Harsh Strike, Shining
Peak, Wrath or Breche handler. Numeric `case 275` there belongs to a
monster-class summon switch, not Clash. This source proves part of the missing
class boundary, but it is not accepted as server-skill logic.

The sibling `MuServer52\Data\Skill\Skill.txt` contains rows 271..279, but
their costs/damage differ materially from the pinned S21 `SkillList.xml` rows
and all class columns are zero. Those rows are excluded from S21 gameplay
semantics and are not copied into this package.

## S21 client packet edge

`CLIENT_PACKET_REVERSE.md` statically closes the client-side local builders:
six skills use `C1:35`, Shining Peak/Breche use extended positional `C1:57`,
and Clash uses movement packet `C1:59`. RISE 5.2 cannot reuse these numbers:
its `0x57` is already guild-create cancellation, while its native skill entry
points are `0x19`, `0x1E` and `F3:DB`. Translation remains fail-closed pending
the missing S21 server handlers.

## Still open

## Direct S21 GameServer tree revalidation — 2026-09-15

The user-supplied `D:\GameServer S21` tree was checked read-only. Its
`Data\Skills\SkillList.xml`, `SkillRequire.xml`, `SkillSettings.ini`,
`SkillTreeData_3rd.xml`, `Data\Skills\MasterSkillCalc_5th.lua`,
`Data\Items\ItemList.xml` and `Data\Scripts\Character\CalcCharacter.lua`
are the same pinned tables already used above (the hashes for the pinned
`SkillList`, `RegularSkillCalc`, `MasterSkillCalc_3rd`, `SkillSettings`,
`SkillTreeData_3rd` and `ItemList.xml` match exactly). The base rows 271..279,
the seven group-12
scroll records (item indices 271..277), Circle Shield settings, active
Grow-Lancer third-master rows and the Wind Soul 5th-master formula were
re-read from this tree. `SkillRequire.xml` still has no base learn rows for
271..279; its active Grow-Lancer rows are enhancement requirements only.

`D:\GameServer S21\GameServer` contains `IGC.GameServer2_R.exe` and
configuration files. The protected executable currently hashes to
`134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74`; it has
no accompanying source, PDB, MAP, C/C++ project or handler dump.
The bounded read-only name search found no additional named handler or class
mapping beyond the XML/Lua contracts above. The executable is not patched or
decompiled into a guessed SS6 handler, so live server-authoritative learn,
cast, damage, target-selection and class ABI evidence remain open.

- Another authoritative learn path for Circle Shield 272 and Clash 275; the
  seven scroll group/index mappings above are now independently corroborated.
- Native 5.2 packet translation and rollback/error response.
- Spin Step target selection and internal Explosion timing.
- Harsh Strike two-hit timing/selection.
- Magic Pin three-hit timing/selection and internal Explosion dispatch.
- Shining Peak line/target selection.
- Circle Shield PvP-only validation, chance equation and AG-decrease application.
- Clash siege/PvP validation, collision/push distance and rollback.
- Wrath effect value, defense cost, duration and refresh/replacement rules.
- Breche target selection and hit count.
