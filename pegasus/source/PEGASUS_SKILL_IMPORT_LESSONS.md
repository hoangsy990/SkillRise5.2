# Pegasus / SS21 skill-import failure lessons

## 2026-09-13: preserve helper wrappers, not just final allocation arguments

- Dex flare calls7BB8A0, wrapping CreateParticle with6FA6F2(1)'s probability
  min(1, animationFactor). Porting it as direct CreateParticle multiplied
  density at unlocked FPS. Native CreateParticleFpsChecked preserves the
  source rule; live count dropped326->54 (two30-tick emitters, expected~60).
- Decode intermediate helpers before mapping a sink. Random-size argument
  evaluation precedes the gate and must stay in that order. Validate density
  separately from lifetime, colour and rendering; do not dim to hide overspawn.

## 2026-09-13: updating Alpha is insufficient if sprite draw ignores it

- Spiral Charge7F1B/7F1C had source-backed alpha curves but native sprite
  default consumed only RGB. Original7CDE94..CC multiplies temporary RGB by
  Alpha; restoring that path makes the authored fade reach the renderer.
- Keep stored RGB unchanged, otherwise repeated draws compound alpha and
  darken faster than the original. Do not borrow Chaos/Deathside dark blend:
  Spiral retains the normal texture sprite blend. Verify each resource path.

## 2026-09-13: particle texture render overrides are part of each skill

- Chaos smoke_line03 (Pegasus7F3A) was created/updated correctly but rendered
  through native additive default. The original renderer's arithmetic ID
  branch7CDE67..89 reaches7CD03D ->87F992 (ZERO/ONE_MINUS_SRC_COLOR).
  Restoring that one branch visibly replaced central white wash with purple
  smoke in QA12624. A correct constructor and asset do not prove rendering.
- Do not apply Deathside's RGB*Alpha rule to every dark particle: Chaos7F3A
  passes stored RGB unchanged. Trace both material helper and colour pointer.

## 2026-09-13: local QA must reproduce input preconditions

- ReceiveMagic rotates remote actors, not Hero; real input already turns
  Hero toward the selected target. Direct packet-injection QA omitted that
  step, so heading-dependent effects were captured using a stale yaw.
- Establish native CreateAngle facing before injected dispatch, skip self
  buffs, never move the actor, and trace source/target/yaw. A local injected
  screenshot with wrong heading is not evidence that production dispatch
  or the imported trigonometry is wrong. Retest before modifying gameplay.
- Read GPU state at the actual queued draw, not after RenderMesh returns.
  For colour, one-draw framebuffer subtraction isolates emitted light from
  terrain/overlapping effects; raw screenshot RGB includes the background.
  Image-derived tuning must remain distinct from original binary constants.

## 2026-09-13: audit native work after the imported handler

- Spearstorm's recovered updater stopped motion on contact, but native
  ZzzEffect still ran MoveParticle(o,true) after the handler. The model
  continued underground while its lifetime and render gate remained valid.
  Record post-handler motion/animation/fade as part of every import, not
  just dispatch and the private updater. Bypass only source-proven duplicate
  work; do not globally disable native motion for all imported resources.
- Measure transformed mesh vertices relative to terrain at flight/contact/
  fade. In this incident late vertices were all906..1334 units underground;
  extending lifetime or adding another spear would hide, not fix, the cause.
- Deathside illustrates two independent omitted renderer rules: its scythe
  RGB blur needs ZERO/ONE_MINUS_SRC_COLOR, and its three smoke resources need
  temporary RGB*age-alpha with the same blend. An alpha-test substitution
  produced opaque black ribbons; default additive particle rendering produced
  white clouds. Trace the called GL state helper, not an inferred name.
- Ensure BMD action speeds/loop flags are initialized explicitly when the
  recovered model loader does so; native Open2 alone does not provide them.
  Capture a repeat attack if the first cast only summons a carrier.

## 2026-09-13: prove math helper identity before validating formulas

- AF45F0 is SIN, AF4050 is COS, proven through fallback calls to x87
  fsin/fcos in the hash-pinned Pegasus executable. Earlier reverse notes
  swapped them; Spearstorm tests then encoded the same wrong formula.
- Use Tests/VerifyPegasusTrigHelpers.py, then trace each caller's complete
  operand flow, destination axis and sign separately. Do not globally swap
  sin/cos: some callsites were already correct. SS21 requires its own pinned
  helper evidence; these addresses are not portable.
