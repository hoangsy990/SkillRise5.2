# S21 Grow Lancer class-data evidence

Status: `REFERENCE_ONLY` / `IN_PROCESS`.

This is a read-only extraction from the supplied `D:\GameServer S21\Data`
tree. It records values that can safely feed a future class adapter; it does
not raise SS6 `MAX_CLASS`, change packet widths, or enable class 8.

## Pinned source files

| file | bytes | SHA-256 |
|---|---:|---|
| `CalcCharacter.ini` | 46416 | `BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D` |
| `StatSpecialize.xml` | 11529 | `D79F1CBBE8A6FD7207499CC5198E2F0E99A5BE06E2607C0A8B4879A3378E9AC9` |
| `ClassQuest.xml` | 25207 | `5B2CE593AF8694F55061BAEFD95CC566E9CD4AAA2491CB0BB9819FF87201D6AC` |
| `Warps\ClassQuest.xml` | 24239 | `20787D902A8B58208804E1989F6EE50AC301E689CCF167511964C16A448724F3` |

## Exact S21 values

`CalcCharacter.ini` contains the following Grow Lancer keys: `GLLevelUpPoint
= 7`, `GLLevelUp3rdTreePoint = 1`, `GLLevelUp4thTreePoint = 1`,
`GLMaxAttackSpeed = 273`, `GLDamageDevideToSD = 90`,
`GLSuccessAttackRateOption = 20000`, `GLShieldGageConstA = 12`,
`GLShieldGageConstB = 30`, `GLDamageStuckRate = 10`, and
`GLPvMDamageRate = 555` in the primary PvM section. The same file also has
later optional PvP/PvM sections (`GLPvMDamageRate = 100.0` and repeated 56/100
tables); these are separate configuration sections and must not be flattened
into one SS6 constant.

`StatSpecialize.xml` class `ID="8"` has two options, both using limits
1500/2000/3000 and values 10/30/45:

* `StatType=1, Index=1` (strength attack power)
* `StatType=2, Index=4` (agility defense)

`CalcCharacter.lua` (SHA-256
`9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70`)
defines source base class7 and the GL fist values as `Strength/8 +
Dexterity/10` for each minimum hand and `Strength/4 + Dexterity/6` for each
maximum hand. Magic is `Energy/9` minimum and `Energy/4` maximum. The source
comment names Grow Lancer and Mirage Lancer; it does not identify every
intermediate stage/packet encoding. The native source adapter keeps the Lua
fractions as `double` before OBJECT integer conversion; that conversion and
item/skill damage remain OPEN, so no DK/RF formula is used as a substitute.
Focused x86 /W4 /WX source-value tests and pinned Lua verifier pass.

`ClassQuest.xml` and `Warps\ClassQuest.xml` contain explicit
`GrowLancer="0/1/2/3/4"` columns in the class/evolution job rows. They prove
that Grow Lancer eligibility is represented in S21 data (the separate
`StatSpecialize.xml` ID8 is not the source base-class byte7), but do not by themselves
prove the SS6 create/login/persistence ABI or a skill-learning grant path.

Reproduce the hash and value checks with
`tools/grow_lancer/verify_s21_grow_lancer_class_data.py`.
