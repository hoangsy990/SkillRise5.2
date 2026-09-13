#include "stdafx.h"
#include "PegasusCrusherChargeRuntime.h"
#include "PegasusCrusherChargeMath.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "GlobalBitmap.h"
#include "ZzzLodTerrain.h"
#include "ZzzOpenglUtil.h"
#include "PegasusRuntimeTrace.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

namespace rise { namespace pegasus {
namespace {
int g_crusherSteamBitmap = BITMAP_UNKNOWN;
int g_crusherSmokeLinesBitmap = BITMAP_UNKNOWN;
int g_crusherFlareMonoBitmap = BITMAP_UNKNOWN;

bool EnsureCrusherChargeBitmaps()
{
    // Pegasus registrations 0x86E373..0x86E3C5 map 0x7F1D..0x7F1F to
    // Steam_01, smokelines03 and flare_mono. Keep source numeric IDs out of
    // RISE and let the path-keyed bitmap loader allocate collision-free IDs.
    const char* root = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    if (g_crusherSteamBitmap == BITMAP_UNKNOWN)
        g_crusherSteamBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "Steam_01.jpg").c_str(), GL_LINEAR,
            GL_CLAMP_TO_EDGE);
    if (g_crusherSmokeLinesBitmap == BITMAP_UNKNOWN)
        g_crusherSmokeLinesBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "smokelines03.jpg").c_str(), GL_LINEAR,
            GL_CLAMP_TO_EDGE);
    if (g_crusherFlareMonoBitmap == BITMAP_UNKNOWN)
        g_crusherFlareMonoBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "flare_mono.jpg").c_str(), GL_LINEAR,
            GL_CLAMP_TO_EDGE);
    return g_crusherSteamBitmap != BITMAP_UNKNOWN &&
        g_crusherSmokeLinesBitmap != BITMAP_UNKNOWN &&
        g_crusherFlareMonoBitmap != BITMAP_UNKNOWN;
}

const char* ModelFile(int modelId)
{
    return (modelId == kCrusherChargeRingModel) ? "03_buff_obj.bmd" : "02_buff_obj.bmd";
}

void CreateShard(OBJECT& ring, int subtype, float scale, float r, float g, float b)
{
    vec3_t light;
    Vector(r, g, b, light);
    CreateEffect(kCrusherChargeShardModel, ring.Position, ring.Angle, light,
        subtype, &ring, -1, 0, ring.Kind, 0, scale);
}
}

bool EnsureCrusherChargeModel(int modelId)
{
    if (!EnsureCrusherChargeBitmaps())
        return false;
    if (!Models || !IsCrusherChargeModelId(modelId))
        return false;
    // Pegasus 0x65ACAB renders 0x27B0 as Steam_01 terrain alpha only. It is
    // not a third copy of 02_buff_obj.bmd.
    if (modelId == kCrusherChargeShardModel ||
        modelId == kCrusherChargeAuraModel)
        return true;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture)
        return true;
    char directory[] = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    char filename[32];
    strcpy_s(filename, ModelFile(modelId));
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
    return model.NumMeshs > 0;
}

void CreateCrusherChargeRoots(OBJECT& owner, int /*skill*/)
{
    vec3_t relative, position;
    Vector(0.0f, 0.0f, -60.0f, relative);
    VectorCopy(owner.Position, position);
    if (owner.BoneTransform)
        Models[owner.Type].TransformByObjectBone(position, &owner, 0, relative);
    CreateEffect(kCrusherChargeCoreModel, position, owner.Angle, owner.Light,
        0, &owner, -1, 0, 0, 0, 0.05f);
    CreateEffect(kCrusherChargeRingModel, owner.Position, owner.Angle, owner.Light,
        0, &owner, -1, 0, 0);
}

void CreateCrusherChargeAura(OBJECT& owner)
{
    // Original 0x65B0BD rejects a non-live owner before replacing its aura.
    if (!owner.Live)
        return;
    // Pegasus 0x65B0A5: buff 0xD7 owns one persistent subtype-1 aura.
    DeleteEffect(kCrusherChargeAuraModel, &owner);
    vec3_t light;
    Vector(0.2f, 0.3f, 0.7f, light);
    CreateEffect(kCrusherChargeAuraModel, owner.Position, owner.Angle, light,
        1, &owner, -1, 0, 0, 0, 1.5f);
}

