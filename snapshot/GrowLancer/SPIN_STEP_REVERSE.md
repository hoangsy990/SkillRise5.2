# Spin Step (skill 271) reverse evidence

Status: `IN_PROCESS` — identity, local/receive dispatch, action, root controller,
controller constructor, sound registration and one child-model identity are
static evidence. The full root update fan-out, hit reaction and cleanup chain
remain open.

## Identity and dispatcher

Implementation correction (2026-09-13): anchored constructor1481C51 has
MOVSS constant1B4E93C into Angle.Z at1481CED..1481CF5, not an addition.
Native cross now sets absolute120 degrees. Three calls157CC1E/157CC7A/
157CCD6 pass the same resolved actor Angle pointer; removed fabricated
i*120 input fan spread. Gate157CA90 compares frame with1B7B1EC and JBE
skips at157CA97, so lower bound is strict (>4.7); upper6.5 remains inclusive.
These are source corrections, not proof the previous owner ring-only symptom
is fully fixed. Retained-index owner resolution and frame/visual QA remain.

- `SkillList.xml` row 271 identifies `Spin Step`, Grow Lancer only, level 83,
  damage 100, mana 12, BP/AG 0, range 2, delay 500 ms and dexterity 150.
- The primary client dispatcher reads the skill at `0x010E34E2` and compares
  `0x10F` at `0x010E3547`; both base skill 271 and master skill 687 route to
  `0x010E38FF`.
- The local branch sets action 186 (`0xBA`) through `0x1327DE8` at
  `0x010E3A86..0x010E3AA3`. It does not construct the visual root there.
- Do not merge the adjacent `0x010E3AAB` branch into Spin Step: that branch is
  skill 20, sets action 138 and emits bitmap `0x7F68` (`Shiny03.jpg`).

## Receive path

Target-index closure (2026-09-13): packet7/8 forms local-1974 at12C50D0,
mask7FFF at12C52DD;13F5F39 result stored-1978 at12C5306. Receive12CB1E9
passes this target index as wrapper argument+34.143E5AA stores its WORD
in metadata+8;143E6EA/143E6EE reads it into-314C and143E94D stores effect+364.
Update157CADE consumes that index and resolves its character, not root.Owner.
Thus blur remains caster-owned but subtype1/three crosses spawn at current
target position/angle.157CD44 passes null owner for ground8145. Native now
passes TargetIndex through m_sTargetIndex and resolves it at the frame gate;
missing/dead targets skip fanout after consuming the one-shot counter as source.
No S21 offsets were inserted into native structures. Constructor retains caller
caster scale via native CreateEffect Scale argument. QA F7 uses selected target.

- At `0x012CB1BC..0x012CB1E5`, the received-skill path avoids resetting action
  for the local actor; remote actors are set to action 186.
- It then constructs secondary effect `0x5F8` through `0x143E57C` at
  `0x012CB1E5..0x012CB253`, using the actor position/angle and the receive
  record's retained owner/target metadata.
- It plays sound `0x455` at `0x012CB256..0x012CB264` and marks the actor state
  byte `+0x21 = 1`.
- Sound registration `0x018E2CD1..0x018E2CFD` binds ID `0x455` to
  `Data\Sound\SpinStep_Cast.wav` (string VA `0x01BB52E4`). The next entry,
  sound `0x456`, binds `Data\Sound\SpinStep_Hit.wav` (string VA `0x01BB5304`);
  its exact hit call site is the received internal skill-280 branch described
  below.

## Authoritative hit/explosion receive branch

- The current `SkillList.xml` row 280 is Grow-Lancer-only internal skill
  `Explosion`, type 1/use type 6, damage 50 and range 2. It is the secondary
  formula component already referenced by Spin Step's server calculation.
- The received-skill switch covers IDs `279..298`: it subtracts 279, maps ID
  280 through selector byte 1, and dispatches to `0x012CB276`. This proves the
  branch identity from switch structure rather than the nearby sound name.
- That branch creates `0x5F8` subtype 2 at the received hit actor's
  position/angle, with that actor as owner and scale `1.0`, then plays sound
  `0x456` (`SpinStep_Hit.wav`) at `0x012CB2E5`.
