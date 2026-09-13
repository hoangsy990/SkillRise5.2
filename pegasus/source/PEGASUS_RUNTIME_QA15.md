# Pegasus + Wind Soul one-run runtime QA

## 2026-09-11 Deathside FPS-normalized recheck

- Runtime/Delivery client:
  `646863E0FAB645784B4CF2DD672349493CFB46A4254415CFEAEC60D70FDF11D3`.
- Captures are triggered from the appended `CAST_DISPATCH` row and taken at
  +200 ms and +900 ms. At 151-162 FPS the previous opaque white screen wash is
  gone; Reaper and a localized smoke layer remain visible.
- The three recovered ambient probabilities now use RISE's native
  `rand_fps_check`, so their per-second density is stable under unlocked FPS.
- All 26 focused client/server regression commands PASS after the correction.
- The stale remote server again omitted/rejected a variable subset of later
  QA casts and still sent only the summon-side Deathside traffic. Those missing
  echoes are not client visual failures; deploy staged `deathside-repeat-v3`
  before the authoritative one-run server/client acceptance recording.

## 2026-09-11 latest isolated-client run

- Runtime/Delivery client:
  `AB53A36D7888CF2D69EB069C749AE646AA07E0059C04AAA1AA7A5005A6343D80`.
- The black terrain square after Crusher Charge was reproduced, traced to a
  missing native alpha-blend state around terrain rendering, fixed, rebuilt
  and visually rechecked live. It no longer appears.
- Because the remote server is still older than `deathside-repeat-v3`, the
  isolated client now removes stale QA-only visual markers between samples.
  Runtime telemetry proves Reaper cleanup, Crusher aura cleanup, both
  Elemental subtype cleanups, correct Sword Wrath retention for Sword Blow,
  and final `AUTO_QA_POOL value1=0 value2=0 result=CLEAN`.
- These are real isolated-client runtime results. They do not validate the
  staged v3 GameServer on the VPS and do not establish visual parity for the
  remaining skill rows.

This harness is compiled only by `ExGameServer/Tests/BuildPegasusIsolated.cmd`
with `PEGASUS_ISOLATED_TEST`. It is not present in a production GameServer.

## Run once

1. Run the matching isolated client and isolated GameServer build.
2. Log in with a connected GameMaster level-1 character.
3. Start one continuous screen recording, then enter `/qa15` once. If no
   valid monster is nearby, the isolated command automatically creates eight
   class-0 targets; class 275 is excluded because native RISE deliberately
   suppresses its non-success `SetPlayerShock` branch. A manual `/spot`
   command is no longer required. The
   caster must stand outside a safe zone; otherwise the command stops with a
   preflight notice before any sample is submitted.
4. Do not move or manually cast until the completion notice appears. The
   sequence plus its final cleanup snapshot finishes in roughly two and a half
   minutes. `/qa15stop` cancels it safely.

The accepted start notice must contain `reaction-sample-v4`. If it does not,
the remote GameServer is stale and the recording is not valid for this build.

The server provisions each of the fourteen Pegasus group-15 skills plus
standalone Wind Soul only when its timed sample begins (never as a 15-packet
burst), uses the real client-to-server skill handler, chooses the nearest valid monster
for attack skills, keeps the selected isolated QA monster alive across later
steps, refills native Mana/BP before every submission, and self-targets buff
skills. Before each sample it removes prior Pegasus buff markers to keep visual
and pool evidence isolated. The sole intentional carry-over is Sword Wrath from
step 10 into Sword Blow at step 11; it is then removed. Stop, death and normal
completion also clean every QA marker. Each step emits a numbered notice and a
`[PegasusQA15]` server-log row; cleanup emits `BUFF_CLEANUP`. `SUBMIT` proves
only that the QA packet was sent; the video and client behavior decide runtime
acceptance.

Deathside step 01 performs two real casts 1.25 seconds apart: the first summons
the persistent Reaper and the second selects attack action `3/4/8` plus its
matching scythe child. Raining Arrow, Chaos Blade, Havoc Spear, Sword Blow,
Shining Bird, Dragon Violent and Spearstorm each perform ten ordinary native
casts 1.25 seconds apart so the randomized non-success monster-shock branch can be sampled
without forcing a server damage flag or changing client reaction behavior.
Each repeated skill still owns one three-phase framebuffer capture window.
All remaining steps cast once.

Current matched artifacts:

- Client SHA-256:
  `8EB0E5B59A6F3C0087A1BC2A05B9CA5589B8B21808ACB22560E10916D0A5CA55`.
- GameServer SHA-256:
  `5DDE88FE82EAB7D598D09D9F7728FD1DAA03E27139BB57277BD9757E2416ED3B`.

For each `SUBMIT`, require a matching `[PegasusCast] ... ECHO_SEND`. If it is
missing, the preceding `[PegasusCast] ... REJECT_*` row identifies the exact
native server gate; do not diagnose that case as a BMD/renderer failure.

## Evaluation sheet

