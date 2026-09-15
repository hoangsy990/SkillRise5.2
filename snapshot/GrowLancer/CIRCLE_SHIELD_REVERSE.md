# Circle Shield (skill 272) Season 21 client reverse

### Bounded native upper-arm allocator outcome QA — 2026-09-15

The S21 post-buff traversal proves four subtype-12 upper-arm mono particle
calls per character render. Native 5.2 `CreateParticleInternal` can still
reject a request when effect options are off, the shared FPS/distance budget
rejects it, or no native pool slot is available. The close-camera budget
is unconditional inside 650 world units, so rejection is only a possible
cause of missing owner pixels, **not** a demonstrated cause. The QA-only
owner/skill272 probe now counts successful native indices out of four per
traversal (maximum 64 records per F7), plus ready/bone gate, camera distance,
FPS and effect options. It preserves all four source calls, null S21 owner,
stock SS6 allocator/budget and buff state. A clean local F7/F11 is needed to
decide whether emission reached the pool; a count of four still does not
prove visible paired-shoulder pixels.

`verify_circle_persistent_source.py` and
`verify_circle_emission_qa_contract.py` PASS; isolated QA/nonQA Win32
compile/link PASS. New private staged QA Engine SHA-256
`EEF727AE8377D2844DDAAEA05C8F7847B45C22E6F348DA6765461CADCBE7CFDF`
has old `E3EC7999...` rollback and complete private Data verifier PASS.
Hidden offline GPU QA on this exact stage PID10688 exit0 reports 64x64
CPU/GPU-equal mono bytes, GL error0, valid1 and stable release1. No new
Circle gameplay cast, real buff receipt or owner visual PASS follows.

The hash-pinned selected S21 particle renderer `0x1635D58..0x1635DFC`
calls its 11-argument native sprite helper `0x18E8B0C` with mode 4.
The callee tests its final mode bit 4 at `0x18E8EDB` and applies the
rotation to axis 2 (Z). Native 5.2 `RenderSprite` likewise transforms the
world position through `CameraMatrix` and constructs `(0,0,Rotation)`
for the sprite quad; subtype12 passes its own light, texture and rotation.
`verify_circle_mono_renderer_contract.py` PASS. This narrows the render-mode
hypothesis; it does not verify exact projected pixels, depth or batching.

### Native resource and targeted pool QA checkpoint — 2026-09-15

Hidden offline private QA PID44356 and follow-up PID34608 both exited0:
native bitmap32983 loaded the exact hash-staged source `firehik_mono01`,
64x64 RGB CPU/GPU bytes matched, min/mag were linear, S/T were clamp-edge,
GL error0, and bitmap memory/count returned to baseline after unload.
The historical staged QA Engine SHA256 13F48071... also added QA-only F11/map
`circleMono` live subtype12 particle and `circleContact` live subtype3 joint
counts; `verify_circle_pool_snapshot_contract.py` PASS. This does not claim
actual shoulder pixels, emission cadence, pool exhaustion or gameplay buff
receipt. Those and matching-frame S21 parity remain OPEN.

## Isolated post-buff shoulder preview — 2026-09-15

The source-verified five flares/four `firehik_mono01` subtype-12 particles
are callable from the native character-render stage either when real status
216/221/222 is present or, only in `RISE_GROW_LANCER_RUNTIME_QA`, for three
seconds after an explicit Circle Shield F7 cast. The preview is bound to the
current live Hero and map; unrelated actors in the traversal cannot consume
or cancel it. It does not register a buff, send a skill packet, change
persistence, or demonstrate S21 server acceptance. The new Win32 QA candidate
built and the guarded asset+EXE stage dry-run verified hashes, but refused to
overwrite the older live QA process. Owner visual and pool checks remain OPEN.

## Persistent upper-arm pair source closure — 2026-09-15

The previously unattributed purple shoulder pair now has a direct S21 source
edge. The pinned client image (SHA-256
`6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637`)
decodes continuously from `13ECB52..13ECFBC`: character buff membership
`216/221/222` reaches **five** `0x7EF7` flare01 sprite calls and **four**
`0x8086` firehik_mono01 subtype-12 particle calls. Five `0x132EC63`
bone transforms use indices `20,26,35,25,34` in source order. The hash-pinned
original S21 Grow Lancer `HelmClass08.bmd` bone records name these `Head`,
`R UpperArm`, `L UpperArm`, `R Clavicle`, `L Clavicle` respectively. Exactly
two mono particles are emitted after each `R UpperArm`/`L UpperArm` flare;
their source owner arguments are null, while the flares own the character.
This is a persistent buff-render branch following Wrath in the same
`13DB9D7` character traversal, **not** the local cast root `0x5DD` and not
Spin Step. The owner photo is consistent with the upper-arm origin, but
matching-frame GPU pixels still require ingame QA.

