# Sword Wrath shared renderer registry evidence

Pinned `D:\Pegasus\MU-x86.exe` SHA256:
`02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA`.
Read-only disassembly, 2026-09-13. No renderer change or visual acceptance.

## Registry creation

`6C33B1..6C33C4` obtains a temporary through `6EF74E`, then moves its
pointer/control block to global `67FD31C` through `6BF7CB`. The latter is a
move/ownership helper, not the registry's constructor or registration routine.

`6EF74E..6EF7A4` allocates 12 bytes, calls `6EECCD`, wraps the result through
`6EEB6A`, then calls `6EF23E` on the resulting object. `6EECCD` installs
vtable `B726B4` and initializes the collection at +4 via `6EEC88`.

Complete `6EF23E..6EF74D` explicitly adds the following map IDs (hex), then
registers each factory result through `6EFBEC`. These are the additions at
this site, not yet proof that constructors or other callers add no maps.

| Factory | Explicit map additions |
|---|---|
| 5136A9 | 39, 3A |
| 4FB3D1 | 3E |
| 50FECF | 3F |
| 4DAD21 | 40 |
| 4D4DD0 | 41 |
| 4D663C | 42 |
| 4D7E7B | 43 |
| 4D90AA | 44 |
| 4DB965 | 45 |
| 4E21C3 | 46 |
| 4E4B8A | 47 |
| 4E8077 | 48 |
| 4FF820 | 4F |
| 4F570C | 50, 5D, 51 |

First factory `5136A9` allocates 0x24 bytes and calls `512614`. That constructor
explicitly zeros vector fields +4/+8/+C, installs vtable `B56294`, and does not
call another initializer. Thus this factory starts with an empty map vector;
all thirteen remaining factories were subsequently decoded through their return.
Each explicitly zeros +4/+8/+C inline before its ownership wrapper. Their
allocation sizes vary (0x10/0x14/0x18), but no map insertion occurs before
the wrapper call. Ownership wrappers and later mutation paths are not yet
exhaustively excluded. No global exclusion of callback14 is established.

## Registry follow-up: all factory initializations and dispatch

Exact immediate scan of 6EFBEC found 14 direct calls, all in 6EF23E:
6EF2B1, 6EF2F9, 6EF341, 6EF389, 6EF3D1, 6EF419, 6EF461,
6EF4A9, 6EF4F1, 6EF539, 6EF581, 6EF5C9, 6EF611, 6EF69B.
This scan does not exclude indirect callers or later vector mutations.

6EFF23..6F003B is the exact renderer iteration. It calls 6F02D5 with
current map [CBB440] before invoking virtual byte offset +0x14. 6F02D5
searches the DWORD vector [+4,+8) for exact map equality, returning false
when empty or absent. There is no wildcard fallback in that lookup.
6EFBEC registers through 6EEBE4 after a duplicate check via 6EF163.

File-backed vtable +0x14 pointers (not copied to RISE):

| Factory | Vtable | Render function |
|---|---|---|
| 5136A9 | B56294 | 518E03 |
| 4FB3D1 | B5585C | 4FB466 |
| 50FECF | B56174 | 511B52 |
| 4DAD21 | B54F60 | 4D48C0 |
| 4D4DD0 | B54D30 | 4D5C7E |
| 4D663C | B54E38 | 4D6D87 |
| 4D7E7B | B54E98 | 4D8AE1 |
| 4D90AA | B54EF8 | 4D9D11 |
| 4DB965 | B54FC0 | 4E0387 |
| 4E21C3 | B551C4 | 4E3C80 |
| 4E4B8A | B55294 | 4E6FE6 |
| 4E8077 | B55338 | 4EA64E |
| 4FF820 | B559D4 | 500457 |
| 4F570C | B55710 | 4F7634 |

The directly initialized registration path contains no map 0x33. This is
a conditional exclusion for that registry state, not proof of the original
runtime registry state on every map or after all later initialization.

## Direct renderer entry guards: map 0x33

Follow-up inspected all fourteen render entries and the complete called map
predicates. This independently narrows the known handlers on QA map 0x33,
without relying on their registration vectors remaining unchanged.

| Render entry | Entry guard / result |
|---|---|
| 518E03 | Calls 513691: maps39/3A/41 only; false ->518E2D AL=0 |
| 4FB466 | Map3E only, then model10 only; mismatch AL=0 |
| 511B52 | Calls52989F onCBB43C: map3F only; false ->512156 AL=0 |
| 4D48C0 | Unconditional XOR AL,AL; RET0C |
| 4D5C7E | Map41 only; mismatch ->4D5C8C AL=0 |
| 4D6D87 | Map42 only; mismatch ->4D6DA8 AL=0 |
| 4D8AE1 | Map43 only; mismatch ->4D8AEE AL=0 |
| 4D9D11 | Map44 only; mismatch ->4D9D1D AL=0 |
| 4E0387 | Predicates52984B/53/5B/63 allow45/46/47/48/49/4A/5F; otherwise4E03DA AL=0 |
| 4E3C80 | Predicate529853 map46 only; false AL retained through4E3CE3 return |
| 4E6FE6 | Predicate52985B map47 only; false ->4E701B AL=0 |
| 4EA64E | Predicate529863 maps48/49/4A/5F only; false AL retained through4EA6C0 return |
| 500457 | Map4F only, then model8/1E only; mismatch AL=0 |
| 4F7634 | Predicate4F56F2 maps50/5D/51 only; false branches4F7E9B |

