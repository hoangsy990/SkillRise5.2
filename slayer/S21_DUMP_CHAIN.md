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
| 292 (`0x124`) | Sword Inertia | `0x012CEF03` | `0x012D0F0F` | `0x68A`, `0x689` |
| 293 (`0x125`) | Bat Flock | `0x012CEF13` | `0x012D10A0` | `0x682`, `0x683`, `0x684` |
| 294 (`0x126`) | Pierce Attack | `0x012CEF23` | `0x012D1209` | `0x679` |
| 295 (`0x127`) | Detection | `0x012CEE1E` | `0x012D139B` | `0x692` |
| 297 (`0x129`) | Demolish | `0x012CEF58` | `0x012D1443` | `0x695` |

The handler prologues also set the native action IDs `0xE0`, `0xE3`,
`0xE4`, `0xE8`, and `0xE9` respectively.  The isolated merged player model
maps the first four unique clips to 5.2 actions `284..287`.  A zero-mesh
action hash comparison shows S21 action `0xE9` (233) is byte-identical to
`0xE8` (232), so Demolish reuses merged action `287` instead of adding a
duplicate clip.

There is a second skill-action switch at VA `0x010E6830`. It is not the
ReceiveMagic dispatcher above: its 292 branch creates `0x679` and then
`0x68A/0x689`, while its 294 branch creates `0x68A` and then `0x679`.
Those call-site differences are recorded as evidence, but no unobserved
children (`0x681`, `0x688`, `0x693`, `0x694`) are claimed for these Slayer
handlers.

The 5.2 implementation keeps these numeric values as evidence labels only;
it does not copy S21 addresses, hooks, or executable code into the 5.2 binary.
The actual port re-creates the same dispatch/target/effect ownership in the
isolated Slayer overlay and uses hash-pinned S21 Effect assets.

The native receive bridge is fail-closed on class identity: a normal build
accepts only S21 class 9.  The isolated QA fixture may bypass that gate only
when `SlayerAutoQA.enable` is present, and records the legacy fixture class in
`SlayerRuntimeQA.log`; this does not claim that 5.2 class-9 persistence or
protocol migration is complete.

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

The checker verifies the pinned dump hash, reads the PE section table itself,
and compares the exact machine-code bytes at each skill compare, handler,
action branch, action push, and sound push.  It also decodes the conditional
jump displacements and verifies the five handler edges before checking the
human-readable disassembly windows.  A PASS here proves the mapping is
present in the captured binary, not merely in a guessed text note; it is not
a substitute for a live server cast or owner visual acceptance.

`verify_port_surface.py` is the companion static check for the 5.2 overlay: it
requires every imported ID to be wired through the shared contract, server
catalog/handler, client receive/use route, native runtime/resources, and packet
row.  It likewise does not claim legacy class-9 persistence or ingame approval.
