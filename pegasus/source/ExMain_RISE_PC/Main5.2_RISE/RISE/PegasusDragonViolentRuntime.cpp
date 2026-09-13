#include "stdafx.h"
#include "PegasusDragonViolentRuntime.h"
#include "PegasusDragonViolentMath.h"
#include "PegasusTextureIds.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "ZzzLodTerrain.h"
#include "ZzzOpenglUtil.h"
#include "GlobalBitmap.h"

namespace rise { namespace pegasus {
namespace {
struct DragonBitmapRoute
{
    int id;
    const char* path;
};

const DragonBitmapRoute kDragonBitmaps[] = {
    { kDragonViolentFlareBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\flare01.jpg" },
    { kDragonViolentLightningBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\lighting_mega03.jpg" },
    { kDragonViolentSmokeBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\smoke01.jpg" },
    { kDragonViolentImpactBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Impack03.jpg" },
    { kDragonViolentComboBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\Combo4.jpg" },
    { kDragonViolentTwilight1Bitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\twlighthik01.jpg" },
    { kDragonViolentTwilight2Bitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\twlighthik02.jpg" },
    { kDragonViolentRingBitmap, "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\ring_of_gradation.jpg" },
};

bool EnsureDragonBitmaps()
{
    for (const DragonBitmapRoute& route : kDragonBitmaps)
    {
        if (!Bitmaps.FindTexture(route.id) &&
            !Bitmaps.LoadImageFile(route.id, route.path, GL_LINEAR, GL_CLAMP_TO_EDGE))
            return false;
    }
    return true;
}

bool CrossedIntegerTick(float lifeTime, float animationFactor)
{
    return static_cast<int>(floorf(lifeTime)) !=
        static_cast<int>(floorf(lifeTime - animationFactor));
}

void CreateDragonHead(const vec3_t sourcePosition, OBJECT& owner)
{
    vec3_t position, angle, light;
    VectorCopy(sourcePosition, position);
    position[2] = RequestTerrainHeight(position[0], position[1]) +
        kDragonViolentHeadTerrainOffset;
    // 6639CE..6639F8: authored yaw draw precedes subtype draw.
    Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), angle);
    Vector(0.0785f, 0.314f, 0.92f, light);
    CreateEffect(kDragonViolentHeadModel, position, angle, light,
        (rand() % 2) + 1, &owner, -1, 242, 0, 0,
        kDragonViolentHeadScale);
}

void CreateDragonCrack(const vec3_t sourcePosition, OBJECT& owner)
{
    vec3_t position, angle, light;
    VectorCopy(sourcePosition, position);
    position[2] = RequestTerrainHeight(position[0], position[1]);
    Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), angle);
    Vector(0.0785f, 0.314f, 0.92f, light);
    CreateEffect(kDragonViolentCrackModel, position, angle, light,
        0, &owner, -1, 242, 0, 0, 0.9f);
}

void CreateDragonWind(OBJECT& crack)
{
    vec3_t angle, light;
    Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), angle);
    Vector(0.2355f, 0.942f, 2.76f, light);
    CreateEffect(kDragonViolentWindModel, crack.Position, angle, light,
        7, crack.Owner, -1, 242, 0, 0, 0.64f);
}

void CreateDragonLayers(OBJECT& crack)
{
    vec3_t angle, light;
    for (int subtype = 0; subtype < 3; ++subtype)
    {
        Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), angle);
        if (subtype == 1)
            Vector(0.0785f, 0.314f, 0.92f, light);
        else
            Vector(0.4f, 0.5f, 0.92f, light);
        CreateEffect(kDragonViolentLayerModel, crack.Position, angle, light,
            subtype, crack.Owner, -1, 242, 0, 0, 0.0f);
    }
}

const char* Directory(int modelId)
{
    return modelId == kDragonViolentHeadModel
        ? "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\"
        : "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
}
const char* Filename(int modelId)
{
    if (modelId == kDragonViolentHeadModel) return "dragonhead.bmd";
    if (modelId == kDragonViolentWindModel) return "wind_foce.bmd";
    return "knight_plancrack_a.bmd";
}
}

bool EnsureDragonViolentModel(int modelId)
{
    if (!EnsureDragonBitmaps())
        return false;
    if (!Models || modelId < kDragonViolentFirstModel || modelId > kDragonViolentLastModel)
        return false;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture) return true;
    char directory[MAX_PATH], filename[MAX_PATH];
    strcpy_s(directory, Directory(modelId));
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

void CreateDragonViolentRoots(OBJECT& target, int skill)
{
    // Pegasus 0x66279B/0x663475: generate eight positions. The inner four
    // immediately receive head/crack pairs; the outer four receive delayed
    // 0x27C2 orchestrators with subtype delays 0..3.
    vec3_t positions[8];
    for (int index = 0; index < 8; ++index)
    {
        const int quadrant = index < 4 ? index : index - 4;
        const float degrees = static_cast<float>((rand() % 90) + quadrant * 90);
        const float radius = index < 4
            ? static_cast<float>((rand() % 251) + 100)
            : static_cast<float>((rand() % 201) + 150);
        const float radians = degrees * Q_PI / 180.0f;
        // 0x662802 COS supplies X; 0x662834 SIN supplies Y.
        positions[index][0] = target.Position[0] + cosf(radians) * radius;
        positions[index][1] = target.Position[1] + sinf(radians) * radius;
        positions[index][2] = target.Position[2];
    }
    for (int index = 0; index < 4; ++index)
    {
        CreateDragonHead(positions[index], target);
        CreateDragonCrack(positions[index], target);
    }
    vec3_t angle, light;
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(0.0785f, 0.314f, 0.92f, light);
    for (int index = 0; index < 4; ++index)
    {
        CreateEffect(kDragonViolentBaseModel, positions[index + 4], angle,
            light, index, &target, -1, static_cast<WORD>(skill), 0, 0, 0.0f);
    }
}

void CreateDragonViolentTargetImpact(OBJECT& target, int skill)
{
    // Pegasus 0x663C14: target-owned 0x27C7 at +30 and the immediate 0x7F45
    // combo particle at +100. This function is deliberately separate from
    // the radial root cascade because the recovered multi-target list invokes
    // it once per validated target.
    vec3_t position, angle, light;
    VectorCopy(target.Position, position);
    position[2] += 30.0f;
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(0.0785f, 0.314f, 0.92f, light);
    CreateEffect(kDragonViolentEndModel, position, angle, light,
        0, &target, -1, static_cast<WORD>(skill), 0, 0, 0.0f);

    position[2] = target.Position[2] + 100.0f;
    Vector(0.96f, 0.82f, 0.43f, light);
    CreateParticle(kDragonViolentComboBitmap, position, angle, light,
        0, 1.0f, &target);
}

void InitializeDragonViolent(OBJECT& effect)
{
    effect.BlendMesh = -1;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kDragonViolentBaseModel:
        effect.LifeTime = 30.0f;
        effect.m_iAnimation = 0;
        effect.Timer = 0.0f;
        effect.Alpha = 0.0f;
        break;
    case kDragonViolentHeadModel:
    {
        effect.LifeTime = kDragonViolentHeadLifetime;
        effect.Scale = kDragonViolentHeadScale;
        // Pegasus 0x662991 starts hidden. JointSpirit02 subtype 18 owns the
        // reveal: 0x7AA466..0x7AA49D sets target Alpha to 1 when its lifetime
        // exceeds 5, then copies the joint position into the head. Native
        // RISE's corresponding joint branch already performs both writes.
        effect.Alpha = 0.0f;

        // Pegasus 0x662966..0x6629E1 creates JointSpirit02 subtype 18 at
        // head Z-140 before applying the head pitch/random roll.
        vec3_t jointPosition, jointAngle, jointLight;
        VectorCopy(effect.Position, jointPosition);
        jointPosition[2] += kDragonViolentHeadJointZOffset;
        Vector(0.0f, 0.0f, 0.0f, jointAngle);
        Vector(0.0785f, 0.314f, 0.92f, jointLight);
        CreateJoint(BITMAP_JOINT_SPIRIT2, jointPosition, jointPosition,
            jointAngle, kDragonViolentHeadJointSubtype, &effect,
            kDragonViolentHeadJointScale, -1, 0, 0, -1, jointLight, -1);
        effect.Angle[0] = -30.0f;
        effect.HeadAngle[2] = static_cast<float>(rand() % 360);
        break;
    }
    case kDragonViolentCrackModel:
        effect.LifeTime = 15.0f;
        effect.Scale = effect.SubType == 1 ? 0.9f : 0.0f;
        effect.Alpha = 0.0f;
        effect.CurrentAction = 0;
        effect.AnimationFrame = 0.0f;
        effect.PriorAnimationFrame = 0.0f;
        break;
    case kDragonViolentWindModel:
        effect.LifeTime = 15.0f;
        effect.Scale = 0.64f;
        effect.Alpha = 0.9f;
        break;
    case kDragonViolentLayerModel:
        effect.LifeTime = 15.0f;
        effect.Alpha = 1.0f;
        effect.Position[2] = RequestTerrainHeight(effect.Position[0], effect.Position[1]) + 10.0f;
        break;
    case kDragonViolentEndModel:
        effect.LifeTime = 12.0f;
        effect.Alpha = 0.0f;
        break;
    }
}

