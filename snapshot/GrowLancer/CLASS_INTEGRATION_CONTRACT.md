# Native Grow Lancer class integration — IN_PROCESS

Owner requires a distinct native class; isolated DK testing is temporary only.
No production class/DB change is authorized by this inspection.

## Supplied S21 backend identity check — 2026-09-15

`D:\GameServer S21\GameServer\GameServer.ini` points to a local DataServer
(`127.0.0.1:57960`) and refers to `\DataServer\config.ini`, but the supplied
S21 directory contains no such DataServer configuration or executable. Its
read-only `3-2. Sub-2.zip` (SHA-256
`3DA097CD069CB80683D2DD09DC55F2F4CECBA0A482FB045C8559E6EEFF8D01F0`,
1,219 entries) likewise contains no DataServer/database config, SQL file or
`DefaultClassType` export. Thus the local `MuOnline52` class-112 row is not
proven to be this S21 server's DB default and cannot authorize native class
activation. Neither supplied file was modified or executed.

## Supplied patch cross-check — 2026-09-15

Read-only `Rise-S21-Server-Patch.zip` (SHA-256
`303C9C671C28D8DF572B7CBB28577A29996D7ECDD1E7F2CD3CBF900F5FB8A79B`,
1,493 entries) contains 5.2 GameServer source and staged server files, but its
`User.h`, `ObjectManager.cpp` and `Protocol.cpp` do not define `CLASS_GL` or a
Grow Lancer native create/login path. `SkillManager.cpp` has only two Grow
Lancer text references. This supplied patch is not a complete class-7/server
skill port and must not be merged wholesale into the isolated worktree or
treated as authorization to enable Grow Lancer cast. The ZIP was not modified.

## Verified source identity

### Cast-speed flags priority pinned — 2026-09-16

Producer follow-up: complete source `0x1408580..0x14086AA` initializes
two unsigned-word speed locals from record `+1B6/+1C2`. A manager-present
path calls `0xC5B813(1)`, whose complete body walks three active slots,
compares descriptor type to argument1 and sums matching integer `+1C`
values. That ONE returned sum is added to BOTH speed locals. A separate
optional record selects a lookup, then calls `0xB60918(0x3F0)` for each
local; its `0x10FC071` callee returns zero on miss. Both returns are
again added before source flags/Tornado/cap. This proves the arithmetic
shape, not that native AttackSpeed already contains/excludes these values
or what manager slot type1/lookup1008 mean. New dormant
`S21CastSpeedProducer.h` composes SOURCE-DOMAIN values only;
`verify_s21_cast_speed_producer.py` pins producer/callees and actual x86
`/W4 /WX` static-assert compile PASS. Ordinary class7 activation remains
fail-closed until actor/bonus source→native mapping is proven.

Owner/input correction (2026-09-16): complete source
`0x1408580..0x14087C8` reads base words `+1B6/+1C2` through GLOBAL
stat pointer `0x1E04FBC`; it reads the actor argument only at the
optional Tornado presence gate `0x1408716/0x1408721`. The source
player-model caller `0x140DD63..0x140DD87` tests model0xA94 and passes
its actor object, but no per-remote speed words are read in this
producer. Native `SetAttackSpeed` also writes global player-model
action speed from local `CharacterAttribute`; native join packet supplies
named DWORD `SpeedAttack/MagicAttack`. Thus per-remote base stat
transport is NOT a prerequisite proved by this S21 chain. Class7
local/global stat value equivalence, packet width, manager bonus
double-count, actor-specific Tornado and global model last-writer
behavior remain OPEN. `verify_s21_global_speed_owner.py` pins both
complete source/caller blocks and native ownership boundary.

Complete S21 `0x14086AA..0x1408716` basic blocks prove bit1 and bit8
each can add20 to both source speed locals, but the bit1-true branch
JUMPS over the bit8 test at `0x14086DF`. Therefore flags value9 adds
20 once, not40. The existing dormant `S21AdjustCastSpeedTail` OR-mask
formula was correct; a draft double-add interpretation was immediately
reverted after full-block decode, before any build/stage/deployment.
`verify_s21_speed_flag_priority.py` pins branch/jump/writes/constant and
the flags9 pure test; actual VS18 x86 `/W4 /WX` compile PASS. This
clarifies source math but does not map source speed `+1B6/+1C2`, manager
bonuses, Tornado/cap or real class7 actor into native 5.2 fields.

Protected supplied S21 file `D:\GameServer S21\Data\Scripts\Character\CalcCharacter.lua`
line21 declares `CLASS_GROWLANCER = 7`. SHA256:
`9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70`.
This proves the supplied calculation-script base class number, not all S21
network/DB encodings or evolution IDs.

## Current native boundaries

### Fixed-action owning-class guard — 2026-09-16

The isolated ordinary client now resolves the `CHARACTER` that owns a received
player `OBJECT` (Hero by pointer identity first, otherwise the live
`CharactersClient` pool in the main scene) before preparing one of the verified
constant-speed S21 clips. Only exact unevolved class byte `7` may receive this
fixed-action preparation. Unknown/evolved bytes and SS6 classes fail closed;
the explicit RuntimeQA local Dark Knight preview path retains its prior scope.
The guard never substitutes local `CharacterAttribute` for a remote actor and
does not widen `MAX_CLASS` or the skill slots. Source verifier
`verify_fixed_action_actor_class_gate.py` PASS; isolated Win32 normal and QA
client links exit0, hashes F352A9C5... and 6C4B52E0... respectively. The
comparison uses the package's existing named source-class constant, distinct
from the native UI selector `18`. This
protects fixed-action dispatch from attaching GL clips to unrelated classes;
it does **not** initialize class7, establish the dynamic-speed Magic/Spin/
Harsh/Shining rate, validate server learning/cast, or prove gameplay pixels.
No QA executable/Data was staged and no production client was touched.

### Class-7 base speed recalculation gap — 2026-09-16

`CharacterMachine::CalculateAttackSpeed` initializes attack/magic speed only
for seven SS6 class indices 0..6 before item, ability and buff additions.
For base class7, `index=-1` leaves the previous `Character.AttackSpeed` and
`Character.MagicSpeed` untouched and then adds the ordinary modifiers. The
isolated GS `ObjectManager.cpp` similarly lacks a `CLASS_GL` base-speed
assignment before drink/effect/item additions. A repeated class7 calculation
can therefore carry stale values and compound modifiers; it is not a valid
normal dynamic-action input. `verify_class7_speed_recalculation_gap.py`
pins both native boundaries and the hash-pinned S21 Lua branch.

