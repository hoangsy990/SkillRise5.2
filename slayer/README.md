# Slayer S21 overlay for RISE 5.2

This directory is an isolated, engine-independent port contract for the four
Slayer skills documented by Webzen. It follows the integration discipline used
by `snapshot/GrowLancer`: keep the native 5.2 source and the S21 reference
checkout unchanged until the class, packet and persistence ABI are verified.

Reference material:

- S21 reference checkout: `D:\RISE-CrossPlatform\Source\_PC_GrowLancer`
- Public guide: <https://muonline.webzen.com/th/gameinfo/guide/detail/100>
- Owner's 5.2 data extracts: `C:\Users\DELL\Desktop\Skills`

## Ported contract

| ID | Skill | Config profile | Guide profile | Runtime events |
|---:|---|---|---|---|
| 292 | Sword Inertia | level 30, STR 50, DEX 100, Darkness | level 30 | three boomerang projectiles; one hit per target is a consumer-side collision rule |
| 293 | Bat Flock | level 150, STR 100, DEX 380, Darkness | level 270 | two direct hits, then five-second DOT |
| 294 | Pierce Attack | STR 300, DEX 1100, Darkness | Bat Flock + 10 mastery points | dash, two hits (four when target has Bat Flock), return |
| 295 | Detection | level 350, DEX 800, no element, 5 s cooldown | level 400; bead table says level 300 | minimap mark event; mark lifetime remains unresolved |

`SlayerSkillContractData.h` is the single source of truth for IDs,
requirements, mastery IDs 779/780/781/782/794, the known Pierce bead mapping
(item group 12/index 479), and the recovered regular-damage formulas.
`SlayerSkillRuntime` turns an authorized cast into deterministic client/server
events without assuming a renderer, packet opcode, or class array layout.
`SlayerSkillEffectBridge` and `SlayerSkillDamageBridge` expose narrow sinks for
binding those events to the native effect pool and authoritative damage path;
they do not duplicate the engine's collision, PvP/PvM, resistance or rounding
logic.
Every cast carries a monotonic `castId`; the included Sword Inertia ledger can
therefore enforce one accepted contact per target without suppressing later
casts or different targets.

The merge-ready data fragments under `data/` carry the four exact `SkillList`
rows, the verified Pierce `SkillRequire` row, the five third-master rows, and
the two `SkillSettings` keys. They are wrapped as fragments on purpose: merge
them into the production files only after checking the owner's current file
hash and preserving unrelated rows.

## Activation boundary

The 5.2 snapshot still has seven native class columns (0..6). Slayer's S21
script/table class number is 9, so `SlayerServerCatalog` refuses activation
until both persistence and protocol migrations are explicitly complete.
`SlayerPacketContract` keeps all opcodes at `-1`; no packet number is guessed.
The repository contains no proprietary S21 binaries, skill textures, models or
sound files. This branch is therefore `IN_PROCESS`: contract and focused
runtime behavior are covered, but native class creation, live networking,
asset/effect binding, build integration and owner in-game acceptance remain
open.

Run the focused test from this directory with
`tests\\RunSlayerSkillContractTest.cmd` (or compile the two C++ sources with
C++14). A passing test is source-level evidence only.
