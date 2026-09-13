#include "stdafx.h"
#include "PegasusDeathsideRuntime.h"
#include "../DSplaysound.h"
#include "PegasusSoundRuntime.h"
#include "PegasusRuntimeTrace.h"
#include "../ZzzAI.h"
#include "../ZzzBMD.h"
#include "../ZzzCharacter.h"
#include "../ZzzEffect.h"
#include "../ZzzObject.h"
#include "../ZzzLodTerrain.h"
#include "../ZzzOpenglUtil.h"
#include "../GlobalBitmap.h"
#include "../_struct.h"

#include <cmath>
#include <string>

namespace rise { namespace pegasus {
namespace {
// Pegasus resource 0x7E22 resolves to Effect\WATERFALL4.jpg at 0x86F4E1.
// RISE assigns that semantic resource a different enum value, so only the
// native BITMAP_WATERFALL_4 symbol is portable across the two layouts.
const char* const kDeathsideFiles[] = {
    "Reaper.bmd", "ReaperHead.bmd", "ScytheEffAni01.bmd",
    "ScytheEffAni02.bmd", "ScytheEffAni03.bmd"
};
int g_deathsideGroundStar = BITMAP_UNKNOWN;
int g_deathsideDamage = BITMAP_UNKNOWN;
int g_deathsideGroundSmoke = BITMAP_UNKNOWN;
int g_deathsideSmoke = BITMAP_UNKNOWN;
int g_deathsideBlur = BITMAP_UNKNOWN;

bool EnsureDeathsideTextures()
{
    const char* skill = "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\";
    const char* effect = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    if (g_deathsideGroundStar == BITMAP_UNKNOWN)
        g_deathsideGroundStar = Bitmaps.LoadImageFile(
            (std::string(skill) + "ground_star.jpg").c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_deathsideDamage == BITMAP_UNKNOWN)
        g_deathsideDamage = Bitmaps.LoadImageFile(
            (std::string(effect) + "damage01mono.jpg").c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_deathsideGroundSmoke == BITMAP_UNKNOWN)
        g_deathsideGroundSmoke = Bitmaps.LoadImageFile(
            (std::string(effect) + "ground_smoke_mono.jpg").c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_deathsideSmoke == BITMAP_UNKNOWN)
        g_deathsideSmoke = Bitmaps.LoadImageFile(
            (std::string(effect) + "smoke01.jpg").c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_deathsideBlur == BITMAP_UNKNOWN)
        g_deathsideBlur = Bitmaps.LoadImageFile(
            (std::string(effect) + "blur02_mono.jpg").c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE);
    return g_deathsideGroundStar != BITMAP_UNKNOWN &&
        g_deathsideDamage != BITMAP_UNKNOWN &&
        g_deathsideGroundSmoke != BITMAP_UNKNOWN &&
        g_deathsideSmoke != BITMAP_UNKNOWN &&
        g_deathsideBlur != BITMAP_UNKNOWN;
}

bool LoadDeathsideModel(int type, const char* filename)
{
    if (!Models)
        return false;
    BMD& model = Models[type];
    if (model.NumActions > 0 &&
        (model.NumMeshs == 0 || (model.Meshs && model.IndexTexture)))
        return true;
    char directory[] = "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\";
    model.m_iBMDSeqID = type;
    if (!model.Open2(directory, const_cast<char*>(filename)))
        return false;
    // Open2 does not initialize Action_t::PlaySpeed. Port the skill's
    // explicit setup0x660019..0x6600FF, not the generic fallback .02.
    if (type == kDeathsideReaperModel && model.Actions)
    {
        for (int action = 0; action < model.NumActions && action < 9; ++action)
        {
            Action_t& state = model.Actions[action];
            state.Loop = action != 0 && action != 2;
            if (action == 0 || action == 1 || action == 2)
                state.PlaySpeed = 0.1f;
            else if (action == 6)
                state.PlaySpeed = 0.08f;
            else if (action == 7)
                state.PlaySpeed = 0.13f;
            else if (action == 3 || action == 4 || action == 8)
                state.PlaySpeed = DeathsideAttackPlaySpeed(state.NumAnimationKeys);
        }
    }
    else if (type >= kDeathsideScythe01Model && type <= kDeathsideScythe03Model &&
        model.Actions && model.NumActions > 0)
        model.Actions[0].PlaySpeed = 0.2f;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        const std::string texturePath = std::string(directory) +
            model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(texturePath,
            GL_LINEAR, GL_REPEAT);
        if (model.IndexTexture[mesh] == BITMAP_UNKNOWN)
        {
            model.Release();
            return false;
        }
    }
#ifdef jdk_shader_local330
    if (model.NumMeshs > 0)
        OGL330MODEL::ConvertOldMeshToVaoMesh(type);
#endif
    return model.NumActions > 0;
}

OBJECT* FindDeathsideRoot(OBJECT& owner)
{
    for (int index = 0; index < MAX_EFFECTS; ++index)
    {
        OBJECT& effect = Effects[index];
        if (effect.Live && effect.Type == kDeathsideReaperModel &&
            effect.SubType == 0 && effect.Owner == &owner)
            return &effect;
    }
    return nullptr;
}

void AdvanceDeathsideAnimation(OBJECT& effect)
{
    BMD& model = Models[effect.Type];
    if (effect.CurrentAction >= model.NumActions)
        return;
    model.CurrentAction = effect.CurrentAction;
    const float speed = maxf(model.Actions[effect.CurrentAction].PlaySpeed,
        kDeathsideMinimumPlaySpeed);
    const bool playing = model.PlayAnimation(&effect.AnimationFrame,
        &effect.PriorAnimationFrame, &effect.PriorAction, speed,
        effect.Position, effect.Angle);
    TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,
        effect.AnimationFrame);

