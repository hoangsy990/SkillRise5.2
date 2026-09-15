# Breche (skill 279) reverse evidence

Status: `IN_PROCESS`. IMPORTANT CORRECTION: the previously claimed
action-specific body-equipment chain below is RETRACTED. It is an item-index
switch, not an action switch. Historical notes are retained for audit only.

## QA caster-action correction (2026-09-14)

The isolated F7 fixture previously wrote `CurrentAction=289` and reset both
animation fields directly. That did not mirror the recovered S21 call through
the native action setter. `CreateBrecheAction` now uses `SetAction(289, true)`
so the appended S21 clip receives the outgoing action/frame transition. The
QA-only cast path also mirrors the receive-side source state (`Skill=279`,
selected target, success flag, attack flag, target coordinates and
`AttackTime=1`). This is a lifecycle correction, not evidence of a new
caster-owned effect root. The verified S21 base local branch still has action
190 + sound 1119, while the 5FD root and five children remain owned by the
resolved secondary actor in the receive path.

## Owner-side producer pinned from S21 per-character update (2026-09-15)

The fresh read-only S21 cast capture is
`artifacts/runtime_qa/s21_reference/breche_owner_recast_20260915_b`.
Frame `frame_00042.jpg` is the decisive owner/target separation: the
Grow Lancer has a large red/orange ground vortex centred under the caster,
while each pig has an independent upright white/orange fire column. These
are not one root copied to both actors.

Static decompilation now pins the owner-side call site. In the per-character
update/render routine `0x1424B34`, the normalized skill switch compares
`0x117` (base skill `279`, Breche) at `0x14259F9..0x1425A03` and branches to
`0x142B21D`. That branch calls `0x143E57C` with runtime type `0x5FD`,
subtype `0`, scale `10.0f` (float at `0x1B4E4D0`), the current character's
position (`object+0x158/+0x164/+0xEC`) and the current character as owner.
This is distinct from receive `0x12CB7F5..0x12CB854`, which creates the same
type with subtype `1` on the packet-resolved secondary actor and then emits
the five child layers.

The pinned full-dump cache and subtype-0 constructor still show the owner
`0x5FD` record as a lifecycle/control object with zero bones, meshes and
actions and no child allocation. Therefore this call site proves the owner
control producer, but it does not identify the visible vortex's draw producer
or asset. The former `bursta*.bmd` lead is explicitly excluded: its `0x5ED`
chain belongs normalized skill `700` (Burst), not Breche `279`; no such asset
is staged for Breche.

That initial checkpoint is superseded by the 2026-09-15 owner-handler closure
below: the owner draw producer is now pinned to `0x1535478` and its exact
owner-only layer calls are recorded there.  The safety boundary remains: do
not attach target children to the caster and do not synthesize a root BMD or
choose `Fire_Blood`/`Damage_ring_2` by appearance alone.

## Attribution correction (authoritative, 2026-09-12)

### Actual local/receive branch recheck

#### Receive owner and constructor follow-up

Root model follow-up: primary subtype1 exit1538599 jumps1574472.
Runtime cache closure from owner-authorized full dump (2026-09-13):
artifacts/grow_lancer/full_dump/s21-21072-a2dc6fe4f8124ea1a22827e774d6bb06.dmp,
1395943076 bytes, SHA256
374d1ecb06719389416938e0be27a32596e91e0a5b5d5a7c6f8df690c326efed.
Memory64 stream has4511 regions. Singleton1E04C8C ->manager26D1B600,
map head27A5F290,count4967. Bounded tree lookup reaches key1533 at3B0E3FD0,
value/model3B3492E0. Signed counts at+22/+24/+26 are0 bones/0 meshes/0 actions.
Reproduce offline: python -B tools/grow_lancer/inspect_breche_full_dump.py.
This directly supports empty control-model root5FD in the captured runtime,
consistent with item-config absence and zero-model fallback. Do not stage a
root BMD or render invented root geometry. Native adapter must retain root
ownership/lifetime and all five constructor children. Snapshot is not a cast
trace: it does NOT prove child timing, contact, cleanup or visual parity.
Next implementation: bounded native root lifecycle/child emission, using
BRECHE_CHILDREN_REVERSE.md for exact child contracts. No more root asset hunt.

