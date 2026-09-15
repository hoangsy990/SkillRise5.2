# Magic Pin (skill 274) reverse evidence

## Native bright-mesh fixture raster — 2026-09-15 (owner foot still FAIL)

Stock `OGL330MODEL::ConvertOldMeshToVaoMesh` rejects the private model-ID
tail (`>=MAX_MODELS`); the package-scoped native BMD upload correction now
gives all four Magic models real VAO/VBO/IBO (18/18 visible-model hidden
audit PID3248 PASS). Old `magic-submit` rows could still have fallen through
to legacy `BMD::RenderMesh` without a VAO, so they were not raster proof.

QA-only `RISE_GL_MAGIC_NATIVE_DRAW_QA=1` uses existing native `BMD::Open2`,
`Animation`, `RenderMesh(RENDER_TEXTURE|RENDER_BRIGHT)` and shader flush for
exact `magicpin01` 0x5E6 and `magicpina01_new` 0x5E8 material models. The
pose/projection are marked fixtures; no synthetic skill renderer was added.
Initial Bin PID8400 exited2 with shader disabled because the EXE-local shader
files were absent, so that run says nothing about Magic. Isolated staged
QA Engine 5C997622... with A129E20D... rollback/Data verifier PASS then
yielded hidden PID1900 exit0, shader enabled1, angular strip6484 samples,
annular root15804 samples, GL error0 and stable model/bitmap release.
`verify_magic_native_fixture_draw.py --pid 1900` PASS. This establishes only
that those exact two BMDs can draw fragments under native bright-mesh flags.
At the source scale0.7, real caster angle/height, depth and gameplay camera,
the owner's caster-foot triangle is still visually FAIL until a controlled
F7/F11/F9 pixel/query comparison on the current staged candidate.

The follow-up private QA Engine 570A7B99... retained an exact 5C997622...
rollback and reran the same native bright-mesh fixture at S21 model scale0.7
and Z0/90/180/270. Hidden PID24324 exit0: `magicpin01` fragments
3141/8456/3141/8456; annular root fragments7708 at all four angles,
GL error0 and release stable1. Focused PID-bound draw verifier PASS.
Only a fixed fixture projection was used; this rules out a zero-fragment
orientation *in these conditions*, not a gameplay camera/depth/timing problem
or the owner's missing foot visual.

## Two source-proven caster-foot model raster probes — 2026-09-15

The owner still sees no caster-foot triangle. The previous opt-in GPU query
covered only `0x5E6`/`magicpin01.bmd` at peak alpha; the source-proven
always-created `0x5E8`/`magicpina01_new.bmd` annular root was not sampled.
The QA-only `GL_SAMPLES_PASSED` diagnostic now covers both exact model
types in their existing native `BMD::RenderMesh` call, with independent
eight-query caps per F7 cast. It labels angular-strip versus annular-root
and retains the before/inside shader-queue flush, VAO/index/program,
depth and cull breadcrumbs. `verify_magic_raster_probe_contract.py` PASS;
isolated QA/non-QA Win32 link PASS. The private staged QA EXE SHA-256 is
`4972546B6230C5D1CC090C94F74FBCB20F2DC79E465DD97D4FD86E25601CD1EA`,
with pre-probe `587A2976...` hash-pinned rollback. Neither a query nor a
submit count proves triangle pixels: no cast on this binary has produced
root/strip samples, target burst evidence or matching-frame S21 comparison.

## Shader-queue raster probe correction — 2026-09-15

Native `BMD::RenderMesh` returns immediately when
`GMMeshShader->AddMeshCommand` accepts the mesh, and the actual GPU draw is
deferred to `FlushAllMesh`. The earlier QA `GL_SAMPLES_PASSED` query ended
immediately after `RenderMesh`, so a zero result on the shader backend could
mean *no draw occurred inside the query*, not that MagicPin01 was invisible.
The opt-in QA query now flushes prior queued meshes before opening and flushes
the MagicPin01 draw while still open, guarded by shader availability and only
under `RISE_GL_MAGIC_RASTER_QA=1`. It also records the native shader backend,
mesh VAO/index count, bound shader program, depth test and cull state so a
zero sample can be separated from a failed mesh upload, deferred enqueue or
depth/backface rejection. No production render path or SS6 skill batching
changed. `verify_magic_raster_probe_contract.py` PASS pins both boundaries;
isolated QA x86 link PASS, SHA-256
`3E5861785524CC539B30E8294290FD9D16A69D05B1A5443132F376742DBF070C`.
The older staged QA SHA `FD3CAE63213E8B8163616E5F8AFA592816F5571522DE43A44A7102F1C483B8D8`
was retained as an isolated rollback (and its own predecessor 43C2155C...
also remains private); full QA Data verifier PASS.
No cast on this candidate has yet produced a sample or pixels, so caster-foot
triangle and target burst remain owner visual FAIL/OPEN, not PASS.

## Caster-foot material UV and peak-alpha QA — 2026-09-15

The separate hash-pinned `inspect_magic_pin_world_z.py` follows the actual
native `BMD::Animation` yaw-only parent transform, QuaternionMatrix row Z,
the one source Plane02 action key and caster model scale `0.7`. Its local
vertices map to relative Z `-7.466..+63.311` (example earlier QA origin
Z184.5 gives `177.034..247.811`). This conditional calculation excludes
the whole 5E6 mesh sitting below the caster's own floor-height origin if
the effect has yaw only; it does not prove real terrain depth, caster pitch,
camera projection or GPU samples. New QA angle telemetry appends actual
effect Angle X/Y/Z to each render row so the yaw-only premise can be checked
in the next cast. The updated client linked but is not staged over live
PID25016; owner visual remains FAIL.

The new read-only `inspect_magic_pin_uv_alpha.py` pins the original `0x5E6`
`magicpin01.bmd`, its isolated v0C plaintext equality, and original
`wind1.OZT` before testing exact mesh triangle UVs against 64x64 alpha
texel centers. Eight triangles cover 3,968 unique texels; 3,573 have
nonzero alpha under either V orientation. Every triangle samples at least
78.27% nonzero alpha (V unchanged) or 82.70% (V flipped). One triangle's
mean alpha is only 3.187 in the unchanged orientation, so this is not a
claim that the full angular mesh is bright. Existing real QA PID36328 and
PID37752 each recorded `0x5E6` peak-alpha `1.0` native submissions; its
64x64 GPU texture was allocated. Missing caster-foot pixels therefore
cannot be explained by wholly transparent source UVs, zero peak alpha, or
no native mesh submission in those casts. World/bone transform, depth,
blend, shader queue/raster and exact photo attribution remain OPEN.

Added an explicit QA-only `RISE_GL_MAGIC_RASTER_QA=1` peak-alpha
`GL_SAMPLES_PASSED` query around `magicpin01` RenderMesh, capped at eight
per F7 cast. The local launcher defaults it OFF and exposes
`-MagicRasterProbe`; it restores the parent environment after launch.
Queries do not alter buff/server state. The Win32 candidate built, but the
older QA client is still live and its hash-pinned stage is WAIT; no new
sample query has run. A shader batch could defer draws beyond the query,
so zero or positive samples are only diagnostic, never visual PASS.

## Animation-pose and converted-asset equality — 2026-09-15

The hash-pinned read-only mesh inspector now follows the exact native
`BMD::Open` action header and bone Position/Rotation array order for all four
S21 Magic Pin BMDs. Every model has one action and one key, so these models
have no internal animated pose progression that could explain intermittency.
The angular `0x5E6`/`magicpin01.bmd` root bone `Plane02` has static position
`(-8.84291,-6.00705,27.04561)` and rotation
`(-3.141593,1.570796,0)` radians. The always-created circular
`0x5E8`/`magicpina01_new.bmd` bone `Cylinder03` is at
`(0,0,3.568491)` with zero rotation. `0x5E7` and `0x5E9` likewise have
one fixed bone key, with their nonzero positions/rotations reported by
`inspect_magic_pin_model_geometry.py --animation-summary`.

All four private staged RISE v0C BMDs were checked against their pinned
SHA-256 values and decrypted; each plaintext payload exactly equals the
decrypted original S21 v0F payload byte-for-byte. Thus the isolated format
conversion has preserved mesh, UV, bone and action data for these four models.
This does **not** prove native pixel parity: stage creation, owner/object
transform, depth, blend and GPU draw remain open. The absent owner foot
triangle is not justified as a missing animation key or damaged converted
asset by current evidence.

## Controlled owner retest — 2026-09-15 (still visual FAIL)