S21 `CalcCharacter.lua::CalcAttackSpeed` selects DEX/20 for class7, but its
own preceding comment calls this an anti-hack speed calculation with a
version-applicability qualifier. The script branch alone does not prove
the S21 GameServer caller, integer/packet rounding, or the client animation
stat producer. No DEX/20 branch was inserted into the ordinary client/GS;
class7 activation and action-speed parity remain fail-closed pending those
contracts. This audit does not change the local DK visual QA path.

### Native join-to-recalculation speed ownership — 2026-09-16

`verify_class7_join_speed_chain.py` pins the isolated GS's final
`lpObj->PhysiSpeed/MagicSpeed` copy into join `DWORD SpeedAttack/MagicAttack`,
client named-field assignment, then the second join `SetCharacterClass(c)`
after `Hero=c`, which calls `CalculateAll/CalculateAttackSpeed`. Class7 has
no base assignment in GS or client; client speed then adds weapon/ability/
buff terms to the just-received final GS value. This is a concrete source
path for stale/double-counted class7 speed, not measured runtime magnitude.
Ordinary item/stat/buff consumers also call `CalculateAll` repeatedly.

Native join fields are GS final speed, not a source-backed class7 DEX divisor.
S21 head82/sub04 receives WORD speed into a global member, but its initial
sender, actual GS rate/rounding and opcode equivalence to native join remain
unproven. Do not implement DEX/20 solely from qualified anti-hack Lua or
reuse final GS join value as an additive client base. Class7 activation
remains fail-closed until scoped packet/recalc ownership is established.

### Native speed divisor packet capacity and update ownership — 2026-09-16

`verify_class7_speed_packet_capacity.py` pins the separate native F3:ED
speed-divisor message: client/server packet structs and `pMain->ThisValue`
are exactly 14 bytes, GS sender fills pairs0..13 for seven SS6 classes and
client receiver copies exactly14. `CalculateAttackSpeed` selects the same
base-class indices0,2,...12. Class7 would need pair14/15 only if this
same-table scheme is retained; neither pair nor GS config keys exist. This
is a lookup/packet-width issue, not the character's skill-slot count or
skill-ID capacity. Expanding F3:ED globally without rate/compatibility proof
could regress existing SS6 clients; no packet or global count was changed.

The GS sender calls F3:ED during character calculation, but this packet
contains divisors only, not updated final speeds. Within the isolated GS
source, `SpeedAttack/MagicAttack` final DWORD fields are populated for join;
no separate named final-speed update sender was found in the searched GS
source. S21 opcode82/sub04 WORD speed update is a distinct packet chain,
not a verified drop-in native opcode. Thus class7 item/stat/buff changes
need a source-backed local recalc rate or an explicitly scoped authoritative
speed-update design. Both remain open.

### S21 direct global-speed pointer candidate classification — 2026-09-16

`scan_s21_stat_pointer_refs.py --near-speed` hashes the 266 MB S21 mapped
main dump and narrows 782 literal `1E04FBC` occurrences to 17 candidates
whose following 200 bytes also mention `+1B6/+1C2`. Focused
`verify_s21_speed_pointer_candidate_roles.py` decodes selected blocks:
`1296442` is the proved opcode82/sub04 receiver writing both WORD fields;
`B553/B56D`, `127D95D`, `1408580` and four `14140/14155` visual-scalar
paths read them; `F62506` reads indexed member+1E6, and `B552BD/D5` read
flag+1A6. This is a bounded literal-pointer/proximity classification, not
complete alias/write proof. Constructor zero writes precede global assignment.

No packet capture or original GS `CalcAttackSpeed` caller was found in the
scoped supplied S21 QA evidence; existing logs are skill/crash traces. Initial
sender/rate/rounding/packet timing remain OPEN. Numeric pointer xrefs alone
must not become class7 speed authority.

### Same-offset outbound stat buffer is not the global speed writer — 2026-09-16

The pinned S21 `1296442` opcode82/sub04 receiver really writes WORDs from
its input `+4/+8` to **global** stat pointer `1E04FBC` at `+1B6/+1C2`, then
refreshes the display; it does not directly recalculate PlaySpeed. A raw
numeric xref at `95C0C1` had the same destination offsets, but full function
and caller decode shows `ECX` is a separate object, `[EBP+8]` is caller stack
temporary `9572A6` at `[EBP-210]`, and the copy takes source object WORDs
`+15E/+1EC` into temporary `+1B6/+1C2`. On a successful copy the caller
constructs a `C2:AE` buffer and appends the 0x200-byte temporary. This is
not the initial write of the global speed words or proof of receiver82 wire
equivalence. `verify_s21_speed_temp_packet.py` pins this source/caller and
the only direct call lead `9572E8`; `verify_cast_speed_receive.py` separately
pins the actual global receiver. The field producer, full outbound transport,
initial global stat join and S21 GS rate/rounding remain OPEN. Supplied GS
`IGC.GameServer2_R.exe` SHA134BAEBF... has virtualized `.vm_sec`, `.winlice`
and `.boot` sections; static file checks did not recover its Lua caller, and
the protected executable was not executed or patched.

### Global S21 speed-stat member lifecycle — 2026-09-16

Complete source `127879C..1278805` allocates 0x2DF0-byte outer record,
calls outer constructor `A07788`, assigns the outer pointer to global
`1E04FB8`, and assigns `outer+8` to **global stat pointer** `1E04FBC`.
The outer constructor calls `A09F4B` on exactly `this+8`; its member
constructor zeros WORD fields `+1B6/+1C2`. Hence the same member is what
the opcode82/sub04 handler `1296442` later updates from incoming WORDs.
`verify_s21_global_stat_lifecycle.py` pins allocation, both constructors,
global assignments, zero writes and receiver ownership in SHA6422CB4E...
dump. It does **not** show the initial character-join speed packet, all
subsequent writes, source GS rate/rounding, or native 5.2 DWORD mapping.
The read-only minidump's later 273/273 words are not constructor defaults.

The next candidate native 5.2 join opcode `C1:F3:03` must not be assumed
equal to S21: S21 head-F3/sub03 dispatches through `12884DB` into an
indirect/status switch `986942`, while plain head03 dispatches to a different
protected continuation `1297213`. `verify_s21_join_opcode_boundary.py`
pins both paths. Neither path has yet supplied a complete initial S21
global-speed delivery/wire-rate chain; ordinary class7 speed reset remains
fail-closed.

