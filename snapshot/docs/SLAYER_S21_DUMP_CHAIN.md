# Slayer S21 dump chain (source of truth)

This port is based on the captured Season 21 `main.exe` image, not on the
Webzen screenshots.  The protected input is:

- `D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe`
- PE image base: `0x00400000`
- size: `266,697,351` bytes
- SHA-256: `6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637`

The dispatcher at VA `0x012CEB00` compares the decoded skill value and jumps
to these native handlers:

| Skill | S21 name | Dispatcher compare | Handler | Native effect root(s) |
|---:|---|---|---|---|
| 292 (`0x124`) | Sword Inertia | `0x012CEF03` | `0x012D0F0F` | base action: `0x679`; receive `0x68A/0x689` requires upgrade `0x818` |
| 293 (`0x125`) | Bat Flock | `0x012CEF13` | `0x012D10A0` | `0x682`, `0x683`, `0x684` |
| 294 (`0x126`) | Pierce Attack | `0x012CEF23` | `0x012D1209` | no unconditional receive root; base action branch creates `0x68A` |
| 295 (`0x127`) | Detection | `0x012CEE1E` | `0x012D139B` | `0x692` |
| 297 (`0x129`) | Demolish | `0x012CEF58` | `0x012D1443` | `0x695` |

The handler prologues also set the native action IDs `0xE0`, `0xE3`,
`0xE4`, `0xE8`, and `0xE9` respectively.  The isolated merged player model
maps all five native action slots to private 5.2 actions `284..288`. A
zero-mesh action hash comparison shows S21 action `0xE9` (233) is
byte-identical to `0xE8` (232), but its updater still checks a distinct
action ID, so Demolish keeps merged action `288` instead of aliasing 287.
The S21 player speed setter at `0x1408874..0x140A62D` derives its Slayer
attack term as raw `AttackSpeed * 0.002`. It writes action `E0` as
`0.43 + term`, actions `E1/E2/E3/E4` as `0.40 + term`, and actions
`E8/E9` as `0.10 + term`. RISE 5.2 uses `0.004` for many legacy attack
actions, so the isolated Slayer action slots use this separate native
S21 term rather than inheriting that legacy multiplier or fixed `.35`.
The private five-action `Tests\SlayerBuild\Client\Data\Player\player.bmd`
is SHA-256 `BF6F5C0F029E1AF6AE78C805E1AAC56DCC2B3C6A37675A61CAEED36C95D41D25`.
It was produced by the audited Grow Lancer skeleton merger using selected
S21 actions `224,227,228,232,233`, read-only S21 player hash
`E0F4CB5A0956192D04026135766C566BC6B9B7B559D2F196F74D24D28E2E755A`,
and read-only 5.2 base player hash
`2964E7C41DCF686B79E53A74261192EBFC6879D36BC030C2E503EF0B8CEF38EF`.
The previous private 288-action player was preserved as
`player.bmd.before-action288.bak` before replacing it; the base client
player tree was not changed.
The local 5.2 cast path now applies those merged clips and seeds the cast
roots immediately after sending the canonical skill request; it no longer
runs `SetPlayerAttack`, which replaced the native clip with a weapon/fist
attack until the server cast packet arrived. The canonical receive path
applies the clip/root for other clients. For the local caster, the accepted
`0x19` acknowledgment consumes the pending local graph without restarting
the actor clip or duplicating the roots. A fresh server join clears the
pending local-ack state. Damage, DOT and buff acceptance remain GS-owned.
The incoming 0x19 cast route resolves the target slot but no longer drops an
already accepted offensive graph merely because the target died between the
hit and packet arrival. Local click selection still requires a living target.
This keeps successful cast visuals separate from target survival and does not
authorize damage after death.
The targeted local cast accepts live monster and player actors, excluding
the caster; 5.2 GameServer remains the authority for safe-zone/PvP/hostility
legality. The previous monster-only client gate silently suppressed Slayer
casts against valid PvP opponents.
The S21 `D:\GameServer S21\Data\Skills\RegularSkillCalc.lua` (SHA-256
`F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847`)
uses fractional `STR/8 + DEX/28 + 120` for Sword, Bat, and Pierce; Bat's
strike then applies `0.5`. 5.2 GS now evaluates the same ratio as
`(7*STR + 2*DEX + 6720)/5600` (or `/11200` for Bat) and truncates once
at the final integer damage boundary. Its previous integer STR/8 and
DEX/28 terms lost damage at non-multiple attribute values. The second supplied
Lua copy, `D:\GameServer S21\Data\Scripts\Skills\RegularSkillCalc.lua`
(SHA-256 `78A1502C187D9EA248E682CCAA62EA8C4510D7119192510D1A966A18E6992E4D`),
does expose `BatFlockDotDamage`: both player and monster results are `DOT/100`.
Its comment points to `FormulaData.xml::Character` formula 9 (SHA-256
`9A513DD4375116CFD0FA599114B2103B4D21B194B775AE376E0C6289BA36E6FF`),
which substitutes caster Energy four times. GS Bat DOT now follows that
Energy-based formula instead of reusing half of Bat's initial hit and keeps
the S21 five-second duration.
The [Webzen Slayer guide](https://muonline.webzen.com/es/gameinfo/guide/detail/100)
explicitly describes **two** Bat Flock contact-damage hits before DOT.
5.2 `BasicSkillAttack` calls `gAttack.Attack` once, so the two GS invocations
on each accepted target are intentional; each applies the separately pinned
half-damage formula. A transient one-hit edit was withdrawn after checking
this primary specification. Exact protected-GS rounding and ingame damage
timing remain unverified.
The protected GS still prevents verifying
which of the two supplied Lua paths it loads and its exact final rounding.
The S21 `SkillList.xml` rows also specify Slayer class stages 1/2/3 and
STR/DEX requirements. The GS skill gate now checks the DB-stage and the
imported level/attribute requirements; the legacy `SKILL_INFO` struct has
no STR/DEX columns, so class-only gating previously accepted these skills
at any attributes.
The hash-pinned S21 `D:\GameServer S21\Data\Skills\SkillRequire.xml`
(SHA-256 `4E6B765FF7DCFB62E58AD9720F5E8C8E81DC8ECC5BA30C3BC3B842B390C9AC58`)
adds a separate Pierce Attack bead/use prerequisite: learned Bat Flock 293
and 10 points in Bat Flock Mastery 782 (item 12:479). The 5.2 GS now
checks those stored skill/mastery slots before accepting Pierce; a Slayer
with stats alone cannot bypass the S21 prerequisite.
The 5.2 tree stores the displayed mastery level as `m_level+1`; a fixed
10-point mastery node is stored at level 9. The Pierce gate therefore uses
`GetMasterSkillLevel(...,782) >= 10`, not raw `m_level >= 10`, which would
make the S21 prerequisite impossible even after the node is learned.
The native 5.2 mastery path may replace the active base `Skill[]` entry with
an upgraded index. The Pierce gate accepts an active Bat lineage entry
`293/781/782` alongside the independently persisted `782/10` node instead
of rejecting an upgraded Slayer only because `Skill[]` no longer has raw 293.
The client demand path previously checked Pierce STR/DEX but not that
mastery prerequisite. Since it seeded local effects before GS acceptance,
an ineligible Pierce request could still paint a false graph. The isolated
client now reads the GS-sent displayed level of node `782` from the Master
Slayer UI (at least 10) and requires an active Bat lineage `293/781/782`.
`UseSkillSlayer` repeats the gate after pathfinding, immediately before
sending the request. GS remains authoritative; this is a local rejected-
cast visual guard, not proof Pierce motion or damage passes ingame.
The separately hash-pinned S21 `SkillTreeData_3rd.xml` (SHA-256
`CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0`)
places Bat Flock Strengthener `781` at slot 58 and Bat Flock Mastery `782`
at slot 62 in the Master Slayer `Class ID=512` tree; `782` requires `781`.
The isolated 5.2 QA `MasterSkillTree.txt` has neither 781 nor 782, so the
new Pierce gate is currently **unreachable by normal 5.2 mastery learning**.
This is a remaining class/tree import dependency, not proof that Pierce is
playable merely because its GS binary builds. The same S21 Slayer tree has
`Rush` as a distinct skill `631`; it is not evidence for a base Pierce
world-position write and must not be used as a substitute animation.
The read-only S21 `Data\Local\masterskilltreedata.bmd` is SHA-256
`A0381045194779304C7922685D1DDB9EA1E23DD0354FE02A36A9C91C8305D586`.
Its Bux-XOR 24-byte records carry 2048 slots; Master Slayer uses class bit
`512` on **58 nodes** across all three categories. Bat nodes are native
records 653 (`Index=58`, `Skill=781`, `MaxLevel=20`) and 655 (`Index=62`,
`Skill=782`, `RequireSkill[0]=781`, `RequiredPoints=10`, `MaxLevel=10`).
The read-only verifier now matches **all 58** client BMD nodes against the
hash-pinned server XML by slot/group, minimum/max points, both parents and
MagicNumber; no two-node imitation is a valid replacement for this tree.
S21 class 512 alone owns IDs `631` and `779..794`, but the private 5.2
`MasterSkillTree.txt` (SHA-256
`28D9C022DA82DB60C94137DA7EFBAFAF9AC9685D1A36E2FE29A00FBBFB878E81`)
already contains an unrelated `631` row and none of `779..794`.
Therefore the GS class-learning guard scopes only `779..794` by ID. The
`631` collision must be separated by class/tree provenance in the full
adapter; gating it by number would regress the legacy 5.2 row.
The 5.2 `MasterSkillTreeData.bmd` is SHA-256
`D67B20890CBB2DCFF9FF9CAB670E30D51A3DD13C97B17D4C641676B43A7DDECC`,
has exactly 512 occupied records, and its UI field `DefValue` interprets the
last four bytes as a float, whereas the S21 rows contain integers `22/23`.
Cross-checking all 58 Slayer records with the pinned S21 `SkillList.xml`
proves that this integer is **SkillList Damage**, not an opaque unknown:
`781=22`, `782=23`, and every other node agrees too. A native-to-5.2
adapter can convert those numeric defaults to floats without copying their
integer bit patterns. The S21 `masterskilltooltip.bmd` (SHA-256
`76E07F264FCA9C20692EF8C1870B90C63B2C0EB0456E273D9DBBC6628BE73F0A`)
has 4096 Bux-XOR records of 404 bytes, not the 5.2 616-byte tooltip struct;
all 58 Slayer MagicNumbers have matching native tooltip records. The Bat
tooltips are records 854 (`781`) and 856 (`782`), with native rank/required
point strings. They need field-aware repacking, not a raw file copy.
`convert_s21_master_tree.py` now emits **58-node** private 5.2 records
`MasterSlayerTree.bmd` and `MasterSlayerTooltip.bmd` under the isolated
SlayerBuild client. It converts integer Damage to float DefValue, expands
the 404-byte native tooltip fields to the 616-byte 5.2 struct, checks every
record, and appends a private CRC32 trailer. These are staged data inputs,
not an active client loader or a server learning-path PASS yet.
The next client code dependency is real: `_define.h` has `MAX_SKILLS=650`,
`Winmain.cpp` allocates exactly 650 `SkillAttribute` rows, while the SS21
Slayer tree references 781/782 and other IDs up to 794. Loading the new
tree without a Slayer-specific high-skill metadata capacity/access path
would index beyond that allocation. The 650-slot `CharacterAttribute`
arrays and the checksum-protected 650-record `Skill.bmd` must stay at their
legacy sizes; a blanket change to `MAX_SKILLS` would alter those ABIs.
5.2 `GetCharacterClass` also currently returns Knight for the reserved Slayer
client marker, so `SetMasterType` cannot select class bit 512. The isolated
`verify_s21_master_tree_shape.py` pins all of these facts. Importing only
two rows or copying the SS21 BMD without a full client/GS class-tree adapter
would manufacture an unusable Pierce prerequisite; no such shortcut is used.
The GS `CGMasterSkillRecv` independently checks persisted Master Slayer
class/stats and the 781/10 parent. A private 58-node class-512 tree and UI
are now mounted, and a class-scoped learning adapter derives 781/782
slot/group/rank/points/parents/replacement from that S21 overlay. The older
claim that the UI and learning rows were absent is superseded. This remains
unaccepted ingame, with mastery point-value/DOT-duration curves unresolved.
The S21 `SkillList.xml` rows 781/782 (SHA-256
`3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0`)
show both Bat mastery nodes as **castable** stage-3 Slayer skills, with
`UseType=4`, brands `293 -> 781`, level 160 and STR/DEX `100/380`.
`MasterSkillCalc_3rd.lua` (SHA-256
`771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699`)
has distinct damage functions for the two IDs. The pinned S21 `ReceiveMagic`
reads the incoming 16-bit skill at `0x12CEBAE`, calls `0xBCFF9F` at
`0x12CEBBC`, and switches on its return at `0x12CECB7..0x12CECBD`.
The helper follows each SkillList row's `Brand` at `+0x5C`, stopping at
0 or 75 and returning the ancestor skill at `+4`: `782 -> 781 -> 293`.
Thus upgraded Bat casts reuse the native **293 visual graph**, rather than
having separate direct 781/782 effect-switch comparisons. The 5.2 client
now resolves that pinned high-ID Brand chain for local and remote visuals
while preserving the raw 781/782 ID in cast packets; GS routes both IDs
through Bat's target graph and retains their own skill-row resources/damage.
The per-level mastery values and normal 781/782 acquisition are still
unverified ingame, so this is code provenance and build evidence, not PASS.
Decoded, hash-pinned S21 Master Slayer tooltip records sharpen the remaining
behavior: 781 says it **increases Bat Flock damage**, while 782 says it
**increases the time Bat Flock deals damage by `%d` seconds**. The pinned S21
`SkillSettings.ini` gives the base DOT time as **5 seconds**. The current
5.2 Bat handler still applies a fixed five-second DOT for raw 782, so it is
not mastery-complete; the point-to-seconds formula is not present in the
client BMD or `SkillTreeData_3rd.xml` and must be recovered before assigning
a numeric extension. This is why cast routing/build alone is not a QA gate.

The root `CreateEffect` argument windows in the same handlers pass the
caster's position (`native OBJECT+0x158`) and angle (`+0x164`) without an
extra height offset. Bat Flock, Detection and Demolish copy the
caster's light (`+0xEC`); base Sword's `0x679` and upgraded Pierce `0x679` use a white
`(1,1,1)` constant, whereas the base action's `0x68A` copies caster light. The root
scale argument is zero. Bat Flock's three roots `0x682/0x683/0x684` all
start at caster position and do not follow the packet target. The native
handler `0x12D10A0` passes zero target-list count/pointer to all three root
calls. The allocator `0x143E975..0x143E9EC` only copies a list when both
values are nonzero; the `0x682/0x683` update cases choose a target from that
list or use `-1` when it is empty. Thus the canonical roots carry no 5.2
target-index adapter. The attack action and sound still use the packet target.
Supplemental target-list effects need their own proven receive path; the
current 5.2 fanout adapter cannot yet be called S21 attack-shape parity.
The upgraded Pierce root `0x679` also starts at caster position, not the target.

That supplemental path is now identified: S21 client receive branch
`0x12A6225..0x12A622F` selects skill `0x125` and enters `0x12A76B5`.
Its payload reads skill at `+4`, caster key at `+6`, a capped target count
at `+8` (maximum 10), and target keys from `+0xA` with two-byte stride.
It creates an additional `0x682` subtype-1 root and `0x683` subtype-0
root at caster position, then appends each resolvable target key to both
roots' lists and sets each list count (`0x12A77F5..0x12A793A`). Native
`0x682` subtype 1 skips its `0x688` child initializer. The 5.2 port now
implements this as a distinct private `0xF4:E3` fanout envelope (not a
transplanted S21 packet layout), a validated receiver, and a sidecar list
for these two effects. GS now enumerates eligible viewport enemies within
the imported range-6 catalog row, capped at the native list limit 10; each
affected enemy receives two server-authoritative strikes and the existing
five-second DOT. The supplemental visual packet carries exactly the affected
set. This is a provisional 5.2 collision adapter: the S21 client dump does
not disclose GS collision geometry, so multi-enemy attack shape parity is
still unverified. Clients accept resolved actors even if a prior damage
packet has set their Dead flag, allowing the authored killing-blow visual.

There is a second skill-action switch at VA `0x010E6830`. It is not the
ReceiveMagic dispatcher above: its 292 branch creates `0x679` unconditionally,
then compares the active skill to `0x818` at `0x10E6A43..0x10E6A4D`.
Only that upgrade creates `0x68A/0x689` and plays `0x50D`;
the base action plays `0x509` at `0x10E68F9..0x10E6907`.
The `0x12D0F0F` ReceiveMagic handler likewise checks `0x818` before its
`0x68A/0x689` calls and skips all of them for base `0x124`.
The earlier 5.2 ordinary Sword graph incorrectly spawned those upgrade-only
roots and is now switched to base `0x679` only. Its 294 action branch always creates `0x68A` at the caster;
the later `0x679` call is gated by `skillStruct+8 == 0x816` at
`0x10E6DE4..0x10E6DEE`. The ReceiveMagic 294 handler itself starts with
`cmp [ebp-0x164c], 0x816; jne 0x12D1396` at `0x12D1209`, so its `0x679`
root and sound `0x509` are upgrade-only. The base action sound is `0x50D`.
The separate network `0x679` root at `0x12FB380` is selected for skills
`0x124/0x816/0x818`, not base `0x126`. The 5.2 ordinary base cast therefore
creates only `0x68A` and does not play the upgraded `0x509` hit sound.

The `0x68A` subtype-zero update at `0x1546793..0x1546803` reads the owning
actor's animation frame (`owner+0xD0`) and sets root alpha (`root+0xDC`) to
`frame/3.5` through frame 3.5, then `1-(frame-3.5)/3.5`. The literal at
`0x1B4ED34` is `3.5f`. The 5.2 root uses the merged actor clip frame for
that triangular envelope rather than its own 30-tick lifetime.
The same subtype-zero update creates child model `0x691` only after
`rand() % 6 == 0` at `0x1546BEA..0x1546D75`; the earlier per-frame 5.2
child emission was not S21-backed.
Its creation arguments at `0x1546CC1..0x1546CED` retain the root position
and replace angle Z with a `0..360` random sample. The 5.2 child now uses
the same yaw range instead of inheriting the fixed caster angle.

The base Pierce `0x126` has its own *list-bearing* receive path, selected at
`0x12A6235..0x12A623F` and entered at `0x12A7945`. It resolves caster key
at packet `+6`, creates `0x68A` for remote casters only, then always creates
`0x689` at caster position/angle/light. The root receives the packet skill
at `+4`; the loop at `0x12A7AD3..0x12A7B87` appends target keys from
`+0xA` (two-byte stride) and sets the list count from `+8`. The `0x689`
constructor at `0x12A7A3D` snapshots the caster actor's original selected
target key (`actor+0x3E`) into `effect+0x364`. Its update at
`0x1545A39..0x1545A80` checks that fixed actor still resolves before it
walks the separate list at `effect+0x1DC`; this is not the currently active
lane key. The 5.2 GS fanout puts the clicked victim first and its client
bridge keeps that index in a pool-reuse-safe sidecar, separate from the
mutable `m_sTargetIndex` inherited by each lane child.
The `0x689`
update at `0x1545A08` reads the next key from that list, resolves the actor,
and emits `0x68D`, `0x68B`, `0x68E`, and `0x68C` for that lane. The
launch-position block at `0x1545D43..0x1545E78` rotates `(0,-1,0)` by
the caster-to-target yaw (`0xD32DC5`), multiplies the forward vector by
`100`, and subtracts it from the target's position. The earlier 5.2 port
had the signs reversed and launched beyond the target; the native 5.2
AngleMatrix/VectorRotate bridge now preserves the subtraction. The
`0x68B/0x68E` child-creation windows at `0x1545EA1`, `0x1545EF8`,
and `0x154605F` read scale from their owning `0x689 EFFECT+0xA0`.
The supplemental receive at `0x12A7A43` passes zero incoming scale;
S21 `CreateEffect` at `0x143E7E1..0x143E80D` first normalizes that to
`0.9` in `EFFECT+0xA0`. The `0x689` init at `0x1490F7A..0x149104C`
leaves the normalized value untouched. The 5.2 bridge now passes root
effect scale rather than caster actor scale. A later
state transition pops the key before advancing: the `0x68B` update calls
helper `0x1417693`, and on failure writes `2` to its owning `0x689`
state at `0x154759F`. The root's `0x15466CF` branch then pops the key
and resets state to zero. The 5.2 overlay now has a
separate private `0xF4:E5` fanout packet, a validated receive bridge, and
a pool-reuse-safe target-list sidecar on `0x689`; it does not alter 5.2
`OBJECT` or transplant the S21 packet ABI. The current GS attack resolver
now uses the S21 AOE declaration (`D:\GameServer S21\Data\Skills\SkillAOETargetting.xml`,
row 294, SHA-256 `CE5C501CB82D2179DB5ED5407DC89BEA4CB118446BA444D8FF95D7DE96F984D9`)
plus native 5.2 viewport/range/legality checks to authorize up to ten victims.
It now sends that serialed accepted list without damaging all victims at
cast time. Each local `0x689` lane sends one private `F4:E6` request; GS
checks current connection, cast serial/window, learned/class-qualified
Pierce skill, map/safe-zone/target legality, cast-origin range, target tile,
and one-use target membership before applying two strikes (or four if the
target carried the Bat Flock mark when the lane lands). The lane uses the
same 5.2 `gAttack.Attack` damage call after validating the saved cast-origin
range: `BasicSkillAttack` would otherwise recheck the moving caster tile
and drop a valid delayed lane. This restores the
S21 per-target attack cadence instead of an immediate all-target burst.
The first 5.2 transaction adapter stored only one pending Pierce cast per
caster, so an immediate recast discarded all unconsumed lanes from the
previous cast despite S21 Pierce having `Delay=0`. GS now retains up to 32
simultaneous cast sessions per connected caster, assigns a serial not used
by any session still inside the 15-second replay window, and prunes only
expired or previous-connection sessions. Fully consumed and previous-map
sessions remain serial tombstones until expiry: their lanes are rejected,
but a delayed packet cannot be matched to a fresh cast after wrap.
New casts fail closed when the bounded queue is full; an authorized lane
cannot consume a different session after serial wrap. This is a concurrency
repair of the 5.2 adapter, **not** recovered S21 GS collision geometry.
The accepted 5.2 fanout now excludes safe-zone occupants and duel
spectators at cast time too, matching the delayed-lane legality gate; the
private Bat Flock affected-list selection uses that same 5.2 filter.
The `F4:E6` transaction and 15-second replay window are 5.2-native
adapters; their exact S21 GS implementation is unavailable. This restores
list-bearing multi-enemy behavior rather than forcing
one target, but **the AOE collider/selection geometry remains provisional**:
the supplied S21 GS is a protected binary without a matching dump/source.
Static PE preflight of the supplied
`D:\GameServer S21\GameServer\IGC.GameServer2_R.exe` (SHA-256
`134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74`)
shows x64 machine `0x8664`, an entrypoint in a high-entropy `.boot`
section, and a `.vm_sec` section; no local `.dmp/.pdb/.map/.idb` for
that GS exists. It was inspected read-only and never executed by this
port. Its protected on-disk bytes are not reliable evidence of the native
Pierce collider or caster movement path.
The caster outbound position/return path also remains unverified. The
model-helper completion condition is now decoded: `0x1417693` resolves the
child's target actor model, selects effect action zero, then calls animation
step `0x132D0CD` with that model action's `PlaySpeed`. The native allocator
zeros effect action/frame at `0x143E7A8/0x143E92D`; the animation step
returns false at the action-key boundary. The 5.2 bridge now uses its own
`BMD::PlayAnimation` with the same actor model/action-speed input and advances
lanes on the child-to-root state transition. This closes the arbitrary
eight-tick/lifetime completion guess, but not S21 actor-movement parity.

At `0x15464BE..0x15466A0`, only the local caster builds a native `C1:00`
packet after placing the selected target. The block reads that target's
world XY from its position vector, divides by the 100-unit terrain tile
factor, and writes the target tile coordinates with skill bytes. The
`0x15465A1..0x154668E` append order includes skill high, literal `1`, skill
low, target tile X, a direction byte, target tile Y, a zero byte, and the
selected target key split into high/zero/low bytes. `0xD8DE14` finalizes
and sends that packet once per local `0x689` lane. The helper at
`0x173EA5C` maintains a per-lane outbound counter: its global word increments from
1 through 50, wraps to 1, and writes the low byte. It is not actor yaw.
The private 5.2 `F4:E6` adapter now carries the same 1..50 byte and GS
rejects out-of-range values, without treating it as the accepted cast serial.
This is evidence of a
per-target outbound skill/position request, not proof that the client
directly moves the caster: the block does not write caster `OBJECT+0x158`.
RISE 5.2 uses head `0x00` for chat, so the S21 packet was not copied into
5.2. The native `F4:E6` adapter above carries the accepted cast serial,
target key and tile to GS for validation; it does not mutate caster world
position. The separately sent action/cast
packet at `0x10EEB1E` is native `C1:57`, another incompatible 5.2 route.
Its constructor at `0x10EEB92..0x10EEC61` is shared: the native Slayer
action switch calls it for Pierce at `0x10E6D4F`, Detection at
`0x10E6F5B`, and Demolish at `0x10E7029` (among many other skills).
The Pierce call window reads actor world XY to form packet arguments, but
does not write back to actor `OBJECT+0x158`. The adjacent action helper
`0x1327DE8` sets action/attack flags via `0x140DC35`; it is not evidence
of a Pierce-only world-position transform. These checks narrow the next
reverse target to the S21 GS/receive or later actor-update path, without
justifying a guessed 5.2 teleport.
The pinned native inbound parser at `0x1303939..0x130399C` selects
`packet[3]` for `C1` and `packet[4]` for `C2` before its `0x57` comparison
at `0x1303B01`. The `C1:57` receive arm at `0x130401C` calls
`0xB18248`, whose direct body selects packet byte 4 values 1..5 and
formats message text through `0x9609DC`/`0xB4F647`; it does not directly
write actor XY. This does not prove every server response to a Pierce cast
uses this arm, nor does it reveal a world-position-update packet.
The native `C1:15` receive arm is likewise **not** the old 5.2
`ReceiveMovePosition` path: `0x12FE560..0x12FE5AE` selects C1 byte 3,
`0x12FE5DB` routes head `0x15` to `0x12BAFDF`, and that function passes
packet DWORD `+4` into singleton manager `+0x1E4` via `0xBA8389`.
No caster XY write appears in the direct handler. Thus importing a
5.2 `C1:15` move assumption as the S21 Pierce rush would be unsupported;
the actual S21 movement/return route remains OPEN.
The separate native first-byte dispatch table at `0x130505C` maps head
`0x1E` to `0x12FEA84`, which directly calls `0x12D8419`. This is a real
generic actor-movement receiver: `C1` bytes `+3/+4` identify the actor,
`+5/+6` supply destination tiles, and the handler calls path search
`0x1328C8F`. For the local hero it writes `CHARACTER+0x180/+0x184`
destination tiles at `0x12D84CB`; for ordinary actors it copies incoming
tiles into `CHARACTER+0x2D/+0x2E` at `0x12D8599` before path search.
If both path-search attempts fail, `0x12D877C..0x12D880A` restores tile
state from **that actor's already-copied `+0x2D/+0x2E`** and writes
`OBJECT+0x158` XY at `tile*100 + native offset`. This is an actor-position
write, but it is path-failure recovery, **not** a demonstrated Pierce
teleport. The head `0x1E` is incompatible with 5.2's current `0xD4`
`ReceiveMoveCharacter` route. The protected SS21 GS has not yielded an
authoritative Pierce-specific rush/return packet trace, so the association
of this generic receiver with Pierce, its pacing, and the return packet
remain OPEN. No caster XY mutation is added from this inference.
The pinned S21 `Data\Skills\SkillSettings.ini` also sets
`RushSkillsCheckTargetPosition = 1` and describes a guard against landing
on another player's tile. That is source evidence for a generic rush
safety setting, **not** proof that Pierce uses the generic `C1:1E`
receiver or a numeric rush/return schedule. A 5.2 position adapter must
preserve this collision constraint once its actual SS21 packet chain is
identified.
The actual Slayer rush/return owner-position chain remains to be traced
before any world-position mutation is added to the 5.2 port.
The native `player.bmd` action record for Pierce `0xE4` has seven animation
keys but its action-level position-track flag is zero; merged action `286`
preserves both values. The rush/return is therefore not carried as an
action-record world-position track. The read-only S21 `Bip01` root bone
positions for those seven keys contain a local-Y excursion from `+0.76`
to `-18.17` and back to `+0.76` (span `18.92`). The merged 5.2 action `286`
retains these exact bone bytes, so the authored short visual lunge/return is
already imported. Bone-local motion does not prove an actor/world XY path
to the target, so the outbound/receive chain is still needed; adding a
second speculative local dash would duplicate the imported clip.
An additional hash-pinned main-image search for `cmp eax,0xE4` found an
actor-action branch at `0x173F390`: when the owner is playing Pierce action
`0xE4`, it selects the direct `0x173F3CA` path instead of the following
terrain/angle interpolation `0x173F3F3..0x173F4C5`. That branch reads the
current actor-related vector and writes a local integer; it contains no
write to actor `OBJECT+0x158`. The E4 compare at `0x133F0F4` only sets
draw alpha to `.3`, and the other E4 comparison at `0x1A5A5AD` is an
unrelated upper-bound check. These E4 hits narrow the likely movement
origin but still do **not** identify a native rush/return implementation.
An expanded decoded immediate-`0xE4` scan in the mapped main-image code
found eight comparison sites. Beyond the action initializer, `0x13F254D`
sets object alpha (`+0xDC`) to `0.3`, `0x154676E` selects the `0x818`
upgrade/list-effect branch and reads owner action progress for fade, and
`0x12F67DE`/`0x17526F3` merely gate the broad `0xE4..0xFA` action range.
`0x17D641D` groups E4/EA in an animation/render branch. None of these
decoded branches writes caster XY (`OBJECT+0x158` components 0/1).
This bounded direct-compare result does **not** exclude generic receive,
indirect dispatch or GS-driven movement; it prevents assigning a speculative
Pierce-only teleport to these specific branches.

A wider decoded `0xE4` search found the action-init dispatcher at
`0x1289DD1` (called only by `0x12FD7BB`). Its Pierce branch at
`0x128A082 -> 0x128BE14` writes `OBJECT+0x158` component 2 **plus 5.0**,
clears bit 0 at `OBJECT+0x68`, invokes `0x14B6619`, and creates supplemental
effect `0x81CD` at that actor's position. The caller passes its `OBJECT*`
as the second argument at `0x12FD7B5`; this is a real native actor-Z write,
so the narrower statement above that the earlier `0x173F390` branch has no
position write remains true, but a blanket "E4 never moves an actor" claim
would be false. It does **not** disclose the Pierce XY rush/return or whether
the local 5.2 use/receive callbacks correspond to this one-shot action-init
call. It must be attached to the action initializer once, not independently
to both local use and receive callbacks; it does not justify a teleport.
The shared 5.2 `ApplyCastAction` bridge now carries that exact one-shot
Pierce `+5 Z` lift after a valid Slayer action selection. Local `UseSkillSlayer`
checks a live target before calling it; `DispatchNativeReceive` consumes the
local cast acknowledgment via its pending-graph guard and does not re-run
the initializer for that cast. This now starts the `0x81CD` subtype-2
controller once, with all ten direct children now ported; it does not prove XY
rush/return parity.
The 5.2 stationary `SetPlayerStop` path does not ground `OBJECT.Position[2]`;
only movement sets it from `RequestTerrainHeight`. Without a scoped retirement,
repeated local Pierce casts would accumulate this genuine S21 `+5 Z` write.
The isolated port now saves the actor's pre-cast Z and retires that one-shot
lift after the action transition or before another Slayer cast. It restores
only when the actor still has exactly the saved `+5` offset, never overwriting
a newer movement/server correction. This is a **5.2 lifecycle adapter**, not
decoded evidence of the S21 caster rush/return or native grounding path.
The same native E4 block calls `0x14B6619` at `0x128BE8A`, after the Z lift
and before creating `0x81CD`. That helper scans two effect-object pools:
`0x10F0032` and `0x14B679A` compare each live record's owner pointer
(`OBJECT+0x34C`) with the actor and destroy matches. Native `0x81CD`
destruction calls the helper again at `0x14B6915` for children owned by that
root. The isolated 5.2 bridge now sweeps both `Effects` and `g_SkillEffects`
at the same one-shot action point, releasing target-list sidecars on
destruction. It is deliberately restricted to private Slayer effect types:
deleting unrelated legacy class/equipment effects would broaden this port
beyond the isolated client. This is owner/pool cleanup parity within Slayer,
**not** evidence for an XY rush or complete visual parity.
The missing graph is now bounded by dump branches, rather than a vague
"extra Pierce particle": native `0x81CD` initializer compare at
`0x143F67F` jumps to `0x147BE22`; subtype 2 enters `0x147D373`, copies
position/angle/light, stores owner `OBJECT+0x34C`, and sets life to 30.
Its ten direct child `CreateEffect` calls in `0x147D5A3..0x147DEBF`
are `0x81CE` subtypes 3/4/5, `0x80BA` subtypes 6/7/7/7, `0x8149`
subtype 2, `0x81CF` subtype 2, and `0x5D8` subtype 1. `0x81CE`
initializer starts at `0x147E153` and its renderer at `0x15AE426`;
`0x81CD` subtype 2 itself uses the ordinary object draw path at
`0x15AF05F`, while subtypes 3/4/5 take a terrain-alpha path at
`0x15AEF90`. The 5.2 port must recover each child's update, asset,
ownership and pass before calling this action-start graph complete.
The pinned native bitmap loader at `0xAA997F` binds `0x81CE` to
`NPC\\marks_m04.JPG` (string at `0x1B52784`). The read-only S21 `NPC` and
`Effect` copies of `marks_m04.OZJ` have the same SHA-256
`42DDB35AB1EB1F34E4EF6194959BB1B19E577ADC832BF272C1FAEBCAA89A4AB7`.
The high-code update dispatcher routes `0x81CD` at `0x14B8383` to
`0x1533E1A` and `0x81CE` at `0x14B82E2` to `0x15341D6`; the renderer routes
them separately at `0x15A1724/0x15A1734`. The private 5.2 Slayer overlay
now hash-pins/copies `marks_m04.OZJ`, reserves bitmap ID `33012`, and
registers that exact sprite. This was an asset prerequisite; the first
three `0x81CE` children, `0x80BA` subtype 6, `0x8149` subtype 2 and the
`0x81CF` subtype-2 parent, `0x5D8` subtype 1 and three `0x80BA`
subtype-7 flare objects are now spawned. All ten direct calls, the
`0x81CF` nested `0x8012` subtype-17 object, and subtype-7's per-frame
particle fanout are present. Visual parity is still unproven.
Further render decode shows `0x81CE` subtype 3 submits bitmap `0x81CE`
at `0x15AE502`, but subtypes 4 and 5 submit bitmap `0x81CD` at
`0x15AE675/0x15AE740`. Loader `0xAA995F` binds `0x81CD` to
`NPC\\marks_m03.JPG`, already present as a hash-pinned S21 overlay file.
The private 5.2 adapter now reserves bitmap ID `33013` and registers it
separately; using only `marks_m04` for all three children would be wrong.
Native `0x5D8` model registration at `0xAA99ED` names
`Data\\Effect\\marks_cylinder.bmd` (string at `0x1B527D8`); its renderer
selector goes to `0x15AE9A2`, which calls the generic model wrapper
`0x176D621`. The source BMD SHA-256 is
`F91EA00CFC10DC3E36935FE80AD6CA391EAED64142E7D572E5AB2939A7689BE3`.
The formerly unused private model slot `MAX_MODELS+41` is now assigned to
this named child asset and the direct subtype-1 effect is now spawned in
the native root-call order with actor ownership, scale argument zero and
light `(0.8, 0.5, 1)`. The S21 initializer jump table maps `0x5D8` to
`0x147ED07`: subtype 1 receives life 30, scale/alpha 1, the current
millisecond clock and the incoming angle. Its updater jump table maps to
`0x1534A58`; that handler refreshes subtype 0 only, so the Pierce
subtype-1 model naturally expires after 30 ticks. Renderer `0x15AE9A2`
passes it through the generic `0x176D621` model wrapper. The registered
second-manager callback for `0x5D8` is the trivial false handler
`0xA48614`, so that registry alone does not replace the flag-2 fallback.
The first manager `0x18917BA` looks up `Type-0xAE9` and tests an object
special flag before its fallback. Its direct map-specific branches are
map `0x66`, map `2`/type `0x7A`, and map `8`/types `0x5D`/`0x65`; none
directly selects model `0x5D8`. The owner-authorized Grow Lancer full-memory
snapshot (local SHA-256
`374D1ECB06719389416938E0BE27A32596E91E0A5B5D5A7C6F8DF690C326EFED`)
supplies the heap nodes missing from the mapped-main image. Offline
`verify_s21_full_dump_cylinder_map.py` traverses the entire first-manager
red-black tree: 4,950 unique signed keys, range `0..10759`, no negative
keys. `0x5D8-0xAE9=-1297` is therefore absent **at that snapshot**. Four
code anchors in the full dump agree byte-for-byte with the separately
SHA-pinned main image, including this lookup and the Pierce child call.
This excludes that map's interception for the sampled runtime state, not
the map-independent render manager or later mutation. The full
process dump remains local and is not included in the GitHub branch.
Further offline virtual-table resolution changes the render conclusion:
the manager at `0x1E057D8` has four live callback slots, and its second
slot points to `0xAABC91`. At `0xAACD0C` that callback **directly compares
model type `0x5D8`**, copies the object's RGB light, scales it by
`(sin(WorldTime * 0.005) + 1) * 0.25 + 0.2`, and submits mesh zero with
flag `0x42` (texture plus bright) at `0xAACEA2`. Thus the absent map key
and false second-registry callback do **not** imply ordinary opaque
flag-2 rendering. The old 5.2 `RENDER_TEXTURE` cylinder pass was a real
source mismatch; the isolated adapter now uses the pinned bright pass
and light wave without changing the authored RGB `lines2` texture.
The callback's adjacent `0x5D7` branch is **not** another Slayer child:
the native loader at `0xAA99B1..0xAA99DE` names it `fire_arrow` in
`Data\Effect\`. Do not extend the Slayer model graph from numeric
adjacency alone.
The fast special-draw flag is now bounded more tightly. Native
`CreateEffect` calls `OBJECT` reset `0x1315E97` at `0x143E71F`; that
reset calls `0x131679A`, which clears byte `OBJECT+0x3B8`. The generic
first manager tests that byte through `0x18A2107` at `0x18918E1` before
entering its map branch. The decoded `0x5D8` subtype-1 initializer
`0x147ED07..0x147EDDB` sets life, scale, alpha, clock and RGB but does
not re-enable `+0x3B8`. Thus this child does not enter the fast special
path immediately after creation. The mapped-main snapshot alone stores
only a heap pointer for the separate map, so the full-memory check above
is needed for its captured key set. The separate virtual callback above
proves the bright pass for the same captured S21 build; ingame frame
parity still needs the later acceptance gate.
The hash-pinned v0F-to-v0C conversion reports one mesh, two bones, one
action (`marks_cylinder.SMD`), and its sole material `lines2.jpg` from
S21 `lines2.OZJ` SHA-256
`79D2A20143B15E406344F43DCDAF6232DD131986BC28603B10BF70DA4F208F7A`.
Both model and texture are copied only to the isolated Slayer client.
The `0x5D8` material is an authored dark-field
source: its single S21 mesh reaches `Z=214.582`, and its `lines2.jpg` UV
range wraps `V=0..6.6296` around the cylinder. The hash-pinned S21
`lines2.OZJ` decodes to 32x64 RGB; 1,095 of 2,048 pixels have peak RGB
`<=16`, with dark corners at gray 38 and 68 as well. The earlier 5.2
`RENDER_TEXTURE` submission could paint the repeated dark field opaque;
the pinned S21 callback instead submits it with flag `0x42`.
The earlier in-memory RGBA floor-16 conversion of `lines2` was a 5.2
compatibility experiment with no native SS21 loader evidence and has now
been removed, including its bitmap API. Fresh and resident paths require
the single authored `lines2` material to remain three-component RGB; the
S21 BMD/OZJ bytes remain unchanged. This is not proof that this specific
node caused every pixel of an older screenshot or that the corrected
5.2 framebuffer now matches SS21 ingame.
The earlier rebuilt/staged private Win32 client SHA-256 was
`6B4AEB81B8BDD351AFC76EED13DB267CEEB46423CE085F8F1618BC4891F02006`;
its runtime appearance has not been checked.
The native `0x81CE` initializer at `0x147E153` sets subtype-3 scale
`2.86`, subtype-4/5 scale `4.55`, and snapshots the S21 millisecond clock
at `OBJECT+0xB4`. Native CreateEffect starts alpha at 1
(`0x143E7C0`); the complete subtype-3 and subtype-5 initializer blocks
`0x147E16F..0x147E20C` and `0x147E2F7..0x147E394` do not overwrite it,
while subtype 4 writes 1 explicitly at `0x147E22E`. The previous 5.2
generic bitmap initializer momentarily sets zero, **but its existing
`kPierce81CEEffect` case then sets alpha 1 for all three modes before the
effect is used**. The September 16 claim that Pierce marks were invisible
because of this initializer was wrong. The later generic-alpha change was
behaviorally redundant and has been removed; the complete S21 init blocks
remain hash-pinned, and the source verifier now checks effective alpha at
the end of the dedicated case. Visual parity is still unverified.
Its update dispatcher at `0x15341D6` refreshes
LifeTime 30/20 through a 6000-ms window; subtype 4 derives scale from
`(20-LifeTime)*.05+4.35` and alpha from `LifeTime/20`. The isolated 5.2
effect-pool port now creates the first three `0x81CE` subtypes from the
one-shot Pierce action initializer, uses actor ownership, refreshes their
lifetime from `WorldTime`, and selects `marks_m04` only for subtype 3,
`marks_m03` for 4/5. Render is the scoped terrain-alpha bitmap pass, not
the ordinary model path. Native `0x80BA` subtype 6 is the next direct child:
initializer `0x1471BDD` stores scale 7 and the S21 clock; updater
`0x151F08A` refreshes lifetime 30 until 6000 ms; renderer `0x15A97FC`
submits native bitmap `0x7EF7`. Loader `0x18BD1DA` binds that bitmap to
`Effect\\flare01.jpg`, distinct from `flare01_red` used by an older
Slayer-owned branch. The hash-pinned S21 `flare01.OZJ` input has SHA-256
`874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8`;
the private 5.2 adapter reserves bitmap ID `33014` and spawns subtype 6 as
child four. Native subtype 7 additionally emits three particles per frame;
its three flare objects and nine particles per update are now ported.
Child five is native `0x8149` subtype 2: loader
`0x18D234A` binds bitmap `0x8149` to `Skill\\ground_star.jpg`; parent call
`0x147D840` supplies light `(0.9, 0, 1)` and scale `1.7`. Its initializer
`0x147637E` sets life 50, alpha `0.9` and the current millisecond clock;
updater `0x152BA06` refreshes life only when it reaches two ticks and
expires after 6000 ms. The subtype-2 renderer jumps to `0x15B145C` and
creates a sprite through `0x172760A`, not a terrain tile. The private
5.2 adapter keeps this as a parent-owned effect object and submits a
one-frame `kGroundStarBitmap` sprite during RenderEffects, which is then
consumed by RenderSprites. Sprite material/blend parity still requires
 ingame validation after the whole graph is ported. The subtype-7 particle
 fanout is now ported; owner/class-9 ingame parity remains open.
The three direct `0x80BA` subtype-7 calls at `0x147DB3D/0x147DCFE/0x147DEBA`
each supply light `(0.38, 0.2, 1)`, scale 2 and actor ownership. Their
positions are the root position plus vectors `(-126.71, 73.69, 0)`,
`(134.72, 84.32, 0)` and `(0, -144.92, 0)` transformed by a fixed
`AngleMatrix(180, 0, 0)` at `0x147D4DE`, not by actor yaw. The private 5.2
adapter now creates those three separate positions in call order with the
S21 `flare01` terrain-alpha draw and six-second timer. Native subtype-7
updater `0x151F0D0` additionally creates three particles *per object per
frame* (`0x807E..0x8080`, then two `0x7FFD`). The first lane is now ported:
`0x151F1C8` picks a random smokeline bitmap, subtype 9 and scale 1.3;
`0x1661781` gives it 25 ticks, scale `(55..104)/100 * incoming`, rotation
`0..359`, gravity `(35..38)*0.15`, and the flare object as owner;
`0x16D5AD6` contracts scale `(10..29)*0.001` per tick and follows that owner.
The two `0x7FFD` subtype-17 Clud64 children now follow the smokeline in
each flare update with native lights `(0.2,0.2,1)`/`(0.8,0.8,1)` and scales
`2.3`/`1.3`. The native loader
`0x18BDB86` names `Effect\\Clud64.jpg`; its S21 `Clud64.OZJ` source SHA-256
`64C359CE760E517B1D5201DF10AE3A0EFFCA4F08A3690CAB6FEEF1C34FFE379D`
is copied and registered at private bitmap 33016. Constructor `0x1654E03`
sets life 25, scale `(50..57)/100 * incoming`, rotation `0..359`, gravity
`(35..44)*0.2`, zero initial alpha/light and owner position snapshot.
Updater `0x16C498D` reveals alpha by `0.3/0.4` until full, fades `0.2`
after life below 10, sets RGB from authored light times alpha, contracts
scale by `0.055..0.059`, moves up by gravity and follows the flare owner.
Thus the full three-particle *creation and update graph* is now present in
the 5.2 overlay. This is code/build evidence, not visual parity acceptance.
The next direct parent `0x81CF` subtype 2 is now connected to the root in
its native call order with light `(0.8, 0.6, 1)` and incoming scale zero.
Initializer `0x147E7C4` overrides scale to 1, alpha to 1, sets life 15 and
stamps the millisecond clock. Update `0x1534735` expands scale by 0.15 per
tick, derives alpha from remainingLife/15, refreshes life/scale at one tick
until its 6000-ms cutoff. Render `0x15AE82A` submits `0x81CF` through the
terrain-alpha path with light multiplied by object alpha, using the
hash-pinned `magic_ground12` texture already in the private overlay.
That initializer also creates `0x8012` subtype 17 with light
`(1, 0.28, 0.95)`, incoming scale zero and no owner. S21 loader
`0x18BD19A` names `Effect\\ShockWave.jpg` for bitmap `0x8012`;
its OZJ SHA-256 is
`F8152B6F1A247578AABA09B2FD55B98AB098AADCFD7048616E685E90B1246B7F`.
The subtype-17 initializer `0x145CBEA` overrides scale to 4, sets life 15
and alpha 1; updater `0x14FD824` grows scale by 0.5 per tick and derives
alpha from remainingLife/15, with no refresh clock or owner-follow.
Its effect-object draw branch at `0x159D2BE` submits only on Kalima maps
24..29 and Lost Kalima 36 through nine-argument `0xE2BD4D`.
The existing 5.2 `BITMAP_SHOCK_WAVE` Kalima path also uses
`RenderWaterTerrain` with that argument shape; the private adapter now
registers S21 ShockWave as bitmap `33015`, creates the subtype-17 object
from `0x81CF` and routes only those maps through the scoped 5.2 water
terrain pass. This pass equivalence is an inference from call signature
and map gate, not byte-for-byte recovery or ingame visual PASS.

The native character render/update paths at `0x133F0EA` and `0x13F2546`
compare current action to `0xE4` and assign `OBJECT+0xDC = 0.3`; the
ordinary branch assigns `1.0`. This constant is read as IEEE-754 float
from `0x1B4E6E0`. The 5.2 port applies the same translucency only while
drawing a Slayer actor in its merged Pierce action `286`, restoring the
pre-draw alpha afterward so other 5.2 buff opacity is not clobbered.
The adjacent action `0xE0` branches at `0x133F07D..0x133F0E8` and
`0x13F24E8..0x13F2544` instead set actor alpha to `1-frame/10` while
the actor animation frame is at most `5`, then zero. The dump floats at
`0x1B4E4CC` and `0x1B4E4D0` are `5f` and `10f`. The merged 5.2 Sword
action `284` now gets that fade in the same scoped draw path, not as a
persistent OBJECT alpha mutation.
Those call-site differences are recorded as evidence, but no unobserved
children (`0x681`, `0x688`, `0x693`, `0x694`) are claimed for these Slayer
handlers.

The 5.2 implementation keeps these numeric values as evidence labels only;
it does not copy S21 addresses, hooks, or executable code into the 5.2 binary.
The actual port re-creates the same dispatch/target/effect ownership in the
isolated Slayer overlay and uses hash-pinned S21 Effect assets.

## Secondary effect pool graph

The Slayer roots are initialized by `0x0143E57C`, updated by the 200-slot
secondary-pool traversal `0x01595834 -> 0x014B76C0`, and submitted by
`0x015A0C98`. The decoded table entries below are direct dump facts:

| Code | Initializer | Per-frame update | Renderer case |
|---:|---:|---:|---:|
| `0x679` | `0x148ED2C` | `0x154113C` | `0x15B29DB` (controller) |
| `0x67A` | `0x148EE3E` | `0x1541E69` | controller |
| `0x67B` | `0x148EEC7` | `0x1542273` | controller |
| `0x67C` | `0x148F0DF` | `0x15430A6` | controller |
| `0x67D` | `0x148F0F1` | `0x1543173` | controller |
| `0x67E` | `0x148FBBD` | `0x1543335` | controller |
| `0x680` | `0x148FE79` | `0x154369F` | controller |
| `0x681` | `0x148FECD` | `0x154371B` | `0x15B29DB` (controller) |
| `0x682` | `0x14903ED` | `0x1543B9F` | `0x15B29FD` (controller) |
| `0x683` | `0x1490741` | `0x15442BB` | `0x15B29FD` (controller) |
| `0x684` | `0x1490765` | `0x1544771` | `0x15B29FD` (controller) |
| `0x688` | `0x1490F18` | `0x15458EB` | `0x15B2A02` (model submit) |
| `0x689` | `0x1490F7A` | `0x1545A08` | `0x15B2A1F` (controller) |
| `0x68A` | `0x1491051` | `0x1546709` | `0x15B2A1F` (controller) |
| `0x68B` | `0x14913C9` | `0x15473A9` | controller |
| `0x68C` | `0x1491645` | `0x15475B8` | controller |
| `0x68D` | `0x1491669` | generic/no-op | controller |
| `0x68E` | `0x1491AD8` | `0x154795F` | controller |
| `0x68F` | `0x1491AEA` | `0x1547BB4` | controller |
| `0x690` | `0x1492040` | `0x1547E0D` | controller |
| `0x691` | `0x149221A` | `0x1548372` | `0x15B2A24` (model submit) |
| `0x692` | `0x1492364` | `0x154856F` | `0x15B2BC5` (controller) |
| `0x693` | `0x149266B` | `0x1548B7B` | `0x15B2BC5` (controller) |
| `0x694` | `0x149267D` | `0x1548D30` | `0x15B2BCA` (model submit) |
| `0x695` | `0x149289F` | `0x1548F0C` | `0x15B2D52` (controller) |
| `0x696` | `0x1492B54` | `0x1549518` | `0x15B2D52` (controller) |

Detection root `0x692` has exact initializer children `0x693`, `0x694`,
`0x81CF` modes 4/5, `0x7FDD` mode 2 and `0x82F6`. Demolish root `0x695`
has exact children `0x696`, `0x694` modes 0/1, `0x7F78` mode 8 and
`0x82F6`. Their per-frame cases are paired implementations gated on native
owner actions `0xE8/0x142` and `0xE9/0x143`; each emits four `0x7F5A` or
random `0x807E..0x8080` smoke records and a `0x691` model child. Children
`0x693/0x696` emit `0x678` (`Bat_van01`) in subtype 1/4 respectively.
Although the supplied `player.bmd` clips for native actions `0xE8/0xE9`
are byte-identical, those owner-action compares are distinct. The private
5.2 player now appends both S21 clips to IDs `287/288` rather than aliasing
Demolish to Detection and allowing their lingering roots to react to the
other buff cast.
The root smoke windows (`0x15486A4..0x154873F`,
`0x1549041..0x15490DC`) read the effect's saved `OBJECT+0x1D0` cast
position rather than the moving owner position. The paired root branches
at `0x15485CD..0x1548619` and `0x1548F6A..0x1548FB6` first choose the
smoke family with `rand() & 1`, then call `0x1267C3C` independently for X
and Y using `Random(-30, 30, 1)` (`0x1B4E4D8 = 30.0`). The first port's
fixed-radius/yaw circle changed the S21 distribution and consumed random
values in the wrong order; both private branches now use the decoded
square offsets and family-first order. The `0x693/0x696`
update windows (`0x1548BAC..0x1548C31`, `0x1549549..0x15495CE`)
also create their `0x678` children at saved cast position plus 100 Z.
The paired per-frame `0x691` child uses the saved root position and scale
`0.2` (`0x1B4DF08`). Its angle-Z branch at `0x1548A57..0x1548AE9` and
`0x15493F4..0x1549486` selects inclusive `Random(0,360,1)` on even
remaining life and negates inclusive `Random(1,360,1)` on odd life. The
first port omitted the upper endpoint and allowed zero on the negative
branch; both private buff roots now preserve the decoded ranges.
All four cases delete their effect when the owner handle is invalid.
The initializer argument windows at `0x14923BB..0x149251A` and
`0x14928F6..0x1492B47` show that `0x693`, `0x696`, and Detection's two
`0x81CF` modes begin with incoming scale zero. `0x694` uses scale 1 (and
Demolish's second mode uses `.5`). The color/scale constants for the
Detection blue flare and both buff rings were read as IEEE-754 values from
the pinned mapped image, rather than inferred from screenshots.

Full basic-block disassembly of native `0x694` initializers at
`0x14926CC..0x14926EA` and `0x1492797..0x14927B5` shows raw incoming
scale written to `+0xA0`, then `xorps xmm0,xmm0` before `+0xDC`:
**initial alpha is zero in both modes**. The same pattern occurs in
native `0x691` subtype 0 at `0x1492259..0x1492277` and Bat Flock
trail `0x688` subtype 0 at `0x1490F57..0x1490F75`. Thus Detection/
Demolish `0x694` scale `1.0`/`.5`, buff-wave `0x691` scale `.2`, and
Bat-trail `0x688` scale `.85` all start at alpha zero and fade in via
their separate updaters. A filtered disassembly print omitted the
intermediate `xorps`, causing three incorrect scale-to-alpha commits
(`c8d87fb`, `d5256c3`, `a6aaf80`). This corrective implementation
restores zero alpha, and the dump verifier pins the entire scale/clear/
alpha write sequence. The black-field artifact remains a separate
ingame parity question; neither static proof nor the correction claims
visual PASS.

Full disassembly of S21 random helper `0x1267C3C` shows unit-step
bounds converted with `cvttss2si`, a width increment, then one integer
modulo. Bat Flock model `0x678` passes step `1` at its scale/motion
initializers (`0x148E5ED` and paired branches) and subtype-0 yaw
updater (`0x1540192`). Its root spread, red joint and orbit-child
branches use the same helper/step (`0x1543C4C`, `0x154435F`,
`0x154479D`). Sword's one-in-six `0x691` mark yaw also passes step
`1` at `0x1546CC1`. The first 5.2 port sampled those ranges as
continuous floats. The isolated graph now samples inclusive integer
ticks for these decoded ranges; it still uses the 5.2 private RNG, so
this is distribution/endpoint fidelity, not protected-PRNG seed parity.
The native main-pulse block `0x1543C4C..0x1543CC9` calls Random for
X, stores vector index 0, then calls it for Z, storing index 2. The
5.2 `Vector` helper is an inline function, so nesting both RNG calls
as function arguments could reverse consumption on Win32. Both Bat
spread helpers now sample explicit X and then Z locals before calling
`Vector`; this preserves native draw order even when the underlying
private RNG sequence differs.

Detection's minimap reveal is separate from the `0x692` cast graph. The
Webzen Slayer guide states that nearby life forms are marked on the minimap
for a duration, but `0x12D139B..0x12D143E` only creates the root visual.
The 5.2 GS now sends a private `0xF4:E4` buff-start signal only after its
authoritative 60-second effect is accepted; only the local Slayer receives
it. The full minimap renders live monster/player positions from the current
5.2 character pool while that server-supplied result is active and clears
them on expiry, death, login reset, or map change. This is a functional
5.2 porting adapter, not recovered S21 minimap renderer/radius parity; the
exact S21 reveal scope is still unverified.

The upgrade-only Pierce root `0x679` is not a generic target burst. The block at
`0x154117D..0x15411DB` restores its position from the saved launch vector;
`0x15411DF..0x15411E2` compares the root effect's own animation frame
(`OBJECT+0xD0`), not the owner frame, to `4.0` and emits the center
`0x67B/0x681` pair plus a `+90` degree, 200-unit flank containing
`0x67A/0x67C/0x681`. The block at `0x15418B3` compares the same root frame
against `7.0` and
emits the mirrored `-90` degree flank. This replaces the earlier inferred
`3/5/7` gates.
Native `0x148EE0F..0x148EE2F` also copies player action `0xE0` PlaySpeed
into this root's `EFFECT+0xBC`. The 5.2 `0x679` initializer now uses its
imported Sword action `284` velocity (`.43 + AttackSpeed*.002`) instead of
the generic `.4`, so those effect-frame `4/7` gates advance at the native
action rate. This is static timing parity, not an ingame cast PASS.
Unlike `0x689` lane children, the `0x679` flank creation windows at
`0x1541607..0x1541619` and `0x1541AFB..0x1541B0D` dereference
`EFFECT+0x34C` and read the **owner actor's** `+0xA0` scale for their
`0x67A/0x67C` children. The 5.2 root keeps actor scale on these flank
models while using effect scale only for `0x689` lane models.

Bat child `0x685` has a 60-tick initializer with scale `40..50` and signed
rotation step `-15..15`. Its update rotates for three ticks in each direction,
advances by `scale/3`, and emits exactly three sequential `0x82E8`
(`firehik01_magenta`) particles with light `(1,.8,.2)`. Both initializer
branches of child `0x686` loop exactly three times when creating subtype-3
`0x678` records.

The `0x678` Bat model is not a target-attached placeholder. Its five native
subtypes use separate free-flight, accelerating, target-homing and owner-homing
branches. Initial scale is caller scale times a random `.60..1.20` envelope;
subtypes 0 and 2 fade in the final life third; subtype 0 also shrinks by
`.005` per tick. Subtype 2 checks expiry against the real target within 50
units, then aims from the bat XY projected to terrain height toward the real
target position (`0x1540601..0x154091B`). A missing target handle retains the
existing heading and still moves/emits its trail; it does not snap to a
guessed target-height offset. The initializer at `0x148E57D..0x148ED27`
samples floating-point ranges rather than discrete integer steps: all modes
scale by `.60..1.20`, mode 0 speed is `50..100 * .05`, modes 1/4 use
`50..100 * .025`, and mode 2 specifically uses `80..100 * .5` (not `50..100`).
Root `0x683` at `0x15442BB` emits one `0x82F3` subtype-1 joint at its first
update, then gates later emissions at `1/15`; its position is root position
plus a normalized random X/Z spread independently scaled by `[0,160]`,
then `+90 Z`, with Angle Z perturbed `[-15,15]`. It does not attach the root
position to target `+35 Z`. Its joint owner comes from a randomly selected
entry of the S21 multi-target list (`OBJECT+0x1DC/+0x1E4`), or is null when
the list is empty. The canonical `0x12D10A0` cast roots use that empty-list
branch in both S21 and this 5.2 port. The 5.2 port now carries supplemental
list-bearing effects and server-authoritative multi-enemy strikes via private
fanout adapters; exact S21 GS target-selection geometry remains unavailable.
Root `0x684` at
`0x1544771` uses a separate `1/15` gate, an X/Z spread with nonnegative
initial Z, the same `+90 Z` and angle perturbation, and creates `0x685`
with null owner and zero incoming scale.
Main root `0x682` at `0x1543B9F` gates on positive `LifeTime % 5 == 0` or
an independent `1/20` pulse. Each pulse normalizes signed random X/Z,
scales its axes independently by `[40,90]`, rotates by root Angle,
adds root Position and `+90 Z`, then perturbs Angle Z by `[-15,15]` for
the `0x678` subtype-2 model. That child owns the `0x682` root, not caster,
and uses scale `2.5` and white light. The same launch position emits two
particle-pool records using the unperturbed root Angle: `0x810B`
(`pin_star02_red`) with light `(.5,.5,.5)` and scale `2 * [.8,1]`,
then `0x8070` (`empact01`) with light `(.65,.65,.65)` and scale
`.26 * [.8,1]`. Both texture names were resolved through the S21 loader;
hash-identical copies are in the private Slayer overlay
(`pin_star02_red` is absent from the 5.2 base Effect tree), not substitutes.
`0x686` subtype 1 gates its randomized `0x687`/`0x8070` burst at `1/15` and
its `0x82EF` joint at `1/8`; `0x687` renders `0x82F0` and `0x82F1` at `.6` and
`.8` of the incoming scale.

The subtype-2 `0x678` branch also has an independent `1/2` particle gate at
`0x1540ADE`: light `(.5,.05,0)`, subtype 2, scale `2`, bitmap `0x8020`.
The S21 bitmap loader at `0x018BE786` maps that id to
`Effect\\WATERFALL4.jpg`; the hash-identical `waterFall4.OZJ` is pinned in the
isolated Slayer overlay instead of substituting a visually similar texture.

Bat subtype-1 initializer `0x148E895..0x148E907` also allocates an
`0x80E3` subtype-6 joint at `40 * batScale` with the bat as owner. S21
loader `0x18D1C19` maps `0x80E3` to `Effect\\force_Pillar.jpg`, and the
5.2 base `force_Pillar.OZJ` is SHA-256-identical to the S21 asset.
The S21 constructor `0x15DEED8` assigns 50 tails and five life ticks;
the subtype-6 updater `0x1613393` expires if its owner is no longer live,
otherwise copies the bat position. A Slayer-guarded 5.2 joint branch now
implements those rules and bypasses the pre-existing dummy-model-only
`BITMAP_FORCEPILLAR` update. The generic force-pillar joint remains untouched
outside that subtype/owner combination.

Native `0x678/0x688/0x691/0x694` renderer dispatches at
`0x15B29BE/0x15B2A02/0x15B2A24/0x15B2BCA` all pass six default arguments
to `0x176D621`, the generic object Calc/Draw wrapper. They do not request
an extra model draw at this dispatch call site; the registered handler can
still submit additional passes. The isolated 5.2 renderer keeps
`Calc_RenderObject` for animation, light and origin, then brackets each
required `RenderBody` pass under `BeginRender/EndRender` and the shader
scope. The earlier direct `RenderMesh` loop skipped that scope.
The registered one-mesh `0x678` handler `0xA517D5` draws subtype 0/1 with
base flag `2` only. Subtypes 2/3 draw that base pass plus flag `0x42`
(`RENDER_TEXTURE|RENDER_BRIGHT`) using bitmap `0x82F9`, loaded at
`0x18C04FD` from `Effect\\bet_grilsshot2red.jpg`, with blend-light `.6`.
Subtype 4 swaps only the overlay to bitmap `0x82FA`, loaded at `0x18C067C`
from `Effect\\bet_grilsshot2gold.jpg`, with blend-light `1`. Both overlay
calls target mesh zero and blend-mesh zero, not all meshes by a generic
whole-body bright pass. The gold OZJ is hash-pinned
`9CA43326D7261352C245DA4BE4C0D45272E026C7060D321B966753C5EAD96269`
and staged to the private Slayer overlay; isolated bitmap 33017 is reserved
before the unnamed allocator. The 5.2 model path now matches these base/
overlay passes for subtypes 0..4. The same handler transforms the zero vector
through authored bat bone 7 (`0x132F433`, bone-matrix index `7`) after drawing
and submits one sprite in each subtype 1..4 through `0x172760A`. Subtype 1
uses bitmap `0x7FDD` (`Effect\\flareBlue.jpg`) at scale `.4` and white light;
subtype 2 uses `0x7FE0` (`Effect\\flareRed.jpg`) at `.6` and white light;
subtype 3 uses the same red bitmap at `.3` with a shared RGB factor
`rand()%20/25 + .2`; subtype 4 uses `0x7F78` (`Effect\\Flare.jpg`) at `.6`
and white light. S21 loader paths/IDs and the new `flareRed.OZJ` input SHA-256
`FC7B772D1B8685B5B89989838431616D19224E321A1492CC95F9B5369B794A48`
are pinned. The isolated 5.2 renderer now follows bone 7 and sends those
four sprites to its sprite pool. Native `0x172760A` forwards two extra
arguments beyond the seven exposed by 5.2 `CreateSprite`: the first is
mask `4`, stored at sprite `+0x8C` and used by `0x18E8B0C` to rotate around
Z; the second is a default triple `(1,1,1)` from `0xA13A00`, stored at
`+0x90/+0x94/+0x98` for the full UV atlas. All these handler calls supply
rotation zero, so the 5.2 Z-angle zero and full-UV quad cover their active
values. The native sprite subtype zero selects `GL_ONE/GL_ONE` at
`0x1726F17 -> 0x18E7137`, matching 5.2 `EnableAlphaBlend` for subtype zero.
Native `0x1726F76..0x1726F8C` retires sprites after render passes 0/2,
matching 5.2 `RenderSprites`' one-frame retirement for those passes; the
native animation-frame increment is `.1`, the same 5.2 factor at ordinary
frame rate. These source checks narrow the adapter gap; visual parity still
awaits the later ingame gate.
Full basic-block decoding of `0x1887EB0` shows the **fallback** native body
flag is `2`. This is conditional: `0x1887DDB` first calls special-model
manager `0x18917BA` and skips the fallback if it returns true; `0x1887E1C`
then tests a second custom-draw registry through `0x18A1EF9` and may skip
the fallback after its dynamic draw. The dump alone does not prove every
instance of `0x678/0x688/0x691/0x694` actually takes the flag-2 branch.
The second registry does register `0x688` at `0xA1B9B6` with handler
`0xA52E26`, `0x691` at `0xA1B9E9` with `0xA52FEC`, and `0x694` at
`0xA1B9FA` with `0xA53114`. All three handlers multiply each model-light
RGB component by `OBJECT+0xDC` alpha and submit flag `0x82`
(`RENDER_TEXTURE|RENDER_DARK`), returning handled. Native `0x1331F5D`
tests bit `0x40` for bright and bit `0x80` for dark; the latter calls
`0x18E71C3`, which sets `GL_ZERO/GL_ONE_MINUS_SRC_COLOR`. The `0x694` handler
does this in both subtypes. The first manager may still intercept a given
instance; these registered paths nevertheless disprove the prior inference
that these models use the ordinary flag-2 pass. The
registered `0x688` handler at `0xA52E35..0xA52E3B` and `0x691`
handler at `0xA52FFB..0xA53001` submit their dark mesh only when
`OBJECT+0x60` subtype is zero; other subtypes return handled without a
model draw. The earlier 5.2 adapter drew both models unconditionally.
The isolated adapter now applies this native subtype gate before `RenderBody`.
`0x694` is different: it explicitly draws subtypes zero and one at
`0xA53123..0xA5312D`. This removes a source-proven extra dark submission,
but does not prove the remaining screenshot black pixels match S21 ingame.
The native alpha
allocator at `0x143E759..0x143E797` sets HiddenMesh/BlendMesh to `-1`,
blend-light to `1`, and both blend-UV offsets to `0`. The ordinary draw
call at `0x1887E5E..0x1887EBA` forwards those fields plus `OBJECT+0xDC`
alpha and flag `2`; this is a fallback comparison, not the active 5.2
Bat/buff draw contract. Missing default BlendMesh state is therefore
not an evidence-backed explanation for the black funnel. The native alpha
helper `0x18E709C` enables `GL_BLEND` with `GL_SRC_ALPHA` /
`GL_ONE_MINUS_SRC_ALPHA`; `0x18E7137` is the separate `GL_ONE`/`GL_ONE`
bright helper. For registered `0x688/0x691/0x694`, the authored dark pass
uses RGB light times alpha to fade, not a texture-alpha mask. A read-only
structural parse of the pinned S21
BMDs, checked against the staged v0C imports, finds `0x694`
`Van_object04_skill` has two authored **flat** meshes (all vertex Z=0),
26 animation keys, and seven bones whose X/Y rotation channels remain zero
(position offsets are at most 0.000023). `0x691` `van_object03_skill` has one static 248-vertex mesh
whose vertex Z reaches 331.943. The tall black funnel in the older 5.2
preview therefore cannot be attributed to `0x694` model geometry; `0x691`
is a plausible component, but no per-mesh framebuffer attribution proves
the exact culprit yet. This does not justify cutting more dark pixels from
the S21 textures by eye.
Read-only BMD UV inspection pins `0x691`'s `Elite_monster_ground02` mesh to
U `0..1`/V `.0211..9682` and both flat `0x694` materials (`ark`,
`empact01`) to full `0..1` UV. Hash-pinned OZJ/JPEG analysis finds the
previous experimental alpha key removed 57,258/65,536 texels from the silver
`Elite_monster_ground02`, 13,343/16,384 from `ark`, and 48,046/65,536
from `empact01`; only 859 silver texels and 4,668 `empact01` texels remain
in the dark `17..48` band. These are **authored SS21 model/texture fields**,
not proof of which mesh wrote the black pixels in the old 5.2 screenshot.
Those cutoff calculations are retained for audit only; the active
`0x688/0x691/0x694` model path again loads the original S21 RGB JPEGs.
5.2's unknown-model `Draw_RenderObject` default is opaque. A bounded QA
isolation showed the white buff vortex remained when `0x694`, `0x678`, the
Slayer bitmap objects and Slayer-owned particles were withheld, disappeared
when the entire S21 buff root was withheld, and disappeared again when only
`0x691` (`van_object03_skill`) was withheld. The asset and spawn are S21;
the white block implicates the old port's `0x691` color/fade path, not a
legacy 5.2 graph. In 5.2 `RENDER_BRIGHT` uses `GL_ONE/GL_ONE`, so merely
passing the authored Alpha to `RenderBody` did not fade repeated `0x691`
children. The isolated renderer now sets the model RGB light to
`effect.Light * effect.Alpha` before the subtractive dark draw, as the registered S21
handlers do. The experimental RGBA key and textured-alpha pass for
`0x691/0x694` have been removed.
The S21 `0x691` updater at `0x1548395` shifts the integer 35-tick life
right by two, so the subtype-zero alpha window is exactly 8 ticks, not
the old port's floating 8.75. Its nonzero subtype jumps straight to the
case exit at `0x15484D3` and has no separate fade. The `0x694` updater
at `0x1548D5D/0x1548DE0` similarly divides life by two in integer
registers: subtype one's 35-tick window is 17, not 17.5. The isolated
port now preserves these windows. This is a dump-backed fade correction,
not framebuffer evidence that the previously black buff vortex is gone.
Shared native update exit `0x1594624..0x1594636` checks
`EFFECT+0x6C LifeTime <= 0` and calls destruction only after the model
updater; the ordinary final decrement is later at
`0x15946E3..0x15946ED`. The prior isolated 5.2 bridge decremented first
and destroyed at zero in that same update, dropping a live/renderable
zero-life frame from every Slayer effect. It now checks before decrement,
matching this native order. This is lifecycle parity, not framebuffer
proof that the old black vortex has disappeared.
The same integer-window audit finds `0x678` bat subtype 0 at
`0x154011A` and subtype 2 at `0x1540607` each execute `IDIV 3`
before alpha fade. Their 40/3 and 50/3 windows are therefore 13 and
16 ticks rather than the port's former floating 13.33 and 16.67.
Both bat-flight paths now use that decoded divisor; this still requires
later ingame visual acceptance.
The shaped `0x678` bat's registered
model passes and bone-7 sprite branch are now ported below; its 5.2 sprite
API adapter still needs visual acceptance.
The `0x688` Bat trail is a separate three-mesh ring (`marks_m03.jpg`,
`empact01.jpg`, `macardkmono.jpg`), not the shaped bat. The imported v0C
plaintext is byte-identical to the hash-pinned S21 BMD, and all three meshes
map a full `0..1` UV square. Native `0x688` starts at `Alpha=0` and has a
fade-in/out curve; the old 5.2 additive `GL_ONE/GL_ONE` pass discarded that
alpha. The port now applies the registered dark RGB pass with light
attenuation, not the brief textured-alpha compatibility experiment.
Hash-pinned RGB audit finds `marks_m03` has 48,703/65,536 texels at peak
`<=16`, while `macardkmono` has only 222/32,768 there; these field counts
are **not** a current alpha-mask configuration or framebuffer proof.
In the earlier private MainRF fixture, Detection/Demolish captures
show the silver vortex over an intact terrain tile instead of an opaque
black/white block; these captures preceded the registered-handler correction
and do **not** validate the newest build or Slayer class-9 acceptance.
The older extracted `Media1_20260914/frame_019.jpg` shows red attack
fragments, **not** a dark buff vortex. The earlier frame-19 attribution was
incorrect and is withdrawn. The current `C:\Users\DELL\Desktop\Media1.mp4`
(SHA-256 `CF1C58E19DFDAF968D72F2095CC85D091DA4CC2506B8C980681A57E302F78DDE`)
is a 24.31-second, 10-fps **5.2 MainRF** recording created on 15 September,
not the SS21 capture from which the older frame set was extracted. The old
frame set has no retained source-video hash, so it is observation only,
not authoritative current-video provenance. The former `0x5D8` lines2
color key was a 5.2 adapter and is withdrawn; RGB/native material parity
is still **not a full ingame PASS**.
Body light is multiplied by alpha once for registered `0x688/0x691/0x694`
dark model draws. Every diagnostic skip
used to establish causality was removed from code and the private QA client.

Sword child `0x68D` has two native branches. The base branch used by the 5.2
skill emits `0x82F5` scale `.2`, `0x82F8` scale `.45`, and `0x80BA` subtype 1
scale `2`. The four-sprite branch (including `0x8101`) is gated by S21 master
skill `0x13B`; that unavailable master upgrade is deliberately not injected
into the base 5.2 skill.

Sword root `0x689` is also split between the base skill and S21 skill `0x817`.
The base one-shot anchors `0x68D` at the resolved target plus `100 Z`, derives
the caster-to-target yaw, moves the launch point back `100` units, and creates
exactly one `0x68B` subtype 0, one `0x68E` subtype 0, then `0x68C` another
`100 Z` above that launch point. The `0x686` bat burst and its `1/3` gate belong
to `0x817`, so they are not emitted by imported skill 292. In `0x68B`, the
random `0/180/90` lane changes Angle Y; it does not translate Position X.

Model `0x681` is not a missing visible mesh: the supplied
`van_swordline01.bmd` intentionally has zero meshes, four bones, one action
and seven keys. Update `0x154371B` animates it and uses bones 3 and 1 as the
two blur endpoints. Subtypes 0..2 select native bitmap `0x82EC`
(`Effect\\blur02_mono_long_van.jpg`) and subtype 3 selects `0x82F4`
(`Effect\\blur02_mono_long_van2.jpg`); both exact OZJ files are hash-pinned.
The per-frame block computes its ribbon frame as elapsed lifetime times the
copied action speed, then advances by speed divided by the seven key samples.
Initializer `0x148FFEE..0x14902D7` copies player action `E0/E1/E2`
`PlaySpeed` into both model `0x681` action 0 and effect `+0xBC` for
subtypes 0/1/2 respectively. These two flank action speeds are used only
as source metadata; no extra player clip is needed in the private 5.2 BMD.
Subtype 3 instead reads its owner's **current action index** at
`0x1490304..0x14903E2`, resolves that index in the global player-model
action table, and copies its `PlaySpeed` to the same fields. Its owner
may be the preceding `0x68B` effect rather than the caster actor.
The previous fixed `.35` setting for all `0x681` modes was removed.
Subtypes 0..2 keep alpha 1 through frame 5, then use `1-frame/keyCount`;
subtype 3 retains its initialized alpha 1. Initializer modes 1 and 2 add a
uniform `140..160` and `60..80` degrees respectively to Angle Y. The earlier
5.2 frame-from-`AnimationFrame` and lifetime-ratio alpha substitute did not
match these decoded blocks and has been removed.
The 5.2 blur pool therefore accepts an explicit bitmap for this isolated path
instead of substituting `BITMAP_BLUR + Type`. Controller `0x68C` also stays at
the endpoint supplied by `0x689` and emits randomized `0x8149`
(`Skill\\ground_star.jpg`); it does not reattach itself to the selected target.

Sword children `0x68F` and `0x690` preserve the native launch origin,
direction and distance fields. Their update step is
`Distance/(InitialLife+1)`; `0x68F` uses three-tick subtype ranges and the
native `.3`/`.5` alpha envelope, while `0x690` emits its two independently
randomized smoke records only after movement. Model children `0x688`, `0x691`
and `0x694` use `OBJECT+0xDC` as alpha/light multiplier and `OBJECT+0xA0` as
render scale; the earlier interchange of these fields has been removed.

`tools/slayer/inspect_s21_effect_graph.py` reproducibly decodes these three
dispatch layers from the pinned mapped image and can print an individual case
window. It is reverse evidence, not runtime acceptance.

The root initializer call sites also distinguish two separate S21 allocation
pools. At `0x1491122/0x14911C4/0x1491266`, the shared `0x68A` root
(Sword upgrade `0x818` or base Pierce `0x126`, not base Sword) creates bitmap
objects `0x80BA/0x82F6/0x82F7` through `0x143E57C` (effect-object pool),
not `0x1724176` (particle pool). Detection `0x692` and Demolish `0x695`
likewise create `0x81CF/0x7FDD/0x7F78/0x82F6` through the effect-object
pool. The isolated port now allocates those six node kinds via `CreateEffect`
and gives them their own bitmap renderer. `0x82F6` subtype 0 initializer at
`0x1492857` sets lifetime 30, incoming scale, and initial alpha zero;
`0x82F7` subtype 0 at `0x149227C` uses lifetime 40. The root bitmap-object
update and terrain draw paths below are decoded, but the wider child graph
and ingame fidelity gate remain open. A compile or source-surface PASS does
not close that gate. The `0x80BA` occurrence inside Sword child
`0x68D` is different: `0x149185E` calls the *particle* allocator, so that
specific occurrence remains a particle.

The allocator signature stores its sixth argument at `OBJECT+0x34C` as
owner. Sword's `0x80BA/0x82F6/0x82F7` and the Detection/Demolish flare/ring
objects pass zero; Detection's `0x81CF` subtypes 4/5 pass the root itself.
Detection `0x693` and Demolish `0x696` pass the caster owner, while their
`0x694` mark/impact model passes the root. A 5.2 node must not chase the
caster/root position unless its own update code does so. Native `0x81CF` subtypes 4/5
(`0x147EA05/0x147EA5E`) start with alpha 1, lifetime 30 and incoming scale;
the other root bitmap nodes above start with alpha zero. The previous generic
quarter-life fade was not dump-backed and has been removed. The native
secondary effect-object update switch starts at `0x14B7B60`: `0x82F6 →
0x1548E75`, `0x82F7 → 0x15484D8`, `0x80BA → 0x151EC85`, `0x81CF →
0x153467D`, and `0x7F78 → 0x14FEB9E`. Root subtypes `0x82F6/0x82F7`,
`0x80BA` A, and `0x7F78` 8 increment alpha by `1/(LifeTimeInit/2)` while
life is in its first half and decrement it by the same amount in the second
half. `0x80BA` A also turns Angle Z one degree per update. `0x81CF` subtypes
4/5 start at alpha 1, decrement by `1/30` and grow scale by `.3`/`1` per
update. These are now the 5.2 bitmap-object update rules. `0x7FDD` subtype 2
is absent from that *low-code* dispatcher because it uses the **high-code**
effect-object chain: initializer selector `0x143F30D` → subtype-2 block
`0x1481E34` (30-tick lifetime, raw child scale, Angle Z/alpha zero), updater
selector `0x1574C55` → `0x157D57E` (alpha rises by `1/15` while remaining
life exceeds 15, then falls by `1/15`; Angle Z gains 5 degrees each tick),
and renderer selector `0x15A1559` → subtype-2 `0x15AD0DE` (RGB × alpha,
terrain-alpha draw at `0x15AD22D`). The previous 5.2 case initialized alpha
zero but omitted this updater, leaving Detection's blue flare invisible.
Native LifeTime loses one tick per update; the 5.2 Slayer effect path loses
`FPS_ANIMATION_FACTOR` per update, so its alpha and 5-degree rotation deltas
are multiplied by that same factor to keep the phase/lifetime ratio.
This recovered branch is now ported and dump-byte pinned; it is not evidence
of full ingame visual parity.

The `0x82F6` object renderer case `0x15B2BE7` makes one `0x1765DF1`
draw call. The separate xref at `0x15B7781` emits bitmap `0x82F6` from a
different object case; it must not be copied as a second pass on the
`0x82F6` object. `0x1765DF1` iterates terrain tiles using XY position,
size, color, rotation and height in the same order as 5.2
`RenderTerrainAlphaBitmap`. The six imported bitmap-object subtypes use
that API, not billboard `CreateSprite`; `0x82F6`/`0x81CF` pass zero
rotation and the root subtypes of `0x80BA`/`0x82F7`/`0x7FDD` pass Angle Z.
The root calls pass alpha 1 and terrain height 5. S21 `CreateEffect`
normalizes all non-positive incoming effect scales to `0.9` at
`0x143E7E1..0x143E80D`. This is only the allocator's initial value:
`0x691` subtype 0 at `0x1492259`, and `0x81CF` subtypes 4/5 at
`0x147EA2A/0x147EA83`, immediately restore the raw scale argument.
Their scale-zero seeds therefore **start at zero and grow** in update;
forcing `0.9` there creates oversized initial geometry. Detection child
`0x693` and Demolish child `0x696` are only 30-tick controllers and keep
the allocator's `0.9` fallback. The neighboring `0x1492B85` constant
`1.0` store belongs to native `0x697`, not Demolish's `0x696`; `0x697`
is created by independent non-Slayer call sites near `0xE8A20A` and is
outside this import. The 5.2 port passes the raw argument into its
initializer and applies only the confirmed subtype-specific overwrites.

Bat Flock also creates **joint-pool** entries, not billboard/particle substitutes:
`0x82EF` (`gostmark02`) and `0x82F3` (`gostmark02_red`). Native constructor
dispatch `0x15C7692/0x15C76A2` routes them to `0x15E00D4/0x15E0780`.
`JOINT+0x68` is `NumTails=-1`, `+0x6C` is `MaxTails` (10 for `0x82EF`, 5
for `0x82F3`), `+0xA00` is LifeTime (15..29 and 30), and `+0xA50` stores
initial life for the half-life fade. The 5.2 private joint cases now
initialize these fields and bind the two authored textures instead of
falling through its uninitialized default. Native updater
`0x15E5E41/0x15E5E51` routes to `0x1615406/0x1615A15`; 5.2 now handles
owner expiry, motion, tail creation, and a render-time half-life color
envelope because its JOINT has no separate alpha field. This is a porting
step, not an ingame fidelity PASS.

The native receive bridge is fail-closed on class identity: a normal build
accepts only S21 class 9.  The isolated QA fixture may bypass that gate only
when `SlayerAutoQA.enable` is present, and records the legacy fixture class in
`SlayerRuntimeQA.log`; this does not claim that 5.2 class-9 persistence or
protocol migration is complete.

The hash-pinned private asset converter now inspects the decrypted S21 BMD
mesh texture filenames rather than assuming its texture list is complete.
The five imported models reference `bet_grilsshot2`, `empact01`,
`macardkmono`, `marks_m03`, `ark`, and `Elite_monster_ground02`; all map
to pinned `.OZJ` inputs in the isolated Slayer manifest. This is a
read-only provenance/asset-coverage gate before model loading, not proof
that those textures draw correctly ingame.

The GameServer `RunningSkill` path applies the same fail-closed rule before
dispatching any of the five imported rows, including direct packets that skip
the normal learned-skill checks.  The server catalog still leaves the legacy
seven-byte `RequireClass` field untouched until a complete class-9 ABI map is
available.

To reproduce the provenance check from the Slayer worktree, run:

```powershell
python tools/slayer/verify_s21_dump_chain.py
python tools/slayer/verify_port_surface.py
```

The checker verifies the pinned dump hash and the five dispatcher/action-switch
windows.  A PASS here proves the source mapping is present in the captured
dump/disassembly; it is not a substitute for a live server cast or owner
visual acceptance.

The pinned S21 SkillList Brand chain also maps raw 779/780 to Sword Inertia,
787/788 to Demolish, and 794 to Detection. Merely loading their metadata
previously allowed 5.2 to draw a base Slayer graph for a cast its GS does not
handle. The isolated client now permits raw casts only for the five base
skills and Bat Flock mastery 781/782. Its input, movement, 0x19 receive, and
Bat fanout routes reject the other exclusive 779..794 rows before any local
graph or actor state is changed. This prevents a false preview; it does not
claim those S21 mastery upgrades themselves have been ported.
The client demand path also checks the same pinned level/STR/DEX rows used by
the GS before the legacy Energy-zero shortcut; Bat 781/782 use their native
level-160, STR-100, DEX-380 row. It cannot replace the GS's class-stage,
mastery-point or target validation, so local rendering is still not a server
acceptance proof.

The isolated Slayer client project now disables its legacy post-build
`xcopy` commands whenever `SlayerIsolatedBuild=true`. Those commands target
the shared `D:\RISE-CrossPlatform\Client\Data` tree rather than
`Tests\SlayerBuild`, so normal/QA Slayer builds must not execute them.
The private S21 asset converter and stage checker populate only the
dedicated Slayer test client. This also avoids concurrent build races in
the shared asset destination.

`verify_port_surface.py` is the companion static check for the 5.2 overlay: it
requires every imported ID to be wired through the shared contract, server
catalog/handler, client receive/use route, native runtime/resources, and packet
row.  It likewise does not claim legacy class-9 persistence or ingame approval.

Texture-loader boundary: RISE 5.2's unnamed material IDs begin at `33001`,
while its fixed loader textures begin at `33100` and run through `35100`
(`_TextureIndex.h`). The isolated Slayer branch reserves `33001..33011`
for imported effects and starts unnamed material allocation at `33012`.
The old sequential allocator would reach fixed ID `33100` after only 88
material loads, potentially aliasing a character material and a named
effect texture. `GlobalBitmap::FindAvailableTextureIndex` now skips the
compiled fixed-loader block and any already loaded ID for Slayer builds;
non-Slayer builds retain their original path. All 39 staged Slayer OZJ
files also decode as JPEG after the same 24-byte offset used by
`OpenJpeg`/`OpenJpegTurbo`. These static checks narrow the black-texture
failure modes, but whether the reported ingame symptom is gone still
requires the later isolated runtime QA gate.

The September 15 owner preview exposed a separate black-background path.
`RenderEffects(false)` dispatched the imported Slayer bitmap objects in its
ordinary model pass, but 5.2 `RenderTerrainAlphaBitmap` does not set blend
state. The source OZJ/JPEG ground/ring images carry black backgrounds, so
opaque submission painted black tiles during casts. The six imported
bitmap-object subtypes now bracket that terrain submission with 5.2's
additive `EnableAlphaBlend`/`DisableAlphaBlend`; model objects remain in the
generic textured `Calc_RenderObject`/`Draw_RenderObject` path. This is a
render-state repair pending a new owner screenshot, not visual parity proof.
The subsequent screenshot did show terrain restored, but still showed a
dark stacked model at the caster. The `0x688` Bat model is the pinned
`van_object02_skill.bmd` from S21 (source SHA-256
`EC45FCBADBC129D1056D64EBFB70EA32D3AD3C0A29F47AF7D341A5A762DF5A5B`);
its three authored textures are `empact01`, `macardkmono` and `marks_m03`,
not a placeholder image. The QA log also showed Detection's local graph
entering every ~16 ms while a `MOVEMENT_SKILL` intent remained active,
stacking `0x692` buff roots regardless of GS acceptance. `UseSkillSlayer`
now retires that movement intent after a local cast. This addresses the
movement replay path, but the first rerun still showed ~16-ms Detection
graph repetition from the separate held-right-button `Attack` route. That
route now latches a Slayer self-buff once per physical press and re-arms only
after release. The later unattended capture confirms one cast per QA step;
it does not yet prove class-9 or all-skill visual parity.

The four pinned private effect models (`Bat_van01`,
`van_object02_skill`, `van_object03_skill`, `Van_object04_skill`) retain
scoped `RenderBody` calls after the generic Calc path: `0x678` uses one or
two passes by subtype, while `0x688/0x691/0x694` use one. The decoded S21
draw registry, however, submits `0x688/0x691/0x694` with flag `0x82`
after multiplying their model RGB light by OBJECT alpha; the previously
assumed flag-2 ordinary body is only fallback. The 5.2 port now mirrors
that registered dark RGB path for those three models and no longer
color-keys their textures. The S21 OZJ loader at `0xCC53AC/0xCC562D/
0xCC58E5` removes the 24-byte wrapper, decodes three components and
uploads `GL_RGB`. The prior cutoff-48 `ark` and cutoff-16
`empact01/Elite_monster_ground02` RGBA masks were compatibility experiments,
not S21 behavior, and have been withdrawn. Fresh or resident model loads
retain the authored mesh-count guards; Pierce cylinder `0x5D8` has only
its exact lines2/RGB material guard, not an alpha-key adapter. No ingame
capture of this corrected
handler path exists yet, so it is build/static evidence, not visual PASS.
The QA log formerly labelled shared `0x694` Demolish submissions as Detection
and called every textured pass opaque; its diagnostic labels now use the
effect's skill ID and report textured-alpha while the effect alpha is below
`0.99`. These labels are diagnostics, not visual acceptance.
The same log later still labelled shared `0x691` model submissions from
`skillId=294` as Detection because its name helper only recognized Demolish.
The native graph uses `0x691` for Sword, Pierce, Detection and Demolish;
the helper now branches on those four raw skill IDs. Earlier model-name
counts cannot be used as per-skill render evidence. The isolated QA client
rebuilt and restaged after this attribution correction, but it was not
launched before the full-skill implementation/QA gate.
The x86 isolated QA build and five-skill source-surface verifier pass. The
first new QA client reached character select rather than the game map.
Windows Graphics Capture of that window failed twice with
`SetIsBorderRequired failed: No such interface supported (0x80004002)`,
so this remains a source/build result, not a visual correction claim.
The QA-only character-list trace showed the current `admin4` login receives
one unblocked `MainRF` row (`serverClass=224`, `clientClass=7`) and no
`Slayer` row. The launch script defaults to the requested `Slayer`; an
explicit `-UseLegacyFixture` switch selects `MainRF` solely for renderer
smoke. That fixture rerun (PID 19528) reached the game map, received one
authoritative cast per skill 292/293/294/295/297, and submitted the four
private S21 effect models. It cannot close the class-9 or visual gate.
Later packet logs exposed a QA-harness flaw: without a live monster,
`SelectedTargetCharacter()` returned `Hero`, so the fixture's offensive
292/293/294 packets had `targetIsSelf=1`. Their server acknowledgments and
frames are **not combat/target-contact evidence**. The harness now returns
no target, bounds unattended target search to the imported six-tile range,
and pauses offensive auto-casts rather than sending a self-target request.
The prior captures remain useful only for scoped buff blend isolation.
The current isolated GameServer Ex603/Win32 source also builds with
`SlayerServerQA=true` into the private `Bin12` output (`QA6` SHA-256
`4918181E33E00F6E4DC48AA4E72BA6AEBDF0A475F8E75A7C2839FE11001D9679`).
The fixture run above used the older private `QA5` server; this new QA6
artifact has not been swapped into the live stack or ingame-tested.
After the `0x7FDD` and `0x81CE` client-alpha corrections, the matching
five-skill GameServer source was rebuilt independently as Ex603/Win32 with
`SlayerServerQA=true` and private `Bin19`/`Int19` output. The resulting
`Bin19/GameServer.exe` SHA-256 is
`A79C5DFC148D2F579F93C9EC58BCC7339AF79B2C1D6D58EB1D8F21B70796AD4C`.
It has not been launched or treated as ingame parity proof; S21 Bat/Pierce
AOE geometry and the Pierce caster-position chain remain unresolved.
The QA-only framebuffer capture hook now schedules three JPEG frames at
0.5/1.5/3.0 seconds after each of the five authoritative fixture casts,
using 5.2's own `SaveScreen()` after the completed render rather than
Windows Graphics Capture. The 15 PID-12668 frames were written only beside
the private Slayer executable. Bat Flock's early trail and the floor are
visible without the previous black terrain square; Detection/Demolish at
sample 1 are overbright white under the all-model-additive trial.
The private `Media1_20260914` directory retains 78 samples from an earlier
SS21 capture, but its exact source video/hash was not retained. The named
Desktop path now holds the different hash-pinned **5.2 MainRF** recording
identified above, not that SS21 source. Direct inspection of
`frame_012.jpg`, `frame_013.jpg` and `frame_019.jpg` does **not** show the
previously claimed black floor square or dark `0x694`-like vortex: the first
two show an idle/transition pose, and the latter shows red attack fragments.
Those visual attributions were wrong and are withdrawn. The native `0x694`
model/asset chain is pinned independently by the S21 dump and file hashes;
the black funnel in the older 5.2 preview remains a renderer-parity issue and is
not owner-accepted. A fresh per-model comparison is still pending **after**
the five-skill code port is complete.

The same preview found 127 stale QA Data junctions targeting the mutable
shared `D:\RISE-CrossPlatform\Client\Data` instead of the frozen Slayer
worktree, including an extra `Map` junction. `prepare_runtime_qa_client.ps1`
now rebinds only exact private QA junctions to
`D:\RISE-CrossPlatform\Source_PC_Slayer\Client\Data` and removes stale
private junctions without traversing their targets. The stage verifier uses
`lstat()` rather than `stat()` to detect reparse points, checks the exact
126-junction set, and rejects any target outside the isolated source.

The September 16 private QA6 login reached the local GS and submitted the
saved `admin4` credential, but its character list contained only `MainRF`.
The Slayer-specific DS character-list/create adapter formerly encoded S21
DB classes `144/145/146` all as server byte `0xE0`, erasing Royal/Master
stage bits before the 5.2 client decoded the class. It now reuses the native
5.2 stage arithmetic with reserved base slot 7: `0xE0/0xF0/0xFF` decode to
client class `7/15/31`. An isolated Ex603 GS build and wire-stage verifier
pass; this is packet/class-stage repair, **not** proof of the five skill
visuals or Class09 avatar. The separate viewport preview helper
`gObjFixClassPacket` now encodes `0xE0/0xF0/0xF8` for those same DB stages,
preserving the lower equipment bits like the legacy 5.2 `PR_*` constants.
Both DS routes and that viewport route are mirrored in the GitHub snapshot.
No shared DB character was changed.
A read-only query of the live `RISE5.2` DB confirmed there is no character
named `Slayer` for `admin4`; the QA launcher now rejects that state before
spawning another non-Slayer preview. The supplied S21 class patch contains
all 15 Helm/Armor/Pant/Glove/Boot `Class09/209/309.bmd` files. Their private
QA Player copies now pass byte-for-byte comparison with that supplied patch.
The isolated client still keeps `MAX_CLASS=7`, but its 24-body-slot groups
leave indices 21..23 unused by the original seven classes and three stages.
`OpenPlayers` now loads the 15 Slayer body files into those three slots;
`GetSkinModelIndex` maps client class `7/15/31` to them only under
`RISE_SLAYER_PORT`, leaving the ordinary 5.2 fallback unchanged. The
isolated Win32 client rebuilt and restaged (SHA-256
`48B235644EE26921B2780A28BF062DA674400CF1852BAA410693C0DBB134E273`).
This closes the loader wiring gap, **not** actual avatar or five-skill visual
acceptance: no unique `admin4` character named `Slayer` exists in the shared
DB yet, and no shared DB record was changed.
The private Class09 body BMDs are **native S21 v0F**, not guessed or v0C
replacements. The isolated `BMD::Open2` already has a guarded v0F/LEA-256
reader under `RISE_SLAYER_PORT`; stage validation checks that route, exact
bytes for all 15 files against both the read-only S21 Player tree and the
supplied patch, and plaintext mesh materials. All base/Royal meshes name
`HQSkinClass109.jpg`; all Master meshes name `HQSkinClass309.jpg`. Both
staged OZJ textures match the native S21 Player files by SHA-256. This
eliminates a missing-body-material explanation for a future black Slayer
avatar, but a visible ingame rendering check is still required.

The separate Master Slayer resource conversion is now wired to the client
master UI. The pinned S21 tree and tooltip convert to 58 private 5.2 records;
the pinned S21 SkillList adds a third 58-record, 77-byte metadata overlay
(`MasterSlayerSkills.bmd`, SHA-256
`1A8932E3C3CDE1155F3DBFF426F627758293132CAA7ED75C54B9CAB559CC806C`).
The reader checks exact byte length, EOF, plaintext CRC32, class bit 512,
unique slots and equal tree/tooltip/metadata skill sets before exposing any
node. Raw Slayer marker 7 routes only that class to the overlay; other
classes retain the existing 512-record tree. `SkillAttribute` is a separate
metadata allocation enlarged to 800 in isolated Slayer builds, while
`MAX_SKILLS=650` still controls legacy Skill.bmd, character skill slots and
packets. Only SS21 Master Slayer IDs >=650 fill its unused metadata tail;
low IDs such as 631 remain class-scoped to the Slayer UI and do not overwrite
5.2 legacy metadata. The isolated Win32 client built and private stage
verified after this change. The visible category captions are explicitly
temporary; the S21 icon atlas is now imported, while GS master-tree values
and high-ID gameplay dispatch are not yet accepted ingame. No ingame
preview was launched from this build; static conversion/build/staging does
not close five-skill behavior or render parity.
The master icon previously still came from 5.2's
`Interface/new_Master_Icon.OZJ` (SHA-256 `DD1E63C006EE004CD11D7D2DF8A4C649ECB9169E34334C2C7BEA87DAF30CB5AA`).
The pinned S21 first 512x512 atlas has SHA-256
`DF3D1F863741E720EFC7B9ECC90117BB1BE9CA49852A86420FA8CA3E396D9F74`;
all 58 Slayer SkillList groups are <=399 and fit its 25-column page.
It is now copied byte-for-byte to the private Slayer interface overlay and
loaded at master UI bitmap slot `+16` only for class bit 512. Other classes
continue to render from legacy slot `+2`; private stage verification pins
the exact S21 atlas hash. This is asset provenance and code routing, not
ingame UI/icon acceptance.
The GS now reads the same pinned 58-node tree/skill metadata packets from a
private `Data/Skill` sibling overlay. It verifies exact size/EOF/plaintext
CRC32, class bit 512, unique slots/IDs, the complete 58-node count and all
parent references, then stores a separate Slayer shape map without touching
`MasterSkillTree.txt`. When a Slayer requests learning, that map first
excludes nodes outside its class-512 tree and rejects any legacy row whose
group/rank/min/max/parents differ after converting SS21 group `0..2` to
5.2 GS group `1..3`. A hash-pinned comparison finds 39 shared low-ID rows
with matching **shape** and 19 missing rows, but it does not prove matching
per-point values or class permissions. Legacy `631` and S21 Slayer Rush
`631` also match numerically after that conversion, so the code explicitly
holds `631` out of Slayer learning until its class/option provenance is
mapped; the earlier claim of a raw group mismatch was wrong. The isolated
Ex603/Win32 GS first built into private `Bin22`, but was not launched.
The new class-scoped `GetInfoForActor` now derives 781/782 learning rows
from pinned S21 shapes: 781 slot58/group1/rank6/1..20 points/Brand293 and
782 slot62/group1/rank7/10 points/parent781/Brand781. It is used by
learning, prior-rank checks and learned-node packet sends; other classes
still use legacy `GetInfo`, and 631 remains held. `RequireClass[DK]` only
carries the S21 UI slot under the legacy packet layout; persisted DBClass
is the actual Slayer gate. The structural adapter compiles in isolated GS
`Bin25` after pinning the native group-1 Bat slots, without launching it.
Its per-point value array is explicitly
unknown/zero rather than fabricated: Mastery 781 damage increase, 782 DOT
seconds, the remaining 17 absent nodes and shared-node passive values are
still not fidelity-complete. Normal acquisition and Pierce remain ingame
unverified before the five-skill QA gate. The first 781/782 learn path now
snapshots the replaced Bat skill and restores it if `AddMasterSkill` fails
(for example a full master-skill slot list); it returns before MasterPoint
deduction or packet sends. Legacy other-class learning is unchanged. This
transaction guard compiled in the isolated Ex603/Win32 `Bin26` build; it is
source/build proof only, not a forced-full-list runtime test.
A further hash-pinned comparison of the 39 shared 5.2 ID rows found three
Slayer-versus-DK UI slot differences: `303` is Slayer slot 8 versus DK 7,
`307` is 12 versus 11, and `310` is 16 versus 15. The previous GS
`GCMasterSkillListSend` used `RequireClass[DK]`, so a Slayer could receive
those wrong slots even though group/rank/parents matched. The actor-scoped
`GetInfoForActor` now verifies the shared row's shape/brand against the
S21 class-512 overlay and replaces only its UI slot with the S21 slot;
its legacy per-point MainValue remains untouched and unproven as SS21
parity. Non-Slayer classes still receive their original row. S21/legacy
ID `631` remains excluded for Slayer. The isolated Ex603/Win32 GS rebuilt
in private `Bin20` with SHA-256
`B761F27B25C1158B3CC26CDDC41EB5C4F607A118D13EE9968260ED63BDBBC222`;
this is not ingame evidence.
A separate pinned-main immediate search for Master Bat IDs 781/782 found
three 32-bit CMP sites for 781 at `0x13DC8AE/0x13DDBBF/0x14241E7` and none
for 782. Disassembly shows all three compare the object's **model Type**
at `OBJECT+0x146`, not a skill ID; this does not prove an upgraded Bat cast
dispatcher nor prove its absence. The subsequent full basic-block read of
`0x12CEBAE` and `0xBCFF9F..0xBD000B` **does** recover the Brand-based
canonicalization before the native base-293 ReceiveMagic branch. The GS
delay array was 629 entries and directly indexed by packet skill ID,
which made a high-ID cast unsafe; the isolated GS now allocates/clears 800
entries and bounds-checks delay accesses (pointer layout unchanged). Both
781/782 route through the Bat graph; `Attack` recognizes them as Bat half
strikes, and the private fanout client accepts their raw IDs only for a
Slayer caster after Brand resolution. Ex603/Win32 GS built privately into
`Bin23` and the client built/staged privately, without an ingame run.
