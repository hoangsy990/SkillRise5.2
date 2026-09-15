# Breche receive-root children — IN_PROCESS

Parent: normalized skill279 -> effect0x5FD subtype1. See BRECHE_REVERSE.md.
The constructor emits five CreateEffect calls; these are effect-pool records,
not CreateParticle calls merely because their IDs are texture IDs.

## Cached owner pose and native producer audit (2026-09-13)

### Position-bracket membership and action predicate

Creator-input closure: the four direct calls to10C2FD4 are in12ED574
(12ED667/12ED6B3) and13F4E96 (13F5277/13F52C1), not Breche dispatch.
12ED574 walks a separate received list: byte5 count, records starting at6.
It resolves record key bytes0/1 to a character; bytes2/3 become character
word322E through1314F4C. An explicit CCF562 value whitelist selects the
normal creator(value,record byte4,character); otherwise positive record
byte5 selects creator(-1,record byte5,character), setting character3234=1.
The normal path sets3234=0. No skill279 selection occurs in these audited
creation branches. Packet opcode and gameplay labels are not inferred.

13F4E96 reconstructs the same association from character state:3234 true
uses creator(-1,character3230,character); otherwise word322E!=FFFF uses
creator(word322E,byte322C,character). Getters BB62BB/BB875A/1314FAE were
decoded at entry and confirm these exact field sources. This isolates the
offset dependency to the target's existing visual configuration. It is not
a Breche child that the skill constructor must spawn.

Compatibility boundary: preserve native target Position and native cached
bone sampling. Do not create S21 auxiliary configuration records in the
Breche package. S21-only configurations requiring this offset must remain
explicit target-configuration QA/compatibility cases; they are not evidence
that ordinary targets need a fabricated offset. Remaining Breche work returns
to root model update/render and emitter integration; broader configuration
support is not silently counted as implemented or visually verified.

Factory follow-up:10C464B..10C501C starts with null result. Configuration
category getter10C656A reads descriptor+0; category2 accepts only descriptor
value1/2/3 (getter10C6534 reads+4). Their constructors10C5A73/10C5A90/
10C5AAD install vtables1B879D0/1B87A14/1B87A58. All three virtual+10
entries point to1097AFE. Anchored1097AFE..1097C15 zeros a relative vector,
looks up bone name string1FB0AD4="sit_pos_0" on auxiliary object+1C via
13164A0, transforms through132F433 with output record+24 and trailing0,
then multiplies all three output components by auxiliary object.Scale(+A0).
This is a bone-derived offset, not a fixed Breche displacement. Full
TransformPosition argument semantics should reuse its separate audit.

Further anchored lookup/transform closure:13164A0 returns a MATRIX POINTER,
not a bone index. For the caller's boolean1 it prefers object+318 cached
matrices, falling back to global70B5400 when that pointer is null. Missing
model/bone array/name returns null.1097AFE passes Translate=false to132F433,
which invokes the transform without BodyScale/BodyOrigin, then caller scales
record24 separately by auxiliary Scale. Zero relative input therefore samples
matrix translation before that explicit scale; it is not a world-position add.
132F433 returns without touching output on null matrix, but the caller still
scales the existing record24. Do not silently claim missing bone yields zero.
Native BMD::TransformPosition(...,false) has the corresponding transform path
but NO null check; compatibility must handle missing matrices explicitly before
calling it. This source behavior does not authorize copying object layouts or
global matrix addresses. Thirteen selected instructions are pinned by the
Obsidian action verifier because this auxiliary system is shared.

Category2 model setup closure: the three vtables above resolve virtual38 to
1098DEE/1099B2F/109A909 respectively. Their complete setup bodies assign
model73F/740/741, resolve the model into record20, reject null, assign the
auxiliary object's Type and allocate matrices using its model bone count.
Shared1098102 first sets auxiliary Scale to float32(.85), constant1B50100.
Registry identifies Data/Skill/Giant_DarkWizard_01, Giant_Elf_01 and
Giant_GrowLancer_01 (registrations18D5018/18D506D/18D50C2).
These are descriptor category2 configuration models, NOT additional skill279
root children. They do not establish the model for every Muun-backed record.
The verifier pins virtual38/10 associations, model IDs, model-pointer stores
and the scale constant. Asset loading/parsing and native compatibility remain
separate; no model was staged on registry naming alone.

Read-only asset closure via inspect_giant_attachment_assets.py: all three
source BMDs are v0F and parse through meshes, animations, bone keys and tail.
DarkWizard:5 meshes/112 bones/8 actions, sit_pos_0 index97 parent20,
SHA710EF1764CE5EB6E19C618CA6FA6612A216B484E63F72BE0F7BE26E7440C4949.
Elf:4 meshes/112 bones/8 actions, seat97 parent20,
SHA7ED8A4797796AA86568E87EDE260EFC6303A9100863C3484AFE5E8B0BB35EF9D.
GrowLancer:6 meshes/116 bones/8 actions, seat102 parent20,
SHA3B66BD850CF0BF2D22A44472ED1C3F57F744AA8919378E32653B45BE685E202B.
The differing seat index confirms why copying one numeric bone index across
configurations is wrong. Texture names are emitted from mesh records, not
filename guesses; their resolution/hash/native load are not yet verified.
No asset conversion or staging occurred. Structural parse is not native BMD
load, animated seat-position validation or visual parity.

Registration follow-up18D5018..18D5134: each model loads mesh textures with
directory Skill\\ via18BB6C0. Additionally18F8FA2 registers84E8..84EB using
Giant_darkW_Neon_render.jpg, Giant_Elf_Neon_r.jpg,
Giant_GrowLancer_Neon_render.jpg and Giant_GrowLancer_Jewel_render.jpg.
These are not all present in BMD mesh texture names. Inspector pins their
binary strings and candidate OZJ hashes, decoding JPEG data after24-byte
headers: first three128x128, jewel32x32. Registration alone does not prove
render usage; consumer and actual runtime texture binding remain OPEN.
Hashes are recorded in inspect_giant_attachment_assets.py; source untouched.

Extra-texture consumer: fully decodedA5D5F6..A5DB14 has ten calls to the
mesh wrapper968CB3, not particle creation. First four draw meshes0..3 with
flags2 and texture override-1. CallsA5D82D/A5D87E redraw meshes2/3 with
flags42, blend indices2/3, explicit textures84EA/84EB and shared pulse value.
Then meshes4/5 use flags42 and override-1. Final meshes0/1 use flags44 and
texture7F1F, so the four extra registered images are not the entire material
dependency set. Model RGB is changed between pass groups. Pulse uses helper
18F930D before scaling/offset; helper semantics and outer dispatch/restoration
are not yet closed. verify_giant_render_passes.py pins ten call sites and
explicit extra texture/pass arguments; it does not certify full renderer parity.

Binding closure:A1C016 pushesA5D5F6 then741 and callsA1DCE6. Its map helper
conditionally stores the supplied function for the supplied key; this proves
registration association, not an unconditional overwrite of an existing key.
Pulse18F930D is NOT safely replaceable by guessed sin(time): the caller passes
first argument-4 (constant1B50150), second0. Helper converts phase/time and
6283.185546875/firstArg to signed integers viaCVTTSS2SI, applies signedIDIV,
then converts remainder back to float and multiplies by .001 and firstArg.
It callsA01178 and applies a sign depending on comparison with floatPI,
then maps(result+1)*.5; caller maps that*.6+.4. A01178 wraps1A29A90 with
float/double conversion. Exact transcendental identity, global70B53D8 time
producer and edge behavior remain OPEN. Preserve integer/sign operations in
the contract rather than inferring a frequency or substituting float fmod.

Further pulse closure: scalar fallback1A36C9F of1A29A90 executes FCOS at
1A36CBD, with FPREM1 reduction and another FCOS at1A36CF4 for large inputs.
This identifies cosine, not sine; optimized CRT path numerical parity is
still separate. Filtered direct-store scan found13268FB as a clock writer:
13268D4 calls IAT1B4C9A8, pinned import winmm.dll!timeGetTime, converts
unsigned DWORD to double via correction table(0,4294967296), then to float
and stores70B53D8. This is a sampled float clock, not a per-effect elapsed
counter. Large-clock float precision and CVTTSS2SI range behavior must not
be silently changed in compatibility code. Other indirect writes/scheduling
are not ruled out by the direct-store scan.

Native comparison: RISE/CustomItemRenderMesh.cpp Render22 uses work=a1/100,
period int(6283.185546875*work), immediate timeGetTime() and skips cosine
when reduced phase equals0. S21 divides by a1 and uses its sampled floatclock;
at a1=-4 the divisors differ(-251 versus-1570). At phase0 native returns.5
while S21 returns0. Therefore Render22 is NOT an equivalent reusable helper.
ZzzAI.cpp also declares native WorldTime as double and assigns CTimer elapsed
time, unlike S21's float timeGetTime snapshot. Do not globally change either
existing helper/clock: existing item renderers consume Render22. Any new
compatibility adapter must own its explicit sampling/numeric contract. The
verifier checks source differences and counterexamples, not live timing.

Private implementation candidate: compat/S21GiantPulse.h implements ONLY
the verified fixed(-4,0) call and final mesh remap. Input is an externally
sampled float, with x86 CVTTSS2SI and signed remainder retained; it neither
reads nor changes global time. Nine boundary checks plus1570 tick range checks
compile/run x86 W4/WX fp:precise. Out-of-range/infinity/NaN conversion follows
the x86 intrinsic, not undefined C++ float-to-int casting. std::cos uses the
native CRT after float-to-double conversion; exact S21 optimized CRT result
parity is not asserted. This helper is not yet connected to a renderer or
included in the client project; configuration activation remains OPEN.