Owner visually retested isolated QA PID 36328 and reported **no caster-foot
triangle**. The process used the explicit 40 ms Magic clock and the four-model
render probe. Log SHA-256 after exit:
`4B53399A4932BC583ABB2ED3BF810B3E2CDDC3EABD51AB64B4367A3FF2D8F35B`.
The PID-filtered frame parser reports three consistent F11 groups, 408 owned
rows, 169 completed quanta, zero dropped/truncated groups, and
`visual_acceptance=false`. First Magic F7 selected target 18 and created base
274; all four caster BMD types 10125..10128 (5E6..5E9) reached native
`magic-submit` (17/14/37/13 first-cast submissions respectively). The first
controller did reach stage 1 at owner frame 5.508000851 and stage 2 after
frame 7.128002644; no stage 3/subtype-1 5E7 submission was recorded before
action changed to 231. Thus 40 ms stepping resolves the representative
stage-0 *non-creation*, but does not reproduce the foot triangle or prove the
three-stage S21 sequence. Ensure/calc/submit are not GPU pixels. The QA log
has no PID-36328 base-281 hit dispatch/F9: target white burst remains wholly
untested in this retest. The GS `RunningSkill` branch still returns false for
both 274 and 281; no authoritative hit producer can be inferred from QA F7.

The opt-in clock was also found to run while unrelated action 231 had no live
Magic record, which could perturb other QA animation. A QA-only guard now owns
frames only during local action 287 or while one of the five private Magic
model types remains live, including target-owned internal-281 controller.
Controlled extracted frame-flow and ownership tests PASS, isolated Win32
client build PASS, new candidate/staged hash
`141C2859677F2C4682491A6E0B83BB22B62087E988F8FE0081F008C53DA142B8`.
The prior 63EC... binary is retained hash-pinned under isolated RuntimeQA/Bin;
stage verifier PASS. This new clock-guard candidate has **not** been retested
ingame. No ordinary SS6/global renderer or production client was changed.

After the owner visual FAIL, the hash-pinned mesh inspector was extended to
decode actual vertex/triangle indices, not only min/max bounds. The always-
created `0x5E8` source mesh forms a twelve-segment annular wall, with paired
vertices at local Z0 and Z108.5; `0x5E7` is a long tapered radial mesh and
`0x5E9` a shallow circular disk. `0x5E6` instead has only ten vertices/eight
triangles arranged as two opposing tapered strips with local Z both positive
and negative. **Inference:** among these BMDs `0x5E6` is the stronger angular
foot candidate, while `0x5E8` is the blue circular surround; this does not
exclude a bitmap/joint/sprite layer or prove screenshot attribution. PID36328
already showed `0x5E6` submit at caster world Z184.5, so a render submission
alone cannot explain the absent owner pixels. A QA-only material probe now
records each private BMD's embedded texture filename, exact bitmap-cache path,
CPU/GL texture dimensions and GL allocation without persistent GL rebinding.
Isolated Win32 build/staging/verifier PASS, current QA hash
`8F544F139D46AD446427A7E0D3FC6322BB6C80CC191362572199D426BC625C67`;
it has not been ingame retested. The result will distinguish a missing/wrong
material from transform/occlusion/shader/raster causes, not prove pixels itself.

The subsequent isolated cast on that material-probe binary, PID 37752,
recorded **all four exact cache paths and GL allocations**: model types
10127/10125 share private `wind1.tga` bitmap ID35442, while 10126/10128
share private `alpha_line03.tga` ID35443; both have CPU and GPU 64x64,
four-component textures and `exactPath=1/gpuAllocated=1`. The first stage
material sample appeared at 1344 ms. One F11 frame group had 104 owned rows,
42 completed quanta, no dropped/truncated group. The owner still has not
accepted the angular foot layer; bitmap allocation is narrower than visible
pixels. Hash-pinned read-only OZT inspection following the native `OpenTga`
offsets found source `wind1.OZT` alpha nonzero in 3663/4096 pixels (max241),
and `alpha_line03.OZT` nonzero in 3058/4096 (max255). A wholly transparent
source texture is ruled out, but mesh UV sampling, GL raster/shader execution,
depth/occlusion and native-vs-S21 transform are still open. PID 42588 was
launched from the same verified QA stage for separate F7/F9 observation;
no contact result was recorded at launch time.

## Owner S21 pixels and isolated QA gate diagnosis — 2026-09-15

Two owner-supplied native S21 gameplay screenshots (SHA-256
`2B8E0C355DEA85CD22EF4657971AAC1C58617F3953B8769EDFB486D60A167E68`
and `0387109B665943138B9B73D9B81F9996B25FF2BCD44E3BDF1C1BD6D2CFA71A66`)
show a large white-blue burst at the target and blue-white angular/triangular
visuals around the caster's feet. The owner reports that 5.2 Magic Pin is
intermittent and misses both layers. These crops are native visual references,
not resource-ID or spawn-edge evidence: the precise foot primitive must still
be assigned among the always-created `0x5E8` root and stage-gated
`0x5E6`/`0x5E7`/`0x5E9` children by model/particle QA. The target burst is
not a base-274 root child: S21's received internal skill 281 creates target-
owned `0x5B9` subtype 1 at target Z+140; isolated F7 currently makes only the
base cast, while F9 explicitly previews that separate hit route for a selected
target. Neither F9 nor an auto-preview is server-authoritative damage evidence.

Hash-pinned offline BMD mesh inspection now narrows the caster-foot candidates
without claiming a screenshot-to-model match. The always-created `0x5E8`
`magicpina01_new.bmd` has 24 triangles using `wind1.tga`, with local Z range
`0..108.5015` and XY extent ±139.4. The stage-0-only `0x5E6`
`magicpin01.bmd` has 8 triangles using the same texture, local Z range
`-149.2761..149.2759`; `0x5E9` has a nearly flat `alpha_line03.tga`
mesh (48 triangles, local Z about `-8.46..8.03`) but is born from `0x5E7`.
All four source BMD SHA-256 values are checked by
`inspect_magic_pin_model_geometry.py` before decrypt/layout validation. These
are model-local bounds only: action/bone transforms, world Z, render pass and
pixel attribution still require the new native QA probe and S21 comparison.

Staged QA PID 28160 captured a full local Magic Pin action 287 with controller
stage remaining 0 throughout. Its sampled owner frames jump
`4.991940975 -> 5.877425194` across the exact S21 stage-0 gate `[5.2,5.8]`;
later `6.463793755 -> 7.355614662` and `9.985474586 -> 10.864792824`
also skip narrow gates while stage 0 has not advanced. Other rapid casts show
action 34 at the controller's first sample despite action 287 immediately
after F7 dispatch, so they expire before any stage child. This is direct native
QA evidence for missing child *creation* in those casts, not a renderer verdict
for all four BMDs. The PID-specific magic-frame inspector found 41 groups,
2 dropped groups, 0 truncated groups, 2638 rows, 0 owned rows and 0 completed
40 ms quanta: the clock opt-in was OFF. Thus the staged opt-in has not yet been
live-tested and the owner visual FAIL remains.

A new QA-only probe records `magic-ensure`, `magic-calc` and `magic-submit`
through the existing native BMD path for the four private Magic models. The
isolated x86 QA build linked PASS, candidate/staged SHA-256
`63EC10F15FC8C0908B41AF2B749F776FCF160A43E9A4978B51E11C76773DDCC0`;
the prior staged D36C... executable was hash-pinned in a private rollback
backup. Stage verifier PASS after staging. The launcher now offers explicit
`-MagicFixedTick`, scoped to its child process and OFF by default even when
the parent shell inherits an old opt-in. Source/clock tests PASS, but no new
process has cast with this candidate yet. Do not claim corrected foot pixels,
target burst, all three stages or S21 parity from the build and probe alone.

2026-09-13 primitive phase order pinned: selected source scene18F4140 invokes
character traversal,1595834 model effects,1618EF4 joints, then1620972 particle
wrapper in that order. Complete joint traversal iterates500 primary slots, reads
Live per slot and calls15E5838, then processes secondary10F0381. Complete wrapper
1620972 forwards to16AA2ED. This reuses and corroborates the earlier Breche shared
phase findings, not a new guess from adjacent addresses. Native MoveMainScene
has the same character/effect/joint/particle ordering. New pinned/source verifier
PASS. Private catch-up must place these phases inside each tick, not execute all
model ticks before all particle ticks. Particle container iteration, insertion
while iterating and sprite update/render lifecycle still require separate coverage;
this phase result is not full primitive pool closure or runtime QA.

2026-09-13 Magic model pool routing closed: hash-pinned complete selector bodies
10EF919 and10EFBA1 were decoded and their reached comparison paths evaluated
for5E5/5E6/5E7/5E8/5E9. Each reaches XOR EAX,EAX followed by LEAVE/RET8.
Second selector also returns zero when its map predicate is false; when true its
type path still returns zero. Allocation143E674/143E688 therefore skips secondary
allocation10EFEFB and retains the primary pointer for these five model types.
No subtype/owner input is read on their reached selector paths. Verifier PASS.
This closes model-record routing only; bitmap particles, joints and sprites
remain separate primitive contracts, not implied primary-model allocations.

