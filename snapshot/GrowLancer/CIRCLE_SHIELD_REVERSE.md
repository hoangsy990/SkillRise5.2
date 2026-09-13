# Circle Shield (skill 272) Season 21 client reverse

Status: `IN_PROCESS` — cast dispatch/action/root/model and direct cast children
are recovered. Separate buff-contact joint subtype3 constructor/update is
now transcribed in native ZzzEffectJoint.cpp, but its receiver is NOT wired yet;
the previous broad child-coverage statement did not cover this receiver.
Both controller branches and direct Shiny04/flare01 primitives are transcribed;
runtime visual parity QA remains.

## Identity and dispatch

- `SkillList.xml` identifies base skill `272` as `Circle Shield`, Grow Lancer
  tier 1, level 220, mana 100, AG 50, range 0, buff index 216 and PvP-only.
- The local cast dispatcher compares `0x110` and enters `0x10E3E77`. That path
  performs the cast throttle/packet send and does not create the visual root.
- The received-skill dispatcher selects jump-table slot 22 and enters
  `0x12CB2F7`. It marks the caster character active, sets action `187`, passes
  action argument `316`, creates `0x5DD subtype 0` owned by the caster object,
  and plays sound `1111`.
- Sound 1111 is registered at `0x18E2D4F` as
  `Data\Sound\CircleShield.wav`.

## Root controller 0x5DD

### Separate received buff-contact path — integration OPEN

The cast root is not the sole incoming trigger. Pinned main dispatcher
`12FE380` selects opcode `75h` via table `130505C`, entering `1300320`
and calling `129E990` at `1300369`. The latter reads actor indices from
bytes4/5 and6/7, and a big-endian buff selector from bytes8/9. Its diagnostic
string at `1B99864` says `버프이팩트 전달 %d` (buff-effect delivery).
Selector216,221,222 enters `129F04C`; it computes direction between the
two resolved objects and calls CreateEffect at `129F14B` with model5DD,
subtype1, owner second object. Remaining emissions in this handler still
need full tracing; this is NOT a completed native receiver adapter.

Read-only BuffEffectManager.xml SHA256
`201452C00D9D5CE0DB83820EF48EC4317A6654864C0BE9176A6389694A4AD6B0`
names216 Circle Shield and275 Bleeding. Thus the numeric275 branch in
this handler is not evidence for Clash skill275 or its learning source.
`verify_packet_effect_lead.py` pins dispatcher and selector evidence.
Do not copy opcode75 or source offsets directly into SS6; establish the
native server-authoritative trigger/packet adapter and paired-target QA.

The complete216/221/222 branch ends129F60C (unconditional exit), before
unrelated buff333 at129F60E. It creates exactly one5DD/subtype1 then three
80E3/subtype3 joints, scale40, owner second object, target position second
object. Using A for first object, the three start/angle pairs are:

| Call | Start | Angle |
| --- | --- | --- |
|129F28A|A.Position+(0,0,100)|A.Angle with Z minus135|
|129F447|A.Position+Rotate((100,50,0),A.Angle)|A.Angle with Z minus90|
|129F604|A.Position+Rotate((-100,50,0),A.Angle)|A.Angle with Z plus135|

Subtype3 constructor dispatch15DEA92/99 ->15DECA1 sets life100, tails10,
caller scale, random sign +/-1 atfieldA20, velocity10, reverseUV2. Its full
update/render contract is still being traced. Native BITMAP_FORCEPILLAR
currently handles subtype2 specially and all others as legacy life7/tails5;
This described the pre-contact implementation gap. The new subtype3 branch
now sets those fields separately and runs whole ticks with pre-turn tail
matrix, target+120Z homing,3D contact distance and literal speed brake.
It rejects null/non-live native targets as an explicit compatibility safety
guard. Pool reuse, renderer parity and incoming receiver remain OPEN.

- Constructor switch handler: `0x147FA97`.
- Update switch handler: `0x1575B60`.
- Render callback: `0xA48742`, an exact no-draw stub returning false.
- Subtype 0 lifetime is 30 ticks. Its constructor transforms local offset
  `(0,-50,0)` through the controller angle, adds the owner's position, then
  emits 20 records through the 14-argument SS21 `CreateJoint` family
  (`0x15C6594`) with type `0x80E3`, subtype 2, scale 20 and caster owner. The
  SS21-only final argument is zero. Each joint uses random angle X `[0,89]`,
  angle Y 0, angle Z `[0,359]`; the position passed to the joint call is the
  randomized transformed displacement from the owner point. Type `0x80E3` is registered at
  `0x18D1C0B` as `Effect\force_Pillar.jpg`.
