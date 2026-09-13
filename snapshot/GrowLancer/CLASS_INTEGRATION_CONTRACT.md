# Native Grow Lancer class integration — IN_PROCESS

Owner requires a distinct native class; isolated DK testing is temporary only.
No production class/DB change is authorized by this inspection.

## Verified source identity

Protected supplied S21 file `D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua`
line21 declares `CLASS_GROWLANCER = 7`. SHA256:
`9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70`.
This proves the supplied calculation-script base class number, not all S21
network/DB encodings or evolution IDs.

## Current native boundaries

Real-tokenizer regression now runs, not only constexpr mocks:
GrowLancerMemScriptColumnsTest links the isolated GameServer MemScript.obj
and required stdafx.obj (native /GL objects, /LTCG), plus its own small main.
It opens Tests/fixtures/legacy_class_columns.txt read-only through CMemScript,
checks two quoted-name rows, // comments, next row IDs901/902, seven values,
extra zero columns, end token and EOF at capacities7/8/16. Execution exit0.
This fixture deliberately does not model every field of an entire skill/item/
quest file, and does not prove malformed-input rejection or live reload.
No GameServer main/network/database code was started by this harness.

Remaining six RequireClass token readers migrated: QuestObjective,
QuestReward, QuestWorld, QuestWorldObjective, QuestWorldReward and ItemBagEx.
For quest tables the helper remains after RequireState (RequireCooldown for
QuestWorld) and before SetInfo. ItemBagEx remains section4-only after
OptionValue and before lookup/DropInfo insertion. Other bag sections unchanged.
Ten readers total now use a fixed seven-column schema independently of array
capacity. Scope/source-order checks and focused helper compile PASS; isolated
Ex603 build PASS. No claim of real complete configuration-file loading yet.
Search for the former RequireClass[n]/GetAsNumber pattern returns no matches;
this is not proof that all class-dependent configuration consumers are covered.

Quest/master follow-up: Quest::Load reads the bounded seven class columns
after RequireMaxLevel and before SetInfo; MasterSkillTree::Load does so after
RequireSkill[1] and before map insertion. RelatedSkill, ReplaceSkill, both
RequireSkill values and MainValue parsing are unchanged. Consecutive synthetic
row tests retain following IDs901/902 at capacity8, and source-order checks
cover the actual insertion points. Ex603 x86 build PASS; no class/master
activation. Remaining matching token loops: QuestObjective, QuestReward,
QuestWorld, QuestWorldObjective, QuestWorldReward and ItemBagEx.

Item-table follow-up: ItemManager::Load now calls the same bounded generic
ReadLegacyClassColumns helper inside its existing section!=14 guard. It still
performs the separate GetToken for sections<=11 or13. No section field order,
item eligibility check or section14 behavior changed. The old skill-specific
helper remains a forwarding wrapper. Compile-time section0..15 tests verify
zero reads for14, seven for others and no implicit class7 permission; source
checks verify the real loader guard. This is not a full item-file parse/runtime
acceptance test. Ex603 isolated Win32 rebuild PASS. Quest/master readers remain
pending; no MAX_CLASS expansion was made.

Legacy skill-row migration correction: SkillManager::Load now delegates only
the class-column read to Shared/LegacySkillClassColumns.h. It consumes exactly
seven GetAsNumber calls irrespective of destination capacity, and zeroes extra
columns. Tests for capacities7/8/16 preserve the next-row sentinel and original
seven values. This closes the SkillManager grammar hazard described below;
other item/quest/master loaders are not claimed migrated. No eighth-column
auto-detection or new format is accepted. Future GL learning stays in its
separate package contract; current MAX_CLASS remains7 and activation is off.
Isolated Ex603 Win32 build PASS; source/default/rate gates remain open.

### Supplied S21 directed damage rates recovered

CalcCharacter.ini hashBE300D43... (full pin below) contains all15 directed
GL/legacy-class PvP rates in [PvPDamage]. They are decimal percentages, not
integers. In order DW,DK,ELF,MG,DL,SU,RF,GL, the GL outgoing values are
31.24703548,34.11724265,29.22544179,33.36155585,19,25.55220782,
33.87089928,22.49264061. Incoming from the first seven are14.63748459,
29.05930889,41.85446802,47.13405707,16.31879461,44.63967386,26.83767228.
[PvMDamage] GLPvMDamageRate=555; [PvPDamage] GLDamageStuckRate=10.
The same DLToGL_PvPDamageRate key in [DarkSpirit] is5.0, NOT16.31879461.
The section-aware verifier preserves these separate identities and decimals.

