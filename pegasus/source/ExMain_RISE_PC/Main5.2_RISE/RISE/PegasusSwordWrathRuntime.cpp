#include "stdafx.h"
#include "PegasusSwordWrathRuntime.h"
#include "PegasusSwordWrathMath.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "GlobalBitmap.h"
#include "ZzzLodTerrain.h"
#include "ZzzOpenglUtil.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"
#include <cmath>

namespace rise { namespace pegasus {
static_assert(PLAYER_SKILL_VITALITY == 0x43,
    "RISE self-buff action no longer matches the recovered Pegasus fallback");
namespace {
int g_swordWrathIronBitmap = BITMAP_UNKNOWN;
int g_swordWrathGroundStarBitmap = BITMAP_UNKNOWN;
int g_swordWrathWaveBitmap = BITMAP_UNKNOWN;
int g_swordWrathLineBitmap = BITMAP_UNKNOWN;
int g_swordWrathSmokeBitmap = BITMAP_UNKNOWN;
int g_swordWrathPinStarBitmap = BITMAP_UNKNOWN;

float SwordWrathRandom(float low, float high)
{
    return low + (high - low) * static_cast<float>(rand() % 1001) / 1000.0f;
}

bool EnsureSwordWrathBitmaps()
{
    const char* root = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    if (g_swordWrathIronBitmap == BITMAP_UNKNOWN)
        g_swordWrathIronBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "mzine_irondefence.jpg").c_str(),
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_swordWrathGroundStarBitmap == BITMAP_UNKNOWN)
        g_swordWrathGroundStarBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "ground_star.jpg").c_str(),
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_swordWrathWaveBitmap == BITMAP_UNKNOWN)
        g_swordWrathWaveBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "wave.jpg").c_str(),
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_swordWrathLineBitmap == BITMAP_UNKNOWN)
        g_swordWrathLineBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "Swordeff_mono2line.jpg").c_str(),
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_swordWrathSmokeBitmap == BITMAP_UNKNOWN)
        g_swordWrathSmokeBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "smoke01_strong.jpg").c_str(),
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_swordWrathPinStarBitmap == BITMAP_UNKNOWN)
        g_swordWrathPinStarBitmap = Bitmaps.LoadImageFile(
            (std::string(root) + "pin_star02_red.jpg").c_str(),
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    return g_swordWrathIronBitmap != BITMAP_UNKNOWN &&
        g_swordWrathGroundStarBitmap != BITMAP_UNKNOWN &&
        g_swordWrathWaveBitmap != BITMAP_UNKNOWN &&
        g_swordWrathLineBitmap != BITMAP_UNKNOWN &&
        g_swordWrathSmokeBitmap != BITMAP_UNKNOWN &&
        g_swordWrathPinStarBitmap != BITMAP_UNKNOWN;
}

int SwordWrathLayerBitmap(int subtype)
{
    if (subtype == 0 || subtype == 1) return g_swordWrathIronBitmap;
    if (subtype == 2) return g_swordWrathGroundStarBitmap;
    return g_swordWrathWaveBitmap;
}

void GetSwordWrathBone17(OBJECT& effect, vec3_t position)
{
    VectorCopy(effect.Position, position);
    // Pegasus 0x67AADD passes the root owner (EDI), not the 0x27B3 effect,
    // into 0x67A6F2. That helper resolves the owner's model and bone 17.
    OBJECT* owner = effect.Owner;
    if (!owner || !owner->Live || !Models || !owner->BoneTransform ||
        owner->Type < 0 || owner->Type >= MAX_MODELS ||
        Models[owner->Type].NumBones <= 17)
        return;
    vec3_t relative;
    Vector(0.0f, 0.0f, 0.0f, relative);
    Models[owner->Type].TransformByObjectBone(position, owner, 17, relative);
}

