# Grow Lancer lance item/model chain (isolated candidate)

Source S21 item registry `D:\MU FICA Season 21\Data\Local\item.bmd`
SHA-256 `9B9D62B7F3BA085D31B4AEFBCC9DE5DB505C48D45C336A18CAD424C93D3B02F6`
maps item group3 indexes13..19 to seven source `Data\Item` BMDs. Source
`D:\GameServer S21\Data\Items\ItemList.xml` SHA-256
`61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78`
gives Grow Lancer stages1 for13..18 and3 for19, slot0, and the distinct
strength/dexterity requirements. `inspect_gl_lance_item_chain.py` pins each
original model SHA-256, source item metadata, and embedded mesh texture name.

| Source item | Model BMD | Native Open2 meshes/bones/actions | Embedded textures |
| --- | --- | --- | --- |
| 3:13 Lapid Lance | RapideLance.bmd | 1/8/1 | RapideLance.jpg |
| 3:14 Conmocion Lance | ConmocionLance.bmd | 1/3/1 | ConmocionLance.jpg |
| 3:15 Pluma Lance | PlumaLance.bmd | 3/22/1 | PlumaLance.jpg, gra.jpg, flareBlue.jpg |
| 3:16 Vis Lance | VisLance.bmd | 2/15/1 | VisLance.jpg, flame_chrom.jpg |
| 3:17 Prickle Lance | PrickleLance.bmd | 2/5/1 | PrickleLance.jpg, rgb_mix.jpg |
| 3:18 Alacran Lance | AlacranLance.bmd | 1/5/1 | AlacranLance.jpg |
| 3:19 Bloodangel Lance | bloodangellance01.bmd | 2/9/1 | bloodangellance01.jpg, bloodangellance02.jpg |

All seven source BMDs are v0F, not directly the native 5.2 v0C format.
`stage_gl_lance_candidates.py --apply` pins source hashes, converts a copy
v0F to native v0C with plaintext roundtrip verification, validates the mesh
table, and copies the twelve original hash-pinned OZJ/JPEG textures into
`ExMain_RISE_PC\Tests\GrowLancerBuild\ClassItems\Data\RISE\GrowLancer\ClassItems`.
The original S21 Data is read-only. Dry-run and idempotent hash checks PASS.

Offline QA PID42344 on the staged QA EXE SHA-256
`F0112DA56BD1D33A91B283707146D5A68527D0D10E8932A770F05E0AAAC7DC04`
returned exit0. `verify_gl_lance_native_probe.py --pid 42344` checks its
log: native `BMD::Open2` loaded all seven models with expected counts,
`valid=1`, then native release returned for all seven. Textures were
intentionally not loaded (`texturesLoaded=0`); this proves only geometry
parse/release, not GPU texture registration, item equip, render, or gameplay.

Current native QA `Data\RISE\Config\Item.bmd` SHA-256
`C5A5693D6C94F07A65ED49E02AB598644458B38ADA7CA5B92B429DAE0D52AECA`
has empty records for all seven lance item types1549..1555 (name, dimensions,
requirements and seven class columns zero). GameServer `ItemManager.cpp`
reads only seven SS6 class columns and `CheckItemRequireClass` rejects base
class7. Hash-pinned native QA GS `Data\Item\Item.txt` has no group3
indexes13..19; the existing bow/crossbow/arrow rows13..19 are in group4,
not group3. `verify_gl_lance_item_id_collision.py` checks actual section
boundaries and rejects any future group3 ID collision. Client-only
entitlement enablement would be false progress; source
slot/stat/model metadata, authoritative GS item record and stage mapping
must be integrated together before class7 equip QA. No candidate was staged
to production client or used to alter another task's assets.

Superseded 2026-09-15: the owner narrowed this task to the nine skills and
explicitly excluded equipment and class-change quests. The short-lived
metadata adapter compiled in isolated QA but never replaced the live GS;
its client/GS runtime hooks and descriptor were removed. The isolated
model candidates and original hash provenance remain read-only reference,
not a current implementation task. Current QA skills-only client SHA-256
`B7049F50D3689B0269CBED246E74E3C7F15B03355CC0060E8CA1C77582ECC2BF`
is staged with a hash-pinned rollback. No lance item is authorized or
registered by this task.
