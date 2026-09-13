#include "stdafx.h"
#include "PegasusSpearstormRuntime.h"
#include "PegasusSpearstormMath.h"
#include "PegasusTextureIds.h"
#include "DSPlaySound.h"
#include "PegasusSoundRuntime.h"
#include "PegasusRuntimeTrace.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "GlobalBitmap.h"
#include "ZzzLodTerrain.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

#include <cmath>

namespace rise { namespace pegasus {
namespace {
bool EnsureSpearstormBitmaps()
{
    if (!Bitmaps.FindTexture(kSpearstormTrailBitmap) &&
        !Bitmaps.LoadImageFile(kSpearstormTrailBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\sharke_arrows.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE))
        return false;
    if (!Bitmaps.FindTexture(kSpearstormComboBitmap) &&
        !Bitmaps.LoadImageFile(kSpearstormComboBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\!Combo3.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE))
        return false;
    if (!Bitmaps.FindTexture(kDragonViolentSmokeBitmap) &&
        !Bitmaps.LoadImageFile(kDragonViolentSmokeBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\smoke01.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE))
        return false;
    return true;
}

bool CrossedIntegerTick(float lifeTime, float animationFactor)
{
    return static_cast<int>(ceilf(lifeTime)) !=
        static_cast<int>(ceilf(lifeTime - animationFactor));
}

const char* Filename(int modelId)
{
    return modelId == kSpearstormCrackModel
        ? "knight_plancrack_a.bmd" : "3cha_Kskill.bmd";
}

void CreateSpearstormBlade(OBJECT& root)
{
    // Pegasus 0x675C19..0x675D36: one blade at a random 100..639 radius.
    const float degrees = static_cast<float>(rand() % 360);
    const float radius = static_cast<float>((rand() % 540) + 100);
    const float radians = degrees * Q_PI / 180.0f;
    vec3_t position, angle, light;
    // COS at0x675C77, SIN at0x675C9D (verified helper identities).
    position[0] = root.Position[0] + cosf(radians) * radius;
    position[1] = root.Position[1] + sinf(radians) * radius;
    position[2] = root.Position[2];
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(0.2f, 0.2f, 0.2f, light);
    OBJECT* owner = root.Owner ? root.Owner : &root;
    CreateEffect(kSpearstormBladeModel, position, angle, light, 0, owner,
        -1, 0, 243, 0, 2.0f);
}

void CreateSpearstormCrack(OBJECT& blade)
{
    // First two impact particles at 0x6758E4..0x6759AA.
    vec3_t position, angle, light;
    VectorCopy(blade.Position, position);
    position[2] += 30.0f;
    Vector(1.0f, 1.0f, 0.7f, light);
    CreateParticle(BITMAP_EXPLOTION_MONO, position, blade.Angle, light,
        0, 1.2f, nullptr, -32768, true);
    Vector(0.2f, 0.35f, 1.0f, light);
    CreateParticle(kDragonViolentSmokeBitmap, blade.Position, blade.Angle,
        light, 0, 2.0f, nullptr);

    // Pegasus constructs 0x27CA at StartPosition.z-60+3, not at the raw
    // terrain contact Z. Preserve that deliberately low ground-plane origin.
    VectorCopy(blade.StartPosition, position);
    position[2] -= 57.0f;
    Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), angle);
    Vector(0.2f, 0.35f, 1.0f, light);
    CreateEffect(kSpearstormCrackModel, position, angle, light,
        0, blade.Owner, -1, 0, 243, 0,
        3.0f * kSpearstormModelScale);

