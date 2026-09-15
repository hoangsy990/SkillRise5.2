# Clash (skill 275) reverse evidence

Status: `IN_PROCESS` — the S21 client action, local and received target edges,
controller, both visible children, render/update/lifetime contracts and assets
are statically closed. Server siege validation/pushback and RISE runtime parity
remain open.

### QA log correction — 2026-09-15

The earlier no-launch statement below is historical, not the current QA
status. Isolated QA PID 23952 invoked Clash skill 275 with a selected live
target (index 12): `cast-input` changed native action 11 to 293 and
`root-dispatched` recorded the target. The `target-skill-render` records
include 993 successful native `clash-ensure` gates, 871 successful
`clash-calc`/`clash-submit` pairs, and 120 `clash-calc result=0` skips. Every
skip has alpha exactly 0.000000, consistent with the intentional invisible
alpha envelope; no nonzero-alpha calculation skip was found. The two visible
child model types are 10135/10136 in the private RISE ID range.
Visible native submit samples for front 10135 (560 records) span life 25..0,
alpha 0.05..0.65 and scale 0.76..0.89; rear 10136 (311 records) span life
8..0, alpha 0.18..0.90 and scale 0.85..1.25. These match the bounded front
alpha peak 0.65 and rear peak 0.90 encoded by the native scalar adapter;
they do not provide S21 frame/pixel measurements. The rear theoretical final
scale 1.30 is not submitted at alpha-zero expiry and is not a visual mismatch.
These observations prove this selected-target native submit path reached its
renderer; they do **not** prove correct pixels, source action-194 timing parity,
caster/target displacement, siege eligibility or GameServer pushback. Clash
remains `IN_PROCESS` and owner visual acceptance is OPEN.

## Identity and dispatch

- `SkillList.xml` identifies skill 275 as Grow Lancer `Clash`: damage 50,
  mana 50, BP/AG 50, range 6, delay 10000 ms, brand 75 and siege-only use.
- The common local dispatcher resolves the target index at
  `0x010E4A8C..0x010E4AA1`, rejects a missing target, sets action 194 through
  `0x1327DE8` at `0x010E4AB5..0x010E4AD2`, and creates controller `0x5FA` at
  `0x010E4AF4..0x010E4B64` with caster ownership and target index.
- The received PvP/siege branch compares skill 275 at `0x012E042F`, validates
  a distinct caster and an active target, computes caster/target facing,
  plays sound 1115, sets action 194, and creates the same caster-owned `0x5FA`
  controller while retaining the target index.
- Sound 1115 is registered at `0x018E2E4B..0x018E2E77` from string
  `Data\Sound\Crash.wav` (`0x01BB538C`). The similarly named on-disk
  `shieldclash.wav` is not the resource registered or called by this chain and
  is excluded.

## Local movement and packet edge

- After root creation the local branch subtracts caster map coordinates
  (`object +0x180/+0x184`) from the current cursor map coordinates
  (`0x0B51ECD4/0x0B51ECD8`), normalizes the vector and derives a destination
  tile in `0x010E4B67..0x010E4CAF`.
- It reads the destination terrain word at `0x0B7AAF60` and rejects flags
  masked by `0x1C` at `0x010E4CB5..0x010E4D0F`.
- The following branch serializes map/source/destination values and invokes
  the native movement/cast send paths. Exact packet names and the server-side
  pushback response are deliberately left open; no S21 packet layout or
  pointer is transplanted into RISE.

## Invisible controller `0x5FA`

Correction: source lower frame gate is **1.0**, not0.0.157DA26 compares
controller frame with float at1B4DD48=1.0 and157DA2D skips on <=; upper bound
1B58D74=6.5 is inclusive. Native ClashEmissionDue now uses `(1.0,6.5]` with
counter0, correcting premature emission. Earlier `(0,6.5]` statements in
historical checkpoints are superseded. Boundary regression adds0.9999/1/1.0001;
missing-target retry cadence test uses1.5 inside the valid window. Pinned
verify_clash_emission_gate.py prevents interpreting the constant as zero again.

Pinned offline full dump (SHA374D1ECB...6EFED), inspected via
`inspect_breche_full_dump.py --model 0x5FA`, contains model5FA at3B348F80 in
manager26D1B600's4967-entry map. Counts at22 are bones0/meshes0/actions0.
This closes cache-presence evidence for this snapshot: the controller is not
missing merely because there is no registered BMD asset. Native logical
controller need not fabricate a BMD to satisfy S21's model-cache plumbing.
It does not prove all fields in that cache record are inert, nor replace the
separate owner-model validation and child model load gates. Source handler
consumers after cache lookup must still be checked for non-geometry fields.

