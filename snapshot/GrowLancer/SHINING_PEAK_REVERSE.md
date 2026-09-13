# Shining Peak (skill 277) reverse evidence

## Animation caller and frame cadence audit

Clock identities now verified from the mapped PE import-name thunks:
IAT1B4C4A0=kernel32.GetTickCount;1B4C4A4=kernel32.Sleep;
1B4C9A8=winmm.timeGetTime. At18EF05D..18EF06E, prior BEACF44 is saved to
local928 then refreshed by GetTickCount. The previously decoded duration,
sleep-padding and carry remainder therefore use milliseconds: fixed40ms
simulation quantum (25Hz), including catch-up. This closes the unit question
below; it is not a claim of measured display FPS. Native ZzzAI.h also defines
REFERENCE_FPS=25, but animation frame advances by speed*FPS_ANIMATION_FACTOR
each native update. At factor1 the ordinary source/native stepping matches;
the final-window counterexample is therefore not uniquely a native bug.
Captured source speed273 and clip7keys are a snapshot, not proof those values
were used for a particular observed cast. A correction must reconcile actual
cast samples, not force all four emissions based only on this counterexample.

S21 scene dispatch now distinguished:18EE972/18EE98F maps scene3 to18F40BE,
while18EE984/18EE99F maps scene6 to18F2EBF. The latter was decoded from
prologue through RET18F3CE8: character traversal18F3BB1 precedes effect
traversal18F3C30. Character1424B34 loads a valid model (null exits), writes
object scale/action into model+74/+A8, calls1423BA4 at1424D62; its ordinary
path reaches CharacterAnimation14115BB at1423CA8. This closes the earlier
missing inner call edge; it does NOT justify changing native update order.
Outer18EE7E4 gates the update on globalBEACF88>=40 at18EE826, after a call
to108C83C0 outside this mapped main image. Units/producers/catch-up cadence
are not yet proven; do not assume that comparison alone establishes25Hz.

Native scheduling audit: MoveMainScene calls MoveCharactersClient before
MoveEffects. The character loop calls MoveCharacterClient -> MoveCharacter;
the ordinary animation path calls CharacterAnimation and then handles false
with PlayerStopAnimationSetting. MoveEffects iterates live native pool slots
and calls MoveEffect. Shining controller is NOT in that function's explicit
AdvanceWholeTicks list (its body/pin/spin/shockwave children are); it reaches
the generic private IsEffectType path, UpdateEffect then LifeTime decrement
by FPS_ANIMATION_FACTOR. Controller subtype0 checks action288 before reading
owner AnimationFrame. Thus ordinary completed action state is visible to the
later controller update; extending controller life alone cannot restore a
missed frame window. This is a source-level ordering audit, not runtime
coverage of every scene/character branch or S21 cadence parity.

Ordinary source action-end1423159 is now decoded through RET14232ED.
Actions185..194 satisfy111<=action<327, bypass exclusions159/213/313,
and call140AF0E at14232BC. Native appended284..293 were outside the stock
stop conditions. A private MODEL_PLAYER/valid-action-count branch now invokes
native SetPlayerStop for exactly284..293 in PlayerStopAnimationSetting.
No stock action ranges, animation keys or effect windows were changed.
An exhaustive16-bit action-domain constexpr test verifies the private predicate.
Source branch pins and isolated x86 build PASS; ingame action-end and pool
cleanup remain unverified. This corrects indefinite cast looping, not the
separate skipped-final-emission timing question.

Source caller14115BB (anchored prologue, decoded continuously to1411A56)
loads model Actions[CurrentAction].PlaySpeed at141162C using16-byte records.
It clamps negative speed to zero. Source action189 bypasses action-specific
modifiers for3/4,156/305,146,167 and159. Player model0xA94 bypasses model
modifiers; it is not model0x7FE. Thus positive Shining PlaySpeed reaches
132D0CD unchanged, unless one of status61/72/178/280 prevents the call.
Those status numeric identities are not translated here into native enums.