### Scoped native base-body gameplay caller — 2026-09-15

The ordinary 5.2 SetCharacter writes `MODEL_BODY_{HELM,ARMOR,PANTS,GLOVES,BOOTS}
+ GetBaseClass(Class)` into native part indices1..5. Base7 therefore reaches
unregistered SS6 placeholder models. The isolated normal-player render loop
now selects private `SubmitClassBaseBody` only for source byte7, native player
body indices1..5 and unevolved class, preserving worn equipment when render
equipment is on. When render equipment is off it selects the verified base
body instead. The private pass reuses native BMD::Open2/Transform/RenderBody
and returns before the placeholder when an asset is unavailable. This is a
scoped initial base-body caller, not class login/equipment/skeleton/pixel
acceptance; selection outline and transformed/mounted passes remain OPEN.
Focused C++14 x86 W4/WX choice tests, QA/nonQA client builds and a current
hidden WGL native body probe passed, but no class7 ingame actor was rendered.

### Default-stat candidate isolated — 2026-09-14

S21 `CalcCharacter.ini` is pinned to
`BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D` and
sets `GLLevelUpPoint=7`, `GLLevelUp3rdTreePoint=1`,
`GLLevelUp4thTreePoint=1`, `GLMaxAttackSpeed=273`. It explicitly delegates
`LevelLife`, `LevelMana`, `VitalityToLife` and `EnergyToMana` to
`MuOnline.dbo.DefaultClassType`; the supplied S21 server tree contains no
such row.

The read-only `D:\MU-CrossPlatform\MuServer52\Data\Character\DefaultClassInfo.txt`
(`F0FE413F2E239F96754F6F7109E65E17E2FD0E431246D24733F06DAEFA5A4849`) has a
row 7 candidate `30 30 25 24 0 110 109 2.0 1.0 2.0 1.0`. This is useful for
planning the adapter but is not S21 proof and is not copied into the branch.
`tools/grow_lancer/verify_grow_lancer_default_candidate.py` keeps that
distinction executable.

A separate read-only local SQL cross-check found the matching base row at
`MuOnline52.dbo.DefaultClassType.Class=112` (`30/30/25/24`, Life/MaxLife
`110/110`, Mana/MaxMana `40/109`, map `0`, position `182,128`, DbVersion `3`).
The result is recorded in `GrowLancer/LOCAL_DB_CLASS_PROFILE_REVERSE.md`.
Because the supplied S21 directory does not identify this local database as
its `MuOnline` backend, it remains a reference lead rather than activation
authority; no DBClass or native class table has been changed.

### Actual combined adapter/cache test passed — 2026-09-14

Opt-in RISE_GL_BODY_ADAPTER_QA=1 inside offline texture probe creates a native
BMD table only when Models is null, calls the real EnsureClassBodyModel for
all5 parts twice, checks invalid class/part rejection, and destroys the table
before GL teardown. PID20404 exited0 atUTC2026-09-14T00:16:32.204..205Z:
every part loaded1/cached1/countersStable1, final bitmap baselineRestored1.
This executes combined geometry/material/refcount code, unlike older primitive
probes. Cache counter stability alone does not prove individual refs unchanged;
final baseline restoration provides the bounded lifecycle check. Partial second
material failure and retry still need separate actual execution.

Build/staged SHA77520D15A113E15991164FBF96028680B4C69AA6CB03C1C576AE8CE377F5D35C.
Audit also found New_ModelBMD.cpp::ConvertOldMeshToVaoMesh rejectsmodelId>=MAX_MODELS.
Therefore its existing call cannot upload private body slots for the shader
path; do NOT claim shader/render acceptance from this successful legacy texture
context probe. Map the body-only path to native mesh/VAO APIs or extend a proven
range boundary without broadening unrelated behavior, then test actual drawing.

### Actual native shader/body draw probe passed — 2026-09-14

The private RuntimeQA executable was run with the opt-in native OpenGL body
probe (PID35908, exit0).  The latest log records shader initialization, all3
hash-pinned body textures with CPU/GPU-equal readback, five body parts loaded
and cached with stable bitmap counters, VAO/VBO/IBO size and caller-binding
checks, native draw samples for every part (`accepted=1`, samples>0,
`glError=0`), and the owner submit guard matrix (normal, hidden, near-zero
alpha and excessive bone-count rejection) passing for every part.  The probe
also records `body-gpu released=1` and `body-texture-release
baselineRestored=1`.  `tools/grow_lancer/verify_class_body_gpu_runtime.py`
selects the latest PID and verifies these rows.

This closes the bounded native upload/cache/draw/release probe.  Separate
clean-process fixtures PID15760 (conflicting sampler rejection/retry) and
PID31380 (missing-second-material rollback) also pass, with bitmap references
restored.  It does not
activate class7, prove S21 body pose/evolution selection, or claim final
gameplay screen-pixel parity; broader class/server/runtime gates remain open.

### Private runtime base assets staged — 2026-09-14

stage_gl_body_runtime.ps1 dry-run/apply/repeat apply all PASS. Five converted
base BMDs plus unchanged hair/skin textures now exist under isolated
RuntimeQA/Client/Data/RISE/GrowLancer/ClassBody, the adapter's real relative
directory. All7 source/output hashes enforced explicitly; different existing
outputs and reparse paths refused. Exact QA executable process check found no
running target before apply. Second apply preserved existing identical files.
No higher-evolution files deployed and no original/production paths changed.
The existing full QA verifier also passed afterward, but its13-private-file
count does not cover these7 additions; the new staging verifier supplies that
coverage separately. Client EXE remains016DAFA5 and cannot call the new adapter
until a newer build is staged; Bin0A5612D5 remains unstaged.
The bounded adapter invocation/cache, sampler-conflict and partial-material
rollback fixtures now pass; broader allocator, class-caller and gameplay
activation tests remain OPEN.

### Descriptor-backed native body adapter compiled — 2026-09-14

EnsureClassBodyModel(sourceClassByte,part,filter,wrap) uses the reserved32..36
tail, exact source07 descriptor gate, native Open2, and native bitmap refcount
loading. A current GL context is required. It accepts explicit native sampler
arguments, not a guessed final S21 sampler policy. Files are private relative
Data/RISE/GrowLancer/ClassBody, not original S21 paths. Gameplay does not call
this function yet, and those relative files have not been staged into Client.

