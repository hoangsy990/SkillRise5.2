# Spin Step (skill 271) reverse evidence

## Hidden oblique/flat-depth pose ribbon control — 2026-09-15

The owner still reports a ground ring without the caster weapon ribbon. A
new QA-only `RISE_GL_SPIN_POSE_DEPTH_QA=1` uses the same exact private merged
player action285/bone33 sampler and native style-1 `CreateObjectBlur`/
`RenderObjectBlurs`, now with a -55° oblique fixture camera, real GL depth
test and a black Z0 flat plane. This is not a S21/native gameplay cast,
real terrain, caster frame/scale, 360° camera or owner visual acceptance.

The first probe candidate drew 103 no-plane and 115 plane pixels, but it
directly disabled GL_TEXTURE_2D while native `TextureEnable` stayed cached
true. The second draw could therefore be **untextured**, and those 115
pixels were explicitly rejected as evidence. A depth read at screen
(128,64) also missed the plane (1.0). The fixture was corrected to read
the plane center (128,128) and use native `DisableTexture(false)` before
clearing/drawing the plane; native `EnableAlphaBlend` now restores texture
for the actual blur pass instead of leaving a stale GL/cache mismatch.

Current hash-staged isolated QA Engine SHA-256
`3DEAC1EE5B654C8A04E445A3A187399DE542CD550436DBBA56FE3DA454FF3609`
has exact private rollback `0E8C3537...`, stage verifier PASS. Hidden
PID15520 exit0 reports source-mapped poseReady1/minWidth181.108,
103 no-plane and **103 textured plane-pass lit pixels**,
groundDepth0.501395 before ribbon, GL error0/releaseStable1. Its PID-bound
native blur verifier PASS. The unchanged depth-off pose regression PID12572
exit0 reports 114 pixels/GL0/releaseStable1, verifier PASS. The private
shader/whole gameplay terrain path was not invoked by this compatibility
WGL control; it only shows the entire synthetic bone33 ribbon is not
occluded by a depth-writing flat Z0 plane at this pose/camera. Owner
ring-only visual FAIL, actual world vertices/texture query, contact and
S21 same-frame comparison remain OPEN. No global SS6 renderer changed.

## Native merged-player action285 pose ribbon fixture — 2026-09-15

Current staged private QA Engine SHA-256 `6DE3A1FECF7A868F428A962851084470FA6F0687527CB575210A5F75740218BF`
has exact `2E69CECC...` rollback and full Data stage verifier PASS. New
QA-only hidden probe `RISE_GL_SPIN_POSE_BLUR_QA` uses actual merged
`Data\Player\player.bmd` via native `BMD::Open2`, action285
`BMD::Animation`, bone33 `TransformPosition(...,true)`, then the unchanged
private style1 `CreateObjectBlur`/`RenderObjectBlurs` with the pinned S21
sampler. Fixture frames are 0.5/2.5/4.5, prior action0, owner scale1,
angle0 and orthographic/depth-off camera; these are explicitly synthetic
pose inputs, not captured owner frames.

Staged PID17052 exit0 reported poseReady1, minimum endpoint width181.108,
maximum absolute coordinate170.424, 114 nonblack native framebuffer pixels,
GL error0 and stable release1. The PID-bound verifier passes. Current
synthetic-segment PID22320, Spin-cross PID3532 and Magic-pin PID4448 all
exit0/verify PASS; isolated QA/nonQA x86 links pass. This rules out an
intrinsically degenerate bone33 strip for the sampled merged-player pose,
but actual caster scale/frame/action speed, blur pool segment coordinates,
gameplay camera/depth/blend and owner-visible pixels remain unverified.
No owner QA was launched after the owner chose continued hidden checking.

## Hidden native style-1 blur raster and world-transform audit — 2026-09-15

