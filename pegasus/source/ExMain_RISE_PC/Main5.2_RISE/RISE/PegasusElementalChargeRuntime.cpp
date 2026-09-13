#include "stdafx.h"
#include "PegasusElementalChargeRuntime.h"
#include "PegasusElementalChargeMath.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "GlobalBitmap.h"
#include "PegasusTextureIds.h"
#include "PegasusRuntimeTrace.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

namespace rise { namespace pegasus {

namespace {
const int kElementalBitmaps[kElementalLayerCount] = {
    kElementalChargeBlueBitmap, kElementalChargeYellowBitmap,
    kElementalChargeRedBitmap, kElementalChargePurpleBitmap,
    kElementalChargeGreenBitmap
};
bool EnsureElementalChargeBitmaps()
{
    static const char* paths[kElementalLayerCount] = {
        "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\ElementalCharge\\flareBlue.jpg",
        "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Flare_yellow.jpg",
        "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\flareRed.jpg",
        "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Flare_puple.jpg",
        "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Flare_green.jpg"
    };
    for (int n = 0; n < kElementalLayerCount; ++n)
    {
        if (!Bitmaps.FindTexture(kElementalBitmaps[n]) &&
            !Bitmaps.LoadImageFile(kElementalBitmaps[n], paths[n],
                GL_LINEAR, GL_CLAMP_TO_EDGE))
            return false;
    }
    struct ChildBitmap
    {
        int id;
        const char* path;
    };
    static const ChildBitmap childBitmaps[] = {
        { kElementalFlare03Bitmap,
          "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\flare03.jpg" },
        { kElementalShockwaveBitmap,
          "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Shockwave.jpg" }
    };
    for (int n = 0; n < 2; ++n)
    {
        if (!Bitmaps.FindTexture(childBitmaps[n].id) &&
            !Bitmaps.LoadImageFile(childBitmaps[n].id,
                childBitmaps[n].path, GL_LINEAR, GL_REPEAT))
            return false;
    }
    return true;
}
}

bool EnsureElementalChargeModel()
{
    if (!EnsureElementalChargeBitmaps() || !Models) return false;
    BMD& model = Models[kElementalChargeModel];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture) return true;
    char directory[] = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    char filename[] = "05_buff_obj.bmd";
    model.m_iBMDSeqID = kElementalChargeModel;
    if (!model.Open2(directory, filename)) return false;
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
    OGL330MODEL::ConvertOldMeshToVaoMesh(kElementalChargeModel);
#endif
    return model.NumMeshs > 0;
}

void CreateElementalCharge(OBJECT& target, int /*skill*/)
{
    // Pegasus 0x664CF8 resolves target bone 17 and creates 0x27B2 subtype 0.
    vec3_t position, relative;
    VectorCopy(target.Position, position);
    Vector(0.0f, 0.0f, 0.0f, relative);
    if (target.BoneTransform)
        Models[target.Type].TransformByObjectBone(position, &target, 17, relative);
    CreateEffect(kElementalChargeModel, position, target.Angle, target.Light,
        kElementalCastSubtype, &target, -1, 0,
        0, 0, 1.0f);
}

void CreateElementalChargeBuffAura(OBJECT& target)
{
    // Original 0x664D9C gates the fallback search and creation on owner live.
    if (!target.Live)
        return;
    // Pegasus 0x664D8E: the buff-add path creates subtype 1 only when the
    // authoritative cast subtype 0 is not already present (e.g. viewport or
    // relog restoration).
    if (!ShouldCreateElementalBuffFallback(
        SearchEffect(kElementalChargeModel, &target,
            kElementalCastSubtype)))
        return;
    vec3_t position, relative;
    VectorCopy(target.Position, position);
    Vector(0.0f, 0.0f, 0.0f, relative);
    if (target.BoneTransform)
        Models[target.Type].TransformByObjectBone(position, &target, 17,
            relative);
    CreateEffect(kElementalChargeModel, position, target.Angle, target.Light,
        kElementalBuffFallbackSubtype, &target, -1, 0, 0, 0, 1.0f);
}

void DeleteElementalChargeAuras(OBJECT& target)
{
    // Pegasus 0x664DC6 removes both the cast root and the relog fallback.
    const int before = (SearchEffect(kElementalChargeModel, &target,
        kElementalCastSubtype) ? 1 : 0) +
        (SearchEffect(kElementalChargeModel, &target,
            kElementalBuffFallbackSubtype) ? 1 : 0);
    DeleteEffect(kElementalChargeModel, &target, kElementalCastSubtype);
    DeleteEffect(kElementalChargeModel, &target,
        kElementalBuffFallbackSubtype);
    const int after = (SearchEffect(kElementalChargeModel, &target,
        kElementalCastSubtype) ? 1 : 0) +
        (SearchEffect(kElementalChargeModel, &target,
            kElementalBuffFallbackSubtype) ? 1 : 0);
    TraceRuntime("BUFF_EFFECT_CLEAR", kElementalChargeModel, before,
        after == 0 ? "CLEARED" : "STILL_LIVE");
}

void InitializeElementalCharge(OBJECT& effect)
{
    // The Pegasus buff root uses a deliberately effectively-unbounded life;
    // owner liveness/effect deletion remains the authoritative cleanup gate.
    effect.LifeTime = 100000000.0f;
    effect.Scale = 1.0f;
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    if (effect.SubType == kElementalCastSubtype)
    {
        // Pegasus 0x66481B..0x664949. These are five CreateEffect calls,
        // not joint-pool entries: three flare03 carriers followed by two
        // Shockwave carriers. Preserve source ordering because each child
        // captures the root's initial position and owns its own fade.
        vec3_t lights[kElementalConstructorChildCount] = {
            { 0.075f, 0.2f, 0.2f },
            { 0.075f, 0.2f, 0.2f },
            { 0.01f, 0.2f, 0.2f },
            { 0.1f, 0.2f, 0.2f },
            { 0.1f, 0.2f, 0.2f }
        };
        for (int n = 0; n < kElementalConstructorChildCount; ++n)
        {
            const int type = n < 3 ? kElementalFlare03Bitmap :
                kElementalShockwaveBitmap;
            CreateEffect(type, effect.Position, effect.Angle, lights[n],
                kElementalConstructorChildSubtypes[n], &effect, -1,
                0, 0, 0, kElementalConstructorChildScales[n]);
        }
    }
}

bool IsElementalChargeChild(int type)
{
    return type == kElementalFlare03Bitmap ||
        type == kElementalShockwaveBitmap;
}

void InitializeElementalChargeChild(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.Alpha = 1.0f;
    if (effect.Type == kElementalFlare03Bitmap)
    {
        // Pegasus 0x6647DB..0x664816: Gravity captures the authored red
        // component before the body light becomes the blue flare gradient.
        effect.LifeTime = kElementalFlare03Lifetime;
        effect.Gravity = effect.Light[0];
        Vector(0.6f, 0.8f, 1.0f, effect.Light);
    }
    else
    {
        // Pegasus 0x6647B7..0x664816.
        effect.LifeTime = kElementalShockwaveLifetime;
        effect.Scale = kElementalShockwaveScale;
        effect.Gravity = kElementalShockwaveGrowthStart;
        Vector(0.3f, 0.8f, 1.0f, effect.Light);
    }
    TraceRuntime("ELEMENTAL_CHILD_CREATE", effect.Type,
        effect.SubType, "ALLOCATED");
}

void UpdateElementalChargeChild(OBJECT& effect, float animationFactor)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0.0f;
        return;
    }

    effect.Gravity += effect.Scale * animationFactor;
    if (effect.Type == kElementalFlare03Bitmap)
        effect.Alpha -= kElementalFlare03AlphaDecay * animationFactor;
    else
        effect.Alpha = effect.LifeTime / kElementalShockwaveLifetime;

    // Pegasus 0x664A20..0x664A72 compounds the carrier light by the current
    // alpha and emits a sprite with the growing Gravity value as its scale.
    VectorScale(effect.Light, effect.Alpha, effect.Light);
    CreateSprite(effect.Type, effect.Position, effect.Gravity, effect.Light,
        &effect, 0.0f, 0);
}

