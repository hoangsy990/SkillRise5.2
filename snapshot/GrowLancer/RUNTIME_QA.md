# Grow Lancer isolated runtime QA

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
