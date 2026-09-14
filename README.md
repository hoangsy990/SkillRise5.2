# Engine-Port S21 — Slayer

This branch ports the five Slayer S21 skills to the RISE 5.2 source snapshot:

- Sword Inertia (292)
- Bat Flock (293)
- Pierce Attack (294)
- Detection (295)
- Demolish (297)

The client mappings come from the captured S21 `main.exe` dump, not from
screenshots. The recovered dispatcher/handler/root/action/sound chains and
SHA-256 are recorded in [`docs/SLAYER_S21_DUMP_CHAIN.md`](docs/SLAYER_S21_DUMP_CHAIN.md).

`slayer/` contains the merge-ready contract, data fragments, native bridges,
and focused tests. `snapshot/` contains the source overlay; existing GrowLancer
files remain present and are not replaced by this Slayer branch.

Status is **IN_PROCESS**. Source/static checks and the isolated client QA pass;
class-9 persistence, authoritative GameServer deployment, and owner in-game
visual acceptance are still open. The isolated executable is named
`Engine-Slayer S21.exe` to avoid collision with GrowLancer QA.