132D155 adds the supplied speed directly to owner+D0. Shining controller
157A63C reads that same field. Unlocked frame wrap uses integer modulo plus
fractional remainder; this agrees with native arithmetic, not a longer clip.
Native CharacterAnimation also reads Actions[CurrentAction].PlaySpeed, but
native BMD::PlayAnimation multiplies it by FPS_ANIMATION_FACTOR; ZzzAI.cpp
computes that factor as min(REFERENCE_FPS/FPS,2.5). This is a per-call
difference, NOT proof of a wall-clock speed mismatch: outer S21 update cadence
and both effect schedules still need comparison. Do not remove the native FPS
factor globally or force child emission from this partial finding.

Next source callers:142332A (conditional special path) and1423CA8 (return
value controls subsequent character transitions). The latter calls1422CF7
before CharacterAnimation, then on false may call1422EC8 depending on its
local flag. Trace these branches before concluding whether a missed frame
window can survive another animation cycle. Numeric call matches alone do
not prove the complete scheduler. verify_source_animation_step.py pins the
decoded caller and arithmetic against the full dump hash.

## Shockwave renderer correction (latest)

Pinned AD6 registration A1CF92/A1CF97/A1CF9C selects callback A72AD0.
Decoded from its prologue through RET A73122, not from numeric proximity.
Subtype6 misses comparisons1..5 and follows A7300C. It copies object Light
to model BodyLight without an Alpha multiplier, then calls RenderMesh968CB3
twice (A730C4,A73118), both mesh0, flags42, Alpha, blend mesh0,
BlendMeshLight, U/V offsets and texture-1. No texture80E9 override applies
to subtype6 (that is on other branches).

Private native shockwave now uses those two RenderMesh calls instead of
Draw_RenderObject, retaining Calc_RenderObject native transform preparation.
verify_shining_shockwave_renderer.py pins registration/branch/render calls
against the dump SHA and checks private source integration. This proves
the selected call contract only, not GPU parity or transform equivalence.
Isolated Win32 build/link PASS; lifetime and renderer included in candidate
2EEBB97F6EA2AEE8731688B8BAC4D5BD4E2049E9C4565E27F34D7E1821CA805F.

Status: `IN_PROCESS` - local cast identity, action, sound, both roots, all
four controller emissions, registered child models and their update equations
are pinned. Exact received dispatcher identity, target/contact behavior and
server line targeting remain open.

## Identity and local dispatch

- `SkillList.xml` row 277 is Grow-Lancer-only `Shining Peak`: required level
  92, damage 50, mana 8, BP/AG 5, distance 4, delay 0, required strength 600,
  element 6, damage skill.
- The local skill switch compares 277 at `0x10E359C` and jumps to
  `0x10E4271`. That branch submits the cast request, sets Dark Lancer action
  189 (`0xBD`), creates roots `0x5F6` and `0x5F3` on the caster, then plays
  sound `0x45D` (1117).
- Sound registration at `0x18E2EC9..0x18E2EF0` binds ID 1117 to the literal
  `Data\\Sound\\ShiningPeak.wav`; this is not a filename inference.
- Received-skill function `0x12CEA40` decodes the packet skill reference,
  resolves the caster character at `0x12CEBE6..0x12CEC20`, and stores its
  object pointer as `character+0x3254`. The comparison at `0x12CEEB0` is the
  exact base selector `cmp skillId,0x115` (277); its `je` at `0x12CEEB8`
  targets `0x12D09E7`.
- That base-277 branch sets action 189 through `0x1327DE8` and creates both
  `0x5F6` and `0x5F3` from caster object Position/Angle/Light fields, with the
  caster object as owner. Although the packet function also decoded a target
  index earlier, this branch never reads it: the recovered client visual chain
  is caster-owned and has no target pointer, target bone or contact child.
- A second branch at `0x12D2DFF` proves upgraded skill 2064 uses the same
  action and visual roots.

## Root `0x5F6`: immediate pin-light launch layer

- Constructor table `0x14B3224` maps type `0x5F6` to handler `0x1481494`.
  It sets lifetime 60 and raw float field `+0xA0` to 1.0.