Consumer follow-up: anchored selected block157D996..157DF45 uses each cached
result local11A08/11A10 exactly twice: store EAX then compare against zero.
Neither local is loaded or its address taken later in that block. The frame
copy at157DA00..157DA12 is from owner object11A04 to controller object11A0C,
NOT from either cached model. Thus this block does not require bone/action
data from the empty controller model. Source lookup side effects and missing
owner-model conditions remain a separate compatibility issue. Do not add a
fake controller BMD, require controller NumActions>0, or copy model offsets
just because two model lookup calls exist. verify_clash_emission_gate.py pins
the result-use counts through the actual common-tail jump.

Native owner-model follow-up: CreateClashRoot is shared by QA and WSclient
receive paths and already requires loaded action293. Controller update now
rechecks CanPlayAction(owner,293) before emission, returning without spending
the one-shot counter if unavailable; private whole ticks still decrement life.
This is a scoped compatibility guard: S21 checks model-cache existence, whereas
the native adapter requires its appended action to remain available. It does
not require a controller BMD or modify other skills. Live actor model changes
and retry behavior remain QA cases; source ordering/static build is not proof
of runtime model mutation safety or owner pointer reuse.

- Correction from complete block decode: provisional rear vector-50 at
  157DD1B..157DD86 is overwritten at157DD8B..157DE2F by caster+UNIT
  direction, before5FC call157DED0. Front5FB gets caster+direction*30.
  Both constructors retain incoming Position (front addsZ150), and only
  store newly computed direction*30/-50 for subsequent update. Thus rear
  creation pose and tick1 pose differ; old port wrongly collapsed these.
  Native code now preserves input pose and uses correct rear initial point.
- D308C3 normalizes allXYZ, including height difference; threshold is strict
  length<1e-6, which zeros the vector but does not abort emission. Old native
  code forcedZ0 and rejected length<=.001. Corrected locally using existing
  native VectorNormalize plus explicit source threshold, not a global math
  rewrite. Nonfinite inputs and engine target/pool tests remain open.
- Missing target at157DA7B..157DA84 skips to157DF45/common lifetime path;
  this does NOT explicitly destroy controller. Current native failure path
  still expires it; reconcile that separately with native target safety.

- Model callback registration `0x00A1B345` points to `0x00A4A0FB`; that
  callback returns false without drawing.
- CreateEffect jump slot `0x103` (`0x014B3224 + 0x103*4`) points to constructor
  `0x01482038`. Subtype 0 clears the one-shot counter and sets lifetime 30.
- MoveEffect dispatch enters `0x0157D958`. It destroys the controller through
  `0x13167BB` when owner action differs from 194, resolves the retained target
  index, and gates the one-shot block on copied action progress `(0, 6.5]`.
- The block computes `normalize(targetPosition - casterPosition)`, then creates
  child `0x5FC` at the caster plus unit direction, followed by child
  `0x5FB` at the caster plus direction times `30`. Both calls retain the caster
  owner and target index, use scale 0.75 and light `(1.0, 1.5, 1.5)`, then the
  controller increments its counter.

## Visible child `0x5FB` / `crasha01.bmd`

- Registered at `0x018D2FD2` as `Data\Effect\crasha01.bmd`; callback
  `0x00A4A102` multiplies RGB by alpha and renders mesh 0 with flags `0x42`.
- Constructor slot `0x104` points to `0x01482071`: subtype 0 has lifetime 27,
  alpha 0, caller scale 0.75, normalized target direction times 30, and a
  vertical position offset of 150.
- Update `0x0157DF4A` recomputes position from caster plus the constructor's
  stored vector (it does not re-aim from a moving target),
  adds 150 to Z, applies a symmetric half-life alpha envelope with peak 0.7,
  increases scale by 0.005 and angle Y by 10 each tick.
- The renderer additionally subtracts 0.05 from blend-mesh texture U and 0.01
  from blend-mesh texture V on every render call before issuing mesh 0 with
  blend-mesh index 0. These are object offsets `+0xAC/+0xB0`, not angles.
- The v0F model decrypts as `crasha01.SMD` (one mesh, one bone, one action) and
  embeds `alpha_line2.jpg`; the required source texture is therefore
  `Data\Effect\alpha_line2.OZJ`.

## Visible child `0x5FC` / `crasha02.bmd`

- Registered at `0x018D3007` as `Data\Effect\crasha02.bmd`; callback
  `0x00A4A24A` multiplies RGB by alpha and renders mesh 0 with flags `0x42`.
