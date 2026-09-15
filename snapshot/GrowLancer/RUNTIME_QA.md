# Grow Lancer isolated runtime QA

Current staged private Engine SHA-256 `95736241063643B58F8ECE167A2B9CFBADB6EAB1FCDBE59CBF06F2CBEDC14643`.
For one Spin target-bound cast, launch the owned local four-stack first and
then `start_runtime_qa_client.ps1 -Launch -LocalServer -SpinCrossRasterProbe`.
Select Spin Step (F6 panel), choose one stationary target, cast F7 once, then
use F9 **once on the same target** for separate internal-280 contact. F11
flush and capture both caster and target-contact fixed-camera/rotated frames;
do not overlap multiple casts. F9 is a QA-only visual fixture, not a received
GameServer hit/damage packet. New
`spin-cross-raster-probe` rows report target-owner/effect position and native
mesh samples; existing `spin-blur-draw` rows report ribbon vertices. Compare
probe ON/OFF if shader flush affects presentation. There is no new paired F7/F9 Spin
on this candidate; owner visual remains FAIL. Only 127.0.0.1:44412 is allowed.

Current Wrath black-square diagnostic: the private staged QA client
95736241... supports opt-in `-LocalServer -WrathPixelProbe` only after the
Grow Lancer-only stack is verified/running. Select Wrath, one F7 at fixed
camera, F11 flush, and capture the actual owner frame. New rows compare
`wrath-ground` actual/expected `brocken` GL texture, GL_ONE/GL_ONE state,
bounded ground fragments and five before/after pixel values. If those points
do not darken, inspect other layers at the screenshot frame; do not label
Wrath visual PASS from a diagnostic sample. No fresh F7 pixel result exists.

## Exact Breche wind material candidate (2026-09-15)

The prior private `RENDER_BRIGHT` compatibility inference is retracted. S21
common reset sets BlendMesh to zero; the generic wrapper forwards flag 2 and
BlendMesh zero, and wind_foce's sole mesh uses texture slot zero. Therefore the
S21 RenderMesh blend-selector reaches its additive branch before ordinary
texture alpha testing. The isolated adapter now submits exactly
`RENDER_TEXTURE` with BlendMesh zero. Build SHA-256 is
`F023313844C3192B26D093942E5A6DD4003D7AC7CFF91E2A779EB3371C97C0E7`.
Static/dump checks pass; owner-visible spiral and map-change movement remain
OPEN. The verified stage was launched as isolated PID 23952; login and cast
are not inferred from process startup.

## Local-only Breche F7 isolation (2026-09-15)

The F7 Breche fixture no longer writes `Hero->Skill`, `AttackTime`,
`TargetCharacter`, `SkillSuccess`, or `AttackFlag`. `SetAction(289)` already
enters the normal character animation path; the fixture now remains visual-only
and cannot wait for a GameServer reply it never requested. Isolated build and
stage verification PASS for Engine SHA-256
`2873EFB0C226163A9D9191613B2FE019F84191D58B55C9C9B1D85659B25D3342`.

This does not enable the server handlers for IDs 271..281. They remain
fail-closed pending authoritative S21 target/damage/buff recovery.

## Breche corrected spatial candidate (2026-09-15)

The frame reported as still offset came from the pre-correction binary. Its
runtime log measured caster Y=12950, target Y=12850 and visible owner
ring/wind Y=12750. Exact S21 call-argument decoding proves ring and lightmarks
use `root.Owner.Position`, while AD9 wind is emitted only at remaining life 7
from owner snapshot with Y-5. The final rebuilt isolated candidate also pins
the S21 AD9 subtype-6 constructor lifetime 20, initial alpha 0.9 and initial
light `(3,.6,.6)`. Its SHA256 is
The `0x81EB/subtype0` fire-ring constructor was then corrected from the
non-native life20/alpha1 approximation to exact life12/alpha0 with a
six-tick fade-in and six-tick fade-out. Final SHA256:
`A4D9F2ABF572E87041AF33BD0B4A0E93B3AD3CDF53E5E6B906B5091BC973DA3F`.
Owner QA accepted its center position and yellow subtype-5 slash streaks, but
reported the red/yellow central ground spiral missing. PID22760 proved the
private `wind_foce` carrier was loaded, alive, visible and submitted. The new
candidate maps its black-keyed three-component `ground_wind` material to an
additive SS6 body pass only for this private Breche model. Its SHA256 is
`DD72E9056FC2A93EFDFD833F6157623A91FB9192D99E4552E30294889305D20C`.
PID12756 is live; the visible spiral result remains OPEN until the next F7 cast.