Before any acquisition all IndexTexture slots receive native no-delete
sentinel. Source unique mesh slots are checked; on mismatch slots are made
safe for Release and loading is rejected. On texture failure only previously
acquired refs are owned, then native Release runs and allocation flag resets
for retry. No legacy EnsureModel behavior or effect range was changed.
Source verifier and isolated full Win32 build PASS; not failure-injection or
actual adapter invocation proof. Prior native probes covered individual BMD
and texture APIs, not this new combined wrapper. Cache/sampler behavior still
needs testing before caller activation.

### Private body capacity allocated without legacy renumbering — 2026-09-14

GrowLancerRuntimeCapacity now retains original32-entry skill tail and appends
five body slots at offsets32..36; total private capacity37. Slot31 stays spare.
ClassBodyModelId rejects part>=5, returning-1 explicitly with signed arithmetic.
Existing skill effect kLastModel remains offset30; IsGrowLancerEffect's range
therefore excludes the new body slots. No MAX_CLASS or legacy enum changes.
OpenPlayers allocation uses MAX_MODELS+1024+capacity and initialization uses
RuntimeModelLimit; Winmain release loops already use that same runtime limit.
The new capacity flows through these native boundaries, not a second model pool.

Focused x86 W4/WX test initially found a signed comparison in its fixture;
explicit safe casts corrected it. Focused and full isolated QA builds PASS.
Current Bin SHA C6E97739A10EBB438F43BE6A7DFBE8958A4C09025BB2E9D07D1B1B74D09EE767;
staged Client remains016DAFA5. No body loader invocation/registration or runtime
acceptance added this run. EnsureClassBodyModel and render range consumers
remain required before class activation.

### Separate base-class body/material descriptor — 2026-09-14

GrowLancer/compat/ClassBodyDescriptor.h now expresses the verified five base
body model identities, native-loader-observed mesh/bone/action counts and
per-mesh skin/hair mapping. Head mesh0 uses hair777D; head mesh1 and all other
body mesh0 use skin772C. The contract explicitly labels IDs as S21 evidence,
NOT native registration IDs. Exact source class byte07 is accepted; all other
values remain unsupported by this base descriptor (not a declaration that GL
evolutions do not exist). Full class/equipment/server activation remains required.

C++14 x86 W4/WX constexpr test passes512 input class values, all6 valid mesh
material bindings, invalid mesh indices and invalid part. This descriptor is
not yet wired to the gameplay model registry. Capacity audit found current
private tail32 entries,31 used by skill models/controllers. Five bodies cannot
fit without explicit allocation/bounds migration; no MAX_CLASS or legacy IDs
were increased to make room. Next reserve a distinct body range while keeping
effect-family predicates and legacy model numbering unchanged.

### Actual character-to-model selector writer recovered — 2026-09-14

Character render13E2DBA..13E2DD3 reads class byte12, calls1418FBA and writes
returnedAL to selected model byteA4. Full helper1418FBA..1418FF1 returns low
nibble base +16*(stage-1). Full stage helper141848E..14184E4 takes high nibble,
returns1 for zero, otherwise scans bits3 down0 and returns highestSetBit+2.
Therefore base7 yields selector7/23/39/55/71 according to highest high-nibble
bit. This is not popcount and not the native5.2 base*2+change-up formula.
Native character rendering currently uses that older formula; it must not be
globally replaced when mapping GL. The following special selector copy branch
is guarded base4 (Dark Lord), so is not a GL-specific override.

Source byte7 gives selector7: known skin token7725 becomes772C and hair7776
becomes777D, both registered to the previously verified Class108 files. This
closes that conditional unevolved source-byte material chain. Which character
creation/persistence paths can supply each class byte, valid GL evolutions,
uppercase Class308 texture handling and higher-stage skin table remain open.
Verifier now pins writer and both complete helpers, with16 high-nibble algebra
cases explicitly not runtime character/evolution validation.

### S21 renderer token substitution and file table — 2026-09-14

Full selected renderer branches133146D..133155F and1331854..1331946
check tokens7725/7776, skip drawing when source model byteA5 is nonzero,
otherwise sign-extend model byteA4 and add it to the token. This proves a
stored selector, not automatic selection from the body filename. The branches
also have optional color-state work under byteCA; not all renderer behavior
or alternate render paths are closed by these selected branch pins.

Six concrete registration blocks now verified against source strings/calls:
skin_Class108.jpg at772C and773C (relative selectors7/23), hair_Class108.tga
at777D/779D/77AD/77BD (relative selectors7/39/55/71). These are registered
possibilities, NOT proof of which GL evolution writes each selector. No hair
selector23 registration was inferred from the pattern. All numbers are hex
resource IDs, whereas the relative selectors above are decimal.
verify_s21_body_texture_dispatch.py records these checks. Next find the actual
GL caller writing model byteA4 before choosing native Skin/material adapter.

### Source body texture token dispatch changes next mapping step — 2026-09-14

Full SHA-pinned18BBA1A..18BBD33 decode reveals that native S21 body texture
registration does NOT always load the filename stored in the mesh. Lowercase
`ski` byte prefix returns token0x7725, and lowercase `hair` returns0x7776,
both jumping directly to return instead of ordinary file load. Therefore
skin_Class108.jpg and hair_Class108.tga being present/decoded/uploadable does
NOT yet prove those exact files are sampled for every GL body/evolution.
The earlier native upload tests remain valid asset compatibility evidence,
not proof of final S21 material assignment. Uppercase Skin_Class308 does not
match the lowercase ski byte tests; follow its other checks before declaring
the complete path. Do not normalize case before emulating this source logic.

Verifier verify_s21_body_texture_dispatch.py pins helper plus five body caller
argument sequences18C79D8..18C7A70 (0x2600/0x2901). Caller loops five tiers,
16 classes, excluding Class2 for helper14191B7 classes3/4/6/7. Renderer token
resolution and final per-class texture tables remain to trace before private
body registration. Native5.2 BMD also substitutes BITMAP_SKIN+Skin for its
skin token; copying a physical texture ID into all meshes would bypass that
contract. No runtime mapping was changed based on this new evidence.

### GPU readback matches native decoded pixels — 2026-09-14