Native ServerInfo declares integer m_DamageRateTo and Attack.cpp applies
damage*rate/100 after general PvP and attacker-class multipliers. Therefore
copying S21 decimals through GetPrivateProfileInt would lose precision;
replacing the entire existing matrix with floats would change legacy behavior.
A scoped GL-involved adapter is a candidate, but S21 handler conversion,
rounding and application order must be verified first. Config values are now
available; authoritative runtime semantics and class defaults remain missing.
verify_s21_class_damage_rates.py checks config SHA,15rates,section collision
and the current native storage/application boundary. No rates were activated.

Migration hazard verified in native SkillManager.cpp: each text skill row reads
RequireClass via a MAX_CLASS-count GetAsNumber loop. Increasing7->8 without
migrating the token-based input schema consumes an extra token per row; it is
not merely an array allocation change. A legacy-seven-column compatibility
reader or versioned eight-column package is required before activation.
ServerInfo.cpp explicitly loads all49 m_DamageRateTo[0..6][0..6] entries,
not a MAX_CLASS-driven loop. Eight classes need15 additional directed cells
(including GL->GL); neither zero nor DK duplication is established as correct.
DefaultClassInfo::Init zeroes rows and SetInfo range-rejects class7 today;
raising capacity without a verified default row would retain zero stats.
verify_class_table_migration_boundary.py checks these concrete source hazards.
Do not widen MAX_CLASS until loader grammar, defaults and rates are supplied
and tested alongside the existing seven classes.

- Client CharacterManager.h GetBaseClass masks0x7. The existing conversion
  ChangeServerClassTypeToClientClassType extracts base from serverByte>>5,
  second evolution bit4->clientbit3, third bit3->clientbit4. Base7 fits those
  fields; capacity is not the missing implementation.
- Client GetCharacterClass only handles base0..6 and initializes its result
  to0. An unimplemented base7 silently resolves to0. IsFemale only recognizes
  Elf/Summoner. Do not activate class7 through these unmodified switches.

### Reaction sound sex classification evidence

Pinned S21 `140AD18` invokes `BB6224` using character class byte12.
Complete helper BB6224..BB629B returns true for low-nibble base classes
2,5,7,8,9,12,14; complete 960FB1..960FBB applies `class & 0xF`.
Thus class7 uses female reaction sound selector109/110 at140ADC2, not
the DK male selector105..107. There is an additional suppression when
character word322E equals1A25; field meaning is NOT established here.
Verified by `tools/grow_lancer/verify_player_reaction_sound.py` against the
SHA-pinned main dump. This is an implementation requirement for final native
Grow Lancer, not authorization to change isolated DK sex or sound globally.
Sound registration18E015B..18E01D2 maps109/110 to
`Data/Sound/pFemaleScream1.wav` and `pFemaleScream2.wav`. Source S21 and
isolated RuntimeQA files are byte-identical RIFF/WAVE assets:
SHA256 `469133E1F77907F321BF385E19E93A608D1FC88E51630C77C35AB7D1990AE701`
and `EAE7D56D04DEC5417BB98CAE3A246F801549D9CFD5928AFE89ED0DA14656AB06`.
Native ZzzOpenData.cpp4748/4749 already registers those paths at109/110.
No additional asset staging is required for these two sounds. Suppression
field semantics, playback configuration and live sound acceptance remain open.

Further identity evidence: pinned/checksummed708-byte item catalog contains
exactly key1A25, explicit group13/index37, name Horn of Fenrir and model
fr_6.bmd. Thus the scream exclusion uses a Fenrir item identity, not a
Grow Lancer buff ID. Guarded auxiliary reaction branch140AAA6..140AB45
also compares getter10C64D8 result with1A25 and selects700/701 after the
random modulo3 gate. Full auxiliary activation predicate remains separate;
do not replace it with a bare item comparison or change global DK sounds.

### Remaining native boundaries

Pinned GS/client catalog cross-check (`inspect_grow_lancer_weapon_categories.py`):
53 records in groups0..6 have GS GrowLancer>0 and matching client KindB251.
Counts (group/category):0/0=1,0/1=3,1/4=2,3/7=23,6/15=24.
None uses auxiliary reaction categories89/90/114. Therefore those special
book/orb/elixir branches are not selected by these eligible catalog items.
Do not reduce Grow Lancer equipment to spear/shield only: Kris, Short Sword,
Rapier and two axes are also permitted by this supplied table. This does not
prove equipped hand assignment, dual-wield permission or cast requirements.
When normal hand-presence reaction path is selected, source chooses338 for
both hands,336 for hand0 only,337 for hand1 only; unarmed path selects335.
The preceding state predicate and actual equipped item IDs remain required.