## Code-first 5.2 port note — 2026-09-14

## Latest isolated checkpoint — 2026-09-15

The current staged RuntimeQA executable is SHA-256
`70EDA3EE7DA22F07F059818F62E187D9ECD2C4C51D258FF33584E5B40A518FE4`.
`prepare_runtime_qa_client.ps1` and `verify_runtime_qa_stage.py` both pass
against the private stage (base RISE 2521 files, Grow Lancer overlay 87 files,
root Data 3 files and 126 junctions into the versioned worktree snapshot).
The stage no longer reads mutable Data from `D:\RISE-CrossPlatform\Client`;
that live client is currently being changed by a separate class/item task.
Player and RISE are physical private directories. The x86 rebuild includes a
QA-only Breche owner-resource breadcrumb:
`breche-owner-resource-gate ready=<0|1> controllerCreated=<0|1> target=<n>`.
It is emitted only for skill 279 and does not change production rendering.

The current build also restores the three native owner-motion branches that
were absent from the first visible 5.2 preview: ring-of-gradation scale/RGB
contracts by 1.1 per tick, the fire ring rotates +15 degrees with its alpha
envelope, and each wind BMD carrier rotates -50 degrees and fades by 0.01.
For F7 only, caster facing is resolved toward the selected target before the
owner controller snapshots the angle, matching the named-field behavior of
ReceiveMagic. The prior screenshot remains a visual FAIL; this rebuilt binary
still requires a new owner-observed frame sequence.

A newer staged candidate additionally replaces the old static
bright mesh-zero submission for owner `wind_foce.bmd` with the dump-verified
generic `BMD::RenderBody(RENDER_TEXTURE, ...)` path. Its SHA-256 is
`70EDA3EE7DA22F07F059818F62E187D9ECD2C4C51D258FF33584E5B40A518FE4`.
The prior staged executable was preserved as
`Engine-Port S21.exe.before-ad9-render-3E1BAEC8.bak`; PID 38752 was closed
before staging. That session is not valid movement/cast evidence: it was connected
to a separate Slayer QA GameServer currently stopped on a relative CashShop
path error.

The owner reported a direct Breche cast in the separate S21 reference client,
but its GFx log produced no cast/effect/frame record. That acknowledgement is
reference-only; no 5.2 owner pixels or parity gate is promoted. The isolated
client is not relaunched automatically because it is resource-heavy.

The port is driven by the hash-pinned S21 client dump/reverse files and the
native RISE 5.2 adapter. Entering S21 Siege is not a prerequisite for the
implementation. S21 runtime remains reference-only; isolated 5.2 QA is the
only client launched by this task. The QA panel now pulses a gold border around
the selected skill icon and shows `Target:selected/none`, so target-dependent
Spin/Breche checks cannot be mistaken for a different icon or a targetless cast.

Breche's S21 7EF7 flare is byte-identical to the native SS6 `BITMAP_LIGHT`.
The isolated client therefore tags only Breche emitter sprite-pool slots and
applies the recovered 1727079 visibility envelope privately; ordinary SS6
sprites keep their existing lifecycle and subtype behavior.

## Wrath post-buff owner layer — pinned, visual QA still open

The remembered player-following tail is present in the native adapter and
matches the recovered S21 order: two `flare01.jpg` sprite submissions at
owner bones 29 and 38, followed by the selected four blue `firehik_mono`
particle submissions on the same bone pair. The source verifier and bounded
QA telemetry both preserve this order. The QA record bound is 32 entries so it
includes the eleven preceding lightmarks plus the two scatter, two flare and
four mono records; the telemetry is observation-only and does not grant buff
424/425 or create extra effects.

