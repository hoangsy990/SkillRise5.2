# Slayer runtime QA status

Last automated check: 2026-09-15 (Asia/Bangkok)

- Isolated checkout: `D:\RISE-CrossPlatform\Source_PC_Slayer`
- Binary: `Engine-Slayer S21.exe` (private `SlayerBuild\RuntimeQA\Client` stage)
- Account/character requested by owner: saved credential `admin4` / `Slayer`
- Stage verifier: PASS (private Player/RISE roots, root login keys, `Config\Mix.bmd`, no `Data\RISE\GrowLancer` overlay)
- Build: PASS (Release x86; Slayer runtime QA and effect bridge linked)
- Contract/data tests: PASS
- Runtime process: PASS (`Engine-Slayer S21 - Slayer`, PID 22224, responsive after the sequence)
- Runtime capture: PASS (105 frames from the isolated client window)

Rebuild checkpoint after the DB-class compatibility adapter:

- Client Release/QA build: PASS (x86; `SlayerRuntimeQA=true`, `SlayerIsolatedBuild=true`)
- QA binary SHA-256: `46DA9A73B232C9DF32A2DE8D2D9C28594BA3F32BCC717B2FD0C232DA42509603`
- GameServer compatibility build: PASS (isolated `GameServer-Slayer-S21.exe`)
- Static class marker check: PASS (`DBClass` 144/145/146 -> legacy slot 1; packet byte `0xE0` -> client marker 7)
- Fresh remote startup: reached `winhook complete` but did not reach server/character auto-select in the observation window; no new skill-cast evidence is claimed from this attempt.

The captured in-game session recorded accepted native receive/effect chains for all five skills:

| Skill | Runtime evidence |
|---|---|
| 292 Sword Inertia | accepted receive probe; three visible sword projectiles |
| 293 Bat Flock | accepted receive probe; visible bats and four one-second DOT ticks |
| 294 Pierce Attack | accepted receive probe; visible sword/impact ring |
| 295 Detection | accepted receive probe; visible cyan detection mark around the actor |
| 297 Demolish | accepted receive probe; self-buff graph and particle create/render-submit telemetry |

All five skills were exercised in one automated in-game sequence in PID 22224,
not as five independent test runs. The runtime used converted, hash-pinned S21
models, textures, sounds and player actions from the owner's local reference
client; those proprietary assets are intentionally not committed here.

This proves the 5.2 client dispatch/effect/render path for all five rows. The sequence injects a
local receive probe after exercising the production cast path, and uses the
explicit isolated `SlayerAutoQA.enable` fixture to bypass the legacy class gate,
because the
connected remote GameServer has not received this branch. It therefore does
not prove class-9 persistence/protocol migration, remote server-authoritative damage, minimap detection semantics,
reconnect/persistence, or final owner acceptance.
