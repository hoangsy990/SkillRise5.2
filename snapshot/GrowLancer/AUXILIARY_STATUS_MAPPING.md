# Auxiliary post-render status naming evidence

Scope: S21 callback 0x1893296 after auxiliary renderer dispatch, not an
additional Breche child and not proof of server-authoritative skill behavior.

Read-only source: `D:\GameServer S21\Data\BuffEffectManager.xml`.
SHA-256: `201452C00D9D5CE0DB83820EF48EC4317A6654864C0BE9176A6389694A4AD6B0`.
`General/Buff/@Index` is distinct from `EffectType` and skill ID.

| Callback key | Decimal | Supplied table Name | EffectType |
|---|---:|---|---:|
| 0x38 | 56 | Ice | 28 |
| 0x8D | 141 | Reduce attack speed | 86 |
| 0x92 | 146 | Freeze | 90 |
| 0x9E | 158 | Frost | 93 |
| 0xF0 | 240 | Paralysis | 237 |
| 0x112 | 274 | Chilling | 266 |
| 0x116 | 278 | Freezing | 270 |
| 0x56 (alternate branch) | 86 | Cold | 49 |

The first seven keys share an incoming-RGB body overlay; alternate 86 changes
RGB before its body draw. Earlier binary verifier pins both draw branches.
These names are table evidence only: client status ingestion/translation must
still be traced before treating this server table as the client enum contract.
In particular, decimal 274 and 278 here are NOT Magic Pin / Wrath skill IDs.

Native `_enum.h` prefix, parsed with comments removed and explicit assignments
respected, gives `eDeBuff_Freeze=56`, `eDeBuff_BlowOfDestruction=86`.
`Draw_RenderObject` already has that ordered two-branch overlay with matching
render argument shape. This supports the two legacy numeric correspondences;
it does not authorize mapping the other six statuses to Freeze, reusing their
server EffectType numbers, or enabling nonexistent SS6 buff semantics.

Verifier: `tools/grow_lancer/verify_auxiliary_buff_names.py`.
The supplied XML has an invalid triple-hyphen comment at line 414. The verifier
hashes the original bytes and strips comments in memory before XML parsing;
the original file remains untouched. No claim is made about GS parser acceptance.

Next: trace S21 status-container insert/packet decoding and native status
ingestion; keep unmatched statuses explicitly unimplemented until verified.

## Storage and packet-width follow-through

Pinned helper `0x130C63E..0x130C6CA` looks up its first argument in the
container at this+4. Only an absent key allocates a 12-byte payload initialized
to `(1, second argument, -1)` and inserts the first argument as key. A duplicate
does not refresh those fields in this helper. Their gameplay meanings are not
yet established. Membership `0x130C8A5..0x130C8FB` passes its argument directly
to the same lookup helper, without an intervening skill/effect-type conversion.
Removal `0x130C769..0x130C7EA` obtains the stored pointer, passes it and size 12
to `0x19B902C`, then erases the entry. Do not transplant this ownership layout.

Native `Buff::RegisterBuff` instead stores `map<eBuffState,DWORD>` with value 1
for absent keys, with no separate 12-byte allocation. Reuse native membership
for proven states; do not pretend that its DWORD implements the unknown S21
payload fields. This is storage-level evidence, not proof of network ID origin.

`WSclient.h` defines `PMSG_VIEWSKILLSTATE::BuffIndex` as BYTE, and all five
`s_BuffEffectState[MAX_BUFF_SLOT_INDEX]` declarations use BYTE. `ReceiveSkillStatus`
casts BuffIndex directly and rejects values outside the native enum range.
Therefore buff 274/278 and Wrath's 424/425 cannot be represented on those paths.
Increasing the skill-ID table or enum alone does not extend these packets.
Truncation would yield 18/22/168/169, respectively, which must NEVER be used
as aliases. Separate buff packet design, sender/receiver, viewport, removal,
reconnect and persistence policy must be verified together before activation.

`verify_buff_storage_contract.py` checks these exact source/binary boundaries.
Next: S21 caller ingress and GS effect-send contract; no global packet widening
or production activation is performed by this evidence checkpoint.

### GS and DS audit

The barrier starts before serialization: `CEffect::Set` takes a BYTE index and
`CEffect::m_index` is BYTE; `Clear`/`IsEffect` reserve 0xFF as empty. The lookup
capacity is MAX_EFFECT=256 while actor slots are MAX_EFFECT_LIST=32. Those are
different capacities; raising slots will not make Wrath's ID representable.

`GCEffectStateSend` builds opcode 0x07 with BYTE effect, sends to the owner
and (except Chaos Castle) viewport; `GCPeriodicEffectSend` uses 0x2D, BYTE effect,
then invokes the state sender. `GenerateEffectList` uses BYTE viewport entries;
party entries also use BYTE effect, with party sender gated at update >=802.
Do not claim the party sender is active in the Ex603 build.

Persistence uses `EffectByteConvert`: 13 bytes per slot, index in byte zero,
four bytes count/time, four WORD values. Empty or non-saving effects serialize
as all FF. Both GS DSProtocol and DS DataServerProtocol have two declarations
of `Effect[MAX_EFFECT_LIST][13]`. DS reads Character.EffectList with GetAsBinary
and updates it in the character save query. Widening the existing record in
place would change both message layouts and saved data interpretation.

The pinned S21 table calls 424 Wrath and 425 Wrath Strengthener; both use
EffectType=123, Type=0, ClearType=1 (death/regen according to file comment).
This does NOT establish logout persistence, duration, or native m_save. Do not
infer a logout-clear policy from ClearType=1 or skip persistence investigation.

Implementation prerequisite: choose a scoped versioned extension only after
source GL buff metadata/duration is traced. Preserve legacy packet layouts and
32 actor slots unless evidence requires otherwise; never alias 424 to 168.
