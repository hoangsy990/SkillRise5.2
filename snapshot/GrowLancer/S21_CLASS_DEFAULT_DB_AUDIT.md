# Grow Lancer class-default database provenance — 2026-09-15

S21 `D:\GameServer S21\Data\CalcCharacter.ini` SHA-256
`BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D`
explicitly says per-class `LevelLife`, `LevelMana`, `VitalityToLife` and
`EnergyToMana` come from `MuOnline.dbo.DefaultClassType`. The supplied
S21 server directory has no matching `.bak`, `.mdf`, `.ldf` or SQL source
for its original table. This config is evidence of the required source,
not of actual class7 default values.

Read-only SQL queries on both installed SQL Server instances found the
following local rows. The default instance has `MuOnline`, `MuOnline52`
and owner-approved isolated `GrowLancer_QA`; SQLEXPRESS has no user DB.

| Local DB | DB Class | STR/DEX/VIT/ENE | Life/MaxLife | Mana/MaxMana | Provenance |
|---|---:|---|---|---|---|
| MuOnline | 96 | 25/27/32/20 | 100/100 | 40/40 | RF only; no 112 |
| MuOnline52 | 96 | 25/27/32/20 | 100/100 | 40/40 | RF only |
| MuOnline52 | 112 | 30/30/25/24 | 110/110 | 40/109 | Unknown local modification, not pinned S21 DB |
| GrowLancer_QA | 96 | 25/27/32/20 | 100/100 | 40/40 | isolated RISE5.2 restore |
| GrowLancer_QA | 112 | 25/27/32/20 | 100/100 | 40/40 | isolated candidate, copied from RF; QA-only |

The supplied `Rise-S21-Server-Patch.zip` SHA-256
`303C9C671C28D8DF572B7CBB28577A29996D7ECDD1E7F2CD3CBF900F5FB8A79B`
contains `Season21ClassPersistence.sql` explicitly mapping `(112,96)`
(`Grow Lancer <- Rage Fighter`) and calls these values *nearest starter
templates* to replace later with dedicated S21 balance. Its
`DefaultClassInfo.txt` class7 row is a copy of class6. These files are
useful compatibility references, not authority for this strict S21 port.
The other task and its zip were read-only; nothing was merged.

GS `Protocol.cpp::CGCharacterCreateRecv` currently rejects DB112; GS
`ObjectManager.cpp` character-info whitelist rejects DB112. Local
`DefaultClassInfo.txt` has only class0..6. Class7 create/load must remain
closed until an explicit QA profile or true S21 class defaults/rates are
separately identified. A successful local SQL stored-procedure insert is
not evidence that the live GS can load or move class7.

Next required external evidence for balance parity: original S21
`MuOnline.dbo.DefaultClassType` row `Class=112` (and related class rates),
or a S21 GS runtime trace that records those four multipliers and stats.
Do not substitute the local/zip RF-derived rows as confirmed S21 values.