2026-09-13 effect birth-order audit: complete primary traversal1595834..15958E9
reads Live at each increasing slot, not a frozen list. Conditional consequence:
a child allocated to a later unvisited slot can update this tick, while one
allocated earlier waits until the next traversal. Allocation143E5F0 prefix also
has primary scanning and conditional secondary selection; Magic child routing
through both selectors is not yet established, so no specific slot claim made.
Pinned verifier PASS. Ordering fixture demonstrates tick-major2 passes give a
later child born on tick2 one update, while record-major two-tick catch-up gives
it two. Earlier child gets zero in both fixture cases. C++14 x86 W4/WX PASS.
Fixture is a scheduling counterexample, not an allocator runtime test. Private
correction must preserve tick-major actor/pool order, not merely replay owner
frames while leaving all root ticks ahead of child ticks. Do not force immediate
child updates regardless of slot position. No scheduling code changed yet.

2026-09-13 character-level step-factor propagation: CharacterAnimation keeps its
original public signature for character preview/legacy callers and delegates to
CharacterAnimationStep with the existing global factor. Explicit step now flows
through all native speed modifiers and BMD::PlayAnimationStep without global
factor mutation. AnimationCharacterStep retains original NPC processing, action
completion and stop handling; ordinary AnimationCharacter wrapper passes the
same global factor as before. Source verifier compares both algorithm bodies
against repository baseline, allowing only explicit factor/call substitution;
PASS. Native clock verifier and isolated Win32 build PASS, candidate
17C358B80F840103AD566E828635640D356F4F2427978BA40C15B299F5639A1A.
Not staged. No fixed-tick Magic caller activated yet: child birth/update ordering
must be preserved across catch-up ticks, not only owner-frame sampling.

2026-09-13 encoding blocker resolved and native step-factor API added: pinned
comment formatter found exactly10 invalid UTF-8 bytes, all lexically within
comments. It backed up original831a4c0b... to GrowLancerBuild/
ZzzBMD.before-comment-format.bin and replaced only those bytes with ASCII hex
annotations. Code/string bytes were preserved by formatting; apply_patch could
then refactor the native method. PlayAnimation retains its original signature
and delegates to PlayAnimationStep with FPS_ANIMATION_FACTOR. The new nonvirtual
entrypoint accepts an explicit per-call factor and reuses the original body,
without shared-factor mutation, new fields, loader duplication or alternate
renderer. Baseline test compares that body against pinned backup after only
normalizing CRLF and substituting the factor operand; extracted native constexpr
tests PASS for legacy calls, explicit1 step, wrap/hold/lock/guards. Initial test
mismatch was newline normalization, corrected without weakening body comparison.
Full isolated Win32 build PASS, candidate671682A79E414BA6A131CED9E4792D93A38AD94048591348D995D17E0A7DB50D.
Not staged; private Magic timing has not yet selected the new entrypoint.

2026-09-13 explicit-step API attempt NOT applied: apply_patch rejected legacy
ZzzBMD.cpp as invalid UTF-8 at byte62492. Verified neither header nor implementation
contains PlayAnimationStep afterward. No encoding conversion or manual byte rewrite
performed. Added test_native_animation_baseline.py instead: reads source losslessly,
extracts the actual animation body into an isolated compile fixture and checks
fractional factor, blend prior, modulo wrap, held end, locked positions and early
guards. x86 W4/WX C++20 constexpr fixture PASS (C++20 is test-only; production
language settings unchanged). Original BMD bytes are untouched. A supported,
encoding-preserving refactor path is required before adding an explicit-step API;
do not temporarily mutate the shared FPS factor as a workaround.

2026-09-13 observation consumer connected to QA: ReadCharacterAnimationObservation
validates live Magic subtype0, local owner, scene, actor identity and actual primary/
secondary slot, then reads via that controller's cursor. Duplicate binding does not
rewind it; repeated catch-up cannot consume the same observation twice. QA rows now
append observationAvailable/observedAction/observedFrame, with explicit unavailable
sentinels. This allows comparison of post-animation state with later controller
sampling without using observations for emissions. The sampler fixture uses a
clearly marked consumer stub for wiring; real batch/cursor semantics have separate
actual-helper tests. Sampler, source-hook/lifecycle verifiers and Win32 build PASS.
CandidateD8DF01E9F90AD15AB322A3CB5B707AD0397BE5659068DDE55D16EFE598ED2D92
not staged. Missing observation is not proof of a dropped source tick: producer
frequency remains native fractional animation calls and buffer holds latest only.

2026-09-13 native observational producer connected: after AnimationCharacter,
MoveCharacter invokes PublishCharacterAnimationObservation. It only accepts the
armed private local actor identity in gameplay, records actual action/frame as a
single published observation, and captures the first action exit before disarming.
Null/changed hero, dead actor, wrong type/scene or serial exhaustion invalidate
identity/cursors/observation; same-action local recast can rearm without inventing
a frame reset. The observation is explicitly NOT a40ms tick and must not be
replayed as several virtual ticks. No BMD advancement, interpolation or effect
emission occurs in this hook. Hook verifier and isolated Win32 build PASS;
candidate765DC2DC3953D11F0B8BCE69E9F3CB817F86F0A233063F6F5FCE47E6916E1498
not staged. No consumer or coherent fixed-tick animation production yet.

2026-09-13 private actor identity hooked: local approved Magic root preparation
binds an opaque actor-address token to a monotonically allocated generation.
Same bound actor retains identity; retirement then reuse of the same address
allocates another generation. Both DeleteCharacter overloads and reconstruction
through CreateCharacterPointer retire only the matching private actor; map reset
invalidates identity as well as cursor arrays. Generation exhaustion refuses new
binding, never wraps to a stale ID. This identity is not a copied S21 pointer or
new OBJECT field. Tests use a small generation limit to exercise exhaustion;
x86 W4/WX test, lifecycle source checks and isolated Win32 build PASS.
CandidateBB0029457ADA810E48567FEE80BA8686BC54FD530458E262860DE142EEBD7543
is not staged. Identity/cursor infrastructure remains unconsumed by animation
timing; next producer must check successful binding explicitly. Current casts
retain their existing behavior if binding fails, since timing is not enabled.

2026-09-13 native private cursor reset hooks integrated: separate arrays retain
MAX_EFFECTS primary and MAX_SKILL_EFFECTS secondary cursors, without OBJECT
layout changes. Numeric address validation checks actual pool base, stride,
alignment and index bounds without subtracting unrelated object pointers.
CreateEffect resets after selecting the actual object and before Live/type writes,
including ordinary SS6 reuse and resource failure. EffectDestructor resets that
slot; DeleteObjects resets both cursor arrays before its direct pool clearing.
Unknown/null objects are rejected by the private lookup. No owner pointer, Live,
skill lifecycle, renderer or animation field is changed by these hooks.
Slot-bound tests and lifecycle-site verifier PASS; isolated Win32 build PASS,
candidateSHA256 EA4DB0FAD1DCF25FF06A849F655C168E45F13DEBE8575639E5BE8BCF401465A2.
Not deployed. Cursors are reset-only infrastructure until actual producer/consumer
and actor-generation handling are connected; no cadence correction claimed.

2026-09-13 lifecycle insertion audit: native CreateEffect may replace its primary
pool pointer with a secondary-pool object before allocation. Never index sidecar
state with icntEffect without checking the actual selected object. Resource-load
failure clears Live and returns before imported InitializeEffect, so resetting
only in that initializer misses allocations. EffectDestructor clears Live/Owner,
but CMapManager::DeleteObjects clears primary Live flags directly without invoking
it. verify_animation_sample_lifecycle_sites.py records these source boundaries,
PASS. A correct handoff integration must cover actual allocation, destruction and
map reset, including slot reuse by ordinary SS6 types. No lifecycle hooks have
been inserted yet and no pointer-to-index assumption has been adopted.

2026-09-13 handoff publication/cursor contract: experimental batches require
explicit Publish before any read. Published frames are immutable; append rejects
without destroying published data. Pre-publication overflow invalidates the whole
batch and cannot publish. Each controller can hold its own AnimationSampleCursor;
successful reads advance only that cursor, failed reads preserve outputs/index,
and same/backward serial rebinding is rejected within an actor generation.
Producer Begin also rejects backward serial. Caller must allocate a new actor
generation before serial rollover and reset cursors on pool lifecycle events.
Expanded actual-helper constexpr tests compile PASS x86 C++14 W4/WX for partial
publication, independent readers, stale batches, duplicate consumption, backwards
serial, overflow and reset. This is single-threaded storage, not an atomic queue
or ownership guarantee. Native hooks/generation source/cursor placement and timing
are still pending; no source runtime behavior or visual acceptance claimed.

