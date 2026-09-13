# Pegasus group-15 versus Season 21 color audit

Date: 2026-09-10

2026-09-13 evidence correction: Alpha*.65 below is the RISE preview
compatibility transfer, NOT a literal SS21 render-call constant. Recovered
SS21 traces record blendLight1 and do not identify the resolved bitmap or
final shader colour. OBJECT/PASS MATCH is therefore not a complete material
match. White-tail owner rejection remains open. Local queued draw measured
program6, ONE/ONE enabled, correct Swordeff_mono texture35453 and non-neutral
blue BodyLight. Before/after one-draw framebuffer measurement confirmed the
old143/233,155/233 material emitted .61278/.66441/1. A background-subtracted
reference-frame candidate .394/.476/1 now emits .38925/.46979/1 in QA24588.
This narrows the colour discrepancy, but does not prove complete visual parity.

2026-09-13 owner rejection supersedes the Sword Blow framebuffer implication
below:1501 tails were still neutral white/yellow. Applied the recorded
143:155:233 image ratio only to mesh1 BodyLight, preserving blue energy and
restoring BodyLight after draw. This is image-derived material tuning, not
a recovered SS21 constant. Build/test pass; revised framebuffer UNVERIFIED.

This audit separates exact runtime color evidence from visual inference. A
Pegasus constant, matching asset name, or compiled renderer is not by itself
proof that the Season 21 RGB/light/material contract is identical.

| Item | Skill | Current isolated implementation | SS21 color evidence | Result |
|---|---:|---|---|---|
| 15:102 | 2023 Raining Arrow | Exact Pegasus colors/constants | SS21 root/child chain exists; no complete color trace package | IN_PROCESS |
| 15:103 | 2024 Dex Booster | Exact Pegasus buff layers | SS21 constructor/resource evidence only | IN_PROCESS |
| 15:104 | 2012 Chaos Blade | Exact Pegasus effect colors | SS21 handler `0xA20804` writes candidate vector `(0.9,0.8,1.0)` before child `0x7F20`; provider argument semantics/render contract are not yet closed | IN_PROCESS |
| 15:105 | 2013 Havoc Spear | Exact Pegasus model/particle colors | No equivalent complete SS21 runtime color trace | IN_PROCESS |
| 15:106 | 2014 Spiral Charge | Exact Pegasus model/bitmap colors | SS21 allocator/resource evidence only | IN_PROCESS |
| 15:107 | 2015 Crusher Charge | Exact Pegasus model/bitmap colors | SS21 child/resource evidence only | IN_PROCESS |
| 15:108 | 2016 Elemental Charge | Exact Pegasus five-color bitmap order and pulse | SS21 constructor/resource evidence only | IN_PROCESS |
| 15:109 | 1500 Sword Wrath | Exact Pegasus layer/particle colors and textures | SS21 row is named Sword's Fury; current handler evidence is a stub and provides no comparable color contract | IN_PROCESS |
| 15:110 | 1501 Sword Blow | SS21 object lights plus native-5.2 material transfer applied and compile-time guarded | Complete delimited SS21 object/pass trace plus completed RISE Visual Studio compatibility renderer | OBJECT/PASS MATCH; revised framebuffer color owner-QA pending |
| 15:111 | 241 Shining Bird | Exact Pegasus renderer/particle colors | No isolated SS21 color trace package | IN_PROCESS |
| 15:112 | 242 Dragon Violent | Exact Pegasus renderer/particle colors | No isolated SS21 color trace package | IN_PROCESS |
| 15:113 | 243 Spearstorm | Exact Pegasus renderer/particle colors | No isolated SS21 color trace package | IN_PROCESS |

## Confirmed SS21 Sword Blow object lights

- Root, blade and plane constructor/object state: `(0.2,0.2,0.2)`. This is not
  the final visible color. The current 5.2 compatibility renderer uses mesh 0
  once with unit blend light and small-sword bright mesh 1 with `Alpha*.65`.
  This transfer still needs framebuffer parity verification. A duplicate
  white mesh-0 pass is forbidden.
- Plan/glint: `(0.7,0.7,1.0)`.
- Multishot 1/2: `(0.4,0.5,1.2)`; multishot 3: `(0.35,0.4,0.48)`.
- Target lightning: `(0.3,0.6,1.0)`; flare: `(0.65,0.7,0.9)`.

## Capture requirement for the remaining skills

Use the generalized RISE Visual Studio Season 21 tracer per skill and retain
the delimited package containing `CreateEffect`, immediate/secondary helpers,
light vectors, material flags and object identity. Compare by semantic node,
not by raw runtime ID, because Pegasus, SS21 and RISE ID domains differ.