- Constructor slot `0x105` points to `0x014822E2`: subtype 0 has lifetime 10,
  alpha 0, caller scale 0.75 and normalized target direction times `-50`.
- Update `0x0157E16D` recomputes position from caster plus the constructor's
  stored vector (it does not re-aim from a moving target),
  applies a symmetric half-life alpha envelope with peak 0.9, and increases
  scale by 0.05 each tick.
- The v0F model decrypts as `crasha02.SMD` (one mesh, one bone, one action).
  Its single 32-byte texture record is `wind1.jpg`; bytes after the first NUL
  are stale padding, so `2.jpg` is not an extra dependency. The required file
  is `Data\Effect\wind1.OZJ`.

## Cleanup and open work

- Selected renderer audit complete:A4A102/A4A24A set BodyLight=RGB*Alpha,
  mesh0 flags42 blend0/blendLightAlpha and storedUV. Front-only U-.05/V-.01
  per render. Current native branch matches;verify_clash_renderer.py PASS.
  Isolated Win32 linked/staged SHA90416CB2616CC265D650BBFDD1E62E596D0F785B21CAB90A059F31A617DFA18C.
  Before-clash GUID backup retains6BB0C35B. No game launch/visual PASS.
- Cache boundary: EnsureModel accepts visible models with meshes/bones/actions
  and uses native Open2; invisible Clash controller has no kModels row and
  cannot be passed to this helper (would always fail). S21 pointer-presence
  gate must not be mistranslated as visible-geometry requirement. Native
  private controller logical readiness remains an explicit compatibility
  item; do not insert a fake model file merely to satisfy a cache check.

- Shared pose functions now drive controller initial points and child stored
  offset updates. Six cases cover vertical, sloped and zero directions plus
  owner movement; tests intentionally do not claim native normalization API
  coverage. Controller now resets Timer0 and uses private whole ticks:31
  visits30..0. Shared emission predicate retains exact(0,6.5] and counter0.
  Missing-target lookup now skips without consuming lifetime/counter beyond
  the normal tick decrement; owner null/Live/action safety remains intact.
  Four controller cadence cases include two skipped attempts then one emit.
  Total12 tick+6 pose cases and frame boundary checks PASS,W4WX x86;
  isolatedClCompile PASS. Native model-presence gates, pool execution and
  selected renderer remain to audit before linking/staging.

- Shared GrowLancerClashTick.h now drives both native child scalar branches
  and GrowLancerClashTickTest.cpp. Eight x86 W4/WX cases cover front/rear
  at factors.25/.5/1/2:28/11 updates including life0, one destruction,
  alpha endpoints0 and peaks.65/.9, final scale.89/1.30 and front angleY
  increment280. These do not simulate engine owner/pool allocation or GPU.
  Project inclusion3cpp24headers PASS; isolated ClCompile PASS,not linked.
  Controller follow-up157D9AE..157D9F9 resolves owner.Type and controller.Type
  model cache via968B22/EF2A81 and skips body if either is missing. This is
  separate from target-index validation; retain that distinction in mapping.

- Native correction: selected subtype0 front/rear now use the private
  AdvanceWholeTicks path with Timer reset at allocation. S21 front157E168
  and rear157E327 both reach15945F1 before common lifetime decrement;
  expected update visits are28 (27..0) and11 (10..0). Previous fractional
  updates gave FPS-dependent alpha sampling. Owner-relative position,
  controller timing and global SS6 behavior are not changed by this patch.
  Isolated Win32 ClCompile PASS; NOT linked/staged yet. Shared scalar
  regression and actual pool/visual checks remain to run.

- The controller has explicit owner-action destruction. Both visible children
  use native effect-pool lifetime expiry and owner-relative updates.
- Still open: authoritative GameServer siege/PvP eligibility, target
  validation and pushback displacement; exact packet symbol names; isolated
  model loading, native RISE IDs and in-game comparison.

### Isolated model/texture render pipeline rechecked — 2026-09-14

`tools/grow_lancer/verify_clash_model_pipeline.py` now pins the two private
model rows (`crasha01.bmd` and `crasha02.bmd`), native `BMD::Open2`, the
existing bitmap loader's `.jpg` -> `.OZJ` path and the renderer order
`EnsureModel -> Calc_RenderObject -> RenderMesh(mesh0)`.  It also verifies the
four staged Clash asset hashes against `asset_provenance.json`.

This closes the source/staging contract only.  `BMD::RenderMesh` may use the
native shader queue or its existing legacy fallback; no renderer replacement
or S21 pointer transplant is allowed.  GPU pixels, pool/cache reuse and a
selected-target owner cast remain open.