- Subtype 2 has lifetime 15. At remaining lifetime 10 it emits one blue flare
  at scale `3.5`, five subtype-4 flare particles at scale `0.2`, and three
  `0x5F9` crosses with light `(0.2, 0.6, 1.0)` and scale `12.0`. At remaining
  lifetime 5 it emits two white `Hole.jpg` effects at scales `1.4` and `2.1`.

## Visual resources proven so far

- Effect `0x5F8` has a registered render callback `0xA49FE8`; that callback
  returns false without drawing. Therefore it is an invisible controller, not
  the visible Spin Step layer.
- `CreateEffect` dispatches type `0x5F8` through jump-table slot `0x101`
  (`0x14B3224 + 0x101 * 4`) to constructor handler `0x1481956`.
- For the received subtype `0`, that constructor sets lifetime `+0x6C = 32`,
  counter `+0x84 = 0`, and copies caller scale to `+0xA0`. It resolves action
  186 through `0x968B22 -> 0xD6DB7E` and stores the returned model/action field
  `+4` at controller `+0xBC`. The semantic name of that cached field remains
  deliberately unresolved until its update consumer is decoded.
- Adjacent model `0x5F9` is registered at `0x18D2F9D` as
  `Data\Effect\spin_cross.bmd`. Its registered render callback is `0xA49FEF`;
  that callback multiplies model light by effect alpha and renders mesh 0 with
  flags `0x42`, establishing it as a visible child rather than a controller.
  Its exact spawn edge from controller `0x5F8` is not yet closed.
- `SpinStep_Cast.wav` and `SpinStep_Hit.wav` exist in the read-only S21 Data
  tree. `spin_cross.bmd` decrypts as `spin_cross.SMD` with 1 mesh, 1 bone and
  1 action. A hash-guarded copy of both WAVs and a v0C conversion of the BMD
  are staged only in the isolated Grow Lancer test client.

## Proven root fan-out (subtype 0)

- The effect-update switch subtracts `0x5F3`, reads selector byte 4 for
  `0x5F8`, and dispatches it to `0x157C1E6`. This closes the update-handler
  identity independently of numeric xrefs.
- Subtype 0 requires owner action 186; otherwise it calls the native effect
  destruction path at `0x13167BB` and leaves the case.
- The handler mirrors the owner model/action transforms and samples the action
  three times per update. The complete `0x157C2A8..0x157CA4A` block is now
  mapped: action speed divided by 3, sampled frame range `[0.0,4.5]`, caster
  bone 33, local endpoints `(0,0,0)` and `(0,-180,20)`, rotated local offset
  `(0,-10,0)`, white light, object-blur type 1, grouping subtype 0, native
  lifetime, and SS21 render style 1.
- The ninth blur argument is not an unknown boolean: allocator
  `0x15C01CC` stores it in the new render-style byte, and renderer branch
  `0x15C09DE..0x15C0BB8` advances V down the tail while spanning U across its
  width. SS6 only has style 0, so the isolated adapter adds style 1 while all
  existing calls retain the default style 0.
- Once the owner's action frame is within `(4.7, 6.5]` and controller counter
  is zero, it increments the counter and resolves the retained owner index at
  `+0x364` to the owner's object (`+0x3254`). At that object position/angle it
  creates one `0x5F8` subtype 1, followed by three `0x5F9` subtype-0 children.
- Each of the three `0x5F9` calls uses light `(0.2, 0.6, 1.0)`, scale `17.0`,
  the owner object, and its position/angle. The same one-shot block also emits
  `0x8145` (`magic_ground1_3.jpg`) subtype 1, light `(0.1, 0.15, 0.4)` and
  scale `5.0`.

## Secondary controller subtype 1

- Constructor `0x14819DB` sets max lifetime and lifetime to 10, randomizes
  angle Z in `[0,179]`, sets alpha and the auxiliary float at `+0xA8` to zero,
  and clears its one-shot counter.
- It immediately emits one `0x7FDD` (`flareBlue.jpg`) effect at scale `5.0`
  plus five particles of the same bitmap at scale `0.2`, subtype 4, attached
  to the retained owner.
- Update branch `0x157CD85` waits until remaining lifetime is at most 5 and,
  once only, emits two `0x8003` (`Hole.jpg`) effects at the controller position
  with white light and scales `2.0` and `3.0`.
- Subtype 2 belongs to the separate authoritative internal-skill-280 hit path,
  not to the initial base-skill-271 cast branch.

## Visible cross child (`0x5F9`, subtype 0)