The prior private staged QA Engine SHA-256 `2E69CECCECC0BDFBF3D71CE7162218C038B00E50A933674BE651BA5A5A9C14E8`
has exact prior staged `95736241...` rollback and complete Data verifier PASS.
An opt-in hidden orthographic fixture calls the real `EnsureSpinMotionBlurBitmap`,
`CreateObjectBlur` three times with a live Spin controller owner, then native
`RenderObjectBlurs` and framebuffer readback. PID22740 exit0 recorded 6,379
nonblack pixels, GL error0, and stable bitmap/pool release; the PID-bound
`verify_spin_native_blur_fixture.py` passes. Current independent hidden Spin
cross PID10372 and Magic Pin PID24184 regressions also exit0/verify PASS.
QA/nonQA isolated x86 links pass. This proves the private style-1 texture
and renderer can rasterize a bounded fixture, **not** that the owner's caster
ribbon exists in gameplay or matches S21.

`verify_spin_blur_world_transform.py` decodes the complete S21
`132F433..132F56F` helper and `157C828..157CA4A` caller: translate=true
transforms both bone33 endpoints by model scale and body origin, then adds
the same rotated local offset before allocating the blur. Native
`BMD::TransformPosition(...,true)` and the private emission code map those
operations directly. The transform math is source-consistent; live owner
pose/origin, allocated segment coordinates, gameplay camera/depth and actual
pixel visibility remain OPEN. Owner deferred the requested fresh cast, so no
visible QA client or server stack was started in this checkpoint.

## Caster ribbon bone/clip identity — 2026-09-15

Read-only `verify_spin_caster_bone_33.py` pins original S21 `player.bmd`
SHA-256 `E0F4CB5A...` and the private merged v0C `player.bmd`
SHA-256 `0CC3D22D...`. Both have 60 bones; bone33 is the non-dummy
`knife_gdf` child of bone28 in both. S21 action186 and native action285 each
have six keys and identical action hashes. The native ribbon site still
samples bone33 with source endpoints `(0,0,0)`/`(0,-180,20)`.
Raw bone-header padding differs between the clients, so this verifier
compares name/parent/dummy identity, not unused bytes. This rules out a
wrong bone index or wrong transplanted clip as the explanation for the
owner's ring-only cast; it does not prove identical playback speed, live
world coordinates, blur-pool allocation, rendered pixels or S21 parity.

## QA acceptance sequence is two messages — 2026-09-15

Pinned S21 native frame41 SHA-256 `3FFD5023...` visibly has a blue target-
side column/contact. Client receive dispatches base271 to
`CreateSpinStepRoot` and internal280 to **separate** `CreateSpinStepHit` on
the target. QA F7 calls only 271; QA F9 invokes only the internal contact
fixture. `verify_spin_two_message_visual_contract.py` pins receive/QA branches
and the unchanged S21 frame. The next like-for-like isolated visual test must
pair one target-bound F7 with one F9 on the same target and capture caster and
target phases independently. F7-only cannot close the two-message chain.
This does not excuse the owner's separate report that caster action/ribbon
itself is absent; that root phase remains FAIL. F9 is neither server damage
authority nor a substitute for the real GameServer 280 handler.

## Current opt-in gameplay cross draw query — 2026-09-15

The selected-target older PID28160 proves native cross submits, but not
per-mesh shader-batch GPU samples. QA-only `RISE_GL_SPIN_CROSS_RASTER_QA=1`
now wraps only `spin_cross` `BMD::RenderMesh(0, RENDER_TEXTURE|RENDER_BRIGHT)`
in a bounded 12-draw non-nested `GL_SAMPLES_PASSED` query during armed Spin
F7. It flushes the native shader queue before the query and the cross draw
inside it; it records target-owner/effect world position, alpha, scale, life,
VAO/index, program/depth/cull and samples. The opt-in launcher switch
`-SpinCrossRasterProbe` restores parent environment, defaults OFF and requires
`-LocalServer`/owned loopback stack for interactive QA.

The prior private staged QA Engine SHA-256 `95736241063643B58F8ECE167A2B9CFBADB6EAB1FCDBE59CBF06F2CBEDC14643`
had exact `73602038...` rollback; full Data verifier and isolated QA/nonQA
x86 builds PASS. Source-scope verifier PASS. Hidden staged Spin fixture PID1776
and Magic regression PID19448 pass independently. **No gameplay F7 cross
query has run**; samples are not owner-visible pixels or S21 parity. If
diagnostic shader flush changes appearance, compare with probe OFF first.