| Step | Item | Skill | Route | Runtime result | Review focus |
|---:|---:|---|---|---|---|
| 01 | 15:100 | 288 Deathside | target | NOT EVIDENCED | action, Reaper root/children, hit, cleanup |
| 02 | 15:101 | 289 Darkness | self | NOT EVIDENCED | cast, persistent buff, remove/death cleanup |
| 03 | 15:102 | 2023 Raining Arrow | target | NOT EVIDENCED | bow/arrow root, target list, bone-0 4x4 damage sheet, impact, cleanup |
| 04 | 15:103 | 2024 Dex Booster | self | NOT EVIDENCED | cast, 19 joints, 20 particles, aura lifetime |
| 05 | 15:104 | 2012 Chaos Blade | target | NOT EVIDENCED | action, blade/trails, target impacts, cleanup |
| 06 | 15:105 | 2013 Havoc Spear | target | NOT EVIDENCED | cast burst, six joint routes, odd bursts, hits |
| 07 | 15:106 | 2014 Spiral Charge | self | NOT EVIDENCED | animation carrier, nine joints, anchor/buff |
| 08 | 15:107 | 2015 Crusher Charge | self | NOT EVIDENCED | roots/children, buff ownership, cleanup |
| 09 | 15:108 | 2016 Elemental Charge | self | NOT EVIDENCED | conversion layers, buff, third-Havoc behavior |
| 10 | 15:109 | 1500 Sword Wrath | self | NOT EVIDENCED | cast/root, 15-frame atlas, 30 converging line joints, buff/cleanup |
| 11 | 15:110 | 1501 Sword Blow | target | PASS | owner accepted no-lunge, sword direction, tail blend and color |
| 12 | 15:111 | 241 Shining Bird | target | NOT EVIDENCED | bird/root, pillars/particles, hit, cleanup |
| 13 | 15:112 | 242 Dragon Violent | target | NOT EVIDENCED | action, roots/layers, double hit, cleanup |
| 14 | 15:113 | 243 Spearstorm | target | NOT EVIDENCED | storm/blades, target center, damage cadence |
| 15 | separate | 737 Wind Soul | target | PASS | owner accepted bird color, blend/smoke and stationary cast |

Use only `PASS`, `FAIL`, or `NOT EVIDENCED` after reviewing the complete cast.
Compilation, a server `SUBMIT` row, or a visible root alone is not visual parity.

## Isolated client trace

The `PegasusIsolatedBuild=true` client writes `PegasusRuntimeQA.log` beside
`Engine.exe`. Ordinary client builds do not contain this trace.

- `MODEL_STORAGE_INIT value1=10128 value2=10256 PASS` proves the running
  isolated client zero-initialized the complete native plus 128-slot Pegasus
  BMD range before any lazy model load.
- `ASSET_PREFLIGHT value1=<skill> value2=<family-size> PASS/FAIL` runs after
  native image initialization and proves whether that skill's complete model/
  fixed-bitmap family can load before login. Animation-only carriers may emit
  `GPU_UPLOAD ... NO_MESH`; the family PASS is authoritative for their expected
  no-mesh structure.
- `CAST_ECHO value1=<skill> ... HIT/MISS` proves the client received an
  authoritative `0x19` row for that high skill. `HIT/MISS` is the target
  success bit, not server acceptance. Only the matching GameServer
  `[PegasusCast] ... ECHO_SEND` proves the server route accepted the cast.
  Absence of `CAST_ECHO` separates a server/packet failure from a renderer
  failure.
- `CAST_DISPATCH value1=<skill> value2=<root-model> DISPATCHED` proves that
  the accepted cast entered that skill's dedicated client switch branch. Use
  the sequence `CAST_ECHO -> CAST_DISPATCH -> EFFECT_CREATE -> EFFECT_RENDER`
  to locate the first missing runtime boundary without guessing from visuals.
- `GPU_UPLOAD value1=<model> value2=<mesh-count> PASS` proves a BMD in the
  isolated `MAX_MODELS+0..127` tail reached the shader VAO/GPU upload path.
- `GPU_UPLOAD ... NO_MESH` is a hard model-load failure. Correlate the model
  ID with the owning runtime header and the staged BMD dependency verifier.
- `MODEL_ENSURE value1=<model> value2=<subtype> FAIL` means the model or one
  of its required textures failed before effect-pool allocation. All Pegasus
  model families, including Deathside's `MAX_MODELS+47..51`, pass through this
  common load guard.
- `EFFECT_CREATE value1=<model> value2=<subtype> ALLOCATED` proves the dynamic
  effect obtained a live pool entry; `POOL_FULL` identifies pool exhaustion.
- `CAST_ROOT_GATE ... DUPLICATE_SKIPPED` proves repeated multi-target `0x19`
  damage rows did not recreate the caster/target root. One logical cast must
  have one `CAST_DISPATCH`; target impacts remain owned by packet `F3:35`.
- `EFFECT_RENDER value1=<model> value2=<tail-offset> PASS/FAIL` is emitted once
  per state and dynamic model. It proves whether the family-specific renderer
  was actually reached and accepted the object after allocation; a later PASS
  is still recorded even if the first visible frame returned FAIL.

These trace rows are diagnostic evidence only. They do not prove visual
direction, color, blend mode, child cadence, collision, lifetime or parity.