Pinned on-disk config audit (2026-09-13): Data/Local/item.bmd has SHA256
9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6,
size3504608, count4950 and exactly8+4950*708 bytes. The previously recovered
97904E checksum (seedE2F1) passes on the encoded payload; record-local XOR
FC CF AB produces4950 distinct signed DWORD keys, min0/max10759, none negative.
Thus key5FD-AE9=-4EC (-1260) is absent from THIS pinned file. This removes
the negative-key uncertainty for the on-disk item table, not for all live
cache/config writes. Dump config count1FAEB04=1356h also equals4950, supporting
the association but not proving heap contents or the obfuscated filename call.
Reproduce: python -B tools/grow_lancer/verify_breche_model_config.py.
Scope: read-only config provenance; no asset staging or visual acceptance.
Next: inspect model cache population/registration paths for an existing5FD
entry. Do not load adjacent crasha models or synthesize a root mesh.

Configuration producer follow-up: D0506D reads a 32-bit count, then
count*0x2C4 payload bytes and a trailing 32-bit check value. Each record
passes97900E (repeating 3-byte XOR key at1DD7448), is copied into a newly
allocated record, and enters D07635 atD0537B. D07635 constructs key/value
via D087C4 ->D07983; the key is the first decoded DWORD, not a zero-extended
WORD. Thus key nonnegativity cannot be assumed from the field width.
The sole raw direct-call lead for D0506D is0E0E512E in a trampoline region;
its filename argument is not yet resolved. No file decoder was run blindly.

Global config manager returned by D04FE0 is1FAEB00. Its map head pointer
in this dump is26CC53A0, outside the main-image coverage; full map contents
cannot be reconstructed from that pointer alone. The current evidence
therefore supports the loader/insertion contract, not proof of absent key-4EC.
Next: resolve trampoline/filename or locate an existing captured configuration
payload; inspect decoded record keys and root model cache provenance.

Model-manager fallback closure: complete EF2A81..EF2BFE first searches
its cache. On miss, D06184 looks up a configuration using runtimeId-AE9.
If absent, IDs in[0,412F] allocate a D0-byte model, construct it, explicitly
zero all D0 bytes atEF2B51..EF2B60, insert it under the requested ID and
return it. If configuration exists, D05887 loads it using configuration
strings at+8/+10C; successful result is cached. Existing entries are simply
returned atEF2BE7. Thus a nonnull model can be an empty control container.
For5FD the configuration lookup key is-4EC. D06184 is a map lookup, not
a signed-range guard: absence of that negative key must still be established
before asserting that5FD takes the empty branch. No root BMD is assumed.

PlayAnimation132D0CD initializes success=true. Zero NumActions(+26)
returns at132D13C without advancing the frames. BMD::Animation132D35E
also exits when action count<=0 (earlier pose audit). This explains how an
empty model could participate in generic update without animation; it does
not by itself prove current5FD cache contents or mesh-output absence.
Next evidence is configuration insertion/key domain and cache population,
not another filename guess. The available process23824 report lists module
dumps; a heap snapshot has not been established by this inspection.

Shared wrapper176D621..176D65C first calls1886382 with the first four
arguments; only a true result calls1887B8B with all six. Thus the default
route alone proves an attempted model path, NOT visible mesh output.
1886382 rejects Alpha<0.01 at1886403 and a missing model after EF2A81
at1886424..188642F. It corresponds structurally to native Calc_RenderObject
then Draw_RenderObject, but its remaining branches still need comparison.

Raw immediate5FD leads were checked: F41CA1 is a text lookup through
9609DC,1869F6E is a C++ cleanup-state local, and A1B37D participates in a
function registration sequence throughA1DCE6. None is model provenance.
The extracted registration neighborhood has crasha01=5FB and crasha02=5FC;
this does NOT justify assigning either mesh to5FD. Investigate model manager
cache/lazy resolution for this root, including a possible empty control model,
without asserting absence merely from registry extraction.

Type5FD is not among the skip types in15745E0..15747A0, and falls within
the model range[190,7A6) checked at15747A7. It resolves the model, copies
CurrentAction and calls132D0CD at1574861 with current/prior frames,
prior action, velocity(+BC), position and angle unless action isFF.
On false return it stores model action key count into current frame and
sets CurrentAction=FF. Native ZzzEffect.cpp generic BMD::PlayAnimation
block has the same named-field structure. Model registration and initial
velocity/action still require closure; no animation speed was guessed.