    // Pegasus 0x65FA60..0x65FB27 advances every Reaper/scythe BMD here.
    // A finished non-dismissal action other than 0/2 is reset to action 0;
    // this includes the initial action 7 and attack actions 3/4/8.
    if (!playing && effect.SubType != 1 &&
        effect.CurrentAction != kDeathsidePostAttackAction &&
        effect.CurrentAction != 2)
    {
        effect.PriorAction = effect.CurrentAction;
        effect.CurrentAction = kDeathsidePostAttackAction;
        effect.AnimationFrame = 0.0f;
        effect.PriorAnimationFrame = 0.0f;
    }
}

bool TransformDeathsideBone(BMD& model, int bone, vec3_t position)
{
    if (bone < 0 || bone >= model.NumBones)
        return false;
    vec3_t zero;
    Vector(0.0f, 0.0f, 0.0f, zero);
    model.TransformPosition(BoneTransform[bone], zero, position, true);
    return true;
}

void CreateDeathsideTrailSegment(BMD& model, OBJECT& effect, int firstBone,
    int secondBone, int subtype, int lifetime, vec3_t light,
    const vec3_t mirrorCenter = nullptr)
{
    vec3_t start, end;
    if (!TransformDeathsideBone(model, firstBone, start) ||
        !TransformDeathsideBone(model, secondBone, end))
        return;
    if (mirrorCenter)
    {
        start[0] = mirrorCenter[0] * 2.0f - start[0];
        start[1] = mirrorCenter[1] * 2.0f - start[1];
        end[0] = mirrorCenter[0] * 2.0f - end[0];
        end[1] = mirrorCenter[1] * 2.0f - end[1];
    }
    // Pegasus 0x794152 is the eight-argument OBJECT_BLUR allocator, not the
    // joint allocator. The final argument is the lifetime limit (100 or -1),
    // while the bitmap uses a direct texture ID. Its renderer selects
    // ZERO / ONE_MINUS_SRC_COLOR (0x794728 -> 0x87F992), not alpha test.
    CreateObjectBlur(&effect, start, end, light, g_deathsideBlur, false,
        subtype, lifetime);
}

void UpdateDeathsideScytheTrail(OBJECT& effect)
{
    if (!effect.Owner || effect.Owner->Type != kDeathsideReaperModel)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    BMD& child = Models[effect.Type];
    BMD& reaper = Models[kDeathsideReaperModel];
    const int reaperAction = effect.Type == kDeathsideScythe01Model ? 3 :
        (effect.Type == kDeathsideScythe02Model ? 4 : 8);
    if (child.NumActions <= 0 || reaper.NumActions <= reaperAction)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    const int sampleCount = child.Actions[0].NumAnimationKeys;
    const float playSpeed = reaper.Actions[reaperAction].PlaySpeed;
    if (sampleCount <= 0 || playSpeed <= 0.0f)
    {
        effect.LifeTime = 0.0f;
        return;
    }

    // Pegasus 0x65F089..0x65F4BF: the invisible scythe carrier samples one
    // complete Reaper-action delta across its own action-key count. Every
    // sub-frame emits two authored bone trails plus three mirrored layers.
    float frame = (kDeathsideScytheLifetime - effect.LifeTime) *
        playSpeed - playSpeed;
    if (frame >= static_cast<float>(sampleCount) - playSpeed)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    const float frameStep = playSpeed / static_cast<float>(sampleCount);
    child.BodyScale = effect.Scale;
    child.CurrentAction = 0;
    VectorCopy(effect.Position, child.BodyOrigin);
    vec3_t light;
    Vector(kDeathsideTrailLight, kDeathsideTrailLight,
        kDeathsideTrailLight, light);

    for (int sample = 0; sample < sampleCount; ++sample)
    {
        child.Animation(BoneTransform, frame, frame - 1.0f, 0,
            effect.Angle, effect.HeadAngle, false, false);
        // Pegasus65F140/65F141 pass both flags0. TransformDeathsideBone
        // applies scale/origin once; translated Animation would add it twice.
        if (frame >= kDeathsideTrailStartFrame)
        {
            if (effect.Type == kDeathsideScythe01Model)
            {
                CreateDeathsideTrailSegment(child, effect, 5, 6, 1,
                    kDeathsideTrailLifetime,
                    light);
                CreateDeathsideTrailSegment(child, effect, 9, 11, 3,
                    kDeathsideTrailLifetime,
                    light);
            }
            else if (effect.Type == kDeathsideScythe02Model)
            {
                CreateDeathsideTrailSegment(child, effect, 4, 8, 1,
                    kDeathsideTrailLifetime,
                    light);
                CreateDeathsideTrailSegment(child, effect, 8, 9, 3,
                    kDeathsideTrailLifetime,
                    light);
            }
            else
            {
                CreateDeathsideTrailSegment(child, effect, 14, 19, 1,
                    kDeathsideTrailLifetime, light);
                CreateDeathsideTrailSegment(child, effect, 15, 20, 3,
                    kDeathsideTrailLifetime, light);
            }

            vec3_t center;
            if (TransformDeathsideBone(child, 0, center))
            {
                CreateDeathsideTrailSegment(child, effect, 4, 9, 4,
                    kDeathsideTrailLifetime,
                    light, center);
                CreateDeathsideTrailSegment(child, effect, 4, 9, 5,
                    kDeathsideTrailLifetime,
                    light, center);
                CreateDeathsideTrailSegment(child, effect, 5, 10, 6,
                    kDeathsideMirroredTailLifetime,
                    light, center);
            }
            TraceRuntimeModelStateOnce("DEATHSIDE_TRAIL", effect.Type,
                MAX_MODELS, true);
        }
        frame += frameStep;
    }
    effect.AnimationFrame = frame;
    TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,
        effect.AnimationFrame);
}
}