The static fire-dot route is separately guarded by
`tools/grow_lancer/verify_wrath_flare_native_adapter.py`: it checks the private
64x64 RGB `flare01.jpg` load, native owner-bone `CreateSprite`, subtype-0
additive blend, and the private visibility envelope. This removes a source
route ambiguity behind the earlier black-square report, but not the live pixel
or cadence gate.

A current owner cast is still required to accept screen pixels, cadence,
blend, recurrence/removal and pool cleanup. Until that cast is captured, this
row remains `QA OPEN` even though the reverse/sequence/build checks pass.

## Owner preview session observed in log — 2026-09-13

Read-only review of the isolated GrowLancerRuntimeQA.log for PID20032 found
root-dispatched events for271(8),272(2),273(2),274(2),276(3),277(35),278(7)
and action-dispatched-visual-chain-unverified for279(17). Clash275 was rejected
six times for missing target. No contact-dispatched or Circle paired-contact
event was present for that PID. These are dispatch counts, NOT successful
allocations, accepted renders, damage or visual parity. No screenshot comparison
was available. The last observed Breche event was2026-09-13T04:00:25.058Z.

On the user's continuation request, PID20032 was no longer running; its exit
cause is unknown. Full stage verification passed again (2658base,79overlay,
3rootData,EXE/Player hashes/CRC). The same isolated client was reopened as
PID10964 with the correct Client working directory. Login and the next preview
cast remain unverified. Next: owner login, select a live target for Clash and
the separately triggered F9 contact layers. No production client was launched.

## Shining Peak seed-tail / mixed-pool comparison (OPEN)

Run separately on the isolated 5.2 client and authorized S21 reference session.
Record hashes, fixed camera, caster pose and the full cast, without overlapping
casts. Compare fresh-process first cast, repeated Shining casts after cleanup,
and Shining after a recorded other joint-producing skill has expired. Capture
the prior skill, allocation/seed Scale if supported by approved instrumentation,
and pool reuse evidence; do not claim the same slot was reused from appearance.
Inspect updates5..17, especially8..10 where seed-adjoining UV crosses the bright
texture center. Keep screenshots/video and pool counts separate: neither alone
proves seed geometry parity. Record residual tail shape, camera360 behavior,
cleanup/crash/FPS independently. Do not reset global Scale to manufacture a PASS.

This test has not been executed; no fresh game session is launched by this note.

Owner-selected temporary actor: DK. Final delivery must add native Grow Lancer,
not permanently map these skills to DK. Existing isolated CastCurrentSkill uses
Hero->Object with the appended player actions, so DK can invoke local visual
tests via F6/F7; it does not grant learned skills or send authoritative damage.
F9 supplies separate Spin Step/Magic Pin contact QA on a selected target.
Circle Shield272 now also has an isolated F9 paired-actor visual fixture:
first actor=Hero, second actor=selected live non-Hero target. It invokes the
recovered controller1 plus three joint3 adapter without resetting the cast
animation. Log event `circle-contact-qa-only-first-hero-second-selected`
means fixture dispatched, NOT server buff/damage acceptance or allocation PASS.
No network packet is sent. Staged client SHA256 is now
`E6FED7D0C85494143A10EF5F519EE536891459512CE4AB2A81F580C0B70B9DAD`.
Prior0EA22D64 EXE is retained in Bin/pre-circle-contact backup. Staging is
not evidence the fixture has been exercised in-game.
Breche equipment parity cannot be accepted using ordinary DK armor.

Status: `S21_REFERENCE_CAPTURED_52_COMPARISON_NEXT`. The first isolated launch stopped at
`Data\RISE\Config\Mix.bmd - File not exist.`; the next launch reached login but
crashed at `CSimpleModulus::EncryptBlock+0x55` with integer divide-by-zero
because root-level `Data\Enc1.dat` and `Data\Dec2.dat` were absent. The staging
script now copies and hash-verifies every root Data file as well as the complete
base RISE tree and private Grow Lancer overlay. The merged Player.bmd CRC is
accepted alongside the unchanged legacy SS6 CRC.