    // Pegasus 0x675A30..0x675AA3: five native stone children, each with
    // random model 235/236 and random Z rotation. Native RISE names those
    // same semantic slots MODEL_STONE1/MODEL_STONE2.
    vec3_t stoneAngle;
    for (int child = 0; child < kSpearstormStoneChildCount; ++child)
    {
        Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), stoneAngle);
        CreateEffect(MODEL_STONE1 + (rand() % 2), blade.Position,
            stoneAngle, light, 10, blade.Owner, -1, 0, 78, 0, 0.0f);
    }

    // Pegasus 0x675AA5..0x675B17: fifteen smoke particles in the exact
    // 80x80 impact square, scale .8.
    for (int smoke = 0; smoke < kSpearstormImpactSmokeCount; ++smoke)
    {
        position[0] = blade.Position[0] + static_cast<float>(
            (rand() % (kSpearstormImpactScatter * 2)) - kSpearstormImpactScatter);
        position[1] = blade.Position[1] + static_cast<float>(
            (rand() % (kSpearstormImpactScatter * 2)) - kSpearstormImpactScatter);
        position[2] = blade.Position[2];
        CreateParticle(kDragonViolentSmokeBitmap, position, blade.Angle,
            light, 0, 0.8f, nullptr);
    }
}
}

bool EnsureSpearstormModel(int modelId)
{
    if (!EnsureSpearstormBitmaps())
        return false;
    if (!Models || modelId < kSpearstormFirstModel || modelId > kSpearstormLastModel)
        return false;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture) return true;
    char directory[] = "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    char filename[MAX_PATH];
    strcpy_s(filename, Filename(modelId));
    model.m_iBMDSeqID = modelId;
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
    OGL330MODEL::ConvertOldMeshToVaoMesh(modelId);
#endif
    return model.NumMeshs > 0;
}

void CreateSpearstormRoot(OBJECT& target, int skill)
{
    // Pegasus 0x6754B6 creates 0x27C8 on the animated caster with scale .9.
    vec3_t light;
    Vector(0.2f, 0.2f, 0.2f, light);
    CreateEffect(kSpearstormRootModel, target.Position, target.Angle, light,
        0, &target, -1, static_cast<WORD>(skill), 0, 0, 0.9f);
}

void InitializeSpearstorm(OBJECT& effect)
{
    switch (effect.Type)
    {
    case kSpearstormRootModel:
        // Pegasus 0x676023: invisible five-tick blade orchestrator.
        effect.LifeTime = kSpearstormRootLifetime;
        effect.Alpha = 0.0f;
        effect.BlendMesh = -1;
        break;
    case kSpearstormBladeModel:
    {
        // Pegasus 0x675E55..0x676020.
        effect.LifeTime = kSpearstormBladeLifetime;
        effect.Scale = 2.0f;
        effect.Alpha = 1.0f;
        effect.Velocity = 1.0f;
        effect.BlendMesh = -1;
        effect.BlendMeshLight = 0.7f;
        effect.Angle[0] = static_cast<float>((rand() % 11) + 60);
        effect.Angle[1] = static_cast<float>(rand() % 11);
        effect.Angle[2] = 0.0f;
        SpearstormBladeDirection(effect.Angle[0], effect.Angle[1],
            effect.Direction);
        VectorCopy(effect.Position, effect.StartPosition);
        effect.StartPosition[2] = max(effect.Position[2],
            RequestTerrainHeight(effect.Position[0], effect.Position[1])) + 60.0f;
        // Pegasus 0x675FB6..0x676013 starts the falling blade six full
        // direction steps behind the terrain contact point.
        for (int axis = 0; axis < 3; ++axis)
            effect.Position[axis] = effect.StartPosition[axis] -
                effect.Direction[axis] * kSpearstormSpawnLeadTicks;

        vec3_t trailPosition, trailLight;
        for (int axis = 0; axis < 3; ++axis)
            trailPosition[axis] = effect.StartPosition[axis] -
                effect.Direction[axis] * 3.9f;
        Vector(0.92f, 0.59f, 0.273f, trailLight);
        for (int trail = 0; trail < 2; ++trail)
            CreateJoint(kSpearstormTrailBitmap, trailPosition, trailPosition,
                effect.Angle, 0, nullptr, 180.0f, -1, 0, 0, -1,
                trailLight, -1);
        break;
    }
    case kSpearstormCrackModel:
        effect.LifeTime = kSpearstormCrackLifetime;
        effect.Alpha = 1.0f;
        effect.Velocity = 1.0f;
        effect.BlendMesh = -1;
        break;
    }
}