2026-09-13 per-step action exit: experimental AnimationSampleBatch now stores
action with every frame. ReadStep returns the actual step's action; the older
action-filtered read refuses a step after that action has exited. New fixture
uses actual MagicPinStageDue against supplied287/5.5,287/7.2,11/0 samples and
proves the first two emissions remain eligible before exit. A controller using
only final owner action11 would incorrectly discard both. C++14 x86 W4/WX
tests PASS, including stale-identity read output preservation and second-reader
access to the same earlier sample. These supplied frames establish ordering,
not source speed/timing; native producer/consumer integration still pending.

2026-09-13 experimental handoff storage: compat/AnimationSampleBatch.h stores
caller-supplied actual frames, keyed by explicit actor generation, batch serial
and action. Reads do not consume samples, permitting multiple controllers;
overflow/invalid frames invalidate the entire batch instead of silently losing
a tick. It does not derive samples from blend frames, interpolate gates, reset
animation, age effects or alter pools. Compile-time C++14 x86 W4/WX tests PASS
for broadcast reads, stale identities, action changes, repeated batch rejection,
wrapped frame preservation, overflow, invalidation and untouched failed output.
This is NOT integrated and NOT a timing fix. Producer hook, generation lifetime,
serial rollover, batch publication, per-controller cursor and action-exit ordering
still need integration. In particular, a final owner action cannot be substituted
for every intermediate step if an action ends during a multi-tick scene update.
Capacity3 in the test is a fixture, not a claimed source pool/packet limit.

2026-09-13 transition sampling regression: extracted actual native SetAction
and actual controller sampler are now compiled together in the existing bounded
fixture. Assertions cover transition11->287 preserving4.25 as prior frame,
same-action287 retaining current5.5, exit287->11 preserving5.5 and exposing
the new action, and rejecting action300 with NumActions300. All PASS.
Important adapter boundary: PriorAnimationFrame is blending state, NOT the
previous simulation sample. Native BMD::PlayAnimation stores the integer old
frame only when the integer frame changes; SetAction stores the outgoing clip
frame on a transition. Never interpolate pending Magic samples from that field.
The proposed coherent sample adapter therefore needs its own explicit previous
sample and action/epoch identity. No queue or scheduling change is integrated
yet; this test covers setter/sampler composition, not BMD stepping/pool reuse.

2026-09-13 ordinary gameplay call-order audit: MainScene calls CalcFPS before
MoveMainScene. Selected MoveMainScene has one MoveCharactersClient call before
one MoveEffects call. Character traversal calls MoveCharacterClient, which calls
MoveCharacter before visuals; MoveCharacter calls AnimationCharacter, which calls
CharacterAnimation. Effect traversal subsequently invokes MoveEffect for each
live pool slot. Added source-path checks to verify_native_animation_cadence.py,
PASS. This agrees with S21's character-before-effect ordering; swapping these
global traversals is not justified. Native whole-effect catch-up occurs after
the fractional owner advancement, so repeated catch-up updates cannot reconstruct
intermediate owner frames themselves. Source token checks are not proof of all
preprocessor variants, callback side effects or runtime invocation counts.
Next adapter must keep owner animation samples coherent with controller ticks,
including same-action recasts, action exit/wrap and pool reuse; do not mutate
shared owner state temporarily inside child updates or widen the source gates.

2026-09-13 native clock units checked: ZzzAI.h REFERENCE_FPS is25, and CalcFPS
computes factor=min(25/(1000/elapsedMs),2.5). Thus the native unit is already
40ms, not a30Hz unit needing conversion. At200ms it represents only100ms because
of the cap. verify_native_animation_cadence.py checks actual source expressions
and six numerical cases; PASS. This does not prove scene call multiplicity or
source/native overload parity. Keep the global SS6 cap unchanged; the remaining
Magic issue is fractional owner sampling vs whole controller ticks, not a proven
25/30 clock mismatch. Existing offline snapshot reader was rerun: cached source
action188 speed=.705999970 at snapshot374d1ecb..., not a cast/last-writer proof.
Receive verifier also confirms source82/04 writesWORD+1B6 from payload+4 and
calls display refresh, not direct speed registration. These are existing proven
inputs to use, not missing work to reverse again.

2026-09-13 QA sampling instrumentation: bounded before-update controller rows
for Spin/Magic/Harsh now append priorFrame and actionSpeed. Speed is read from
the native player action table only after model/type/action bounds and action
array checks; -1 explicitly means unavailable. It is the registered PlaySpeed,
NOT final post-modifier frame delta or GPU visibility time. Existing512 sample
capacity and deferred disk flush remain unchanged. Extracted actual sampler
test now stops at the Spin-outcome function boundary, avoiding accidental
inclusion of that separate hook; tests cover prior frame/speed, saturation,
filters and five unavailable-speed cases. Test and isolated Win32 build PASS.
New candidate SHA25638692699D7FE6A7E863A76AAE64072F4D71CED94947CB7B08FD44E96A2488AD8
is in RuntimeQA/Bin only. Client process3712 remains live, so no EXE overwrite
or restart was attempted. No new sample or visual acceptance yet.

2026-09-13 received setter and speed-input audit:12CB397 compares the received
character against globalA484634; equality jumps12CB488 past the remote action
and root block. The other branch pushes alternate-1 and action188 to1327DE8,
matching the local setter contract. Extended action-reset verifier PASS.
Speed registration verifier now accepts the native SetAction transition while
still requiring preparation before it. Additional source pins establish that
1408580 initializes local-8 from unsignedWORD stat+1B6 and local-4 from+1C2;
after adjustments, local-8 is argument8 to1408811 and feeds the common .002
term used by action188. This identifies the source data path, not semantic
stat-name/SS6 field equivalence; bonus producers and actual cast value remain
open. Both verifiers PASS. No new client mutation or live capture in this audit.

2026-09-13 action transition correction: local10E4170 pushes alternate action-1
and primary188 before1327DE8. Its alternate sentinel branch selects1327D72.
Complete setter decode proves identical action returns without frame writes;
on transition it copies current action/frame to prior action/frame, assigns the
new action and zeroes only current frame. Native SetAction(...,true) provides
this behavior. CreateMagicPinRoots now uses SetAction(&caster,287,true) instead
of direct action assignment and unconditional zeroing of both frames. Scope is
Magic only; resource preparation remains first, emission gates are unchanged.
verify_magic_action_reset.py pins source branch/writes and native integration;
isolated Global Release Win32 build PASS. Not staged or runtime validated.
This corrects transition/repeated-action semantics, not the missed-gate cadence.

2026-09-13 synchronized sampling counterexample: source animation-step verifier
PASS confirms character traversal before effect traversal and the 40ms update
quantum. Extended GrowLancerMagicRecordedGateTest uses the actual action188 speed
formula and gate helper with explicitly assumed zero initial frame, constant
adjusted speed and one advance/sample per tick. Adjusted inputs0/100 reach all
three gates; the initial class-profile cap273 reaches only two: .706 increments
skip the third window between approximately9.884 and10.590. x86 C++14 W4/WX
compile PASS. This is a conditional counterexample, NOT evidence that a live S21
cast drops its third hit. It rules out claiming shared cadence alone guarantees
all three emissions. Actual cast speed, initial/reset frame and controller stage
writes must be checked together before selecting the native scheduling change.
No gameplay comparisons, speed caps or emission policy were changed.

2026-09-13 sampling audit: current MoveEffect sends Magic controller through
AdvanceWholeTicks, which only calls UpdateEffect when Timer accumulates1 and
supplies factor1. BMD::PlayAnimation advances owner frame by Speed multiplied
by FPS_ANIMATION_FACTOR on each animation call. These are distinct sampling
cadences; accumulating effect ticks alone does not reconstruct intermediate
owner frames. Recorded PID16052 stage0 gap5.001->5.883 misses[5.2,5.8].
GrowLancerMagicRecordedGateTest.cpp replays all16 pre-idle frames through the
actual MagicPinStageDue helper and proves stage remains0; later7.354/10.004
frames satisfy stages1/2 only, so cannot rescue the missed first stage.
Helper made constexpr without changing comparisons; x86 W4/WX and client build
PASS. This reproduces the historical failure, not a timing fix. Shared S21
producer/consumer cadence and current-build capture must guide a correction;
do not widen gates, emit missed stages unconditionally or remove child ticks.

Status: `IN_PROCESS` — identity, cast/receive dispatch, action, base controller
timing, internal explosion receive branch, four model identities, child update
semantics, render paths and all currently proven assets are pinned. Server
three-hit scheduling and the native 5.2 adapter remain open.

## Base cast

- The common local dispatcher enters Magic Pin at `0x010E3FF2`, performs its
  packet-rate gate, then sets action 188 through `0x1327DE8`.
