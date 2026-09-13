#include "stdafx.h"
#include "PegasusDexBoosterRuntime.h"
#include "PegasusDexBoosterContract.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "ZzzOpenglUtil.h"
#include "ZzzLodTerrain.h"
#include "GlobalBitmap.h"

namespace rise { namespace pegasus {
namespace {
struct BitmapRow { int id; const char* path; };
const BitmapRow kBitmapRows[] = {
    // CGlobalBitmap accepts logical .jpg names and resolves the encrypted
    // on-disk .OZJ sibling internally. Passing .OZJ directly is rejected by
    // its extension dispatcher before any file is opened.
    {kDexBoosterImpactBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Impack03.jpg"},
    {kDexBoosterShinyBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Shiny02.jpg"},
    {kDexBoosterHikororaBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\hikorora.jpg"},
    {kDexBoosterGroundBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\twlighthik01.jpg"},
    {kDexBoosterFlareBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Flare.jpg"},
    {kDexBoosterFlare01Bitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\flare01.jpg"},
};
}

bool EnsureDexBoosterBitmaps()
{
    for (const BitmapRow& row : kBitmapRows)
    {
        if (!Bitmaps.FindTexture(row.id) &&
            !Bitmaps.LoadImageFile(row.id, row.path, GL_LINEAR, GL_CLAMP_TO_EDGE))
            return false;
    }
    return true;
}

void CreateDexBooster(OBJECT& owner, int /*skill*/)
{
    if (!EnsureDexBoosterBitmaps())
        return;
    CreateEffect(kDexBoosterAuraModel, owner.Position, owner.Angle, owner.Light,
        3, &owner, -1, 0, 0);
    CreateEffect(kDexBoosterOrchestratorModel, owner.Position, owner.Angle, owner.Light,
        0, &owner, -1, 0, 0);
}

void InitializeDexBooster(OBJECT& effect)
{
    effect.BlendMesh = -2;
    switch (effect.Type)
    {
    case kDexBoosterAuraModel:
        effect.LifeTime = kDexBoosterAuraLifetime;
        effect.Scale = 0.0f;
        effect.Alpha = 1.0f;
        break;
    case kDexBoosterOrchestratorModel:
        // Pegasus 0x66117A emits its children during initialization and keeps
        // no visible orchestrator alive afterward.
        effect.LifeTime = 0.0f;
        CreateDexBoosterLayers(effect);
        break;
    case kDexBoosterGroundModel:
        effect.LifeTime = kDexBoosterGroundLifetime;
        effect.Alpha = kDexBoosterGroundStartLight;
        break;
    case kDexBoosterFlareModel:
        effect.LifeTime = kDexBoosterFlareLifetime;
        effect.Scale = kDexBoosterFlareScale;
        break;
    }
}

void CreateDexBoosterLayers(OBJECT& orchestrator)
{
    if (!orchestrator.Owner || !orchestrator.Owner->Live)
        return;
    vec3_t light;

    // Pegasus 0x661190..0x6611CE: nineteen native flare joints, subtype 48,
    // at the owner-bound cast origin. 0x7D83 is the native BITMAP_FLARE
    // semantic in this engine family; do not allocate a custom texture ID.
    for (int n = 0; n < kDexBoosterInitialFlareJointCount; ++n)
    {
        CreateJoint(BITMAP_FLARE, orchestrator.Position,
            orchestrator.Position, orchestrator.Angle,
            kDexBoosterFlareJointSubtype, orchestrator.Owner,
            kDexBoosterFlareJointScale, 2, kDexBoosterSkill);
    }

    // Pegasus 0x6611D0..0x66120C: twenty Impack03 particles with exact
    // (0.4,1,1) light and 0.005 scale. This layer was omitted by the first
    // adapter and is independent of the three ground rings below.
    Vector(0.4f, 1.0f, 1.0f, light);
    for (int n = 0; n < kDexBoosterInitialImpactParticleCount; ++n)
    {
        CreateParticle(kDexBoosterImpactBitmap, orchestrator.Position,
            orchestrator.Angle, light, 0, kDexBoosterImpactParticleScale);
    }

    Vector(1.0f, 0.5f, 0.0f, light);
    for (int subtype = 9; subtype <= 11; ++subtype)
        CreateEffect(kDexBoosterGroundModel, orchestrator.Position, orchestrator.Angle,
            light, subtype, orchestrator.Owner, -1, 0, orchestrator.Kind, 0, 5.0f);
    // Pegasus 0x66125A..0x6612EF selects a fresh random owner bone for each
    // of the two flare emitters. They are intentionally unowned after their
    // spawn and finish their own thirty-tick lifetime.
    if (!Models || !orchestrator.Owner->BoneTransform ||
        orchestrator.Owner->Type < 0 || orchestrator.Owner->Type >= MAX_MODELS)
        return;
    BMD& ownerModel = Models[orchestrator.Owner->Type];
    if (ownerModel.NumBones <= 0)
        return;
    Vector(1.0f, 0.8f, 0.6f, light);
    for (int index = 0; index < 2; ++index)
    {
        vec3_t bonePosition;
        ownerModel.TransformByObjectBone(bonePosition, orchestrator.Owner,
            rand() % ownerModel.NumBones);
        CreateEffect(kDexBoosterFlareModel, bonePosition, orchestrator.Angle,
            light, 4, NULL, -1, 0, 0);
    }
}

void UpdateDexBooster(OBJECT& effect, float animationFactor)
{
    if (effect.Type == kDexBoosterAuraModel)
    {
        if (effect.Scale < 2.0f) effect.Scale += 0.1f * animationFactor;
        else if (effect.Scale < 2.4f) effect.Scale += 0.02f * animationFactor;
        if (effect.LifeTime <= 40.0f) effect.Alpha -= 0.1f * animationFactor;
        if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
        if (effect.Owner && effect.Owner->Live)
        {
            vec3_t relative;
            Vector(-10.0f, -30.0f, 0.0f, relative);
            VectorRotate(relative, effect.Owner->Matrix, effect.Position);
            VectorAdd(effect.Position, effect.Owner->Position, effect.Position);
            effect.Position[2] += 130.0f;
        }
    }
    else if (effect.Type == kDexBoosterGroundModel)
    {
        if (effect.SubType != 10 && effect.Scale > 0.0f)
        {
            effect.Scale -= 0.1f * animationFactor;
            if (effect.Scale < 0.1f) effect.Scale = 0.0f;
        }
        effect.Alpha -= kDexBoosterGroundAlphaStep * animationFactor;
        if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
        Vector(effect.Alpha, effect.Alpha * 0.5f, 0.0f, effect.Light);
        effect.Angle[2] += ((effect.SubType == 9) ? kDexBoosterGroundRotation9 :
            (effect.SubType == 10 ? kDexBoosterGroundRotation10 :
                kDexBoosterGroundRotation11)) * animationFactor;
    }
    else if (effect.Type == kDexBoosterFlareModel)
    {
        // Pegasus 0x66131C..0x6613A8: 0x27A1 is not itself a visible
        // sprite. 0x66139D calls7BB8A0, whose6FA6F2(1) probability gate
        // normalizes emission to min(1, animationFactor), not every render
        // update. Preserve that wrapper through native RISE's checked API.
        const float scale = static_cast<float>((rand() % 20) - 10) * 0.1f +
            kDexBoosterFlareParticleBaseScale;
        CreateParticleFpsChecked(kDexBoosterFlare01Bitmap, effect.Position,
            effect.Angle, effect.Light, kDexBoosterFlareParticleSubtype,
            scale);
    }
}

bool RenderDexBooster(OBJECT& effect)
{
    if (!EnsureDexBoosterBitmaps())
        return false;
    if (effect.Type == kDexBoosterOrchestratorModel)
        return true;
    if (effect.Type == kDexBoosterAuraModel)
    {
        float width = effect.Scale < 2.2f ? effect.Scale : 4.4f - effect.Scale;
        if (width < 0.0f) width = 0.0f;
        const float blend = effect.Alpha;
        vec3_t light;
        Vector(blend, blend * 0.92f, blend * 0.4f, light);
        CreateSprite(kDexBoosterImpactBitmap, effect.Position, width, light,
            effect.Owner, static_cast<float>(WorldTime) * -kDexBoosterAuraRotation);
        Vector(blend * 0.1f, blend * 0.94f, blend, light);
        CreateSprite(kDexBoosterShinyBitmap, effect.Position, width, light,
            effect.Owner, static_cast<float>(WorldTime) * -kDexBoosterAuraRotation);
        if (effect.Scale > 1.0f)
        {
            Vector(blend, blend * 0.3f, 0.0f, light);
            CreateSprite(kDexBoosterHikororaBitmap, effect.Position, width,
                light, effect.Owner,
                static_cast<float>(WorldTime) * -kDexBoosterAuraRotation);
            if (effect.Scale > 2.0f)
                CreateSprite(kDexBoosterHikororaBitmap, effect.Position, width,
                    light, effect.Owner,
                    static_cast<float>(WorldTime) * kDexBoosterAuraRotation);
        }
        return true;
    }
    if (effect.Type == kDexBoosterGroundModel)
    {
        // Pegasus 0x661781/0x6617F0 brackets the terrain bitmap with the
        // additive-alpha pair. Alpha-test leaves the black RGB background of
        // twlighthik01 visible as a solid quad in RISE 5.2.
        EnableAlphaBlend();
        RenderTerrainAlphaBitmap(kDexBoosterGroundBitmap, effect.Position[0],
            effect.Position[1], effect.Scale, effect.Scale, effect.Light,
            -effect.Angle[2], 1.0f, 5.0f);
        DisableAlphaBlend();
        return true;
    }
    if (effect.Type == kDexBoosterFlareModel)
    {
        // The source 0x27A1 route emits flare01 particles in Update and has
        // no direct model/sprite render pass.
        return true;
    }
    return false;
}

}}