## Native visible-cross fixture pixel evidence — 2026-09-15

Hash-pinned read-only S21 `spin_cross.bmd` SHA-256 `B0097042...` and
isolated v0C `6E0B4051...` decrypt to identical plaintext. The exact source
mesh has 12 vertices, 8 triangles in four intersecting planes, one bone/action
and embedded `line_fire.jpg`; bounds X/Z ±8.3502, Y -0.0051..28.8716.
`inspect_spin_cross_model_geometry.py` validates the hashes, conversion,
native-compatible full layout, triangle/UV indices and animation, without
altering either client Data tree.

QA-only `RISE_GL_SPIN_CROSS_DRAW_QA=1` exercises the real private BMD Open2,
GPU upload, native `BMD::Animation`/bright mesh render and release at the S21
constructor angle Z=120, first-tick scale8.5, light `(0.2,0.6,1.0)`.
Only camera/pose/orthographic view are fixtures. Current hash-staged private
Engine `73602038...`, hidden PID19452 exit0, native GPU samples
12250/11844/2548/11844 and non-black framebuffer pixels
4761/4716/1152/4813 for four fixture camera sweeps; GL error0 and stable
release1. PID-bound `verify_spin_cross_native_fixture_draw.py` PASS. Independent
Magic fixture regression PID5644 still PASS. This disproves an intrinsically
undrawable child mesh/texture on those native fixture views; it does **not**
prove current gameplay target coordinates, ribbon visibility, hit or S21 frame
parity. Owner's ring-only Spin report remains visual FAIL pending a fresh one-
cast local F7/F11 target-bound camera QA.

## S21 ribbon sampler mapped to isolated native 5.2 bitmap — 2026-09-15

Pinned S21 `main.exe` registration `0x18BDDB8..0x18BDDD5` loads exact
`Effect\motion_blur.jpg` at S21 runtime resource `0x7F08` with
`GL_NEAREST` (`0x2600`) and `GL_CLAMP` (`0x2900`). The read-only S21
`Data\Effect\motion_blur.OZJ` SHA-256 is
`934716E4C09EBCA25B43E62BC1F6B69B26663FA54306F5054FA12A354B6DC5AF`
and is byte-identical to the frozen Grow Lancer copy. Stock 5.2 registers
the same image in shared `BITMAP_BLUR+1` with `GL_NEAREST` and
`GL_CLAMP_TO_EDGE`: image parity alone did not establish sampler parity.

The isolated adapter stages the pinned copy under
`Data\RISE\GrowLancer\SpinStep`, registers it through the existing native
bitmap loader at private 5.2 slot `32986`, and selects that slot only for
Spin controller owner/type1/style1 weapon ribbon. `32986` is a private 5.2
allocation, **not** an inferred S21 resource ID. Stock SS6 blur modes and
shared bitmap sampler remain untouched. `verify_spin_private_sampler_contract.py`
PASS covers registration bytes, asset hash, scoped source adapter and stage.
Isolated QA/nonQA Win32 builds/link PASS; current private QA Engine SHA-256
`E3EC7999EE3D9D361A6F41724D0E9077FE4042AD92ABCC97E2DD3C9BE0A7F2D3`.

Hidden offline WGL QA PID16616 exited 0 before socket/login: native stock
and private bitmap decode to identical 32x64 pixels; private GPU readback
matches CPU; stock wrap stayed `33071` (`GL_CLAMP_TO_EDGE`) before/after
private load, private wrap is `10496` (`GL_CLAMP`), both filter `9728`
(`GL_NEAREST`), GL error0; release restored bitmap count/bytes. This proves
native image/sampler isolation on that GPU, **not** actual gameplay ribbon
pixels. Owner's last visible Spin result remains ground-ring-only FAIL.
Fresh local-only one-cast F7/F11, ribbon vertex/texture telemetry and fixed/
360-degree owner pixels against S21 frames are still required.
The draw-log inspector now accepts optional `--expected-texture 32986` for
that fresh candidate without rewriting historical evidence: PID28160 still
passes the old draw/retirement check, while the private-slot assertion
correctly fails on that older shared-bitmap cast. This is a verifier fixture,
not evidence of a new gameplay cast.