The flare color uses one `rand()%15 / 37.5 + .4` pulse per traversal and
RGB `(0.5*pulse,0,1*pulse)`, scale2 and zero rotation. The mono has source
RGB `(0.3,0.38,1)`, scale argument0.6 and subtype12. Source subtype table
entries pin constructor `166844B`, update `16DB10E`, and billboard renderer
`1635D58`. Constructor life is `rand()%5+27`, size
`(rand()%72+52)*.01*.6`, rising velocity `(rand()%14+20)*.1`, rotation
`rand()%360`, alpha0. Whole-tick update fades after life<15, multiplies
initial RGB by alpha, shrinks scale, raises world Z by velocity and rotates
+3. The 16,340-byte source OZJ SHA-256 is
`C3E18C474BA5AE9F06B9E7159AFC0DAB34869B78EAB425CB5538B94FE0E7138F`;
the exact hash is staged only in CircleShield's isolated asset package.

RISE 5.2 now has a Circle-only private bitmap slot32983, hash-staged asset
and native loader with S21 `GL_LINEAR/GL_CLAMP_TO_EDGE`. The adapter uses
native `BMD::TransformByObjectBone`, `CreateSprite`, the private particle
allocator, whole-tick `MoveParticles`, and existing `RenderSprite`; it runs
only with live buff216/221/222 during native character rendering. It does not
reuse Wrath's bitmap ID/particle ownership or alter other SS6 subtype cases.
`verify_circle_persistent_source.py` PASS for image tables/call counts/bones/
asset/source, focused x86 `/W4 /WX` constexpr tick test PASS, and isolated
QA Win32 client build/link PASS (Bin SHA-256
`F78482281B189518048B3D718D18902D0AB424E5DEC14972AA9AA8648E342220`).
The active prior QA PID25016 still uses staged SHA4A606548; this new binary
and private asset have **not** been restaged into the running client. Actual
buff receipt, paired pixels, FPS cadence, repeated casts, map cleanup and
visual parity remain OPEN. Do not promote static/build results to owner PASS.

## Owner S21 purple-shoulder observation — 2026-09-15

The owner corrected the attribution of the native S21 gameplay crop
(temporary screenshot SHA-256
`6F0692ACB12765BD1E27EB3F3635268CD4FAACE64715FAFFB46254A5B2500F40`):
two purple fire-like lights at the caster's shoulders belong to Circle
Shield, **not** Spin Step. This is an owner-visible layer requirement. The
The direct source-owned emitter/attachment is closed in the section above;
the photo itself alone did not identify the primitive. The tempting purple filenames
`fire01_purple.jpg`/`fire02_purple.jpg` (IDs `0x8310`/`0x8311`) have known
consumers in separate post-1000 handlers; no Circle spawn edge to them is
proved. Do not stage those filenames merely because their color matches.

Status: `IN_PROCESS` — cast dispatch/action/root/model, direct cast children
and persistent upper-arm emitter are recovered. Separate buff-contact joint subtype3 constructor/update is
now transcribed in native ZzzEffectJoint.cpp, but its receiver is NOT wired yet;
the previous broad child-coverage statement did not cover this receiver.
Both controller branches and direct Shiny04/flare01 primitives are transcribed;
runtime visual parity QA remains.

## Identity and dispatch

Focused action-order audit `verify_circle_receive_action_order.py` pins
the entire local `0x10E3E77..0x10E3FF2` branch as send-only: no root5DD
construction or `0x1327DE8` setter. Received block
`0x12CB2F7..0x12CB392` constructs caster-owned controller5DD FIRST,
then calls the preserving setter with primary187 and conditional316.
The isolated native receive adapter now creates the controller before
`SetAction(&caster,286,true)` for proven primary187→286; auxiliary316
remains unmapped. The direct F7 QA preview uses this receive adapter for
visual probing and is not proof that S21 local cast created a root.
QA/nonQA isolated Win32 links PASS; owner buff pixels/GS authority OPEN.

- `SkillList.xml` identifies base skill `272` as `Circle Shield`, Grow Lancer
  tier 1, level 220, mana 100, AG 50, range 0, buff index 216 and PvP-only.