- Both local and received base-skill-274 paths create type `0x5E5` subtype 0
  and type `0x5E8` subtype 0, owned by the caster. The received switch proves
  skill 274 by subtracting 201 and mapping index 73 through selector 24 to
  `0x012CB397`.
- The received branch plays sound `0x459`, registered as
  `Data\Sound\MagicPin.wav`.

## Controller `0x5E5`

Thunder audit found an incomplete adapter: native BITMAP_ENERGY constructor
and update implement subtypes1..7 but NOT9/10. Calling the right texture
primitive alone does not establish equivalent behavior. S21 child constructor
1480B6D/1480BBB explicitly pushes7F2B for particle10/9. Registry also has
8148 with the same Thunder01 filename but different wrapping; it must NOT
be substituted based on filename. Exact dispatcher compares1641908/16AA98D
select constructor16508CA and update16BF9D6.
Earlier statements that native primitive preserved this whole dependency are
superseded. Constructor1650BF2..1650D9B now maps both subtypes to life3,
scale1.5, alpha.5 and white RGB, retaining common constructor random draws.
Update16C01D3..16C03D0 resets rotation0; subtype10 grows scale+.45 per tick.
Both compare CURRENT remaining life against integer life/2+1, so life2 AND1
multiply R/G by.55 (B unchanged). Only subtype9 subtracts alpha.32 within
that conditional; life3 alpha remains.5, then.18,-.14. No alpha clamp.
Native private whole-particle ticks execute life3,2,1, then pool Live=false
at the next expiry check, without a life0 update. Per-slot remainder resets
on every allocation, including ordinary SS6 reuse. Velocity starts zero;
the selected update writes no position. Shared helper regression24 cases
includes eight Thunder runs across four FPS factors and checks every sample.
Win32 isolated build PASS; GPU and actual pool stress remain OPEN.

Thunder renderer follow-up: actual comparison tree1620BE0..1620D55 for
7F2B reaches default1640DD2, not the separate8148 renderer. Default passes
full UV(0,0,1,1), rotation+60, rawRGB+38, position+20 and texture+10 to
18E8B0C at1640E6E. It neither reads alpha+44 nor multiplies RGB by it.
Thus subtype9 alpha changes are preserved as state but must NOT be added
as an extra native render fade. Shared setup1620AD0..1620B52 looks up
the texture, scales dimensions by particle scale, selects additive for RGB;
18E7137 pushes blend factors1/1. Native default RenderSprite raw Light and
rotation already match these selected arguments. verify_magic_thunder_renderer.py
walks the pinned comparison tree and checks selected callback/source arguments.
This does not establish sprite raster internals or owner visual acceptance.

Renderer follow-up: registrationA1B1F1..A1B234 maps5E6->A48EFA,
5E7->A49029,5E8->A49189,5E9->A492BB. MagicPin01 A48FEF supplies
literal1 blend light, notAlpha. Its UVs A48FDD/A48FE6 are zero; rootUVs
A4926C/A49275 alsozero but root blendlightA49281 remainsAlpha.03/Aux
incrementV by.035 atA49044/A492D6 and passUV fields. Private renderer
now distinguishes these; SpinCross in shared branch stays unchanged.
All four child scalar fixes and this renderer correction linked/staged as
AE87B3740DA5EAA096034C57E32810050219FE5C0F1B8777500AE7CC294D525D.
Win32linkPASS; build/static/staging do not prove visual acceptance.

Focused regression follow-up: scalar updates extracted to private shared
GrowLancerMagicPinTick.h and called by real runtime plus x86 test.
16cases (four children/fourFPS) PASS11/11/15/11 visits,single cleanup,
01 finalAlpha-.2,03/Aux triangles,root finalAlpha.43/angle+300,
Aux finalScale~1.2543417. Removed root's unused default-range clamp to
match the source directly; no global helper changed. Native vector movement
stays in runtime and is NOT covered by this scalar test. Project inclusion
3cpp/21headers PASS,QAClCompilePASS,notlinked/staged. Existing7650105C
candidate remains current pending renderer audit/link.

Follow-up5E7/5E8/5E9: selected bodies15786D7..15788B1,
15788B6..1578906,157890B..15789D8 reach common15945F1, so initialize
Timer0 and use private whole ticks, yielding11/15/11 update visits.
5E7/5E9 threshold and denominator are5, peaks.8/.75; existing symmetric
formula agrees on reachable integerlife10..0, so no new envelope needed.
5E8 alpha never reaches its existing clamp with defaultlife14 (15updates,
final.43), but actual renderer/frame QA remains open.

Aux scale correction:157891C calls96A03C with literalfloat30; wrapper
converts to double, calls1A22CC0 then rounds result to float. CRT fallback
1A36D8F contains fsin1A36DAD: radians, not degrees.1578930 flips sign,
1578937 multiplies by.005,157893F adds to scale. Replaced invented-.0025
with -float(sin(30.0))*.005, positive approximately.004940158 per tick.
Private calls use native math/runtime, no S21 pointer/address transplant.
ClCompilePASS; extended pinned verifier covers sin call/constant evidence,
not execution parity of CRT fast path, actual pool or rendered scale.

Latest child5E6 correction: update1578657 divides stored maxLife10 by2,
then adds1/5 when life>5, otherwise subtracts1/5. It does NOT recompute
triangular alpha and has no clamp before15786D2->15945F1. StartingAlpha0
therefore yields.2,.4,.6,.8,1,.8,.6,.4,.2,0,-.2 over life10..0 (float
rounding applies). Private MagicPin01 Timer0/whole-tick path added and
SymmetricAlpha replaced by incremental step. ClCompilePASS, not linked or
staged. verify_magic_pin01_alpha.py pins opcodes/source integration only;
actual runtime samples/renderer/pool remain untested.

- Constructor handler `0x1480871` sets lifetime 75, stage counter 0 and scale
  1.0. Its renderer is controller-only; visual children are emitted by update.
- Controller correction:1578320/1578495/1578581/1578613 push the controller
  itself as owner of all four child creations, NOT its caster owner. Angles
  and light also come from controller164/EC. First transformed point uses
  controller angle15783AB and position15783EF/1578425/157845A, not caster
  position at emission. Native adapter now preserves this snapshot transform
  and child owner. Caster remains the source of action/frame validation only.
  Constructor subtype0 explicitly sets scale1 and Timer0; private whole-tick
  dispatch executes through life0 using common15945F1 ordering. Synthetic
  hit subtype1 is left outside this timing change pending its separate audit.
  verify_magic_controller.py PASS pins owner/transform sites and source
  integration; isolated Win32 ClCompile PASS, not linked/staged yet.
  Low-FPS animation-window sampling and real child-owner reuse remain open.
- The exact effect-update switch maps `0x5E5` to `0x1578288`. It destroys the
  root when owner action is no longer 188.
- At owner frame `[5.2,5.8]`, stage 0 creates `0x5E6` subtype 0 and computes a
  transformed point from local offset `(0,20,30)`. It creates `0x5E7` subtype
  0 at that point and retains the point in controller vector `+0x110`. The
  `0x5E6` root itself is created at the controller's untransformed position.
- At frame `[7.0,7.5]`, stage 1 creates a second `0x5E7` subtype 0 at the
  retained point. At frame `[10.0,10.5]`, stage 2 creates `0x5E7` subtype 1 at
  the same retained point. These are the three distinct timed layers that
  match the skill's three server damage multipliers; the exact packet/hit
  association still requires server call-chain proof.

## Internal explosion receive path

- Separate damage route now pinned: opcode23 ->12FEB6B ->128FE90; key bytes3/4
  high bit controls reaction call, key masked7FFF resolves actor, damageDWORD8
  and type byteD low nibble passed to140A9C0. That helper excludes actions then
  allows damage types12/15 OR positive damage. Native opcode11/SetPlayerShock
  is not the same wire contract. Full reaction branches and the GS producer
  tying damage types/timing to Magic Pin remain open; do not force shock281.
- Prefix5372..5540 traces received secondary object into1948, then conditional
  facing writes caster1944.Angle.Z, and target index/success/skill to caster194C.
  Before dispatch140DC35 receives caster194C and normalized skill1950, not
  the target. These are not target hit-action writes. Metadata90==5 additionally
  suppresses facing in S21. Pinned full snapshot374D1ECB confirms281 record
  164EF404: byte0=0,key4=281,category90=0 (also280 category0); this is not
  inferred from GameServer XML UseType. Thus category5 facing suppression
  does not apply to base281. Helper140DC35 category0/key281 passes the6/15
  exception test, sets caster31E8=1 and timestamp31EC, then conditionally
  visits auxiliary-manager descendants. It is not a target hit-action write.
  These S21 fields remain evidence, never native offset transplants.
