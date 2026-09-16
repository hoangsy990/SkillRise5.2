# Source Grow Lancer item requirement descriptor — 2026-09-15

Original S21 GameServer `D:\GameServer S21\Data\Items\ItemList.xml`
SHA-256 `61711D9E6B0E6F14028191AA739AAD4A3B0C5911CDB3468A3BC61B988B12AF78`
has per-item `GrowLancer` values `0/1/3/4/5` and section/index fields;
the file comments identify those as evolution-dependent equip rules.
The native PC 5.2 `MAX_ITEM_TYPE=16`, `MAX_ITEM_INDEX=512` and
`ITEM_ATTRIBUTE::RequireClass[MAX_CLASS=7]` are distinct dimensions.
Changing the seven-column field would break its fixed Item.bmd layout;
indexing it with base class7 is out of bounds.

The private generated descriptor
`RISE/GrowLancerItemRequirements.generated.h` stores the exact 869
nonzero S21 values for native-capacity sections0..15: stage1=767,
stage3=34, stage4=51, stage5=17. 757 eligible source rows in newer S21
groups are explicitly excluded rather than mapped to a SS6 item type.
Section3 item13 `Lapid Lance` maps to native item type1549 and has source
value1; section0 item3 `Katana` has source value0. This is requirement
provenance only: GL evolution-stage packet mapping, new-group ID capacity,
native Item.bmd/model/slot/stat parity, and GS authoritative item check
remain OPEN. The generated table is not yet used to grant equipability.

`extract_gl_item_requirements.py --verify` reparses the pinned XML and
checks every generated row/order; PASS. Focused native Win32 `/W4 /WX`
`GrowLancerItemRequirementsTest.cpp` compile/run PASS for source allowed,
denied, evolved-only and out-of-capacity sample types. The original XML
was read-only and no S21 item catalog was staged into production.

Client legacy inventory/tooltip/guild/set/quest dynamic class lookups now
guard index7 before reading fixed seven-column records. Further inspection
found MuHelper `class_character[7]`, `CProtect::MaxAttackSpeed[7]` and
`ClassAttribute[7]`; all their dynamic class7 reads/writes are now bounded
without resizing saved SS6 structures. This prevents out-of-bounds reads
but denies unproven legacy entitlements/profile rather than borrowing Rage
Fighter. `verify_client_legacy_class_lookups.py` PASS; isolated QA and
non-QA x86 client builds PASS. Current QA EXE SHA256
`6B115C5660128432ADC26FA02A185BFAF7C29FF5BD6F6DD103663D8D723EED36`
was staged only in Grow Lancer RuntimeQA with hash-pinned rollback. Actual
class7 equip/login and in-game visual remain unverified.
