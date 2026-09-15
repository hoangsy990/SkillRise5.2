# Harsh Strike (skill 276) reverse evidence

Status: `IN_PROCESS`. The client visual controller, blur, children, assets and render passes below are statically recovered and implemented. Server hit timing, target reaction and runtime parity remain open.

## Identity and cast dispatch

- S21 skill ID: `276` (`0x114`), class field `GrowLancer=1`.
- Caster action: `185` (`0xB9`).
- Local caster branch: `0x010E36D7..0x010E38FA`.
- Remote/received branch: `0x012CB122..0x012CB1B7`.
- Native root creation: `CreateEffect` at `0x143E57C`, type `0x5E0`/1504, subtype 0, caster position/angle/light, caster object as owner, no target index.
- Local dispatch creates the action/root/sound immediately. The received branch skips the local caster and creates the action/root only for other actors, so one client owns one visual root.
- Sound buffer: `0x45C`/1116, played locally after root creation. Registration `0x018E2E8A..0x018E2EB6` binds it to `Data\Sound\HashStrike.wav`.

The optional `C1:35` packet builder at `0x010E379F..0x010E385A` is gated independently before the visual cast. Its gameplay meaning is not yet classified and is not treated as part of the visual contract.

## Root controller `0x5E0`

Constructor branch: `0x014803D8..0x01480425` inside native `CreateEffect`.

- lifetime: 40 ticks (`+0x6C = 0x28`)
- phase/counter: 0 (`+0x70 = 0`)
- scale: 3.8
- controller animation speed/state: copied from action 185 through the native action/model lookup
- owner: caster object
- renderer: handler `0x00A4881A` is a return-false stub; the root is an invisible controller

Update branch: `0x01577014..0x01577D6C`.

- It immediately destroys itself through `0x13167BB` when the owner is no longer in action 185.
- It resolves the caster model from `owner + 0x5C` at
  `0x01577047..0x0157706E`, then independently resolves the controller model
  from `effect + 0x5C` at `0x01577073..0x0157709D`; unresolved records stop
  that update path.
- The apparent controller-model dependency is now closed. At
  `0x015774B9..0x0157751D`, S21 evaluates the caster action into the shared
  bone buffer. It then calls animation on type 1504 at
  `0x01577542..0x015775A6`. Model 1504 has no static load and the model manager
  creates a zero-action record; native `BMD::Animation` returns immediately
  when `NumActions <= 0`, leaving the caster bone buffer unchanged. The later
  bone-8/5 transforms therefore consume caster bones, while applying the
  controller scale 3.8 and controller origin. Model 1504 is an intentional
  empty transform carrier, not a missing visible BMD.
- It samples the cast animation with a six-step denominator and reconstructs blur endpoints from controller bones 8 and 5. Bone 33 is also sampled in the animation preparation path and remains to be named semantically.
- The shared endpoint offset is local `(-30, 0, 100)`, rotated by the controller angle and added to both bone positions.
- The blur frame window is exactly `2.0..4.0`; six samples are generated.
- Each sample calls the nine-argument S21 blur allocator `0x15C01CC`. Its first eight arguments map exactly to native SS6 `CreateObjectBlur(Owner, p1, p2, Light, Type, Short, SubType, iLimitLifeTime)`: owner/controller, endpoints, white `(1,1,1)`, type 11, `Short=false`, subtype/group 3, lifetime `-1`. The ninth SS21-only byte is zero at the Harsh Strike callsite, so it needs no compatibility behavior.
- The S21 pool has 1000 records of size `0x386C`; recovered fields include owner `+0x0C`, type `+0x04`, light `+0x14`, lifetime `+0x08/+0x20`, subtype/group `+0x3864` and the final byte `+0x3868`.

### Timed child creation

The controller phase counter prevents duplicate children.

- Owner animation frame `[5.2, 5.8]`, counter 0: increment counter, create one `0x5E1` at rotated local offset `(0,-120,90)`, then one `0x5E2` at `(0,-180,90)`.
- Owner animation frame `[5.3, 6.0]`, counter 1: increment counter, create a second `0x5E2` at `(0,-380,90)`.
- Child calls use native `CreateEffect`, subtype 0, no owner and no target index.

