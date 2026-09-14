# Slayer runtime QA status

Last automated check: 2026-09-14 (Asia/Bangkok)

- Isolated checkout: `D:\RISE-CrossPlatform\Source\_PC_Slayer`
- Binary: `Engine-Slayer S21.exe` (private `SlayerBuild\RuntimeQA\Client` stage)
- Account/character requested by owner: `cuongdo-cuongdo` / `CuongDo`
- Stage verifier: PASS (private Player/RISE roots, root login keys, `Config\Mix.bmd`, no `Data\RISE\GrowLancer` overlay)
- Build: PASS (Release x86; Slayer runtime QA and effect bridge linked)
- Contract/data tests: PASS
- Runtime log verifier: PASS

The captured in-game session recorded accepted event chains for all four skills:

| Skill | Runtime evidence |
|---|---|
| 292 Sword Inertia | three sword-projectile events per accepted cast |
| 293 Bat Flock | two bat-hit events, 5,000 ms DOT, and one-second ticks |
| 294 Pierce Attack | dash, four hits with Bat Flock target fixture, and return |
| 295 Detection | self detection-mark event and 5,000 ms cooldown rejection |

This is adapter/bridge QA evidence, not final owner acceptance. Native
proprietary Slayer effects/assets, real server-authoritative damage, minimap
rendering, reconnect/persistence, and visual preview are still open and must
not be inferred from the event log.