Renderer15A0C98 routes5FD through bounded table15A11A4 (base4EE,
limit190), selector byte51 at15BD910+(5FD-4EE), target15BD5DC in15BD840.
The default branch range[40F,7A6) includes5FD and calls176D621 at15BD60A
with object,0,0,0,-1,-1. This is an attempted generic model pass, not proof
of visible geometry. The full-dump cache snapshot now resolves key1533 to a
model with zero bones, meshes and actions; the RISE root therefore remains a
lifecycle-only record and must not stage or synthesize a root BMD. The bounded
dispatch remains guarded by `verify_breche_dispatch_tables.py`; root output is
guarded by `verify_breche_root_contract.py`. Remaining work is child render,
contact, lifetime and pool evidence, not another root-asset hunt.

Protocol mapping follow-up (2026-09-13): anchored ProtocolCore12FE380
bounds its first argument to0xFD at12FE3CD and indexes130505C directly.
Entry0xDF points to12FEB99, which forwards buffer/size/encrypted arguments
and calls12C4F86 at12FEBA2. Thus the audited S21 receive path is opcodeDF,
NOT native5.2 opcode19. The pinned table verifier now checks this branch
and the direct call, plus full dump SHA256 before all table checks.

Native source corroboration: WSclient.h::PRECEIVE_MAGIC orders header,
MagicH/L, SourceKeyH/L, TargetKeyH/L. GameServer SkillManager.h's
PMSG_SKILL_ATTACK_SEND matches; GCSkillAttackSend emits opcode19, writes
skill first, lpObj->Index second, aIndex last with success in its high bit.
S21 instead reads source from3/4 and skill from5/6 (target7/8 unchanged).
Never send a captured S21 buffer directly to the native receiver or copy
its field offsets. Use native named fields and native opcode for the port.

S21 receive12C5496 computes facing from source.Position toward the
secondary object's Position and writes source.Angle.Z at12C54BD;12C54CC
stores the secondary index into source character+40. This corroborates
the secondary object's target role within the client. Success and skill
are then stored on the source at12C5516/12C5526. This does not establish
the S21 GameServer damage/AOE producer or imply every target is a victim
(self-targeted skills may use the same protocol). Breche root remains
attached to the received secondary object, not unconditionally Hero.

Anchored receive function0x12C4F86 reads actor key from packet bytes3/4,
secondary key from7/8, skill from5/6. Skill is normalized by0xBCFF9F before
switching: it can follow the skill table's +0x5C parent chain. Thus this switch
is normalized skill279, not necessarily an unchanged wire skill279.
Secondary key is masked0x7FFF at0x12C52D7 and resolved by0x13F5F39,
which searches400 live characters for matching key at character+0x3E.
The resolved secondary character becomes local-0x1958 at0x12C5336 and its
object(+0x3254) becomes-0x1948 at0x12C537D. The receive root copies that
SECONDARY actor's object position/angle/light and owner, not caster local
-0x1944. Semantic target naming still needs packet producer corroboration;
do not substitute Hero for this received owner.

Constructor dispatch0x143EECA subtracts0x4F7, then table0x14B3224 entry
0x106 selects0x148251E. Subtype1 branches to0x148259C: lifetime and
max-lifetime (+0x6C/+0x70) both20. It creates FIVE children before return:

| Call VA | Type | Subtype | Supplied scale | Supplied RGB |
|---|---|---|---|---|
|0x1482645|0x81EC|0|2.0|(1,.07,.07)|
|0x14826A7|0x809F|0|2.5|(1,.07,.07)|
|0x148274E|0x809E|14|0|(1,.7,.2)|
|0x14827B0|0x809E|14|.2|(1,.7,.2)|
|0x1482818|0x806E|15|0|root inherited light|

All five pass root position/angle and root owner(+0x34C) to the existing
CreateEffect wrapper0x143E57C. Supplied arguments are NOT final child scale,
color or lifetime: each child constructor can override them. Registration,
constructor/update/render/destruction of these four child types remains open.
Subtype1 jumps0x1482B26 after these calls; follow its shared tail separately.
Reproduce receive/constructor entries with verify_breche_dispatch_tables.py.

Local compare 0x10E35BC tests skill 0x117 (279) and jumps to 0x10E43CB.
That branch sends via 0x10EEB1E, calls 0x1327DE8 with action0xBE (190)
at 0x10E442B, and plays 0x45F at 0x10E443C. Its immediate CreateEffect
calls are guarded by skills0x812 and0x829; base279 skips those calls.
This is a LOCAL-branch observation, not proof that the skill has no root.