- The local cast dispatcher compares `0x110` and enters `0x10E3E77`. That path
  performs the cast throttle/packet send and does not create the visual root.
- The received-skill dispatcher selects jump-table slot 22 and enters
  `0x12CB2F7`. It marks the caster character active, sets action `187`, passes
  action argument `316`, creates `0x5DD subtype 0` owned by the caster object,
  and plays sound `1111`.
- Sound 1111 is registered at `0x18E2D4F` as
  `Data\Sound\CircleShield.wav`.

## Root controller 0x5DD

### Separate received buff-contact path — integration OPEN

The cast root is not the sole incoming trigger. Pinned main dispatcher
`12FE380` selects opcode `75h` via table `130505C`, entering `1300320`
and calling `129E990` at `1300369`. The latter reads actor indices from
bytes4/5 and6/7, and a big-endian buff selector from bytes8/9. Its diagnostic
string at `1B99864` says `버프이팩트 전달 %d` (buff-effect delivery).
Selector216,221,222 enters `129F04C`; it computes direction between the
two resolved objects and calls CreateEffect at `129F14B` with model5DD,
subtype1, owner second object. Remaining emissions in this handler still
need full tracing; this is NOT a completed native receiver adapter.

Read-only BuffEffectManager.xml SHA256
`201452C00D9D5CE0DB83820EF48EC4317A6654864C0BE9176A6389694A4AD6B0`
names216 Circle Shield and275 Bleeding. Thus the numeric275 branch in
this handler is not evidence for Clash skill275 or its learning source.
`verify_packet_effect_lead.py` pins dispatcher and selector evidence.
Do not copy opcode75 or source offsets directly into SS6; establish the
native server-authoritative trigger/packet adapter and paired-target QA.
The supplied protected `IGC.GameServer2_R.exe` is x64 and its pinned
CodeView `RSDS` record points to a `Season 20\GameServer\Release` PDB path
(GUID `30c7611f-171b-4f28-9b0d-8dfc4377f4aa`, age3); no PDB exists in
the supplied server tree. This embedded path is build provenance, **not**
proof that the actual binary implements S20 instead of S21 skill semantics.
The S21 `BuffEffectManager.xml` status216 is a data contract, not decoded
proof that this particular server emits the opcode75/contact packet. Keep
real receiver integration and GS authority OPEN; the isolated F9 adapter
only previews exact client visuals, without manufacturing server semantics.

The complete216/221/222 branch ends129F60C (unconditional exit), before
unrelated buff333 at129F60E. It creates exactly one5DD/subtype1 then three
80E3/subtype3 joints, scale40, owner second object, target position second
object. Using A for first object, the three start/angle pairs are:

| Call | Start | Angle |
| --- | --- | --- |
|129F28A|A.Position+(0,0,100)|A.Angle with Z minus135|
|129F447|A.Position+Rotate((100,50,0),A.Angle)|A.Angle with Z minus90|
|129F604|A.Position+Rotate((-100,50,0),A.Angle)|A.Angle with Z plus135|

Subtype3 constructor dispatch15DEA92/99 ->15DECA1 sets life100, tails10,
caller scale, random sign +/-1 atfieldA20, velocity10, reverseUV2. Its full
update/render contract is still being traced. Native BITMAP_FORCEPILLAR
currently handles subtype2 specially and all others as legacy life7/tails5;
This described the pre-contact implementation gap. The new subtype3 branch
now sets those fields separately and runs whole ticks with pre-turn tail
matrix, target+120Z homing,3D contact distance and literal speed brake.
It rejects null/non-live native targets as an explicit compatibility safety
guard. Pool reuse, renderer parity and incoming receiver remain OPEN.

- Constructor switch handler: `0x147FA97`.
- Update switch handler: `0x1575B60`.
- Render callback: `0xA48742`, an exact no-draw stub returning false.
- Subtype 0 lifetime is 30 ticks. Its constructor transforms local offset
  `(0,-50,0)` through the controller angle, adds the owner's position, then
  emits 20 records through the 14-argument SS21 `CreateJoint` family
  (`0x15C6594`) with type `0x80E3`, subtype 2, scale 20 and caster owner. The
  SS21-only final argument is zero. Each joint uses random angle X `[0,89]`,
  angle Y 0, angle Z `[0,359]`; the position passed to the joint call is the
  randomized transformed displacement from the owner point. Type `0x80E3` is registered at
  `0x18D1C0B` as `Effect\force_Pillar.jpg`.
