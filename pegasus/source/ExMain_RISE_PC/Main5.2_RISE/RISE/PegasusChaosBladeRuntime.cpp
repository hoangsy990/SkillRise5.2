#include "stdafx.h"
#include "PegasusChaosBladeRuntime.h"
#include "PegasusRuntimeTrace.h"
#include "DSPlaySound.h"
#include "PegasusSoundRuntime.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "GlobalBitmap.h"

namespace rise { namespace pegasus {
namespace {
const char* ModelDirectory(int modelId)
{
    return modelId == kChaosBladeCrackModel
        ? "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\"
        : "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\";
}

const char* ModelFile(int modelId)
{
    return modelId == kChaosBladeCrackModel
        ? "knight_plancrack_a.bmd" : "M_sword_effect.bmd";
}

struct ChaosBitmapRoute
{
    int id;
    const char* file;
};

const ChaosBitmapRoute kChaosBitmaps[] = {
    { kChaosBladeFlareBlueBitmap, "flareBlue.jpg" },
    { kChaosBladeMagicGroundBitmap, "Magic_Ground2.jpg" },
    { kChaosBladePinStarBitmap, "pin_star03.jpg" },
    { kChaosBladeSteamBitmap, "Steam_01.jpg" },
    { kChaosBladeSmokeBitmap, "smoke01.jpg" },
    { kChaosBladeShockwaveBitmap, "Shockwave2.jpg" },
    { kChaosBladeSmokeLineBitmap, "Smoke_line.jpg" },
    { kChaosBladeSmokeLine03Bitmap, "smoke_line03.jpg" },
    { kChaosBladeBlurLongBitmap, "blur02_long_magum.jpg" },
    { kChaosBladeBlurBlackBitmap, "blur02_mono_long_magum_black.jpg" },
};

bool EnsureChaosBladeBitmaps()
{
    for (const ChaosBitmapRoute& route : kChaosBitmaps)
    {
        if (!Bitmaps.FindTexture(route.id))
        {
            const std::string path =
                std::string("Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\") + route.file;
            if (!Bitmaps.LoadImageFile(route.id, path.c_str(), GL_LINEAR, GL_CLAMP_TO_EDGE))
                return false;
        }
    }
    return true;
}

void GetChaosBladeBone17(OBJECT& target, vec3_t position)
{
    VectorCopy(target.Position, position);
    if (!Models || target.Type < 0)
        return;
    BMD& model = Models[target.Type];
    if (model.NumBones <= 17)
        return;
    vec3_t relative;
    Vector(0.0f, 0.0f, 0.0f, relative);
    model.TransformByObjectBone(position, &target, 17, relative);
}

void CreateChaosBladeImpactLayers(OBJECT& target, int skill, vec3_t position)
{
    vec3_t light;
    Vector(1.0f, 0.3f, 0.4f, light);
    CreateParticle(kChaosBladeFlareBlueBitmap, position,
        target.Angle, light, 12, 3.0f);

    Vector(0.8f, 0.45f, 1.0f, light);
    CreateParticle(kChaosBladePinStarBitmap, position,
        target.Angle, light, 9, 2.0f, &target);

    Vector(0.2f, 0.2f, 0.2f, light);
    CreateEffect(kChaosBladeArcModel, position, target.Angle,
        light, 2, &target, -1, static_cast<WORD>(skill), 0, 0, 0.9f);

    Vector(0.5f, 0.35f, 0.85f, light);
    CreateParticle(kChaosBladeSteamBitmap, position,
        target.Angle, light, 1, 0.5f, &target);

    Vector(0.75f, 0.9f, 1.0f, light);
    CreateEffect(kChaosBladeCrackModel, position, target.Angle,
        light, 19, &target, -1, static_cast<WORD>(skill), 0, 0, 2.0f);

    // Pegasus 0x654858: the only proven impact call in the recovered Chaos
    // Blade slice is buffer 0x3DB (Hit 1), played once after all five layers.
    PlayBuffer(kChaosBladeHit1Sound, nullptr, false);
}

void EmitChaosBladeArcPulse(OBJECT& effect)
{
    vec3_t position;
    Vector(effect.Position[0] + static_cast<float>(rand() % 41 - 20),
        effect.Position[1] + static_cast<float>(rand() % 41 - 20),
        effect.Position[2] + static_cast<float>(rand() % 41 - 20), position);

    if (effect.LifeTime <= 5.0f)
    {
        vec3_t smokeLight;
        Vector(0.4f, 0.2f, 0.8f, smokeLight);
        for (int i = 0; i < 6; ++i)
        {
            const float scale = (rand() & 1) ? 2.75f : 1.75f;
            CreateParticle(kChaosBladeSmokeBitmap, position, effect.Angle,
                smokeLight, 19, scale, &effect);
        }
    }

    vec3_t waveLight;
    Vector(0.7f, 0.45f, 0.25f, waveLight);
    for (int i = 0; i < 2; ++i)
    {
        CreateParticle(kChaosBladeShockwaveBitmap, effect.Position,
            effect.Angle, waveLight, 2, 0.1f, &effect);
    }
}

void EmitChaosBladeCrackBurst(OBJECT& effect)
{
    for (int i = 0; i < 5; ++i)
    {
        const float randomAngle = static_cast<float>(rand() % 360 - 180);
        vec3_t angle;
        Vector(randomAngle, randomAngle, randomAngle, angle);
        // Pegasus 0x654BFD uses the native 0x7DE0 joint, subtype 19.
        CreateJoint(0x7DE0, effect.Position, effect.Position, angle, 19,
            &effect, effect.Scale, -1, 2012);
    }
}

void EmitChaosBladeRootFrame(OBJECT& effect, int frame)
{
    static const int kRadiusByFrame[10] = {
        0, 320, 355, 560, 560, 560, 560, 560, 560, 560
    };
    if (frame < 0 || frame >= 10 || !EnsureChaosBladeModel(effect.Type))
        return;

    BMD& model = Models[effect.Type];
    model.BodyScale = effect.Scale;
    VectorCopy(effect.Position, model.BodyOrigin);

    vec3_t smokeLineLight;
    vec3_t smokeLine03Light;
    Vector(0.4f, 0.2f, 0.6f, smokeLineLight);
    Vector(0.5f, 0.5f, 0.5f, smokeLine03Light);

    float animationFrame = static_cast<float>(frame) * 1.2f;
    model.CurrentAction = 0; // Pegasus 0x654F08
    TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS, animationFrame);
    for (int radial = 0; radial < 11; ++radial)
    {
        vec3_t position;
        VectorCopy(effect.Position, position);
        if (kRadiusByFrame[frame] > 0)
        {
            const float degrees = effect.HeadAngle[2] - 117.8f
                - static_cast<float>(frame * 11 + radial) * 4.79f
                + static_cast<float>(rand() % 91 - 45);
            const float radius = static_cast<float>(rand() % kRadiusByFrame[frame]);
            const float radians = degrees * Q_PI / 180.0f;
            // 0x654FCE calls COS into X; 0x654FF1 calls SIN into Y.
            position[0] += cosf(radians) * radius;
            position[1] += sinf(radians) * radius;
        }

        const float lineScale = static_cast<float>(rand() % 15) * 0.1f + 1.0f;
        CreateParticle(kChaosBladeSmokeLineBitmap, position, effect.Angle,
            smokeLineLight, 1, lineScale, &effect);
        const float line03Scale = static_cast<float>(rand() & 1) * 0.1f + 0.6f;
        CreateParticle(kChaosBladeSmokeLine03Bitmap, position, effect.Angle,
            smokeLine03Light, 0, line03Scale, &effect);
    if (animationFrame >= 9.8f || model.NumBones <= 11)
        continue;

    vec3_t zero;
    Vector(0.0f, 0.0f, 0.0f, zero);
        const float priorFrame = animationFrame - 1.0f;
        model.Animation(BoneTransform, animationFrame, priorFrame,
            effect.PriorAction, effect.Angle, effect.HeadAngle, false, false);
        model.Animation(BoneTransform, animationFrame, priorFrame,
            effect.PriorAction, effect.Angle, effect.HeadAngle, false, true);
        animationFrame += 0.1090909168f;
        if (animationFrame < 2.0f)
            continue;

        vec3_t start;
        vec3_t end;
        vec3_t ribbonLight;
        Vector(1.0f, 1.0f, 1.0f, ribbonLight);
        // Translated pose already contains BodyOrigin. Original TransformPosition
        // calls pass false, and 0x794152 appends to owner/subtype OBJECT_BLURs.
        model.TransformPosition(BoneTransform[7], zero, start, false);
        model.TransformPosition(BoneTransform[11], zero, end, false);
        CreateObjectBlur(&effect, start, end, ribbonLight,
            kChaosBladeBlurBlackBitmap, true, 0, 8);

        model.TransformPosition(BoneTransform[6], zero, start, false);
        model.TransformPosition(BoneTransform[10], zero, end, false);
        CreateObjectBlur(&effect, start, end, ribbonLight,
            kChaosBladeBlurLongBitmap, true, 1, 8);
        CreateObjectBlur(&effect, start, end, ribbonLight,
            kChaosBladeBlurLongBitmap, true, 2, 8);
    }
}
}

bool EnsureChaosBladeModel(int modelId)
{
    if (!EnsureChaosBladeBitmaps())
        return false;
    if (!Models || modelId < kChaosBladeFirstModel || modelId > kChaosBladeLastModel)
        return false;
    BMD& model = Models[modelId];
    const bool animationCarrier = modelId != kChaosBladeCrackModel;
    if ((animationCarrier && model.NumBones > 0 && model.NumActions > 0) ||
        (!animationCarrier && model.NumMeshs > 0 && model.Meshs && model.IndexTexture))
        return true;
    char directory[MAX_PATH];
    char filename[MAX_PATH];
    strcpy_s(directory, ModelDirectory(modelId));
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
    return animationCarrier
        ? model.NumBones > 0 && model.NumActions > 0
        : model.NumMeshs > 0;
}

void CreateChaosBladeRoot(OBJECT& target, int skill)
{
    // Pegasus 0x654DC3 creates only the caster-owned 0x27BE orchestrator.
    // The independent F3:35 route applies 0x654702 to each damage target.
    vec3_t position;
    GetChaosBladeBone17(target, position);
    vec3_t light;
    Vector(0.2f, 0.2f, 0.2f, light);
    CreateEffect(kChaosBladeRootModel, position, target.Angle, light,
        0, &target, -1, static_cast<WORD>(skill), 0, 0, 1.2f);
}

void CreateChaosBladeTargetImpact(OBJECT& target, int skill)
{
    // Pegasus 0x654BA3 validates each target in the bounded list, then calls
    // 0x654702 with its object and list index. The visible layers anchor to
    // bone 17; list index is not consumed by the recovered helper body.
    vec3_t position;
    GetChaosBladeBone17(target, position);
    CreateChaosBladeImpactLayers(target, skill, position);
}

void InitializeChaosBlade(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kChaosBladeRootModel:
        effect.LifeTime = 100.0f;
        effect.Velocity = 0.45f;
        effect.Timer = 0.0f;
        effect.Alpha = 1.0f;
        EmitChaosBladeRootFrame(effect, 0);
        break;
    case kChaosBladeArcModel:
        effect.LifeTime = 8.0f;
        effect.Scale = 0.9f;
        effect.Velocity = 0.3f;
        effect.Timer = -1.0f;
        effect.Alpha = 1.0f;
        break;
    case kChaosBladeCrackModel:
        effect.LifeTime = 5.0f;
        effect.Velocity = 1.0f;
        effect.Timer = 0.0f;
        effect.Alpha = 1.0f;
        EmitChaosBladeCrackBurst(effect);
        break;
    }
}

void UpdateChaosBlade(OBJECT& effect, float animationFactor)
{
    if (effect.Type == kChaosBladeRootModel)
    {
        // Pegasus 0x654481..0x65459E advances the cascade at the cast-time
        // bone-17 position. It never follows or lifetime-gates on Owner.
        // Target death therefore cannot truncate already-created arc/crack
        // children or drag the ground cascade with a moving actor.
        const int firstFrame = static_cast<int>(effect.Timer) + 1;
        effect.Timer += animationFactor;
        const int lastFrame = static_cast<int>(effect.Timer);
        for (int frame = firstFrame; frame <= lastFrame && frame < 10; ++frame)
            EmitChaosBladeRootFrame(effect, frame);
    }
    else if (effect.Type == kChaosBladeArcModel && effect.LifeTime <= 7.0f)
    {
        if (effect.Timer < 0.0f)
        {
            effect.Timer = 0.0f;
            EmitChaosBladeArcPulse(effect);
        }
        else
        {
            const int firstPulse = static_cast<int>(effect.Timer) + 1;
            effect.Timer += animationFactor;
            const int lastPulse = static_cast<int>(effect.Timer);
            for (int pulse = firstPulse; pulse <= lastPulse && pulse < 7; ++pulse)
                EmitChaosBladeArcPulse(effect);
        }
    }
    else if (effect.Type == kChaosBladeCrackModel)
    {
        const int firstBurst = static_cast<int>(effect.Timer) + 1;
        effect.Timer += animationFactor;
        const int lastBurst = static_cast<int>(effect.Timer);
        for (int burst = firstBurst; burst <= lastBurst && burst < 6; ++burst)
            EmitChaosBladeCrackBurst(effect);
    }
}

bool RenderChaosBlade(OBJECT& effect)
{
    // Pegasus 0x6545A1: root and arc are orchestration-only. Only the crack
    // model renders, using Magic_Ground2 as the exact texture override.
    if (effect.Type != kChaosBladeCrackModel)
        return true;
    if (!EnsureChaosBladeModel(effect.Type) || !Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    model.BodyLight[0] = effect.Light[0] * effect.Alpha;
    model.BodyLight[1] = effect.Light[1] * effect.Alpha;
    model.BodyLight[2] = effect.Light[2] * effect.Alpha;
    effect.BlendMesh = -2;
    effect.BlendMeshLight = effect.Alpha;
    model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha, -2,
        effect.Alpha, effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
        kChaosBladeMagicGroundBitmap);
    return true;
}

}}