Mounted selector state must be a stored character property, not a fresh terrain
query: S21 byte4 has a terrain-bit1 writer, but9F71B9..71CB copies it from an
owner character into an embedded character,12F1352 assigns true explicitly,
and12522E6..2323 /1253AC1..3AF6 derive it from other context fields. The exact
meaning of those latter contexts remains open. Preserve native SafeZone state
ownership; do not overwrite it globally from a Grow Lancer reaction helper.

- GS DefaultClassInfo.h MAX_CLASS=7 and array size7 exclude index7. Its
  class constants stop atRF6; DB class constants are a distinct namespace.
- GS DSProtocol.cpp lines593/878 uses `(Class/16)*32` for class packing;
  line1848 persists DBClass. Viewport.cpp line1034 uses `(Class<<5)&0xE0`.
  These are native-format evidence, not proof that S21 DB class is the same.
- Character class identity, default stats, formula dispatch, equipment masks,
  model/UI selection, create/list packets and persistence must be integrated
  together. Merely raisingMAX_CLASS or selecting unused enum7 is insufficient.

## Next bounded implementation prerequisite

### Default-stat evidence and namespace separation

Supplied CalcCharacter.ini explicitly points LevelLife, LevelMana,
VitalityToLife and EnergyToMana to MuOnline.dbo.DefaultClassType. No values
are inferred from that comment. File SHA256:
BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D.
Native DefaultClassInfo.cpp loads base attributes, HP/MP and those multipliers
from Character/DefaultClassInfo.txt; a new row requires actual evidence.

StatSpecialize.xml uses ID8 for Grow Lancer, unlike Lua base7. Its two
option rows are (StatType1,Index1) and(StatType2,Index4). Do not treat this
one-based table index as the native class enum. SHA256:
D79F1CBBE8A6FD7207499CC5198E2F0E99A5BE06E2607C0A8B4879A3378E9AC9.
CalcCharacter.lua GrowLancerDamageCalc defines both hands min=STR/8+DEX/10,
max=STR/4+DEX/6. These are source expressions only; execution, numeric
conversion/rounding and server dispatch remain separate evidence gates.
verify_class_source_profiles.py verifies all three source hashes, namespaces
and expressions. It does not claim unavailable starting stats or HP/MP.

### Indexed consumer audit

### Formula precision and native application boundary

ObjectManager.cpp native stat calculation forms int Strength/Dexterity/etc
from base plus added attributes; ServerInfo division constants are int.
PhysiDamageMinLeft in User.h is __int64, but the wider destination does not
undo integer division performed before assignment. Copying S21 expressions
with unmodified int operands is therefore not a faithful numeric mapping.
For STR7/DEX9, raw minimum7/8+9/10 is71/40=1.775, while native integer
terms sum to0; even truncating only after addition yields1, a different result.
S21 GS bridge rounding/clamping remains unknown; no production formula changed.

Verified Grow Lancer source branches in pinned CalcCharacter.lua:
CalcAttackSpeed returns DEX/20 for both speeds; CalcAttackSuccessRate_PvM
uses(NormalLevel+MasterLevel)*5+DEX*1.25+STR/4;
CalcDefenseSuccessRate_PvM usesDEX/4; CalcDefense usesDEX/7.
The source comment restricts older-version attack-speed applicability, so
do not automatically equate its anti-hack calculation to client animation speed.
verify_class_source_profiles.py checks selected branch expressions plus the
rational counterexample; no Lua interpreter was available via lua/luajit/lupa
in the inspected environment. It does not execute Lua or claim server parity.

Three independent headers define MAX_CLASS7:DefaultClassInfo.h,
CustomStartItem.h andCustomStartItemDame.h. Updating one alone is insufficient.
Indexed consumers include item requirements inItemManager/ItemBagEx,
MasterSkillTree, Quest/QuestObjective/QuestReward and worldquest counterparts.
ServerInfo contains level-up points, HP/MP/BP/SD recovery, reset rates and
two-dimensional PvP/elemental damage matrices. Each loader must supply a
verified class7 value; zero initialization is not an acceptable substitute
for missing balance or learning rules. No global macro changed in this audit.

### Additional pinned source formulas (not server execution)

The same CalcCharacter.lua hash verifies eight more Grow Lancer branches.
Here L means NormalLevel + MasterLevel, verified within each relevant function.

| Source function | Grow Lancer expression |
| --- | --- |
| CalcAttackSuccessRate_PvP | DEX * 2.5 + 3 * L |
| CalcDefenseSuccessRate_PvP | DEX / 5 + 2 * L |
| ElementalDamageCalc | min = itemMin + DEX / 5; max = itemMax + DEX / 3 |
| ElementalAttackRateCalc_MvP | 3 * DEX / 2 + 5 * L + STR / 4 |
| ElementalAttackRateCalc_PvP | 3 * DEX / 2 + 5 * L + STR / 4 |
| ElementalDefenseCalc | STR / 9 + DEX / 9 |
| ElementalDefenseRateCalc | DEX / 3 |
| CalcAbilityGauge | STR * .15 + DEX * .2 + VIT * .3 + ENE * 1.0 |

