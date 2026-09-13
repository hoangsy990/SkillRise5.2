# Charge third-attack protocol investigation

## Evidence boundary, 2026-09-13

Read-only pinned `D:/Pegasus/MU-x86.exe`, SHA256
`02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA`.
Addresses below identify this binary only, not RISE hooks or object layouts.
Use `ExMain_RISE_PC/Tests/DisassemblePegasusRange.py` to reproduce bounded
decoding. Xref scans locate leads; they do not prove absence of indirect calls.

## Resolved leads

- Chaos cast callback `654AF9..654BA0` selects action285 with183fallback,
  invokes `6F07B5` and creates caster root through `654DC3..654E3B`.
  Havoc callback `666CB6..666D9E` selects284 with183fallback and creates27A5.
  Neither callback contains an every-third counter or charge-marker test.
  This finding is scoped to these functions, not the entire client/server.
- `6F07B5..6F08A7` is NOT a third-hit increment: its relevant branch calls
  `4485BC` then stores two returned words on the object. No increment/modulo3
  exists there. Meaning of the stored pair is not asserted from this alone.
- Input callers are Chaos `65384F..654327` and Havoc `664DF3..665CF3`,
  reached from `7F4EEB..7F4F01` and `7F4ECE..7F4EE4` respectively.
  Full bounded decoding covered2777/2777 and3841/3841bytes; call/cmp summary
  identifies shared packet serialization, not complete decoded field semantics.
- Both input builders call `812810` (Chaos653EC8, Havoc66587B).
  Complete helper `812810..812847` checks a non-null output pointer,
  increments a GLOBAL WORD, wraps values greater than50 to1, writes its
  low byte and returns the byte when the pointer is non-null. FOLLOW-UP:
  Chaos653EC6 and Havoc665879 both explicitly PUSH0 before calling it.
  Therefore THESE callsites take the early XOR AL,AL return, do not mutate
  the global sequence, and append ZERO at653EDC/66588F. The initial helper
  description alone must not be read as evidence these casts send1..50.
  Neither the helper nor these callsites proves a three-attack counter.
- Havoc helper `665CF4..665D30` clamps coordinate deltas to[-8,7] and packs
  two nibbles. It is a direction/offset encoding helper, not an explosion flag.

## Target-list route

The parser block `6E6A39..6E6B53` chooses payload offset4/5 for header form,
reads skill/caster/count as paired bytes, clamps count to10 and available
two-byte entries, resolves target IDs and fans the list into skill handlers.
Havoc callsite666DA1 is6E6B06; Chaos654BA3 is6E6B36 (one direct xref each).
Havoc `666DA1..666E13` and Chaos `654BA3..654BFA` filter their skill IDs,
walk valid targets and call impact helpers. In this inspected block there
is no separate third-hit flag or modulo3 gate. Do not reinterpret list index
or list count as the charge attack number.

## Outgoing fields narrowed further

Chaos appends source XY, heading, two literal zeros (653C2B/653CD3), target
key high/low bytes and the zero helper result. Havoc's corresponding
nonconstant fields include the packed delta from665CF4 (stored66524C,
appended6655DA) and a heading-derived byte (stored665270, appended665687).
These are observed value origins, not permission to transplant packet
encoding or assume unidentified fields are charge counters. In particular,
the two Chaos zero writes and both null-helper results provide no third-hit
payload variation. Client-side decoded wire evidence remains distinct from
server-side decisions and any additional incoming effect messages.

## Still open / next route

### Nova constructor cross-check and actual adapter fix

Direct CreateEffect call-context scan for model27A2 yields666970 (target
impact) and667238 (odd-tick burst). Both original calls pass SkillIndex2013.
Tracing the shared impact helper also found27A4/27A7 carry2013 at666B35/
666B9F. Current adapter had dropped those tags and stored root/target tags
in adjacent Skill instead. Corrected all six constructor paths and added
regression checks. This restores eligibility for the impact shockwave's
second texture pass; it does NOT implement an every-third damage counter.
New isolated build427A11... staged/hash-verified, runtime confirmation pending.

### Buff-side cross-check, 2026-09-13

Pinned BuffEffect.xml (SHA256
`1B12B27C6D56EAA721943AD0E15AAAD30F4D376B1F66B83947C17EA2E3BBBF76`)
uses214 Spiral,215 Crusher, **227 Elemental**, not216. Elemental's own
description explicitly requires a Nova on every third Havoc. Thus this
requirement is supported by pinned Pegasus metadata, not solely Webzen prose.
Spiral/Crusher descriptions mention stats/range, not the third-hit formula.

Inspected buff dispatch `6E3A2C..6E3A66` routes214->677042,
215->65B0A5,227->664D8E. Complete677042 wrapper tail-jumps676CFA.
Elemental664D8E checks live object and existing27B2 subtype0, then calls
664CF8 with boolean0; the callee XORs that boolean with1 at664D4E to create
subtype1. This confirms the existing relog/aura path, not an explosion gate.
Do not mistake the boolean0 argument for a subtype0 creation.

Exact-immediate227 scan also finds6FA5E9 inside the unrelated bitwise generator
loop (227 iterations, mask9908B0DF), and84C2DA in a renderer branch. The former
is not a charge counter; the latter is still an unclassified renderer lead,
not proof of buff semantics. Numeric xrefs alone must not select a damage
implementation. No counter/formula recovered in the scoped buff callbacks.