- It rotates local offset `(0,0,35)` by the root angle and adds it to world
  position. The exact joint calls at `0x1481637..0x14817F9` map their first 13
  arguments directly to native SS6 `CreateJoint`; the extra S21 argument is
  zero. Call 1 uses subtype 2, scale 12.0 and adds `rand()%40-20` to angle Y.
  Calls 2 and 3 first add 80 to world Z, then use subtypes 3 and 4 with angle Y
  set to `rand()%360` and its negative respectively. All three retain the
  caster owner and use registered texture `Effect\\pin_lights.jpg`.
- Update switch selector for `0x5F6` is the default/no-op path and registered
  render callback `0xA49EDE` is a no-draw stub. The three constructor joints
  are therefore its visible output.

## Root `0x5F3`: staged controller

### Correction: pin-light subtype chain was NOT native-equivalent

Audit2026-09-13: calls148168C/148174E/14817EF select8073 with subtypes2/3/4.
Constructor compare15C7495 ->15DE358 ->15DE53A differs from legacy subtype0.
RISE previously applied subtype0 to all: random velocity15..74,life1..3,
MaxTails5,velocity+.1 and Light*.9. That is not the Shining Peak chain.

Recovered2/3/4 constructor15DE53A..15DE61E: callerScale12,reverseUV3,
velocity60,life20,MaxTails18,Collision0,angleX-=135,Light=(0,0,0).
S21 also writes word+A44=20 at15DE586; its consumer is still to be audited
before assigning any native field. Do not copy the S21 layout/offset.

Update1612773 dispatches all three to1612963. At pre-decrement life<=16,
once, sets Collision1 and Light(.6,.6,1), then multiplies all channels by.88
EVERY tick. Subtype2 angleX increments: life>=16:+32;15..10:-5;
9..5:+2; below5:no change. Subtypes3/4 share increments:life>=14:+30;
13..5:-15; below5:+3. Velocity does not change in these branches.
All branches join1612CB8->1618ADA native tail/decrement cleanup.
Float values were read from pinned data1B4E934,1B99910,1B4E998,
1B502EC,1B6C6B4,1B4FA9C,1B4E4D8,1B4EDF4,1B4E860.

GrowLancerPinJoint.h implements this scoped constructor/update with native
vector math/CreateTail underneath. Private accumulator resets each allocation;
MaxTails is NOT divided by render FPS for2/3/4. Legacy subtype0/1 remain
unchanged. x86/W4/WX helper regression12cases PASS:21 updates, color onset,
angle endpoints and expiry across fourFPS factors. Follow-up link/stage PASS:
candidate6653FBC797BCCA6C13B5E3FABEDDF8911687F82800FB5F8215274D35E99812E3.
No full-chain/visual PASS is claimed.

Follow-up renderer/field audit: selected body1612963..1612CB8 and complete
renderer1618F44..161AEB9 contain no direct+A44 field access. Other joint update
branches do read it, but this is not evidence those branches execute for8073.
No invented native field mapping added; external/indirect accesses remain an
explicit limitation. ReverseUV3 at1619524..1619580 sets U1=1-j/(MaxTails-1),
U2=1-(j+1)/(MaxTails-1), matching native mode3. Default face3 selects both
quads; pin2/3/4 do not enter the other-type per-segment color/scroll branches.
The default RGB multiplier is initialized1 by shared constructor, and selected
pin update changes Light, not that multiplier. Native color path remains in use.
verify_pin_joint_contract.py PASS12 pins/8 float constants and direct-field
operand audit. Initial-tail geometry, indirect external state and gameplay
comparison remain OPEN, separate from linked/staged PASS.

Pool follow-through: allocator15C65D6 scans500 ordinary slots, but calls
10F0332 before checking Live and can substitute10F02CE's record. Complete
predicate10F0332..10F037E requires local-player object ownership AND type7FCE
with subtype0/10/11/12. Since Shining uses8073, that special pool is excluded
even for the local caster; do not port this unrelated manager into pin joints.
The selected constructor ends15DE61E->15DE6E6->15E21B9; the final block only
caps MaxTails at50 and returns15E21F0, leaving18 unchanged and creating no
additional tail there. Earlier common-prefix initial geometry remains to audit.
General initialization15C670A/15C670E copies low16 bits of stack argument+38
to+A44, then the selected subtype overwrites it with20. This identifies its
input width, not its gameplay meaning or proof that external readers are absent.
Focused verifier now pins the special-pool exclusion and final cap as well.