Dispatch consumer found1887DF2..1887E5C: lookup key is object.Type(+5C)
in the same1E050B0 map; it calls the registered function indirectly at
1887E4E with model/object arguments. A true result skips the default body
draw but continues to1893296 at1887ED0. This closes registration-to-consumer
association; it does not prove the subsequent callback restores model RGB.

Native CCustomRenderEffect::RenderItemMesh has TWO distinct paths: custom
mesh-list rendering saves/restores BodyLight around each pass, but its
default-effect path calls runtime_make_render directly without restoration.
runtime_make_render saves body_light at entry yet does not perform an
unconditional restore at exit (only selected branches use that copy). Thus
neither path can be assumed to provide automatic scope cleanup for a new
Giant renderer. Do not patch global item behavior for this skill task; keep
any needed restoration scoped to the new auxiliary renderer after its
post-render consumer requirements are verified.

Complete postcallback1893296..189346C has no741-specific dispatch or RGB
restoration. It checks object+358 membership IDs38,8D,92,9E,F0,112,116
(hex); any true draws body via968C58 using incoming model RGB, then returns.
Otherwise it checks56; true writes new model RGB then draws body; false
returns without drawing. Both draw branches use flags2, blendindex-2,
blendlight1, object alpha/UV/hidden-mesh and texture override-1.
Therefore restoring RGB immediately after the ten-pass renderer would change
the first postcallback branch. If a private native scope is used, its boundary
must encompass the equivalent post-render behavior as well. Membership IDs
are not assigned guessed buff names here; native status mapping remains OPEN.
Verifier now includes both conditional body callsites and complete callback end.

Native overlay counterpart is in Draw_RenderObject, ZzzObject.cpp2622..2630:
Freeze draws RenderBody(RENDER_TEXTURE,Alpha,-2,1,UV,HiddenMesh) with incoming
BodyLight; else BlowOfDestruction writes(.3,.5,1) then the same call. This
resembles the two S21 postcallback branches but does not cover the larger S21
membership set automatically. Keep numeric membership/name mapping separate.
Do not substitute RenderPartObjectEffect instead: that path's status branches
can emit/delete a skull effect on Stun and route through equipment-specific
RenderPartObjectBody. The auxiliary object must enter the appropriate native
object render path, not acquire equipment side effects through a convenient
custom-item API. Source verifier now checks the exact late-overlay pair.

Other category branches switch on explicit descriptor values rather than
using a generic fallback. Example0x1A02 ->constructor10C5C10 ->vtable
1B87618 ->virtual+10 at10BBBC4. The factory contains additional distinct
virtual implementations; do not generalize1097AFE to all records. Unsupported
values return null through10C500D. Helper10C4474 yields category2 only when
first input is-1/0xFFFF and second input>0; remaining categories depend on a
separate predicate. Descriptor input gameplay names remain unclassified.

Next narrow boundary: callers of manager creator10C2FD4 determine which
equipment/configuration produces these records. The Breche package should
consume target native positioning, not recreate this entire manager or copy
S21 bone/layout offsets. Any unsupported target configuration remains an
explicit compatibility case, not an invented global movement correction.

Initializer closure: anchored1089CD3 stores the supplied character at
record+14, character+3254 at record+18 (1089D1D), and a newly allocated
0x3D8-byte object at record+1C (1089D68). The new object's Owner+34C
points back to record+18 at1089D98. Its initial Position copies the
character Position at1089E40..1089EC2. This is an auxiliary object,
not a second character selected from the incoming Breche packet.

Manager creator10C2FD4 obtains/configures a record, calls that initializer
at10C3049 with its character argument, and on success inserts the record
under character+3E at10C3055..10C3077. This closes the association with
the key-based lookup10C30B1. Allocation failure deletes the failed record;
no native allocator/layout from this S21 path is copied.

Record update108A1BE first copies character Position/Angle from+18 to the
auxiliary object+1C (108A1C7..108A2CD). Its flag producer108A3D6..108A408
sets record+34 to character-object Live AND NOT character byte+31E8.
The latter byte's gameplay meaning remains unclassified. Before this flag
write, a virtual slot+10 runs at108A3D3; initialization also calls virtual
slots+38,+8,+10. Record vector+24 producer/configuration remains to trace.
Thus update bracket's false path restores the auxiliary object's saved
base position to the character; true path adds the record offset under the
already documented action predicate. This offset is not Breche velocity.

Anchored10C30B1 looks up its character argument's signed key(+0x3E) in
manager container+4 and returns that entry's record pointer, or null if no
entry exists. Character renderer13DB9D7 calls this at13DC220 with the
character being rendered, then requires record field+0x38>0 via10C6510
and predicate108AB01 before adding record vector+0x24 to its position.
9FAC9C is the direct getter for that vector. This associates the record
with a character key; it is not a global transform applied unconditionally.

Complete predicate108AB01..108ABD3 requires nonnull record+0x18 and
nonzero record byte+0x34. It accepts the referenced object's action+0x2A
only in half-open ranges [38,50), [71,79), [103,111), [300,327),
[335,342). Otherwise it returns false. Update bracket108A8F5 calls this
predicate only for its true flag; accepted records copy object(+1C)
Position plus vector24 to object(+18) Position. Rejected/false-flag paths
copy the base position without adding the vector. Rendering adds a further
record+38 gate; do not assume update and render gates are identical.

Breche caster action190 is outside these ranges, but the receive root's
owner is the secondary target, whose action can differ. Thus this predicate
does NOT establish zero offset for every Breche owner. Remaining evidence:
record construction linking +18/+1C to the keyed character and assignment
of flags/offset, including target membership. No native global offset or
guessed mount classification has been implemented from numeric ranges.

### Producer identified: native animation helper correspondence

### Scene ordering closure for the ordinary state-6 path

Anchored outer function18EE7E4 contains both update and render dispatches.
State value6 calls18F2EBF at18EE99F. Within that update function,
18F3BB1 calls character update13F3FB0;18F3C30 calls effect update1595834,
then18F3C3F calls particle update wrapper1620972. Later in the same outer
function, state6 reaches the render decision at18EF0E4. The ordinary branch
calls18F60EF at18EF155; the alternative invokes a virtual method at18EF14A
and is NOT covered by this ordinary-path conclusion.

18F60EF calls secondary effect renderer1596B00 at18F681F, then character
renderer13F3BA4 at18F682C. The latter iterates characters and checks live
and visibility at13F3E64..13F3E7C before calling13DB9D7 at13F3E9C or
13F3EAE (selection flag0/1). That call reaches the cached matrix producer
documented below. The native MoveMainScene/RenderMainScene split has the
same update-before-character-render ordering. This supports retaining native
cached-pose sampling; it does not justify an extra animation evaluation in
the Breche emitter. Do not claim every scene/virtual path or every possible
matrix writer is closed, nor label cached matrices as the current tick pose.

Additional native producer: ZzzObject.cpp::Calc_RenderObject also evaluates
o->BoneTransform with the same BMD::Animation arguments when enabled.
Its following BMD::Transform call consumes the matrix buffer. Character
default player animation uses Calc_ObjectAnimation; transformed monsters
and special paths must retain their native producer instead of forcing the
player path. BodyScale remains shared model state at effect sampling, not
automatically the selected owner's Scale. Ownership/scale association and
the source position-offset bracket remain the next emitter-specific gates.

Follow-up anchored decode identifies 0x1887965..0x1887B8A as the
Calc_ObjectAnimation counterpart. The complete helper checks object Alpha
against float0x1B4DF00=0.01, resolves the model, sets height/contrast/light,
copies owner scale at0x1887A1E, action at0x1887A2B and position, then
selects cached owner matrices when object byte+0xD is true. The true path
calls getter1316430 at1887B08 and passes its result to132D35E at1887B11;
the false path passes global0x70B5400 at1887B78 to the same function.
Arguments are output matrices, object+D0 current frame, +D4 prior frame,
+2C prior action, +164 angles, +170 head angles, false, !Translate.
This matches native Calc_ObjectAnimation's named-field/API contract.
The world-dependent Alpha subtraction constant1B4E6D8 is0.15; the world
predicate itself was not classified in this follow-up.

132D35E is anchored at its entry through ret0x20 at132DC48. It bounds
actions, computes current/prior frame indices and interpolation fraction,
and passes output+boneIndex*0x30 into matrix composition D3102E at
132DC08/132DC2F (root/parent branches). Full interpolation math parity is
not claimed from this inspection. The output buffer is not merely a texture
or UI object despite unrelated raw +318 numeric hits elsewhere.

Character caller13DB9D7, decoded sequentially to13DC0C1, selects mode2
for default model type0xA94 at13DBD85..13DBD98; mode2 calls1887965 at
13DC0BC with its object argument. This corresponds to native default
MODEL_PLAYER -> CHARACTER_ANIMATION -> Calc_ObjectAnimation. Outer scene
ordering and other writers still need closure before emitter activation.

Allocation/lifetime helper1316385..13163F8 frees an existing +318 buffer,
allocates count*12*4 bytes with overflow handling, and stores the result at
13163EE. Helper13163FB..131642F frees and clears it. These allocate/free
matrices, not evaluate the current pose. The bulk character cleanup at
13F6F33 is not a pose producer; raw getter xrefs there must not be counted
as animation updates. Similarly F092A9 is a UI virtual-call member, not
this owner bone field. These rejected leads prevent repeating that search.

