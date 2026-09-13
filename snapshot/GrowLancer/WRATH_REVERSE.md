# Wrath (skill 278) Season 21 client reverse

Status: base cast controller and its complete subtype-0 child continuation are
statically recovered. Server defense-cost buff semantics and runtime QA remain
open.

## Identity/dispatch

### Supplied server formula evidence (not yet application semantics)

`D:\GameServer S21\Data\FormulaData.xml`, SHA-256
`9A513DD4375116CFD0FA599114B2103B4D21B194B775AE376E0C6289BA36E6FF`:
Character/44 (combat power increase) is `(%d+%d)*0+10`, parameters STR/AGI;
Character/46 (attack increase) is `(%d+%d)/3`, STR/AGI;
Character/47 (defense decrease) is `%d*(((%d+%d)*0+0.05)*-1)`, DEF/STR/AGI.
These simplify mathematically to 10, (STR+AGI)/3 and -0.05*DEF, respectively.
Application units for combat power, order of defense modifiers, argument
selection and integer rounding still require call-site evidence. Do not label
10 as ten percent without the consumer or truncate each stat before division.
Formula IDs repeat in other XML sections with different expressions; the
Character section is essential, not an optional qualifier.

The supplied Skills/SkillList.xml hash matches the accepted Desktop/Skills
profile (`3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0`).
Base278/buff424 and master895/buff425 both have Duration=0. This does not prove
zero-second expiry or infinite/toggle lifetime: the server consumer is not
recovered. Visual life70/controller and repeating aura life30 cannot supply
that missing gameplay duration. No default 60-second timer is introduced.
Verifier: tools/grow_lancer/verify_wrath_server_formula.py.

- Base skill 278 is Grow Lancer `Wrath`: level 66, mana 40, AG 30, strength
  200, dexterity 200 and buff index 424.
- Local handler `0x10E470E` sends the packet, selects action 192 with argument
  321 and plays sound 1118.
- Received handler `0x12CB6AC` mirrors action 192/321 for a remote caster,
  creates invisible controller `0x5EA subtype 0` owned by the caster, and marks
  the character active.
- Sound 1118 registration: `0x18E2F08` -> `Data\Sound\Wrath.wav`.

## Controller 0x5EA

- Controller Timer0/private whole ticks added after verifying common tail
  1579222->15945F1: life70..0 allows71 update visits unless owner safety
  guard terminates. Removed superseded unreachable fractional particle bodies.
  Child owners verified at1578D1C/1578ED3:5EC and5EB use controller.Owner
  (caster), unlike MagicPin children; existing mapping remains unchanged.
  Ground81E3 call15791D5..1579222 uses null owner and controllerLight at
  15791F3, now passed explicitly instead of a temporary white vector. Its
  constructor may override light, so this call-argument fix alone is not
  evidence of visible change. No action-change guard was invented: source
  reads owner frame immediately at15789DD. Native null/Live safety remains.
  Isolated ClCompile/project3cpp22headers PASS; not linked/staged yet.
- Constructor `0x1480E08`: lifetime 70, internal stage counter 0, scale/raw A0
  1.0.
- Renderer `0xA4941B`: exact no-draw stub.
- Update `0x15789DD` reads the owner animation frame and advances four one-shot
  stage windows:
  - frame `[3.0,3.5]`: local offset `(0,-70,250)`, particle `0x81E4`
    (`!Combo3.jpg` dispatcher, subtype 0, null owner).
  - frame `[3.5,4.2]`: local offset `(0,-80,30)`, effect `0x5EC subtype 0`.
  - frame `[5.0,5.5]`: local offset `(0,-30,-40)`, effect `0x5EB subtype 0`.
  - frame `[5.6,6.5]`: local offset `(0,-80,220)`, particle `0x81E6`
    (`alpha_light.tga`), followed by effect `0x81E3` (`brocken.jpg`) at local
    offset `(0,-80,5)`.
  Every local offset is angle-transformed and added to the controller position.

## Bitmap children 0x81E4, 0x81E6 and 0x81E3