- The subtype-2 constructor branch is `0x15DEB72..0x15DEC9C`: scale is the
  supplied 20, velocity starts at 0, lifetime is 18, max tails is 10, target
  position is copied then raised by 100 on Z, reverse-UV mode is 2 and light
  is `(0.2,0.45,0.65)`.
- Its update is `0x1612F8F..0x1613037`: velocity increases by 2 each tick;
  while pre-decrement lifetime is above 10, the joint turns toward the stored
  target position with turn value 10. Movement PRECEDES this handler (see
  ordering audit below); tail creation follows it. Cleanup is native Live=false.
- RISE's native `force_Pillar.OZJ` is byte-identical to SS21 (SHA-256
  `921F24FEA42D3182130BACAED610720640F71B5D46155A775D54F5BA61691898`).
  The port therefore uses `BITMAP_FORCEPILLAR` directly and adds only a
  subtype-2 guarded constructor/update branch; legacy subtype behavior remains
  unchanged.
- Subtype 1 lifetime is also 30 ticks and has no constructor burst.

## Root timed emissions

### Joint renderer audit (2026-09-13, scoped static evidence)

Renderer1618F44 visits the fixed500 pool plus entries from10F03DA overflow
manager. Live/NumTails>0/RenderFace and pass-selection gates1618FF7..1619057
match the native gates for Circle (byOnlyOneRender0). Blend type1 calls
18E7137, whose18E7158/18E715A arguments are ONE/ONE, matching native
EnableAlphaBlend; no extra ForcePillar-specific texture scrolling is selected.

Generic color16193FA..1619460 multiplies Light by S21 scalar+A4C.
The base constructor15C6850..15C6858 initializes that scalar to1; Circle's
subtype2 constructor15DEB72..15DEC9C and update1612F8F..1613037 do not
modify it. Thus the reviewed local chain agrees with native glColor3fv(Light).
External scalar writes are not established by this local-chain observation.
The auxiliary-pool selection for Circle is resolved below; other joint types
are outside this audit.

UV base1619640 computes (NumTails-j)/(MaxTails-1); reverseUV2 at161AA94
replaces both U values by1-U. V stays0/1. Face-bit2 at161ABE2 selects quad
161AC85..161AD92: current[0],current[1],next[1],next[0], with
(U1,0),(U1,1),(U2,1),(U2,0), matching native RenderJoints.
No new renderer adapter is warranted by these reviewed branches.

verify_circle_joint_renderer.py pins19 instructions plus scalar1 against
the module SHA. It is a regression of reviewed evidence, NOT proof of whole
renderer control flow, tail construction, overflow behavior or GPU parity.
Follow-up tail audit found an actual native FPS correction conflict:
S21 subtype2 sets MaxTails10, branches15DEC9C->15DEF1C->15E21B9, and
the common epilogue ONLY caps to50. Native formerly divided10 by FPS factor
after its switch, changing the UV denominator and retained trail length even
though Circle updates now run at whole ticks. The native FPS division is now
bypassed ONLY for ForcePillar/subtype2. Other SS6 joints retain their behavior.
Verifier now pins21 instructions and the native source guard. Isolated
Win32 build/link PASS; no GPU acceptance implied.

Non-blur tail15E37EF increments NumTails, clamps toMaxTails-1, shifts all four
vertices backward from indexNumTails-1 (15E3818..15E38FC), then creates
the negativeX/positiveX pair from +/-Scale*.5 transformed by the passed matrix
and translated by Position (15E3901..15E3B68), matching native CreateTail.
The negativeZ pair continues15E3B6C..15E3C9E, and positiveZ pair
15E3CA2..15E3DD0; both use Scale*.5, the supplied matrix and Position,
stored at vertices2/3 respectively, matching native CreateTail.
Important: S21 initial tail at15C6D4F reads storedScale before subtype2 assigns
the callerScale; native does the same. Do not silently move that initialization
without establishing the allocation/reset contract and actual reuse behavior.

### Circle fixed-pool selection (supersedes earlier overflow uncertainty)

CreateJoint15C65D6 scans500 records;15C6609 calls predicate10F0332 with
type/subtype/owner. Regardless of owner,10F0353 accepts ONLY type7FCE
(plus its subtype/owner restrictions). Circle80E3 returns0 through10F037B,
so15C6610 skips auxiliary allocator10F02CE. It uses first non-live fixed
record, or returns at15E21D9 if all500 are live, like native MAX_JOINTS500.
This is not a general overflow fallback for Circle. Do not add extra capacity
or replacement policy based on the generic renderer's auxiliary visitor.