Texture probe now checks bounded glGetTexImage readback against the actual
native BITMAP Buffer (not Pillow output), after confirming expected dimensions
and component count. PID12168 exited0 on NVIDIA GeForce GTX1060 3GB,
OpenGL4.6.0 driver582.66: all3 buffers byte-equal; hair RGBA alpha min0/max255;
all upload/readback glError0. RGB logs mark hasAlpha0; their alpha extrema are
unused sentinel values, not a claimed alpha range. Missing candidate filename
was rejected and bitmap count/tracked bytes restored after unloads.
UTC records2026-09-13T19:57:36.334..398Z in RuntimeQA/Client/GrowLancerRuntimeQA.log.

Compiled/staged SHA016DAFA5BFF5D46E369552D5CECA478F93CDC3EB34061414E2AD2E2283F85D46.
This validates actual native decode-to-GPU transfer including hair alpha, not
the S21 sampler/blend/lighting/body pose or gameplay-context visual result.
Missing-file cleanup is one failure case, not exhaustive allocation/context
failure injection. No global texture settings or production files changed.

### Actual native texture upload PASS — 2026-09-14

RunBodyTextureProbeQA is QA-only, exact RISE_GL_BODY_TEXTURE_QA=1 opt-in,
offline startup exit. It creates a hidden16x16 WGL test surface with the same
16-bit RGBA/depth pixel-format flags as native CreateOpenglWindow, without
that function's global game handles/foreground/error dialogs. This adapter
only supplies a context; native Bitmaps.LoadImageFile does all decoding/upload.
Texture references acquired by the probe are released with force=false before
context teardown; no shared registry reset or new renderer is involved.

Full isolated build/stage SHA53A43945D8BB1DC9D72E98D005D18EB223D7C3C998B47584BFC85E56BA2F7810.
Initial compile lacked ZzzTexture.h; included it and rebuilt PASS. Hidden test
PID24084 exited0, UTC2026-09-13T19:25:16.260..264Z: hair GPU64x64, both skin
GPU256x256, all loaded=1/glError=0. Native bitmap count and tracked byte counter
returned to baseline after3 non-forced unloads. This is not comprehensive
heap/VRAM leak measurement or visual parity. Pixel colors/alpha, GL driver
identity and gameplay-context comparison remain open. No user UI automated.
RuntimeQA/Client/GrowLancerRuntimeQA.log SHA at capture:
1F547171CD1163273C34F23D20A6F358BE239B2D7E131DE00F7C0BAE103E6B6C.

### Body textures staged with native file-layout cross-check — 2026-09-14

GlobalBitmap::LoadImageFile dispatches jpg/tga to OpenJpeg/OpenTga and those
replace extensions with OZJ/OZT. OpenJpeg skips24 bytes before libjpeg;
OpenTga reads dimensions at16/18,32-bit pixels from22 and swaps B/R while
flipping rows. Thus the previously named candidates are actual native filename
resolution inputs, not merely guessed extension equivalents.

stage_gl_body_candidates.py now hash-pins and stages the3 textures unchanged
beside the10 converted BMDs. Hair is64x64,32-bit with observed alpha0..255;
both skins decode as256x256 RGB using Pillow after native24-byte prefix.
Dry-run/apply/repeat apply PASS; different outputs/reparse outputs refused.
Pillow/header-span validation is NOT native libjpeg/GL upload acceptance.
No runtime Client files or shared texture IDs were changed this run.

Ownership follow-through: dynamic LoadImageFile increments Ref on matching
cached filename; UnLoadImageFile decrements Ref and frees on zero unless
forced. A native texture probe must release only references it acquired and
must not use force=true on shared assets. A valid current GL context is
required before upload; the offline geometry path has none and cannot be
reused unchanged to claim texture success.

### Actual native geometry load PASS — 2026-09-14

Moved probe out of gameplay hotkey polling to explicit offline QA startup.
RunBodyLoadProbeQA returns-1 when disabled, otherwise exits the process after
geometry tests before WinHook/window/game connection. Environment is child-
process-only and restored in the launch shell; no UI/authentication automated.
ReleaseGpuMeshes exits immediately for constructor-initialized null NewMeshes.

Full isolated build/staged SHA26D1E6D26F52D1279A60B8FFDB6832B8E46CEBB0BFFED6DA58FFB76A599A9D3E.
Attempt from Bin exited0xC0000135 (missing dependency), no probe log. After
hash-guarded staging into complete RuntimeQA/Client, PID964 exited0 and logged
all10 filenames loaded=1, expected meshes/51bones/1action, then10 destructor
returns. Log GrowLancerRuntimeQA.log SHA at capture:
EFF5A25E54B14F3741423E346B4816CB90E479B8E8F3653D82D23B0F1FD8AE70;
records UTC2026-09-13T18:20:10.355..360Z. Log is append-only and may later change.
This proves real native Open2/Init/Release geometry execution, not textures,
rendering, leaks, game startup or full class integration. No permanent process
left by the successful offline test. Latest4D8B9C rollback and olderEF1595 retained.

### Native geometry-only body probe compiled — 2026-09-14

QA HandleRuntimeQAHotKey now performs a once-per-process probe only when
RISE_GL_BODY_LOAD_QA is exactly1. It opens ten fixed BodyCandidates paths
through real BMD::Open2 on local BMD instances, logs native counts and then
logs return from scope destruction. No global model slot/class is activated.
No textures are loaded: Open2 leaves IndexTexture uninitialized, while Release
reads mesh.Texture slots and may DeleteBitmap. The probe assigns every slot
BITMAP_SKIN, the native no-delete sentinel, immediately after successful Open2.
This avoids claiming ownership of shared textures; destructor performs native
geometry cleanup. It is not a general-purpose malformed-file loader.

Before opt-in launch, rerun stage_gl_body_candidates.py --apply to enforce
source/output hashes and parser checks. Probe itself does not rehash files.
Do not mutate candidates between preflight and load. Source guard verifier
PASS and full isolated Win32 QA build PASS, candidate SHA256:
4D8B9CEFB5640D57F80D5CFF130C80A80403996EA5F2CC090E3553B799C08A45.
Not deployed/launched this run; no body-native-load/release runtime evidence
yet. Returned destructor is not by itself a leak test or rendering acceptance.

### Isolated body conversion candidates — 2026-09-13