Pinned S21 helper 0x1316442 was decoded from its function entry through
ret 4 at 0x131649D. It resolves owner.Type (+0x5C) via the model manager,
rejects a missing model or null owner field +0x318, bounds the bone index
against model.NumBones (+0x22), then returns owner[+0x318] + index*0x30.
This is a cached matrix lookup, not an animation evaluator. The emitter's
existing 0x132EC63 lookup therefore does not itself establish pose freshness.

The anchored branch 0x142F522 additionally copies object.Position into
model.BodyOrigin; 0x142F5A3 writes object[+0xA0] into model[+0x74], and
0x142F5B3 copies the action. These writes occur before the visibility check
at 0x142F5C3. This closes another scale writer but not last-writer identity
or the producer of owner[+0x318]. No offsets are ported into native code.

Native ZzzObject.cpp::Calc_ObjectAnimation sets BodyScale, CurrentAction,
and BodyOrigin, then calls BMD::Animation with o->BoneTransform when
EnableBoneMatrix is true (global BoneTransform otherwise). Its Alpha<0.01
guard can skip evaluation. ZzzCharacter.cpp's CHARACTER_ANIMATION branch
calls this helper, with the default MODEL_PLAYER case selecting that branch.
ZzzScene.cpp separates MoveMainScene (MoveCharactersClient, MoveEffects,
MoveParticles) from RenderMainScene's RenderCharactersClient call. This
proves a render-path producer exists; it does NOT prove it is the only writer,
nor that a particular emitter sees a newly evaluated pose on each update.

Activation stays gated. Next exact evidence: trace the S21 writer of the
cached owner bone buffer and its scene ordering; audit other native writers
before choosing cached-pose sampling versus explicit pose evaluation. Do not
insert a new BMD::Animation call or force owner.Scale without that comparison.

## Registered source assets (read-only, 2026-09-12)

## Particle update follow-up (2026-09-13)

### Common movement and retirement follow-up

Reuse closure: constructor allocator172513C calls172502F with the same
globalA726100 used by retirement.172502F obtains count via1725983,
decrements to last index, and if nonnegative copies the final handle
via1724F16 (begin+index*8) and1724E90. It removes that vector element
via17257E0: shift helper172491C, last-handle destruction1724AD5,
end-=8 at1725869. Empty storage instead allocates0xB4 bytes at172507A,
constructs9B37A2 and wraps the new record.172513C sets byte+4 and Live
back to1 before returning. This confirms a reusable retired-record pool.
Active-list retirement172579F calls17255D1, which relinks previous/next
at17255E3..17255FB and decrements active count at1725604, then17248B9
releases the node's handle and node storage through helpers. Refcount
and allocation-failure/growth internals are not claimed fully closed.

Native comparison: ZzzEffectParticle.cpp::MoveParticles currently decrements
LifeTime by FPS_ANIMATION_FACTOR before the per-type switch, sets Live=false
at<=0, but still runs the switch. S21 checks incoming life/Live before the
update and decrements once afterward. A Breche-only update entry before
native decrement (plus native pool reuse) is required; do not globally
reorder SS6 particle lifetime. Random per-tick draws must also use the
GrowLancer tick policy rather than random per rendered frame. Pending
implementation must preserve alpha overshoot and death-test ordering above.
Existing GrowLancerTick.h::AdvanceWholeTicks is explicitly effect-specific:
it calls update even at life0, then invokes EffectDestructor. Do NOT reuse
it unchanged for these particles. A particle-specific tick path must check
incoming life first and release only native particle Live state; it must
not pass PARTICLE to the effect destructor.

Generic constructor164152E sets byte+68=1, so the common movement call
is enabled.1641638..164168E zeros all three vector+70 components.
Re-read selected constructors1667599..166775C,164BBFF..164BD9F,
166C479..166C619: none changes byte+68 or vector+70. Their tails reach
16A9524 enqueue without an additional field write.
13272A0..1327351 constructs a matrix viaD30B1A, multiplies input vector
throughD3189D, then adds the resulting XYZ to Position. D3189D uses
the three matrix-row dot products only, with no translation column.
For these zero-vector particles and finite angles this adds zero;
their selected update's positive Z increment remains the actual motion.
No owner-following behavior is implied: the supplied owner is null.

Retirement1725235 checks the handle with17243E3;172527C invokes
17252D2 with0 (sets record byte+4=0), then1725289 clears Live+8.
17251C5 is an empty method, not a child-destroying callback.
17252B2 calls17251CE on globalA726100.17251CE rechecks the handle,
then1725213->172596C->1724615 forwards it into a vector-like container
(compares end/capacity, normal insertion17245B9 or growth1724669).
This is evidence of a retention/reuse path, not immediate raw deletion.
Container consume/reuse, growth and active-list erasure internals still
require audit; do not implement delete-owner or S21 smart-pointer layout
in native5.2. No runtime parity or complete pool-lifecycle claim.

Pinned dump, anchored branches of 16AA2ED; these are particle records,
not effect OBJECT offsets to transplant into SS6.

| Selected particle | Update body | Fade when incoming counter below | Scale decrement when incoming scale positive |
|---|---|---|---|
|8084/0|16DA4CF..16DA804|15|(R%3+5)*.01|
|806E/4|16B9584..16B98B9|10|(R%3+6)*.01|
|8085/0|16DF6FD..16DFA32 (previous audit)|10|(R%3+7)*.01|

806E subtype chain is 16B8BDD (0),16B8C9E (1),16B9090 (2),
16B9151 (3),16B955E (4); equality at16B956D selects16B9584.
All three selected bodies subtract .2 from Alpha below their counter
threshold; otherwise, incoming Alpha<1 adds (R%2+2)*.1, else sets1.
The increment can overshoot1: do not add an immediate clamp.
Alpha<.1 clears Live, then RGB is still written as savedRGB*Alpha.
Scale<=0 clears Live; positive scale is decremented without a second
death test. Position.z then increases by parameter+6C and rotation by3.
There is no early return after Live clear in these selected bodies.
8084 subtype6-only tail and806E subtype9-only tail are excluded, so no
rotation reset or extra Z multiplier is part of these Breche particles.
No child allocation call occurs in these selected update bodies.
8084 exits16DA804->16DE8BD->172343C;
806E exits16B98B9->16BBB1A->172343C.

Common manager ordering:16AA648..16AA66E checks counter<=0 or !Live
BEFORE dispatch. That path calls1725235 at16AA68D, advances iterator,
calls172579F at16AA6C3, then releases temporary handle1724E3B.
Exact erase/destruction internals of these helpers remain OPEN.
For updated particles,172343C..172345F decrements counter once AFTER
the selected update;1723461..172348F adds global1DD801C to field+AC
(meaning/value not inferred), advances iterator then releases handle.
Thus life0 never reaches the selected particle update, unlike the
previously documented effect-pool update-before-life0-cleanup ordering.
Live cleared by the selected body reaches removal on a later manager
visit; render eligibility between visits still requires audit.

Predispatch16AA6E6..16AA743 conditionally calls13272A0 with Position,
Angle,vector+70 when byte+68 is set. Constructor value/producers of
byte+68 remain OPEN; do not claim the branch-local Z motion is the
entire common movement yet. Owner+50=0 for these emitted particles
skips the following owner-position block16AA743..16AA7D2.

Constants re-read from dump:1B4DD48=1,1B4DD3C=0,1B4DF00=.01,
1B4DF04=.1,1B4DF08=.2,1B4E860=3. No client binary or runtime PASS.

## Asset registration evidence

### Native particle reuse map (2026-09-13)

### Owner-bone helper mapping

Character scale writer FOUND:13F3EF8 checks character.Object live, updates
visibility, calls14356FB then1424B34 then142F1E9 thenDCDE3D with
character and character+3254 object. In1424B34, object.Type selects model
via968B22/EF2A81 at1424C07..1424C19;1424C54..1424CC4 copies object
Position to model+78;1424CC6..1424CD5 copies object+ A0 to model+74;
1424CD8..1424CE5 copies object action+2A to model+A8. This is the
source-side producer analogous to native MoveCharacter BodyScale=o->Scale.
It does not prove that no later character/effect overwrites shared state;
keep multi-actor last-writer behavior separate from this proven assignment.

Bracket108A8F5 does not set model scale: when flag!=0 and108AB01 succeeds,
it sets object at this+18 Position to object at this+1C Position plus vector
this+24, all three coordinates. Otherwise it copies source Position without
offset. No model/bone lookup occurs in the body. Therefore10C343C(1/0)
around effects is a position-offset bracket, not owner-scale setup.
Its association with the Breche root owner requires container membership
evidence; do not apply this offset to every target or invent its meaning.
Next:142F1E9 pose production and caller order; identify this+18/+1C objects
only if they participate in the selected owner's runtime chain.