void CreateSwordWrathAccentParticles(OBJECT& effect)
{
    vec3_t position, angle, light;
    VectorCopy(effect.Position, position);
    position[0] += SwordWrathRandom(-50.0f, 50.0f);
    position[1] += SwordWrathRandom(-50.0f, 50.0f);
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateParticle(g_swordWrathPinStarBitmap, position, effect.Angle, light,
        1, SwordWrathRandom(0.6f, 2.5f) * kSwordWrathPinStarInputScale,
        effect.Owner);

    if (effect.LifeTime > static_cast<float>(kSwordWrathSmokeEmitThreshold))
    {
        GetSwordWrathBone17(effect, position);
        Vector(0.25f, 0.05f, 0.0475f, light);
        CreateParticle(g_swordWrathSmokeBitmap, position, effect.Angle, light,
            2, SwordWrathRandom(1.5f, 3.0f), effect.Owner);
    }
}
}

int GetSwordWrathLineBitmap()
{
    return EnsureSwordWrathBitmaps() ? g_swordWrathLineBitmap : BITMAP_UNKNOWN;
}

bool IsSwordWrathLineBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN && bitmapId == g_swordWrathLineBitmap;
}

int GetSwordWrathSmokeBitmap()
{
    return EnsureSwordWrathBitmaps() ? g_swordWrathSmokeBitmap : BITMAP_UNKNOWN;
}

bool IsSwordWrathSmokeBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN && bitmapId == g_swordWrathSmokeBitmap;
}

int GetSwordWrathPinStarBitmap()
{
    return EnsureSwordWrathBitmaps() ? g_swordWrathPinStarBitmap : BITMAP_UNKNOWN;
}

bool IsSwordWrathPinStarBitmap(int bitmapId)
{
    return bitmapId != BITMAP_UNKNOWN && bitmapId == g_swordWrathPinStarBitmap;
}

bool EnsureSwordWrathModel(int modelId)
{
    if (!EnsureSwordWrathBitmaps())
        return false;
    if (!Models || modelId < kSwordWrathRootModel || modelId > kSwordWrathLayerModel)
        return false;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture)
        return true;
    char directory[] = "Data\\RISE\\Pegasus\\SwordWrath\\Effect\\";
    char filename[] = "Fire_wall_knight.bmd";
    model.m_iBMDSeqID = modelId;
    if (!model.Open2(directory, filename))
        return false;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        const std::string texturePath = std::string(directory) + model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(texturePath, GL_NEAREST, GL_REPEAT);
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

void InitializeSwordWrath(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    effect.Alpha = (effect.SubType == 2) ? 0.0f : 1.0f;
    if (effect.Type == kSwordWrathRootModel)
    {
        effect.LifeTime = kSwordWrathRootLifetime;
        effect.Scale = kSwordWrathRootScale;
        return;
    }
    switch (effect.SubType)
    {
    case 0: effect.LifeTime = kSwordWrathLayerPulseLifetime; effect.Scale = kSwordWrathIronScale; break;
    case 1: effect.LifeTime = kSwordWrathLayerPulseLifetime; effect.Scale = kSwordWrathIronInnerScale; break;
    case 2: effect.LifeTime = kSwordWrathGroundStarLifetime; effect.Scale = kSwordWrathGroundStarScale; break;
    case 3: effect.LifeTime = kSwordWrathWaveLifetime; effect.Scale = kSwordWrathWaveScale; break;
    default: effect.LifeTime = 1; break;
    }
}

void CreateSwordWrathLayers(OBJECT& root)
{
    if (!root.Owner || !root.Owner->Live)
        return;
    for (int subtype = 0; subtype <= 3; ++subtype)
    {
        vec3_t light;
        const SwordWrathColor color = subtype == 3
            ? kSwordWrathWaveLight : kSwordWrathRedLight;
        Vector(color.r, color.g, color.b, light);
        const int copies = (subtype == 2) ? 2 : 1;
        for (int n = 0; n < copies; ++n)
            CreateEffect(kSwordWrathLayerModel, root.Position, root.Angle,
                light, subtype, root.Owner);
    }
    const int lineBitmap = GetSwordWrathLineBitmap();
    if (lineBitmap != BITMAP_UNKNOWN)
    {
        vec3_t light;
        Vector(kSwordWrathParticleLight.r, kSwordWrathParticleLight.g,
            kSwordWrathParticleLight.b, light);
        for (int n = 0; n < 30; ++n)
            CreateJoint(lineBitmap, root.Position, root.Position, root.Angle,
                0, root.Owner, 10.0f, -1, kSwordWrathSkill, 0, -1, light, -1);
    }
}