Renderer attribution correction (2026-09-13): registrationA1B334 pushes
A49FEF then5F9 and callsA1DCE6. Full callbackA49FEF..A4A0FA multiplies
each effect.Light component by effect.Alpha, writes model BodyLight, then
calls968CB3(mesh0,flags42,alpha,blendMesh0,blendLightAlpha,objectUV,-1).
The descriptor's old fixed(.37,.57,1) callbackA49422 is registered for5EB
atA1B246, NOT5F9. Native cross renderer already matches the recovered
stored-light contract; corrected descriptor instead of changing correct code.
Reproduce: tools/grow_lancer/verify_spin_renderer_binding.py (full dump hash
and both push/push/call instruction sequences pinned). Runtime QA still open.

- Constructor `0x1481C51` sets max lifetime/lifetime to 10, retains caller
  scale at `+0xA4`, starts render scale at caller scale times `0.5`, alpha at
  `1.0`, and angle Z at `120.0`.
- The exact update switch maps `0x5F9` to `0x157D348`. During the first half of
  its life, scale INCREASES by `callerScale * 0.1` per tick; during the second
  half it DECREASES by the same amount. Earlier inverse description was wrong.
  ADDSS157D38B and SUBSS157D3BE prove the signs. Alpha decreases by `1/maxLifetime`
  each tick. Native pool lifetime expiry owns final cleanup.
- Render callback `0xA49FEF` multiplies light RGB by current alpha and renders
  mesh 0 with flags `0x42`.
- The BMD embeds `line_fire.jpg`; the corresponding
  `Data\Effect\line_fire.OZJ` is therefore a model dependency in addition to
  the three directly addressed bitmap layers.

## Open chain parts

- Server selection/timing that sends internal skill 280 and any target reaction
  state outside this received visual branch.
- Runtime comparison of the new bone-33/style-1 weapon ribbon and the separate
  selected-target internal-280 contact sequence.
# Particle7FDD/subtype4 follow-up (2026-09-13)

## Hole8003/sub0 and terrain blend correction

Constructor143F382->1481F9E, subtype0->1481FB8: life/max10, supplied scale,
absolute rand()%180, alpha0. Update1574B74->157D8B2, subtype0->157D8CC:
same +/-1/(max-max/2) envelope as flare, includes life0 via effect manager.
Renderer15A1599->15AD3BF, subtype0->15AD3DC..15AD536: terrain1765DF1,
RGB=Light*Alpha,size=Scale,rotationZ,alpha1,height5,terrain heights1,flipV0.
Caller157CD85..157CDCA sets whiteRGB for cast holes;157D247 onward sets white
for hit holes too. Native previously supplied blue; corrected to white.
Hole now uses the proven shared flare envelope helper and whole-tick selection,
while retaining its own texture/subtype. Native ground/flare/hole explicitly
EnableAlphaBlend before terrain call, DisableAlphaBlend after, matching S21
18E7137/18E6EFF. RISE render interception itself did not guarantee additive state.
Native helpers retain sprite-batch flush/state-cache handling. Terrain pass,
visibility and gameplay visual acceptance remain open.

## Ground8145/sub1 follow-up

Initial scalar resolved:143E71F calls reset1315E97;131607C writes zeroB4.
Native correction now implemented in GrowLancerSpinGround.h and carrier
constructor/update/render; Distance stores multiplier, StartPosition stores
initialRGB in private carrier only. Native terrain divides by size, so zero-area
draw before first update is skipped. Helper PASS fourFPS cases; terrain pass,
culling and visual acceptance remain open. Earlier pending notes below are
historical evidence, superseded for initial scalar and native implementation.

Constructor dispatch143F5F9->1474DFD, subtype1->1474E38 sets life10;
1474E8F..1474F11 saves initial RGB at104,1474F13..1474F21 sets alpha1.
Primary update dispatch14B831E->15288D7, subtype1->152894A sets scalarB4
to(11-life)*0.1 and subtracts0.1 from alpha. Shared tail15289F2..1528ABB
sets RGB=initialRGB104*alpha. Constant1B4DF04 verified0.1.
Renderer15A165F->15AAC22, subtype1->15AACE9..15AADBE uses additive terrain
1765DF1: XYposition,sizeXY=Scale*B4,RGB(current),rotation0,alpha1,height5,
terrain heights1,flipV0. Unlike flare it does NOT multiply RGB by alpha again.
Current generic carrier has wrong sprite orientation, fixed size and envelope.
Correction is pending scalarB4 initial value / full update lifecycle audit;
do not invent an initial size or transplant S21 field offsets into native OBJECT.
Descriptor now records the proven contract and explicitly pending native mapping.