Scene-call anchors: direct callers of1595834 are18F3C30,18F3D6A,
18F4158 in functions18F2EBF,18F3CE9,18F40BE respectively. Each passes
ordinary poolA5B14F0, then calls1618EF4, then particle manager1620972
withA490B9C;1620972 directly invokes16AA2ED. All three bracket the
effect/joint/particle segment with10C343C(1) and10C343C(0).
10C343C iterates a container and passes the flag to108A8F5; its semantics
must be traced before treating the bracket as pose setup or a rendering pass.
13F3FB0 is called earlier in all three contexts; its first stage clears
terrain bit2 over65536 entries, iterates400 character objects (character
+3254), populates occupancy, and writes visibility byte+B. This is a
character-loop anchor for following pose/scale setup, not an assumed
complete decode of the function. Context classification (gameplay/login)
still needs caller state evidence rather than inferring from address order.

S21 model-manager identity and effect-pass audit:968B22 uses static-init
guard/TLS epoch but returns global manager pointer1E04C8C at968B9D.
It takes no owner argument; the owner.Type pushed before it is consumed
by subsequent EF2A81 (ret4). This is not a per-owner model factory.
MoveEffect14B76C0 predispatch through14B780C copies owner Position into
effect field1D0 and samples terrain height, but does not bind owner scale
into model BodyScale. Exact direct-call leads to14B76C0 are10F02C3,
1594686,15946B4,15958D3. Anchored1595834 loops ordinary effect records
(200, or150 under its world predicate), calls14B76C0, then10F027E loops
another200 records and calls the same update. Neither per-record loop
sets model scale or evaluates owner pose before calling the update.
The preliminary world6 helper chain at159584B..1595874 is not yet
decoded and is not assumed to refresh every emitter owner.
Next scene-level callers of1595834, plus S21 character model+74 writers,
are the remaining scope for resolving which shared scale reaches emitter.

Scale/scheduling follow-up: native MoveCharacter assigns shared Models[type]
BodyScale=o->Scale before AnimationCharacter. Gameplay ZzzScene.cpp calls
MoveHero, MoveCharactersClient, mounts/pets/points/planes, then MoveEffects
and MoveParticles. MoveCharactersClient traverses multiple characters; a
shared model's last writer is not necessarily the emitter owner. Therefore
reading native model BodyScale at effect update is not sufficient proof that
it is the owner's scale. Render-side writes in ZzzCharacter.cpp add another
possible producer; do not use temporal proximity alone as identity proof.

S21 emitter14C18A5..14C1916 obtains model by owner.Type and calls bone
transform, with no branch-local BodyScale assignment. EF2A81 is a model
manager lookup/cache/lazy-load function: existing entry returns pointer at
EF2BE7..EF2BEF; it has no owner argument and does not establish owner scale.
The previous unresolved BodyScale producer is still genuinely unresolved,
not implicitly solved by substituting native o->Scale. Next trace S21 model
scale setup before effect pass, and native per-owner pose sampling/update.

Native BMD::TransformByObjectBone (ZzzBMD.cpp) already implements the
S21 helper132EC63 contract: index check against NumBones, owner pointer,
owner BoneTransform or global BoneTransform fallback, translation column
when relative position is null, multiply by model BodyScale, then owner
Position. S21 fallback132ECE0 uses the global matrix array; null-relative
branch132ECF2..132ED44 extracts translation,132ED58..132EDB0 scales,
then adds owner position. Reuse the native helper, not a duplicate transform.

Emitter integration must check NumBones>0 and <=native MAX_BONES (200)
before selecting R%NumBones; no modulo-zero or uninitialized output. When
owner matrices are absent, the global fallback must be proven to belong to
this owner's current pose before using it. Model BodyScale provenance at
emitter update remains OPEN; do not silently substitute owner.Scale or
overwrite shared model scale. Existing DK equipment/skeleton is temporary
QA, not proof of final Grow Lancer bone/equipment parity.

Native particle lifecycle search found DeleteParticle and map reset only
clear Live (DeleteParticle also clears Target), not reallocate records.
CreateParticleInternal remains the observed allocation route and resets
Breche mode/remainder for every successful reuse. This read-only source
audit is not a live exhaustion/reallocation regression test.

### Final sprite submission follow-up

The RISE adapter now keeps the 7EF7 flare on a private per-slot marker because
the pinned S21 `Effect/flare01.OZJ` is byte-identical to native SS6
`BITMAP_LIGHT`. `CreateBrecheSprite` captures the free slot before calling the
shared allocator, marks only an unambiguous successful allocation, and leaves
native subtype0/additive blending intact. Allocation reuse, render-pass
retirement and the low-Z pass clear the marker. `RenderSprite` applies the
recovered S21 1727079 envelope (`Visible ? +.1 : -.1`, clamped to `.2..1`)
only for that marker; ordinary SS6 sprites still use their existing path.

`RenderBrecheFireSprite` flushes the native sprite queue before changing the
bound texture's wrap mode, uses the verified S21 `GL_LINEAR/GL_CLAMP` contract,
submits the normal full-UV native billboard, then flushes and restores the
previous sampler state. This closes the scoped sampler/state boundary without
mutating the shared `BITMAP_LIGHT` registration. The adapter is source/build
verified; pass frequency, GPU visibility and owner pixel parity remain runtime
acceptance gates.

### S21 flare-render function boundary pinned

The preserved image dump now has a byte-pinned verifier for renderer
`1727079`. It reads the sprite visibility byte at `+0x0B`, advances the
visibility envelope by `+0.1` when visible and `-0.1` otherwise, and clamps it
to `[0.2, 1.0]` using the dump's `.1`, `.2` and `1.0` constants. The resulting
envelope is multiplied by the stored sprite scale before the texture's native
width/height fields are read. The renderer has a separate `0x7FEA`
FormationMark UV jump table; Breche's proven `0x7EF7` `flare01.jpg` does not
enter that branch. Its A13A00 `1x1` seed therefore follows the default full-UV
path and reaches the native `18E8B0C` billboard primitive with the stored axis
mask. This closes the dump-side flare branch and keeps the RISE adapter on the
native full-UV route; it does not promote owner pixels, GPU visibility or pool
retirement to PASS.

### S21 loader metadata and sampler follow-up

Registration18BD7AC/18BD7CC/18BD50C passes respectively8084/8085/806E,
filter2601 (LINEAR), wrap2900 (CLAMP), trailing1,0 to18F8FA2.
18F905A forwards ID/path/filter/wrap to managerCC461D. Extension selector
CC4779 compares string1B70218="jpg" (direct dump string read) and selects
CC52F7 atCC4793; second extension1B7021C is"tga", not this path.
Anchored JPEG loaderCC52F7 writes metadata+214=3 atCC562D, dimensions
atCC5609/CC561F. Thus the source loader contract selects component3
additive and glColor3fv, not inferred from the filename alone. This is
static successful-load evidence, not a fresh live texture inspection.

Important sampler difference: CC5907..CC594D sets both MIN/MAG filter
from argument+10 and WRAP_S/T from+14, so registration2900 reaches
GL unchanged. Native ZzzOpenData.cpp registers these shared firehik textures
with GL_CLAMP_TO_EDGE, not GL_CLAMP. Hash identity does not close sampler
parity. Keep this difference OPEN for scoped Breche draw handling and QA;
do not alter the shared texture globally. A scoped sampler change must
flush queued native sprite draws and restore state; a texture alias would
instead need explicit hash-pinned registration/provenance. Select only after
checking actual native shader/sampler facilities and boundary behavior.

960443..96046E explicitly initializes all angle components to0. Therefore
mask4 selects pure Z rotation, closing the zero-X/Y assumption above.
18E900C..18E90CB fills UV corners3=(u,v),2=(u+width,v),
1=(u+width,v+height),0=(u,v+height) via18EDC3F, matching native TEXCOORD.
18E90CE begins GL_QUADS; component3 at18E90EB calls glColor3fv at18E90F3;
18E91CF..18E922F submits four UV/vertex pairs then glEnd. No hidden
Alpha multiplication or second texture submission occurs in this path.
Pinned import table confirms the referenced GL calls (1B4C754 glBegin,
1B4C774 glColor3fv,1B4C778 glTexCoord2f,1B4C77C glVertex3fv,
1B4C764 glEnd). Dimension factor1B4DF14 is .5.

S21 additive state18E7137..18E71C2 matches native EnableAlphaBlend:
blend ONE/ONE,18E6E2C disables CULL_FACE(0B44),18E6E6F sets
glDepthMask(false), disables ALPHA_TEST(0BC0), enables TEXTURE_2D(0DE1)
and disables FOG(0B60) when active. No depth-test enable/disable is added.
Native JPEG loader GlobalBitmap.cpp sets Components=3 explicitly; native
RGB asset selection is thus supported by loader source, not filename alone.
S21 loaded texture metadata still requires lookup/loader verification;
the primitive's component3 behavior is now closed statically. Runtime
camera, shader queue, texture load and visual acceptance remain separate.

Render audit follow-up: S21 particle renderer1620984 gates Live at1620A3E,
then pass1 rejects Position.z>350 (constant1B4EAE0) and pass2 rejects
z<=300 (1B4E4E8), verified by direct float reads. Native RenderParticles
uses the same gates. Both derive width/height from loaded texture dimensions
times Scale, then select additive ONE/ONE when Components==3. Native
EnableAlphaBlend explicitly disables culling, depth writes, alpha test and
fog, and enables texture2D. Full state equivalence still needs S21 helper
comparison, not just the blend factors.