- The subtype-2 constructor branch is `0x15DEB72..0x15DEC9C`: scale is the
  supplied 20, velocity starts at 0, lifetime is 18, max tails is 10, target
  position is copied then raised by 100 on Z, reverse-UV mode is 2 and light
  is `(0.2,0.45,0.65)`.
- Its update is `0x1612F8F..0x1613037`: velocity increases by 2 each tick;
  while pre-decrement lifetime is above 10, the joint turns toward the stored
  target position with turn value 10. Movement PRECEDES this handler (see
  ordering audit below); tail creation follows it. Cleanup is native Live=false.
- RISE's native `force_Pillar.OZJ` is byte-identical to SS21 (SHA-256
  `921F24FEA42D3182130BACAED610720640F71B5D46155A775D54F5BA61691898`).
  The port therefore uses `BITMAP_FORCEPILLAR` directly and adds only a
  subtype-2 guarded constructor/update branch; legacy subtype behavior remains
  unchanged.
- Subtype 1 lifetime is also 30 ticks and has no constructor burst.

## Root timed emissions

### Joint renderer audit (2026-09-13, scoped static evidence)

Renderer1618F44 visits the fixed500 pool plus entries from10F03DA overflow
manager. Live/NumTails>0/RenderFace and pass-selection gates1618FF7..1619057
match the native gates for Circle (byOnlyOneRender0). Blend type1 calls
18E7137, whose18E7158/18E715A arguments are ONE/ONE, matching native
EnableAlphaBlend; no extra ForcePillar-specific texture scrolling is selected.

Generic color16193FA..1619460 multiplies Light by S21 scalar+A4C.
The base constructor15C6850..15C6858 initializes that scalar to1; Circle's
subtype2 constructor15DEB72..15DEC9C and update1612F8F..1613037 do not
modify it. Thus the reviewed local chain agrees with native glColor3fv(Light).
External scalar writes are not established by this local-chain observation.
The auxiliary-pool selection for Circle is resolved below; other joint types
are outside this audit.

UV base1619640 computes (NumTails-j)/(MaxTails-1); reverseUV2 at161AA94
replaces both U values by1-U. V stays0/1. Face-bit2 at161ABE2 selects quad
161AC85..161AD92: current[0],current[1],next[1],next[0], with
(U1,0),(U1,1),(U2,1),(U2,0), matching native RenderJoints.
No new renderer adapter is warranted by these reviewed branches.

verify_circle_joint_renderer.py pins19 instructions plus scalar1 against
the module SHA. It is a regression of reviewed evidence, NOT proof of whole
renderer control flow, tail construction, overflow behavior or GPU parity.
Follow-up tail audit found an actual native FPS correction conflict:
S21 subtype2 sets MaxTails10, branches15DEC9C->15DEF1C->15E21B9, and
the common epilogue ONLY caps to50. Native formerly divided10 by FPS factor
after its switch, changing the UV denominator and retained trail length even
though Circle updates now run at whole ticks. The native FPS division is now
bypassed ONLY for ForcePillar/subtype2. Other SS6 joints retain their behavior.
Verifier now pins21 instructions and the native source guard. Isolated
Win32 build/link PASS; no GPU acceptance implied.

Non-blur tail15E37EF increments NumTails, clamps toMaxTails-1, shifts all four
vertices backward from indexNumTails-1 (15E3818..15E38FC), then creates
the negativeX/positiveX pair from +/-Scale*.5 transformed by the passed matrix
and translated by Position (15E3901..15E3B68), matching native CreateTail.
The negativeZ pair continues15E3B6C..15E3C9E, and positiveZ pair
15E3CA2..15E3DD0; both use Scale*.5, the supplied matrix and Position,
stored at vertices2/3 respectively, matching native CreateTail.
Important: S21 initial tail at15C6D4F reads storedScale before subtype2 assigns
the callerScale; native does the same. Do not silently move that initialization
without establishing the allocation/reset contract and actual reuse behavior.

### Circle fixed-pool selection (supersedes earlier overflow uncertainty)

CreateJoint15C65D6 scans500 records;15C6609 calls predicate10F0332 with
type/subtype/owner. Regardless of owner,10F0353 accepts ONLY type7FCE
(plus its subtype/owner restrictions). Circle80E3 returns0 through10F037B,
so15C6610 skips auxiliary allocator10F02CE. It uses first non-live fixed
record, or returns at15E21D9 if all500 are live, like native MAX_JOINTS500.
This is not a general overflow fallback for Circle. Do not add extra capacity
or replacement policy based on the generic renderer's auxiliary visitor.

Fixed-pool allocation15C6622 onward overwrites explicit fields, not the whole
record, before initial tail reads storedScale15C6D4F. Native has the same
ordering. Startup/global resets and real cross-type reuse remain runtime
coverage gaps; this does not justify inventing a Scale reset or moving the
tail initialization. The verifier now pins31 instructions plus constants1/.5,
including the type predicate and selected Z-pair operations.

