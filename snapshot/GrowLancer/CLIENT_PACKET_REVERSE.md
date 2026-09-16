# Grow Lancer S21 client cast-packet evidence

Status: `STATIC_EVIDENCE_COMPLETE` for the local S21 builders used by base
skills 271..279. Translation to RISE 5.2 remains disabled because S21 opcodes
must not be copied over the older protocol namespace.

## Player stat/join opcode numbering is not portable — 2026-09-16

The pinned S21 head table `130505C` maps plain head `0x03` to `12FEA31`
and head `0xF3` to `12FF92C`. The F3 dispatcher uses subtype byte+3 for
C1 or byte+4 for C2; subtype `0x03` goes to `12FFB21` and calls wrapper
`12884DB`, then indirect/status switch `986942`. Plain head03 goes to
`1297213`, whose continuation crosses an obfuscated/protected branch and
does not supply a complete initial-stat chain in this dump. This does not
establish that the native 5.2 `C1:F3:03` join packet has the same S21 meaning
or layout. `verify_s21_join_opcode_boundary.py` pins these handlers and
the native named join-speed assignment, but deliberately leaves S21 join
semantics and speed delivery OPEN. The separate S21 head82/sub04 handler
`1296442` is the proved global-speed WORD update; it cannot be mapped to a
native DWORD receive merely by field name.

2026-09-16 selected opcode82/sub04 byte-offset boundary: pinned
`verify_s21_speed_packet_offset_boundary.py` shows C1 subtype at raw byte+3,
C2 subtype at raw byte+4, both dispatch sub04 to `1296442` with the unchanged
raw buffer pointer. That receiver always reads WORD speed fields at raw+4
and raw+8. The selected branch does not compare packet-length bytes; upstream
validation remains unknown. For C2, raw+4 is also the subtype byte, so no
actual C2 sub04 payload compatibility can be asserted without sender/runtime
proof. Neither opcode/width nor this fixed layout was transplanted into
native 5.2; initial S21 sender and rate remain OPEN.

## Common target packet: `C1:35`

Harsh Strike 276, Spin Step 271, Circle Shield 272, Magic Pin 274, Obsidian
273 and Wrath 278 all construct a `C1:35` packet after the same 200 ms cast
gate. The buffer calls append four source bytes in this exact wire order:

1. target/index high byte;
2. skill high byte;
3. target/index low byte;
4. skill low byte.

The branch finalizes/sends the packet and then calls `0x172B412(skillId)`.
That final call is retained as an unlabelled post-send function; its semantics
are not inferred from the argument alone.

Target/index source by skill:

| Skill | Local branch | Packet target source |
|---|---:|---|
| 276 Harsh Strike | `0x10E36D7` | cast record `+0x14` |
| 271 Spin Step | `0x10E38FF` | cast record `+0x14` |
| 272 Circle Shield | `0x10E3E77` | local actor index global `0x70B12BC` |
| 274 Magic Pin | `0x10E3FF2` | cast record `+0x14` |
| 273 Obsidian | `0x10E455F` | local actor index global `0x70B12BC` |
| 278 Wrath | `0x10E470E` | local actor index global `0x70B12BC` |

Thus the three buff skills are explicitly self-targeted in this client edge;
the other three use the selected cast target. This does not by itself prove
server target authorization or multi-hit scheduling.

## Extended positional packet: `C1:57`

Shining Peak 277 (`0x10E4271`) and Breche 279 (`0x10E43CB`) call the shared
builder at `0x10EEB1E` with nine arguments. Its exact serialized payload is:

1. 32-bit caster map X;
2. skill high byte;
3. 32-bit caster map Y;
4. skill low byte;
5. caller value 4 (the local dispatcher temporary at `ebp-0xF5AC`);
6. selected target high byte;
7. caller value 5 (zero at both base call sites);
8. selected target low byte;
9. caller value 6 (zero at both base call sites);
10. serial byte returned by `0x173EA5C`; both base calls pass a null pointer,
    so the emitted value is zero.

The ninth function argument is a send/post-send flag and is one at both base
call sites; after send it invokes `0x172B412(skillId)`. The builder has many
other callers, so its opcode is not named from these two skills alone.

## Clash movement packet: `C1:59`

2026-09-15 focused byte-level continuation: `verify_clash_movement_payload.py`
pins contiguous terrain/send/local-move block `10E4CB5..10E4E21`, the
`0x95DB09` one-byte writer and `0xD8DC5A` C1 builder. The six writer calls
consume the six stack inputs in reverse push order, so the listed wire order
is verified rather than inferred from push order. A dormant, opcode-free
`Shared/GrowLancerClashMovementPayload.h` encodes seven payload bytes and
rejects terrain flags masked by `0x1C`; focused x86 `/W4 /WX` test PASS.
No native opcode registration, client send, GS handler or movement authority
is enabled by this pure contract.

After calculating and terrain-validating its destination tile, Clash 275
builds `C1:59` at `0x10E4D3C`. It first appends byte 1, then appends six source
bytes through the chained byte writer in this exact wire order:

1. target high byte;
2. skill high byte;
3. target low byte;
4. skill low byte;
5. destination X byte;
6. destination Y byte.

Correction after complete `0xD6DC05..0xD6DCA3` decode: this helper builds and
sends a SECOND `C1:31` packet with destination X/Y. It does not mutate the
local character position. Therefore Clash emits `C1:59` followed by `C1:31`;
the earlier "local movement edge" reading of the call was wrong. Native 5.2
top-level `C1:31` invokes `CGNpcTalkCloseRecv`, while native movement uses
`C1:D4`. Neither S21 packet may be forwarded by numeric equivalence. Exact
S21 siege/movement GS handler and server collision/push/rollback remain OPEN.

## RISE 5.2 mapping boundary

The target server receives ordinary attack on `0x19`, duration/positional
attack on `0x1E`, and multi-attack under `F3:DB`. Its top-level `0x57` already
means guild-create cancellation. Therefore neither S21 `0x57` nor `0x35/0x59`
may be registered or forwarded by numeric similarity. A native 5.2 adapter
must map the proven fields into a skill-local packet/handler contract without
changing existing opcode behavior; it stays disabled until authoritative
server target/hit/push semantics are recovered.

## Isolated RISE 5.2 target-bearing duration echo — 2026-09-15

The native `SendRequestMagicContinue` sender and
`PMSG_DURATION_SKILL_ATTACK_RECV` receiver on C1:1E already carry a 16-bit
skill, map tile, direction and selected 16-bit target. The stock 5.2 server
echo `PMSG_DURATION_SKILL_ATTACK_SEND` stops after caster key, tile and angle;
its matching client `PRECEIVE_MAGIC_CONTINUE` cannot recover the selected
target. A plain opcode translation would therefore lose the secondary actor
used by Breche's target-contact chain.

The isolated client/server adapter now defines a target-bearing C1:1E echo
for Breche (and the positional Shining Peak transport shape), appending the
two target-key bytes after the unchanged 5.2 prefix. Every existing SS6 echo
retains the old struct and size. Breche receive checks the exact extended
size and resolves a live target before creating its independent caster and
contact visuals; no S21 `0x57` receiver is registered. This is a static
transport correction, not a GameServer cast PASS: `HasProvenServerRuntimeHandler`
still rejects Breche until its authoritative target/hit-count and class
handler is implemented.