Native Open2 was inspected directly: decrypt branches support0C and0E, not
encrypted S21 0F. Do not hand original0F files to this native parser. New
stage_gl_body_candidates.py defaults to dry run, pins source hashes from this
contract, verifies mesh/skeleton records, preserves all plaintext including
unknown tails and uses the existing converter with expected hash/model name.
Different existing outputs are refused; target reparse ancestors are refused.

Executed dry run and two apply passes:10 converted0C candidates verified under
ExMain_RISE_PC/Tests/GrowLancerBuild/BodyCandidates/Data/RISE/GrowLancer/ClassBody.
Second apply only compared identical files, no rewrite. Output bytes decrypt
to exact S21 plaintext; output hashes are printed by the staging command.
No textures copied yet, no model IDs reserved and no RuntimeQA/Client files
changed. This is conversion readiness, NOT native Open2/load/render success.
The native resource loader already uses Open2, so the next probe must reuse
that real implementation, not install a second BMD loader.

### Serialized body ABI checked with x86 compiler — 2026-09-13

`test_body_serialized_abi.py` extracts the actual five mesh typedefs from
ZzzBMD.h and checks native vec_t/vec3_t declarations. MSVC x86 C++14 W4/WX
confirms Vertex16, Normal20, TexCoord8, Triangle36 and Triangle_t2=64 bytes.
The first fixture incorrectly expected Triangle34 and failed; corrected to36
(the final bool plus alignment). The body inspector already advances64 per
serialized triangle, so no mesh offsets were changed or silently reinterpreted.
This verifies isolated typedef layout, not all project include/packing state
or actual BMD::Open execution. No runtime source/header was changed.

### Body-to-Player bone prefix matched — 2026-09-13

The body inspector now pins actual RuntimeQA Player SHA0CC3D22D... (full
constant in tool) and compares every body bone name/parent/dummy record with
its Player index. All51 indices match for all10 BMDs. Mesh vertex and normal
node indices are in range and none references a dummy in either skeleton.
This closes the observed51-versus60 header-count concern at index/header level,
not full pose/geometry correctness. No retargeting or global skeleton replacement
is justified by this evidence. Armor base uses nodes25/34 additionally; other
mesh node sets are emitted by the tool, not assumed identical between tiers.

Native ZzzCharacter body render invokes RenderPartObject with GlobalTransform
false; ZzzObject.cpp11273..11280 then submits o->BoneTransform to BMD::Transform.
ZzzBMD.cpp265/268 and300 index this matrix directly by vertex/normal Node.
Thus shared Player pose matrices, not independently played body action0, are
the relevant binding path. Other render modes and live poses remain untested.
Four parser fixtures pass, including negative/out-of-range node rejection.
Unknown trailing bytes, native loader execution, texture decode and final GL
class selection remain open; no source assets or runtime files changed.

### Decoded body mesh dependencies — 2026-09-13

`inspect_gl_body_dependencies.py` enforces the10 recorded BMD hashes before
read-only LEA decryption, then reads mesh records using native serialized
strides16/20/8/64 and bounded texture fields. All models report51 bones,
one action/one key;45 non-dummy bones. Helm models have2 meshes, others1.
This is offline parsing, not execution of native BMD::Open or proof of bone
binding against the60-bone Player skeleton. No new renderer/loader was added.

Texture names come from mesh fields. Candidate source files under Data/Player:

| Mesh texture | Candidate | Observed SHA256 |
| --- | --- | --- |
| hair_Class108.tga | hair_Class108.OZT | e13026b066e69982cf8cddc0f1a8d9558fda0673b2a52b82f56f920614ea4a7b |
| skin_Class108.jpg | skin_Class108.OZJ | 37d230ab5e35c76220b54df28abb52f619e1e86f9a4cb9a96c77b8c54c344a89 |
| Skin_Class308.jpg | Skin_Class308.OZJ | cb264c223a1e6691ccb7caff885cdb35c3aeac10381262ddd808fe0d92cb5ba6 |

Both helm variants reference the same hair texture; base/Class3 body meshes
reference their respective skin. Extension resolution is only a candidate
until actual loader follow-through/decoding is checked. After parsed skeleton
records, plaintext retains NONZERO trailing bytes of lengths6/4/4/8/16 for
base helm/armor/pant/glove/boot and10/4/8/8/12 forClass3. Do not silently
strip them or label them zero padding. Native observed Open consumes its
records then discards Data, but S21 trailing semantics remain unverified.
The inspector reports these bytes verbatim. Three synthetic parser tests pass
for truncation, invalid flags and preservation of unknown tails; no asset
compatibility/runtime acceptance follows from those tests.

### S21 body registration recovered — 2026-09-13

Pinned main18CCBEC..18CCDF7 registers16 base indices. Base7 loads the five
Class08 bodies at370A/375A/37AA/37FA/384A and Class308 bodies at
372A/377A/37CA/381A/386A (hex). Helper14191B7..1419201 masks base with0xF
and returns true for3/4/6/7, skipping the entire Class2 block. This establishes
that GL must join the second-tier registration exclusion; it does not yet prove
the live character selector or DB evolution representation. Shared loader's
index<10 format is `%s0%d.bmd`; filenames are not guessed from class names.

`verify_s21_body_registration.py` checks full selected linear decodes,15 calls,
the helper branches and10 read-only BMD headers/hashes. All10 headers are0x0F;
no decode/load, texture completeness or staging is claimed. Fixed-argument
model_registry.json has no HelmClass records: these loop-derived IDs require
this separate evidence, not an assumption that absent registry means no model.

Observed SHA256, Data/Player source (not yet hash-enforced asset staging):

| Asset | SHA256 |
| --- | --- |
| HelmClass08.bmd | 09128f0b627d718967fccd14a3ad30ecab86659c4a7317ef4a3b8f824951d668 |
| ArmorClass08.bmd | 85f7a0a36aa7b863a7f443bd7aa05dd710f6fd7e929a5a764b268df90b0d1350 |
| PantClass08.bmd | d653929caf8fc5c6853b0b9e6ddf126c8c141317db7a3792ae03bf1c729edb72 |
| GloveClass08.bmd | 931cec27df3d0452ce4bdc54a3a05405fab7cdb3f236536978254139ab563f71 |
| BootClass08.bmd | f0ac15c7fa664ac09611bb648c58cb78c128b064cff4b2e703cc9489eff22bc6 |
| HelmClass308.bmd | aec2e24b89a10301ef3e0e956fd96952d92e7b4d25143e2e0b18bdcb7941e080 |
| ArmorClass308.bmd | b675f19c1c7928e72df464516d2ae674f5e3a6e8bb718c57c345c9616a84669a |
| PantClass308.bmd | 0467e29f19c2fe0dab7cb7b9b8fc4b7c89da9e07690e6a722fa2c9bac30afe4c |
| GloveClass308.bmd | bb082b650ba5bc8efc65f35a1af6c46d4a946c7d38b33f571cca9759a9177db0 |
| BootClass308.bmd | 5a2d432e6cbb8d63833303ea0cec8cd41b6f34b9ca5a9e374f27f1bb9fe22680 |