Initial-tail follow-through: common15C6B25 sets NumTails0 and start-tail flag1.
The complete type suppression tests through15C6D20 exclude8073, so the four
vertices are emitted before subtype initialization.15C6D42 computes the matrix
from copied caller Angle;15C6D98/15C6EE0/15C702F/15C7174 rotate offsets
(-Scale/2,0,0), (Scale/2,0,0), (0,0,-Scale/2), (0,0,Scale/2), then add Position
into tail0's four vertices. Half constant1B4DF14 is verified0.5.

Importantly, these loads use the slot's stored Scale (+14), BEFORE the pin
subtype writes callerScale and subtracts135 from AngleX. Native CreateJoint
has the same four-vector construction before InitPinJoint. Do not move that
geometry after InitPinJoint or assume the initial orientation is AngleX-135.
The shown prefix does not initialize Scale: prior pool-slot contents/first-use
initialization still affect the seed geometry. This ordered structural match
does not prove identical pool reuse state across engines. No global scale reset
or guessed seed-tail fix was introduced; later tail overwrite/render visibility
must be checked before deciding whether this difference needs an adapter.

Tail-history follow-through:1618B13 pushes Blur=false and1618B1F calls15E2756.
Its false branch15E37EF increments NumTails/caps17, then descending-copy shifts
all four vertices from tail[j] to tail[j+1] before writing the new tail0.
Renderer1619486/1619489 loops j<NumTails; its quad path includes j+1 endpoint
loads (e.g.161AB79..161AB83). Native CreateTail(false) has the same shift order.
A deterministic token-history check shows the constructor seed survives at
index5 on fifth update (pre-decrement life16, first nonzero Light) through
index17 on update17; it is discarded on update18. Thus early darkness alone
does NOT prove the seed geometry is irrelevant. It remains an endpoint in the
history when light turns on. Actual raster contribution/UV edges and source
pool seed Scale still need evidence; do not label this a proven visual defect
or reset source behavior to callerScale simply for determinism.

Pool constructor evidence: static initializer8DDA8B pushes constructor10F040E,
500 elements, stride0xA64 and arrayA5E15B0 to19B8E3F. Complete element constructor
10F040E..10F04D1 initializes vector members at18/24/30/3C/48/58, an array of
204 vectors (12 bytes each) at70, and directionA0C. It has no scalar Scale14
write. Do not confuse vector initialization with a full-record memset.
Explicit deletion15E2604..15E269F writes only Live=0 on its two successful
branches; ordinary lifetime expiry1618B67 also clears only Live. These paths
preserve Scale across reuse, as does native DeleteJoint. Native Joints is a
static global POD array in ZzzEffect.cpp, but the precise S21 pre-initializer
image/loader state is not proved by the already-running mapped dump. Thus no
new first-use Scale constant is inferred from this constructor alone.

The remaining mismatch is history-dependent pool contents, not evidence that
the reviewed port omitted a Scale reset present in these S21 routines. Keep
the native lifecycle; runtime QA must include mixed prior joint types as well
as repeated isolated Shining casts to assess whether the seed matters visually.

Texture follow-through: provenance-pinned pin_lights.OZJ SHA A8C4DB19...FB2288
decodes from its 24-byte wrapper to RGB16x128. Per-column maximum channel values
are `[1,1,1,1,1,1,13,152,212,152,13,1,1,1,1,1]`. UV3 uses MaxTails-1=17,
not current NumTails. The seed-adjoining segment on update n is j=n-1, so
its U range is `[1-n/17,1-(n-1)/17]`. At updates8..10 it overlaps the bright
middle columns while the seed remains in tail history and Light is nonzero.
This rules out the proposed blanket explanation that only a black texture
border can ever cover the seed segment. It does not measure raster area or
prove visible error; even border texels reach1 due to the actual JPEG payload.
Native loading uses GL_LINEAR/GL_CLAMP_TO_EDGE at ZzzOpenData.cpp. The focused
`verify_pin_seed_texture.py` checks raw decode and UV-neighbor reachability,
not mip/driver behavior. Keep mixed-pool seed geometry as an explicit runtime
comparison item rather than rewriting matching source lifecycle speculatively.

