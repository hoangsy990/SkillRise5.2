# Pegasus high-skills recovery snapshot

Source-only recovery overlay, 2026-09-13. This is NOT a standalone game client
and NOT a declaration that the twelve skills are finished.

## Restore

1. Obtain `https://github.com/hoangsy990/Rise-CrossPlatform` at commit
   `f2e8e558874005708312becbb8273ecb8d7d61bf` in a NEW isolated checkout.
2. Verify every file in `source/` using `BACKUP_MANIFEST.json` SHA256 values.
3. Overlay `source/` on that checkout, preserving relative paths. Apply any
   `deleted_paths` listed in the manifest (currently none).
4. Read `PEGASUS_IMPORT_STATE.md` before building. Re-stage authorized original
   assets using the included staging scripts and their pinned hashes.

Included: all changed/new Pegasus source, server/client integration files,
test sources/scripts, catalogs and recovery/audit documents in the allowlist.
Excluded: original game assets/binaries, generated engines/server binaries,
runtime client, QA logs/videos, account data, caches, user configs and build
outputs. Therefore keep the base source and authorized asset sources too.

## Verified state

39/39 source gates passed after I/O recovery. RuntimeClient and Delivery were
restaged and verified against engine SHA256
`B923614BC008793288F3E50DB8D4AF2DE8126B6C46E0F67ECA787C64013F46EA`,
plus 173 assets. These binaries/assets are deliberately not in this backup.
Latest live QA remains QA5544, before the Spiral mounted-action and Deathside
safe-zone fixes. Mounted runtime, proper-class visual parity, Shining Bird
authoritative damage and charge-stat/third-Havoc evidence remain open.

The existing Grow Lancer snapshot and main branch were not replaced.
