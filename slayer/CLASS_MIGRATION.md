# Native Slayer class migration gate

The five skill contracts are ready, but the supplied 5.2 repository is a
source-only Grow Lancer recovery snapshot and still owns seven class columns.
Slayer's verified S21 class number is 9. The following boundaries must be
migrated together before setting the runtime `slayerClassEnabled` gate in a
production build.

The S21 `CalcCharacter.lua` declares `CLASS_SLAYER = 9`. The local 5.2 SQL
reference contains a `DefaultClassType` row for DB class `144` with the
Slayer-shaped starting attributes `STR=28, DEX=30, VIT=15, ENE=10` and
`Life/MaxLife=130`, `Mana/MaxMana=10`. Under the existing DB convention this
gives the stage candidates `144/145/146`; the shared contract records these
values for validation only. The SQL row is not applied automatically because
the authoritative owner database and protocol round-trip still need proof.

| Boundary | Native 5.2 areas to migrate | Required proof |
|---|---|---|
| Server class constants/defaults | `ExGameServer/GameServer/DefaultClassInfo.h/.cpp`, `CustomStartItem*.h/.cpp` | `MAX_CLASS` capacity, class 9 defaults, creation/evolution names and start map |
| Server config readers | `SkillManager.cpp`, `ItemManager.cpp`, `ItemBagEx.cpp`, quest readers, `MasterSkillTree.cpp` | New 10-column grammar consumes legacy rows safely and preserves following row IDs |
| Server class-sized state | `ServerInfo.h`, `MasterSkillTree.h`, item/quest structures and every `RequireClass` array | No out-of-bounds reads/writes; old seven-column files are migrated or rejected |
| Client class arrays/UI | `_define.h`, `_struct.h`, `ZzzInfomation.*`, `CharMakeWin.*`, `NewUIMuHelper.*`, inventory/class model loaders | Class 9 is selectable, persisted and rendered without shifting legacy model IDs |
| Client/server protocol | `ExGameServer/GameServer/Protocol.h/.cpp`, client `WSclient.h/.cpp` and send helpers | Class byte/evolution and skill list packets round-trip with fixed sizes |
| Persistence/database | character create/load/save and class/evolution fields | Reconnect, relogin and rollback retain Slayer identity and learned skills |
| Assets/effects | player model/equipment files, icons 86–89, Slayer effects/sounds | Licensed assets hash-match and each skill is owner-accepted in-game |

Until each row has evidence, do not raise `MAX_CLASS`, append a tenth value to
legacy tables, or reuse a Grow Lancer hook/address. The current branch keeps
the class metadata and skill events isolated so the original 5.2 checkout is
not mutated accidentally.