Higher registration blocks18CCDF7 onward use Class4 twice; leave their class
selection/evolution semantics open until traced. No S21 runtime ID is assigned
directly into the native5.2 enum by this audit.

### Body-model capacity migration boundary — 2026-09-13

`verify_class_skin_boundary.py` checks the real GetSkinModelIndex byte slice,
all15 body registration expressions and texture tier indexing. These THREE
consumers share MAX_CLASS. Changing7 to8 renumbers11 of21 tested legacy
base/evolution combinations, but registration also shifts: renumbering alone
does NOT prove an incorrect rendered legacy model. Pinning only the selector
to7 would break that agreement. Migrate registration, selector and textures
together, preserving existing resource consumers and verifying actual assets.

For hypothetical base7 with second bit only, the current selector's else
branch returns7 even with capacity8. The registration loop would additionally
load second-tier slot15, since it excludes only MG/DL/RF. This is a concrete
selector/registration policy mismatch, NOT evidence that GL supports that
evolution or that filenames Class208 exist. Source class/evolution eligibility
and hash-pinned body assets remain required. MODEL_BODY_NUM is independently24;
fitting24 slots is not proof of semantic mapping or successful asset loading.

No runtime code changed: an attempted selector-only patch was rejected before
writing because CharacterManager.cpp has non-UTF8 bytes. Follow-through then
established that selector-only change is insufficient; do not convert the
whole file or retry that isolated edit as a fix. Current QA binary unchanged.

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
DefaultClassInfo::Init zeroes rows and originally range-rejected class7;
the 2026-09-15 isolated capacity reservation now accepts the index but has no
verified class7 row, so it still retains zero stats.
verify_class_table_migration_boundary.py checks these concrete source hazards.
The capacity-only reservation is not activation: keep class7 create/login/cast
rejected until loader grammar, defaults and rates are supplied and tested
alongside the existing seven classes.

- Client CharacterManager.h GetBaseClass masks0x7. The existing conversion
  ChangeServerClassTypeToClientClassType extracts base from serverByte>>5,
  second evolution bit4->clientbit3, third bit3->clientbit4. Base7 fits those
  fields; capacity is not the missing implementation.
- Client GetCharacterClass originally handled only base0..6 and initialized
  its result to0, so base7 silently resolved to Wizard. The 2026-09-15
  isolated mapping now returns private selector18 for unevolved base7 and
  explicit unsupported255 for unproven evolution bits. IsFemale includes
  source base7; SS6 base0..6 selectors and enum IDs are unchanged. This
  corrects identity handling but does not register class7 body/UI/equipment,
  create/login or third-master types. GetSkinModelIndex still uses the SS6
  MAX_CLASS stride and must not be used as proof of a GL body model.

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

- GS DefaultClassInfo.h and both CustomStartItem headers now reserve
  MAX_CLASS=8 and class index7 without enabling a verified default row.
  DB class constants are a distinct namespace and create/load whitelists
  still reject the new class.
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
2026-09-15: added the isolated, unused
`ExGameServer/GameServer/RISE/GrowLancerAbilityGauge.h` numeric adapter
with the exact pinned S21 Lua expression and a focused x86 compile-time
test (including fractional result `1.65` for four unit attributes). The
S21 Lua returns its number, while SS6 `CharacterCalcBP` casts a full sum
to `int`; effective input/stat-option mapping, rounding and assignment to
native `MaxBP` remain separate unverified bridges. No class7 switch, global
constant, item/skill validation or live server dispatch was changed.
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
# Actual partial-material rollback/retry — 2026-09-14

Offline PID17552 exit0, 00:53:29Z, executable SHA256
3E2E1C062D8701CFA5CDD0CBA76B2FEC9E52F136476E4AF6E812653758900F10.
New opt-in RISE_GL_BODY_ROLLBACK_QA=1 (inside existing texture + adapter opt-ins)
temporarily selects BodyRollbackFixture cwd, containing pinned helm and hair only.
Native first material loads independently; second material is absent. Actual
EnsureClassBodyModel fails, NumMeshs=0/allocation flag reset and bitmap counters
return to baseline. Cwd restored before logs and retry. Same head slot then loads
and caches successfully alongside remaining four body parts; final native table
destruction restores baseline. No asset renaming, no original/production writes.
This tests one concrete material failure, not all failure paths or GPU rendering.
Source acquisition order supports partial-first-ref interpretation; internal
per-load telemetry was not added. Private shader upload remains open.
# Private GPU boundary mapped, runtime pending — 2026-09-14

EnsureClassBodyGpu now bypasses only the legacy registry wrapper's MAX_MODELS
limit, not the native renderer or loader. It accepts only five descriptor slots,
uses native LoadMeshToVAO/UploadAllToGPU and checks live VAO/VBO/IBO handles and
nonzero counts. Preserves caller vertex-array/array-buffer/element-buffer bindings.
Both fresh and cached body loading call it; failed GPU records release for retry.
Native shader-disabled path remains legacy. Global model wrapper stays unchanged.
Build CCE52A3F PASS, not staged or runtime-tested. Actual shader setup, buffer
sizes/data, cache handle identity, deletion and drawing remain required; IsShader
was false in previous hidden probes, so previous adapter PASS did not test GPUs.
# Actual shader-enabled lifecycle PASS, no draw acceptance — 2026-09-14

PID26296 exit0 at01:58:36Z using compiled/staged CD89D71D executable.
Opt-in RISE_GL_BODY_GPU_QA=1 enables real glewInit and OGL330::Init within
the hidden WGL probe; shader enabled1 confirmed. Five actual adapter body loads
produce matching native GPU vertex/index buffer byte sizes and stable cached
VAO/VBO/IBO identities. After model-table destruction, all6 VAOs and12 buffers
are no longer live GL objects. Bitmap counters restore baseline, and earlier
missing-second-material rollback/retry still passes with shaders enabled.
Shader teardown precedes context destruction. No gameplay, no scene draw,
no pose/vertex-content parity proof or GPU allocation-failure injection.
Next inspect native material/bone draw contract and test nonzero caller bindings.
# Native draw prerequisites and binding regression — 2026-09-14