void UpdateElementalCharge(OBJECT& effect, float)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    // Pegasus 0x664960..0x664AB7 only tracks owner bone 17 here. Buff
    // removal is authoritative through DeleteElementalChargeAuras; adding a
    // delayed local marker check makes valid effects disappear when packet
    // timing or viewport restoration differs.
    vec3_t relative;
    Vector(0.0f, 0.0f, 0.0f, relative);
    if (effect.Owner->BoneTransform)
        Models[effect.Owner->Type].TransformByObjectBone(effect.Position,
            effect.Owner, 17, relative);
}

bool RenderElementalCharge(OBJECT& effect)
{
    if (!EnsureElementalChargeModel() || !EnsureElementalChargeBitmaps())
        return false;
    BMD& model = Models[kElementalChargeModel];
    // Pegasus 0x664ABA..0x664B27 advances the 05_buff_obj BMD before its
    // bone transforms and five mesh passes. Dynamic RISE model IDs live past
    // MODEL_SKILL_END, so the native generic effect loop never does this.
    model.CurrentAction = effect.CurrentAction;
    model.PlayAnimation(&effect.AnimationFrame,
        &effect.PriorAnimationFrame, &effect.PriorAction, effect.Velocity,
        effect.Position, effect.Angle);
    TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,
        effect.AnimationFrame);
    if (!Calc_RenderObject(&effect, false, 0, 0))
        return false;
    vec3_t zero, position, pulseLight;
    Vector(0.0f, 0.0f, 0.0f, zero);
    for (int layer = 0; layer < kElementalLayerCount; ++layer)
    {
        const float pulse = ElementalPulse(WorldTime, layer);
        Vector(pulse, pulse, pulse, pulseLight);
        model.TransformPosition(BoneTransform[kElementalBones[layer]], zero,
            position, false); // 0x664B74: Calc_RenderObject(false) already translated
        const int spriteSlot = CreateSprite(kElementalBitmaps[layer], position, 1.0f, pulseLight,
            &effect, 0.0f); // 0x664BB8: sprite owner is the buff carrier
#ifdef PEGASUS_ISOLATED_TEST
        static bool tracedTip[kElementalLayerCount] = {};
        if (!tracedTip[layer] && pulse > 0.75f)
        {
            tracedTip[layer] = true;
            char detail[256];
            _snprintf_s(detail, sizeof(detail), _TRUNCATE,
                "bone=%d slot=%d pulse=%.3f tip=%.1f,%.1f,%.1f root=%.1f,%.1f,%.1f frame=%.3f",
                kElementalBones[layer], spriteSlot, pulse,
                position[0], position[1], position[2],
                effect.Position[0], effect.Position[1], effect.Position[2],
                effect.AnimationFrame);
            TraceRuntime("ELEMENTAL_TIP_INPUT", layer, kElementalBitmaps[layer], detail);
        }
#endif
        VectorCopy(pulseLight, model.BodyLight);
        if (layer < model.NumMeshs)
        {
            // Pegasus 0x664C04..0x664C33: alpha=pulse, BlendMesh=0,
            // BlendMeshLight=pulse and the native bright-texture flag 0x42.
            model.RenderMesh(layer, RENDER_TEXTURE | RENDER_BRIGHT,
                pulse, 0, pulse, effect.BlendMeshTexCoordU,
                effect.BlendMeshTexCoordV, -1);
        }
    }
    return true;
}

}}