void DeleteCrusherChargeAura(OBJECT& owner)
{
    // Pegasus 0x65B129, called when buff 0xD7 is removed.
    const bool existed = SearchEffect(kCrusherChargeAuraModel, &owner);
    DeleteEffect(kCrusherChargeAuraModel, &owner);
    TraceRuntime("BUFF_EFFECT_CLEAR", kCrusherChargeAuraModel,
        existed ? 1 : 0, SearchEffect(kCrusherChargeAuraModel, &owner)
            ? "STILL_LIVE" : "CLEARED");
}

void InitializeCrusherCharge(OBJECT& effect)
{
    effect.BlendMesh = -2;
    switch (effect.Type)
    {
    case kCrusherChargeCoreModel:
        // Pegasus 0x27AE is a deliberately invisible child-orchestration
        // carrier: 0x65A49A initializes zero scale/alpha and the updater's
        // type dispatch begins at 0x27AF. Do not invent a fade/scale route.
        effect.LifeTime = (effect.SubType == 1) ? 20.0f : 35.0f;
        effect.Scale = 0.0f;
        effect.Alpha = 0.0f;
        effect.Gravity = 0.05f;
        break;
    case kCrusherChargeRingModel:
        effect.LifeTime = (effect.SubType == 1) ? 20.0f : 35.0f;
        effect.Alpha = 0.0f;
        effect.Scale = 0.5f;
        Vector(0.1f, 0.35f, 0.05f, effect.Light);
        effect.HeadAngle[0] = static_cast<float>(rand() % 361);
        effect.HeadAngle[1] = static_cast<float>(rand() % 361);
        effect.HeadAngle[2] = static_cast<float>(rand() % 361);
        if (effect.SubType == 0)
            CreateCrusherChargeChildren(effect);
        break;
    case kCrusherChargeShardModel:
        effect.LifeTime = 35.0f;
        effect.Alpha = 1.0f;
        if (effect.SubType == 4)
            effect.Gravity = (effect.Light[0] == 0.2f) ? 0.4f : 0.1f;
        break;
    case kCrusherChargeAuraModel:
        effect.LifeTime = kCrusherChargeAuraLifetime;
        effect.Alpha = 1.0f;
        effect.Scale = 1.5f;
        Vector(0.2f, 0.3f, 0.7f, effect.Light);
        break;
    }
}

void CreateCrusherChargeChildren(OBJECT& ring)
{
    CreateShard(ring, 5, 1.2f, 0.4f, 0.8f, 0.9f);
    CreateShard(ring, 5, 1.4f, 0.4f, 0.8f, 0.9f);
    CreateShard(ring, 4, 0.5f, 0.2f, 0.8f, 0.5f);
    CreateShard(ring, 4, 0.5f, 0.05f, 0.4f, 0.2f);
}

void UpdateCrusherCharge(OBJECT& effect, float animationFactor)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    const int remaining = static_cast<int>(effect.LifeTime);
    if (effect.Type == kCrusherChargeShardModel && effect.SubType >= 4)
    {
        if (effect.SubType == 4)
            effect.Scale += effect.Gravity * animationFactor;
        else
            effect.Angle[2] += 5.0f * animationFactor;
        if (remaining < 17)
            effect.Alpha = static_cast<float>(remaining) / 17.0f;
        else
            effect.Alpha += animationFactor / 17.0f;
    }
    else if (effect.Type == kCrusherChargeRingModel)
    {
        effect.Angle[2] += 10.0f * animationFactor;
        const int halfLife = (effect.SubType == 1) ? 10 : 17;
        if (remaining < halfLife)
            effect.Alpha -= animationFactor / static_cast<float>(halfLife);
        else
        {
            if (effect.SubType == 0)
            {
                const float previous = effect.LifeTime + animationFactor;
                const float thresholds[] = {32.0f, 28.0f, 24.0f, 20.0f};
                for (float threshold : thresholds)
                {
                    if (effect.LifeTime <= threshold && previous > threshold)
                        CreateEffect(kCrusherChargeRingModel, effect.Position,
                            effect.Angle, effect.Light, 1, &effect, -1, 0,
                            effect.Kind);
                }
            }
            effect.Alpha += animationFactor / static_cast<float>(halfLife);
        }
    }
    else if (effect.Type == kCrusherChargeAuraModel)
    {
        // Pegasus 0x65A80B..0x65A835 only follows the live owner here.
        // Buff removal is packet-owned by 0x65B129; a local marker check can
        // incorrectly erase the aura while add/relog packets are in flight.
        VectorCopy(effect.Owner->Position, effect.Position);
    }
    if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
    if (effect.Alpha > 1.0f) effect.Alpha = 1.0f;
}