void UpdateDragonViolent(OBJECT& effect, float animationFactor)
{
    switch (effect.Type)
    {
    case kDragonViolentBaseModel:
        effect.Timer += animationFactor;
        effect.m_iAnimation = static_cast<int>(effect.Timer);
        if (effect.m_iAnimation >= effect.SubType + 6)
        {
            // Pegasus 0x662FC0 consumes the stored owner pointer for the
            // delayed spawn but does not reject an owner merely because the
            // actor's Live flag changed. Guard only an impossible null slot.
            if (!effect.Owner)
            {
                effect.LifeTime = 0.0f;
                break;
            }
            CreateDragonHead(effect.Position, *effect.Owner);
            CreateDragonCrack(effect.Position, *effect.Owner);
            effect.LifeTime = 0.0f;
        }
        break;
    case kDragonViolentHeadModel:
        // Pegasus 0x662E8F does not follow the actor owner. The attached
        // subtype-18 joint moves this head; see InitializeDragonViolent.
        if (CrossedIntegerTick(effect.LifeTime, animationFactor))
        {
            effect.Angle[2] += static_cast<float>((rand() % 11) + 10);
            if (effect.SubType == 1)
            {
                vec3_t light;
                Vector(0.16f, 0.26f, 0.35f, light);
                CreateParticle(kDragonViolentFlareBitmap, effect.Position,
                    effect.Angle, light, 23, 3.0f, &effect);
            }
            if ((rand() & 1) == 0)
            {
                const float radians = static_cast<float>(rand() % 360) * Q_PI / 180.0f;
                const float radius = static_cast<float>(rand() % 121);
                vec3_t position, light;
                // 0x662F3B COS, 0x662F5D SIN; preserve source draw order.
                position[0] = effect.Position[0] + cosf(radians) * radius;
                position[1] = effect.Position[1] + sinf(radians) * radius;
                position[2] = effect.Position[2] + static_cast<float>(rand() % 101 - 50);
                Vector(0.5f, 0.6f, 1.0f, light);
                CreateParticle(kDragonViolentLightningBitmap, position,
                    effect.Angle, light, 0, 1.0f, &effect);
            }
        }
        break;
    case kDragonViolentCrackModel:
        effect.Angle[2] += 10.0f * animationFactor;
        effect.Alpha = effect.LifeTime >= 7.5f
            ? 1.0f : effect.LifeTime / 7.5f;
        if ((effect.LifeTime >= 15.0f && effect.LifeTime - animationFactor < 15.0f) ||
            (effect.LifeTime >= 10.0f && effect.LifeTime - animationFactor < 10.0f))
            CreateDragonWind(effect);
        if (effect.LifeTime >= 12.0f && effect.LifeTime - animationFactor < 12.0f)
            CreateDragonLayers(effect);
        break;
    case kDragonViolentWindModel:
        effect.Angle[2] += 10.0f * animationFactor;
        effect.Alpha = effect.LifeTime / 15.0f * 0.9f;
        break;
    case kDragonViolentLayerModel:
        effect.Angle[2] += 10.0f * animationFactor;
        if (effect.SubType == 0 && effect.Scale < 3.0f)
            effect.Scale += 0.2f * animationFactor;
        else if (effect.SubType == 1)
            effect.Scale += 0.36f * animationFactor;
        effect.Alpha = effect.LifeTime >= 7.5f
            ? (15.0f - effect.LifeTime) / 7.5f : effect.LifeTime / 7.5f;
        break;
    case kDragonViolentEndModel:
        // Only target-impact 0x27C7 has the explicit owner/live termination
        // at 0x662AFB..0x662B0C. Radial head/crack/wind/layer objects above
        // complete their authored lifetimes after target death.
        if (!effect.Owner || !effect.Owner->Live)
        {
            effect.LifeTime = 0.0f;
            break;
        }
        VectorCopy(effect.Owner->Position, effect.Position);
        effect.Position[2] += 30.0f;
        if (CrossedIntegerTick(effect.LifeTime, animationFactor))
        {
            // Pegasus 0x662B4A..0x662C9B: two side groups, each with a
            // one-in-three chance of emitting two impact particles.
            vec3_t position, angle, light;
            Vector(0.0f, 0.0f, 0.0f, angle);
            Vector(0.96f, 0.82f, 0.43f, light);
            const float rise = min(150.0f,
                (12.0f - static_cast<float>(static_cast<int>(effect.LifeTime))) * 50.0f);
            for (int side = 0; side < 2; ++side)
            {
                if ((rand() % 3) != 0)
                    continue;
                for (int particle = 0; particle < 2; ++particle)
                {
                    position[0] = effect.Owner->Position[0] +
                        (side ? 60.0f : -20.0f) + static_cast<float>(rand() % 71) * 0.1f;
                    position[1] = effect.Owner->Position[1] +
                        (side ? 20.0f : -60.0f) - static_cast<float>(rand() % 71) * 0.1f;
                    position[2] = effect.Owner->Position[2] + rise +
                        static_cast<float>((rand() % 141) - 70) * 0.1f;
                    CreateParticle(kDragonViolentImpactBitmap, position,
                        angle, light, 0, 1.0f, nullptr);
                }
            }

            // Pegasus 0x662CA1..0x662D51: occasional smoke point in a
            // 65-unit cube around the target.
            if ((rand() % 8) == 1)
            {
                position[0] = effect.Owner->Position[0] + static_cast<float>((rand() % 65) - 32);
                position[1] = effect.Owner->Position[1] + static_cast<float>((rand() % 65) - 32);
                position[2] = effect.Owner->Position[2] + static_cast<float>((rand() % 65) - 32);
                Vector(1.0f, 1.0f, 1.0f, light);
                CreateParticle(kDragonViolentSmokeBitmap, position,
                    angle, light, 0, 1.0f, nullptr);
            }
        }
        break;
    }
}