- Constructor table maps `0x5F3` to `0x1481801`: lifetime 100, stage counter
  0 and raw float field `+0xA0` 0.1. Render callback `0xA49D70` is a no-draw
  stub. Update selector 0 enters `0x157A60F` and destroys the controller if
  owner action is no longer 189.
- The controller has four one-shot action-frame windows:

  - stage 0: `[3.3,3.8]`;
  - stage 1: `[5.0,5.6]`;
  - stage 2: `[5.8,6.6]`;
  - stage 3: `[6.8,7.6]`.

- Stage 0 rotates `(0,-50,110)` by the controller angle, adds controller
  position, then randomizes world X/Y by `rand()%100-50`. It emits one
  `CreateParticle(0x8100, subtype 1, scale 1.8)` with white light and null
  owner; the eighth S21-only trailing argument is also zero. It then
  rotates/adds `(0,150,90)` and creates two identical `0x5F5`, subtype 0,
  owned by the caster.
- Stage 1 emits one white `0x8100`, subtype 1, scale 1.8 from rotated
  `(0,0,110)` with world X/Y randomized by `rand()%100-50`, again with null
  owner. At rotated
  `(0,-220,90)` it creates two `0x5F4`, subtypes 0 and 1. At `(0,150,90)` it
  creates two `0x5F5`, subtype 0. At `(0,-100,90)` it creates one `0x5F7`,
  subtype 0, plus two `0x7FDD` (`flareBlue`) particles, subtype 5, scale 2.5.
  Both particle calls pass a null owner. Constructor
  `0x1649316..0x1649428` sets lifetime/max-life 20, preserves scale 2.5,
  starts alpha at zero, randomizes rotation `[0,359]`, and replaces the input
  light with white. Update `0x16B25E8..0x16B26EE` generates a symmetric
  half-life alpha envelope peaking at 0.85; renderer branch `0x1629002`
  submits `white * alpha` through the sprite path.
- Stage 2 creates `0xAD6` (`shockwave01`) subtype 6 at rotated offsets
  `(0,-350,90)` and `(0,0,90)`, scales 0.3 and 1.05 respectively. From
  rotated `(0,-350,110)` it randomizes world X by `rand()%200-150` and Z by
  `rand()%200-100`, then emits one white `0x8100`, subtype 1, scale 1.8. At
  `(0,150,90)` it creates two `0x5F5`, subtype 0.
- The `0xAD6` dispatcher comparisons at `0x143EFC2..0x143EFD8` select
  constructor `0x146F4CC`; subtype 6 branch `0x14700FE..0x1470205` sets
  lifetime 6, keeps the caller scale, sets alpha 0.85, subtracts 90 degrees
  from angle Y and Z, and replaces light with `(0.2,0.3,1.0)`. In the main
  update switch, type `0xAD6` follows the unmatched route
  `0x1574B64..0x1574C65` into default helper `0x1595A11` with its rotate-
  direction flag. That helper rotates Direction by Angle and adds it to
  Position; CreateEffect initializes Direction to zero, so this subtype is
  stationary with constant scale/alpha/light for its six-tick lifetime.
  Render selector `0x15A1348` reaches shared wrapper `0x15AD77D`, which calls
  the native object calc/draw wrapper `0x176D621` with default render args.
- Stage 3 emits three white `0x8100`, subtype 1, scale 1.8 from rotated
  `(0,-450,110)`; each point randomizes world X by `rand()%100-50`, Y by
  `rand()%60-30`, and Z by `rand()%100-50`; all are null-owned. It then uses
  light `(3,3,3)` and
  emits four `0x7FFD` (`clud64`), subtype 19, scale 2.0 from rotated
  `(0,-120,110)`, with each world axis randomized by `rand()%50-25` and a
  null owner. The earlier `0x7FDD` transcription was wrong. This is the same
  proven constructor family used by Magic Pin: it switches the submitted
  texture to native byte-identical `smoke01`, lifetime 20, alpha 1 and random
  rotation.
