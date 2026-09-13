# Engine-Port S21 — Grow Lancer backup

Source-only recovery snapshot dated 2026-09-13. Status: **IN_PROCESS**.
This backup does not declare the nine skills complete or visually accepted.

## Scope

220 surviving files (4,826,956 bytes) from the isolated Grow Lancer worktree:
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

## Known recovery gaps

Two original documents were found entirely zero-filled after the I/O incident
and deliberately excluded without modifying their originals:

- `GROW_LANCER_STATE.md`: 355,022 bytes.
- `GrowLancer/RUNTIME_QA.md`: 7,731 bytes.

Their hashes and exclusion reasons are in the manifest. Start with
`snapshot/GROW_LANCER_RECOVERY_STATE.md`, the surviving per-skill reverse
documents and `snapshot/GrowLancer/ACCEPTANCE_GATE_MATRIX.md`; these do not
reconstruct the missing full checkpoint/QA history.

The recovery note predates discovery of the second damaged document.
`verify_runtime_qa_stage.py` still pins the older `0EA22D64...` executable;
do not treat it as validation of the latest `E6FED7D0...` build. No new build
or runtime acceptance was performed to create this backup. Final Grow Lancer
class, authoritative server integration and visual acceptance remain open;
DK activation is an isolated test arrangement only.