Fixed-pool allocation15C6622 onward overwrites explicit fields, not the whole
record, before initial tail reads storedScale15C6D4F. Native has the same
ordering. Startup/global resets and real cross-type reuse remain runtime
coverage gaps; this does not justify inventing a Scale reset or moving the
tail initialization. The verifier now pins31 instructions plus constants1/.5,
including the type predicate and selected Z-pair operations.

### Joint ordering audit (2026-09-13)

Pinned dump function15E5838: 15E58DB gates generic movement,15E5900 builds
the pre-turn AngleMatrix and15E596D rotates (0,-oldVelocity,0), then adds
displacement without a render-rate multiplier through15E5A2A. Handler1612F8F
adds2, homes by10 only at life18..11. Homing1327059 uses CreateAngle/TurnAngle2
for Z then X, without FPS scaling. Tail1618B1F receives the SAME pre-turn
matrix, not a recomputed matrix.1618B45 decrements lifetime then1618B58 kills
at negative: life18..0 means19 updates,8 homing updates.

RISE subtype2 now has a private per-pool-slot whole-tick accumulator, reset
on every CreateJoint allocation. Shared GrowLancerCircleJoint.h preserves
move/acceleration/turn/tail/decrement order; native vector/angle/CreateTail APIs
remain underneath. No changes to legacy subtype0/1 or global FPS values.
Native MoveHumming math is used with fixed turn10 (the existing wrapper
would otherwise scale it by FPS again). Joint GPU renderer, actual pool
reuse/stress and visual comparison remain OPEN; helper tests do not prove them.

Correction2026-09-13: native formerly emitted child on every render-rate update
and an extra pair at bone42. Anchored1575B60..15766EE proves both subtypes:
pair window life>19 && life<25; child window life>10 && life<20 && life%3==0.
The latter resolves bone42, but then independently transforms local(0,-50,110)
by controller angle and adds controller position for child5DE. No Shiny/flare
call occurs in1575F0F..1576138. Constants1B70300=-50,1B4FA94=110.
Native now emits five timed pairs and three children (when owner/model valid),
with no extra bone pair, and controller uses integer ticks/Timer0.
Earlier prose below claiming another pair at bone42 is superseded.

Both update branches are structurally parallel and use the same recovered
constants. Direct float-VA reads prove Shiny04 light `(0.3,0.7,1.0)`, flare01
light `(0.25,0.18,1.0)`, local emission offset `(0,-60,100)`, and visible
child light `(0.45,0.35,1.0)`.

- At remaining lifetime 20 through 24, the controller transforms a local
  offset and creates one native effect type `0x7F6B` (`Effect\Shiny04.jpg`),
  subtype 0, owned by the controller.
- The exact Shiny04 subtype-0 constructor at `0x147FF27..0x147FFC6` sets
  lifetime 15, scale 3, alpha 0, angle X/Y 0 and angle Z to world time modulo
  360. Update `0x1576851..0x15768AA` adds alpha 0.2 while lifetime is at least
  10, subtracts 0.1 below 10 and destroys on negative alpha. The isolated
  effect-pool carrier now implements those exact fields instead of its former
  generic ten-tick symmetric envelope.
- The same window creates one sprite type `0x7EF7`
  (`Effect\flare01.jpg`) through S21 sprite allocator `0x172760A`, with scale
  10, the controller as owner, subtype 0 and rotation `WorldTime % 180`.
- `flare01.OZJ` is byte-identical to the existing SS6 asset loaded as
  `BITMAP_LIGHT`. The port now calls native `CreateSprite(BITMAP_LIGHT,
  scale 10, owner controller, rotation WorldTime % 180, subtype 0)` directly;
  the duplicate dynamic texture slot is deliberately unused.
- At remaining lifetime 11 through 19, every third tick, it resolves owner
  model bone 42 and emits another Shiny04/flare01 pair at the transformed bone
  position.
- Each branch finishes by creating visible child `0x5DE` with light
  `(0.45,0.35,1.0)` and the controller subtype.

## Visible child 0x5DE

- Model registration: `0x18D2B0E` ->
  `Data\Effect\circle_shields.bmd`.