Native FIRE_CURSEDLICH/4 submits stored Light and Rotation directly;
FIRE_HIK1/0 and FIRE_HIK3/0 have no dedicated render switch case and reach
the default RenderSprite with the same arguments. Default UV is0,0,1,1.
No second multiplication by particle Alpha occurs, matching selected S21
render branches. Native RenderSprite transforms Position by CameraMatrix,
halves dimensions and rotates around camera Z; component3 uses glColor3fv.
Its optional shader path queues the same geometry/RGB with alpha1.
S21 axis-mask4 full geometry/UV and loaded component metadata are now pinned
by the selected-branch verifier below; exact GPU state and pixel equivalence
remain runtime questions.
Further anchored18E8D4B..18E9006: nonzero rotation creates corners with
local Z=0; mask tests1/2/4 at18E8E9F/18E8EBD/18E8EDB assign rotation
to angle X/Y/Z respectively. Mask4 selects Z only, then D30B1A matrix,
D3189D vector rotation and addition of all camera-space center XYZ.
Native uses local Z=centerZ before Z-only rotation and adds only centerXY;
for zero X/Y rotation these are mathematically equivalent. Constructor
960443 zero initialization and final UV/color submission remain to verify.
Do not expand native renderer for arbitrary S21 masks1/2: these particles
only submit mask4. This narrows the compatibility requirement to this path.
Native render tail clamps negative LifeTime to0; S21 selected common tail
1640E76 releases handle and advances. For private integer ticks, life cannot
become negative from the current selected updates, so this clamp is inactive.

The byte-pinned `verify_breche_particle_render_contract.py` now closes the
selected static renderer boundary: S21 `1620984` gates Live and the two
terrain-height passes, derives dimensions from the loaded texture and Scale,
and dispatches `8084/0`, `806E/4` and `8085/0` to the same native billboard
primitive `18E8B0C`. RISE's private Breche marker reaches
`RenderBrecheFireSprite` after the same width/height computation, while the
ordinary SS6 switch remains untouched. This is static branch equivalence only;
GPU texture visibility, render-pass retirement, pool reuse and matching owner
pixels remain runtime gates.

Read actual native PARTICLE in Main5.2_RISE/_struct.h: named fields map
S21 savedRGB+7C ->TurningForce, upward increment+6C ->Gravity (despite
the name, selected branches add it to Z), counter+48 ->LifeTime,
angleZ+60 ->Rotation, owner+50 ->Target, vector+70 ->Velocity.
No binary offsets or S21 record layout are needed in the adapter.

|S21 particle|Existing native registration / subtype|Native source asset|
|---|---|---|
|8084/0|BITMAP_FIRE_HIK1 /0|Client/Data/Effect/firehik01.OZJ|
|806E/4|BITMAP_FIRE_CURSEDLICH /4|Client/Data/Effect/firehik02.OZJ|
|8085/0|BITMAP_FIRE_HIK3 /0|Client/Data/Effect/firehik03.OZJ|

All three native asset SHA256 values match the pinned source table exactly
(A322...C229,F054...8C85,5A82...CE7 respectively). ZzzOpenData.cpp loads
these filenames into the stated native enums. CreateParticle already has
the matching selected random lifetime/scale/rotation/Gravity, Alpha0,
TurningForce=Light and Light0 constructors. Reuse them; do not introduce
three duplicate texture loaders or IDs. This does not establish final
native render blend/UV parity, still to verify independently.

CreateParticle scans native inactive slots, initializes fields, returns
index i on success but also returns false/0 on failure. A wrapper must
NOT infer success from a non-null/int result, nor tag slot0 after failure.
Private per-slot tick metadata should be assigned inside the successful
allocation branch, with an explicit private creation mode (or equivalent
unambiguous internal result) and reset on every reuse including SS6 reuse.
No global change to existing CreateParticle return convention is authorized
by this fix. No private metadata integration has been claimed yet.

Registry is resource_registry.json textureRecords from pinned mapped S21 image.
No assets staged by this audit. Each actual OZJ hash was read from source.

| Type | Registry path | Registration VA | Actual source under D:\MU FICA Season 21\Data | SHA256 |
|---|---|---|---|---|
|0x81EC|Effect/lightmarks_red.jpg|0x18BEFD8|Effect/lightmarks_red.OZJ|E447C3F15664B277A2278360317C08C86CCF8921DA1746FE0084E012A5D7608E|
|0x809F|Skill/twlighthik02.jpg|0x18BD42C|Skill/twlighthik02.OZJ|9872B87E5A87A965B61A3865F217A5973C39FB58D54E12A7858F560FD9C04B69|
|0x809E|Skill/twlighthik01.jpg|0x18BD40C|Skill/twlighthik01.OZJ|6BE0AE4DC839CD40C6B63AA1CC74847E33326CE441194714440EA93FB1DC7A8E|
|0x806E|Effect/firehik02.jpg|0x18BD50C|Effect/firehik02.OZJ|F05456CD63D8DAD6DE3BE8666FE0DA97333D5B092A1CD456AC01434FB8478C85|

## Constructor findings

## 0x81EC subtype0 render path

Handler0x15AEC85 checks subtype0 and enters0x15AECA2. It enables additive
ONE/ONE blend via0x18E7137, computes a temporary RGB=stored Light*Alpha,
then calls0x1765DF1 at0x15AEDF2 with:
texture=effect+0x5C, X/Y=Position.x/y, SizeX/SizeY=Scale, Light=temporaryRGB,
rotation=Angle.z, alpha argument1, height5, extra arguments1 and0.
It restores blend via0x18E6EFF and exits this branch at0x15AEE00.
This is a terrain-aligned bitmap path, NOT a camera-facing sprite.

Anchored0x1765DF1 computes world-to-terrain X/Y, rotates UVs and loops tiles,
then calls tile renderer0x1769C7F at0x176642E. Its extra true argument reaches
tile+0x28; branch0x1769D50..0x1769EDE reads per-corner height table and adds
height argument5. Native RISE RenderTerrainAlphaBitmap (ZzzLodTerrain.cpp1917)
already has corresponding X/Y/scale/light/rotation/alpha/height parameters,
and RenderTerrainBitmapTile1840 uses terrain corner heights. Extra S21 final
argument0 and full tile branch behavior still need closure; do not blindly
drop arguments when claiming parity. S21 also calls tile culling0x176A366,
which is absent at this point in the inspected RISE wrapper.

This finding supports mapping to native terrain rendering, not introducing a
new renderer. Actual invocation is still gated on complete effect update and
remaining child contracts; no rendered outcome has been verified.

### Extra terrain parameters resolved

Full tile function0x1769C7F..0x176A365 consumes its final flag at0x176A2D7:
true computes V=1-inputV, false copies inputV. Breche passes false, so the
ordinary RISE glTexCoord2f(c[i][0],c[i][1]) matches; no UV-flip adapter needed.
The preceding true flag chooses terrain corner height plus5, matching RISE's
BackTerrainHeight+Height path. Do not confuse either with the separate
LightEnable=false argument passed by the terrain-alpha wrapper.

The earlier "tile culling" helper0x176A366 is NOT a camera/frustum check:
it truncates X/Y, forms index=(Y<<8)+X through0x176BA2E, rejects index<0 or
>=65536, then rejects terrain attribute0x200. When scene world id=141 it
also rejects attribute8 or mapping-layer byte14. Symbolic world and table
semantics require corroboration; this is not authorization to map S21 world141
to a guessed SS6 world. Native RISE tile renderer instead explicitly bounds
X/Y individually and does not test0x200 at this site. Therefore direct API
reuse needs a scoped eligibility filter if that terrain behavior is required.
Do not alter global SS6 terrain rendering. Native _define.h calls bit0x200
TW_ATT1, but equal numeric bits alone do not prove equal map semantics.

### Common initialization and final subtype values (rechecked)

### 0x809F/sub0 and 0x809E/sub14 terrain render pass

Anchored function0x1596B00 traverses effect pools, gates Live and byte+B
at0x1596BE0..0x1596C09, then reads Type(+5C) into local-154 at0x1596C53.
This is a different pass from0x15A0C98. In that latter pass809E/809F
fall through without model rendering;806E explicitly skips via0x15A27CF.
That absence must not be interpreted as no visible effect.

Within0x1596B00, ordered comparisons route809E to0x159D2BE at0x1596E4F
and809F to0x159DFE6 at0x1596E5F.809F bounds subtype<=9; pointer table
0x15A0C70 entry0 is0x159E00F.809E skips8012 special handling and its
subtypes4/5, then subtype14 selects0x159D64C via0x159D636.

Both selected branches enable additive blending via0x18E7137, form
temporaryRGB=Light*Alpha and call terrain bitmap0x1765DF1:

| Child | Call | Texture | XY | Size XY | Rotation | Alpha argument | Height | Terrain heights | Flip V |
|---|---|---|---|---|---|---|---|---|---|
|809F/0|159E165|Type|Position.xy|Scale,Scale|Angle.z|stored Alpha|5|true|true|
|809E/14 (twice)|159D7A2|Type|Position.xy|Scale,Scale|Angle.z|stored Alpha|5|true|true|

Both restore blend via0x18E6EFF and leave their selected branches
(159E173->159E874;159D7B0->159DFE1). No sprite creation in these branches.
Unlike81EC/sub0, these pass final flagtrue, which the previously traced
tile function converts to V=1-V. Native5.2's ordinary terrain UV path
therefore needs a private/scoped V-flip option for these two textures.
Do not globally invert terrain UV or assume the81EC flags are shared.
Also retain the distinction between temporaryRGB multiplied by Alpha and
the explicit Alpha argument: do not silently discard one or infer final
pixel opacity without examining the terrain primitive's color operation.
The byte+B visibility producer and render-pass ordering remain to verify.

