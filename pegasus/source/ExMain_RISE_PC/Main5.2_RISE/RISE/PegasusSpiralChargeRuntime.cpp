#include "stdafx.h"
#include "PegasusSpiralChargeRuntime.h"
#include "PegasusSpiralChargeContract.h"
#include "PegasusRuntimeTrace.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzOpenglUtil.h"
#include "ZzzCharacter.h"
#include "GlobalBitmap.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

namespace rise { namespace pegasus {
namespace {
const char* kEffectDirectory = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";

void CreateLayer(int type, OBJECT& root, vec3_t light, int subtype, float scale)
{
    CreateEffect(type, root.Position, root.Angle, light, subtype,
        root.Owner, -1, 0, root.Kind, 0, scale);
}
}

bool EnsureSpiralChargeModel(int modelId)
{
    if (!EnsureSpiralChargeBitmaps() || !Models ||
        modelId < kSpiralChargeFirstModel || modelId > kSpiralChargeLastModel)
        return false;
    BMD& model = Models[modelId];
    // 01spa_skill.bmd is an animation-only carrier (0 meshes, 9 bones,
    // 1 action). Requiring a mesh made every update reopen the model.
    if (model.NumBones > 0 && model.NumActions > 0)
        return true;
    char directory[] = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    char filename[] = "01spa_skill.bmd";
    model.m_iBMDSeqID = modelId;
    if (!model.Open2(directory, filename))
        return false;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        const std::string texturePath = std::string(directory) + model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(texturePath, GL_LINEAR, GL_REPEAT);
        if (model.IndexTexture[mesh] == BITMAP_UNKNOWN)
        {
            model.Release();
            return false;
        }
    }
#ifdef jdk_shader_local330
    OGL330MODEL::ConvertOldMeshToVaoMesh(modelId);
#endif
    return model.NumBones > 0 && model.NumActions > 0;
}

bool EnsureSpiralChargeBitmaps()
{
    struct BitmapRow { int id; const char* name; };
    static const BitmapRow rows[] = {
        {kSpiralChargeFlareBitmap, "flareRed.jpg"},
        {kSpiralChargeShockwaveBitmap, "Shockwave2.jpg"},
        {kSpiralChargeSteamBitmap, "Steam_01.jpg"},
        {kSpiralChargeSmokeBitmap, "smokelines03.jpg"},
    };
    for (const BitmapRow& row : rows)
    {
        if (Bitmaps.FindTexture(row.id))
            continue;
        if (!Bitmaps.LoadImageFile(row.id, std::string(kEffectDirectory) + row.name,
            GL_LINEAR, GL_CLAMP_TO_EDGE))
            return false;
    }
    return true;
}

void CreateSpiralChargeRoot(OBJECT& owner, int /*skill*/)
{
    if (!EnsureSpiralChargeModel(kSpiralChargeRootModel) ||
        !EnsureSpiralChargeBitmaps())
        return;
    vec3_t position;
    VectorCopy(owner.Position, position);
    if (owner.BoneTransform)
        Models[owner.Type].TransformByObjectBone(position, &owner,
            kSpiralChargeOwnerBone);
    CreateEffect(kSpiralChargeRootModel, position, owner.Angle, owner.Light,
        0, &owner, -1, 0, 0);
}

void CreateSpiralChargeAnchor(OBJECT& owner)
{
    // Original 0x676D12 rejects a non-live owner before removal/creation.
    if (!owner.Live)
        return;
    // Pegasus 0x676CFA removes the previous 0x27AD instance, resolves owner
    // bone 17, raises the point by 10, then creates the persistent anchor.
    DeleteEffect(kSpiralChargeAnchorModel, &owner);
    vec3_t position;
    VectorCopy(owner.Position, position);
    if (owner.BoneTransform)
        Models[owner.Type].TransformByObjectBone(position, &owner,
            kSpiralChargeOwnerBone);
    position[2] += kSpiralChargeAnchorHeight;
    CreateEffect(kSpiralChargeAnchorModel, position, owner.Angle, owner.Light,
        0, &owner, -1, 0, 0, 0, kSpiralChargeAnchorCreateScale);
}

void InitializeSpiralCharge(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kSpiralChargeRootModel:
        effect.LifeTime = kSpiralChargeRootLifetime;
        effect.Scale = 0.5f;
        effect.Velocity = 0.3f;
        effect.Alpha = 0.0f;
        effect.AlphaTarget = 0.0f;
        effect.Position[2] -= 30.0f;
        break;
    case kSpiralChargeFlareModel:
        effect.LifeTime = kSpiralChargeFlareLifetime;
        effect.Scale = 5.0f;
        break;
    case kSpiralChargeShockwaveModel:
        effect.LifeTime = kSpiralChargeShockwaveLifetime;
        effect.Scale = 0.0f;
        effect.BlendMeshLight = 1.0f;
        effect.Gravity = 0.65f;
        break;
    case kSpiralChargeSteamModel:
    case kSpiralChargeSmokeModel:
        effect.LifeTime = kSpiralChargeEmitterLifetime;
        break;
    case kSpiralChargeAnchorModel:
        effect.LifeTime = 9999999.0f;
        effect.Velocity = 0.0f;
        effect.Scale = kSpiralChargeAnchorRuntimeScale;
        Vector(1.0f, 1.0f, 1.0f, effect.Light);
        break;
    }
}

