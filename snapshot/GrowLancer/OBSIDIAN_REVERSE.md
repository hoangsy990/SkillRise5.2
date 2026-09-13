# Obsidian (skill 273) Season 21 client reverse

Status: client visual chain is statically verified; server party-buff selection
and SS6 compatibility/runtime QA remain open.

## Identity and dispatch

- Base skill `273` is Grow Lancer `Obsidian`: level 74, mana 50, AG 50,
  required energy 200, buff index 217 and party-buff behavior.
- The local branch starts at `0x10E455F`, sends the cast packet, sets action
  `191` with action argument `317`, and plays sound `1112`.
- The received dispatcher jump-table slot 23 enters `0x12CB59E`. For a remote
  caster it also sets action 191/argument 317, then creates both root models
  `0x5E3` and `0x5E4` subtype 0 using caster position, angle, light and owner.
- Sound 1112 registration: `0x18E2D8E` -> `Data\Sound\Obsidian.wav`.

### Alternate action selection (offline audit)

Local call10E46D5..10E46F0 pushes317,191,target,caster into1327DE8.
The complete helper1327DE8..1327E56 selects its fourth argument317 only
when predicate10C352E(caster), using manager returned by9C54AA(), returns nonzero,
caster byte+31E8 is zero and fourth argument is not-1. Otherwise it selects
third argument191. Both branches pass caster+3254 and selected action to
1327D72. Thus317 is a conditional alternate action, not an unconditional
second animation. Predicate semantics, skeleton clip and native equivalent
remain unverified; these dump offsets must not be transplanted into RISE.

Further stack audit:9C54AA..9C5536 is a no-argument singleton accessor
returning[1E04D80]; the pushed caster survives that call and is consumed by
10C352E's ret4. Do not misidentify it as9C54AA(caster).
10C352E rejects byte+31E8 nonzero, then requires10C34ED(caster).
It searches manager+4 by signed caster word+3E; when a record is found it
returns record byte+34 through10C64EC. Otherwise it compares catalog byte+251
against64 for the ID from caster word+322E (getter1314FAE).
10C34ED accepts either registered entry via10C2F89 or141BA4B, whose loop
tests three caster dwords+3238/+323C/+3240 against the same catalog category64.
Names/meaning of these state fields and category64 are not yet established.

Read-only parsing of original Player.bmd v0F confirms60 bones/410 actions.
Action191 and317 both have10 keys and unlocked root positions, but DIFFERENT
complete per-action hashes (header plus all bone position/rotation keys):

- 191:7322FFFFBE60A7A6F85D8CA2521E440B40A8C0896D57236BAF2EB34144C5F79D
- 317:9641BE400ABD1039026335A2AED8B9CB96C0E27325B89138FE7E6C72779CC5F7

verify_obsidian_action_selection.py checks pinned binary branch instructions
and both original clip hashes. PASS is not a native state/animation mapping.
Clip317 has not been appended to the isolated player or enabled speculatively.

### Category64 catalog identity

Original Data/Local/item.bmd SHA256
9B9D62B7F3BA085D31B4AEFBCC9DE5DB505C48D45C336A18CAD424C93D3B02F6
passes the existing S21 checksum and708-byte per-record XOR decoder.
Loader D052FA decrypts each record; D05301 allocates708 bytes, D05367
copies the decoded record, and D0537B registers it via D07635. Catalog
lookup D06184 returns the stored record pointer, and D064E6 reads+251.
Exactly16 records have byte251=64: groups13 and16, each with indices
231,232,326,327,328,329,330,331. Their model/name pairs are Tibetton
(wolfpet), Repen (rippenpet), Pawn (ponpet), Grippus/Griffs (griffspet),
including evolved variants. The group13 aliases must not be silently merged
with group16 identities.

Read-only supplied GS Data/Items/ItemList.xml identifies the matching entries
as KindB64; Data/MuunSystem/MuunInfo.xml identifies matching Muun records.
File hashes respectively:
61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78,
ED1FE59C2DDD2C09E584DBCB941D868AA2B76B11EC2BF2384B5540288DF9FF69.
This establishes an item/Muun relationship, NOT sufficient proof of a riding
state, nor permission to choose317 whenever any Muun is equipped. Registered
record byte34 and caster byte31E8 still need semantic/transition recovery.
The action verifier now also pins selected loader instructions and verifies
the exact16 identities against the original table hash/checksum. PASS.