#### Terrain color operation verified

The mapped PE import directory identifies IAT0x1B4C774 as glColor3fv and
0x1B4C758 as glColor4f (read using pefile, not guessed from argument count).
At0x1765E04..0x1765E20, alpha argument exactly1 selects glColor3fv(Light).
Otherwise0x1765E22..0x1765E68 passes Light.r/g/b unchanged and Alpha to
glColor4f. There is no extra RGB-by-alpha multiplication in this wrapper.
Thus the caller's Light*Alpha must be retained once; it is not multiplied
again simply because an explicit alpha argument is present. Final pixel
output still depends on texture and active blend state.

Native ZzzLodTerrain.cpp1917 has the same color branch. Its tile routine
only replaces color when LightEnable=true; this wrapper passes false.
The existing native path can therefore retain both arguments as-is.

An attempted explicit FlipV overload edit was rejected by apply_patch
because ZzzLodTerrain.h is not UTF-8. Neither terrain file was modified
(scoped git diff empty). Do not silently transcode these legacy files or
claim the overload exists. A scoped native adapter remains to implement;
UV behavior above is verified evidence, not a delivered runtime change.

#### Scoped native adapter implemented (follow-up)

RISE/GrowLancerTerrainAdapter.h now supplies RenderGrowLancerTerrainBitmap.
It delegates all geometry/color/bitmap work to RenderTerrainAlphaBitmap.
The optional V reflection postmultiplies the current GL texture matrix by
Translate(0,1,0)*Scale(1,-1,1), equivalent to passing(u,1-v) through the
caller's original texture matrix. A scoped guard saves/restores that matrix
and the previous matrix mode, without consuming a GL matrix-stack entry.
Opt-out calls native directly, without any GL state operations.
Native terrain source/header remain unchanged in their original encoding.

test_terrain_adapter.cpp compiled/run with MSVC x86: PASS for identity and
nonidentity matrices, UVs inside/outside[0,1], argument preservation, opt-out,
matrix/mode restoration, and exception restoration. This uses a small GL
matrix test double: it is not a real-context render or visual parity test.
Main.vcxproj ClCompile Global Release/Win32 with isolated QA flags also PASS;
GrowLancerEffectRuntime.cpp includes the adapter for real-header compilation.
No Breche caller activated yet and no executable linked/staged. Terrain
eligibility, actual GL-context behavior and remaining effect chain stay OPEN.

The generic constructor at0x143E7BA sets alpha1. At0x143E7E1..0x143E80D,
scale argument <=0 selects float0.9 (0x1B4EDA4); positive arguments are copied.
Consequently the two supplied zero scales in the parent call list DO NOT
produce zero-sized effects. For the currently traced finite arguments:

| Child | Final constructor scale | Alpha | Lifetime | Max lifetime |
|---|---|---|---|---|
|0x81EC /0|2|0|20|20|
|0x809F /0|2.5|.1|20|20|
|0x809E /14 first|.9|.1|20|20|
|0x809E /14 second|.2|.1|20|20|
|0x806E /15|.9|1|20|not written by subtype branch; common/reset state audit pending|

0x806E subtype15 follows the comparison chain from0x144280E to0x1442B1E;
0x1442B30 sets lifetime20 and jumps0x1442B64 ->common return0x14B2E0C.
Unlike subtype16, subtype15 does not copy lifetime into +0x70.
The subtype-specific branches for the other three children do not override
scale. Shared tails0x145C315 and0x145C5A5 both immediately jump to common
return, so the neighboring constructor starting0x145C5AA is unrelated.
No additional CreateEffect call exists in these selected constructor tails.

Renderer dispatch: type0x81EC compare0x15A16F4 ->0x15AEC85; selected
subtype0 primitive and terrain flags are traced in the sections above.

- Root tail0x1482B26 jumps to common return0x14B2E0C. No sixth child there.
- 0x81EC dispatch compare0x143F710 ->0x1482BFD; subtype0 ->0x1482C2C:
  lifetime/max20, alpha(+0xDC)=0. Tail0x1482D1B goes common return.
- 0x809F dispatch table0x14B3E54 index5 ->0x145C31A; subtype table
  0x14B3F88 index0 ->0x145C343: lifetime/max20, alpha=.1.
  Shared tail0x145C5A5 verified as common-return jump.
- 0x809E table index4 ->0x145BD2E. Subtype14 skips earlier0..13 tests,
  reaches0x145C199 then0x145C1A5: lifetime/max20, alpha=.1.
  Two records supplied scales0 and.2 are distinct calls; do not collapse.
  Shared tail0x145C315 verified as common-return jump.
- 0x806E compare0x143F427 ->0x144280E; subtype15 decoded above.

All above values are constructor writes, distinct from caller RGB/scale in
BRECHE_REVERSE.md. Update, render primitive, lifetime fade, any grandchildren,
movement and pool cleanup are still OPEN. Texture existence does not close them.

## Primary update branches recovered

### 0x806E subtype15: bone-driven emitter, not just one fire texture

Primary type comparison0x14B80DC selects0x14C029A. Following its ordered
subtype tests reaches0x14C1859; subtype15 matches0x14C185C and enters
0x14C1866. A null owner clears Live via0x13167BB and exits to common tail.
Otherwise loop counter0..1 emits twice per update (0x14C1882..0x14C1B08).
Each iteration looks up the OWNER model by owner.Type through968B22/EF2A81.
A missing model exits the loop. Positive model WORD+22 supplies the random
index bound; call132EC63(outPosition, owner, rand()%count, nullptr) obtains
the selected bone position. This is not a fixed hand/weapon bone.

In132EC63, null offset selects the bone matrix translation column at
132ECF2..132ED44.132ED58..132EE39 scales XYZ by model+74 and adds
owner.Position. Matrix comes from owner lookup1316442, with fallback table
70B5400; native mapping must use real owner model/bone transforms, never
copy the S21 fallback pointer. If count<=0, source skips position setup
but still reaches particle emission; do not reproduce an uninitialized
stack position in RISE. A native invalid-bone guard needs a documented
compatibility decision, not a guessed fallback origin.

For a valid bone, sprite call172760A at14C19A1 supplies texture7EF7,
position=bone world position, scale4 (constant1B4E8B0), RGB(1,.2,0),
owner=emitter.Owner, rotation0, trailing arguments0,4 and the result of
A13A00. The trailing metadata and sprite lifetime require allocator tracing.
Wrapper172760A can suppress allocation via its settings gate; a source
call does not prove an actual sprite allocation on every tick.

Then each iteration sets RGB(1,1,1), scale=(rand()%5+13)*.1 (1.3..1.7),
and selects rand()%3. The calls below use bone world position and emitter
Angle, with final two arguments0,0; they call particle wrapper1724176,
NOT CreateEffect. Its world/quality suppression also needs runtime QA.

| Choice | Call VA | Particle type | Subtype |
|---|---|---|---|
|0|14C1A80|8084|0|
|1|14C1AC0|806E|4|
|2|14C1B00|8085|0|

Branch tail14C1B0D ->14C1FCA ->1574472. There is no primary scale/alpha
update for the emitter itself. Its particle806E/sub4 must not be confused
with the pooled effect806E/sub15. Constructor20-tick life does not imply
only five static child nodes: these additional layers are emitted repeatedly.
The apparent renderer1627A2A remains only a lead until its pool is traced;
it may concern particles rather than the emitter record. Main effect render
pass15A0C98 explicitly skips806E at15A27CF.

Additional source textures verified read-only (not staged):

| ID | Registry path / registration | Source under S21 Data | SHA256 |
|---|---|---|---|
|7EF7|Effect/flare01.jpg /18BD1CC|Effect/flare01.OZJ|874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8|
|8084|Effect/firehik01.jpg /18BD7AC|Effect/firehik01.OZJ|A3224392CD5F3AEC808FCAA7C1EA43B2D4451EE8B184BD3B7A912D683812C229|
|8085|Effect/firehik03.jpg /18BD7CC|Effect/firehik03.OZJ|5A8267C2E7D90264E6ED614E1B721E592B2F6096AE3CD7165111849867ECECE7|

Next: sprite metadata/lifetime1726C50; particle constructors/update/render
for8084/0,806E/4,8085/0; native bone transform mapping and invalid-bone guard.

#### Flare sprite allocator and render lifetime

#### Fire particle constructor contracts (not effect-record fields)

Particle wrapper1724176 forwards to16412E5 via17241F2 or172421F,
subject to its quality/world gate. Generic initialization sets Type+0C,
original type+10, subtype+14, Position+20, copied Position+88, Light+38,
scale+18, angle+2C and default counter+48=2. These are PARTICLE offsets;
do not apply the effect record's+60 subtype/+6C lifetime interpretation.

Type dispatch1641751 bounds below8147; >8008 goes16419CF, subtracts8012,
bounds<=134 and uses byte table16A9789/pointer table16A96A1:
8084 selector30->16673C6,806E selector20->164B3FF,8085 selector31->166C453.
8084 has another subtype table16A9C9D; entry0 is1667599, NOT the adjacent
16673F4 branch.806E subtype4 follows tests0/1/2/3 to164BBFF;8085 subtype0
selects166C479. These bounds/entries are covered by the focused verifier.

For each independent random draw R (nonnegative RNG output), selected
constructor writes are:

| Particle/subtype | Counter+48 | Scale+18 | Rotation+60 | Parameter+6C |
|---|---|---|---|---|
|8084/0|R%5+27|(R%72+72)*.01*suppliedScale|R%360|(R%24+64)*.1|
|806E/4|R%5+12|same|same|same|
|8085/0|R%5+17|same|same|same|

All three set+44=0, copy supplied white RGB to+7C, then zero live Light+38.
Do not render them white immediately or retain only the emitter's1.3..1.7
scale: the constructor multiplies that scale by0.72..1.43. Constant1B4DF00
is float32(.01),1B4DF04 is float32(.1). Meaning/use of+6C (6.4..8.7)
must be confirmed in update before calling it velocity or gravity.

Selected branch tails:166775C->166A5C6->16A9524;
164BD9F->164D1E4 (next shared-tail audit);
166C619->166F197->16A9524. No child allocations in the selected blocks.
Common constructor tail/commit, update, lifetime decrement ordering, render
and particle pool release remain OPEN; constructor ranges are not observed
on-screen durations. Next open164D1E4,16A9524 then particle update1620984.

#### Shared constructor tail and particle renderer follow-up

164D1E4 directly jumps16A9524. This common tail passes the constructed
particle handle to1725950, which forwards to1724501; it then releases
the local handle through1724E3B and returns. No additional field overrides
occur in16A9524..16A955E. Container insertion/refcount internals remain
to audit; releasing the temporary handle is not proof of destroying the
inserted particle.

CORRECTION to the next-action wording above:1620984 is the particle RENDER
traversal, not its movement/update function. It computes bitmap width/height
times particle.Scale at1620AFF..1620B32. Blend selection at1620B3A uses
texture metadata byte+214: value3 selects18E7137, otherwise18E709C(0).
Do not infer the loaded metadata solely from a filename extension.

Type dispatch1621102 selects8084->1635D2A. Subtype table16410E8 entry0
is1635D58, ending with18E8B0C call1635DF4.806E at162103C selects
1627A2A; subtype4 skips subtype21 special handling to1627B36, call1627BD2.
8085 at1621112 selects162471C; subtype0 skips24..29 special cases to
1624BE9, call1624C85.

All three selected render calls pass texture from particle+10,
Position+20, computed bitmap dimensions*Scale, stored Light+38,
rotation+60, UV(0,0,1,1), axis mask4. None multiplies Light by+44 in
the selected branch. In particular, do not borrow the default8084 branch
1635E01's Light*+44 behavior for subtype0. All use native-like billboard
primitive18E8B0C, not terrain bitmap. Full blend/visibility context and
axis-mask primitive audit are still required for parity.

Next movement-function candidate:16AA2ED (nearest prologue preceding the
previously audited Wrath update17095FD). Anchor its dispatch before tracing
these three types; this address is a lead, not a verified handler mapping.

#### Particle update dispatch and8085/sub0 behavior

Anchored16AA2ED reads particle Type+0C into local-686C at16AA7D2..16AA7E0.
Types<8147 and>8012 go16AAA64: subtract8013,bounds<=133, byte table
17236F5 and pointers1723611. Entries:8084 selector29->16DA1D5;
806E selector19->16B8BDD;8085 selector30->16DF6D7.8084 subtype table
1723C01 entry0 is16DA4CF, NOT its first adjacent block16DA203.

8085/sub0 enters16DF6FD (shared with6, but skips its final extra motion).
Let L=counter+48,A=field+44,S=scale+18,C=savedRGB+7C,V=field+6C:

```text
if L < 10: A -= .2
else if A < 1: A += (rand()%2 + 2)*.1
else: A = 1
if A < .1: Live = false
Light = C*A
if S > 0: S -= (rand()%3 + 7)*.01
else: Live = false
Position.z += V
Rotation += 3
```

The upper alpha test is before increment: a step may exceed1, then clamp
on the next eligible update. The scale death test is before subtraction,
not after. Do not replace these with post-update clamps or return early
after Live is cleared: the source still writes Light, position and rotation.
Constants0x1B4DD3C=0,0x1B4E860=3; byte+8 is particle Live.
Subtype6-only additional rotation reset/motion at16DF9D6 is excluded.
Subtype0 exits16DFA32->16E37C5; shared tail and decrement/release ordering
remain open. This proves+6C is upward Z increment for this subtype, not
gravity acceleration. No new particles emitted by this selected branch.

Next exact handlers:8084/sub0 at16DA4CF;806E/sub4 within16B8BDD;
shared update tail and container release. No runtime activation yet.

Allocator1726C50 scans1000 sprite records, marks a free record Live, writes
Type, subtype from arg+20 (0 here), Owner, scale4, rotation0, RGB and copies
bone position into both Position and StartPosition. It sets field+D0=1,
but does NOT set an effect-style life counter. Failure returns0, which is
also the first valid index; native success checks must not treat0 as proof
of failure without inspecting the pool/API contract.

Emitter's trailing4 is stored at+8C, not subtype or lifetime. Its final
A13A00 object initializes two integers1,1 and float1; allocator converts
these to+90=1.f,+94=1.f,+98=1. Renderer1727079 sees1x1 and chooses
the full UV rectangle(0,0,1,1), bypassing animated-sheet indexing.
At1727565 it calls18E8B0C with those UVs and the+8C flag4. The flag is
an axis mask used by the rotated path (18E8E9F..18E8EF9 tests1/2/4),
not a lifetime. Rotation here is0, but the full primitive still needs audit
before dropping any S21-only argument in a native adapter.

Renderer1727079 first adjusts+D0 according to byte+B: visible adds.1
and caps1; nonvisible subtracts.1 and floors.2. It scales bitmap dimensions
by D0*storedScale, retaining RGB. Native zzzeffectsprite.cpp RenderSprite
has similar state but its nonvisible branch currently ADDS .1 instead of
subtracting, and uses FPS_ANIMATION_FACTOR. This is a scoped compatibility
difference; do not globally change all existing SS6 sprites to fix Breche.

Pool render1726E56 selects additive blend for subtype0, invokes1727079,
then clears Live at1726F87 for render argument0 or2. Argument1 retains
the record for another pass, with height filtering. Thus these sprites
have render-pass ownership, not emitter lifetime20. Native RenderSprites
already has corresponding Live-clearing branches; caller pass sequence
and visibility producer still need verification. The emitter must recreate
its flares at the proven emission cadence, not retain a single flare until
the root expires or delete the actor owner during sprite cleanup.

These are branch-local contracts, not a claim that the common update tail,
secondary dispatcher, lifetime decrement order or destruction is closed.

### 0x81EC subtype0

The primary MoveEffect function starts at0x14B76C0. Type(+0x5C) is copied
to local-0x12474 at0x14B780C..0x14B7812. The ordered comparison chain
reaches0x14B8398; compare0x14B83E4 selects0x15387E8 for0x81EC.
Subtype0 selects0x153881C. With L=remaining lifetime(+0x6C),
M=max lifetime(+0x70), A=alpha(+0xDC), its exact finite-value operation is:

```text
if float(L) > float(M)*0.5:
    A += 1.0/(float(M)*0.5)
else:
    A -= 1.0/(float(M)*0.5)
```

Constants read directly:0x1B4DF14=0.5,0x1B4DD48=1.0.
For the constructor's M=20, the per-update increment/decrement is0.1.
There is no clamp, scale, position or angle write in this selected branch.
Do not borrow scale growth from subtype2 at0x1538A65 or alpha clamp from
subtype1 at0x153899F. Exit0x15388A3 ->0x1538A83 ->0x1574472.
The common tail has additional conditions, so fixed world position and the
exact first/last rendered alpha still require its audit and tick ordering.

### 0x809F subtype0

Primary type comparison0x14B8248 targets0x14FC714. This subtype dispatcher
bounds subtype<=9 then uses table0x15955F8. Read-back of entry0 gives
0x14FC73A, not the neighboring subtype1 code0x14FC803.

At0x14FC73D the WORD at effect+0x3C is compared to0x117:
equal adds30 to Angle.z; otherwise adds15. The vector accessor0xD2DFCE
returns this+4*index (here index2, vector starts+0x164).
Constants0x1B4E4D8=30 and0x1B4F6B0=15 are read from the pinned dump.
Then0x14FC7AE..0x14FC7FE adds0.1 alpha when L>=11 and subtracts0.1
when L<=10; no clamp or scale write occurs in this selected branch.
Tail0x14FCE05 jumps0x1574472. Alpha starts0.1, not81EC's0.

Child metadata propagation is now resolved for this constructor call:
at0x148264D..0x14826A7 the wrapper's argument at[ebp+0x24] is explicitly0
(push0 at0x1482665), not the parent's skill. Wrapper0x143E592..0x143E596
copies that WORD into temporary metadata+2. The inner constructor reads
metadata+2 at0x143E6BD..0x143E6C4, then writes effect+0x3C at0x143E8F4..
0x143E901. The selected809F/sub0 constructor has no later override of it.
Therefore this Breche child selects the **+15 Angle.z** branch, despite
the receive skill being279. Do not propagate parent's279 into this field:
that would double its rotation speed. Common later updates remain unaudited.

### 0x809E subtype14 (both child records)

