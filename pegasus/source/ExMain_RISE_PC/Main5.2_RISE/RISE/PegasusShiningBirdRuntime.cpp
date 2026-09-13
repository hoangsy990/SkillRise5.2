#include "stdafx.h"
#include "PegasusShiningBirdRuntime.h"
#include "PegasusShiningBirdMath.h"
#include "PegasusRuntimeTrace.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "GlobalBitmap.h"
#include "_struct.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

#include <cmath>

namespace rise { namespace pegasus {

namespace {
void EmitShiningBirdFrameParticles(OBJECT& effect, vec3_t* positions)
{
    // Pegasus 0x6726D7..0x67288F; the callback is gated by the model frame
    // marker in RenderShiningBird, exactly as at 0x6739C8..0x6739F4.
    if (effect.AnimationFrame < 2.4f)
        return;

    vec3_t angle, light;
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(kShiningBirdSmokeLight.r, kShiningBirdSmokeLight.g,
        kShiningBirdSmokeLight.b, light);
    CreateParticle(kShiningBirdSmokeBitmap,
        positions[rand() % kShiningBirdLightBoneCount], angle,
        light, 0, 2.0f);
    if ((rand() & 1) != 0)
        CreateParticle(kShiningBirdSmokeBitmap,
            positions[rand() % kShiningBirdLightBoneCount], angle,
            light, 0, 2.0f);

    if ((rand() % 3) != 0)
    {
        Vector(kShiningBirdPinStarLight.r, kShiningBirdPinStarLight.g,
            kShiningBirdPinStarLight.b, light);
        CreateParticle(kShiningBirdPinStarBitmap,
            positions[rand() % kShiningBirdLightBoneCount], angle,
            light, 0, 0.6f);
    }

    if ((rand() % 3) == 0)
    {
        const int bone = rand() % kShiningBirdLightBoneCount;
        Vector(0.0f, 0.0f, effect.Angle[2] + 180.0f +
            static_cast<float>((rand() % 31) - 15), angle);
        Vector(kShiningBirdWindLight.r, kShiningBirdWindLight.g,
            kShiningBirdWindLight.b, light);
        CreateJoint(kShiningBirdWindBitmap, positions[bone], positions[bone],
            angle, 3, nullptr, 15.0f, -1, 0, 0, -1, light, -1);
    }
}
}

bool EnsureShiningBirdModel()
{
    if (!Bitmaps.FindTexture(kShiningBirdWindBitmap) &&
        !Bitmaps.LoadImageFile(kShiningBirdWindBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\wind01.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE))
        return false;
    if (!Bitmaps.FindTexture(kShiningBirdSmokeBitmap) &&
        !Bitmaps.LoadImageFile(kShiningBirdSmokeBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\smoke01.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE))
        return false;
    if (!Bitmaps.FindTexture(kShiningBirdPinStarBitmap) &&
        !Bitmaps.LoadImageFile(kShiningBirdPinStarBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\pin_star03.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE))
        return false;
    if (!Bitmaps.FindTexture(kShiningBirdPillarBitmap) &&
        !Bitmaps.LoadImageFile(kShiningBirdPillarBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\force_Pillar2.jpg",
            GL_LINEAR, GL_REPEAT))
        return false;
    if (!Models)
        return false;
    BMD& model = Models[kShiningBirdModel];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture)
        return true;
    char directory[] = "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\";
    char filename[] = "lightofbird_attack.bmd";
    model.m_iBMDSeqID = kShiningBirdModel;
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
    OGL330MODEL::ConvertOldMeshToVaoMesh(kShiningBirdModel);
#endif
    return model.NumMeshs > 0;
}

void CreateShiningBird(OBJECT& owner, int skill)
{
    vec3_t angle, light;
    Vector(0.0f, 0.0f, owner.Angle[2], angle);
    Vector(kShiningBirdRootLight.r, kShiningBirdRootLight.g,
        kShiningBirdRootLight.b, light);
    CreateEffect(kShiningBirdModel, owner.Position, angle, light, 0,
        &owner, -1, static_cast<WORD>(skill), 0, 0, 1.3f);
}

void InitializeShiningBird(OBJECT& effect)
{
    // Pegasus 0x6735E8: the 0x27C1 root advances 250 units along its cast
    // heading before the three 0x7F3D force-pillar children are created.
    const float heading = effect.Angle[2] * 0.01745329238474369f;
    // 0x673659 calls SIN; 0x673684 calls COS (verified x87 fallbacks).
    effect.Position[0] += std::sin(heading) * kShiningBirdRootAdvance;
    effect.Position[1] -= std::cos(heading) * kShiningBirdRootAdvance;
    TraceRuntime("MOVEMENT_MILESTONE", kShiningBirdSkill,
        static_cast<int>(kShiningBirdRootAdvance), "FORWARD_OFFSET");
    effect.LifeTime = kShiningBirdRootLifetime;
    effect.Scale = kShiningBirdRootScale;
    effect.Alpha = 1.0f;
    effect.AlphaTarget = 0.0f;
    effect.BlendMesh = -1;
    effect.BlendMeshLight = 0.0f;
    effect.AnimationFrame = 0.0f;
    effect.CurrentAction = 0;
    effect.m_iAnimation = -1;

    if (!EnsureShiningBirdModel())
        return;

    vec3_t pillarAngle, pillarLight;
    Vector(90.0f, 0.0f, effect.Angle[2], pillarAngle);
    Vector(kShiningBirdPillarLight.r, kShiningBirdPillarLight.g,
        kShiningBirdPillarLight.b, pillarLight);
    // 0x673706/40/68 call the joint constructor (0x796B7A), not particles.
    CreateJoint(kShiningBirdPillarBitmap, effect.Position, effect.Position,
        pillarAngle, 0, &effect, 200.0f, -1, 0, 0, -1, pillarLight, -1);
    vec3_t sidePosition;
    VectorCopy(effect.Position, sidePosition);
    sidePosition[2] += 60.0f;
    CreateJoint(kShiningBirdPillarBitmap, sidePosition, sidePosition,
        pillarAngle, 1, &effect, 100.0f, -1, 0, 0, -1, pillarLight, -1);
    CreateJoint(kShiningBirdPillarBitmap, sidePosition, sidePosition,
        pillarAngle, 1, &effect, 100.0f, 1, 0, 0, -1, pillarLight, -1);
}

void UpdateShiningBird(OBJECT& effect, float animationFactor)
{
    // Pegasus 0x673781..0x673808 owns no Owner gate: once launched, the bird
    // and its three root-owned pillars complete the non-looping BMD action.
    // Caster death or viewport loss must not truncate the projectile visual.
    effect.BlendMeshLight += 0.1f * animationFactor;
    if (effect.BlendMeshLight > 1.0f)
        effect.BlendMeshLight = 1.0f;

    // Pegasus 0x6737B2..0x673803 advances the model through BMD::PlayAnimation
    // at speed .5 and marks the effect dead when its non-looping action ends.
    // The dynamic RISE model ID does not enter the native MODEL_BIRD01..
    // MODEL_SKILL_END animation block, so this must remain owned here.
    if (!EnsureShiningBirdModel())
    {
        effect.LifeTime = 0.0f;
        return;
    }
    BMD& model = Models[kShiningBirdModel];
    model.CurrentAction = effect.CurrentAction;
    const bool playing = model.PlayAnimation(&effect.AnimationFrame,
        &effect.PriorAnimationFrame, &effect.PriorAction,
        kShiningBirdAnimationSpeed, effect.Position, effect.Angle);
    TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,
        effect.AnimationFrame);
    if (!playing)
    {
        TraceRuntime("EFFECT_LIFECYCLE_DESTROY", effect.Type,
            effect.Type - MAX_MODELS, "BMD_COMPLETE");
        effect.Live = false;
    }
}

