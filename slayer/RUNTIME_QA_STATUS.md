# Slayer runtime QA status

Last automated check: 2026-09-14 (Asia/Bangkok)

- Isolated checkout: `D:\RISE-CrossPlatform\Source_PC_Slayer`
- Binary: `Engine-Slayer S21.exe` (private `SlayerBuild\RuntimeQA\Client` stage)
- Account/character requested by owner: saved credential `admin4` / `Slayer`
- Stage verifier: PASS (private Player/RISE roots, root login keys, `Config\Mix.bmd`, no `Data\RISE\GrowLancer` overlay)
- Build: PASS (Release x86; Slayer runtime QA and effect bridge linked)
- Contract/data tests: PASS
- Runtime process: PASS (`Engine-Slayer S21 - Slayer`, responsive after the sequence)
- Runtime capture: PASS (105 frames from the isolated client window)

The captured in-game session recorded accepted event chains for all four skills:

| Skill | Runtime evidence |
|---|---|
| 292 Sword Inertia | accepted receive probe; three visible sword projectiles |
| 293 Bat Flock | accepted receive probe; visible bats and four one-second DOT ticks |
| 294 Pierce Attack | accepted receive probe; visible sword/impact ring |
| 295 Detection | accepted receive probe; visible cyan detection mark around the actor |

All four skills were exercised in one automated in-game sequence in PID 34016,
not as four independent test runs. The runtime used converted, hash-pinned S21
models, textures, sounds and player actions from the owner's local reference
client; those proprietary assets are intentionally not committed here.

This proves the 5.2 client dispatch/effect/render path. The sequence injects a
local receive probe after exercising the production cast path, because the
connected remote GameServer has not received this branch. It therefore does
not prove remote server-authoritative damage, minimap detection semantics,
reconnect/persistence, or final owner acceptance.