void UpdateSpearstorm(OBJECT& effect, float animationFactor)
{
    // Pegasus 0x676041..0x67623F never gates this cascade on Owner::Live.
    // The root helper deliberately falls back to the root when Owner is null,
    // and falling blade/crack objects complete their own authored lifetimes.
    switch (effect.Type)
    {
    case kSpearstormRootModel:
        if (CrossedIntegerTick(effect.LifeTime, animationFactor))
            CreateSpearstormBlade(effect);
        break;
    case kSpearstormBladeModel:
        if (effect.LifeTime > kSpearstormImpactLifetime)
        {
            for (int axis = 0; axis < 3; ++axis)
                effect.Position[axis] += effect.Direction[axis] * animationFactor;
            if (SpearstormShouldImpact(effect.LifeTime, animationFactor,
                effect.StartPosition[2], effect.Position[2]))
            {
                VectorCopy(effect.StartPosition, effect.Position);
                effect.LifeTime = kSpearstormImpactLifetime;
                CreateSpearstormCrack(effect);
                TraceRuntime("MOVEMENT_MILESTONE", kSpearstormSkill,
                    static_cast<int>(kSpearstormImpactLifetime), "FORCED_IMPACT");
            }
        }
        if (effect.LifeTime <= kSpearstormImpactLifetime)
        {
            // Pegasus 0x6761E2..0x676201 compares the current lifetime with
            // 64, then the next (current-dt) lifetime. UpdateEffect decrements
            // LifeTime after this handler, so the crossing must face forward.
            if (SpearstormCrossesLifetime(effect.LifeTime, animationFactor,
                kSpearstormHitSoundLifetime))
                PlayBuffer(kSpearstormHitSound, nullptr, false);
            effect.BlendMeshLight -= kSpearstormBladeFadeRate * animationFactor;
            if (effect.BlendMeshLight <= 0.0f)
            {
                effect.BlendMeshLight = 0.0f;
                effect.LifeTime = 0.0f;
            }
        }
        break;
    case kSpearstormCrackModel:
        effect.Alpha = max(0.0f, effect.LifeTime / kSpearstormCrackLifetime);
        // Pegasus 0x6760DF/0x675B28: while above 25 ticks, integer ticks
        // congruent to 1 mod 4 emit one expanding smoke particle.
        if (effect.LifeTime > 25.0f &&
            CrossedIntegerTick(effect.LifeTime, animationFactor) &&
            (static_cast<int>(effect.LifeTime) & 3) == 1)
        {
            const float radius = (kSpearstormCrackLifetime - effect.LifeTime) *
                120.0f / 25.0f;
            const float radians = static_cast<float>(rand() % 360) *
                Q_PI / 180.0f;
            vec3_t position, light;
            // COS at0x675B82, SIN at0x675BA5.
            position[0] = effect.Position[0] + cosf(radians) * radius;
            position[1] = effect.Position[1] + sinf(radians) * radius;
            position[2] = effect.Position[2];
            Vector(0.2f, 0.35f, 1.0f, light);
            CreateParticle(kDragonViolentSmokeBitmap, position, effect.Angle,
                light, 0, 2.0f, nullptr);
        }
        break;
    }
}