- Decoded v0F identity: `circle_shields.SMD`, 1 mesh, 1 bone, 1 action;
  embedded texture `circle_shields.jpg`.
- Constructor `0x147FE13`: subtype 0/1 lifetime 10, scale/raw field A0 1.0,
  mesh selector `-2`, and angle Y `+70`.
- Update `0x15766F3`: subtype 0/1 transform local movement `(0,-3,0)` by the
  object angle and add it to world position; scale/raw field A0 decreases by
  `0.05` per tick.
- Render callback `0xA48749` copies effect angle to the model and calls the
  native BMD mesh path with mesh selector `-2`, effect alpha and mode argument
  2. No replacement renderer is required.

## Asset provenance

- `circle_shields.bmd`: SHA-256
  `6A985AC9EBF517DD86C164F23DBF6D0F66DD527BFEF1D3144B75D77168FA1A98`.
  The isolated v0C conversion is
  `79D241ACBAB24D8B31D8DB357D2D75B453431A6AF6D27CA48B2CC2A971DC7BDB`
  and decrypts to the pinned plaintext SHA-256
  `C0FD780A4F48F06CB514AD9F808BFD9722C6A0C41CFE6AC479284697D087E400`.
- `circle_shields.OZJ`: SHA-256
  `F5ED70BAF0746EB40CC7687A49242D2F0E6DAEB26B3B4C442C65CF9225B83A0C`.
- `force_Pillar.OZJ`: SHA-256
  `921F24FEA42D3182130BACAED610720640F71B5D46155A775D54F5BA61691898`.
- `shiny04.OZJ`: SHA-256
  `568A27A2B3F0E8B8004FDA7DA9CBC9F1C3A83E933AC159554CB5F3A573CC3B13`.
- `flare01.OZJ`: SHA-256
  `874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8`.
- `CircleShield.wav`: SHA-256
  `3B2A719E6A69DBEEC590AF595A98D96C32E3E8262315E51D47CD48EBE020E7B3`.

All source files remain read-only. The BMD conversion and every copied hash are
verified only under the isolated Grow Lancer test client.

## Still open

- Nine insertion sites in function13F93C1 decoded through140794C all load
  [ebp-1754], add3254 and call1316DE7:1400C87,140514C,14052F2,
  1405529,1405692,1406095,14066F2,1407048,1407661. For first site,
  producer1400AD5 calls13F8ED7 and stores its result at1400ADD;
  that producer scans400 character records and uses embedded object+3254.
  These are character-object insertion call sites, not direct sprite-pool
  insertion evidence. Full per-branch payload identities remain unproven.
  Pinned verifier now also checks27 caller instructions and4 producer facts.
  Remaining unclassified direct leads:DD24A0/DD24BE/DD24DC/DD24FA,
  11A3C88. No absence claim about indirect calls or shared objects.

- Caller18CC8C2 attribution narrowed: decoded from actual prologue18CC6A9
  through18CC90B. Two16-iteration loops address gridBE9D2B8 with16byte
  cells, read first object at cell+4, follow object+354, clear list then
  populate by object+5C catalog lookup and invoke13167E1. This is NOT the
  sprite manager's fixed1000 records atA4C1130, stride3D8 (1726E56).
  The linked-grid shape corresponds structurally to native ObjectBlock[256]
  traversal, but no recovered symbolic scene function name is asserted.
  This caller cannot be used to attribute auxiliary-list payloads to Circle
  sprites. Other insertion callers remain unclassified.57instruction verifier
  PASS; no runtime adapter added and no acceptance gate upgraded.

- Member+3D0 writer/arithmetic audit:962D30 supplies1 to96320D;
  962905 negates the operand,9631E0 uses LOCK XADD and returns the prior
  value. Thus the cleanup compare==1 is the last-reference transition,
  with concrete virtual targets still unresolved. Anchored function1316DE7
  constructs a24byte local payload (pointer wrapper,scalar,argument,flags,
  float duration), forwards its owner to A8122A, and calls13189BD on+3D0.
  Anchored13169F0 tests payload+11 flag, subtracts its argument from+14,
  and removes expired records via131892E. Getter/insert/erase symbolic names
  are structural interpretations, not source symbol recovery.
  Direct call candidates for1316DE7: DD24A0/DD24BE/DD24DC/DD24FA,
  11A3C88,1400C87,140514C,14052F2,1405529,1405692,1406095,
  14066F2,1407048,1407661,18CC8C2. These are unvalidated xref leads,
  not proof Circle creates a payload.46-instruction verifier PASS.
  Next anchor caller18CC8C2 and the adjacent render-list call18CC8DD;
  check caller type/arguments before attributing this feature to sprites.