### Registered flag producer and temporary suppression

Cross-reference the existing cached-owner producer audit in
BRECHE_CHILDREN_REVERSE.md: this is the SAME auxiliary configuration manager,
not an Obsidian-owned child system. Re-decoded1089CD3 confirms record+14 is
the character, record+18 its object, and record+1C a new auxiliary object.
Anchored108A1BE..108A42F writes record+34 at108A408 as
`characterObject.byte0 & (character.byte31E8 == 0)`; getter96139E..9613AC
returns byte0. This agrees with the existing character-object Live evidence.
The record initializer/reset108A124..108A183 starts byte34 at zero.

Character byte31E8 is NOT established as a safe-zone or equipped-Muun flag.
140DC35 (called before action selection by1327DE8) can set it to1 at140DCAE,
and stores the time-source result from IAT1B4C4A0 in character+31EC.
Complete10C34A5..10C34EA clears it only when unsigned
`savedCounter + 3000 < currentCounter`; equality does not clear it.
Thus it is a temporary suppression flag in this path. Gameplay trigger labels,
time-source import identity and other write paths remain to verify.

Subsequent pinned PE import audit resolves IAT1B4C4A0 uniquely to
kernel32.dll!GetTickCount. This closes the import-identity gap above. The
anchored10C34A5 block performs DWORD addition3000 and unsigned comparison:
equality retains suppression, strictly later clears it. Six verifier cases
cover before/equal/after deadlines and wraparound. Notably savedFFFFFF00
produces deadline2744 after wrapping, so nowFFFFFF01 already compares later;
do not describe the source as elapsed-time wrap-safe subtraction. This is
source arithmetic evidence only, not implemented native auxiliary-state parity.
Do not replace the unsigned arithmetic with an invented wrap-safe contract
without explicitly documenting compatibility behavior.

The existing action verifier now checks these selected flag-producer/reset
instructions against the pinned dump. Source metadata checks in140DC35 decide
whether this suppression is applied before the191/317 choice; recover those
for skill273 before enabling the alternate action. Native no-Muun target
position handling remains unchanged, consistent with Breche's boundary.

### Offline metadata lookup

963291 uses manager70A218C+108 map; 95ED56 walks nil-byteD nodes with
key10, left0/right8, root=head+4. 960B61 and9632DF yield INLINE metadata
at node+14, not a pointer read from node+14. Missing keys fall back to
manager+38; the offline inspector deliberately rejects missing keys.
The pinned full dump (SHA374D1ECB06719389416938E0BE27A32596E91E0A5B5D5A7C6F8DF690C326EFED)
contains all nine keys271..279. All have byte0=0 and dword4 equal to key.
Dword90=1 for272,273,278; it is0 for271,274,275,276,277,279.
Key273 resolves through435/300/213/265/281/273 to metadata164EE7D4.
Reproducible read-only inspector:inspect_obsidian_metadata_snapshot.py.

Complete140DC15 accepts only6 or15. In140DC35, a metadata90 outside
{0,5,7} preserves the explicit third boolean argument. Therefore for the
observed key273 row and explicit false, this helper does NOT set suppression.
This is conditional on confirming that the local call's saved+0C actually
supplies key273; do not infer that provenance merely from the row ID.
It also does not clear previously active suppression. Metadata is runtime
expanded, not the raw120-byte skill.bmd row; no guessed field mapping used.

Call-key closure: local10E34E8 loads context+0C as the switch value;
10E3567/3571 select the Obsidian branch for273, while10E360D/3617 also
accept693. The branch pushes the SAME context+0C at10E46E5 into1327DE8.
Base273 therefore uses the verified273 metadata row;693 must retain its
separate profile. Received12C5540 preserves local1950 into switch1954;
273-201 indexes selector12CE8CA to23, whose jump-table12CE85A entry is
12CB59E. Remote action call12CB5B6 passes unchanged1950 to1327DE8.
Pinned verifier checks both local and received routes. For base273 and the
explicit false passed by1327DE8,140DC35 does not newly set suppression;
existing suppression and manager membership still control191 versus317.
Descriptor now records this conditional source contract and native mapping
OPEN, rather than presenting317 as an unexplained generic argument.