- Particle cadence correction:81E4 subtype0 and81E6 subtype0/1 now use
  private whole-particle ticks before generic SS6 lifetime pre-decrement.
  1709744/17097C4 reach172343C, so lifetimes10/20 produce10/20 updates
  and no life0 update. Alpha-light starts.5, crosses negative on update17,
  clamps at18; clamp is on entry, not immediately after subtraction.
  Combo scale crossing likewise clamps on next entry and leaves RGB unchanged
  in the clamp branch. Per-slot remainder resets on ALL particle allocations.
  Shared GrowLancerWrathParticle.h and12-case x86W4WX regression PASS,
  with explicit crossing test. Subtypes outside audited selection unchanged.
  ClCompile PASS; not linked/staged. Real pool/GPU sampling remains open.
- `0x81E4` constructor `0x168ED1D`, subtype 0: lifetime 10, scale 2.0,
  alpha 1.0 and light `(1,1,0.62)`. Update dispatch `0x17092F8`, subtype-0
  block `0x17095FD` adds `sin(30 radians)*0.18` to scale and
  multiplies each light channel by 0.85 per tick. Particle dispatch
  `0x1621347..0x16213A7` falls through to `0x1640DD2`, which renders its
  own texture/light/rotation through the sprite helper. The earlier Combo4
  claim incorrectly followed the separate OBJECT renderer; it is withdrawn.
  Helper `0x96A03C` passes 30 unchanged to CRT sin, whose fallback at
  `0x1A36D8F` executes FSIN: radians, not degrees.
- `0x81E6` constructor `0x168EFDD`, subtype 0: lifetime 20, scale 3.0,
  alpha 0.5 and light `(1,3,10)`. Update `0x1709749` subtracts alpha 0.03 per
  tick until zero. Renderer `0x162D2E2` submits a native sprite with each
  light channel multiplied by alpha.
- `0x81E3` is an effect-pool object, not a particle or generic sprite carrier.
  Constructor `0x1480FD7`: lifetime/max lifetime 30, scale 4.0, alpha 1.0,
  white light. Update `0x1579AB4`: below remaining lifetime 11, alpha loses
  0.11 per tick; every tick mutates each stored light channel by multiplying
  it by alpha. Renderer `0x15A0B63` uses the terrain-alpha bitmap path with
  position, scale, stored light, rotation 0, alpha argument 1 and height 5.
- The base controller calls all three with null owner. The native SS6 adapters
  retain their real particle/effect pools and their recovered destruction via
  lifetime; none of these nodes now use the Grow Lancer sprite carrier.

## Child 0x5EB — wratha02

- Model registration `0x18D2D20` -> `Data\Effect\wratha02.bmd`.
- Constructor `0x1480E3A`: lifetime 20, scale/raw A0 0.6, alpha 0.6.
- Update `0x1579227`: alpha `-0.065` per tick; local movement `(0,0,-65)` is
  angle-transformed and added to world position.
  No lower alpha clamp occurs before the jump at `0x1579399` to the shared
  tail; expiry is checked before normal lifetime decrement, including life 0.
- Renderer `0xA49422`: RGB is `(0.37,0.57,1) * alpha`, independent of
  incoming object light. Float VAs `0x1B502A8`/`0x1B502CC` contain
  `A470BD3E`/`85EB113F`. Native BMD mesh 0, flags `0x42`, blend-mesh index
  0 (push at `0xA49527`), blend light alpha, literal UV `(0,0)`, texture -1.
- Decoded model identity: `wratha02.SMD`, 1 mesh, 2 bones, 1 action. Relevant
  embedded texture strings are `alpha_line.tga` and `dust01.tga`.

## Child 0x5EC — wratha01 and persistent continuation

- Shared aura scalar helper now used by runtime and x86 regression:24 total
  Wrath cases PASS. Both aura subtypes execute31 updates (11 rising/20 falling),
  one life1 continuation eligibility. Final alpha is-.097/-.74 without clamp;
  U=.496/.52,V=.043/-.17,subtype1 scale=.7051;subtype0 scale=.95-.15*sin30.
  Test does NOT query real buff maps or execute CreateEffect/pool/rendering.
  Isolated Win32 link PASS, SHA323C7F8FC757D7D99F4BC8BCC7D6D8CDEA3E549DDF5E5CDEA3DDDD818F659B9F.
  Not staged yet; previous7E619364 remains isolated client executable.