void UpdateSwordWrath(OBJECT& effect, float animationFactor)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0;
        return;
    }
    if (effect.Type == kSwordWrathRootModel)
    {
        // Pegasus 0x67ACB8..0x67ACD9 keeps the long-lived root on its owner.
        VectorCopy(effect.Owner->Position, effect.Position);
        effect.Alpha = SwordWrathPulse(static_cast<int>(effect.LifeTime), 20, 0.7f);
    }
    else if (effect.SubType == 2)
    {
        // The ground-star remains at the cast-time position. Pegasus does not
        // copy owner position in the 0x67ABD5 subtype-2 branch.
        effect.Scale += SwordWrathGroundStarScaleDelta(
            static_cast<int>(effect.LifeTime), animationFactor);
        effect.Alpha += SwordWrathGroundStarAlphaDelta(
            static_cast<int>(effect.LifeTime), animationFactor);
        if (effect.Alpha > 1.0f) effect.Alpha = 1.0f;
        if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
    }
    else if (effect.SubType == 3)
    {
        // The expanding wave is likewise a fixed cast-time ground layer.
        effect.Alpha -= 0.05f * animationFactor;
        if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
        effect.Scale += 0.2f * animationFactor;
    }
    else
    {
        // Only layer subtypes 0/1 follow the owner (0x67AC4D..0x67AC6E).
        VectorCopy(effect.Owner->Position, effect.Position);
        effect.Alpha = SwordWrathPulse(static_cast<int>(effect.LifeTime), 15, 1.0f);
    }
    effect.BlendMeshLight = effect.Alpha;
    if (ShouldEmitSwordWrathAccent(effect.Type == kSwordWrathRootModel,
        effect.LifeTime, animationFactor))
        CreateSwordWrathAccentParticles(effect);
}

bool RenderSwordWrath(OBJECT& effect)
{
    if (!EnsureSwordWrathModel(effect.Type))
        return false;
    if (effect.Type == kSwordWrathRootModel &&
        (!effect.Owner || !IsSwordWrathRenderAction(effect.Owner->CurrentAction)))
        return true;
    if (effect.Type == kSwordWrathLayerModel)
    {
        vec3_t light;
        VectorScale(effect.Light, effect.Alpha, light);
        EnableAlphaBlend();
        RenderTerrainAlphaBitmap(SwordWrathLayerBitmap(effect.SubType),
            effect.Position[0], effect.Position[1], effect.Scale, effect.Scale,
            light, effect.Angle[2], 1.0f, 5.0f);
        DisableAlphaBlend();
        return true;
    }
    if (!Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    // Pegasus 0x67AF10..0x67AF5F animates fire_15fmono as a 4x4 sheet with
    // fifteen used frames. Keep this separate from blend subtype semantics.
    effect.BlendMeshTexCoordU = SwordWrathAtlasU(effect.LifeTime);
    effect.BlendMeshTexCoordV = SwordWrathAtlasV(effect.LifeTime);
    // Pegasus 67AF37/67AF54 select texture slot zero and the current pulse.
    // The shared default 850D0D ->84DDFF forwards these to RenderBody(flags2).
    // Do not force every mesh bright: the native blend slot owns that choice.
    effect.BlendMesh = 0;
    effect.BlendMeshLight = effect.Alpha;
    VectorCopy(effect.Light, model.BodyLight);
    model.RenderBody(RENDER_TEXTURE, effect.Alpha, effect.BlendMesh,
        effect.BlendMeshLight, effect.BlendMeshTexCoordU,
        effect.BlendMeshTexCoordV, effect.HiddenMesh, -1);
    return true;
}

}}