bool RenderCrusherCharge(OBJECT& effect)
{
    if (!EnsureCrusherChargeBitmaps())
        return false;
    if (effect.Type == kCrusherChargeShardModel)
    {
        vec3_t light;
        Vector(effect.Light[0] * effect.Alpha, effect.Light[1] * effect.Alpha,
            effect.Light[2] * effect.Alpha, light);
        const float rotation = effect.SubType == 4
            ? effect.Angle[0] : effect.Angle[2];
        EnableAlphaBlend();
        RenderTerrainAlphaBitmap(g_crusherSteamBitmap, effect.Position[0],
            effect.Position[1], effect.Scale, effect.Scale, light, rotation,
            1.0f, 5.0f);
        DisableAlphaBlend();
        return true;
    }
    if (effect.Type == kCrusherChargeAuraModel)
    {
        if (effect.SubType != 1 || !effect.Owner || !effect.Owner->Live ||
            !effect.Owner->BoneTransform)
            return true;
        vec3_t light;
        const float center = CrusherChargeCenterPulse(WorldTime);
        Vector(effect.Light[0] * center, effect.Light[1] * center,
            effect.Light[2] * center, light);
        const float size = effect.Scale * 1.5f;
        EnableAlphaBlend();
        for (int layer = 0; layer < 2; ++layer)
            RenderTerrainAlphaBitmap(g_crusherFlareMonoBitmap,
                effect.Owner->Position[0], effect.Owner->Position[1], size,
                size, light, effect.Angle[0], effect.Alpha, 5.0f);
        DisableAlphaBlend();

        BMD& ownerModel = Models[effect.Owner->Type];
        vec3_t local, position;
        Vector(0.0f, 0.0f, 0.0f, local);
        for (int index = 0; index < kCrusherChargeAuraBoneCount; ++index)
        {
            const int bone = kCrusherChargeAuraBones[index];
            if (bone < 0 || bone >= ownerModel.NumBones)
                continue;
            const float pulse = CrusherChargeBonePulse(WorldTime, index);
            Vector(effect.Light[0] * pulse, effect.Light[1] * pulse,
                effect.Light[2] * pulse, light);
            ownerModel.TransformByObjectBone(position, effect.Owner, bone,
                local);
            CreateSprite(g_crusherFlareMonoBitmap, position, 1.5f, light,
                effect.Owner, 0.0f);
        }
        return true;
    }
    if (!EnsureCrusherChargeModel(effect.Type))
        return false;
    if (effect.Type == kCrusherChargeRingModel && effect.SubType == 0)
    {
        vec3_t light;
        Vector(effect.Alpha, effect.Alpha, effect.Alpha, light);
        const float sizes[] = {4.0f, 3.0f, 2.0f};
        EnableAlphaBlend();
        for (int layer = 0; layer < 3; ++layer)
            RenderTerrainAlphaBitmap(g_crusherSmokeLinesBitmap,
                effect.Position[0], effect.Position[1], sizes[layer],
                sizes[layer], light, effect.HeadAngle[layer], 1.0f, 5.0f);
        DisableAlphaBlend();
    }
    if (!Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    VectorCopy(effect.Light, model.BodyLight);
    // Pegasus 0x65AEC7..0x65AF1F: one mesh-0 bright pass, alpha supplied as
    // both body alpha and blend multiplier. The old all-mesh loop was not the
    // source renderer and changed both material layering and colour.
    model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha, 0,
        effect.Alpha, effect.BlendMeshTexCoordU,
        effect.BlendMeshTexCoordV, -1);
    return true;
}

}}