- Test independent cardinal-angle cases and authored ranges. For the
  Spearstorm falling vector pitch60/yaw0, Z must be -173.20508, not0.
  A test derived only from the port can falsely certify a horizontal flight.
- Capture early flight as well as impact; a forced-impact timer can conceal
  a wrong movement vector in late screenshots.

## 2026-09-12: verify the matched render-state pair in the framebuffer

- Do not treat a terrain renderer as correct merely because the recovered
  texture, color, scale and angle match. Verify the exact state opener and
  closer as a pair. Dex Booster used `EnableAlphaTest()` with
  `DisableAlphaBlend()`, which compiled and passed its old string gate but
  rendered the black RGB background of `twlighthik01` as three giant quads.
- Inspect a three-phase contact sheet before marking any visual gate. It
  exposed the Dex Booster quad, opaque Dragon Violent heads, abbreviated
  Shining Bird chain and absent-in-window Spearstorm blades even though all
  four routes had allocation/model/GPU telemetry.

This is a prevention checklist extracted from the Sword Blow client QA cycle.
It records why V1 through V10 failed and what V11/V12 proved. It does not turn any
static or build result into visual acceptance.

## Sword Blow V1 -> V11

| Version | Runtime result / mistake | Root cause | Rule carried forward |
|---|---|---|---|
| V1 — cast dispatch | Skill 1501 could be learned/selected but clicking a target did not cast. | The high ID was missing from the initial target classifier, mounted/weapon allowlists and movement-continuation path; the generic path could also derive an animation from the high skill ID. | Trace input classification, target selection, weapon/mount gates, continuation, explicit action and outgoing packet as one chain. Never assume adding a skill row makes it castable. |
| V2 — texture IDs | Cast and damage ran, but foreign/custom-item texture sheets covered the screen. | Fixed IDs `34900..34902` collided with the dynamic `BITMAP_NONAMED_TEXTURES_BEGIN` custom-texture stream. | Resolve all derived ID ranges with the real RISE definitions/compiler, reserve below the dynamic stream, and enforce uniqueness/range tests before loading assets. Never copy a source-client numeric texture ID. |
| V3 — caster sword layer | Target effects appeared, but the large sword/caster light was absent; the attack still looked like a stab/lunge. | Dedicated `lightmarks`/`flare01` dependencies were substituted with native IDs, and the caster root was incorrectly coupled to the custom target-list packet. | Recover every loader/renderer dependency. Separate canonical cast/root creation from supplemental target-list impacts and prove each packet's ownership. |
| V4 — incomplete stationary fix | Owner saw behavior effectively unchanged. | Only the intended special branch was handled. Out-of-range failure returned into native `AttackKnight`, so melee chase still executed; local-caster resolution also depended on an unsupported viewport gate. | Audit every return and fallthrough, including out-of-range input. Resolve local `Hero` authoritatively before remote viewport lookup. A special handler must explicitly consume the event when fallback is forbidden. |
| V5 — root ownership correction | Dispatch/root responsibility was corrected, but the sword model was still invisible. | Creation, lifetime, model load and packet flow were present, but that did not prove the renderer was reached in the active world pass. | Treat dispatch, creation, assets and rendering as separate gates. Do not iterate visually by guess once an effect object exists; attach the matching binary/PDB and inspect the live render path. |
| V6 — renderer pass | Live debug finally made the sword visible. | Pegasus renderers were gated on `bRenderBlendMesh == true`; normal world rendering calls `RenderEffects(false)`, while the true pass is water-only. | Verify the actual render-pass caller and state flags. Custom models render once in the normal pass and must not be duplicated in the water-only pass. |
| V7 — server-position jump | Effect was visible, but the caster still snapped/jumped to stab the target. | `c->Movement == false` and no pathfinding were misleading: native `UseSkillWarrior` still reached its final `SendPosition` packet. | Trace every outbound movement/position packet, not only client pathfinding state. Stationary skills must exclude both chase and server-position requests. Owner PASS confirmed stationary casting here. |
| V8 — child origin | Small swords rendered far behind the target. | The adapter used negative local launch Y; Pegasus uses `+RandomRange(600,1000)` and a common local destination at Y `-1500`. | Decode stack arguments and local-to-world signs at the exact helper boundary. Lock start/end roles and signs in a focused test. |
| V9 — pitch operand order | Origin improved, but sword travel/orientation still differed from Pegasus. | Pitch used the operands in reverse. Pegasus calculates `(launchZ-destinationZ)/1500` before clamp and `asin`. | Preserve operand order exactly; equivalent-looking subtraction is not interchangeable once it drives rotation. Test positive, zero and negative pitch. |
| V10 — wrong def-use interpretation | A further “trajectory fix” still produced the wrong direction. | Temporary stack variables were labeled as start/end incorrectly. Random Z was moved to the destination even though the instruction order first creates flat destination `(0,-1500,0)`, then adds `-100..250` to the launch Z. | Reconstruct the full def-use/call order for every vector component. Do not trust provisional variable names or comments; validate them against subsequent consumers and video. |
| V11 — corrected chain | Owner confirms the small-sword direction is correct. | Launch is `(random lateral,+600..+1000,random -100..+250)`; destination is `(0,-1500,0)`; yaw is `CreateAngle(launch -> destination)`; pitch is `asin((launchZ-destinationZ)/1500)`; update rotates local `-Y`. | Keep this recovered chain intact. No guessed 90/180-degree model correction is allowed while the byte-identical model and recovered transform agree. |
| V12 — blend/color correction | Direction remained accepted, but each small sword lacked its luminous tail and two model-family tints differed from SS21. | The adapter omitted the second big/small visual path (white mesh 0 plus bright mesh 1), used Pegasus constructor tint for blades/planes, and had no regression lock for traced colors. | Trace model passes separately from object color. Guarded SS21 runtime proves root/blade/plane object light `(0.2,0.2,0.2)`; blue-white appearance comes from material and bright passes, not a guessed blue object tint. Lock every traced family color in a focused test. |