void CreateSpiralChargeLayers(OBJECT& root)
{
    if (!root.Owner || !root.Owner->Live)
        return;
    vec3_t flarePosition;
    VectorCopy(root.Position, flarePosition);
    flarePosition[2] += 40.0f;
    vec3_t light;
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kSpiralChargeFlareModel, flarePosition, root.Angle, light, 1,
        root.Owner, -1, 0, root.Kind, 0, 5.0f);
    Vector(1.0f, 0.15f, 0.15f, light);
    CreateLayer(kSpiralChargeShockwaveModel, root, light, 2, 0.65f);
    CreateLayer(kSpiralChargeShockwaveModel, root, light, 2, 0.65f);
    Vector(0.55f, 0.3f, 0.1f, light);
    CreateLayer(kSpiralChargeSteamModel, root, light, 3, 1.2f);
    Vector(1.0f, 0.1f, 0.05f, light);
    CreateLayer(kSpiralChargeSmokeModel, root, light, 4, 3.0f);

    // Pegasus 0x677345..0x677385 creates one flareRed joint for each of the
    // nine animation-only root bones. iChaIndex carries the bone index.
    vec3_t zero;
    Vector(0.0f, 0.0f, 0.0f, zero);
    for (int bone = 0; bone < kSpiralChargeRootJointCount; ++bone)
    {
        CreateJoint(kSpiralChargeFlareBitmap, zero, zero, root.Angle, 0,
            &root, kSpiralChargeRootJointScale, -1, 0, 0, bone);
    }
}

void UpdateSpiralCharge(OBJECT& effect, float animationFactor)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    if (effect.Type == kSpiralChargeAnchorModel)
    {
        // Pegasus tests buff 214 here. RISE 5.2 transports the same semantic
        // state through its collision-safe marker 169.
        if (!g_isCharacterBuff(effect.Owner,
            static_cast<eBuffState>(kSpiralChargeNativeEffect)))
            effect.LifeTime = 0.0f;
        else
            VectorCopy(effect.Owner->Position, effect.Position);
        return;
    }
    if (effect.Type == kSpiralChargeShockwaveModel)
    {
        effect.Scale += effect.Gravity * 0.1f * animationFactor;
        if (effect.LifeTime < 10.0f)
            effect.BlendMeshLight = (effect.LifeTime > 0.0f) ? effect.LifeTime / 10.0f : 0.0f;
        return;
    }
    if (effect.Type == kSpiralChargeRootModel)
    {
        if (!EnsureSpiralChargeModel(kSpiralChargeRootModel))
        {
            effect.LifeTime = 0.0f;
            return;
        }
        BMD& model = Models[kSpiralChargeRootModel];
        model.CurrentAction = 0;
        model.PlayAnimation(&effect.AnimationFrame,
            &effect.PriorAnimationFrame, &effect.PriorAction,
            kSpiralChargeRootAnimationSpeed, effect.Position, effect.Angle);
        TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,
            effect.AnimationFrame);
        return;
    }
    if (effect.Type == kSpiralChargeSteamModel ||
        effect.Type == kSpiralChargeSmokeModel)
    {
        // 0x67704B calls FLOOR/0xAF4800, not SIN. SETA compares
        // floor(life+step) > floor(life); callers emit only when true.
        if (SpiralChargeEmitterTick(effect.LifeTime, animationFactor))
        {
            const bool steam = effect.Type == kSpiralChargeSteamModel;
            CreateParticle(steam ? kSpiralChargeSteamBitmap :
                kSpiralChargeSmokeBitmap, effect.Position, effect.Angle,
                effect.Light, steam ? kSpiralChargeSteamParticleSubtype :
                kSpiralChargeSmokeParticleSubtype, effect.Scale, &effect);
        }
    }
}