bool RenderSpearstorm(OBJECT& effect)
{
    if (effect.Type == kSpearstormRootModel)
        return true;
    if (effect.Type == kSpearstormBladeModel)
    {
        // Pegasus 0x675848 preserves the blade's own scale/material.
        // The crater override and normalized scale belong to crack0x675605.
        const bool calculated = EnsureSpearstormModel(effect.Type) &&
            Calc_RenderObject(&effect, false, 0, 0);
        if (!calculated)
            return false;
        BMD& model = Models[effect.Type];
        Vector(1.0f, 1.0f, 1.0f, model.BodyLight);
        const float alpha = SpearstormBladeAlpha(effect.BlendMeshLight);
#ifdef PEGASUS_ISOLATED_TEST
        static bool tracedPose[3] = {};
        const int phase = effect.LifeTime > kSpearstormImpactLifetime ? 0 :
            (alpha > 0.7f ? 1 : 2);
        if (!tracedPose[phase] && model.NumMeshs > 0)
        {
            tracedPose[phase] = true;
            float low = 1.0e9f, high = -1.0e9f;
            int count = 0, above = 0;
            const Mesh_t& mesh = model.Meshs[0];
            for (int v = 0; v < mesh.NumVertices; ++v)
            {
                const Vertex_t& vertex = mesh.Vertices[v];
                if (vertex.Node < 0 || vertex.Node >= model.NumBones) continue;
                vec3_t world;
                VectorTransform(vertex.Position, BoneTransform[vertex.Node], world);
                const float height = world[2] - RequestTerrainHeight(world[0], world[1]);
                low = min(low, height);
                high = max(high, height);
                ++count;
                if (height > 0.0f) ++above;
            }
            char detail[256];
            _snprintf_s(detail, sizeof(detail), _TRUNCATE,
                "life=%.2f alpha=%.3f pos=%.1f,%.1f,%.1f angle=%.1f,%.1f,%.1f zAboveTerrain=%.1f..%.1f above=%d/%d texture=%d",
                effect.LifeTime, alpha, effect.Position[0], effect.Position[1], effect.Position[2],
                effect.Angle[0], effect.Angle[1], effect.Angle[2], low, high, above, count,
                model.IndexTexture[mesh.Texture]);
            TraceRuntime("SPEARSTORM_BLADE_POSE", phase, effect.Type, detail);
        }
#endif
        model.RenderMesh(0, RENDER_TEXTURE, alpha, -1, 1.0f,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
            -1);
        return true;
    }
    if (effect.Type == kSpearstormCrackModel)
    {
        const float savedScale = effect.Scale;
        effect.Scale = 3.0f * kSpearstormModelScale;
        if (!EnsureSpearstormModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
        {
            effect.Scale = savedScale;
            return false;
        }
        BMD& model = Models[effect.Type];
        Vector(0.8f, 0.8f, 0.8f, model.BodyLight);
        model.RenderMesh(0, RENDER_TEXTURE, 1.0f, -1, 1.0f,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
            BITMAP_CRATER);

        struct Layer { int bitmap; float scale; float duration; bool reverse; vec3_t color; };
        Layer layers[2] = {
            { BITMAP_DAMAGE_01_MONO, 15.0f, 20.0f, true, {0.2f, 0.35f, 1.0f} },
            { kSpearstormComboBitmap, 6.0f, 10.0f, false, {0.8f, 0.8f, 0.8f} }
        };
        for (const Layer& layer : layers)
        {
            const float progress = SpearstormLayerProgress(effect.LifeTime,
                layer.duration);
            if (progress <= 0.0f || progress > 1.0f)
                continue;
            // 0x67577D loads 1; only the reverse layer subtracts progress.
            const float alpha = layer.reverse ? 1.0f - progress : 1.0f;
            effect.Scale = progress * layer.scale * kSpearstormModelScale;
            if (!Calc_RenderObject(&effect, false, 0, 0))
                continue;
            model.BodyLight[0] = layer.color[0] * alpha;
            model.BodyLight[1] = layer.color[1] * alpha;
            model.BodyLight[2] = layer.color[2] * alpha;
            model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
                alpha, -2, alpha, effect.BlendMeshTexCoordU,
                effect.BlendMeshTexCoordV, layer.bitmap);
        }
        effect.Scale = savedScale;
        return true;
    }
    if (!EnsureSpearstormModel(effect.Type) || !Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    VectorCopy(effect.Light, model.BodyLight);
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
        model.RenderMesh(mesh, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.BlendMeshLight, -1, effect.BlendMeshLight,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
    return true;
}

}}