bool RenderDragonViolent(OBJECT& effect)
{
    // 0x27C2 and 0x27C7 are orchestration-only in the recovered renderer
    // dispatch and must not draw the shared fallback BMD.
    if (effect.Type == kDragonViolentBaseModel ||
        effect.Type == kDragonViolentEndModel)
        return true;
    if (!EnsureDragonBitmaps())
        return false;
    if (effect.Type == kDragonViolentHeadModel)
    {
        // Pegasus 0x66377C enters the shared object renderer, whose special
        // 0x27C3 branch at 0x853853..0x8538AF copies object Light and draws
        // mesh 0 with RENDER_TEXTURE, blend mesh 0, Alpha for both opacity
        // and blend light, UV from the object and no texture override.
        // Native RISE has no branch for this imported model ID; express the
        // recovered pass here so it does not fall into the opaque default.
        if (!EnsureDragonViolentModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        VectorCopy(effect.Light, model.BodyLight);
        model.RenderMesh(0, RENDER_TEXTURE, effect.Alpha, 0, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        // The following 0x66304A helper attaches the two bone flares.
        if (model.NumBones > 4)
        {
            vec3_t local, position, light;
            Vector(8.0f, 0.0f, 0.0f, local);
            Vector(effect.Alpha, effect.Alpha * 0.12f,
                effect.Alpha * 0.08f, light);
            for (int bone = 3; bone <= 4; ++bone)
            {
                model.TransformPosition(BoneTransform[bone], local,
                    position, false); // 0x6630AF/0x663119: pose is already translated
                CreateSprite(kDragonViolentFlareBitmap, position, 0.6f,
                    light, &effect, 0.0f);
            }
        }
        return true;
    }
    if (effect.Type == kDragonViolentCrackModel)
    {
        // Pegasus 0x663880: the crack object itself does not render its BMD;
        // it draws these exact two terrain layers.
        vec3_t light;
        EnableAlphaBlend();
        Vector(0.0785f * effect.Alpha, 0.314f * effect.Alpha,
            0.92f * effect.Alpha, light);
        RenderTerrainAlphaBitmap(kDragonViolentRingBitmap, effect.Position[0],
            effect.Position[1], 1.16f, 1.16f, light, effect.Angle[2],
            effect.Alpha, 5.0f);
        Vector(0.7f * effect.Alpha, 0.7f * effect.Alpha,
            effect.Alpha, light);
        RenderTerrainAlphaBitmap(kDragonViolentFlareBitmap, effect.Position[0],
            effect.Position[1], 4.0f, 4.0f, light, effect.Angle[2],
            effect.Alpha, 5.0f);
        DisableAlphaBlend();
        return true;
    }
    if (effect.Type == kDragonViolentWindModel)
    {
        // Pegasus 0x6637CA: wind_foce mesh 0 only, with BodyLight amplified
        // by the recovered factor 6 before its textured pass.
        if (!EnsureDragonViolentModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        model.BodyLight[0] = effect.Light[0] * 6.0f;
        model.BodyLight[1] = effect.Light[1] * 6.0f;
        model.BodyLight[2] = effect.Light[2] * 6.0f;
        // Original 0x663871 pushes flags 2, not 0x42. Keep blend mesh 0
        // and alpha unchanged; do not add a renderer flag to tune colour.
        model.RenderMesh(0, RENDER_TEXTURE,
            effect.Alpha, 0, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        return true;
    }
    if (effect.Type == kDragonViolentLayerModel)
    {
        // Pegasus 0x663634: subtype 2 uses twilight-1 and fixed scale 1.2;
        // the other subtypes use twilight-2 and the growing object scale.
        // All pass through the recovered model normalization factor.
        const float savedScale = effect.Scale;
        effect.Scale = (effect.SubType == 2 ? 1.2f : effect.Scale) *
            0.5681818128f;
        const bool calculated = EnsureDragonViolentModel(effect.Type) &&
            Calc_RenderObject(&effect, false, 0, 0);
        effect.Scale = savedScale;
        if (!calculated)
            return false;
        BMD& model = Models[effect.Type];
        const float red = effect.SubType == 1 ? 0.0785f : 0.4f;
        const float green = effect.SubType == 1 ? 0.314f : 0.5f;
        model.BodyLight[0] = red * effect.Alpha;
        model.BodyLight[1] = green * effect.Alpha;
        model.BodyLight[2] = 0.92f * effect.Alpha;
        const int bitmap = effect.SubType == 2
            ? kDragonViolentTwilight1Bitmap
            : kDragonViolentTwilight2Bitmap;
        // Pegasus 0x663762: flags2; blend mesh0 and alpha are separate.
        model.RenderMesh(0, RENDER_TEXTURE,
            effect.Alpha, 0, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, bitmap);
        return true;
    }
    if (!EnsureDragonViolentModel(effect.Type) || !Calc_RenderObject(&effect, false, 0, 0))
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