Native boundary inspection: ApplyExtendedPetCharSet in ZzzCharacter.cpp
uses two MuunHelper slots and CreateMount with renderSlot1/2. GOBoid.cpp
delegates petType8 to RiseMuunFollow, which updates the mount's following
position and idle/move actions. These inspected paths do not expose the
source keyed auxiliary record, suppression flag or sit_pos_0 pose contract.
Legacy WSclient riding branches select SS6 helper animations; that is not
proof of clip317 equivalence. Keep317 native mapping OPEN; do not alias it
to SafeZone, generic mounted state or any equipped Muun. Ordinary290 QA
remains enabled, and whole-skill acceptance still requires this compatibility
case to be resolved, not silently excluded.

## Model 0x5E3 — obsidiana

### Suppression expiry call-site coverage

Direct rel32 scan finds calls10C34A5 at133E8A7 and1424C4E. The latter
is inside anchored1424B34: successful model lookup968B22/EF2A81 is required,
then object.Type must equal0xA94. It passes the character argument+8 and
ignores the returned flag. Anchored13F3EF8 calls1424B34 after a Live check.
This proves a character/model update path, not unconditional timer polling
or an exact per-frame cadence. Caller visibility/scheduling still matters.

The other call's bounded block133E87B..133E8E5 requires signed character
byte38 equal1 and registered-entry predicate10C2F89 true before expiry check.
It writes destination wordA8=0 when suppression is clear, otherwise1; if
byte38 differs it writes0 without the expiry check. Destination ownership
and the full enclosing function's earlier gates remain to be established.
Do not equate this write with the Obsidian191/317 setter. Initial exploratory
windows began mid-instruction; only the re-anchored blocks cited here enter
the verifier. Eighteen selected caller instructions are now pinned.

Destination closure: enclosing prologue133607C obtains model(arg1C) via
968B22/EF2A81 and stores it at local-b7b8 (13360CA). Continuous decoding
from that prologue to133E8E5 confirms the local model pointer assignment.
133E84C..133E85C admits only model5C0/5C1. Registry identifies these as
Data/Item/Robotwing and Robotwing_gold, registrations18CF254/18CF289.
The first registration's dump arguments independently confirm5C0 and loader
18BBEE0. Thus wordA8 writes select the associated model action0/1, NOT
the character191/317 action. Another preceding gate961566 reads character
byte4 and must return zero before the registered/suppression predicates.
Byte4 semantic naming is not inferred. This shared wing-model branch is not
an additional Obsidian child to stage. Eleven more source instructions pin
model provenance and the preceding gates; outer function scheduling is OPEN.

- Correction:1578145..15781C3 computes integer half floor(15/2)=7 and
  denominator15-7=8, NOT symmetric float half7.5. Native rotating alpha now
  uses (life>7 ? 15-life : life)/8*.9, peaking.7875 at life8 and7. Both
  base models initialize Timer0 and use private whole-effect ticks with
  update at life0:16/21 visits before native cleanup. Aura alpha has no
  source clamp1578208..1578283; removed provisional0..1 clamp. Its final
  scalar alpha would be-.1 if owner remains valid across all21 visits.
  verify_obsidian_alpha.py PASS pinned math/source inclusion;ClCompile PASS.
  Actual helper execution/particle/visual acceptance still open.
  Action191 maps to isolated290; second accepted source action317 has no
  verified native action mapping yet. Do not substitute a nearby action.
- Registration `0x18D2BAD` -> `Data\Effect\obsidiana.bmd`.
- Constructor `0x148059E`: lifetime/max-life 15, raw field A0 0.85, alpha 0,
  angle Z `+20`, raw fields AC 0 and B0 -0.45.
- Update `0x1578113`: angle Z `+60` per tick and symmetric half-life alpha
  envelope with peak multiplier 0.9.
- Renderer `0xA48B0E`: RGB `(0.48,1,1) * alpha`; raw AC `-1` and B0 `+0.075`
  per render; native BMD mesh 0, flags `0x42`, blend-mesh argument `0`.
- Decoded identity: `obsidiana.SMD`, 2 meshes, 3 bones, 1 action. Embedded
  textures are `JointThunder01.jpg` and `alpha_motion.tga`.

## Model 0x5E4 — obsidian