The receive switch at 0x12C5619 subtracts279, bounds to19, loads selector
from 0x12CE942 and target from 0x12CE916. Index0 -> selector0 ->0x12CB7F5.
That branch creates effect0x5FD SUBTYPE1 at 0x12CB854. Owner, position,
angle and light come from local [ebp-0x1948] (+0x158/+0x164/+0xEC).
That local is derived from character local [ebp-0x1958]+0x3254 at
0x12C5372..0x12C537D; caster/target identity requires tracing this local's
producer before native mapping. Prior blanket exclusion of0x5FD from base
Breche is RETRACTED. The isolated RISE `ReceiveMagic` branch now preserves the
native source/target lookup and invokes `CreateBrecheAction(*so)` followed by
`CreateBrecheHit(*to)`. This is a client receive-to-visual handoff only; it
does not prove server authority, damage, target validity or owner pixels.
Guard: `tools/grow_lancer/verify_breche_receive_render_wiring.py`.

Update-table leads: primary slot0x59 ->0x153542A; subtype1 takes default
to0x1538599 ->0x1574472. CORRECTION: secondary candidate0x15956D0
->0x157F0A5 is REJECTED for5FD. The table at0x1595660 has base5E1
and bounds<=0x10 at0x1574A8A: valid types5E1..5F1 only. Indexing it
with5FD crosses into another table. The actual comparison0x15748ED
routes5FD (>5F2) to0x1574AA4, which subtracts5F3 and bounds<=0x48.
Byte table0x1595728 entry0xA is0x20; pointer table0x15956A4 entry0x20
is0x15945E5, the generic movement helper path. Focused verifier now checks
this selection. Therefore the particles7F5A/11, random512..515 debris,
and8013/4 emitted by157F0A5 are NOT established Breche children and must
not be added to this chain. Its lifetime-holding behavior is unrelated too.
Primary root/model-animation and rendering still require full closure.

At 0x17D085D..0x17D0873, argument Type at [ebp+0x10] minus 0xAE9 is
reduced modulo 512 and stored in [ebp-0x25F4]. At 0x17D0B9A it is copied
to [ebp-0x2614], then subtracts 185 and indexes table 0x1805F53.
The pants selector at 0x17D3C96 likewise reads [ebp-0x25F4]. Therefore the
number 190 in these tables means ITEM INDEX 190, not animation 190.
The old red/clipping layer is excluded from the Breche descriptor. It has
never been connected to the live Breche renderer. Do not connect it using
these notes. The generic mesh, loader and bone evidence remains valid for
equipment work but does not close ANY Breche-specific visual layer.

Next: revisit actual skill279 dispatcher/action190 consumers and establish
the genuine skill visual chain. Base sound/action claims require independent
dispatch evidence and must not rely on these equipment tables.

## Identity and base dispatch

- `SkillList.xml` identifies skill 279 as Grow Lancer `Breche`: level 300,
  damage 230, mana 15, BP/AG 6, range 5, Strength 300 and Dexterity 300.
- `RegularSkillCalc.lua` proves the base formula
  `(InDamage*0.5)*(Strength/10+97+SkillTreeBonus)/100`.
- The common S21 caster dispatcher sets action 190 for base Breche. Its base
  branch creates no standalone effect root. The separate upgraded-skill
  branches that create `0x738` or `0x5FD` are not attributed to base 279.
- Sound 1119 is registered at `0x018E2F47..0x018E2F73` from string
  `Data\Sound\Breche.wav` (`0x01BB53F4`).

## Action-specific visual path

- The containing routine receives `BMD*` at argument 1, `OBJECT*` at argument
  2 and a model type at argument 3. It normalizes the model type with
  `(Type - 0xAE9) / 512` and `(Type - 0xAE9) % 512`. This proves that the four
  selector values `8..11` are item groups Armor, Pants, Gloves and Boots. They
  are not weapon variants. Group 7 (Helm) uses the routine's normal body path.
- Four 12-entry body-equipment jump tables cover actions `185..196`. Index 5
  therefore maps action 190 to handlers:
  `0x01805F53 -> 0x017D1082`, `0x01805F83 -> 0x017D40F7`,
  `0x01805FB3 -> 0x017D50C9`, and `0x01805FE3 -> 0x017D5AA5`.
- These branches render the caster's currently equipped Armor/Pants/Gloves/
  Boots model through native
  `BMD::RenderMesh` (`0x00968CB3`) or the corresponding native body/mesh path
  (`0x00968C58`). They are not pooled effect constructors.