Blend was checked as a possible missing-ribbon cause, not assumed: the
decoded S21 object-blur renderer enters `0x15C04B6`, calls
`0x18E7137` at `0x15C0544`, then binds the selected texture at
`0x15C0566`. That helper selects blend state 3 with `glBlendFunc(1,1)`
(`GL_ONE/GL_ONE`). Native 5.2 `EnableAlphaBlend` selects the same blend
state/GL factors. The retained PID28160 records contain texture32019 but
**not** world-space head/tail coordinates; the new QA vertex hook was built
after that cast. Neither a blend mismatch nor off-camera geometry is proven
by the old draw count. Do not change blend or ribbon position without a
fresh texture32986/vertex/pixel result.

## Owner attribution correction — 2026-09-15

The owner's purple-shoulder S21 crop (SHA-256
`6F0692ACB12765BD1E27EB3F3635268CD4FAACE64715FAFFB46254A5B2500F40`)
was explicitly corrected by the owner: its two purple fire-like shoulder
lights belong to **Circle Shield**, not Spin Step. Do not add them to the
Spin descriptor or stage a purple asset under Spin based on that crop. The
decoded base-271 local branch sets action 186 and leaves without a shoulder
spawn; the proved `0x5F8` fanout remains weapon blur plus target effects.
Spin's ribbon/target parity is still OPEN for independent reasons below.

## Isolated selected-target render evidence (2026-09-14)

### Historical frame gate is reached; timing alone is not the observed ring-only cause

`verify_spin_historical_cadence.py` pins exactly the second selected-target
PID28160 cast at `2026-09-15T05:37:13.058Z`, its F11-flushed 21-sample
controller record and 20 style-1 native blur draw records. Action285 runs at
logged speed0.706: sample15 reaches frame4.706 at250ms with stage0, sample16
reaches frame5.001 at266ms with stage1, and only sample20 exits to action11.
The stage gate therefore fires before action exit in **that old QA cast**;
the outcome records also attribute type10137 fanout at frame4.706/250ms.
These blur draws used the older shared stock texture32019 and owner still saw
only the ground ring. This eliminates a proposed "never reached fanout frame"
explanation for that historical cast, but does not prove S21 and SS6 actor
cadence are equal, nor diagnose current private-sampler pixels/world/depth.
Stage1 is a client effect outcome, not a confirmed GS hit. Do not retime the
production skill/global animations solely from this old trace.

### Verified native draw and map cleanup, still owner visual FAIL

The retained QA log now includes selected-target PID28160 cast
`2026-09-15T05:37:13.058Z`: `inspect_spin_blur_draw_log.py` finds 20 actual
native style-1 blur triangle-fan draw records with registered bitmap,
19 Spin cross model submits, and two map style-1 records whose
`map-after-delete` count is zero. This is bounded native draw-path and
pool-retirement PASS, not proof the user's missing ribbon/cross appeared
on screen. The last owner observation was only a ground ring; pixel
visibility, caster/target position, frame comparison and hit contact
still require fresh isolated gameplay QA. Do not describe Spin Step as
visual PASS from allocator, submit or triangle-fan telemetry.

The subsequent QA-only `RecordSpinBlurDrawQA` now records the actual native
head/tail ribbon vertices `p1/p2` from the same `RenderObjectBlurs`
triangle-fan pool record, after the draw. Four world-space edge positions
allow a fresh cast to distinguish far/off-camera/zero-width geometry from
a correctly located but invisible blend/depth/texture layer. This changes
no production/SS6 renderer math and is bounded to the armed Spin F7 cast
(128 rows maximum). Isolated QA Win32 link and hash-pinned stage PASS,
SHA-256 `587A2976EC82F05C36EC44AF40A46E1330F08575DA2DD25E896F572396544DC3`;
the pre-geometry skills-only QA SHA `B7049F50...` is retained as rollback.
No new geometry log or owner pixel verdict exists yet.