- The full emission block is anchored at `0x157A60F..0x157BE61`; these are
  decoded call arguments and transform operations, not numeric-xref guesses.

## Registered models and render contracts

Shader bounds follow-up: native New_ModelBMD.cpp AddMeshCommand rejects
negative/index>=NumMeshs/index>=NewMeshes.size before any VAOMesh indexing.
Returning false falls through to ZzzBMD.cpp legacy bounds rejection. Thus
both compiled native paths guard spin's extra mesh call; verifier now checks
both. No shader code alteration needed; this is not rendered-image evidence.

Lighting correction follow-up: common CreateEffect143E719 calls reset1315E97;
reset1315EDB/1315EE2 sets lighting flags6/7=1, then143E74F sets flag5=1.
Shared1885EB0 consequently selects full terrain light (1762EF4) plus object
Light, not the .1 branch, for default newly created effects. Native
CreateEffect LightEnable=true and Calc_RenderObject BodyLight supply the
equivalent default terrain-plus-object composition. Body/spin now retain
that prepared light; only pin retains the explicit Light*Alpha override.
No global BodyLight behavior changed. Draw's registry invocation is pinned
at1887E4E, after buff-color helper189346D and special-render helper18917BA.
Special/buff overrides and terrain sampling equivalence remain separate
runtime/compatibility gates; the correction does not claim those complete.
Mesh renderer verifier PASS; isolated QA ClCompile PASS, not yet linked or
staged. QA executable remains2EEBB97F...21CA805F.

Latest callback/mesh-bound audit: registration A1B2DF..A1B321 maps
5F4->A49D77,5F5->A49DD2,5F7->A49EE5. Body/spin callbacks do not
write BodyLight; only pin multiplies Light by Alpha three times. Current
private runtime multiplies all three, so shared lighting must be resolved
before treating body/spin parity as achieved. Calc1886382 invokes lighting
1885EB0 at188645D; that includes terrain-light selection and flags5/6/7,
not simply objectLight. Draw1887B8B also calls buff-color helper189346D
at1887C85. Next trace callback invocation after that helper and its effect
on these freshly constructed objects; no guessed lighting replacement yet.

Spin's third mesh call A49FDF is now explained statically: wrapper968CB3
calls13312EF, whose1331321..1331330 rejects index>=signed mesh count or
negative index before accessing the mesh. Native legacy RenderMesh has the
same bounds check. This closes the legacy out-of-range-call concern, NOT
GPU acceptance; optional shader AddMeshCommand early path needs a separate
guard audit. verify_shining_mesh_render_calls.py pins these observations.

- `0x5F4`, registration `0x18D2EFE`: `shiningpeakbody.bmd`; constructor
  lifetime/max 12, raw `+0xA0` 0.9, alpha 0.2. Renderer draws mesh 0 with
  flags `0x42` and object alpha/UV fields. Update `0x157BE66` increments
  `+0xA0` by 0.01 only for subtype 0. With `h=floor(maxLife/2)+1`, alpha is
  incremented by `1/h` while remaining life is greater than `h`, otherwise
  decremented by `1/h`.
  Follow-up157BE66..157BF07 audit confirms NO alpha clamp before15945F1.
  Removed non-source0..1 clamps in native body and selected private whole
  effect ticks/Timer0 (includes life0 update). Shared GrowLancerShiningMesh.h
  regression8cases PASS (two subtypes/fourFPS):13 exact samples, final alpha
  .2-3/7 below0, subtype0 scale1.03 versus subtype1 .9, one cleanup.
  ClCompile PASS; not linked/staged. Flare5-only candidateAAA4F984 remains
  the QA EXE. These tests do not prove native renderer/pool/visual acceptance.