- `verify_magic_hit_receive.py` pins complete selected12CB4A8..12CB599
  and return tail12CE702..12CE71C. No direct1327DE8 action dispatch in these
  scopes; native CreateMagicPinHit likewise only creates target-owned visual
  and sound. This does not exclude receiver prefix, constructor side effects
  or separate damage packets from causing target reaction. Sound call passes
  (45A,0,0) to A74F94; native PlayBuffer has a different interface and its
  target pointer is used only for Enable3DSound. Registration currently uses
  default Enable3DSound=false, so pointer presence alone is not a spatial-audio
  discrepancy; playback scheduling/loop semantics still need explicit mapping.
- `SkillList.xml` row 281 is Grow-Lancer-only internal skill
  `Magic Pin Explosion`, type 1/use type 6.
- The received switch for IDs `279..298` maps 281 through selector 2 to
  `0x012CB4A8`. It copies the received target position, adds `140.0` to Z,
  creates type `0x5B9` subtype 1 with the target as owner, and plays sound
  `0x45A` (`MagicPinExplode.wav`).

## Registered models and current constructor evidence

- `0x5E6`: `magicpin01.bmd`, constructor lifetime/max-lifetime 10, scale 0.7,
  alpha 0.
- `0x5E7`: `magicpin03_new.bmd`, constructor lifetime/max-lifetime 10, scale
  0.85, alpha 0; subtype 0 randomizes all position axes by `rand()%50-25`.
  The constructor transforms local offset `(0,-330,95)` and emits
  `Thunder01.jpg` at that point: particle subtype 9 for child subtype 0 or 10
  for child subtype 1, scale 1.0, null owner and a zero S21-only eighth
  argument. It also transforms `(0,0,-30)` and creates
  one `0x5E9` child at the derived point.
- `0x5E8`: `magicpina01_new.bmd`, constructor lifetime 14, scale 0.7, alpha
  1.0, and increases angle Z by 0.1.
- `0x5E9`: `magicpina02.bmd`, constructor lifetime/max-lifetime 10 with alpha
  1.0 and scale 1.2.
- The registered renderer for `0x5E5` is a no-draw stub. Renderers for
  `0x5E6` and `0x5E8` draw mesh 0 with flags `0x42` and light
  `(0.48,0.73,1.0) * alpha`. Renderers for `0x5E7` and `0x5E9` use the same
  light and mesh flags while also advancing texture V by 0.035 per render and
  passing their animated U/V offsets.
- `0x5E6` update fades alpha in/out around half-life. `0x5E7` has a triangular
  alpha envelope over its ten-tick life, capped at 0.8. Each update rotates
  local translation `(0,1,0)` by the object's angle and adds the result to its
  world position. It emits no additional children during update.
- `0x5E8` update adds 20 degrees to angle Z and subtracts 0.038 alpha per tick.
- `0x5E9` has a triangular alpha envelope over its ten-tick life, capped at
  0.75. Each tick also applies `-sinf(30.0) * 0.005` to raw object float field
  `+0xA0`. The field is deliberately not assigned an SS6 semantic name until
  the object-layout mapping is proven. Its renderer advances texture V by
  0.035 and draws mesh 0 with flags `0x42`.
- All four v0F models decrypt as one-mesh/one-bone/one-action BMDs and convert
  to v0C with byte-exact plaintext round trips.

## Explosion controller `0x5B9`, subtype 1

Ownership follow-through for model5E7 (magicpin03): complete selected update
15786D7..15788B1 calculates alpha, rotates local(0,1,0) using SELF Angle164,
adds to SELF Position158, then enters common15945F1. No direct Owner34C access
occurs in this selected block. Native kMagicPin03Model likewise uses own Angle
and Position without checking/dereferencing Owner. Thus changing its owner to
caster, making it follow caster, or killing it solely when controller expires
is not supported by this body. Controller ownership at creation remains intact.
This does not establish owner independence of the complete engine path:
common update prefix, constructor descendants and cleanup still require their
own audits. verify_magic_controller.py now pins these bounded self-field uses.

Common-prefix result:14B77AC..14B780C tests only non-null Owner34C, copies
owner Position158 into effect snapshot1D0 usingD2DF9E, then samples terrain
at its X/Y and overwrites snapshotZ. There is no Live check in that prefix.
Therefore the whole S21 effect update is NOT owner-dereference-free, even for
5E7. The selected5E7 body does not directly read1D0. Native private MoveEffect
returns through AdvanceWholeTicks/UpdateEffect before its legacy prefix and
does not implement this S21 snapshot. This is an explicit unclosed compatibility
surface, not grounds to move child Position to owner Position. Remaining common
tail and renderer/indirect consumers must determine whether snapshot1D0 affects
the selected output. Retain source ownership and do not add an unused snapshot
or global owner read merely to match layout. Prefix RNG consumption is likewise
not equivalent to a per-skill seeded visual contract.

Snapshot consumer follow-through: complete registered5E7 renderer
A49029..A49188 reads object UV/Alpha and writes model BodyLight, then calls
968CB3 with MODEL in ECX and scalar mesh/render arguments. No direct1D0 or
Owner34C appears. Complete common tail15945F1..1594708 also has no direct
snapshot/owner read. Type5E7 skips the413/7EF7 special decrement/recursion
branches; life<=0 calls14B6820, otherwise reaches ordinary decrement.
This preserves11 selected update visits for initial life10. These bounded
checks do not establish whether outer render preparation or destructor helper
internals use the snapshot. No position-follow adapter follows from them.

- Zero-velocity movement audit:13272A0..1327351 calls AngleMatrix into a
  local matrix, D3189D VectorRotate into a local vector, then adds XYZ to
  position. D3189D..D31A0D is nine multiplies/six adds (three dot products),
  no translation term. With finite angles and zero initialized velocity,
  there is no additional displacement or angular update to reproduce.
  Native MovePosition has the same rotate/add structure plus FPS scaling.
  Prefix verifier now pins this structure. Nonfinite values/FP flags and
  external snapshot consumers are not claimed equivalent by this check.
- Common-prefix audit:16AA743..16AA7D2 dereferences non-null owner to
  copy its position into particle54, replacing saved Z with terrain height.
  Therefore the whole S21 path is NOT owner-dereference-free, only selected
  Shockwave1 body is. Selected update1705379..17054EE and default renderer
  1621CF1..1621D95 do not consume snapshot54. Subtype1 takes that renderer
  through1621A15; unlike3..6 it never copies owner110 into particle position.
  Do not introduce target-following motion for1 based on neighbouring cases.
  Shared constructor1641638..164168E zeros velocity70. Native selected
  constructor likewise leaves zero Velocity. Prefix verifier PASS checks
  these bounded consumers; external consumers and helper internals remain open.
  Native EffectDestructor only clears controller Live/Owner for this type,
  without cascading particle deletion. Shockwave stores a non-null pointer
  to stable native pool storage but selected update does not read its contents;
  tests deliberately include inactive owner. Pool reuse still needs runtime QA.
- Smoke19 correction SUPERSEDES earlier claim of no subtype update. Exact
  dispatcher16AAA32 subtracts7FCA;7FFD gives selector byte10 at1723604,
  table17235B1 selects16C2A23. Subtype19 body16C5301..16C5499 multiplies
  RGB by.86, addsScale.03, subtractsRotation.7 and addsPositionZ3 per tick;
  jump16C6317 reaches172343C. Private native whole-particle adapter now
  executes20 updates and resets its accumulator on all slot allocations.
  Renderer1624F33 selects additive18E7137 then1625050 rawRGB/fullUV sprite,
  no alpha multiplication. Native RGB smoke bitmap defaults to additive.
  Forty-four shared-helper cases PASS including smoke motion/scale/rotation
  and decay at four FPS. Actual pool and GPU acceptance remain open.
- Shockwave1 update now uses private whole-particle ticks, before generic
  SS6 pre-decrement. S21 1705425 compares current life<6, so alpha remains
  .6 at life8,7,6, then decreases five times to.2 at life1. Eight updates,
  no life0 body;17054EE->1706179->172343C reaches common post-decrement.
  Constant1B9EBD4 is float.133332998 (literal.133333f), not exact2/15.
  The owner check1705399 is null-only and never dereferences owner in this
  path. Native helper likewise checks pointer without adding owner Live
  dependency. Null clears Live/life, followed by common decrement to-1.
  Per-slot remainder resets on EVERY allocation. Shared helper tests cover
  non-null inactive owner and null owner at four FPS factors, every scale/
  alpha/RGB sample and expiry:40 total MagicPin cases PASS. Real pool and
  rendered sampling still open. ClCompile PASS; not linked/staged yet.