- Sprite member+3D0 cleanup follow-up:9B4068 is a no-op wrapper;
  9694F8->9693F9 returns this.9B3343 null-terminates the linked chain,
  saves each next pointer and calls966734. Node payload starts+8;
  96704F calls968A5C with deleting flag0. Payload destructor9679DC->
  967954 cleans payload+4 via960742, which conditionally calls963437.
  966673 then routes one node through969C9B (count<<5,32bytes) to946140.
  9B436B restores self-linked head pointers and zero count. New verifier
  now pins33instructions plus3scalars PASS. No guessed container port.
  Additional decoded tail963437 calls962D30 on control+4, branches when
  returned value==1 to virtual slot+4, then9635C2 repeats on control+8
  and invokes virtual slot+8. This is consistent with shared ownership,
  but concrete payload type/virtual targets and Circle-specific writers are
  not yet established.962D30->96320D arithmetic must be decoded before
  naming the exact refcount operation. Full sprite cleanup remains OPEN.

- Sprite visibility audit2026-09-13: function1726FCF..1727014 scans1000
  records and sets byte+B=1 only when96139E reports Live. Renderer1727079
  reads the same+B; its +/-0.1 envelope uses1.0/.2 bounds. Native CheckSprites
  similarly sets all live sprites Visible=true; gameplay ZzzScene calls it
  after RenderEffects/RenderBlurs and before sprite rendering. No owner
  dereference occurs in native RenderSprite(OBJECT*,OBJECT*) itself.
  verify_circle_sprite_visibility.py PASS15 pinned instructions,3scalars,
  native guards. This closes the selected producer/consumer field meaning,
  not all S21 scene call ordering; numeric callers18F49CF/18F4F76/18F693B/
  18F6B48 are still leads only.
  Separate S21 owner-retirement1727015 matches+34C, calls Live setter and
  clears owner. Setter13167BB additionally calls9B3D94->9B436B on member+3D0.
  9B436B calls9B4068/9694F8/9B3343, resets two header links to themselves,
  and zeroes a count-like dword. Container payload/writers remain unproven;
  do NOT declare it equivalent to native Live=false or invent a new container.
  No runtime/source implementation change was justified by this audit.

- Controller owner-reuse follow-up2026-09-13: controller update dereferences
  Owner Live/BoneTransform/CurrentAction/Type. Native EffectDestructor clears
  Live and Owner; DeleteEffect(type,owner,-1) filters live owned effects and
  delegates destruction, with skill-pool fallback. Added skill-local
  RetireCircleCharacterEffects before character reactivation; it returns for
  a live character and deletes only kCircleShieldControllerModel owned by the
  dead slot. No new allocator, renderer or global destruction semantics.
  Child kCircleShieldModel uses its own transform/lifetime; Shiny carrier
  updates its own alpha and renders with &effect as sprite owner. No blanket
  child deletion introduced. Remaining sprite-owner visibility/pool ordering
  must still be verified separately. Source guards and extracted joint test
  PASS; QA x86 build/link PASS2936BEB411D2FB10D858DD5F37BF46066F59DB15294DBF99FC02EAC51F92BA64.
  Not staged, not owner visual accepted, no runtime controller-reuse test.

- Dead character slot reuse correction2026-09-13: DeleteCharacter overloads
  and ClearCharacters mark Live=false without deleting force-pillar joints.
  CreateCharacter selects a free slot and CreateCharacterPointer sets Live=true;
  a remove/recreate sequence before MoveJoints can therefore bypass the
  contact3 Live guard. Added only DeleteJoint(BITMAP_FORCEPILLAR,o,3) when
  entering CreateCharacterPointer with !Live, before reactivation. This is a
  native compatibility safety correction, not a recovered S21 generation rule.
  Same-live-key refresh and other joint types/subtypes/targets are unchanged.
  Source guard PASS; test_circle_target_reuse.py compiles the exact native
  DeleteJoint body and actual reuse call against six mock joint categories,
  both dead-slot and live-refresh cases, /std:c++17 /W4 /WX PASS. Initial
  standalone fixture lacked NULL definition; corrected fixture then passed.
  Full isolated QA x86 build PASS, hash355F282A6445E54A329AAB73175C077BE40401E2979F12A755B1A9EBC591CBFE.
  NOT STAGED: preview remainsE6FED7D0. Actual scheduling/reuse QA remains OPEN.
  Circle controller Owner and child-effect slot reuse are separate unresolved
  paths; this joint-only fix must not be called full ownership cleanup.