## Child `0x5E1` / `h_strike_wind01`

Model registration: `0x018D2B43`, loader `0x18BBEE0`, runtime ID 1505, `Data\Effect\h_strike_wind01.bmd`.

Constructor `0x0148042A..0x014804DF`:

- lifetime 8
- scale 1.0
- X angle reduced by 90 degrees
- light initialized to `(1,1,1)`

Update `0x01577D71..0x01577F01`:

- move by rotated local `(0,0,-20)` each tick
- scale `+= 0.1`
- alpha `-= 0.125`, matching the eight-tick lifetime

Renderer `0x00A48821..0x00A48A5A` uses `0x42`, equal to native `RENDER_TEXTURE | RENDER_BRIGHT`, and submits four mesh passes:

- mesh 0 material `(1,0.3,0)`
- mesh 1 material `(1,1,1)`
- mesh 2 material `(1,1,1)`
- mesh 3 material `(1,0.95,0.25)`
- all passes use the current object alpha and blend-mesh arguments equal to
  their mesh indices (`0,1,2,3`)

Decoded BMD v0F identity: 4 meshes, 5 bones, 1 action. Embedded textures are `magic_ground2.jpg`, `flare01_red.jpg`, `impack03.jpg`, and `magic_ground3.jpg`.

## Child `0x5E2` / `h_strike_wind02`

Model registration: `0x018D2B78`, loader `0x18BBEE0`, runtime ID 1506, `Data\Effect\h_strike_wind02.bmd`.

Constructor `0x014804E4..0x01480599`:

- lifetime 7
- scale 1.0
- X angle reduced by 90 degrees
- light initialized to `(1,0.6,0.2)`

Update `0x01577F06..0x0157810E`:

- move by rotated local `(0,0,8)` each tick
- scale `+= 0.45`
- RGB multiplied by `0.9` each tick

Renderer `0x00A48A5B..0x00A48B0D` copies the current object light and submits
mesh 0 once with `RENDER_TEXTURE | RENDER_BRIGHT`, current alpha, blend-mesh
argument 0 and blend-mesh light 1.0.

Decoded BMD v0F identity: 1 mesh, 1 bone, 1 action. Embedded texture is `Damage1mono.jpg`.

## Lifetime and cleanup

Latest linked/staged candidate7650105CF09FC5EFA932FAA173A822D82BFE6F647ADF63FFEAFB3C3BF45A4F65
includes controller and private blur cadence fixes. Stage verifier confirms
2658base+79overlay+3rootData,EXE/player hashes and CRC; no gameplay acceptance.

Style0 renderer audit: actual prologue15C04B6. Selected branch15C07FE..15C09D9
computes RGB multiplier(Number-j)/Number then(Number-j-1)/Number; textureU
j/Number then(j+1)/Number. Four vertices are p1[j],p2[j],p2[j+1],p1[j+1],
with V1,0,0,1 respectively, matching native style0 branch.15C079D passes
primitive6 (triangle fan); selected pass uses additive18E7137 at15C0544.
No style0 renderer replacement needed for these reviewed arguments.
This does not prove GL state,actual sampled geometry or visual/pool parity.

Private cadence correction implemented: HarshBlurClock sidecar captures
owner-typeHarsh/type11/group3/style0 ONLY when allocating a free record.
Every allocation resets mode/remainder, including legacy; ClearAll resets
metadata too. No update-time owner dereference. Existing appends retain
lifetime and clock. AdvanceHarshBlur runs the unchanged lifetime/Number/tail
tick on integer accumulated FPS_ANIMATION_FACTOR only for private records;
legacy gets exactly one original tick. No object/packet/S21 layout copied.

Focused helper tests at .25/.5/1/2 factors PASS30ticks,dead-slot no update,
reused-slot remainder reset and legacy one-tick behavior. Producer-after-
consumer is modeled with an append counter only: tests do NOT execute real
CreateObjectBlur matching,tail arrays,allocator exhaustion or owner reuse.
ClCompilePASS. Controller and blur cadence changes are not linked/staged;
QA candidate90E643C5 remains unchanged. LowFPS batching may still differ
from interleaved native producer/consumer ticks; explicit runtime gate open.