PID10084 exit0, 02:31:08Z, staged executable0DC3CBBF: all5 bodies preserve
nonzero caller VAO/array/element bindings on fresh and cached loads. Previous
sizes/cache/deletion and missing-material rollback still PASS. No draw occurred.

Native New_ModelBMD.cpp::AddBoneTransform stores a borrowed matrix pointer and
model identity. AddMeshCommand requires that same model and non-null pointer;
material comes from IndexTexture[newMesh.Texture], with explicit texture override.
Native CGMShaderBMD::Render binds the texture, selects alpha test for4 components,
uploads mesh-local bone matrices via SendIndexBone then issues glDrawElements.
The borrowed matrix storage must survive queue consumption. SendIndexBone applies
BodyScale/BodyOrigin only for transform=true, and RequestScale has distinct effect
on basis versus translation; do not reconstruct these semantics in another renderer.

IMPORTANT OPEN GAP: native HideSkin checks run only for BITMAP_SKIN/HAIR sentinel
materials. Our concrete private texture IDs intentionally avoid global class-stride
remapping but also bypass those checks. Scoped body draw/appearance handling must
preserve intended hiding before final integration; current loader PASS does not
establish equipment/hide parity. Next trace queue flush and camera matrix setup.
# Native draw smoke test, NOT visual parity — 2026-09-14

Actual opt-in body Animation(frame0/action0 single key) -> native mesh queue ->
native renderer/bone uniform/glDrawElements tested on hidden diagnostic camera.
EndScope flush occurs before pose storage expires; borrowed pointer cleared after.
First16x16 test PID3272 exit1 produced no head/glove samples. Enlarging only the
test surface/viewport to256x256 yielded PID7984 exit0, GL errors0, samples
280/478/492/156/352 for5 body parts. Build/staged44513388, prior binaries retained.
GPU deletion and bitmap baseline still PASS. This confirms rasterization plumbing,
not visual shape/texture/alpha/animation parity; no image comparison performed.
Private HideSkin/equipment behavior and actual class caller remain unimplemented.
# Scoped appearance policy (not yet a gameplay caller) — 2026-09-14

ClassBodyAppearance.h separates Unsupported / KeepEquipment / Hidden / PrivateBase.
Source byte must be exactly7 and part0..4. Existing equipment remains selected when
equipped and rendering enabled; otherwise base body selected unless HideSkin.
Native evidence: ordinary empty-slot assignments in ZzzCharacter.cpp12218..12254;
ordinary-player render-equipment-off fallback9560..9564. RenderPartObject writes
HideSkin, owner scale/light/position and uses owner bone transforms when not global.
All verified base meshes are skin/hair, so scoped base hiding skips the full body
part; this must not be extended to arbitrary equipment with mixed materials.
28672 combinations pass x86 C++14 /W4 /WX static_assert. No final native class ID,
equipment mapping or gameplay call added. Caller must retain Chaos Castle, NPC,
transformation and other special dispatch. Next audit native generic render tail.
# High-level native caller is not a pure body renderer — 2026-09-14

RenderPartObject routes through RageFighterEffect before item/body effects.
MonkSystem.cpp helper checks action IDs alone, not class; DARKSIDE_READY changes
owner angle and reads actor action-table timing. ATT_UP/HP_UP_OURFORCES adds
body passes. Passing a real equipment PART_t also permits cloth mutation/deletion.
RenderPartObjectEffect depends on option/custom systems even for ordinary levels.
Custom-wing lookup is map iteration (not raw item-index array), but this does not
prove the complete high-level path safe for the new private-body context.
RenderPartObjectBody generic fallback calls native RenderBody without HiddenMesh.
Do not blindly invoke this wrapper from the offline body probe or declare parity.
Next map actual owner fields to scoped native Transform/RenderBody, retaining
native renderer while separately accounting for hiding/selection/shadows/buffs.
No gameplay wrapper was added during this audit; existing native behavior unchanged.
# Scoped ordinary base submission compiled — 2026-09-14

SubmitClassBaseBody uses native BMD::Transform and RenderBody, not a replacement
renderer. Requires exact source class7/part0..4, owner and caller light; visible
submission additionally requires >=51 caller bones and native resource readiness.
HideSkin/alpha<=.01 suppress before loading. Owner scale/position/contrast/light
flag mapped; BoneScale temporarily1 and restored. Generic base RenderBody arguments
preserved (no explicit HiddenMesh), native internal shader flush preserved.
Caller owns equipment selection and extra shadow/buff/edge passes; terrain lighting
is caller-provided, not rederived here. No final class/gameplay activation.
Build AA570C17 PASS after declaring existing native BoneScale; not staged/tested.
Next exercise actual OBJECT fixture, hide/alpha guards and scale restoration.
# Actual owner submission regression PASS — 2026-09-14

PID14728 exit0, staged F0B93565.20 cases (5parts x visible/HideSkin/alpha=.01/
50-bone rejection) all pass1 using actual OBJECT and SubmitClassBaseBody.
Visible native samples280/478/492/156/352; all suppressed/rejected cases zero.
BoneScale restored from fixture1.75, bitmap counters stable and GL errors0.
OBJECT::Destroy inspected: does not delete BoneTransform; borrowed pose pointer
explicitly cleared after use. Native shader queue pointer also cleared.
Prior upload/cache/deletion tests still PASS. Resources already cached, so this
is not allocation instrumentation. translate=false single-key fixture only,
not gameplay/visual parity. Next close cached sampler policy before final caller.
# Private material cache validation — 2026-09-14

GlobalBitmap filename cache does not include sampler: cache hit increments Ref
and returns existing ID unchanged. Private body loader now validates identity,
components, live texture and actual min/mag/wrapS/wrapT; never changes sampler
of a shared texture. Restores prior binding after readback. Cached mismatches
fail without reacquiring refs; fresh mismatch releases acquired refs/model.
Actual PID25908 exit0, staged954E1111:5 cached filter/wrap mismatch/retry PASS,
20 owner cases PASS and GPU/bitmap cleanup restored. Fresh preloaded conflict
rollback still needs runtime fixture. Per-call GL query cost not profiled in game.
