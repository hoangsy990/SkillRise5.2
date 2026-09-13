# Grow Lancer S21 third-master mapping evidence

Status: `STATIC_EVIDENCE_COMPLETE`, runtime handlers remain outside the base-nine
port until the recipient class and authoritative server dispatch are available.

## Pinned sources

- `C:\Users\DELL\Desktop\Skills\SkillList.xml`, SHA-256
  `3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0`.
- `C:\Users\DELL\Desktop\Skills\SkillTreeData_3rd.xml`, SHA-256
  `CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0`.
- `C:\Users\DELL\Desktop\Skills\MasterSkillCalc_3rd.lua`, SHA-256
  `771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699`.

## Active tree rows

`Brand` below comes from `SkillList.xml`; `Parent` and point gates come from
`SkillTreeData_3rd.xml`. They are independent fields and are not normalized.

| ID | Name | Brand | Parent | Min/max points | Mana/BP | Range | Buff | Lua formula |
|---:|---|---:|---:|---:|---:|---:|---:|---|
| 687 | Spin Step Strengthener | 271 | 0 | 1/20 | 14/0 | 2 | 0 | yes |
| 690 | Spin Step Mastery | 687 | 687 | 1/20 | 16/0 | 2 | 0 | yes |
| 688 | Harsh Strike Strengthener | 276 | 0 | 1/20 | 14/0 | 3 | 0 | yes |
| 691 | Harsh Strike Mastery | 688 | 688 | 10/10 | 16/0 | 3 | 0 | yes |
| 692 | Magic Pin Strengthener | 274 | 0 | 1/20 | 17/10 | 2 | 0 | yes |
| 695 | Magic Pin Mastery | 692 | 692 | 10/10 | 20/13 | 3 | 0 | yes |
| 693 | Obsidian Strengthener | 273 | 0 | 1/20 | 50/50 | 0 | 220 | yes |
| 696 | Breche Strengthener | 279 | 0 | 1/20 | 16/10 | 5 | 0 | yes |
| 698 | Breche Mastery | 696 | 696 | 10/10 | 16/10 | 6 | 0 | yes |
| 699 | Shining Peak Strengthener | 277 | 698 | 1/20 | 10/7 | 4 | 0 | yes |
| 703 | Circle Shield PowUp | 272 | 0 | 1/20 | 100/50 | 0 | 221 | no |
| 706 | Circle Shield Mastery | 703 | 703 | 1/20 | 100/50 | 0 | 222 | no |
| 895 | Wrath Strengthener | 278 | 0 | 1/10 | 50/40 | 0 | 425 | no |

The unusual Shining Peak relation is exact: row 699 has `Brand=277` but the
active tree has `ParentSkill1=698`. It is preserved as source evidence and not
rewritten into an inferred chain.

## Formula coverage

- 687 and 690 use the same Spin Step primary/explosion formula as base 271.
- 688 matches base Harsh Strike's two barrage multipliers; 691 adds a proven
  third barrage multiplier of `1.2`.
- 692 and 695 use the same three Magic Pin multipliers as base 274.
- 696 and 698 use the same Breche formula as base 279.
- 699 uses the same Shining Peak formula as base 277. The Lua comment says
  `SkillID: 688`; the function name and both authoritative tables identify 699,
  so the mismatching comment is recorded as a source typo rather than copied.
- 693 uses the same Obsidian value `Strength/20` and 240-second duration.
- No matching function for 703, 706 or 895 exists in the pinned master Lua.
  Their `Damage` fields (10, 23 and 312) are retained as raw catalog values and
  are not interpreted as effect equations.

## Legacy Wrath rows

`SkillList.xml` still contains `708 -> 709 -> 710` through `Brand`, with buffs
223, 224 and 225 and 60-second delays. None of those IDs appears in the active
Grow Lancer section of `SkillTreeData_3rd.xml`; the active row is 895. Therefore
708..710 are legacy catalog evidence only and are excluded from the active
master descriptor/runtime contract.
