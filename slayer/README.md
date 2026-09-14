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
`SlayerClassContractData.h` records the guide's class-creation metadata
(Slayer/Royal Slayer/Master Slayer, Lorencia, and starting attributes) for the
later native class migration.
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

The contract exposes separate config, bead and guide stat gates. The runtime
adapter uses the supplied 5.2 config gate; a production owner can select the
guide/bead gate explicitly instead of accidentally mixing the two profiles.

## Activation boundary

The 5.2 snapshot still has seven native class columns (0..6). Slayer's S21
script/table class number is 9, so `SlayerServerCatalog` refuses activation
until both persistence and protocol migrations are explicitly complete.
See `CLASS_MIGRATION.md` for the complete class-sized array/protocol/persistence
gate before enabling the native class.
The runtime adapter additionally requires an explicit Slayer class ID and an
owner-enabled class gate; it cannot be invoked accidentally by another class.
`SlayerPacketContract` keeps all opcodes at `-1`; no packet number is guessed.
The repository contains no proprietary S21 binaries, skill textures, models or
sound files. Hash-guarded conversion/staging scripts under
`snapshot/tools/slayer` consume the owner's local S21 reference data. Native
5.2 dispatch, effect-pool binding, converted player actions and rendering are
implemented for all four skills. The branch remains `IN_PROCESS` until the
matching GameServer build is deployed for authoritative damage/network QA and
the owner accepts the visual result.

## Isolated Slayer runtime QA

The full PC checkout is tested in the separate `D:\RISE-CrossPlatform\Source_PC_Slayer`
worktree so GrowLancer's worktree and QA package are not reused. Its private QA
output is named `SlayerBuild\RuntimeQA\Client\Engine-Slayer S21.exe` and its
working-directory launcher is `tools\slayer\start_runtime_qa_client.ps1`.
The launcher verifies the base `Data\RISE` merge (including `Config\Mix.bmd`),
private `Player/RISE` roots, login keys, and rejects a leaked GrowLancer overlay.
The isolated runtime can select the saved `admin4` credential, enter character
`Slayer`, and execute 292→293→294→295 as one sequence. F8 can trigger the same
sequence manually. The September 14 capture recorded all four visible effects
and the expected Bat Flock ticks in one responsive in-game process. The local
receive probe validates the client pipeline only; server authority and owner
acceptance remain separate gates.

Run the focused test from this directory with
`tests\\RunSlayerSkillContractTest.cmd` (or compile the two C++ sources with
C++14). A passing test is source-level evidence only.