bool EnsureDeathsideModels()
{
    if (!EnsureDeathsideTextures())
        return false;
    for (int offset = 0; offset < 5; ++offset)
        if (!LoadDeathsideModel(kDeathsideFirstModel + offset,
            kDeathsideFiles[offset]))
            return false;
    return true;
}

bool IsDeathsideBlurBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN && bitmapId == g_deathsideBlur;
}

bool IsDeathsideParticleBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN &&
        (bitmapId == g_deathsideGroundStar || bitmapId == g_deathsideDamage ||
         bitmapId == g_deathsideGroundSmoke || bitmapId == g_deathsideSmoke);
}

bool IsDeathsideGroundSmokeBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN && bitmapId == g_deathsideGroundSmoke;
}

bool IsDeathsideSmokeBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN && bitmapId == g_deathsideSmoke;
}

void ApplyDeathsideCasterAction(CHARACTER& caster)
{
    static_assert(PLAYER_SKILL_SUMMON == kDeathsideCasterSummonActions[0],
        "Deathside base summon action drift");
    static_assert(PLAYER_SKILL_SUMMON_UNI == kDeathsideCasterSummonActions[1],
        "Deathside Uniria summon action drift");
    static_assert(PLAYER_SKILL_SUMMON_DINO == kDeathsideCasterSummonActions[2],
        "Deathside Dinorant summon action drift");
    static_assert(PLAYER_SKILL_SUMMON_FENRIR == kDeathsideCasterSummonActions[3],
        "Deathside Fenrir summon action drift");
    static_assert(PLAYER_SKILL_LIGHTNING_SHOCK == kDeathsideCasterRepeatAction,
        "Deathside repeat-cast action drift");
    OBJECT& owner = caster.Object;
    BMD& player = Models[owner.Type];

    // Pegasus 0x6604A3..0x6604CE: once the Reaper already exists, an
    // accepted Deathside command uses player action 0xB9 (185).  The RISE
    // player BMD owns the exact same 11-key payload at action 185.
    if (FindDeathsideRoot(owner))
    {
        if (player.NumActions > PLAYER_SKILL_LIGHTNING_SHOCK)
            SetAction(&owner, PLAYER_SKILL_LIGHTNING_SHOCK);
        return;
    }

    // Pegasus 0x6604F2..0x660555 chooses 0xAC..0xAF for the first summon,
    // matching RISE's native Summoner mount-aware action family.  All four
    // payloads were compared byte-for-byte against the pinned Pegasus BMD.
    int action = PLAYER_SKILL_SUMMON;
    // 6604F2 branches directly to base action172 while in a safe zone;
    // helper equipment may still be present even though it is not mounted.
    switch (caster.SafeZone ? -1 : caster.Helper.Type)
    {
    case MODEL_HELPER + 2:
        action = PLAYER_SKILL_SUMMON_UNI;
        break;
    case MODEL_HELPER + 3:
        action = PLAYER_SKILL_SUMMON_DINO;
        break;
    case MODEL_HELPER + 37:
        action = PLAYER_SKILL_SUMMON_FENRIR;
        break;
    }
    if (player.NumActions > action)
        SetAction(&owner, action);
}