- `0x5F5`, registration `0x18D2F33`: `shiningpeakpin.bmd`; constructor
  lifetime/max 8, raw `+0xA0` 1.8, alpha 0 and light `(0.65,0.8,1.0)`.
  Renderer multiplies the object's light by alpha and draws mesh 0 with flags
  `0x42`. Update `0x157BF0C` increments `+0xA0` by 0.15, computes a
  half-life triangular alpha as `0.2 + normalizedHalf * 0.3`, rotates local
  movement `(0,-50,0)` by object angle, and adds it to world position every
  tick.
- `0x5F7`, registration `0x18D2F68`: `shiningpeakspinmagic.bmd`; constructor
  lifetime/max 20 and raw `+0xA0` 0.8. Renderer draws meshes 0, 1 and 2 with
  flags `0x42`; the model has only two meshes, so the third call is preserved
  as native evidence pending runtime validation of the BMD render guard.
  Update `0x157C119` increments angle Y by 20 degrees and computes triangular
  alpha as `0.1 + normalizedHalf * 0.7`.

## Asset provenance notes

### Pin/spin mesh timing correction (2026-09-13)

157BF2A..157BFB8 computes maxLife-floor(maxLife/2), comparing against
floor(maxLife/2), WITHOUT body's +1. For pin max8 both threshold/divisor
are4; previous port5 was incorrect.157C14B..157C1D9 does the same for spin
max20, threshold/divisor10; previous11 was incorrect. Shared mesh helper
now computes pin.2+triangle*.3 and spin.1+triangle*.7 with these exact halves.
Both use Timer0/private whole effect ticks including life0. Pin addsScale.15
then native rotates/adds local(0,-50,0) once per tick; spin addsAngleY20.
All three mesh helpers together PASS16 x86/W4/WX cases. Pin9 ticks ends
scale3.15/alpha.2; spin21ticks endsY420/alpha.1; peaks.5/.8. Tests do not
cover native rotated trajectory/GPU, only helper math and lifecycle ordering.
QA ClCompilePASS; body/pin/spin source edits still await link/stage. Existing
Follow-up Win32 link/stage PASS:2F245623C56DC148F123B79731287B52F8070B147250D6703CC4E86A5ED8AA03
now includes body/pin/spin corrections (supersedes unstaged notes above).
Stage verifier PASS2658base+79overlay+3root,EXE/Player SHA/CRC;
project3cpp/19headers PASS. No gameplay/visual acceptance upgrade.

Shockwave lifetime re-audit:147010E sets life6. Common15945F1 only special-
cases types413/7EF7 for early decrement; AD6 instead checks expiry1594624
then decrements15946E3. Its life6 thus reaches a life0 body/cleanup visit.
Confirmed default call1574C65->15945E5 invokes1595A11 with rotate flag1,
returns at1595C4F then falls through15945F1 common lifecycle. Native private
shockwave now uses Timer0/whole ticks instead of decrement-then-kill. FourFPS
lifecycle-only tests PASS seven visits6..0/one cleanup; combined mesh suite20
casesPASS. QA ClCompilePASS; shockwave correction not yet linked/staged.
No new movement/renderer implementation: selected Direction remains zero.
Renderer Calc_RenderObject/Draw_RenderObject mapping for private model still
requires comparison with the precise S21 AD6/sub6 render path.

- All three Shining Peak BMDs are S21 v0F containers and were converted to
  native RISE v0C with byte-exact decrypted round trips.
- Embedded runtime texture names are: `shiningpeakbody.JPG`,
  `motion_mono.jpg`, and for the two-mesh spin model
  `firehik_mono02.jpg` plus `marks_m03.jpg`; generic `shockwave01.bmd`
  embeds `Damage1mono.jpg`.
- `pin_lights.OZJ`, `flareBlue.OZJ`, `shockwave01.bmd` and
  `ShiningPeak.wav` are direct constructor/update dependencies. Asset
  `shiningpeakbody_2.OZJ` exists in S21 Data but has no proven binary
  registration or call edge and is deliberately not included.