Native update correction implemented in GrowLancerSpinParticle.h and private
ZzzEffectParticle.cpp whole-tick branch. Generic S21 initialization164151E
sets rotation0;164152E enables movement;1641638..164168E zeros velocityXYZ.
13272A0 builds angle matrix viaD30B1A, rotates velocity viaD3189D, then adds
the three resulting components to position. RISE ZzzAI.cpp MovePosition
has equivalent native AngleMatrix/VectorRotate but adds FPS multiplication;
private integer-tick branch invokes native AngleMatrix/VectorRotate/VectorAdd
directly to avoid multiplying FPS twice. Per-pool-slot remainder resets on
every allocation, even if reused for another particle. Subtype5 unaffected.
Shared helper x86 /W4 /WX test PASS16 life20/39, FPS.25/.5/1/2, movement
enabled/disabled cases; checks pre-jitter movement, draw order, alpha, rotation
and pre-update expiry. Native ClCompile PASS. These tests exclude native
angle-matrix parity, owner snapshot semantics, allocation pressure and visuals.

Constructor164917E..1649311 confirms max/current life rand()%20+20,
caller scale, alpha1 and position jitter (existing native initializer).
Renderer dispatch1620D9C->1628B23, subtype4 check1628EA0 selects1628EAA:
temporary RGB=Light*Alpha, passed into sprite18E8B0C at1628FEA with stored
rotation, full UV0/0/1/1, final argument4. Native renderer now includes
subtype4 in its alpha-scaled branch. Blend/size wrapper parity remains open.

Update body16B2468 selects subtype4 at16B2472..16B25D8:
alpha -= 1/maxLifetime; velocity XYZ each += (rand()%20-10)*0.2;
rotation += rand()%16. Constant1B4DF08 is0.2. Native currently lacks this
body. Do not call the particle static or fully implemented.
Common manager16AA6F1 checks movement-enabled byte68; enabled path calls
13272A0(position20,angle2C,velocity70) BEFORE subtype dispatch.
Owner snapshot update16AA743..16AA7CF is separate. Manager decrements
lifetime at172343C after body, unlike native generic predecrement.
Next: verify constructor default movement-enabled/velocity and13272A0 mapping,
then opt this private particle into its own whole-tick path with native pool
reuse reset. Do not borrow Breche variant semantics or affect subtype5.

# Offline flare subtype-0 audit (2026-09-13; native correction implemented)

Pinned mapped image SHA256 remains
6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637.
Constructor dispatch 143F30D compares type7FDD and branches1481CFE.
Subtype0 selects1481D69: maximum/current lifetime10, supplied scale,
absolute random Z `rand()%180`, initial alpha0. Spin controller subtype1
call1481AD3 passes subtype0 into wrapper143E57C (call1481B05).
Update dispatch1574C55 selects157D400, subtype0 selects157D468.
It computes half=maxLife-maxLife/2, then adds1/half while life>half,
subtracts1/half otherwise. Float constants1B4E474=2,1B4DD48=1.
No clamp in that body. With life10..0 this means five additions and six
subtractions, ending alpha-.2. Current generic carrier constructor alpha1,
relative rand()%360 and SymmetricAlpha update do NOT implement this contract.
Native correction: GrowLancerSpinFlare.h implements the constructor/update;
private flare predicate opts only this carrier into whole-tick lifecycle.
Shared helper test passes12 rotation/FPS cases, including alpha-.2 at expiry.
Renderer dispatch15A1559->15ACDB8; subtype0->15ACE16..15ACF70 enables additive
blend, computes RGB=Light*Alpha and calls1765DF1 at15ACF62: texture7FDD,
positionXY, scaleXY, angleZ, alpha1,height5,terrain heights1,flipV0.
This is terrain-aligned, not a camera-facing sprite. Native flare carrier now
uses existing RenderGrowLancerTerrainBitmap, leaving other carrier subtypes alone.
Terrain culling/world eligibility and render-pass parity remain open, as with
Breche terrain adapter. Particle subtype4 is a separate pending audit.