void CastDeathside(CHARACTER& caster, int skill)
{
    if (!EnsureDeathsideModels())
        return;
    OBJECT& owner = caster.Object;
    OBJECT* root = FindDeathsideRoot(owner);
    if (root)
    {
        // Pegasus 0x66046E..0x66049E selects exactly actions 3, 4 or 8
        // for an already-live Reaper before starting its attack sequence.
        const int variant = rand() % 3;
        // Pegasus 0x66046E..0x660487 plays buffer 0x4B + variant before
        // dispatching the matching {3,4,8} Reaper attack action.
        PlayBuffer(kDeathsideAttack1Sound + variant, nullptr, false);
        SetAction(root, kDeathsideAttackActions[variant], false);
        root->AnimationFrame = 0.0f;
        root->PriorAnimationFrame = 0.0f;
        root->LifeTime = 999.0f;
        // Both root construction and every accepted repeat attack extend the
        // exact Pegasus 10,000 ms active deadline (0xB6B120).
        root->m_dwTime = timeGetTime() + kDeathsideActiveMilliseconds;
        CreateEffect(kDeathsideScythe01Model + variant, root->Position,
            root->Angle, root->Light, 0, root, -1,
            static_cast<WORD>(skill), 0, 0, root->Scale);
        return;
    }

    vec3_t position, angle, light;
    VectorCopy(owner.Position, position);
    position[2] += kDeathsideRootHeight; // 0x6605F8 / 0xB4F45C.
    VectorCopy(owner.Angle, angle);
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kDeathsideReaperModel, position, angle, light, 0, &owner,
        -1, static_cast<WORD>(skill), 0, 0, 1.0f);
    // Pegasus 0x660568..0x660573 plays buffer 0x4E after first creation.
    PlayBuffer(kDeathsideAppearSound, nullptr, false);
}