- `0x8100` is not absent from registration: loader loop
  `0x18D1EA9..0x18D1EF5` formats `Effect\\xsuper000%d.jpg` for values 1..6
  and assigns IDs `0x8100..0x8105`. Although Shining Peak creates base type
  `0x8100`, constructor `0x16892DA..0x16893B0` gives subtype 1 lifetime 6,
  preserves scale 1.8, randomizes rotation `[0,359]`, and replaces white
  input light with `(0.7,0.7,1.0)`. Update `0x17017F5..0x17018FE` sets the
  subtype to `6 - remainingLifetime` and multiplies all light channels by
  0.95 each tick. Renderer branch `0x162C314` submits texture
  `type + subtype`, full UVs, particle scale and rotation. Therefore all six
  `xsuper0001..0006.OZJ` frames are runtime dependencies; the former
  one-file inventory was incomplete and has been corrected. All six are now
  individually hash-pinned and staged.

## Open chain parts

### FlareBlue5 alpha-order correction (2026-09-13)

S21 16B25E8..16B26EE calculates the half-life alpha from CURRENT lifetime,
then16B3A77 jumps172343C for decrement. Native previously ran the same
formula after generic pre-decrement. Moved ONLY private flareBlue/subtype5
to AdvanceParticleWholeTicks using its allocation-reset per-slot flare
accumulator. Shared GrowLancerFlareParticle.h preserves integer half-life and
the .85 envelope; native constructor20/max20,zero velocity,whiteLight and
renderer Light*Alpha remain unchanged. No global particle behavior changed.

Focused x86/W4/WX test fourFPS factors PASS exact20 samples: alpha0 atlife20,
peak.85 atlife10, last.085 atlife1; no life0 update and one expiry. ClCompile
PASS, project3cpp/18headers PASS. Follow-up flare5 link/stage PASS; candidate
AAA4F98428D5287E46A6C07DB297C888C2F9143645BE405C12C48BD4B1F1D77E.
Tests prove helper timing, not GPU sampling,
runtime pool reuse or full Shining Peak visual acceptance.

### Xsuper tick-order correction (2026-09-13)

The native port previously entered generic pre-decrement before computing
6-LifeTime, skipping ordinal0 at factor1 and shortening the six-frame sequence.
S21 body17017F5..17018FE computes ordinal first, then jumps172343C for
decrement. Private Xsuper now uses AdvanceParticleWholeTicks with per-slot
remainder reset on every allocation, before the generic native pre-decrement.
Shared GrowLancerXsuper.h computes0..5 over life6..1 and Light*=.95;
an explicit six-ID array retains the non-contiguous private resource mapping.
No lifetime0 body or invented frame6/clamp. Native creation zeros Velocity,
and this particle body never changes it, so skipping generic movement adds
no missing displacement in this selected path.

x86/W4/WX testPASS fourFPS factors: six update ordinals0..5, six color decays,
expiry with no extra update. ClCompile PASS, project3cpp/17headers PASS.
Follow-up linked/staged candidate13AD9A157E67F00A0B8BBFCC7E89FAF72D007898BDA8679C3B087C2094559161
includes the correction, with prior6653FBC7 candidate backed up.
Simulation frame sequence is not proof every frame is sampled by the renderer
at low FPS, nor proof of gameplay visual parity.

Renderer follow-up:162C314..162C3C6 passes UV(0,0,1,1),rotation field60,
Light directly (no extra Alpha multiply),Position and textureType+ordinal
to18E8B0C with mode4. Shared1620AD0..1620B32 derives width/height from
base texture dimensions*Scale; native derives them from selectedTexType.
All six staged OZJ JPEG payloads were decoded read-only and are RGB128x128,
so that lookup distinction does not change these dimensions. RGB component3
selects additive18E7137 at1620B3A..1620B52, matching native RGB branch.
Native default RenderSprite uses full UVs,Light,Rotation and selectedTexType.
No new raster/renderer adapter needed for reviewed Xsuper parameters.
Stage verifier PASS2658base+79overlay+3root,EXE/Player SHA/CRC; this remains
static/staging evidence, not GPU or owner acceptance.

- Prove server line-target selection/damage scheduling. The client visual
  receive chain is complete and intentionally contains no target/contact or
  reaction layer.
- Runtime-compare the now-native SS6 pin-light triplet, six-frame xsuper,
  flareBlue, clud64/smoke and isolated shockwave model adapters; no S21
  address or object layout is copied into runtime code.