Primary handler0x14FB765 is an ordered subtype comparison chain. Skipping
0..11 and13 reaches0x14FC3CA; compare0x14FC3CD selects14 at0x14FC3D7.
Through0x14FC477 it performs, in order: Angle.z +=15, Scale +=0.1,
Alpha +=0.1 when remainingLife>=11, otherwise Alpha -=0.1 at life<=10.
There is no local clamp, target-follow operation or child emission.
Constants are0x1B4F6B0=15 and0x1B4DF04=float32(0.1). Both constructor
records follow this same branch, starting at scales0.9 and0.2 respectively.
Exit0x14FC477 ->0x14FC70F ->0x1574472. Do not copy subtype13's
skill-conditional scale growth or subtype0's scale shrink into subtype14.

### Common secondary dispatch, movement and expiration

For child types81EC,809F,809E, the common tail's WORD+3C special handling
at0x1574472 applies only to0x7E7/0x7FC/0x7FD, not their supplied zero.
The type skip list0x15745E0..0x15747A0 excludes none of these three.
Their types are outside model-animation range[0x190,0x7A6), so they skip
0x15747C7..0x15748A5, and outside[0x7F4E,0x7F50].
Secondary dispatcher0x15748C1 takes the >0x64B, >0x698, >0x6DA,
>0x8003, <=0x8288 route to0x1574C6A. None matches its comparisons
810B/81E3/8276/8285/8286/8287/8288; default0x1574CE6 ->0x15945E5.
Thus there is no second type-specific handler for these three child types.

The default calls0x1595A11(effect,1), NOT an empty tail. This helper reads
Angle, builds a matrix through0xD30B1A, transforms vector+0xF8 through
0xD3189D and adds its result to Position. True-path0x1595A2C..0x1595B66
then returns via0x1595C43..0x1595C4F (security cookie check only).
Reset0x1315E97 calls0xD2E65E for vector+0xF8 at0x131615C..0x1316165;
that helper explicitly zeros all three float components. The generic
constructor at0x143EB14..0x143EB56 also zeroes all three components after
the record is selected. Auditing the selected Breche entries shows no later
write to this vector: 0x809F/sub0, 0x809E/sub14 (both records), 0x81EC/sub0
and 0x806E/sub15 only write their type-specific life/scale/alpha/angle
fields. Therefore their common secondary movement receives a zero vector,
so they remain at the copied target position; no caster/target movement is
being invented in the 5.2 adapter.

After common movement, these types skip the413/7EF7 special decrement and
recursion cases.0x1594624 tests life<=0 and calls destructor0x14B6820 at
0x1594630 before the ordinary decrement at0x15946E3. Therefore the primary
update also executes at life0; lifetime20 is not permission to run only20
updates or to decrement before the primary branch. Rendering cadence is
separate and still needs runtime verification.

Destructor0x14B6820 optionally frees bone storage when byte+D==1, then
its type switch has no custom cleanup for these three types (or806E/5FD).
They reach0x14B691B ->reset0x1315E97. Reset clears Live at0x1315EAB.
This supports native effect-pool reset ownership, not deleting the actor
owner or recursively destroying arbitrary effects with the same owner.
Full root/grandchild behavior remains unclosed.

### 0x806E subtype15 emitter producer boundary pinned (2026-09-14)

The hash-pinned verifier `tools/grow_lancer/verify_breche_emitter_dump_contract.py`
now checks the exact primary producer at `0x14C1859..0x14C1B0D`.  The selected
subtype is15; a null `+0x34C` owner clears Live and exits.  With a live owner,
the bounded loop runs exactly twice, obtains the owner model by `owner.Type`,
samples a random valid bone through `0x132EC63`, and submits the `0x7EF7`
flare through `0x172760A`.  Its random choice submits only the native particle
wrapper triplet `8084/0`, `806E/4`, or `8085/0` through `0x1724176`.

This proves that the emitter has no fourth direct model/terrain layer.  The
RISE adapter keeps `kBrecheEmitterModel` lifecycle-only and delegates the same
flare/particle families to the native sprite/particle pools; it does not copy
S21 addresses or object layouts.  The verifier passed against the preserved
S21 dump and the current source.  Generic max-life reset, allocator pressure,
owner-bone runtime pixels and cleanup stress remain open.

Next exact actions: 809E/809F render branches; generic `806E/sub15` reset and
pool-retirement boundary; root5FD/sub1 primary/model-animation/render paths.
The former secondary
handler lead0x157F0A5 is EXCLUDED: its apparent table entry was outside
the dispatch bounds. Actual5FD secondary dispatch selects generic movement
0x15945E5; see BRECHE_REVERSE.md correction and focused table verifier.
Keep private RISE resources and native APIs; S21 addresses/layouts are
reverse evidence only, never runtime port constants.

### Breche private particle render retirement correction (2026-09-14)

The RISE `RenderParticles` branch for the three S21 Breche particle families
now renders the marked slot exactly once and then falls through to the common
post-render lifetime clamp.  The former `continue` skipped that shared tail
and made the private route structurally different from ordinary SS6 particle
records.  The ordinary type switch remains inside the `else` branch, so a
Breche slot is not rendered a second time by the legacy switch.  This is a
source/static correction only; allocator reuse, GPU pixels and owner-target
visual parity still require isolated runtime evidence.

`verify_breche_particle_render_contract.py` passes against the pinned S21 dump
and current source.  It explicitly rejects a private-route `continue`, checks
the native full-UV sprite adapter, and confirms the SS6 switch is kept in the
non-Breche branch.

### S21 pool-retirement boundary — 2026-09-14

`tools/grow_lancer/verify_breche_pool_retirement_dump.py` now pins the shared
S21 effect destructor and common tail.  Unlisted model records, including the
Breche controller `0x5FD` and emitter `0x806E`, take the default reset call
`0x1315E97`; only the explicitly listed legacy types receive type-specific
cleanup.  The common tail moves first, calls the destructor when lifetime is
exhausted, and special-decrements only `0x413`/`0x7EF7`.  RISE therefore keeps
Breche model-effect reset separate from its fire `PARTICLE` pool and does not
send particle records through `EffectDestructor`.  Allocator reuse/exhaustion,
GPU state restoration and owner-target pixels remain runtime-open.

### Owner controller subtype 0 and owner-only children (2026-09-15)

The fresh S21 cast and the per-character branch prove that `0x5FD/subtype0`
is an owner-side controller.  It is allocated at `0x142B21D` from the
character update branch for skill `0x117`; the target receive path is the
separate `0x5FD/subtype1` allocation at `0x12CB7F5`.  The owner controller has
`Life/MaxLife=20` and does not preallocate the five receive children.

Its primary updater is `0x1535478` (selected by `0x153542A`).  Each primary
tick, with a valid owner, it submits the following native families through the
ordinary pools: three random owner particle-wrapper records (`0x8084/sub0`,
`0x806E/sub4`, `0x8085/sub0`), ten `0x8073` pin-light joints using the extended
CreateJoint call (subtype 5, target null, scale 6.0, color `1,0.7,0.15`), and
one `0xAD9/sub6` wind model record with scale 3.0 and skill metadata 279.
Owner-local random positions are copied from the controller transform; no
target position is used by this path.  If the owner is null, the controller
clears Live and exits.

The exact timed child submissions are:

| remaining life | S21 type/subtype | port role | exact scalar/light evidence |
|---|---|---|---|
| 17 | `0x80BC/sub1` | owner ring-of-gradation | scale 5.5, red light `1,0.1,0.1` |
| 17 | `0x81EC/sub0` | owner lightmarks | scale 6.0, red light `1,0.1,0.1` |
| 7 | `0x81EB/sub0` | owner fire ring | scale 6.5, white light |
| 15 | `0x809F/sub2` | owner twilight-02 | scale 3.0, skill metadata 279 |
| 15 | `0x809E/sub13` (twice) | owner twilight-01 | creation scale 0, skill metadata 279 |

The owner-specific update branches are distinct from the target branches:
`0x809F/sub2` subtracts `0.1` from scale and adds 15 degrees to `Angle.z`
per tick (`0x14FB821..0x14FB87A`); `0x809E/sub13` adds 15 degrees and, when
skill metadata is 279, adds `0.5` to scale (`0x14FC2EA..0x14FC3C5`) while
applying the same life-dependent alpha envelope.  The target `0x809E/sub14`
branch remains a separate +0.1-scale path.  The common movement vector for
these records is zero, so the copied owner-local/ground position is not
silently advanced toward a target.

The isolated adapter mirrors these roles with private owner markers and native
RISE particle/joint/effect pools.  GPU owner pixels, exact random distribution,
blend/UV flags and pool-reuse stress are still runtime-open; no target root is
replayed on the caster.

### Owner position rewrite and timed-layer split (2026-09-15)

The owner updater's initial vector/matrix sequence is now decoded without
guessing object offsets: `D3189D(input=(0,-200,0), matrix=D30B1A(owner
controller Angle), output=controller Position)` followed by component-wise
addition of the allocator's owner snapshot `+0x1D0`.  The 5.2 adapter maps this
to `owner.Position + VectorRotate((0,-200,0), effect.Angle)` in the private
controller path.  Particle wrappers run first, before the rewrite; pin lights,
ring-of-gradation and lightmarks consume the rewritten controller position.

The life-7 `81EB/sub0` and life-15 `809F/sub2` plus both `809E/sub13` calls
pass `+0x1D0` rather than `+0x158`, so their 5.2 positions are taken from the
live owner snapshot.  This distinction is now guarded by
`verify_breche_owner_adapter.py`; it prevents the target-contact position or
the owner-local offset from being accidentally reused for the timed layers.
The S21 wind call's stack-local position is initialized only on the life-7
branch, therefore no non-life-7 coordinate is promoted to a claimed contract.