bool RenderShiningBird(OBJECT& effect)
{
    if (!EnsureShiningBirdModel() || !Calc_RenderObject(&effect, false, 0, 0))
        return false;
    // Shared Pegasus object draw redirects 0x27C1 at 0x8538B4 to the same
    // mesh-0 additive material as Dragon Violent (0x85385A).
    BMD& model = Models[kShiningBirdModel];
    VectorCopy(effect.Light, model.BodyLight);
    model.RenderMesh(0, RENDER_TEXTURE, effect.Alpha, 0, effect.Alpha,
        effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);

    vec3_t birdPositions[12];
    vec3_t local, light;
    Vector(kShiningBirdBoneLight.r, kShiningBirdBoneLight.g,
        kShiningBirdBoneLight.b, light);
    for (std::size_t boneIndex = 0; boneIndex < kShiningBirdLightBoneCount; ++boneIndex)
    {
        const int bone = kShiningBirdLightBones[boneIndex];
        if (bone < 0 || bone >= model.NumBones)
            return true;
        for (int lightIndex = 0; lightIndex < 4; ++lightIndex)
        {
            Vector(static_cast<float>(lightIndex) * 22.0f, 0.0f, 0.0f, local);
            model.TransformPosition(BoneTransform[bone], local,
                birdPositions[boneIndex], false);
            const float scale = (1.0f - static_cast<float>(lightIndex) * 0.25f) * 4.5f;
            CreateSprite(BITMAP_LIGHT, birdPositions[boneIndex], scale, light,
                &effect);
        }
    }

    const int frameMarker = static_cast<int>(effect.AnimationFrame * 1000.0f);
    if (frameMarker != effect.m_iAnimation)
    {
        EmitShiningBirdFrameParticles(effect, birdPositions);
        effect.m_iAnimation = frameMarker;
    }

    if (model.NumBones > 1)
    {
        vec3_t zero, anchor;
        Vector(0.0f, 0.0f, 0.0f, zero);
        // Calc_RenderObject(false) already builds translated bone matrices.
        // Pegasus 0x6738A7 and 0x67390C likewise pass Translate=false.
        model.TransformPosition(BoneTransform[1], zero, anchor, false);
        for (int i = 0; i < MAX_JOINTS; ++i)
        {
            JOINT& particle = Joints[i];
            if (!particle.Live || particle.Type != kShiningBirdPillarBitmap ||
                particle.Target != &effect)
                continue;
            if (particle.SubType == 0)
            {
                VectorCopy(anchor, particle.Position);
                continue;
            }
            const float angle = (static_cast<float>(particle.PKKey) * 90.0f +
                effect.Angle[2]) * 0.01745329238474369f;
            // 0x673A84/0x673AA6 use the same SIN/COS ordering.
            particle.Position[0] = anchor[0] + std::sin(angle) * 130.0f;
            particle.Position[1] = anchor[1] - std::cos(angle) * 130.0f;
            particle.Position[2] = anchor[2] + 60.0f;
        }
    }
    return true;
}

}}