void InitializeDeathside(OBJECT& effect)
{
    if (!effect.Owner)
    {
        effect.Live = false;
        return;
    }
    if (effect.Type != kDeathsideReaperModel)
    {
        // Pegasus 0x65E81D..0x65E920: animation-only scythe children copy
        // root transform/light and live for 60 ticks at scale 0.9.
        effect.LifeTime = kDeathsideScytheLifetime;
        effect.Scale = 0.9f;
        effect.CurrentAction = 0;
        effect.PriorAction = 0;
        effect.AnimationFrame = 0.0f;
        effect.PriorAnimationFrame = 0.0f;
        effect.Alpha = 0.0f;
        VectorCopy(effect.Owner->Position, effect.Position);
        VectorCopy(effect.Owner->Angle, effect.Angle);
        VectorCopy(effect.Owner->Light, effect.Light);
        return;
    }
    // Pegasus root constructor 0x65E925..0x65EA58.
    effect.LifeTime = 999.0f;
    effect.Scale = 1.0f;
    effect.CurrentAction = kDeathsideIdleAction;
    effect.PriorAction = kDeathsideIdleAction;
    effect.AnimationFrame = 0.0f;
    effect.PriorAnimationFrame = 0.0f;
    effect.Alpha = kDeathsideInitialAlpha;
    effect.AlphaTarget = 0.0f;
    effect.ExtState = 0;
    effect.Kind = 0;
    effect.m_dwTime = timeGetTime() + kDeathsideActiveMilliseconds;
    VectorCopy(effect.Position, effect.StartPosition);
    vec3_t particlePosition, particleAngle, groundLight, damageLight;
    VectorCopy(effect.Position, particlePosition);
    particlePosition[2] += kDeathsideParticleHeight;
    VectorCopy(effect.Angle, particleAngle);
    Vector(0.85f, 0.85f, 0.85f, groundLight);
    Vector(1.0f, 1.0f, 1.0f, damageLight);
    CreateParticle(g_deathsideGroundStar, particlePosition, particleAngle,
        groundLight, 5, 0.8f, &effect);
    CreateParticle(g_deathsideDamage, particlePosition, particleAngle,
        damageLight, 3, 0.396f, &effect);
}

void UpdateDeathside(OBJECT& effect, float animationFactor)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.Live = false;
        return;
    }
    if (effect.Type != kDeathsideReaperModel)
    {
        VectorCopy(effect.Owner->Position, effect.Position);
        VectorCopy(effect.Owner->Angle, effect.Angle);
        VectorCopy(effect.Owner->Light, effect.Light);
        UpdateDeathsideScytheTrail(effect);
        return;
    }
    if (effect.Kind == 1)
    {
        AdvanceDeathsideAnimation(effect);
        return;
    }

    // Pegasus 0x65F9A9..0x65FA1B: ten seconds after creation or the most
    // recent attack, switch to action 6, give the root 20 remaining ticks,
    // set velocity 0.5, and play buffer 0x4F exactly once.
    if (static_cast<LONG>(timeGetTime() - effect.m_dwTime) >= 0)
    {
        effect.Kind = 1;
        effect.CurrentAction = kDeathsideDisappearAction;
        effect.PriorAction = 0;
        effect.AnimationFrame = 0.0f;
        effect.PriorAnimationFrame = 0.0f;
        effect.LifeTime = kDeathsideDisappearLifetime;
        effect.Alpha = kDeathsideDisappearAlpha;
        PlayBuffer(kDeathsideDisappearSound, nullptr, false);
        AdvanceDeathsideAnimation(effect);
        return;
    }
    // Complete Pegasus root-movement state, 0x65F4E8..0x65F6AA. ExtState is
    // a follow-state byte; Kind is the independent dismissal-state byte.
    const float deltaX = effect.Owner->Position[0] - effect.Position[0];
    const float deltaY = effect.Owner->Position[1] - effect.Position[1];
    const float ownerDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
    if (effect.ExtState == 0)
    {
        if (ownerDistance > kDeathsideFollowStartDistance)
            effect.ExtState = 2;
    }
    else if (ownerDistance < kDeathsideFollowStopDistance)
    {
        effect.ExtState = 0;
    }
    if (effect.ExtState == 2)
    {
        vec3_t destination;
        VectorCopy(effect.Owner->Position, destination);
        destination[2] = RequestTerrainHeight(destination[0], destination[1]) +
            kDeathsideRootHeight;
        const float follow = kDeathsideFollowFactor * animationFactor;
        for (int axis = 0; axis < 3; ++axis)
            effect.Position[axis] +=
                (destination[axis] - effect.Position[axis]) * follow;
        const float targetAngle = CreateAngle2D(effect.Position,
            effect.Owner->Position);
        effect.Angle[2] = TurnAngle2(effect.Angle[2], targetAngle,
            kDeathsideTurnStep);
    }
    effect.LifeTime = 999.0f;

    // Pegasus 0x65F6EA..0x65F7D4 resolves resource 0x7E22 to the native
    // WATERFALL4.jpg registration. Emit two subtype-0x74 particles with the
    // recovered +/-30 origins, spread, height and scale.
    // Pegasus sampled these branches once per its fixed simulation update.
    // RISE 5.2 can run the same updater at an unlocked render rate, so use
    // its native FPS-normalized random gate to preserve the original
    // particles-per-second density instead of multiplying it by client FPS.
    if (rand_fps_check(kDeathsideWaterfallCadence))
    {
        vec3_t position, light;
        Vector(1.0f, 1.0f, 1.0f, light);
        for (int side = 0; side < 2; ++side)
        {
            VectorCopy(effect.Position, position);
            position[0] += (side == 0 ? -30.0f : 30.0f) +
                static_cast<float>(rand() % 80 - 40);
            position[1] += static_cast<float>(rand() % 80 - 40);
            position[2] += kDeathsideParticleHeight +
                static_cast<float>(rand() % 16);
            CreateParticle(BITMAP_WATERFALL_4, position, effect.Angle, light,
                kDeathsideSmokeSubtype, 3.0f, &effect);
        }
    }

    // Pegasus 0x65F7DA..0x65F8BE: two white ground-smoke particles on
    // one out of every 13 updates. Their X origins are -30/+30, each with
    // an additional [-40,39] spread; Y uses the same spread and Z [0,17].
    if (rand_fps_check(kDeathsideGroundSmokeCadence))
    {
        vec3_t position, light;
        Vector(1.0f, 1.0f, 1.0f, light);
        for (int side = 0; side < 2; ++side)
        {
            VectorCopy(effect.Position, position);
            position[0] += (side == 0 ? -30.0f : 30.0f) +
                static_cast<float>(rand() % 80 - 40);
            position[1] += static_cast<float>(rand() % 80 - 40);
            position[2] += kDeathsideParticleHeight +
                static_cast<float>(rand() % 18);
            CreateParticle(g_deathsideGroundSmoke, position, effect.Angle,
                light, kDeathsideGroundSmokeSubtype, 0.8f, &effect);
        }
    }

    // Pegasus 0x65F8C7..0x65F998: the upper smoke is intentionally warm
    // gray, subtype 0x74, and uses the recovered narrow asymmetric spread.
    if (rand_fps_check(kDeathsideSmokeCadence))
    {
        vec3_t position, light;
        VectorCopy(effect.Position, position);
        position[0] += static_cast<float>(rand() % 3);
        position[1] += -20.0f + static_cast<float>(rand() % 14);
        position[2] += 223.0f + static_cast<float>(rand() % 18);
        Vector(0.41f, 0.41f, 0.4f, light);
        CreateParticle(g_deathsideSmoke, position, effect.Angle, light,
            kDeathsideSmokeSubtype, 1.0f, &effect);
    }

    AdvanceDeathsideAnimation(effect);
}