The GameServer V1/V2/V3 packages used a separate numbering sequence and must
not be confused with the client V1..V11 cycle. Their reusable lesson is that a
GM authority bit, configured GM-level lookup and test-only command guard are
separate contracts; learning a test skill is not evidence that cast, damage or
visual runtime is correct.

## Mandatory workflow for every remaining Pegasus skill

1. Pin the evidence binary/data hashes and exact handler boundaries.
2. Build a role table before coding: cast dispatch, action/target, root,
   children/assets, renderer/pass, movement/collision, reaction, lifetime,
   ownership/pool and server damage/buff state.
3. Decode complete basic blocks and follow def-use through all vector
   components, stack arguments, subtype branches and later consumers. Numeric
   xrefs and names are leads, not proof.
4. Translate behavior into native RISE 5.2 APIs. Never transplant Pegasus or
   SS21 addresses, object offsets, vtables, packet layouts or raw enum IDs.
5. Reserve client model/bitmap/sound IDs against the real compiled ranges and
   add uniqueness/boundary tests before runtime loading.
6. Keep canonical cast/root packets separate from target-list, hit and
   secondary-effect packets. Define who owns each effect and who releases it.
7. Trace every input exit and all movement outputs, including fallback melee,
   pathfinding, `SendPosition`, knockback and server correction.
8. Verify creation and rendering independently. Check the normal/water/shadow/
   blend pass actually used by the live scene and prevent duplicate passes.
9. Match the exact executable and PDB for live debugging. Break at dispatch,
   object construction, renderer, movement packet and cleanup boundaries.
10. Compare synchronized frames against the reference video. Compilation,
    asset hashes and a visible root do not prove direction, timing or parity.
11. Record owner PASS narrowly per gate. Keep the skill `IN_PROCESS` until all
    gates are evidenced, including cleanup and repeated-cast behavior.
12. When extending a randomized native array, update and test allocation,
    active-range initialization, renderer bounds and shutdown cleanup as one
    capacity contract. Allocating a model tail without zero-initializing it can
    make lazy `NumMeshs`/pointer guards accept garbage and produce intermittent
    missing or partial BMDs even when files and shader upload code are correct.
13. Treat the filename accepted by the native texture API separately from the
    physical packaged filename. RISE `CGlobalBitmap` dispatches on logical
    `.jpg` and internally resolves its encrypted `.OZJ` sibling; passing `.OZJ`
    directly fails before file I/O. For fixed texture IDs, use the fixed-ID
    overload too—an anonymous successful load does not populate the ID used by
    the renderer. Run a post-`OpenImages` startup preflight over every family.
14. Do not define model validity as `NumMeshs > 0` globally. Some recovered
    effect BMDs are intentional animation carriers with zero meshes but valid
    bones/actions. Inspect the BMD structure and encode family-specific
    validity; otherwise a good carrier is repeatedly reopened or rejected.
