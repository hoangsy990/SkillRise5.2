# Grow Lancer player-action compatibility adapter

Status: `STATIC PASS`, `RUNTIME NOT TESTED`.

## Proven incompatibility

- S21 `Data\Player\player.bmd` is BMD v0F, SHA-256
  `E0F4CB5A0956192D04026135766C566BC6B9B7B559D2F196F74D24D28E2E755A`,
  with 60 bones and 410 actions.
- RISE `D:\RISE-CrossPlatform\Client\Data\Player\player.bmd` is BMD v0C,
  SHA-256
  `2964E7C41DCF686B79E53A74261192EBFC6879D36BC030C2E503EF0B8CEF38EF`,
  with the same canonical 60-bone names/parents/dummy layout and 284 actions.
- `compare_player_actions.py` proves every numeric action 185..194 has different
  key data between S21 and RISE. Reusing those numbers would play unrelated
  SS6 clips.

## Isolated append mapping

`merge_player_actions.py` preserves all 284 RISE action chunks and bone headers,
then appends the ten S21 clips without changing global SS6 action numbers:

| S21 | RISE isolated |
|---:|---:|
| 185 | 284 |
| 186 | 285 |
| 187 | 286 |
| 188 | 287 |
| 189 | 288 |
| 190 | 289 |
| 191 | 290 |
| 192 | 291 |
| 193 | 292 |
| 194 | 293 |

The generated isolated file is
`ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\Player\player.bmd`, SHA-256
`0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63`,
with 294 actions. Structural parsing proves the first 284 action hashes remain
byte-exact and each appended action hash equals its S21 source clip.

The source and production files are read-only. This result proves container
structure and payload identity only; `BMD::Open` loading, skeleton playback and
owner in-game visual acceptance remain mandatory.

