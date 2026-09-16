# Engine-Port S21 — Grow Lancer backup

Source-only recovery snapshot refreshed 2026-09-16. Status: **IN_PROCESS**.
This backup does not declare the nine skills complete or visually accepted.

## Scope

542 files (8,488,041 bytes) from the isolated Grow Lancer worktree (including
restored documents):
client/server changes, skill descriptor, reverse notes, provenance metadata,
focused tests and tools. Shared integration files are stored in full, not as
patches. Wind Soul implementation and the Pegasus worktree are not included;
the Wind Soul lessons document is reference material for Grow Lancer only.

No original RISE Git history, raw memory dumps, client assets, executable
outputs, production configuration or runtime captures are included.
This is **not a standalone buildable checkout or a complete machine backup**.

Source branch: `feature/grow-lancer-skills-s21`.
Required RISE base commit: `f2e8e558874005708312becbb8273ecb8d7d61bf`.
Files under `snapshot/` preserve their paths relative to that checkout.

## Recovery

1. Run `powershell -NoProfile -File ./Verify-Backup.ps1` from this repository.
2. Obtain the original RISE repository separately and create a new isolated
   checkout at the base commit above. Do not overlay a working production tree.
3. Review and copy the files under `snapshot/` into that isolated checkout,
   preserving relative paths. Shared files require comparison before merging
   into any later branch. No automatic merge or deployment is performed here.
4. Restore required assets and binary reverse evidence separately from trusted
   originals, using the provenance documents. Review tool paths before running:
   several tools refer to the original local Windows directories.
5. Revalidate source, build, server authority and in-game visuals independently.

`BACKUP_MANIFEST.json` records every snapshot file's size and SHA-256.
Hash verification proves this backup's byte integrity, not correctness or
completeness of the port. Git text conversion is disabled to preserve bytes.
The earlier manifest described only 223 files while the committed snapshot
already held 445; this refresh reconciles all 542 files and passes
`Verify-Backup.ps1`. Use `Update-Backup.ps1 -SourceRoot <isolated checkout>
-DryRun` to preview a future source-only refresh before running it without
`-DryRun`.

## Document recovery

Two original documents were found entirely zero-filled after the I/O incident
and excluded from the initial backup. Both have now been reconstructed from
successful task-history writes and restored:

- `GROW_LANCER_STATE.md`: 355,022 bytes.
- `GrowLancer/RUNTIME_QA.md`: 7,731 bytes.

All 406 recovery operations replayed with zero context failures; sizes match
the originals. No independent pre-corruption document hash was available, so
this is history-based reconstruction, not a certified intact-original match.
Damaged originals were preserved locally before restoring the original paths.
See `snapshot/GrowLancer/DOCUMENT_RECOVERY.md` for hashes and provenance.
The manifest retains the initial incident records separately from restored files.

The recovered files retain historical statements, including older next actions.
`verify_runtime_qa_stage.py` still pins the older `0EA22D64...` executable;
do not treat it as validation of the latest `E6FED7D0...` build. No new build
or runtime acceptance was performed to create this backup. Final Grow Lancer
class, authoritative server integration and visual acceptance remain open;
DK activation is an isolated test arrangement only.

## Latest checkpoint

The isolated PC Win32 normal and RuntimeQA clients linked at this checkpoint
(SHA-256 `F352A9C53E92AFCAFD0CDE1EE6723CA1228C0331F907FF1D3B63E2879B8C097C`
and `6C4B52E04D7286FB1F2FFDB823042F9110E5594A643DEF3EDF582B36196C9E9C`).
These executable outputs are deliberately **not** in this repository.
Normal fixed-action preparation now checks the owning exact base-class-7
actor; the explicit QA Dark Knight preview remains separate. Class-7 initial
speed/GS authority, Magic Pin's ordinary 40 ms activation and owner visual
acceptance for all nine skills remain open. A successful build or manifest
verification does not close those gates.
