# Magic Pin (skill 274) reverse evidence

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