- Cadence correction:1575643 checks life<5;1575818->1575A35 jumps to
  common15945F1. Native private hit carrier now uses whole ticks including
  life0: nine visits8..0 and five bursts4..0 (30 smoke and10 shockwave
  creation attempts). Actual allocations may be refused by native budget/pool.
  DB1E40..DB1F20 confirms random point is three independent integer offsets
  rand()%(2*20+1)-20, not a spherical radius distribution; existing XYZ math
  is correct. Base controller stage predicate is shared with regression;
  32cases PASS across four FPS factors, including boundary windows and
  repeated observation without duplicate stages. No catch-up invented for
  skipped animation windows. ClCompile PASS, not linked/staged yet.
- Constructor dispatch type `0x5B9` selects handler `0x147A155`; subtype 1
  receives lifetime 8. Render callback `0xA47926` is exactly a seven-byte
  no-draw stub (`xor al,al; ret`), so every visible hit layer is a child.
- Update dispatch selects `0x15754C4`; the subtype-1 branch begins at
  `0x1575640`. Only when remaining lifetime is below 5, it derives a random
  point within radius 20 of controller position and sets light
  `(0.6,0.7,1.0)`.
- It emits six `0x7FFD` particles, subtype 19, at the random point. Their scale
  is independently selected as either 2.5 or 3.5. Constructor branch
  `0x165514D..0x16551E4` immediately changes the submitted texture to `0x7F5A`
  (`Effect\\smoke01.jpg`), sets lifetime 20, alpha 1, random rotation
  `[0,359]`, and preserves the supplied scale.
- It then emits two identical `0x8147` particles
  (`Effect\\Shockwave2.jpg`), subtype 1, at controller position/angle with
  light `(0.3,0.3,0.5)`, scale 0.69 and the controller as owner.
- The `0x8147 subtype 1` constructor is `0x168BDAB..0x168BE48`: lifetime 8,
  alpha 0.6, expansion factor 1.3, base scale 0.69, and a saved copy of the
  input light. Update `0x1705379..0x17054EE` requires the controller owner,
  adds `0.133333` to the expansion factor each tick, sets rendered scale to
  `0.69 * expansion`, subtracts `0.08` alpha while lifetime is below 6, and
  submits light as `savedLight * alpha`. A null owner disables the particle
  and clears lifetime immediately. The ordinary particle renderer submits the
  registered Shockwave2 bitmap and the native pool owns expiration.
- The S21 particle call consumes eight stack arguments while native SS6
  `CreateParticle` exposes seven (`type, position, angle, light, subtype,
  scale, owner`). The extra S21 trailing argument is zero at this callsite and
  carries no required compatibility behavior. Native SS6 `BITMAP_CLUD64` is
  extended only for subtype 19; its private whole-tick update now implements
  recovered16C5301 behavior. Earlier no-update claim was incorrect.

## Asset dependencies

- `magicpin01.bmd` has a 32-byte texture record whose NUL-terminated runtime
  filename is `wind1.tga`. The later printable bytes `03.tga` are stale padding
  after that NUL, not a second texture record or dependency.
- `magicpin03_new.bmd` and `magicpina02.bmd` embed `alpha_line03.tga`;
  `magicpina01_new.bmd` embeds `wind1.tga`.
- `wind1.OZT` and `alpha_line03.OZT` exist and are hash-pinned. RISE's native
  `Texture_t` is exactly `char FileName[32]`, and `CLoadData::OpenTexture`
  consumes it as a C-string, independently confirming that no `03.OZT` is
  required.
- `Thunder01.OZJ` is the pinned particle dependency of each timed `0x5E7`
  child. RISE's native `BITMAP_ENERGY` registration points to a byte-identical
  `Data\\Effect\\Thunder01.OZJ`, so the port now preserves the actual
  `CreateParticle` primitive and subtype 9/10 selection instead of a sprite
  carrier. The auxiliary `0x5E9` child is created for both parent subtypes;
  only subtype 0 owns the constructor position randomization.
- `clud64.OZJ`, `smoke01.OZJ` and `Shockwave2.OZJ` are the pinned dependencies
  of the `0x5B9 subtype 1` hit controller. The registry contains repeated
  case-only registrations for ID `0x7FFD`; all resolve to the same
  `clud64.jpg` asset, but subtype 19 submits `smoke01`. Both `clud64` and
  `smoke01` are byte-identical between SS21 and native RISE, with SHA-256
  `64C359CE760E517B1D5201DF10AE3A0EFFCA4F08A3690CAB6FEEF1C34FFE379D`
  and `05D93565B3E9DC6ADE98A4F6C50BEC676E7E205CE1CFD464B7173359B397DA58`.
- `Shockwave2.OZJ` SHA-256 is
  `F47DC1F04D0DDCA6DC50E8EA09D44F430EB4B80D2B2C52D44BEC3BC8D846FEA2`;
  it is not the native SS6 `Shockwave.OZJ` (`F8152B...`) and therefore keeps
  its isolated registered bitmap ID. The RISE adapter enters native
  `CreateParticle`, `MoveParticles`, `RenderParticles` and pool cleanup; only
  this bitmap/subtype pair receives the recovered SS21 field mapping.

## Open chain parts

- Shader follow-through: `verify_magic_blend_shader.py` pins both source and
  QA BlendMesh.vs/fs hashes. VS adds uniform UV, applies normal-light mix
  with flag0 and clamps to[0,1]; selected RGB=(.48,.73,1)*alpha^2 for alpha
  in[0,.8] remains in[0,.64], so that clamp is inactive. FS multiplies sampled
  texture by vColor. New_RenderBMD submits command BodyLight/MeshUv/EnableLight
  unchanged to SendUniform before bone upload and glDrawElements. FlushAllMesh
  visits queued commands in insertion order and clears afterward; source checks
  pass. Live draw/backend remains separate acceptance work.
  QA Data/Effect is a junction to D:\RISE-CrossPlatform\Client\Data\Effect;
  inspected read-only, never edit shader assets through that junction.
- Transform argument audit (`verify_model_transform_lighting.py`): both bone
  matrix branches of preparation tail call968D0E, whose complete wrapper
  forwards to132DF73 with three internal transform buffers. Object E4/E8
  become parameters30/34 in the normal-light loop: dot product times E4,
  plus literal0.4, clamped from below by E8. They are not geometry scale
  parameters. Native CPU BMD::Transform hardcodes0.8/0.4/minimum0.2.
  Common effect creation143E71F calls reset1315E97, whose tail initializes
  E4=0.8/E8=0.2 from pinned float constants, matching native defaults.
  Selected5E7 constructor resolves via table14B3224 to14808E9; complete
  14808E9..1480D49 and return epilogue14B2E0C..14B2E35 contain no direct
  E4/E8 overrides. Indirect callees and shader-path agreement still require
  closure. Native blend-mesh render branch disables per-normal lighting,
  so renderer consumption must be established as well as initial values.
  Pinned original+decoded MagicPin03 asset has mesh texture slot0 (not merely
  mesh index0). S21 blend branch1332CBB..1332D6E compares that texture slot
  to blend argument0, multiplies model BodyLight by BlendMeshLight per RGB,
  then disables local normal lighting79. Native same branch agrees.
  Flag selector1331F47/1332024 excludes color/chrome variants for42 and reaches
  that blend branch. Triangle texture-mode selector1332F3C/43 then local79
  test1333301E/24 skips per-normal color submission to133330E9/133357F.
  Thus the selected fixed-function path bypasses normal-light color, as
  native does. Actual RenderMesh CPU fallback (lower-case argument names,
  distinct from RenderMeshAlternative) was checked too. Shader AddMeshCommand
  texture-match branch multiplies RGB by blend light, sets Alpha1 and disables
  normal lighting; MakeShaderType therefore does not multiply RGB by another
  non-unit alpha. Defined_Global enables capability, but runtime backend depends
  on GL3.3 and successful shader init; failed mesh enqueue flushes before CPU
  fallback. No live backend or shader execution is claimed by source checks.
  Shader/visual parity remains separate. No global BMD change
  is warranted from configurable parameters alone.
- Outer preparation audit: complete helper `0x1885EB0..0x1886381` samples
  lighting from effect Position158 and adds effect LightEC into model54;
  it has no direct snapshot1D0 or owner34C access. This does not exclude
  other preparation helpers. The common block `0x18872E6..0x188737D`
  separately tests non-null owner's status keys 0x3D/0x48/0xB2/0x118 and
  zeros effect field D0 if any matches. Native `Calc_RenderObject` already
  resets AnimationFrame for owner Stun/Sleep and is used by the private
  renderer. Selected 5E7 bypasses type7E4/7E3 special origins, map6E/6F
  branch requiring type0, and map84 branch requiring type1E: it reaches
  this owner-status gate regardless of map. Pinned General/Buff table names
  keys61/72/178/280 as Stun/Sleep/Darkness/Stun; native enum61/72 matches
  the first two. Extra S21 status translation remains open; two distinct
  Stun entries are not permission to alias280 to61 or truncate to24.
  No global render change is justified by table names alone.