### Joint ordering audit (2026-09-13)

Pinned dump function15E5838: 15E58DB gates generic movement,15E5900 builds
the pre-turn AngleMatrix and15E596D rotates (0,-oldVelocity,0), then adds
displacement without a render-rate multiplier through15E5A2A. Handler1612F8F
adds2, homes by10 only at life18..11. Homing1327059 uses CreateAngle/TurnAngle2
for Z then X, without FPS scaling. Tail1618B1F receives the SAME pre-turn
matrix, not a recomputed matrix.1618B45 decrements lifetime then1618B58 kills
at negative: life18..0 means19 updates,8 homing updates.

RISE subtype2 now has a private per-pool-slot whole-tick accumulator, reset
on every CreateJoint allocation. Shared GrowLancerCircleJoint.h preserves
move/acceleration/turn/tail/decrement order; native vector/angle/CreateTail APIs
remain underneath. No changes to legacy subtype0/1 or global FPS values.
Native MoveHumming math is used with fixed turn10 (the existing wrapper
would otherwise scale it by FPS again). Joint GPU renderer, actual pool
reuse/stress and visual comparison remain OPEN; helper tests do not prove them.

Correction2026-09-13: native formerly emitted child on every render-rate update
and an extra pair at bone42. Anchored1575B60..15766EE proves both subtypes:
pair window life>19 && life<25; child window life>10 && life<20 && life%3==0.
The latter resolves bone42, but then independently transforms local(0,-50,110)
by controller angle and adds controller position for child5DE. No Shiny/flare
call occurs in1575F0F..1576138. Constants1B70300=-50,1B4FA94=110.
Native now emits five timed pairs and three children (when owner/model valid),
with no extra bone pair, and controller uses integer ticks/Timer0.
Earlier prose below claiming another pair at bone42 is superseded.

Both update branches are structurally parallel and use the same recovered
constants. Direct float-VA reads prove Shiny04 light `(0.3,0.7,1.0)`, flare01
light `(0.25,0.18,1.0)`, local emission offset `(0,-60,100)`, and visible
child light `(0.45,0.35,1.0)`.

- At remaining lifetime 20 through 24, the controller transforms a local
  offset and creates one native effect type `0x7F6B` (`Effect\Shiny04.jpg`),
  subtype 0, owned by the controller.
- The exact Shiny04 subtype-0 constructor at `0x147FF27..0x147FFC6` sets
  lifetime 15, scale 3, alpha 0, angle X/Y 0 and angle Z to world time modulo
  360. Update `0x1576851..0x15768AA` adds alpha 0.2 while lifetime is at least
  10, subtracts 0.1 below 10 and destroys on negative alpha. The isolated
  effect-pool carrier now implements those exact fields instead of its former
  generic ten-tick symmetric envelope.
- The same window creates one sprite type `0x7EF7`
  (`Effect\flare01.jpg`) through S21 sprite allocator `0x172760A`, with scale
  10, the controller as owner, subtype 0 and rotation `WorldTime % 180`.
- `flare01.OZJ` is byte-identical to the existing SS6 asset loaded as
  `BITMAP_LIGHT`. The port now calls native `CreateSprite(BITMAP_LIGHT,
  scale 10, owner controller, rotation WorldTime % 180, subtype 0)` directly;
  the duplicate dynamic texture slot is deliberately unused.
- At remaining lifetime 11 through 19, every third tick, it resolves owner
  model bone 42 and emits another Shiny04/flare01 pair at the transformed bone
  position.
- Each branch finishes by creating visible child `0x5DE` with light
  `(0.45,0.35,1.0)` and the controller subtype.

## Visible child 0x5DE

- Model registration: `0x18D2B0E` ->
  `Data\Effect\circle_shields.bmd`.
- Decoded v0F identity: `circle_shields.SMD`, 1 mesh, 1 bone, 1 action;
  embedded texture `circle_shields.jpg`.
- Constructor `0x147FE13`: subtype 0/1 lifetime 10, scale/raw field A0 1.0,
  mesh selector `-2`, and angle Y `+70`.
- Update `0x15766F3`: subtype 0/1 transform local movement `(0,-3,0)` by the
  object angle and add it to world position; scale/raw field A0 decreases by
  `0.05` per tick.
- Render callback `0xA48749` copies effect angle to the model and calls the
  native BMD mesh path with mesh selector `-2`, effect alpha and mode argument
  2. No replacement renderer is required.

## Asset provenance

