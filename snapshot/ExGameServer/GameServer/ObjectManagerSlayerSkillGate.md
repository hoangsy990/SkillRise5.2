# ObjectManager Slayer skill gate

Apply this focused change to `ExGameServer/GameServer/ObjectManager.cpp`.
`ObjectManager.cpp` already includes `RISE/SlayerServerCatalog.h`.

In `CharacterInfoSetSkill`, put this branch before the legacy class branches
so persisted S21 DB classes 144/145/146 receive all five ported skills:

```cpp
if (rise::slayerserver::IsSlayerDbClass(lpObj->DBClass))
{
    gSkillManager.AddSkill(lpObj, rise::slayerserver::kSwordInertia, 0);
    gSkillManager.AddSkill(lpObj, rise::slayerserver::kBatFlock, 0);
    gSkillManager.AddSkill(lpObj, rise::slayerserver::kPierceAttack, 0);
    gSkillManager.AddSkill(lpObj, rise::slayerserver::kDetection, 0);
    gSkillManager.AddSkill(lpObj, rise::slayerserver::kDemolish, 0);
}
else if (lpObj->Class == CLASS_DW)
```

In the non-full-skill branch of `CharacterInfoSet`, give a new Slayer only
the S21 starter skill; the other four remain requirement gated:

```cpp
if (rise::slayerserver::IsSlayerDbClass(lpObj->DBClass))
{
    gSkillManager.AddSkill(lpObj, rise::slayerserver::kSwordInertia, 0);
}
else if (lpObj->Class == CLASS_DW)
```

This keeps `lpObj->DBClass` authoritative while `lpObj->Class` remains the
safe legacy array slot. It must be compiled together with the server catalog
overlay; it does not expand `MAX_CLASS` or copy Grow Lancer hooks.
