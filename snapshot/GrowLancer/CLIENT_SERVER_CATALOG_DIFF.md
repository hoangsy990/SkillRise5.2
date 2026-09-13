# Grow Lancer S21 client/server catalog split

Status: VERIFIED DIVERGENCE. Do not merge the two profiles implicitly.

## Sources

| Profile | Source | SHA-256 |
|---|---|---|
| S21 client bundled with the reference client | `D:\MU FICA Season 21\Data\Local\skill.bmd` | `AC74B3A175D08B50D8C5C3D5BD8C05420E9F6690BC7A5C6C95EF3D7F8278BF79` |
| Supplied server configuration | `C:\Users\DELL\Desktop\Skills\SkillList.xml` | `3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0` |

The client table has 750 independently XOR-decoded records of 120 bytes. The
stable fields used below are name bytes `0..31`, level `32`, damage `34`, mana
`36`, BP/AG `38`, range `40`, delay `44`, and icon `74`. The icon value is a
16-bit direct atlas index. Requirements after the common prefix are not treated
as equivalent to the server XML because the S21 client layout/encoding differs.

## Proven comparison

| ID | Skill | Client level/dmg/mana/AG/range/delay | Server level/dmg/mana/AG/range/delay | Result |
|---:|---|---|---|---|
| 271 | Spin Step | 83/100/12/0/2/500 | 83/100/12/0/2/500 | exact |
| 272 | Circle Shield | 220/0/100/50/0/0 | 220/0/100/50/0/0 | exact |
| 273 | Obsidian | 74/0/50/50/0/0 | 74/0/50/50/0/0 | exact |
| 274 | Magic Pin | 30/80/30/20/2/500 | 30/80/5/3/2/500 | cost differs |
| 275 | Clash | 0/50/50/50/6/10000 | 0/50/50/50/6/10000 | exact |
| 276 | Harsh Strike | 74/100/12/0/3/500 | 74/100/12/0/3/500 | exact |
| 277 | Shining Peak | 92/70/50/20/4/500 | 92/50/8/5/4/0 | damage, cost and delay differ |
| 278 | Wrath | 66/0/100/50/0/60000 | 66/0/40/30/0/0 | cost and delay differ |
| 279 | Breche | 44/400/40/20/3/500 | 300/230/15/6/5/0 | all six fields differ |

## Port policy

- Client identity, icon, action and visual reverse remain pinned to the exact
  S21 client binary/data pair.
- The current RISE client catalog seed and GameServer contract use the supplied
  `SkillList.xml` values because they must agree with the server configuration
  that will authorize casts and calculate damage.
- No claim is made that the supplied server configuration is the exact server
  paired with the reference client. Five rows match and four rows diverge.
- Runtime activation stays fail-closed until the recipient class and packet
  adapter are approved. This prevents a client-displayed cost from silently
  disagreeing with server authority.

The executable verifier is `tools\grow_lancer\verify_catalog_profiles.py`.