- Armor renders normal texture meshes 2, 4 and 5, then 0 and 1 except for the
  explicitly excluded object type `0xA94`; its conditional red pass is mesh 3.
  Pants renders normal meshes 0, 1, 2 and 4 with a conditional red mesh-3 pass.
  Gloves uses the native whole-body render call. Boots renders normal mesh 0
  with a conditional red mesh-1 pass. The visual lifetime is therefore the
  duration of action 190 and the four body-part render calls, not a separate
  effect-pool lifetime.
- Conditional special passes use runtime texture `0x8138`, registered at
  `0x018C990E` as `Item\Texture\Item1130_L_red.jpg`; the actual packaged file
  is `Data\Item\texture\Item1130_L_red.OZJ`.
- The bright pass uses flags `0x42` (`RENDER_TEXTURE | RENDER_BRIGHT`). The
  alternate conditional pass uses S21 flag word `0x200002`. Its extra
  `0x200000` bit is decoded in the S21 core renderer at
  `0x01333847..0x013338D7`: the current triangle is omitted only when the
  supplied V threshold is strictly greater than the transformed Z value of
  all three triangle vertices.
- SS6 has no equivalent flag in `ZzzBMD.h`. The existing renderer also flattens
  the complete mesh into one array before `glDrawArrays`, so silently dropping
  the bit would render the wrong half and an OpenGL clip plane would split
  triangles instead of matching S21's whole-triangle rejection. This remains
  a required isolated BMD adapter, not a compatible stock flag.
- The value tested at object offset `+0x2A` is the current character action,
  not an item index. Actions `1..49` and `348..410` move the per-part V/Z
  threshold upward by `0.1 * 40 * 0.2`; other actions (including Breche 190)
  move it downward by `0.1 * 40`, clamped against the model bound. While the
  threshold moves, the `0x200000` pass reveals the red texture by rejecting
  whole triangles. At the lower clamp, the red bright pass is drawn twice.
- Global object `0x1E05010` is a scene-mode state, not a class selector. Its
  literal assignments include states 3, 4, 5 and 6; this renderer enables the
  moving threshold only in state 6. Outside state 6 it snaps the threshold to
  model bound plus 200. The exact symbolic enum name remains unavailable.
- RISE now contains `RenderBrecheClippedMesh`, an isolated adapter that copies
  only retained `Triangle_t` records, temporarily swaps `Mesh_t::Triangles`
  and `NumTriangles`, calls stock `BMD::RenderMesh`, and restores the original
  mesh through RAII. It compiles in the x86 client and exactly preserves the
  strict comparison/equality behavior without changing the global renderer.
- `0x17D50C9` is confirmed as the Gloves handler for action 190. The nearby
  `0x813E` (`Item3129_L_red`) code starts at action 196's handler `0x17D5122`
  and is explicitly excluded from Breche.

## Compatibility consequence

- RISE SS6 does not contain Grow Lancer equipment/class support. Porting only
  the action clip is insufficient for exact Breche appearance: the selected
  compatibility actor must expose Armor/Pants/Gloves/Boots models with the
  verified mesh indices and conditional texture contract.
- The package therefore stages only the proven Breche sound/texture and keeps
  the class/equipment mapping unresolved rather than silently substituting a
  visually similar SS6 weapon.
- The S21 routine is additionally restricted to item indices `185`,
  `189..199`, `205`, `217`, `246`, `264`, `311`, `346` and `400`. Those are
  absent from the native SS6 equipment catalog, so the adapter is deliberately
  not connected to `RenderItemBodyEffect` until a provenance-backed recipient
  model package exists.

## Open work

## Dynamic item model lookup (2026-09-12)

The static model registry's literal-push pattern misses this path. Anchored
function 0xEF2A81 normalizes model ID by subtracting 0xAE9 and calls item-record
lookup 0xD06184 (singleton 0xD04FE0). Its branch 0xEF2B8D passes record+8
and record+0x10C to 0xD05887 at call 0xEF2BA7. Loader 0xD05887 passes those
strings directly to 0x132B730 at 0xD05963, and on failure reports group/index
using division/modulo 512. These are S21 evidence addresses, not port hooks.

Table loader 0xD0506D reads a count, count*0x2C4 bytes, then a checksum;
0x97904E checks encoded bytes with seed 0xE2F1. Each record is then decoded
by 0x97900E, modulo-three XOR key FC CF AB at 0x1DD7448. Current read-only
Data/Local/item.bmd has 4950 records and passes that checksum. SHA256:
9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6.