- Native pool-boundary audit2026-09-13: CreateJoint scans existing MAX_JOINTS,
  resets private Circle remainder before marking a free slot Live, and clears
  then assigns Target. DeleteJoint matches Type/Target/optional SubType and
  only clears Live; Circle contact does not own/delete the target OBJECT.
  MoveJoints supplies the pool index only for live slots. Contact3 rejects
  null/non-live targets before dereference and returns before legacy updates.
  Source guards in verify_circle_pool_boundary.py PASS;31-instruction pinned
  renderer verifier also PASS. This does NOT close identity reuse: Target is
  a raw pointer and Live alone does not distinguish a newly reused character
  slot. Target generation/removal ordering, pool exhaustion and gameplay
  cleanup remain OPEN; no speculative global lifecycle fix was introduced.

- UV/draw-mode audit closed for Shiny04's selected defaults: A13A00 writes
  metadata(1,1,1.0); allocator1726E0F stores columns/rows1 and interval1.
  Renderer172732D compares columns/rows to1 and selects17274DA full UV
  (0,0,1,1), bypassing animated atlas division. Mode4 is NOT a blend mode:
  sprite primitive18E8E9F tests bits1/2/4 for rotationX/Y/Z respectively;
  zero-vector ctor960443 precedes it, so4 sets only Z to supplied rotation.
  Native ZzzOpenglUtil.cpp RenderSprite uses Angle(0,0,Rotation) and default
  full UV; this selected behavior needs no additional renderer/atlas adapter.
  Blend selection is the separate sprite subtype0, already traced additive.
  This closes only default UV and rotation-axis mapping, not sprite visibility
  producer, owner deletion, all draw passes or gameplay visual parity.

- Sprite wrapper172760A forwards to allocator1726C50; arg20 is subtype,
  arg24 is draw mode stored8C, arg28 is metadata copied to90/94/98.
  Shiny caller metadata defaultA13A00 is(1,1,1.0), mode4, subtype0.
  Render manager1726F11 selects additive for subtype0;1726F87 clears Live
  for pass0/2 after draw. Native manager has corresponding branch/cleanup.
  Render1727079 changes frame multiplier by+.1 visible, -.1 invisible with
  bounds.2..1. Native generic invisible branch incorrectly adds; corrected
  ONLY private kCircleShinyBitmap to render-step envelope, preserving SS6.
  This multiplier differs from effect alpha; metadata UV/mode4 consumer
  and native flare01 private identification remain open.

- Renderer follow-up:15A1514->15AE9BF, subtype0->15AE9DC..15AEAE8
  computes Light*Alpha and calls sprite172760A at15AEAE0. Owner argument
  pushed15AEAA4 is the Shiny effect itself, NOT effect.Owner. Native carrier
  now passes &effect only for CircleShiny. Sprite extra arguments0/4 and
  metadata remain to audit; no full renderer parity claim.
  Child update15766F3..1576835 confirms both0/1 move rotated(0,-3,0),
  scale-=.05 without clamp. Child now initializes Timer0 and uses private
  whole-tick lifecycle; no non-source scale clamp. Native ClCompile PASS.

- Re-audit2026-09-13: prior native Shiny04 alpha clamping was NOT in S21.
  1576851..15768AE adds.2 at life>=10, subtracts.1 below10, sets life0
  only if alpha<0, never clamps alpha itself. Corrected private carrier to
  shared UpdateCircleShinyTick and whole-tick lifecycle; constructor Timer0.
  Test x86 /W4 /WX PASS fourFPS cases: six additions peak1.2, ten subtractions
  including life0 ends.2 at16 updates; negative-alpha branch also tested.
  Native ClCompile PASS. This supersedes earlier static-complete wording;
  renderer/owner/visual parity still require separate proof.

- Runtime comparison of the guarded SS6-native `BITMAP_FORCEPILLAR` subtype-2
  joint, exact Shiny04 carrier and native BITMAP_LIGHT sprite against SS21,
  including tail width, UV direction and FPS-normalized acceleration. Static
  constructor/update/render/lifetime mapping is closed.
- Server-authoritative PvP target validation, AG decrease effect and duration.
- Recipient class/skeleton mapping in SS6 and isolated runtime comparison.