- Close server three-hit scheduling/target selection and client target
  reaction, then map to native 5.2 IDs and APIs.
# Particle active-list ordering — 2026-09-13

Option correction: StepMagicPinParticles now retains native MoveParticles'
GetRenderAllEffects/GetRenderSkillEffects early return. Previously the opt-in
phase bypassed it. Four combinations tested on actual extracted driver with
controlled kernels confirm no lifetime mutation while disabled. This is native
compatibility evidence, not new S21 semantics or visual QA. Updated candidate
A6725815B6CDCCBCEECB0872786FD199E022AAF633A4CC3651D9FA97E5F87AFB;
staging script repinned from the earlier unstaged A3AB2493 build to this one.

Capture inspector tools/grow_lancer/inspect_magic_frame_log.py checks summary
integrity and owned-row40ms accounting. NO_CAPTURE, INCOMPLETE, MODE_OFF,
NO_QUANTA and RESET_REVIEW_REQUIRED are not success. Even ACCOUNTING_CONSISTENT
sets visual_acceptance=false. Current historical log contains no new frame rows;
SHA c973c8767bf8fdf0b13a3f49ef4af8097a3c9b07ecd4faa854b4ba741158b9de.

Candidate deployment: tools/grow_lancer/stage_magic_timing_candidate.ps1 defaults
to dry-run; -Apply requires exact oldEF1595/newA3AB2493 hashes, private paths, full
asset preflight and no process using the target. Rollback is preserved as
RuntimeQA/Bin/Engine-Port S21.before-magic-timing-EF1595.exe when applying.
Dry-run passed while QA3712 remained live; no replacement or backup was performed.
No launch/opt-in occurs in this script. Default standalone asset verifier still
expects the old candidate; explicit staging invocation supplies its pinned hash.

Actual frame transition harness now extracts current Begin/Run/End/ownership/reset
functions unchanged and executes with the real clock/identity helpers. Controlled
API/quantum callbacks test zero/multiple ticks, duplicate Run, cast exit, incomplete
batch time retention, death/map/identity reset and mid-batch reset. Separate
processes test enabled and disabled opt-in. C++14 W4/WX compile/runs pass. This is
adapter control-flow evidence, not execution of BMD, native effects or rendering.

Runtime diagnostics now buffer up to512 magic-frame rows at EndMagicPinFrame:
owned/ran, pendingBefore/pendingAfter, completed, action/frame and elapsed time.
Only armed skill274 captures; existing controller flush writes the rows later.
The sampler performs no file I/O or simulation. Actual extracted C++14 sampler
tests cover bounds, values and exclusions; Win32 build passes. No new log capture
exists yet. These fields can expose zero/multiple ticks or mode-off runs, but
cannot alone prove no duplicate render/update, target timing or visual parity.

Opt-in gameplay wiring now exists in QA builds ONLY: environment variable
RISE_GL_MAGIC_FIXED_TICK_QA must equal1 at process start. Begin before character
traversal binds the validated local actor clock; Run before MoveEffects executes
whole quanta; End follows MoveParticles. While owned, legacy Magic animation,
five model types and explicitly tagged particles skip their old update paths.
Unrelated/shared untagged particles remain native. Duplicate Run is guarded;
actor/map reset clears ownership and clock. Default environment leaves mode off.
Build and source wiring audits pass, not gameplay QA. Current isolated client
still holds the old executable. Outstanding: target/pose timing, initial tick,
idle handoff, simultaneous actors, no-duplicate runtime evidence and visual parity.

Clock preparation: MagicQuantumClock retains elapsed milliseconds in64-bit storage
and consumes40ms only for completed quanta. Explicit start grants no guessed first
tick. Unsigned32-bit timestamp rollover works provided samples are less than one
full wrap apart; scene/actor handoff must reset explicitly. Source's whole-scene
sleep is not transplanted into SS6. Focused constexpr tests include211ms stall,
partial sequence completion, remainder and rollover. No native clock sampling or
gameplay ownership hook is connected yet; this is not a live timing fix.

RunMagicPinQuanta now composes actor -> models -> particles once per requested
quantum instead of batching all actor updates first. Local QA identity/scene is
rechecked between quanta; actor action exit leaves model/particle cleanup running.
It returns completed count and does not read wall time or change global factors.
No gameplay caller exists. Clock ownership, legacy exclusions and idle handoff
remain required; do not treat compiled composition as a timing fix or visual PASS.

Actor counterpart: QA-only StepMagicPinActor validates the local Hero/action287,
then repeats MoveCharacter's BMD origin/scale/current-action setup before invoking
native AnimationCharacterStep with1.0. This matters because the speed lookup reads
BMD.CurrentAction, not OBJECT.CurrentAction, and BMD is shared. Native action-end
handling remains intact; no forced frame reset or gate widening. The adapter is
currently uncalled and rejects the post-cast action; residual idle time must be
handled by the coordinator, not counted as a complete simulation implementation.
Source guard/baseline audits and Win32 build pass; runtime capture remains open.

Model counterpart: StepMagicPinModels scans Effects[0..MAX_EFFECTS) forward,
reading Live/type at each slot and selecting only the five private Magic model
IDs. StepModelTick invokes UpdateEffect(1), then native EffectDestructor at
life<=0, otherwise decrements life. Existing accumulator delegates to that same
single-step primitive. Newborn later slots can run this phase; earlier slots wait.
C++14 phase tests, existing lifecycle regressions and isolated Win32 build pass.
No gameplay caller exists yet; no model or particle double-ticking is enabled.

StepMagicPinParticles now connects the ordered sidecar to the three existing
native particle kernels, using StepOrderedParticlePhase over Particles. Dead and
life0 entries unlink before dispatch; life1 executes and reaches0, retaining Live
until the following phase. Next is read after the body to preserve tail appends.
Callbacks must not recycle the current slot; selected kernels allocate nothing.
C++14 constexpr phase tests and Win32 build pass. No gameplay caller exists yet:
running this alongside legacy MoveParticles would double-update tagged records,
so activation requires the coordinated actor/model phase and legacy skip gate.

Native birth-order sidecar now replaces the producer boolean. ParticleBirthOrder
stores a bounded doubly linked index list over native slots, without allocating
particle objects. Successful Magic creation appends; every native slot allocation
unlinks the previous entry before initialization. IsMagicPinParticleSlot still
requires Live. Dead entries may remain linked until reuse or future ordered
traversal retirement; this is bounded by MAX_PARTICLES, not an unbounded history.
No existing update traversal has switched to this list yet. C++14 constexpr tests
cover nonmonotonic slots, reuse, invalid indices, unlinking and append during
traversal; source wiring audit and isolated Win32 build pass. Cross-cast identity,
coordinated tick scheduling and runtime acceptance remain open.

Producer isolation: smoke bitmap/subtype19 is also emitted by Shining Peak.
The Magic hit emitter (smoke19, shockwave1) and Magic03 constructor (thunder9/10)
now call CreateMagicPinParticle, forwarding unchanged arguments to the native
internal allocator. Only successful slots are tagged; -1 failure is kept distinct
from slot0. Every allocation clears provenance before Live=true. The membership
query checks bounds and Live, so dead entries are inert. Shining uses the ordinary
untagged path. This tag does not encode a cast, owner generation or creation order;
current update/render paths remain unchanged until coherent scheduling is ready.
Source wiring verifier and isolated Win32 build pass; pool-stress QA remains open.

Native preparation: GrowLancerTick.h now exposes StepParticleTick, one quantum
without elapsed-time accumulation. Existing AdvanceParticleWholeTicks delegates
to it and retains the old per-record accumulator semantics. Focused lifecycle
and five skill helper suites plus isolated Win32 build pass. This extraction
does not yet enable tick-major scheduling or fix owner-frame gate misses.

Pinned helpers1725658/172574D construct begin from sentinel.next and end from
sentinel. Iterator1724F63 follows node.next; comparison1724EBE compares nodes.
16AA608 refreshes end each iteration. Normal update decrements lifetime at
1723447..345F, then advances at17234A2; dead/expired path advances BEFORE
retiring the old node at16AA6C3. Existing Breche retirement evidence is reused.

Constructor tail16A9524 calls1725950, which supplies sentinel to1724501.
1725571 inserts before that sentinel: new.next=sentinel, new.prev=old tail,
sentinel.prev=new, old tail.next=new. Thus a successful append during an active
particle update is reachable in the SAME traversal, including from the former
last node. This is not a fixed-slot index scan or a frozen live-record snapshot.
Particle creation in an earlier model/joint phase is also visible on entry.

verify_particle_iteration_order.py pins selected instruction ranges and tests
the conditional newborn traversal. It does not establish which concrete Magic
particle updates create children, allocation/refcount failures, or runtime parity.
Private scheduling must preserve each phase's own iteration semantics; the
primary model pool's earlier/later-slot rule cannot be reused for this list.