Reproduce with tools/grow_lancer/inspect_breche_item_models.py. It reports
source paths, per-file SHA256 and four-byte headers without staging anything.
For example current table group8/index185 maps runtime7074 to
Data/Item/ArmorMale128_benzia.bmd, NOT ArmorMale185.bmd. Index189 maps
ArmorMale131.bmd, index190 maps ArmorMale130.bmd. Therefore filename numbers
must not be substituted for item indices.

Important: current installed table mapping is proven for this table hash;
equality with the table loaded in the historical native-reference session is
NOT yet proven. Reused filenames in multiple records need comparison with
captured equipment and original meshes before choosing a recipient package.
Headers alone do not prove successful BMD parsing, skinning or Breche parity.

## Remaining work

### Bone-index compatibility (2026-09-12)

Update after native loader tail audit: all 29 models now pass the offline
structural walk and referenced-bone comparison against both player files.
The seven exclusions described below are historical, caused by the tool's
old <16-byte tail policy, not by a discovered model defect.

Read-only `compare_equipment_bones.py` compared every geometry-referenced bone
and its ancestors by index, name, parent and dummy status against BOTH players:

- Current S21 player SHA256
  e0f4cb5a0956192d04026135766c566bc6b9b7b559d2f196f74d24d28e2e755a.
- Isolated DK QA player SHA256
  0cc3d22d5bbd426128e6bffe9c3766585f9f28bbda5b3dd50122ec7ad6b9ca63.

Both players have 60 bones. All 22 models accepted by the current strict
layout walker have ZERO mismatches to either player for those fields. Thus
different total bone counts alone do not require index remapping for these
models. Seven models remain excluded because of their 16-byte trailing data.
No bind-pose, vertex transform, runtime skinning, class or equipment eligibility
acceptance follows from the header comparison. Five focused tests verify
matching, unused extra nodes, ancestor-name mismatch, parent mismatch, missing
nodes and cyclic-parent rejection (matching/extra share one test).


### Offline mesh and bounds check (2026-09-12)

Superseded tail restriction: full anchored loader 0x132B730 was inspected.
The v0F branch at 0x132BAA3 reads the encoded size, allocates that size and
passes it to decrypt thunk 0x91AA00 at 0x132BB04; the decoded cursor resets
at 0x132BB40. Bone loop termination 0x132C437..0x132C447 jumps straight to
0x132C70C, which frees the decoded buffer, initializes the model and returns
true at 0x132C749. There is no comparison of final cursor to decoded size
and no check of tail byte contents on this exit path.

The offline tool therefore now accepts up to one full 16-byte trailing block,
with arbitrary contents recorded but never interpreted as geometry or texture.
This cap is conservative TOOL policy, not a native S21 size restriction.
No claim is made about the exporter padding algorithm. Three tests exercise
0/1/15/16-byte tails, rejection above that cap and truncated mesh rejection.
All 29 files now complete the walk and bone-header comparison. Combined with
five bone-comparison tests, focused checks total eight PASS. Native GPU load,
bind-pose and visual QA remain open.

`inspect_equipment_meshes.py` reads the existing v0F converter output and walks
the native Open2 mesh/action/bone layout with bounds and index checks. Of 29
unique models, 22 reach the end with at most 15 trailing bytes. Only three
of those have zero-only tails; other tail bytes remain explicitly unverified.
Seven models end with 16 trailing bytes and fail the current strict tail limit;
this is NOT evidence that the assets themselves are corrupt. No native load
PASS or converter output was produced by this inspection.

Observed armor models have six meshes, boots two. Pants are model-dependent:
PantMale129 has two, PantMale128_alice/elf three, PantMale130/131 five.
Bone counts also differ (51, 57, 60 observed), so a recipient skeleton cannot
be selected by class label or filename alone.

Confirmed why absent meshes must not be manufactured: wrapper 0x968CB3 calls
0x13312EF; its anchored entry checks meshIndex against model's signed mesh
count at +0x24 and against zero (0x133131E..0x1331330), returning through
0x1333A15 for invalid indices. Native RISE RenderMesh and the private clipping
adapter already guard the same mesh-index boundary. Thus the Breche call list
does not imply every equipment model contains every listed mesh; missing
passes are skipped in S21. Preserve that behavior, do not duplicate geometry.


- Exact GameServer line/AOE target selection and hit scheduling.
- Evidence-backed recipient class/equipment mapping in RISE.
- Isolated load/build/runtime comparison and regression validation.

## Authoritative owner-side correction from fresh S21 cast (2026-09-15)

