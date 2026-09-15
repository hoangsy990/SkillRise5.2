# Grow Lancer class consumer reference (read-only newer source)

Status: `REFERENCE_ONLY` — this document records a port map; no file from the
newer source tree is copied into SS6 and no class slot is enabled by it.

Current-worktree delta (2026-09-15): GS now reserves capacity8 and a source
class7 identity, while client base7 has a private unevolved selector. The
historical MAX_CLASS7/no-CLASS_GL statements below describe the original SS6
baseline, not current capacity. Create/load/defaults/rates/skills remain
closed. The separate active `Add classes and items to Rise` clone uses RF
compatibility presets for GL; see `ADD_CLASSES_CROSS_TASK_AUDIT.md`. Do not copy newer
presets or that clone as S21 handler/default proof.

## Scope and provenance

The comparison source is
`D:\MU-CrossPlatform\SRCMainGS\Source\GameServer\GameServer`. It is a
source reference only. The direct S21 server package supplied for this task has
`IGC.GameServer2_R.exe` but no handler source, PDB, MAP or project dump, so the
newer source cannot be treated as S21 runtime proof.

Pinned reference file hashes (SHA-256):

| file | hash |
|---|---|
| `DefaultClassInfo.h` | `0726F56BD5004EA5310BF5BAB3818F0AA0D142635900D9232CB154FA1EB5E1D1` |
| `Protocol.cpp` | `5CCC2E60ED3D8BBCE68186F8292C850A8694BCD37C1B5885DC2BA0E2BF0BCC63` |
| `ObjectManager.cpp` | `F8D515A1589E8BF8E93362657ACBBF04C9F66BA5EAE14A37CF99D94089B49C52` |
| `ServerInfo.cpp` | `52AD94DD946A9D13DDE4ED39ACF2F592B4A80AE47ABD7861985EEC3D52C24918` |
| `User.cpp` | `AD242CE75840193035FAA80496D3EDB862020CDB7ABF3792D5C9743041D996A3` |
| `Attack.cpp` | `EE22F6877D42E34C27BF90328AD9077830C78A9B7C0563D0B08758F87E39D83C` |
| `Item.cpp` | `D283C33FA15C79D5101AE1D9C6513ECFE29B43560CD7D4B57F0BA4514738C17F` |
| `CommandManager.cpp` | `C30F20A3BDB0672C7D9B21A40899C5240B6CE75EDE61FB08E773F0A74B1D0536` |

## Exact newer-source leads

- `DefaultClassInfo.h` declares `CLASS_GL = 7`, `DB_CLASS_GL = 112`,
  `DB_CLASS_ML = 114` and `DB_CLASS_GL_4 = 115`. It also uses a larger
  `MAX_CLASS` under the newer update guard. The enum/array shape is a lead,
  not a safe SS6 change.
- `Protocol.cpp::CGCharacterCreateRecv` admits `DB_CLASS_GL` and gates it on
  `ClassCode & 16`. This is the missing create-time consumer in SS6.
- `ObjectManager.cpp` adds GL branches for BP, physical/magic damage,
  attack-success, attack speed, defense-success, defense and item/stat
  recomputation. The exact constants are read from `ServerInfo` fields with
  the `GL...` prefix.
- `ServerInfo.cpp` loads GL PvP/PvM, directed-class and stuck-rate keys. The
  direct S21 `CalcCharacter.ini`/`StatSpecialize.xml` evidence is kept
  separately; these newer INI keys must not be assumed to be S21 values.
- `Item.cpp` includes `ItemInfo.RequireClass[CLASS_GL]` in the class-specific
  weapon/armor branches.
- `CommandManager.cpp` includes GL in class/evolution checks and DB class
  ranges. `User.cpp` and `Attack.cpp` add GL-specific behavior in combat and
  character validation.

## SS6 delta and safe next step

The isolated SS6 files currently have no `CLASS_GL`/`DB_CLASS_GL` symbols and
retain `MAX_CLASS = 7`; their SHA-256 values are recorded in the state audit.
Because this touches fixed-size arrays, character-create packets, DS/GS
round-trip, persistence, item class masks, stat formulas and every class
consumer, a single enum or `MAX_CLASS` edit would be unsafe.

Next implementation step is therefore a separately tested class-capacity and
wire contract: enumerate every SS6 array/packet field, prove its width, add a
private adapter only where the native field can represent the S21 value, then
run create/login/persistence and stat consumers in an isolated server. Until
that evidence exists, `GrowLancerClassAdapter.h` remains fail-closed and the
nine skill IDs remain catalog/render contracts only.

## Explicit non-copy rule

Do not copy newer `DefaultClassInfo.*`, `ObjectManager.cpp`, `Protocol.cpp`,
`ServerInfo.cpp`, `Attack.cpp`, `Item.cpp`, `CommandManager.cpp` or `User.cpp`
wholesale into SS6. Use this table to drive one-file/one-contract audits and
keep any future implementation scoped to `feature/grow-lancer-skills-s21`.