15. A correct bitmap ID and texture file do not establish correct terrain
    rendering. Recover the OpenGL state calls around every terrain-alpha draw
    and bracket them in the translated renderer. Omitting the native alpha
    blend pair made Crusher Charge's black JPG background overwrite a large
    square of terrain even though the texture and effect geometry were right.
16. A one-run visual harness must isolate persistent effects between samples.
    Prove server removal packets, client unregistration, physical-effect
    deletion and the delayed pool snapshot separately. If the remote server is
    stale, a compile-guarded client cleanup may keep diagnostic frames usable,
    but it is not server runtime acceptance. Preserve intentional dependencies
    such as Sword Wrath through Sword Blow, and clear persistent summons such
    as Deathside before judging the next skill.
17. Fixed-client random cadence is not portable as raw `rand() % N` inside an
    unlocked-FPS RISE updater. Preserve the recovered probability through
    RISE's native `rand_fps_check(N)` (or a proven crossed-tick gate), then
    capture at a dispatch-relative timestamp. Deathside's otherwise-correct
    smoke textures became an opaque white wash because emission count scaled
    from the source simulation rate to 150-230 render updates per second.
18. Logical `.jpg` paths require physical `.OZJ` files, not ordinary JPEGs.
    Native `Convert_Format` writes the first 24 JPEG bytes as a dump header and
    then the complete JPEG; `OpenJpeg` skips that header. Direct `.jpg` and TGA
    staging can both exist on disk yet fail every load and render the unknown
    white texture. Trace `SKILL_ICON_LOAD` at startup before judging UV/color.
19. A custom action outside the native stand/attack ranges must be admitted by
    every mouse-input guard. Merely increasing its `PlaySpeed` does not restore
    held-cast cadence when the outer input loop rejects non-stand actions. Keep
    the exception skill-and-action-specific, then prove dispatch intervals and
    airborne retriggers from runtime logs.
20. A delta executable is not a standalone MU client. Relative asset opens may
    occur in C++ global constructors before `WinMain`, so fixing the working
    directory at the entry point is too late. For isolated QA, anchor the
    process to the executable directory with a linker-retained CRT initializer,
    keep integrity checks pointed at the actual staged asset, and validate by
    launching from a deliberately unrelated directory. A misleading "file not
    exist" popup can also be a stale CRC guard for the native file rather than
    proof that the imported asset is absent.

## Additional boundary for importing behavior from SS21

- SS21 is behavioral evidence only. First prove which visible behavior,
  formula, asset role or state transition is relevant; then derive a native
  RISE 5.2 implementation.
- Do not copy SS21 addresses, hooks, object layouts, structure sizes, enum
  numbers, texture/model IDs, action IDs or packet layouts blindly.
- Revalidate class/weapon/mount/range rules against RISE 5.2. Newer-client
  dispatch tables and action spaces may exceed native limits.
- For server formulas, buffs and counters, require authoritative executable,
  configuration or runtime evidence. Tooltip text alone is not a damage
  formula. A client callsite audit that finds only buff-to-visual checks is
  negative evidence; it does not authorize inventing server-side magnitudes.
- If a visual layer cannot be traced to a loader plus constructor/update/
  renderer chain, leave it unresolved instead of substituting a similar native
  effect.
# 2026-09-12: constructor identity and shared draw overrides

- Shining Bird force pillars were mistakenly mapped to PARTICLEs although
  all three calls use the same pinned constructor as joints (0x796B7A).
  Verify call target, argument order and owning pool together. Scale 200 is
  ribbon width here, not particle sprite scale. Anchoring scans must address
  the same pool used by creation (Joints and PKKey for the two side ribbons).
- A call to the original generic object renderer is not evidence that the
  RISE generic renderer is equivalent. Pegasus branches for 0x27C1/0x27C3
  override mesh 0 with additive blending and source RGB; imported IDs bypass
  those branches in native RISE unless explicitly adapted.
- InspectPegasusRuntimeState.cpp currently hardcodes old symbol addresses.
  After a new link it can read valid memory containing unrelated zeros and
  falsely report null Hero/protocol 0 during active gameplay. Resolve symbols
  against the matching EXE/PDB or reject the result; a successful memory read
  alone does not validate an address. Runtime dispatch logs contradicted this
  inspector in PID 6776 on 2026-09-12.