bool RenderSpiralCharge(OBJECT& effect)
{
    if (!EnsureSpiralChargeBitmaps())
        return false;
    switch (effect.Type)
    {
    case kSpiralChargeRootModel:
        // The root carries the recovered lifetime and owns the layer cascade.
        return true;
    case kSpiralChargeFlareModel:
    {
        const float elapsed = 20.0f - effect.LifeTime;
        float alpha = (elapsed <= 10.0f) ? elapsed * 0.1f :
            (20.0f - elapsed) * 0.1f;
        if (alpha < 0.0f) alpha = 0.0f;
        vec3_t light;
        Vector(alpha, alpha, alpha, light);
        CreateSprite(kSpiralChargeFlareBitmap, effect.Position,
            effect.Scale, light, effect.Owner);
        return true;
    }
    case kSpiralChargeShockwaveModel:
    {
        vec3_t light;
        Vector(effect.Light[0] * effect.BlendMeshLight,
            effect.Light[1] * effect.BlendMeshLight,
            effect.Light[2] * effect.BlendMeshLight, light);
        CreateSprite(kSpiralChargeShockwaveBitmap, effect.Position,
            effect.Scale, light, effect.Owner);
        return true;
    }
    case kSpiralChargeSteamModel:
    case kSpiralChargeSmokeModel:
        // Invisible particle emitters; see UpdateSpiralCharge.
        return true;
    case kSpiralChargeAnchorModel:
    {
        if (!effect.Owner || !effect.Owner->Live || !effect.Owner->BoneTransform)
            return true;
        const float pulseA = std::sin(WorldTime * 0.01f) + 1.0f;
        const float pulseB = std::sin(WorldTime * 0.005f) + 1.0f;
        const float low = pulseA * 0.25f + 0.2f;
        const float medium = pulseA * 0.3f + 0.3f;
        const float high = pulseB * 0.25f + 0.5f;
        const float centerScales[3] = {low, high, medium};
        vec3_t light;
        for (float value : centerScales)
        {
            Vector(value, value, value, light);
            CreateSprite(BITMAP_LIGHT_RED, effect.Owner->Position, 1.5f,
                light, effect.Owner);
        }
        BMD& ownerModel = Models[effect.Owner->Type];
        vec3_t local, position;
        Vector(0.0f, 0.0f, 0.0f, local);
        for (int index = 0; index < kSpiralChargeAnchorBoneCount; ++index)
        {
            const int bone = kSpiralChargeAnchorBones[index];
            if (bone < 0 || bone >= ownerModel.NumBones)
                continue;
            float value = medium;
            if (index <= 2)
                value = low;
            else if (index == 5 || index == 7 || index == 8 || index == 9 ||
                index == 14 || index == 15 || index == 16)
                value = high;
            Vector(value, value, value, light);
            ownerModel.TransformByObjectBone(position, effect.Owner, bone,
                local);
            CreateSprite(BITMAP_LIGHT_RED, position, 1.5f, light,
                &effect); // 0x6778A6 pushes aura ESI, not actor EDI
        }
        return true;
    }
    default:
        return false;
    }
}

}}
