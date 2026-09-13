#include "stdafx.h"
#include "WindSoulRuntime.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzAI.h"
#include "DSPlaySound.h"
#include "CustomPet.h"
#include "ZzzInfomation.h"

namespace rise { namespace windsoul {

void ApplyBase737SkillCatalog()
{
    SKILL_ATTRIBUTE& value = SkillAttribute[Base737Contract::SkillId];
    ZeroMemory(&value, sizeof(value));
    strcpy_s(value.Name, "Wind Soul");
    value.Level = 400;
    value.Damage = 130;
    value.Mana = 35;
    value.AbilityGuage = 17;
    value.Distance = 6;
    value.Strength = 717;
    value.MasteryType = 255;
    value.RequireClass[4] = 3;
    value.IsDamage = 1;
}

void ApplyBase737CasterAction(CHARACTER& caster)
{
    OBJECT& owner = caster.Object;
    if (owner.Type != MODEL_PLAYER)
    {
        SetPlayerAttack(&caster);
        return;
    }

    const int customPetType = gCustomPet.GetInfoPetType(
        caster.Helper.Type - MODEL_ITEM);
    if (caster.Helper.Type == MODEL_HELPER + 37 || customPetType == 5)
    {
        SetAction(&owner, PLAYER_FENRIR_ATTACK_DARKLORD_STRIKE);
    }
    else if (((caster.Helper.Type >= MODEL_HELPER + 2) &&
        (caster.Helper.Type <= MODEL_HELPER + 4)) || customPetType == 6)
    {
        SetAction(&owner, PLAYER_ATTACK_RIDE_STRIKE);
    }
    else
    {
        SetAction(&owner, PLAYER_ATTACK_STRIKE);
    }
}

bool CreateBase737Root(CHARACTER& caster, int targetSlot)
{
    if (CharactersClient == NULL || targetSlot < 0 ||
        targetSlot >= MAX_CHARACTERS_CLIENT)
    {
        return false;
    }

    OBJECT& owner = caster.Object;
    OBJECT& target = CharactersClient[targetSlot].Object;
    if (!owner.Live || !target.Live)
    {
        return false;
    }

    vec3_t localOffset, rotatedOffset, launchOrigin, initialTarget;
    float matrix[3][4];
    Vector(0.f, Base737Contract::LocalLaunchY, 0.f, localOffset);
    AngleMatrix(owner.Angle, matrix);
    VectorRotate(localOffset, matrix, rotatedOffset);
    VectorAdd(owner.Position, rotatedOffset, launchOrigin);

    VectorCopy(owner.Position, initialTarget);
    initialTarget[2] += Base737Contract::InitialTargetZOffset;

    for (int index = 0; index < Base737Contract::RootCount; ++index)
    {
        vec3_t position;
        VectorCopy(launchOrigin, position);
        position[0] = launchOrigin[0] + Base737Contract::RandomXBase +
            static_cast<float>(rand() % Base737Contract::RandomXModulo);
        position[2] = launchOrigin[2] + Base737Contract::RandomZBase +
            static_cast<float>(rand() % Base737Contract::RandomZModulo);
        CreateJoint(BITMAP_2LINE_GHOST, position, initialTarget, owner.Angle,
            Base737Contract::RootSubtype, &owner, Base737Contract::RootScale,
            owner.PKKey, 0, owner.m_bySkillSerialNum,
            -1, NULL, static_cast<short>(targetSlot));
    }

    PlayBuffer(SOUND_SKILL_CAOTIC);
    return true;
}

}}