- `circle_shields.bmd`: SHA-256
  `6A985AC9EBF517DD86C164F23DBF6D0F66DD527BFEF1D3144B75D77168FA1A98`.
  The isolated v0C conversion is
  `79D241ACBAB24D8B31D8DB357D2D75B453431A6AF6D27CA48B2CC2A971DC7BDB`
  and decrypts to the pinned plaintext SHA-256
  `C0FD780A4F48F06CB514AD9F808BFD9722C6A0C41CFE6AC479284697D087E400`.
- `circle_shields.OZJ`: SHA-256
  `F5ED70BAF0746EB40CC7687A49242D2F0E6DAEB26B3B4C442C65CF9225B83A0C`.
- `force_Pillar.OZJ`: SHA-256
  `921F24FEA42D3182130BACAED610720640F71B5D46155A775D54F5BA61691898`.
- `shiny04.OZJ`: SHA-256
  `568A27A2B3F0E8B8004FDA7DA9CBC9F1C3A83E933AC159554CB5F3A573CC3B13`.
- `flare01.OZJ`: SHA-256
  `874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8`.
- `CircleShield.wav`: SHA-256
  `3B2A719E6A69DBEEC590AF595A98D96C32E3E8262315E51D47CD48EBE020E7B3`.

All source files remain read-only. The BMD conversion and every copied hash are
verified only under the isolated Grow Lancer test client.

## Still open

- UV/draw-mode audit closed for Shiny04's selected defaults: A13A00 writes
  metadata(1,1,1.0); allocator1726E0F stores columns/rows1 and interval1.
  Renderer172732D compares columns/rows to1 and selects17274DA full UV
  (0,0,1,1), bypassing animated atlas division. Mode4 is NOT a blend mode:
  sprite primitive18E8E9F tests bits1/2/4 for rotationX/Y/Z respectively;
  zero-vector ctor960443 precedes it, so4 sets only Z to supplied rotation.
  Native ZzzOpenglUtil.cpp RenderSprite uses Angle(0,0,Rotation) and default
  full UV; this selected behavior needs no additional renderer/atlas adapter.
  Blend selection is the separate sprite subtype0, already traced additive.
  This closes only default UV and rotation-axis mapping, not sprite visibility
  producer, owner deletion, all draw passes or gameplay visual parity.

- Sprite wrapper172760A forwards to allocator1726C50; arg20 is subtype,
  arg24 is draw mode stored8C, arg28 is metadata copied to90/94/98.
  Shiny caller metadata defaultA13A00 is(1,1,1.0), mode4, subtype0.
  Render manager1726F11 selects additive for subtype0;1726F87 clears Live
  for pass0/2 after draw. Native manager has corresponding branch/cleanup.
  Render1727079 changes frame multiplier by+.1 visible, -.1 invisible with
  bounds.2..1. Native generic invisible branch incorrectly adds; corrected
  ONLY private kCircleShinyBitmap to render-step envelope, preserving SS6.
  This multiplier differs from effect alpha; metadata UV/mode4 consumer
  and native flare01 private identification remain open.

- Renderer follow-up:15A1514->15AE9BF, subtype0->15AE9DC..15AEAE8
  computes Light*Alpha and calls sprite172760A at15AEAE0. Owner argument
  pushed15AEAA4 is the Shiny effect itself, NOT effect.Owner. Native carrier
  now passes &effect only for CircleShiny. Sprite extra arguments0/4 and
  metadata remain to audit; no full renderer parity claim.
  Child update15766F3..1576835 confirms both0/1 move rotated(0,-3,0),
  scale-=.05 without clamp. Child now initializes Timer0 and uses private
  whole-tick lifecycle; no non-source scale clamp. Native ClCompile PASS.

- Re-audit2026-09-13: prior native Shiny04 alpha clamping was NOT in S21.
  1576851..15768AE adds.2 at life>=10, subtracts.1 below10, sets life0
  only if alpha<0, never clamps alpha itself. Corrected private carrier to
  shared UpdateCircleShinyTick and whole-tick lifecycle; constructor Timer0.
  Test x86 /W4 /WX PASS fourFPS cases: six additions peak1.2, ten subtractions
  including life0 ends.2 at16 updates; negative-alpha branch also tested.
  Native ClCompile PASS. This supersedes earlier static-complete wording;
  renderer/owner/visual parity still require separate proof.

- Runtime comparison of the guarded SS6-native `BITMAP_FORCEPILLAR` subtype-2
  joint, exact Shiny04 carrier and native BITMAP_LIGHT sprite against SS21,
  including tail width, UV direction and FPS-normalized acceleration. Static
  constructor/update/render/lifetime mapping is closed.
- Server-authoritative PvP target validation, AG decrease effect and duration.
- Recipient class/skeleton mapping in SS6 and isolated runtime comparison.