Owner confirmation on 2026-09-12: the corrected isolated client reached
gameplay after the root Data key fix. This closes startup/login only; no skill
visual, pool cleanup, damage, target or regression row inherits that PASS.

Owner confirmation on 2026-09-12: after correcting the Grow Lancer-only atlas
divisor from 256 to 512, the non-mutating F10 panel renders one icon per cell
instead of four mini-icons. Icon enabled/disabled atlas layout is PASS. The
client remained responsive at 120 FPS; cast/contact/lifetime rows remain open.

First Spin Step owner run on 2026-09-12: repeated F7 casts did not crash and a
later manual snapshot showed `glEffects=0`, `glParticles=0`, `glJoints=0`, but
the visible output was only the blue/green ground ring. The expected caster
spin/action, cross, blur/trail and target-contact presentation were missing.
Spin Step is therefore visual FAIL; cleanup alone does not override that result.

Native S21 reference captured on 2026-09-12: two isolated batches contain 802
main frames and 272 tail frames. `S21_NATIVE_REFERENCE.md` pins their manifests,
representative frame hashes and visual observations. Spin Step visibly includes
a caster weapon/action onset and a separate target-side blue vertical/contact
phase. The earlier 5.2 F7 run had no selected live target and therefore did not
exercise the separately mapped F9/internal-280 contact root. The next run must
record F7 plus F9 on an explicitly selected target as one continuous sequence.
Clash is not represented by the native batch because it did not cast.

The QA-only build is enabled with MSBuild property
`GrowLancerRuntimeQA=true`. Normal client builds do not define
`RISE_GROW_LANCER_RUNTIME_QA`, so none of these keys exist outside the
isolated QA executable.

Runtime client:

`ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client\Engine-Port S21.exe`

Current staged candidate after the Wrath owner-tail telemetry and F12-only
capture update is SHA-256
`53A72E47ACF05D8F8FBA2A7DBF3096243C9CD9940CDE19BA94C265E4F92C8ECC`.
The QA-only owner sequence buffer now accepts 32 records, enough to retain the
11 lightmarks, 2 scatter particles, 2 bone flares and 4 blue mono particles
from one buff-active render sequence.

After a cast, run `python tools/grow_lancer/inspect_wrath_buff_visual_log.py`.
It validates the first 19 bounded submission records and reports `OPEN` when
no owner buff sequence is present; it never promotes missing telemetry to a
visual PASS.

After the same F12 on/off exercise, wait through the documented lifetime and
press F11 once more, then run
`python tools/grow_lancer/inspect_wrath_buff_pool_log.py`. This separate
fail-closed check requires the 19-record owner sequence, both before/after-off
pool snapshots, and a later same-process F11 snapshot with zero Grow-Lancer
effect/particle pools. Missing or non-zero cleanup evidence remains `OPEN`;
GPU pixels, cadence/blend parity, and server buff authority are separate gates.

All isolated client builds must also pass `GrowLancerIsolatedBuild=true`.
That disables the legacy project post-build copy into `..\..\Client` while
leaving the normal project deployment behavior unchanged.

Controls while logged into gameplay:

- `F6`: select the next base skill in order 271 through 279. The initial
  selection is Spin Step (271), so press `F7` before the first `F6` to cast it.
- `F7`: cast the selected skill from the local character without sending a
  server request.
- `F9`: selected-target contact fixture for Spin Step, Circle Shield,
  Magic Pin or Breche. Circle requires a distinct live Hero/target pair;
  other selected skills log `contact-unsupported`. These are local QA events.
- `F10`: toggle a QA-only two-row icon panel without modifying the character's
  skill list. The top row renders all nine enabled S21 icons; the bottom row
  renders their disabled variants. This replaced the first injection approach
  after runtime evidence showed that mutating the live skill array caused the
  protected client to terminate.
- `F11`: append a pool snapshot with total and Grow-Lancer-specific live
  effect/particle/joint counts plus instantaneous/average FPS. Capture once
  immediately after a cast and again after its documented lifetime; a build or
  visible frame alone is not cleanup evidence.