Predicates52984B..5298A6 compare [ECX+4]; callers supplyCBB43C,
so this is the same global mapCBB440, not a separate state variable.
513691 also permits41 even though its known registration only adds39/3A:
do not mistake a registration list for the full function's applicability.
All known handler entry guards reject33. This is not a global exclusion
for other maps, nor proof no unknown handler can ever be registered.
No runtime/renderer changes were made from these observations.

## Callback5 loader follow-up

`5291D0..5291E8` supplies `1182914` to `493D99` and `1183404` to `493D6E`.
`493D99` copies at most 100 records, stride 0x1C, terminating when unsigned
record[0] > 99. No file-backed raw bytes are returned at `1182914`; do not
interpret that as a populated runtime table or an empty one.

Exact immediate/absolute-memory xref scan returned only loader references
for these tables and classifier table `1128D58`. Such a scan does not detect
base-plus-offset initialization or prove there are no other writers.

## Next evidence

Inspect ownership wrappers/later map mutations, then each relevant virtual
render method for model0x27B3. Separately trace
the source filling table1182914. Keep default Sword Wrath draw conditional
until callback5/12/14 outcomes are established for the actual scene.

## Source configuration recovered (supersedes unknown file population above)

6C0FED..6C1003 anchors the loader call: ECX=106B9D8,
argumentB56E08 is `.\\Data\\MultiLocal\\Pegasus.bmd`, call64266E.
The separate locale loader642567 uses +4D1710 and is not this table source.
64266E checks size185F4C, reads at object+4, and6426F8..642713 decodes:
`decoded[i] = ((raw[i] - (((i >> 8) & 255) ^ 0x95)) & 255) ^ (i & 255) ^ 0xCA`.
No decoded file was written or unrelated config content printed.

Actual read-only `D:\Pegasus\Data\MultiLocal\Pegasus.bmd` SHA256:
`DBE6F972CC699CF18BEDF9869BFE2FD2B31C3144B2A5BE3ED4CCF0DD72A5550B`.
Reproducible inspector: `ExMain_RISE_PC/Tests/InspectPegasusRendererConfig.py`,
requiring the pinned executable hash and original config size.

-1182914 table: four valid records, indices0..3, all model381; record4
  indexFFFFFFFF ends the native loader. Neither direct27B3 nor model+285
  matches. Thus this file's loaded callback5 table does not override Wrath.
-1128D58 classifier:494322 uses stride60, limit300 and unsigned index>299
  terminator, copying field+4 as map key. Eight valid keys:
  1ADC,1ADD,1B2C,1B2D,1B2E,1B2F,1B30,1B31. Record8 isFFFFFFFF.
  Neither27B3 nor its queried item key2320 (=27B3-493) is present.
  4941F6's missing-key branch494212 returnsFFFFFFFF (-1).

These are pinned configuration-load results, not a live memory capture or
proof against arbitrary later registry mutations. With previous map33
callback exclusions they substantially narrow the default draw. Next verify
the native default pass parameters against the current RISE adapter and
add a focused regression test before any isolated build/runtime comparison.

## Native adapter correction staged

Re-read67AECA..67AF78: root render sets BlendMesh0 and BlendMeshLight=Alpha
immediately before wrapper85B1BE. Re-read850D0D..850D3E and84DDFF..84DE10:
default call forwards object HiddenMesh, UV, BlendMeshLight, BlendMesh,
Alpha and flags2. Native RISE RenderBody(RENDER_TEXTURE,...) implements the
mesh selection plus BeginRender/EndRender and shader batch scope; the old
adapter bypassed it with a forced-bright loop and BlendMesh=-1.

Changed only root SwordWrath pass to native body call with source-proven
slot0/pulse/UV and object HiddenMesh. Focused regression added; test/build
pass. New isolated SHA3DF02FA06A390307400CE677AA86A2F01A529873FA45C9FC6E1DC2755E5EA433.
Runtime QA16660 started; results/visual acceptance still pending.

## Executed shader evidence, QA21304

At05:21:04.172 actual draw: program6, texture35195, flags66, GL_ONE/GL_ONE,
blend enabled, commandAlpha1, RGBA(.30,.06,.057,1), unlit and textured.
This is exactly authoredRGB(1,.2,.19) times pulse.30. Native queued blend
selection in New_ModelBMD.cpp multiplies command.Color by blendMeshLight,
sets command.Alpha1 and selects bright flags. Therefore Alpha1/flags66 in
the executed command does not contradict the original texture-only body
call or indicate a lost pulse. No forced-bright adapter was reinstated.

Diagnostic's original alpha-band sampler emitted only one mislabeled band2
because queuedAlpha is always1 on this path. Corrected sampling to executed
red (authored rootred=1); staged sampler-only build CC29C07A9E8C3F11867DF6BBF1AE19AB91EB96A87085102B019821E470B875F0,
not runtime-tested yet. One actual draw establishes this lowpulse path,
not every phase, final composited pixel color or original-reference parity.
