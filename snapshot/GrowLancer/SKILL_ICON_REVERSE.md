# Grow Lancer S21 skill-icon reverse

Status: static chain closed; runtime visual acceptance pending.

## Pinned sources

| Source | SHA-256 | Evidence |
|---|---|---|
| `D:\MU FICA Season 21\Data\Local\skill.bmd` | `AC74B3A175D08B50D8C5C3D5BD8C05420E9F6690BC7A5C6C95EF3D7F8278BF79` | 90,004 bytes = 750 encrypted records of 120 bytes plus CRC |
| `D:\MU FICA Season 21\Data\Interface\newui_skill00.OZJ` | `8C3140438BFE4829507A257BBA9591BDDC858C751944086E431F79207B6FAC29` | 24-byte MU wrapper plus 512x512 JPEG payload |
| `D:\MU FICA Season 21\Data\Interface\newui_skill00_Non_Icon.OZJ` | `658EE7BC6CB19F0654AB783FAA22E41031065F6D36E530BBE573BE899BBEEC19` | disabled-state partner, same 512x512 layout |

The S21 files are read-only. Exact copies are staged under
`Data\RISE\GrowLancer\Interface`; the shared SS6 icon atlases are not
overwritten.

## Skill record evidence

Each 120-byte S21 record is independently decoded with repeating XOR
`FC CF AB`. Records 271..279 decode to the Korean names for Spin Step through
Breche. The 16-bit icon field at offsets 74..75 is respectively 271..279.
This is independent client-table evidence that the icon index is the skill ID;
the server `IconNumber` values are not being used as a substitute.

The same decoded records also reproduce the established level, damage, mana,
BP, range and delay values. Empty records decode to zero, which confirms the
120-byte stride rather than the legacy 88-byte SS6 stride.

## Atlas contract

`newui_skill00` is a direct-ID atlas with 25 columns. Each cell is 20x28 in a
512x512 texture:

```
u = (iconId % 25) * 20 / 512
v = (iconId / 25) * 28 / 512
w = 20 / 512
h = 28 / 512
```

The atlas itself contains numeric index labels in unused cells (0, 25, 50,
...); occupied cells 271..279 contain the nine Grow Lancer icons shown in the
S21 skill catalog. `newui_skill3.OZJ` was explicitly rejected: it has content
only in cells 0..10 and is the legacy 260..270 group, so applying the SS6
`skillId - 260` rule to 271..279 would render black cells.

## RISE 5.2 adapter

- Dedicated native bitmap IDs: 32965 normal and 32966 disabled.
- `CNewUISkillList::LoadImages` loads the exact S21 atlases from the isolated
  Grow Lancer directory.
- `RenderSkillIcon` selects the direct-ID formula only for skill IDs 271..279.
- Existing SS6 skill ranges retain their original atlas and UV rules.
- `UnloadImages` releases both dedicated bitmap records.

No S21 address, object layout, hook or renderer was copied. The adapter uses
the existing RISE `LoadBitmapFile`, `RenderBitmap` and `DeleteBitmap` paths.