- `F12`: toggle an isolated QA-only base Wrath buff fixture (ID 424) on the
  local Hero. Turning it on arms the bounded owner-visual telemetry; turning
  it off flushes the captured sequence before removal. It sends no packet and
  does not grant learning, persistence or server authority; use it only to
  exercise the remembered post-buff fire-dot and blue-mono visual tail. The
  fixture also writes pool snapshots immediately before and after removal so
  lifetime evidence is not inferred from a screenshot alone.

The historical unattended `-AutoWrath` auto-on-login fixture has been
removed. The launcher forces `RISE_GL_AUTO_WRATH_QA=0` only in its child,
then restores the parent environment. Launch with `-Launch -LocalServer`
and press F12 explicitly only when a Wrath buff preview is desired. F12
is still QA-only; it does not prove server authority, learning or persistence.

The post-native-reference Spin Step/Circle Shield/Obsidian revision builds as
Engine SHA-256
`935F3EE895904D65D806C0193CCD5B8D80CABD1DBCCD90441BF7FFE376DD5EB1`.
It adds the exact bone-33 three-sample weapon ribbon, SS21 blur UV style 1,
subtype-4 particle constructor and separates cast/contact fan-out. It remains
build evidence until restaged and accepted in gameplay. Circle Shield now uses
the exact 15-tick Shiny04 alpha envelope and native BITMAP_LIGHT particle.
Obsidian now preserves the native null-owned Thunder01 particle and exact
blend-mesh arguments 0/1 for its two model passes.
Magic Pin now preserves its stage-0 untransformed root, reuses the first
transformed point for stages 1/2, uses the exact `rand()%50-25` spread and
creates Thunder01 subtype 9/10 without an owner.
Wrath now uses native SS6 particle/effect pools for `!Combo3`, `alpha_light`
and `brocken`. The Combo4 attribution was withdrawn after checking the
particle dispatcher; !Combo3 uses the default sprite renderer. Radian sine
was verified through the CRT FSIN fallback. Runtime acceptance remains open.

Before launch, run `tools\grow_lancer\verify_runtime_qa_stage.py`. It verifies
the complete base RISE file tree, the complete private Grow Lancer overlay,
the exact Engine/player hashes and that Player/RISE are physical private
directories rather than production junctions.

Clash requires a selected live target. Every action is appended to
`GrowLancerRuntimeQA.log` in the QA client's working directory. These controls
prove only client rendering and lifecycle; they do not validate GameServer
damage, target selection, PvP rules or persistence.

Required visual pass set for each skill: one isolated cast, repeated casts,
near/far camera, fixed camera, 360-degree camera, transparent caster/target
where available, target stationary/moving, pool cleanup, crash, FPS and
stutter. Compare matching action frames against the S21 native reference.

## Owner report — 2026-09-13, QA reopening

Historical controller timing audit (2026-09-13): analyzer
tools/grow_lancer/inspect_delayed_controller_log.py reads pinned-log hash
c973c8767bf8fdf0b13a3f49ef4af8097a3c9b07ecd4faa854b4ba741158b9de.
Ten Magic Pin groups have no stage0 sample inside[5.2,5.8] and no stage transition.
Representative cast:266ms frame5.001288414 ->312ms frame5.883203983,
then action287 ends and action11/frame0 is sampled at656ms, still stage0.
Two groups contain only interrupted actions34/40. This is evidence of missed
sampling windows in that historical build, not permission to widen source gates.
Five Harsh groups reach the gate at312..329ms; next observed stage is2 at343..360ms.
Samples precede updates, so these are bounds, not exact child creation/GPU onset.
Current candidate must be retested; historical evidence does not prove parity
or that all existing latest-source timing corrections are insufficient.

Latest isolated candidate (2026-09-13): staged Engine-Port S21.exe SHA256
EF1595C1C8A3CF3638C4B3C37043D8B6854BDE041F97256A2C50999152E5FB0D.
Prior6F879C executable preserved in RuntimeQA/Bin/
Engine-Port S21.before-spin-outcome-6F879C.exe. No Grow Lancer process was running;
Pegasus process3412 was observed and left untouched. No new client launched yet.
Added11 missing Wrath assets after source hash/conversion verification. Updated
stage verifier PASS:2658 base RISE,79 original overlay,13 current source overlay,
3 root Data files, EXE/player hashes and private-directory checks.
Includes Spin target-required F7 and bounded spin-outcome telemetry (F11 flush),
plus recent private Wrath work. Wrath negotiated server readiness remains false;
F7 cast does not establish an authoritative persistent buff. Staging is not visual PASS.