- Aura0/1 now initializes Timer0 and uses private whole-effect ticks:
  1579645/15798E2->1579AAF->15945F1 includes31 visits30..0.
  Continuation gate is exact life==1 at157959D and1579844, replacing
  the fractional adapter's <=1 gate. Native buff424/425 conditional remains,
  no fabricated permanent buff. Actual server-owned buff activation still open.
  Stage predicates shared between runtime and regression:16 controller/particle
  cases plus boundaries PASS, including frame3.5 taking stage0 then1 in one
  visit and71 controller visits. ClCompile PASS; aura scalar endpoint tests
  and actual buff recurrence not covered by those16 cases yet.
- Model registration `0x18D2CEB` -> `Data\Effect\wratha01.bmd`.
- Constructor `0x1480E78`:
  - subtype 0: lifetime/max 30, scale/raw A0 0.95, alpha 0, raw B0 -0.7.
  - subtype 1: lifetime/max 30, scale/raw A0 0.7, alpha 0, raw AC 0.25,
    raw B0 -0.7.
  - subtype 2 exists for upgraded skill paths and is not used by base 278.
- Update `0x157939E` rotates angle Z by 1.5 per tick. The exact split is
  `maxLife/2 + 4`, therefore remaining life 19 for a 30-tick object. Subtype 0
  uses alpha `+0.073/-0.045`, scale deltas
  `-sin(30 radians)*0.012/-sin(30 radians)*0.0009`, U `+0.016/+0.016` and V
  `+0.053/+0.008`. Subtype 1 uses alpha `+0.06/-0.07`, scale
  `+0.0001/+0.0002`, U `+0.01/+0.008` and V `+0.03/+0.01`. These constants
  were read directly from their referenced float VAs in the pinned dump.
  When subtype 0 reaches remaining life 1 it creates
  subtype 1 at the same position. When subtype 1 reaches remaining life 1, it
  queries the owner buff list for effect 424 or 425 and recreates subtype 1
  only while either buff remains active. This is the authoritative persistent
  visual ownership/cleanup contract; it must not be replaced by a fixed timer.
- Renderer `0xA49554`:
  - subtype 0 draws native mesh 0, flags `0x42`, using white * alpha.
  - subtype 1 replaces model light with the effect's per-channel light times alpha,
    then draws mesh 0 with flags `0x42`.
  - both base subtypes submit blend-mesh index 0 (pushes at `0xA49676`
    and `0xA4978B`), not SS6's provisional -2 default.
  - subtype 2 copies the effect light and draws mesh 0 in native mode 2.
- The subtype-0 to subtype-1 continuation passes light `(0.65,0.75,1.0)`;
  the values are the three immediate float loads before the `CreateEffect`
  call at `0x157963D`.
- Decoded model identity: `wratha01.SMD`, 1 mesh, 1 bone, 1 action; embedded
  texture `alpha_line.jpg`.

## Assets and open acceptance work

Renderer follow-up A49554..A497AD confirms aura0/1 mesh0 flags42,
blendmesh0,blendlightAlpha and animated UV. Subtype1 assigns BodyLight from
effectRGB*Alpha, not old model light*effectRGB*Alpha; native mapping matches.
verify_wrath_aura_renderer.py PASS checks selected call/assignment arguments.
All current Wrath corrections linked and staged with SHA256
323C7F8FC757D7D99F4BC8BCC7D6D8CDEA3E549DDF5E5CDEA3DDDD818F659B9F.
Previous7E619364 retained in isolated before-wrath GUID backup. Stage verifier
PASS2658base/79overlay/3rootData,Player hash/CRC. Not visual/buff runtime PASS.

The isolated staging verifier pins both BMD containers, `alpha_line.OZJ`,
`dust01.OZT`, `!Combo3.OZJ`,
`alpha_light.OZT`, `brocken.OZJ` and `Wrath.wav`.
No production or S21 source path is modified.

Remaining: prove server defense-cost damage bonus and buff duration/refresh;
complete the action/skeleton runtime comparison; build and compare runtime
cleanup after buff removal. Private RISE runtime IDs are assigned in
`GrowLancerResources.h`; direct S21 addresses/layouts are not transplanted.