Cadence follow-up (not yet corrected): native MainScene calls CalcFPS then
MoveMainScene once; gameplay calls MoveCharactersClient (and MoveBlurs /
MoveObjectBlurs) BEFORE MoveEffects. CalcFPS sets FPS_ANIMATION_FACTOR to
min(REFERENCE_FPS/FPS,2.5). Harsh producer now accumulates that factor,
but object-blur consumer decrements life/Number once per MainScene call.
Thus at factor.5 a30-consumer-update ribbon expires in15 reference ticks
while its producer advances30 reference ticks in60 calls. This is a source
cadence mismatch, not a measured wall-clock/FPS result.

S21 MoveBlurs prologue is15BFCBD; it calls15C030C at15BFE6E. Direct caller
13F4122 follows character updates, similar to native order. The earlier
13F40E0 window began midinstruction and is not a valid function anchor;
find the enclosing character-update prologue before tracing higher callers.
Next implementation should store private cadence metadata at allocation
(not infer owner type after pool reuse), reset it on every reused slot and
preserve all legacy blur paths. Need test consumer-before-producer order,
fractional remainders, append-without-refresh and exhausted/reused slots.

Object-blur pool audit:15C01CC searches live owner/group match before a free
slot, capacity1000. Group3 participates in matching; appending calls15C22D8
without resetting lifetime. Short=false/limit=-1 gives30 at15C02B8.
Append clamps Number599 at15C2542/15C254E; native600-element arrays and
599 clamp match. Move15C030C decrements lifetime AND Number before testing
life<=0; then resetsNumber0/Livefalse, otherwise shifts existing tail.
Native CreateObjectBlur/AddObjectBlur/MoveObjectBlurs match these selected
semantics. No new pool or global blur change warranted by this evidence.
verify_harsh_blur_pool.py PASS; scope excludes allocator stress/owner reuse.

Cadence remains separate: native MoveCharactersClient calls MoveBlurs at
ZzzCharacter.cpp6346, then MoveObjectBlurs. S21 direct caller15BFE6E invokes
15C030C. Need compare higher-level scheduling before assuming30 updates
means equal wall-clock lifetime. Harsh controller now emits on whole ticks,
so mismatched consumer cadence could change ribbon density at highFPS.
Renderer lead15C0500 selects7F07+11=7F12, additive18E7137; exact style0
UV/color/geometry audit remains open. Re-anchor renderer at prologue before
using full-body evidence (earlier04B8 window started inside an instruction).

Controller follow-up:15777033 action-mismatch branch clears Live through
13167BB and reaches15945F1; successful branch15777D6C has the same tail.
Private controller now initializesTimer0 and enters whole ticks; with a
continuously valid owner its life40 yields41 update visits. Invalid owner
still maps to native LifeTime0/destructor on the update tick. Focused test
adds eight lifecycle-only cases, not actual owner/pool execution.
Blur157773D6 loads raw action PlaySpeed, divides by six at157773EB,
subtracts raw speed at15777409. Whole-tick callback now supplies factor1.
Crucially1577748F..157774B3 gates Animation as well as blur allocation:
moved native Animation inside2..4 window, avoiding out-of-window shared
BoneTransform writes. Low-FPS owner-frame interpolation remains runtime QA,
not proven equivalent merely by controller tick counts. QA ClCompilePASS;
these controller edits not yet linked/staged.

Renderer follow-up: A48821..A48A5A writes fixed material colors WITHOUT
Alpha multiplication; Alpha is supplied independently for alpha/blend light.
All four calls A488B4/A4893E/A489C8/A48A52 use literal zero U/V.
Removed private runtime's extra RGB*Alpha and passed literal zero U/V.
verify_harsh_wind_renderer.py pins complete callback, four calls, absence
of multiply, eight UV-zero instructions and material constants.
Win32 link PASS; staged90E643C5A85934AA6A7E664A425C220E9B828FD31C1E96EDDCE5D938661F810F
includes both child lifetime fixes plus wind01 renderer correction.
No owner visual acceptance or GPU measurement performed.