Updated isolated preview staged and launched after prior PID23372 exited:
EXE SHA2566F879C6EBF68E11CA79642E373F425C6F4AF72E3A524216E2976F603F008B2F3,
PID16052. Complete stage verifier PASS; login/cast not yet verified.
Includes Wrath ground additive correction and bounded controller telemetry.
It does NOT include newly discovered Wrath buff scale/persistent lightmarks.
For271/274/276: choose withF6, castF7 once, allow action to finish, pressF11
to flush controller samples. NextF7 also flushes previous samples. Samples
are before-update diagnostics, not visual/hit acceptance.
Previous staged EXE is preserved in RuntimeQA/Bin/
Engine-Port S21.before-wrath-blend-sampler-E6FED7.exe.

- Spin Step: owner reports the attack/effect still does not appear. Runtime FAIL.
- Retained staged-revision follow-up: `verify_spin_selected_target_render_qa.py`
  PASS; the existing isolated log contains a resolved target and successful
  native cross/ground submission stages. The log predates the newest rebuild,
  so this is a partial diagnostic baseline, not current-candidate visual PASS;
  owner matching-frame retest remains required.
- Wrath: owner reports a large black square behind the buff. Runtime FAIL.
- Harsh Strike and Magic Pin: owner reports effects appear late after the
  attack animation has already been playing. Timing acceptance FAIL.

Owner additionally suspects Wrath enlarges the character. UNVERIFIED: current
Wrath adapter changes child effect scales, but the inspected controller/aura
update does not write Owner->Scale. Trace S21 buff424/425 character update and
render consumers before adding any character-scale behavior; distinguish mesh
scale from surrounding aura scale. No enlargement factor is established.

These are owner observations, not diagnosed causes. Check the actual running
EXE/staging revision, then trace Spin dispatch/action gates, Wrath texture and
blend passes, and Harsh Strike/Magic Pin action-frame scheduling against S21.
Do not count previous static/build passes as resolving these reports.

## Current isolated candidate — 2026-09-15

The currently staged private executable is
`Engine-Port S21.exe` SHA-256
`314863094ECFCC7B0158981012F52437CC49362EBFAAF97DE7D9B5A666B7B296`.
Stage verification passes for the private base/overlay tree and merged Player
CRC. The complete offline verifier sweep is `113/113 PASS`, and the explicit
helper allowlist is `33/33 PASS`. The latest hidden native body GPU probe
(PID25532) passes shader init, texture upload/readback, adapter cache, native
draw, owner-submit and release checks.

The owner-authorized S21 Breche recast is pinned under
`artifacts/runtime_qa/s21_reference/breche_owner_recast_20260915_b`; its
134-frame manifest and representative frame are verified by
`tools/grow_lancer/verify_breche_s21_recast_reference.py`. This reference shows
the caster vortex and target fire columns as separate layers. A fresh 5.2
gameplay cast has not been accepted: visual parity, pool stress, class
activation and authoritative GameServer behavior remain `IN_PROCESS`.

The owner reported another S21 Breche cast on 2026-09-15, but no new
machine-readable frame/manifest appeared in the isolated worktree. It remains
an owner acknowledgement only and does not replace a matched 5.2 capture.

For the next owner QA pass, the F10 top-row order is fixed to: `271 Spin
Step`, `272 Circle Shield`, `273 Obsidian`, `274 Magic Pin`, `275 Clash`,
`276 Harsh Strike`, `277 Shining Peak`, `278 Wrath`, `279 Breche`. Use F6 to
advance and F7 to cast; select a live target before Spin/Clash/Breche and use
F9 only for the documented local contact fixture. This order is a test
contract, not a new skill-ID assumption.