The fresh native-reference capture `artifacts/runtime_qa/s21_reference/breche_owner_recast_20260915_b/frame_00042.jpg`
separates two presentations that must never be merged in the port: the caster has
a large red/orange/pink circular sweep with radial sparks, while each contacted
target has its own upright white/orange fire column.  The latter is the receive
root, not the caster effect.

The owner path is now pinned in the protected S21 dump.  Per-character update
`0x1424B34` selects skill `0x117` at `0x14259F9..0x1425A03` and enters
`0x142B21D`.  That producer submits type `0x5FD`, subtype `0`, through the
native allocator `0x143E57C`, with the current character as owner, copied
character transform arguments, and a ten-unit scalar.  The independent receive
packet path normalizes skill 279 and enters `0x12CB7F5`, which submits the same
type with subtype `1` against the resolved secondary target; it is not the owner
presentation.

Primary update dispatch `0x14B769F` normalizes `0x5FD` to table index `0x12A`;
selector `0x59` at `0x153542A` routes subtype `0` to `0x1535478` and subtype
`1` to the common handler `0x1538599`.  The owner handler keeps a 20-tick
lifetime and, while the owner is valid, emits three native particle-wrapper
records per primary tick (`0x8084/0`, `0x806E/4`, `0x8085/0`), ten
`0x8073` pin-light joints (extended CreateJoint, subtype 5, scale 6.0,
orange color), and the timed layers below.  It also emits one `0xAD9` wind
model record per primary tick with subtype 6, scale 3.0 and skill metadata 279.

Owner timed layers recovered from the exact call sites are:

- remaining life 17: ring-of-gradation (`0x80BC/sub1`, scale 5.5, red light)
  and lightmarks (`0x81EC/sub0`, scale 6.0, red light);
- remaining life 7: fire ring (`0x81EB/sub0`, scale 6.5, white light);
- remaining life 15: `0x809F/sub2`, scale 3.0, plus two `0x809E/sub13`
  records with scale 0 and skill metadata 279.

The isolated 5.2 adapter now creates the owner controller separately from the
target-contact helper, uses private hash-pinned owner resources under
`Data\\RISE\\GrowLancer\\Breche\\Owner`, and renders the converted `wind_foce`
through the native BMD Open2/Calc_RenderObject/RenderMesh path.  This is an
implementation checkpoint, not visual acceptance: exact S21 random local
placement, native blend/flip state for every owner bitmap, and an accepted 5.2
owner frame remain open.

## Owner controller position contract (2026-09-15)

The fresh cast was followed by a full decode of the position writes in
`0x15354EA..0x1535561`.  The S21 helper `0xD3189D` receives the local vector
`(0,-200,0)`, the matrix built from the controller's own `+0x164` angle, and
the controller's `+0x158` position as its output pointer.  The output is then
added to the generic allocator's owner snapshot at `+0x1D0` (copied from the
owner `+0x158` by `0xD2DF9E`).  Therefore the native semantic is:

```text
controller.Position = owner.Position + rotate(controller.Angle, (0,-200,0))
```

This is not a raw S21 offset and must not be implemented as a double owner
position.  The first three owner particle records are emitted before this
rewrite and use the previous controller position. Pin lights use the rewritten
controller position. A later exact argument audit corrected an earlier
attribution error: the `0x80BC` ring and `0x81EC` marks calls at
`0x1535C6C..0x1535C86` and `0x1535CD6..0x1535CF0` load Position through
`root.Owner + 0x158`, not through `root + 0x158`. The `0x81EB` fire ring and
the two `0x809E/0x809F` twilight calls use the owner snapshot (`+0x1D0`).
Thus every visible timed ground layer remains centered on the caster; only
the pin/particle carrier retains the rotated -200 offset.

Renderer evidence confirms `0x81EB` subtype 0 enters `0x15AEAED` and calls
`0x1765DF1` with the effect's own `+0x158` X/Y, scale, angle Z and
`Light*Alpha`, using additive blend and terrain height 5.  It does not read
`+0x1D0` at render time. The owner wind call is wholly inside the S21
remaining-life-7 branch (`0x1535DC1..0x1535FCF`): it copies the owner snapshot,
subtracts the pinned `5.0f` constant from local Y, then passes that local at
`0x1535FC2`. It is therefore a single child, not an every-tick emission.
The previous 5.2 mapping created up to 20 overlapping wind bodies at the
forward carrier position and is retracted.

## Owner spatial correction after second 5.2 visual FAIL (2026-09-15)

