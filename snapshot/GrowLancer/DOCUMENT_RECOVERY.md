# Grow Lancer document recovery — 2026-09-13

Result: both historical documents restored; skill-port status remains IN_PROCESS.

## Method and provenance

No tracked versions were found in the worktree's Git history. Recovery used
the existing task history, session `01a08930-234f-7691-aaff-baf9bce1787a`,
from the initial document creation through the last successful document update
at `2026-09-13T01:38:13.251Z` (08:38:13 Asia/Bangkok).

The recovery tool decoded literal patch strings without executing historical
commands. Failed tool calls were excluded. It replayed exact-context patches
and inspected literal text/hash replacement loops into isolated candidate files
on C:. All 406 document operations replayed with zero context failures. The
resulting byte sizes match both zero-filled originals exactly. This is strong
reconstruction evidence, but no independently recorded pre-corruption document
SHA-256 was available to prove byte-for-byte equality against an intact original.

## Restored documents

| File | Bytes | Lines | SHA-256 |
| --- | ---: | ---: | --- |
| GROW_LANCER_STATE.md | 355022 | 4710 | 578f704d6718ff976374472aad9150039358863fa51baf2945c2e56e4708a8dc |
| GrowLancer/RUNTIME_QA.md | 7731 | 128 | c05dfedc29ac86fe4f5a057e99b096c536a8797e24922e652f6f7abaf3024f73 |

Both files were restored to the isolated Grow Lancer worktree and read back
with matching SHA-256. The restored contents are historical, not rewritten
summaries. No source code, client assets, executable or production files changed.

## Preserved damaged originals

Local-only directory:
`C:/Users/DELL/Documents/Codex/2026-09-10/task-ri-ng-port-dark-lord/work/recovery-originals-20260913/`.

- GROW_LANCER_STATE.md: SHA-256
  `806aa3e89d821bb2fa3c820203f9e61f828a57dc709038f60015440a8ffb227c`.
- GrowLancer/RUNTIME_QA.md: SHA-256
  `eda0ee2bb87f2e904e0586462d9dfd6728abab20ab353ad4e2f2a58a69ce9fc7`.

Every original hash was checked before restoration, and the preserved copies
were hash-verified before either original path was replaced.

## Audit and limitations

Local recovery scripts and replay audit are under the task's `work/` directory:
`index_recovery_history.py`, `recover_documents.py`, and
`recovered-documents/REPLAY_AUDIT.json`. Raw task history and extracted tool
outputs are not included in the public source snapshot.

The recovered QA document records the Circle Shield F9 paired-actor fixture and
the historical staged `E6FED7D0...` executable. It does not say the fixture passed
in-game. The long state contains older next-action fields alongside newer
checkpoint entries: read chronologically and verify against source. The stage
verifier still pins the older `0EA22D64...` executable and needs reconciliation
before another runtime-stage claim. No new build or runtime QA was performed
for document recovery. Final new-class/server/visual acceptance remains open.