Latest child correction: both selected update tails1577F01/157810E jump
directly to common15945F1. Neither5E1 nor5E2 is a413/7EF7 early-decrement
exception. Life0 still executes the selected update before destruction;
therefore8/7 initial lifetime produces9/8 update visits respectively.
1577EEE subtracts.125 from Alpha and1577F01 jumps directly to common tail:
no clamp.15778065 adds.45;15778098/CA/FC multiply RGB by.9.
Pinned constants1B4DF04=.1,1B50274=.125,1B4DF10=.45,1B4EDA4=.9.

Private wind01/02 now initialize Timer0 and use AdvanceWholeTicks in the
native MoveEffect dispatcher. GrowLancerHarshWind.h shares scalar updates
with the focused test; native angle/vector movement remains unchanged and
receives factor1 from that dispatcher. Removed invented wind01 Alpha clamp
and fractional pow(.9,factor). No global SS6 effect behavior changed.
Eight x86/W4/WX test cases (two children/fourFPS factors) PASS:9/8 visits,
wind01 terminalAlpha=-.125,exact scale/RGB sequence,single cleanup/no repeat.
Tests do NOT exercise actual vector transforms,pool allocation or GPU output.
QA ClCompile PASS; project3cpp/20headers PASS; not yet linked/staged.

- Root: explicit owner-action kill plus native 40-tick pool lifetime.
- `0x5E1`: native 8-tick lifetime with alpha reaching zero by the final tick.
- `0x5E2`: native 7-tick lifetime with multiplicative RGB fade.
- No target pointer, target bone, projectile collision or target-contact effect exists in this recovered client branch. Server damage/reaction evidence must be recovered independently before that absence is accepted for the complete skill.

## Native 5.2 mapping constraints

S21 local branch `0x010E385B..0x010E38FA` pushes `-1,185` and calls
`0x1327DE8` before constructing invisible controller `0x5E0`; receive
branch `0x012CB122..0x012CB1B7` does the same for non-local actors.
The common setter preserves outgoing action/frame on transitions and
does not restart the same action. Isolated native action `284` now uses
`SetAction(&caster,284,true)` instead of direct action assignment and
unconditional current/prior-frame zeroing. `verify_harsh_action_setter.py`
pins both contiguous dump blocks and native call order; QA and non-QA
isolated Win32 links PASS. This narrows the delayed-animation mismatch,
but owner action/effect timing and gameplay pixels remain OPEN.

- Do not allocate or render `0x5E0` as a visible BMD. Implement it as a skill-local controller using the native effect pool.
- The RISE adapter evaluates merged caster action 284 six times from one action-speed behind the current owner frame, transforms caster bones 8/5 through controller scale/origin, adds the exact rotated `(-30,0,100)` offset and calls native `CreateObjectBlur` type 11/subtype 3 only inside frame `2.0..4.0`.
- Register converted isolated copies of `h_strike_wind01/02`; raw S21 BMD v0F is not loadable by the current SS6 `BMD::Open2` path.
- Preserve the four-pass and one-pass render contracts exactly. Stored object RGB alone is not final material evidence.
- Do not add a target impact layer unless server/client runtime evidence proves one.
- The pinned process dump contains mapped module images, not the heap record
  reached by the model manager, so the live BMD record for 1504 cannot be
  enumerated from this capture. `model_registry.json` proves the surrounding
  direct loads 1502 and 1505/1506 but contains no 1504 row. A scan of all
  direct rel32 calls to loader `0x18BBEE0` also finds no literal 1504 load.
- The apparent 1504 setup at `0x01291A0E` is not a BMD loader. It resolves
  localized text through table `0x70A1B90`; the resolver's missing-key path
  formats `[Text] error index: %d end`. It is excluded from model provenance.
- Exhaustive little-endian reference classification found only renderer
  registration, effect construction/dispatch and unrelated numeric uses for
  1504; no `push 1504` loader call exists. This agrees with the empty-record
  carrier behavior above.
