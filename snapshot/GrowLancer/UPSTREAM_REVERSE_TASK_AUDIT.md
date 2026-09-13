# Audit: Reverse toàn bộ skill SS21 (trừ SS6)

Read-only task: `Reverse toàn bộ skill SS21 (trừ SS6)`  
Thread: `01a07f74-c681-7f63-b7ba-8a222d49e7fe`

## Result

The task is useful as a dump/tooling and global asset-string source, but it does
not contain a completed Grow Lancer base-skill chain for IDs 271..279.

- `SS21_MINUS_SS6_inventory.csv` has no rows in 260..290.
- `chain_gap_matrix.csv` has no rows in 260..290.
- The artifact tree does contain global asset-string candidates used as leads,
  including `h_strike_wind01/02`, `magicpin01`, `magicpin03_new`,
  `magicpina01_new`, `magicpina02`, `obsidian/obsidiana` and `wratha01/02`.
- Those strings alone are not dispatcher, child, renderer, movement, lifetime
  or ownership proof. Every Grow Lancer chain in this package was therefore
  traced independently against the pinned S21 memory dump.

Pinned upstream artifacts inspected:

| File | SHA-256 | Grow Lancer 271..279 rows |
|---|---|---:|
| `D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_full_skill_reverse\SS21_MINUS_SS6_inventory.csv` | `5BE16999DB105A8287019097C34D20BB9EF0ADDF6E43C00FD795777A0044C5EF` | 0 |
| `D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_full_skill_reverse\chain_gap_matrix.csv` | `12BD30B70971874AC9FEB40EFA264E9FA1F2340F0BAC8628AD702D870B51341F` | 0 |

## Import policy

Use the upstream task's pinned dump, registries and tools as read-only evidence.
Do not inherit its completion status for Grow Lancer and do not treat a global
asset-string hit as completion of a visual layer.