- Thunder8 implementation recovered:1650BA3..1650BED sets life/max15,
  restores caller scale after common random draws, leaving caller RGB.
  Update16C00CB..16C01CE resetsRotation0,Scale+.03,RGB*.85, then
  16C03D0->172343C post-decrement. Native private particle clock now handles
  exactly15 updates and resets on every allocation;MagicPin9/10 unchanged.
  Model scalar code extracted to GrowLancerObsidianTick.h used by runtime
  and12-case x86W4WX regression (fourFPS,16/21/15 visits). Tests PASS,
  project3cpp23headers PASS; subsequently linked/staged in candidate6BB0C35B.
  Renderer7F2B routing is shared but subtype8 constructor/update now explicit;
  actual native pool,action317 mapping and visual comparison remain OPEN.
- Registration `0x18D2BE2` -> `Data\Effect\obsidian.bmd`.
- Constructor `0x1480640`: lifetime/max-life 20, raw field A0 0.7, alpha 0,
  angle Z `+0.1`. It transforms local offset `(-10,0,170)` through the effect
  angle and adds it to the effect position, then emits particle type `0x7F2B`
  (`Effect\Thunder01.jpg`), subtype 8, light `(0.5,0.95,1)`, scale 2 and
  null owner. The eighth S21-only trailing particle argument is also zero.
- Update `0x15781D0`: destroys the object unless the owner action is 191 or
  317; otherwise alpha rises by 0.1 for the first half and falls by 0.1 for
  the second half of its 20-tick life.
- Renderer `0xA48C8C`: native mesh 0 uses RGB `(0.21,0.26,1) * alpha`, flags
  `0x42` and blend-mesh argument 0; native mesh 1 uses RGB
  `(0.46,0.53,1) * alpha`, flags `0x42` and blend-mesh argument 1.
- Decoded identity: `obsidian.SMD`, 2 meshes, 2 bones, 1 action. Embedded
  textures are `line_fire.jpg` and `flare_mono.JPG`.

## Assets

All files are read from the untouched S21 client and staged only under the
isolated Grow Lancer test client. The hash-pinned staging verifier is
`tools/grow_lancer/stage_obsidian_assets.ps1`.

- `obsidiana.bmd` v0F SHA-256 `CEA8E36B26C01DF4CA5CF1D177DF4E5BCA9D49756538082F0FB9B5DB44D69C16`;
  v0C SHA-256 `DF4D76E36ED05D9320224AB52B43D250B5E87EBBFCE1F08F618463BBCDF09D1A`.
- `obsidian.bmd` v0F SHA-256 `0F779291DEBEF892E2FED99399769CEFF4F8CA48D5D9E86E25F2A77546AA8B94`;
  v0C SHA-256 `1CB0C7FB5DA30FE6D7A41635517146E750BA44574807281BB36DAAA68BFC2B4F`.
- Texture dependencies: `JointThunder01.OZJ`, `alpha_motion.OZT`,
  `line_fire.OZJ`, `flare_mono.OZJ`, plus direct particle `Thunder01.OZJ`.
- Sound: `Obsidian.wav`.

RISE already registers `Data\\Effect\\Thunder01.OZJ` as native
`BITMAP_ENERGY`. Its SHA-256 is byte-identical to the S21 source
`9A7A117E3E4D4D8A6500F614C933983D80F350305B9BC8C71FCCF7E81E6E4363`.
The port therefore uses `CreateParticle(BITMAP_ENERGY, ..., subtype 8)` rather
than dispatching a new texture ID into a particle switch that does not own it.

## Remaining acceptance work

- Latest selected-renderer audit: A48C5F pushes blend0 for rotating mesh0;
  A48DA1/A48EC6 push blend0/1 for aura mesh0/1 respectively. Calls at
  A48C79/A48DBB/A48EE0 reach the native mesh renderer. Corrected provisional
  blend-2 arguments only in these private Obsidian branches.
  verify_obsidian_renderer.py PASS. Isolated Win32 link and stage verification
  PASS, SHA256 6BB0C35B23D5F92118960FE2D55B31409338BFADCCAA9C3632633560A6BB5FE3.
  This does not establish visual parity or action317 compatibility.

- Prove party target enumeration, duration and server-authoritative buff apply,
  refresh, rejection and persistence behavior.
- Map action 191/317 to an evidence-backed SS6 skeleton contract.
- Isolated IDs and client build/stage exist; complete owner visual QA and
  authoritative server integration (catalog/build alone is insufficient).