The owner-observed frame showed the large white arc still displaced beyond a
one-tile target. Runtime breadcrumbs measured caster Y=12950, target Y=12850,
and the ring/wind carriers Y=12750. Re-reading the complete call arguments
proved that the 200-unit carrier offset was wrongly propagated to visible
ring/marks and that AD9 was wrongly emitted every tick. The adapter now puts
ring/marks at `owner.Position` and emits one AD9 record at owner snapshot Y-5
only when remaining life is 7. This correction is dump-backed and build-tested;
pixel acceptance remains open until the new binary is cast in the QA client.

The same constructor branch at `0x147134E..0x14714FD` sets AD9/subtype6
`LifeTime=MaxLifeTime=20` and alpha `0.9`. Its producer builds light
`(1,.2,.2) * 3`, so the 5.2 adapter now initializes the child with
`(3,.6,.6)` instead of inheriting the caster's white light.

The neighboring `0x81EB/subtype0` constructor at `0x1482B5A` sets both
life/max-life to 12 and alpha to zero. Its update compares life against half
of max-life and changes alpha by `1/(maxLife*0.5)`, so the exact fixed form is
`life > 6 ? +1/6 : -1/6`. The previous life20/alpha1 envelope is retracted.

## Owner motion correction after 5.2 visual FAIL (2026-09-15)

The first visible 5.2 preview proved two symptoms that static submission logs
could not: the forward layer used a stale direction in the F7-only fixture,
and its ring/wind presentation appeared frozen. The verified 200-unit local
offset above is retained. F7 now reproduces the already-established
ReceiveMagic ordering by resolving caster facing toward the selected target
before owner-controller creation; no production packet or target behavior was
changed.

Fresh primary-dispatch decoding closes the missing child motion branches:

- `0x80BC/sub1`, `0x1518760..0x1518819`: divide Scale and all three Light
  components by constant 1.1 each tick;
- `0x81EB/sub0`, `0x15385D2..0x153868B`: add 15 degrees to Angle.z and add or
  subtract 0.1 alpha across the two halves of its 20-tick lifetime;
- `0xAD9/sub6`, `0x151D307..0x151D459`: subtract 50 degrees from Angle.z,
  subtract 0.01 alpha, then multiply each stored Light component by alpha.

`verify_breche_owner_primary_dump_contract.py` pins the branch entries and
float constants; `verify_breche_owner_adapter.py` pins their 5.2 mappings and
the pre-controller QA facing order. Build and static checks pass, but visual
acceptance remains open until the rebuilt frame sequence is observed.

The owner wind model also does not use a private bright mesh-zero pass in S21.
Render dispatch `0x15A136D` recognizes `0xAD9` and reaches `0x15AD77D`, which
calls generic wrapper `0x176D621`. That wrapper runs `0x1886382` for the object
transform and `0x1887B8B` for rendering. The default body submission at
`0x1887E5E` pushes texture -1, the carrier's hidden/blend/light/UV/alpha tuple
and render flag 2 (`RENDER_TEXTURE`). The 5.2 adapter now calls native
`BMD::RenderBody` with the same named fields after `Calc_RenderObject`; it no
longer multiplies light by alpha again or forces `RENDER_BRIGHT` on mesh zero.

## S21 blend-mesh material contract (2026-09-15)

Owner QA accepted the corrected caster-centered position and the yellow
`0x8073/subtype5` slash streaks, while the center spiral visible in native S21
frame 42 was absent. PID22760 nevertheless logged the AD9 child as live,
visible and submitted with one mesh, one bone, one action and texture 35443.
The staged `ground_wind.OZJ` is the hash-pinned S21 asset and decodes after its
24-byte container prefix to a 256x128 three-component JPEG whose background is
black. This narrows the failure to material blending rather than chain timing,
position, model parsing or texture lookup.

S21's generic body call supplies numeric flag 2. Common object reset
`0x1315FEC` initializes BlendMesh field `+0x74` to zero and wrapper
`0x1887E99..0x1887E9F` forwards that value. The decoded model has one mesh and
that mesh references texture slot zero. Consequently RenderMesh
`0x1332CBB..0x1332CFB` selects its additive blend-mesh path before the ordinary
flag-2 texture branch. The isolated adapter now maps this exactly as
`RENDER_TEXTURE + BlendMesh 0`; the prior extra `RENDER_BRIGHT` compatibility
inference is retracted. This changes only the private Breche wind carrier.
Static/build checks and owner-observed visual acceptance remain separate.