bool RenderDeathside(OBJECT& effect)
{
    if (!EnsureDeathsideModels())
        return false;
    if (effect.Type != kDeathsideReaperModel)
    {
        // Pegasus scythe BMDs are intentionally meshless animation carriers.
        // Their visible output is emitted from UpdateDeathsideScytheTrail.
        return true;
    }
    if (!Calc_RenderObject(&effect, false, 0, 0))
        return false;
    // Pegasus 0x65FC18..0x65FCF8 renders Reaper meshes 0,1,2,4 through
    // the texture pass. Its idle alpha pulses from 0.6 to 0.7 every 3200 ms.
    const float phase = static_cast<float>(fmod(WorldTime, 3200.0) /
        3200.0 * 2.0 * 3.14159265358979323846);
    // Pegasus 0x65FC39..0x65FCA3 uses velocity as renderer alpha during
    // subtype-1 dismissal; otherwise it uses the 0.6..0.7 idle pulse.
    const float alpha = effect.Kind == 1 ? effect.Alpha :
        0.65f + std::sin(phase) * 0.05f;
    BMD& model = Models[kDeathsideReaperModel];
    static const int meshes[] = {0, 1, 2, 4};
    for (int mesh : meshes)
        if (mesh < model.NumMeshs)
            model.RenderMesh(mesh, RENDER_TEXTURE, alpha, -1,
                effect.BlendMeshLight, effect.BlendMeshTexCoordU,
                effect.BlendMeshTexCoordV, -1);
    return true;
}
}}