`MvP` is the source function spelling, not a corrected alias. AG above is
the class gauge calculation, NOT a skill-specific consumption or recovery rule.
Elemental calculations are not substitutes for physical skill damage. The
verifier now checks 12 selected branches / 14 exact assignment lines, plus
four physical-damage assignments. Numeric conversion, call-site application,
caps, equipment modifiers and actual server dispatch remain OPEN. No class
activation, guessed defaults or production configuration was changed.

### Load/create closure evidence

Native master dispatch is one-hop: CSkill::Set stores m_index unchanged and
uses GetMasterSkillRelated(index) for m_skill, falling back to index if zero.
That lookup returns a configured RelatedSkill directly; it does not recursively
resolve S21 brand chains. MasterSkillTree::Load reads RelatedSkill separately
from ReplaceSkill and RequireSkill[2]. Do not copy S21 Brand into RelatedSkill
blindly (e.g.690->687->271); the ultimate native dispatch and replacement/tree
requirements are separate mappings. The current271..281 guards do not claim
to reject every S21 master ID. Master activation is still unimplemented.

Weapon-bonus boundary: CalcTwoSameWeaponBonus and CalcTwoDifferentWeaponBonus
have comments limiting intended classes (neither lists Grow Lancer), but their
bodies do NOT test the Class argument. They multiply all four incoming values
by 65/100 and 55/100 respectively, returning minLeft, minRight, maxLeft,
maxRight. The Class parameter alone does not enforce an exclusion. Do not
apply these bonuses to Grow Lancer or claim the comments prove actual dispatch;
the S21 server caller/bridge must establish eligibility and tuple mapping.

Native comparison (ObjectManager.cpp 2970..2999, 3355..3422): weapon base
damage is added before item/set/master/custom option application. Afterwards,
dual-weapon scaling is explicitly restricted to DK/MG/DL/RF, both valid items,
both indices in groups0..3. RF receives 60% minimum /65% maximum; the other
three receive55% on all four damage values. This block does not distinguish
equal from different weapon indices. Thus copying DK into the GL test class
can bring native DK dual-weapon behavior which is NOT proof of S21 GL behavior.
Do not broaden this guard when adding the new class without S21 caller evidence.
The subsequent HP/MP calculations index default-class rows, then add level,
eligible master level and vitality/energy contributions before BP/SD calculation.
Moving the class formula after option/weapon scaling would change that ordering.
These observations do not authorize modifications to existing SS6 classes.

CharacterCalcBP (2705..2732) has only seven explicit cases and no default.
Native DK/RF coefficients match the S21 GL AG expression, but native applies
an explicit int cast to the full sum of base+added attributes. Matching
coefficients do not establish S21 bridge conversion. An unsupported class
does not calculate MaxBP here at all; increasing an array capacity alone
would leave that field unchanged. This is another activation gate, not
permission to reuse DK identity or invent an eighth-case numeric policy.

ObjectManager.cpp load path rejects every DBClass outside its explicit
18-value classic whitelist before zeroing/initializing the object. It then
setsDBClass=receivedClass, baseClass=receivedClass/16, ChangeUp=receivedClass%16
(lines3549..3551). Protocol.cpp CGCharacterCreateRecv rejects everything
outside the seven base DB constants and separately checks unlock bits for
Summoner/MG/DL/RF. Grow Lancer has no authorized unlock mapping yet.

DataServerProtocol.cpp GDCharacterCreateRecv passes receivedClass directly
to WZ_CreateCharacter(account,name,class), reads its result, and only on
result1 writes the selected AccountCharacter GameID slot. The stored procedure
definition was not found among this worktree's SQL files. Actual database
definition/defaults/transaction behavior are NOT verified. No database access
or modification was performed. Preserve existing account slots and errors.

verify_native_class_encoding.py checks current source expressions then24
algebra cases:8 base values x3 evolution inputs. Under the EXISTING native
formula only, unevolved base7 derives DB112 and wire224; client decodesbase7.
This is a derived native representation, not guessed S21 DB identity. It does
not prove Grow Lancer uses all three tested evolution states; those are bit
capacity checks only. Creation/loader whitelists stay unchanged until complete
class defaults, equipment, unlock, UI and persistence handling are ready.

Trace the existing DS load/create assignments from DBClass to base/evolution,
then inventory the array/switch consumers ofMAX_CLASS. Verify supplied S21
Grow Lancer defaults/evolutions independently. Keep recipient activation and
server cast authority fail-closed until that contract is complete. Do not
rename this work as permanent DK skill support or widen skill-slot counts.