This does NOT prove the explosion is absent from Pegasus, nor that it is
entirely server-owned. Exact outgoing field semantics, any other incoming
explosion route, charge-specific counter/reset policy, damage radius and
formula are still unproven. Continue from the complete input serialization
and charge/attack receive dispatch, or supplied authoritative server evidence.
Current RISE weapon/range/marker routes cannot close these requirements.
No gameplay, packet layout, server damage or runtime binary changed here.

### Renderer numeric collision classified (2026-09-13 follow-up)

The previously unclassified comparison84C2DA against227 is a render-model
lead, not evidence of Elemental buff227. Decoding from the known branch
target84C2DA through84C641 shows mesh3 texture rendering, then byte-field
threshold40 at84C398 and20 at84C4B9: high tier mesh0 plus flags0x44 with
RGB(.3,.6,1), lower tiers mesh1/2, followed by map-dependent terrain shadow.
This distinctive sequence matches native RISE ZzzObject.cpp's
MODEL_DARK_SPIRIT branch (around979..1022). Model identity here is a
structural comparison, not a transplanted enum/address or buff-layout claim.
The nearby face-range branch84C21D..84C248 likewise sets RGB(4.8,4.8,4.8),
matching the adjacent native MODEL_FACE branch. Do not use this numeric227
xref to create a third-Havoc counter. No charge-counter operations were found
in this inspected renderer branch; other incoming/server routes remain open.

Reproduction uses hash-guarded DisassemblePegasusRange.py ranges
84C2DA..84C3E5 and84C3E5..84C641. An exploratory84C1A0 start was unaligned
and is NOT instruction-boundary evidence; use known84C1ED/84C2DA targets.

Server lifecycle follow-up: current isolated seeds set Save0/Type0 and
distinct effect groups. SkillPegasusSelfBuff only adds zero-valued markers;
ClearAllEffect removes Type0 entries. These facts alone do not establish
charge stance exclusivity, stat recalculation or death/logout parity.
Next inspect actual clear/save call paths and original charge replacement
behavior before adding policy; do not infer exclusivity from a range comment.

### Native server clear/save paths and executable fixture

ObjectManager user DieRegen==2 branch calls ClearAllEffect during respawn
(around352); this is not an immediate death-event claim. The native method
removes Type0, sends periodic removal, then clears the slot. DSProtocol
character save serializes every effect through EffectByteConvert (1879),
whose Save0 branch emits thirteen FF bytes. Character load clears each slot,
rejects FF through ConvertEffectByte, and only then calls AddEffect (3833ff).
User initialization also clears the effect slots (632).

GeneratePegasusBuffLifecycleTest.py extracts the current CEffect bodies and
actual manager ClearAllEffect/EffectByteConvert/ConvertEffectByte bodies.
Save/Type values are read from the current Pegasus seed loop, not copied
constants. The compiled fixture passes for all three native charge IDs:
no saved record, no restore, one removal notice each, repeated clear is
idempotent. A saved non-Type0 control is serialized/restored and preserved.
RemoveEffect side effects and network transport are stubbed, so this does
NOT validate stat recalculation, live reconnect or visual removal timing.
RunPegasusBuffLifecycleTest.cmd is now included in the full source suite
(39 listed gates; only the new focused gate was run in this turn).

Follow-up: complete39/39 source suite now passed. Recovered buff-add bodies
Spiral676CFA..676D99 and Crusher65B0A5..65B126 delete/recreate only their
own model27AD/27B1. Elemental664D8E..664DC3 checks27B2 subtype0; removal
664DC6..664DF0 deletes subtypes0 and1. None of these scoped callbacks
establishes cross-charge exclusion. All three have an early owner-live gate
missing from the corresponding RISE buff-add adapters; restored it before
any effect operations and added a static ordering regression test. Client
build/staging passed; runtime regression of this change remains open.

### Charge input and active-buff predicate leads

Hash-pinned immediate scans for2014/2015/2016 narrowed the actual dispatch:
7F4E70..7F4EC9 subtracts2012 then selects Spiral676270, Crusher6599E4,
Elemental663CE7. Spiral entry676270 is anchored immediately after the
previous function's RET4 at67626D. Its6762A7 null check exits676CDE;
6762C4 invokes676F0D before outgoing packet serialization at67633F.
The exploratory6762D0 decode was unaligned (correct MOVSS begins6762C9,
DIVSS6762D1); do not use its instruction output for recovered math.

44F4A1 takes object, skill and an output-byte pointer. For these three skills
44F720/44F732/44F744 push214/215/227 and tail into44F896->6E9E47.
The complete6E9E47..6E9E6A helper checks collection size, looks up the key
through44D119 and returns whether the result differs from its sentinel.
This is an active-marker membership predicate, NOT a weapon-family check.
Its two direct callers45102D and45414E consume the result with a seconds*1000
interval and per-entry time state; their full automation semantics are not
yet established. Do not turn this predicate into a global cast rejection.

Other shared charge comparisons811A14..811A71 write globalD36A18=-1;
7E8F3B..7E8F67 enter common input flow. Neither scoped block proves sword
handedness. Current server weapon contract remains metadata-derived; pinned
binary equipment-gate parity and replacement/counter policy are still open.
Next trace common input equipment validation or the complete caller bodies,
rather than treating active-buff membership as a weapon restriction.
