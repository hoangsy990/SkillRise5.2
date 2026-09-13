# Grow Lancer S21 native runtime reference

Status: `CAPTURED_NEEDS_FRAME_EXACT_PORT_COMPARISON`

This evidence was captured from the unmodified read-only Season 21 client at
`D:\MU FICA Season 21`.  It is reference evidence only; no file in that client
was modified.  The capture records the game window at 1696x1089 with a nominal
150 ms interval.  Skill attribution follows the owner-confirmed cast order and
the visible selected-skill tooltip.  It is not promoted to exact event timing
where the frame sequence does not independently prove the boundary.

## Capture sets

| Batch | Frames | Duration | Manifest SHA-256 | Result SHA-256 |
|---|---:|---:|---|---|
| `batch_20260912_2136` | 802 | 150093 ms | `E520671C3517BAE7065D9547FAA1C8A3D66B44459099C659ADB1503590867CF6` | `4CD0352E43BE0D6CE01FC25B5E87A0077D38207319641165837F078AD914DD64` |
| `batch_20260912_2138_tail` | 272 | about 51 s | `29755603CD0248F81767EFDF6C7274F962D09424E81E42A04DA15B06098E557E` | capture was stopped externally after Breche; no result file |

The first batch occupies 313,608,241 bytes across 802 JPEG frames plus its
manifest/result.  The tail occupies 107,196,948 bytes across 272 JPEG frames
plus its manifest.  Both batches are under
`artifacts\runtime_qa\s21_reference`; they are isolated from production data.

## Representative native frames

| Skill | Batch/frame | UTC | SHA-256 | Observed layer at this frame |
|---|---|---|---|---|
| Spin Step | `2136/frame_00037.jpg` | `14:35:09.370Z` | `DAF5E5B168D8CDB7C6810D44BF3EF5445AD8BDC47E9587E2CF875FB52FAA6F64` | caster begins the spear/weapon action; major contact is not yet visible |
| Spin Step | `2136/frame_00041.jpg` | `14:35:10.099Z` | `3FFD5023C9D7D7CA068EAB44E05C582C82BFECE913531BB097BDE18A99C9AE4D` | blue vertical target-side column, compact blue/green contact circle and blue specks |
| Spin Step | `2136/frame_00046.jpg` | `14:35:11.076Z` | `627CDCA8A855E630BE710F7C4A287267BA990222D4E0AD4F9FE0316933A292BD` | late contact residue at the target |
| Circle Shield | `2136/frame_00262.jpg` | `14:35:51.436Z` | `8314F18105D195B5D931391FD867CD68E8D06565A7E587F8A46574FF6762E517` | bright blue-white radial burst around caster/near target with outward blue streaks and purple glow |
| Obsidian | `2136/frame_00346.jpg` | `14:36:07.299Z` | `659BB89467DAD3658F60D2A19D0D4645CAD9DE8A28461572B7C811E72F60EFDD` | pale-blue dome/ring around caster, vertical purple-blue column and white-blue core |
| Magic Pin | `2136/frame_00453.jpg` | `14:36:27.385Z` | `C6D5BF04A84C6243456E8306B7672856FB9B7BC0F44610C1EF34D67B4A406A2E` | large white-blue target explosion, purple side orb and lightning around caster |
| Clash interval | `2136/frame_00574.jpg` | `14:36:49.822Z` | `1E19C84E425953D3584396ECB07EF929D53E52B3A45AC38D847942F095388569` | no Clash cast visible; tooltip shows Harsh Strike selection |
| Harsh Strike | `2136/frame_00688.jpg` | `14:37:11.301Z` | `B85B34DCB3B395EF94A2D30C77AE29944B889D0E0DBDD6FF2E927F755D73C4FB` | long orange-red horizontal lance/slash through target plus circular orange caster trail |
| Shining Peak | `2136/frame_00794.jpg` | `14:37:30.975Z` | `7BF2BE0CD1CE94C12181BEEEB3B04EA8478D7F9654C2D1C9D25DA426B7A5F2E9` | intense blue-white/green spear beam through target with blue energy geometry around caster |
| Wrath | `2138_tail/frame_00041.jpg` | `14:38:04.065Z` | `29823101CA42B41EEB6D59E1B2D4ECA4A688B6E1C4B9C4B706B742FA4C43D8F2` | very large cyan-purple octagonal ground field/dome, purple ground lightning and vertical cyan core |
| Breche | `2138_tail/frame_00153.jpg` | `14:38:25.354Z` | `E5B293BDB39A8F17D9FFFEACAD66A0E166FF0AA43F8BE54366806C5F85CA040A` | large red/orange/pink circular sweeping vortex, sparks and bright white contact core |

## Conclusions that the capture supports

- Spin Step cannot be accepted from a caster-only ground ring.  The native
  presentation includes an action/weapon phase and a distinct target-side
  contact phase with a vertical blue component.  The existing 5.2 F7 capture
  had no selected target and did not emit the separately mapped internal skill
  280 contact root, so that run is a valid visual failure but not a like-for-like
  test of the complete two-message chain.
- Circle Shield, Obsidian, Magic Pin, Harsh Strike, Shining Peak, Wrath and
  Breche all have substantial native layers suitable for frame comparison.
  Compilation or a visible root alone is insufficient for their acceptance.
- Clash is **not captured**.  The owner reported that it could not be cast in
  this runtime state and the recorded interval contains no independent Clash
  effect.  No visual conclusion for Clash may be inferred from this batch.

## Next comparison action

Run the isolated 5.2 QA client with an explicitly selected live target.  For
Spin Step record F7 and the separate F9 internal-280 contact in one continuous
sequence, then compare action onset, caster root, target contact, child timing
and cleanup against frames 37..46.  Repeat the same frame-matched process for
the other captured skills; keep Clash open until a native cast can be produced.
