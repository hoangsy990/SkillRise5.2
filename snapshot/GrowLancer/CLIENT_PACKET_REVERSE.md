# Grow Lancer S21 client cast-packet evidence

Status: `STATIC_EVIDENCE_COMPLETE` for the local S21 builders used by base
skills 271..279. Translation to RISE 5.2 remains disabled because S21 opcodes
must not be copied over the older protocol namespace.

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

After calculating and terrain-validating its destination tile, Clash 275
builds `C1:59` at `0x10E4D3C`. It first appends byte 1, then appends six source
bytes through the chained byte writer in this exact wire order:

1. target high byte;
2. skill high byte;
3. target low byte;
4. skill low byte;
5. destination X byte;
6. destination Y byte.

After sending, `0xD6DC05(destinationX,destinationY)` performs the local movement
edge. The server-side collision/push/rollback handler is not present in the
available inputs and remains open.

## RISE 5.2 mapping boundary

The target server receives ordinary attack on `0x19`, duration/positional
attack on `0x1E`, and multi-attack under `F3:DB`. Its top-level `0x57` already
means guild-create cancellation. Therefore neither S21 `0x57` nor `0x35/0x59`
may be registered or forwarded by numeric similarity. A native 5.2 adapter
must map the proven fields into a skill-local packet/handler contract without
changing existing opcode behavior; it stays disabled until authoritative
server target/hit/push semantics are recovered.
