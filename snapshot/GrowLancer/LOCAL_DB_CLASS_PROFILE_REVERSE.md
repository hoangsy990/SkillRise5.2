# Local SQL class-profile cross-check — reference only

Status: `REFERENCE / NOT S21 AUTHORITY`.

## Current isolated GrowLancer_QA restore — 2026-09-15

The owner-approved `GrowLancer_QA` database was restored from a COPY_ONLY,
CHECKSUM backup of the then-current `RISE5.2` database. Unlike the older
read-only snapshot below, this restored QA DB has one `DefaultClassType`
starter row at `Class=112`: STR25, DEX27, VIT32, ENE20, Leadership0,
Life/MaxLife100/100, Mana/MaxMana40/40, map0 position150,126, DbVersion3.
Those values **differ** from the `MuOnline52` candidate below and must not be
called S21 defaults. The older statement that `RISE5.2` had no `Class>=100`
rows is a historical query result, not current QA database state.

Current QA procedure `dbo.WZ_CreateCharacter` exists and has a `tinyint`
`@Class` parameter. Its definition rejects a missing `DefaultClassType` row
with result2, copies starter stats into `Character`, and uses a transaction for
`AccountCharacter`/`Character` writes. Read-only definition inspection was
followed by a synthetic QA transaction: `EXEC dbo.WZ_CreateCharacter
'GLQA9Z01','GLQA9ZC1',112` returned result1 and temporarily produced a
class112 row with STR25/DEX27/VIT32/ENE20 at map0 position150,126. An outer
`ROLLBACK TRAN` left both synthetic AccountCharacter and Character counts0;
the subsequent independent count query again returned `0/0`.
The repeatable QA SQL verifier additionally checks that an absent starter
class111 returns result2 without writing either synthetic row. It refuses any
database other than `GrowLancer_QA`, refuses pre-existing synthetic identities,
and always rolls back the positive class112 fixture. `sqlcmd -b` exit0/PASS
was recorded on 2026-09-15.

This proves only the isolated DB's create/seed/rollback behavior. The live
DS/GS create packet, login round-trip, class7 native consumer and S21 source
provenance remain unproved; no real account or original database was changed.

This report records a read-only query against the local SQL Server instance.
No database, table or row was changed.

## Query and result

Instance: `localhost` (Windows integrated read-only query)

```sql
SELECT Class, Level, LevelUpPoint, Strength, Dexterity, Vitality, Energy,
       Leadership, Life, MaxLife, Mana, MaxMana, MapNumber, MapPosX, MapPosY,
       DbVersion
FROM MuOnline52.dbo.DefaultClassType
WHERE Class >= 100
ORDER BY Class;
```

`MuOnline52` returned base/evolution families `112, 128, 144, 160, 176,
192, 208, 224`. The exact base Grow Lancer row was:

| DBClass | Level | LevelUpPoint | STR | DEX | VIT | ENE | Leadership | Life/MaxLife | Mana/MaxMana | Map | Position | DbVersion |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---|---:|
| 112 | 1 | 0 | 30 | 30 | 25 | 24 | 0 | 110 / 110 | 40 / 109 | 0 | 182,128 | 3 |

The row agrees with the separately pinned read-only candidate
`D:\MU-CrossPlatform\MuServer52\Data\Character\DefaultClassInfo.txt` row 7
(`30 30 25 24 0 110 109 2.0 1.0 2.0 1.0`). The SQL table has no rows for
`114` or `115`, so no Grow Lancer evolution defaults are inferred.

For comparison, the same read-only query returned no `Class >= 100` rows in
local databases `MuOnline` and `RISE5.2`.

## Provenance boundary

`D:\GameServer S21\Data\CalcCharacter.ini` delegates four life/mana factors
to `MuOnline.dbo.DefaultClassType`, but the supplied S21 directory contains
no database backup or connection configuration that binds it to this local
`MuOnline52` instance. Therefore DBClass112 is a strong implementation lead,
not accepted S21 authority. It must not be copied into the RISE class tables
until the database provenance is confirmed or an S21 export is supplied.