The retained RuntimeQA log contains an earlier selected-target cast with a resolved
target outcome and successful native submission records for the target fanout:
`cross-visible`, `cross-ensure`, `cross-calc`, `cross-submit`, and
`ground-submit` all return `result=1`. `verify_spin_selected_target_render_qa.py`
pins the source fanout and this bounded log evidence. This proves the isolated
5.2 path reached the existing model/terrain renderer in that staged revision;
it does not prove the current executable's screen
pixels, S21 matching-frame parity, weapon blur/trail visibility, contact
placement, movement, or pool cleanup. The old owner report of a caster-only
ground ring therefore remains a historical visual failure until the current
candidate is retested in-game.

The QA build now also records `weapon-blur-submit` at the three native
bone-33 samples when the sampled action frame is in `[0.0,4.5]`. This remains
bounded diagnostic telemetry: a record means the existing blur allocator was
called, not that a ribbon was visible on screen or matched the S21 camera.

### Ribbon texture/draw lifecycle follow-up — 2026-09-15

The S21 resource registry registers `Effect\motion_blur.jpg` at runtime ID
`0x7F08` from registration VA `0x18BDDB8`. Its read-only source OZJ SHA-256 is
`934716E4C09EBCA25B43E62BC1F6B69B26663FA54306F5054FA12A354B6DC5AF`;
the frozen Grow Lancer QA source copy is byte-identical (8,734 bytes). RISE
5.2's existing bitmap registration loads `Effect\motion_blur.jpg` at native
`BITMAP_BLUR+1`, exactly the texture selected by object-blur Type 1. No S21
numeric resource ID is copied into the 5.2 enum.

QA PID 23952 logged root dispatch with target 12 and 273 native blur allocator
calls, but allocation calls do not establish actual triangle-fan draws. The
new isolated QA binary logs `spin-blur-draw` only after the native
`RenderObjectBlurs` fan(s) with Number>=2 and records bitmap registration.
The private Spin controller now calls native `RemoveObjectBlurs` at normal
EffectDestructor and immediately before map teardown clears its native effect
slot; a QA-only map style-1 blur count reports before/after records. Stock
SS6 blur modes and effect cleanup paths are unchanged. Source/build checks
PASS; no new ribbon pixels or post-map retirement log has been captured yet.

Status: `IN_PROCESS` — identity, local/receive dispatch, action, root controller,
controller constructor, sound registration and one child-model identity are
static evidence. The full root update fan-out, hit reaction and cleanup chain
remain open.

## Identity and dispatcher

2026-09-13 outcome telemetry: QA-only64-record buffer now records target resolve
outcome0/1 at the one-shot gate, and outcome2 when native InitializeEffect is
entered for Spin controller/cross/ground carrier. It captures type/subtype,
target index, Hero frame and elapsed clock; overflow is counted. F11/next cast
flushes with existing controller samples. No disk writes occur in these hooks.
This distinguishes invalid target from reaching child initialization, not full
constructor completion, rendering or visual acceptance. Other simultaneous Spin
effects may appear while the sampler is armed, so use one isolated cast.
Isolated QA build PASS; not staged and no new outcome log exists yet.

2026-09-13 recorded QA recheck: RuntimeQA/Client/GrowLancerRuntimeQA.log SHA256
c973c8767bf8fdf0b13a3f49ef4af8097a3c9b07ecd4faa854b4ba741158b9de contains396
Spin controller samples and69 samples in(4.7,6.5], with target indices-1 and3.
The targetless group enters the gate and advances stage0->1 but cannot resolve
a target. Other groups retain3, so missing target is NOT a complete diagnosis
of the owner-reported missing visual. inspect_spin_controller_log.py reports
per-cast groups without claiming allocation or render success. QA F7 now rejects
Spin without SelectedTarget before arming samples or dispatch; native skill logic
is unchanged. Isolated build PASS; no QA staging or new runtime acceptance.

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
