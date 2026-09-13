#include "stdafx.h"
#include "PegasusRainingArrowRuntime.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzAI.h"
#include "ZzzLodTerrain.h"
#include "DSplaysound.h"
#include "GlobalBitmap.h"
#include "PegasusSoundRuntime.h"
#include "PegasusRainingArrowMath.h"
#include "PegasusRuntimeTrace.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

namespace rise { namespace pegasus {
namespace {
#if defined(PEGASUS_ISOLATED_TEST)
std::map<const OBJECT*, int> g_rainingArrowLaunchPhase;
void TraceRainingArrowTermination(const OBJECT& effect, const char* reason)
{
    const auto found = g_rainingArrowLaunchPhase.find(&effect);
    const int remaining = found == g_rainingArrowLaunchPhase.end() ? -1 : found->second;
    TraceRuntime("RAINING_ARROW_TERMINATION", remaining, effect.Type, reason);
    if (found != g_rainingArrowLaunchPhase.end())
        g_rainingArrowLaunchPhase.erase(found);
}
#endif
struct GroundLayer
{
    int textureSlot;
    float scale;
    float scaleStep;
    float alphaStep;
    float red;
    float green;
    float blue;
};

enum RainingArrowTextureSlot
{
    kTextureEmpact,
    kTextureEnemyRing,
    kTextureLightmarks,
    kTextureShockwave,
    kTextureWave,
    kTextureCratered,
    kTexturePinStar,
    kTextureSpark,
    kTextureBowStar,
    kTextureDamageMono,
    kTextureCount
};

struct RainingArrowTexture
{
    const char* filename;
    int bitmap;
};

RainingArrowTexture g_rainingArrowTextures[kTextureCount] = {
    {"empact01.jpg", BITMAP_UNKNOWN},
    {"enemy_ring02.jpg", BITMAP_UNKNOWN},
    {"lightmarks.jpg", BITMAP_UNKNOWN},
    {"Shockwave2.jpg", BITMAP_UNKNOWN},
    {"wave.jpg", BITMAP_UNKNOWN},
    {"Cratered.tga", BITMAP_UNKNOWN},
    {"pin_star03.jpg", BITMAP_UNKNOWN},
    {"Spark04.jpg", BITMAP_UNKNOWN},
    {"bostar3_R.jpg", BITMAP_UNKNOWN},
    {"damage01mono.jpg", BITMAP_UNKNOWN}
};

// Exact Pegasus 0xB6DB30 table, indexed by the 0x2795 subtype. Raw Pegasus
// bitmap IDs are represented by isolated dynamic slots instead of copied.
const GroundLayer kGroundLayers[6] = {
    {kTextureEmpact,    2.0f, 0.0f,  0.0f, 0.70f, 0.50f, 0.40f},
    {kTextureEnemyRing, 3.0f, 0.0f,  0.0f, 0.70f, 0.50f, 0.40f},
    {kTextureLightmarks,4.0f, 0.0f,  0.0f, 0.75f, 0.75f, 0.80f},
    {kTextureShockwave, 0.5f, 0.5f,  0.1f, 0.70f, 0.50f, 0.40f},
    {kTextureWave,      2.0f, 0.2f, 0.15f, 0.70f, 0.50f, 0.40f},
    {kTextureEmpact,    2.0f,0.15f,  0.2f, 0.70f, 0.50f, 0.40f}
};
const float kGroundScale = 0.5681818128f; // Pegasus 0xB6B1C0
int g_rainingArrowDamageBitmap = BITMAP_UNKNOWN;

float RainingArrowRandom(float low, float high)
{
    // Pegasus helper 0x666E16 uses rand()%1001 and a 0.001 multiplier.
    return low + (high - low) * static_cast<float>(rand() % 1001) * 0.001f;
}

bool EnsureRainingArrowDamageBitmap()
{
    if (g_rainingArrowDamageBitmap == BITMAP_UNKNOWN)
        g_rainingArrowDamageBitmap = Bitmaps.LoadImageFile(
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\arrow_damge.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    return g_rainingArrowDamageBitmap != BITMAP_UNKNOWN;
}

bool EnsureRainingArrowRendererBitmaps()
{
    static const char* directory =
        "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
    bool ready = true;
    for (int slot = 0; slot < kTextureCount; ++slot)
    {
        RainingArrowTexture& texture = g_rainingArrowTextures[slot];
        if (texture.bitmap == BITMAP_UNKNOWN)
            texture.bitmap = Bitmaps.LoadImageFile(
                std::string(directory) + texture.filename,
                GL_NEAREST, GL_CLAMP_TO_EDGE);
        ready = ready && texture.bitmap != BITMAP_UNKNOWN;
    }
    return ready;
}

int RainingArrowTextureId(int slot)
{
    return slot >= 0 && slot < kTextureCount
        ? g_rainingArrowTextures[slot].bitmap : BITMAP_UNKNOWN;
}

const char* Filename(int modelId)
{
    if (modelId <= kRainingArrowGroundModel) return "knight_plancrack_a.bmd";
    static const char* names[] = {
        "min_elf_skill_ring01.bmd", "min_elf_skill_ring02.bmd",
        "min_elf_skill_ring03.bmd", "min_elf_skill_ring04.bmd",
        "min_elf_skill_ring05.bmd", "min_elf_skill_ring06.bmd",
        "min_elf_skill_ring07.bmd", "min_elf_skill_ring08.bmd"
    };
    return names[modelId - kRainingArrowRing01Model];
}

void CreateGroundLayer(OBJECT& root, int subtype)
{
    if (subtype < 0 || subtype >= 6) return;
    vec3_t position, angle, light;
    VectorCopy(root.Position, position);
    position[2] = RequestTerrainHeight(position[0], position[1]) + 3.0f;
    Vector(0.0f, 0.0f, 0.0f, angle);
    const GroundLayer& layer = kGroundLayers[subtype];
    Vector(layer.red, layer.green, layer.blue, light);
    CreateEffect(kRainingArrowGroundModel, position, angle, light, subtype,
        root.Owner, -1, 0, 0, 0, layer.scale * kGroundScale);
}

CHARACTER* CasterFor(const OBJECT& root)
{
    const int index = root.m_sTargetIndex;
    if (!CharactersClient || index < 0 || index >= MAX_CHARACTERS_CLIENT)
        return NULL;
    CHARACTER* caster = &CharactersClient[index];
    return caster->Object.Live && root.Owner == &caster->Object ? caster : NULL;
}

void CreateArrowAt(OBJECT& root, CHARACTER& caster, const vec3_t launch,
    const vec3_t angle, int remaining)
{
    (void)remaining;
    int freeIndex = -1;
    for (int index = 0; index < MAX_EFFECTS; ++index)
    {
        if (!Effects[index].Live)
        {
            freeIndex = index;
            break;
        }
    }

    vec3_t savedPosition, savedAngle, localOffset, worldOffset;
    float matrix[3][4];
    VectorCopy(root.Position, savedPosition);
    VectorCopy(root.Angle, savedAngle);
    Vector(-10.0f, -60.0f, 135.0f, localOffset);
    AngleMatrix(angle, matrix);
    VectorRotate(localOffset, matrix, worldOffset);
    VectorSubtract(launch, worldOffset, root.Position);
    VectorCopy(angle, root.Angle);

    // Pegasus 0x671989 calls the native bow-aware CreateArrow entry with
    // SkillIndex/SKKey 2023, then restores the orchestrator transform.
    CreateArrow(&caster, &root, NULL, kRainingArrowSkill, 1, kRainingArrowSkill);
#if defined(PEGASUS_ISOLATED_TEST)
    // /qa15 deliberately exercises every class from one GM character. The
    // native bow-aware helper creates no projectile when that character does
    // not currently wear a bow, which made the impact chain impossible to QA.
    // Keep the real helper first and supply a plain native arrow only when it
    // produced nothing. Production builds retain the equipment contract.
    if (freeIndex >= 0 && !Effects[freeIndex].Live)
    {
        vec3_t fallbackAngle;
        VectorCopy(angle, fallbackAngle);
        CreateEffect(MODEL_ARROW, root.Position, fallbackAngle, root.Light, 0, &root,
            root.PKKey, kRainingArrowSkill, 1);
        TraceRuntime("QA_PROJECTILE_FALLBACK", kRainingArrowSkill,
            MODEL_ARROW, "NO_BOW");
    }
#endif
    VectorCopy(savedPosition, root.Position);
    VectorCopy(savedAngle, root.Angle);

    if (freeIndex >= 0 && Effects[freeIndex].Live &&
        Effects[freeIndex].Skill == kRainingArrowSkill)
    {
        VectorCopy(launch, Effects[freeIndex].Position);
        VectorCopy(angle, Effects[freeIndex].Angle);
#if defined(PEGASUS_ISOLATED_TEST)
        g_rainingArrowLaunchPhase[&Effects[freeIndex]] = remaining;
        TraceRuntime("RAINING_ARROW_LAUNCH_PHASE", remaining, Effects[freeIndex].Type,
            remaining >= 16 ? "ASCENDING" : "FALLING_VOLLEY");
#endif
    }
}

void CreateRainingArrowVolley(OBJECT& root, int remaining)
{
    CHARACTER* caster = CasterFor(root);
    if (!caster) return;

    vec3_t launch, target, angle;
    if (remaining >= 16)
    {
        VectorCopy(root.Position, launch);
        launch[2] = RequestTerrainHeight(launch[0], launch[1]) + 200.0f;
        // Exact Pegasus stack vector at 0x6720C6..0x6722C0.  The prior +90
        // compatibility conversion inverted this axis by 180 degrees.
        Vector(kRainingArrowFirstPitch,
            RainingArrowRandom(kRainingArrowFirstBankMin,
                kRainingArrowFirstBankMax),
            kRainingArrowFirstYaw, angle);
    }
    else
    {
        // Exact Pegasus random helper 0x666E16, called twice with -200/+200
        // at 0x67212B..0x67215B.
        const float offsetX = RainingArrowRandom(
            kRainingArrowVolleyOffsetMin, kRainingArrowVolleyOffsetMax);
        const float offsetY = RainingArrowRandom(
            kRainingArrowVolleyOffsetMin, kRainingArrowVolleyOffsetMax);
        launch[0] = root.Position[0] + offsetX;
        launch[1] = root.Position[1] + offsetY;
        launch[2] = RequestTerrainHeight(launch[0], launch[1]) +
            kRainingArrowVolleyHeight;
        target[0] = root.Position[0] + offsetX * 2.0f;
        target[1] = root.Position[1] + offsetY * 2.0f;
        // Pegasus carries the root Z into this target vector; it does not
        // perform a second terrain-height query at the doubled XY target.
        target[2] = root.Position[2];
        const float dx = target[0] - launch[0];
        const float dy = target[1] - launch[1];
        const float horizontal = sqrtf(dx * dx + dy * dy);
        angle[0] = RainingArrowVolleyPitch(launch[2], target[2], horizontal);
        angle[1] = 0.0f;
        angle[2] = CreateAngle(launch[0], launch[1], target[0], target[1]);
    }
    CreateArrowAt(root, *caster, launch, angle, remaining);
}

void UpdateRainingArrowProjectileJoints(OBJECT& projectile, bool destroy)
{
    // Pegasus 0x66FC64 and 0x670B1F bind native arrow joints through their
    // Target pointer. Keep their angle synchronized, then retire them at the
    // same terrain/height termination point as the projectile.
    for (int index = 0; index < MAX_JOINTS; ++index)
    {
        JOINT& joint = Joints[index];
        if (!joint.Live || joint.Target != &projectile)
            continue;
        if (destroy)
            joint.Live = false;
        else
            VectorCopy(projectile.Angle, joint.Angle);
    }
}

void CreateImpactLayers(OBJECT& impact)
{
    vec3_t light;
    Vector(0.75f, 0.75f, 1.0f, light);
    CreateEffect(kRainingArrowRing05Model, impact.Position, impact.Angle,
        light, 0, impact.Owner, -1, 0, 0, 0, 0.5f);
    Vector(0.80f, 0.40f, 1.0f, light);
    CreateEffect(kRainingArrowRing04Model, impact.Position, impact.Angle,
        light, 0, impact.Owner, -1, 0, 0, 0, 0.25f);
    Vector(0.70f, 0.70f, 1.0f, light);
    CreateEffect(kRainingArrowRing06Model, impact.Position, impact.Angle,
        light, 0, impact.Owner, -1, 0, 0, 0, 0.25f);
    Vector(0.20f, 0.90f, 0.90f, light);
    CreateEffect(kRainingArrowRing06Model, impact.Position, impact.Angle,
        light, 1, impact.Owner, -1, 0, 0, 0, 0.125f);
    CreateEffect(kRainingArrowRing06Model, impact.Position, impact.Angle,
        light, 3, impact.Owner, -1, 0, 0, 0, 0.25f);
    CreateEffect(kRainingArrowRing07Model, impact.Position, impact.Angle,
        light, 0, impact.Owner, -1, 0, 0, 0, 0.13f);

    // Pegasus 0x671DF0..0x671E6C also throws two native Stone1/Stone2 impact
    // fragments. RISE carries the same native semantic route but its numeric
    // model layout has drifted, so use the enum base rather than 235/236.
    vec3_t fragmentAngle;
    Vector(1.0f, 0.0f, 0.0f, light);
    OBJECT* owner = impact.Owner ? impact.Owner : &impact;
    for (int index = 0; index < 2; ++index)
    {
        Vector(0.0f, 0.0f, static_cast<float>(rand() % 360), fragmentAngle);
        CreateEffect(MODEL_STONE1 + rand() % 2,
            impact.Position, fragmentAngle, light, 10, owner, -1,
            0, 77, 0, 0.0f, -1);
    }
}
}

bool EnsureRainingArrowModel(int modelId)
{
    if (!EnsureRainingArrowDamageBitmap() ||
        !EnsureRainingArrowRendererBitmaps() || !Models ||
        modelId < kRainingArrowFirstModel || modelId > kRainingArrowLastModel)
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
        const char* textureName = model.Textures[mesh].FileName;
        // ring07 contains "@@flamestan_mono.JPG" while the pinned Pegasus
        // asset is flamestan_mono.OZJ. Preserve the model script metadata but
        // resolve the actual file name after the two verified marker bytes.
        while (*textureName == '@') ++textureName;
        const std::string texturePath = std::string(directory) + textureName;
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

int RainingArrowDamageBitmap()
{
    EnsureRainingArrowDamageBitmap();
    return g_rainingArrowDamageBitmap;
}

bool IsRainingArrowDamageBitmap(int bitmapId)
{
    return g_rainingArrowDamageBitmap != BITMAP_UNKNOWN &&
        bitmapId == g_rainingArrowDamageBitmap;
}

void CreateRainingArrowRoot(OBJECT& caster, int skill, short casterIndex)
{
    // Pegasus 0x670C2E creates caster-owned 0x2792 and carries the caster
    // index; 0x670A77 validates that index back to this same owner object.
    vec3_t light;
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kRainingArrowRootModel, caster.Position, caster.Angle, light,
        0, &caster, -1, static_cast<WORD>(skill), 0, 0, 1.0f, casterIndex);
}

void CreateRainingArrowTarget(OBJECT& target, int skill)
{
    // Pegasus 0x670D19 iterates damage targets and 0x670AAF creates a 0x2793
    // target-follow effect with SkillIndex 2023 on each valid target object.
    vec3_t light;
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kRainingArrowFollowModel, target.Position, target.Angle, light,
        0, &target, -1, static_cast<WORD>(skill), 0);
}

void CreateRainingArrowChildren(OBJECT& root)
{
    // Exact immediate cascade recovered at Pegasus 0x671E82.
    vec3_t position, light;
    VectorCopy(root.Position, position);
    position[2] += 300.0f;
    Vector(0.50f, 0.65f, 1.0f, light);
    CreateEffect(kRainingArrowRing01Model, position, root.Angle, light,
        0, root.Owner, -1, 0, 0, 0, 2.5f);

    VectorCopy(root.Position, position);
    position[2] += 250.0f;
    Vector(0.65f, 0.30f, 0.70f, light);
    CreateEffect(kRainingArrowRing03Model, position, root.Angle, light,
        0, root.Owner, -1, 0, 0, 0, 1.5f);
    CreateEffect(kRainingArrowRing03Model, position, root.Angle, light,
        0, root.Owner, -1, 0, 0, 0, 2.0f);

    // Pegasus 0x671FC9 passes the root Light vector (OBJECT+0xB0).
    CreateEffect(kRainingArrowRing08Model, root.Position, root.Angle,
        root.Light, 0, root.Owner, -1, 0, 0, 0, 0.8f);

    Vector(0.80f, 0.80f, 1.0f, light);
    for (int index = 0; index < 3; ++index)
    {
        VectorCopy(root.Position, position);
        position[2] += 350.0f + static_cast<float>(index) * 100.0f;
        CreateEffect(kRainingArrowRing02Model, position, root.Angle, light,
            1, root.Owner, -1, 0, 0, 0, 1.8f);
        CreateGroundLayer(root, index);
    }
}

void InitializeRainingArrow(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kRainingArrowRootModel:
        effect.LifeTime = 20.0f;
        effect.Timer = 0.0f;
        effect.Distance = 0.0f;
        effect.Gravity = 0.0f;
        effect.Alpha = 0.0f;
        CreateRainingArrowChildren(effect);
        break;
    case kRainingArrowFollowModel:
        effect.LifeTime = static_cast<float>(kRainingArrowFollowLifetime);
        effect.Distance = static_cast<float>(kRainingArrowFollowLifetime);
        effect.Alpha = 0.0f;
        break;
    case kRainingArrowImpactModel:
        effect.LifeTime = 50.0f;
        effect.Distance = 20.0f;
        effect.Scale = 1.278409f;
        effect.Gravity = 0.0f;
        effect.AlphaTarget = 0.0f;
        effect.Alpha = 1.0f;
        effect.Angle[2] = static_cast<float>(rand() % 360);
        effect.Position[2] += 3.0f;
        CreateImpactLayers(effect);
        break;
    case kRainingArrowGroundModel:
        effect.LifeTime = effect.SubType >= 3 ? 10.0f : 20.0f;
        effect.Distance = effect.LifeTime;
        effect.Alpha = 1.0f;
        effect.AnimationFrame = 0.0f;
        effect.PriorAnimationFrame = 0.0f;
        effect.Angle[2] = 0.0f;
        break;
    case kRainingArrowRing01Model:
    case kRainingArrowRing02Model:
    case kRainingArrowRing03Model:
        effect.LifeTime = 25.0f;
        effect.Alpha = 1.0f;
        if (effect.SubType == 1)
        {
            effect.Distance = effect.Scale;
            effect.Scale = effect.Type == kRainingArrowRing02Model ? 0.0f : 1.0f;
        }
        break;
    case kRainingArrowRing04Model:
    case kRainingArrowRing05Model:
        effect.LifeTime = 20.0f;
        effect.Alpha = 1.0f;
        effect.Distance = effect.Scale;
        effect.Scale = 0.0f;
        if (effect.Type == kRainingArrowRing05Model)
            effect.Angle[1] = static_cast<float>(rand() % 360);
        break;
    case kRainingArrowRing06Model:
        effect.LifeTime = 20.0f;
        if (effect.SubType == 3)
            effect.Alpha = 0.0f;
        else
        {
            effect.Alpha = 1.0f;
            effect.Distance = effect.Scale;
            effect.Scale = 0.0f;
        }
        break;
    case kRainingArrowRing07Model:
        effect.LifeTime = 20.0f;
        effect.Alpha = 0.0f;
        break;
    case kRainingArrowRing08Model:
        effect.LifeTime = 20.0f;
        effect.Alpha = 1.0f;
        break;
    }
}

void UpdateRainingArrow(OBJECT& effect, float animationFactor)
{
    switch (effect.Type)
    {
    case kRainingArrowRootModel:
    {
        if (!effect.Owner)
        {
            effect.LifeTime = 0.0f;
            break;
        }
        const int priorTick = static_cast<int>(effect.Distance);
        effect.Timer += animationFactor;
        const int tick = static_cast<int>(effect.Timer);
        // Pegasus 0x671083 advances at most one authored volley per update;
        // it does not backfill skipped integer ticks after a slow frame.
        if (tick <= priorTick || tick > 20)
            break;
        effect.Distance = static_cast<float>(tick);
        const int remaining = 21 - tick;
        if (remaining < 10)
            break;
        CreateRainingArrowVolley(effect, remaining);
        if (remaining == 10)
            for (int subtype = 3; subtype < 6; ++subtype)
                CreateGroundLayer(effect, subtype);
        break;
    }
    case kRainingArrowFollowModel:
    {
        if (!effect.Owner)
        {
            effect.LifeTime = 0.0f;
            break;
        }
        VectorCopy(effect.Owner->Position, effect.Position);
        if (Models && effect.Owner->BoneTransform && effect.Owner->Type >= 0 &&
            effect.Owner->Type < MAX_MODELS)
        {
            BMD& ownerModel = Models[effect.Owner->Type];
            if (ownerModel.NumBones > 0)
            {
                vec3_t relative;
                Vector(0.0f, 0.0f, 0.0f, relative);
                VectorCopy(effect.Owner->Position, ownerModel.BodyOrigin);
                ownerModel.TransformPosition(effect.Owner->BoneTransform[0],
                    relative, effect.Position, true);
            }
        }

        // Pegasus 0x6710E6..0x671261 emits two transient 4x4-sheet damage
        // sprites every update. Their frame is elapsed lifetime (0..15),
        // light is LifeTime/16, and scale is random(1..2)*1.8.
        if (EnsureRainingArrowDamageBitmap())
        {
            const int frame = RainingArrowDamageFrame(
                effect.LifeTime, effect.Distance);
            const float luminosity = RainingArrowDamageLight(
                effect.LifeTime, effect.Distance);
            vec3_t light;
            Vector(luminosity, luminosity, luminosity, light);
            for (int index = 0; index < kRainingArrowDamageSpriteCount; ++index)
            {
                const float scale = RainingArrowDamageScale(rand() % 1001);
                CreateSprite(g_rainingArrowDamageBitmap, effect.Position, scale,
                    light, &effect, static_cast<float>(rand() % 360), frame);
            }
        }
        break;
    }
    case kRainingArrowImpactModel:
        effect.Gravity += 0.05f * animationFactor;
        effect.Alpha = max(0.0f, effect.Alpha - 0.05f * animationFactor);
        // Pegasus 0x6712BC subtracts 30 from the remaining lifetime, then
        // feeds that elapsed 20-tick window into the triangular envelope.
        effect.AlphaTarget = RainingArrowPulse(effect.LifeTime - 30.0f, 20.0f);
        break;
    case kRainingArrowGroundModel:
        if (effect.SubType >= 0 && effect.SubType < 6)
        {
            const GroundLayer& layer = kGroundLayers[effect.SubType];
            const float elapsed = max(0.0f, effect.Distance - effect.LifeTime);
            if (effect.SubType >= 3)
            {
                effect.Scale = (layer.scale + elapsed * layer.scaleStep) * kGroundScale;
                effect.Alpha = max(0.0f, 1.0f - elapsed * layer.alphaStep);
            }
            else
                effect.Alpha = RainingArrowPulse(effect.LifeTime,
                    effect.Distance);
            effect.BlendMeshLight = effect.Alpha;
        }
        break;
    case kRainingArrowRing01Model:
    case kRainingArrowRing02Model:
    case kRainingArrowRing03Model:
        if (effect.SubType == 1)
        {
            effect.Scale += (effect.Distance / 25.0f) * animationFactor;
            effect.Alpha = RainingArrowPulse(effect.LifeTime, 25.0f);
        }
        else
        {
            if (effect.Alpha <= 0.0f)
            {
                effect.LifeTime = 0.0f;
                break;
            }
            effect.Scale += (effect.Type == kRainingArrowRing03Model ? 0.10f : 0.03f) * animationFactor;
            effect.Alpha = max(0.0f, effect.Alpha - 0.05f * animationFactor);
        }
        effect.Position[2] -= 3.0f * animationFactor;
        effect.BlendMeshLight = effect.Alpha;
        break;
    case kRainingArrowRing04Model:
    case kRainingArrowRing05Model:
        effect.Scale += (effect.Distance / 20.0f) * animationFactor;
        effect.Alpha = max(0.0f, effect.Alpha - 0.05f * animationFactor);
        effect.BlendMeshLight = effect.Alpha;
        break;
    case kRainingArrowRing06Model:
        if (effect.SubType == 3)
            effect.Alpha = RainingArrowPulse(effect.LifeTime, 20.0f);
        else
        {
            effect.Scale += (effect.Distance / 20.0f) * animationFactor;
            effect.Alpha = max(0.0f,
                effect.Alpha - 0.05f * animationFactor);
        }
        effect.BlendMeshLight = effect.Alpha;
        break;
    case kRainingArrowRing07Model:
        effect.BlendMeshTexCoordV = static_cast<float>(static_cast<int>(effect.LifeTime) % 4) * 0.25f;
        effect.Alpha = RainingArrowPulse(effect.LifeTime, 20.0f);
        effect.BlendMeshLight = effect.Alpha;
        break;
    case kRainingArrowRing08Model:
        effect.Angle[2] += 5.0f * animationFactor;
        effect.Alpha = RainingArrowPulse(effect.LifeTime, 20.0f);
        effect.BlendMeshLight = effect.Alpha;
        break;
    }
}

void UpdateRainingArrowProjectile(OBJECT& effect)
{
    if (!effect.Live || effect.Skill != kRainingArrowSkill ||
        (effect.Type >= kRainingArrowFirstModel && effect.Type <= kRainingArrowLastModel))
        return;

    // Pegasus 0x66FC16 keeps the projectile bank neutral and resolves impact
    // against terrain+20. The normal RISE arrow case has already advanced it.
    effect.Angle[1] = 0.0f;
    UpdateRainingArrowProjectileJoints(effect, false);
    const float terrain = RequestTerrainHeight(effect.Position[0], effect.Position[1]);
    if (effect.Position[2] < terrain + 20.0f)
    {
        vec3_t position, angle, light;
        VectorCopy(effect.Position, position);
        position[2] = terrain;
        Vector(0.0f, 0.0f, 0.0f, angle);
        Vector(1.0f, 1.0f, 1.0f, light);
        OBJECT* owner = effect.Owner ? effect.Owner : &effect;
        CreateEffect(kRainingArrowImpactModel, position, angle, light,
            0, owner, -1, kRainingArrowSkill, 0);
        TraceRuntime("PROJECTILE_IMPACT", kRainingArrowSkill,
            kRainingArrowImpactModel, "TERRAIN");
#if defined(PEGASUS_ISOLATED_TEST)
        TraceRainingArrowTermination(effect, "TERRAIN");
#endif
        // Pegasus plays the hit sound only for the first projectile attached
        // to a live 0x2792 root and latches that state in the root subtype.
        if (effect.Owner && effect.Owner->Live &&
            effect.Owner->Type == kRainingArrowRootModel &&
            effect.Owner->SubType == 0)
        {
            PlayBuffer(kRainingArrowHitSound, effect.Owner);
            effect.Owner->SubType = 1;
        }
        UpdateRainingArrowProjectileJoints(effect, true);
        effect.LifeTime = 0.0f;
    }
    else if (effect.Position[2] > terrain + 600.0f)
    {
        TraceRuntime("PROJECTILE_IMPACT", kRainingArrowSkill,
            effect.Type, "ABOVE_LIMIT");
#if defined(PEGASUS_ISOLATED_TEST)
        TraceRainingArrowTermination(effect, "ABOVE_LIMIT");
#endif
        UpdateRainingArrowProjectileJoints(effect, true);
        effect.LifeTime = 0.0f;
    }
}

bool RenderRainingArrow(OBJECT& effect)
{
    // 0x2792/0x2793 are non-rendering orchestrators in Pegasus 0x6715F0.
    if (effect.Type == kRainingArrowRootModel || effect.Type == kRainingArrowFollowModel)
        return true;
    if (!EnsureRainingArrowModel(effect.Type) ||
        !EnsureRainingArrowRendererBitmaps())
        return false;

    if (effect.Type != kRainingArrowImpactModel &&
        effect.Type != kRainingArrowGroundModel)
    {
        // Pegasus 0x671655 routes all remaining visual children through the
        // engine's ordinary object draw path.
        if (!Calc_RenderObject(&effect, false, 0, 0))
            return false;
        Draw_RenderObject(&effect, false, 0, 0);
        return true;
    }

    if (!Calc_RenderObject(&effect, false, 0, 0))
        return false;

    BMD& model = Models[effect.Type];
    if (effect.Type == kRainingArrowGroundModel)
    {
        if (effect.SubType < 0 || effect.SubType >= 6)
            return true;
        const GroundLayer& layer = kGroundLayers[effect.SubType];
        VectorScale(effect.Light, effect.Alpha, model.BodyLight);
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.Alpha, -2, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
            RainingArrowTextureId(layer.textureSlot));
        return true;
    }

    // Pegasus 0x6717A7 renders the impact mesh once with Cratered, then—only
    // while LifeTime > 30—recalculates it at four authored scales and colors.
    VectorCopy(effect.Light, model.BodyLight);
    model.RenderMesh(0, RENDER_TEXTURE, 1.0f, -1, 1.0f,
        effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
        RainingArrowTextureId(kTextureCratered));

    if (effect.LifeTime > 30.0f)
    {
        struct ImpactLayer
        {
            int textureSlot;
            float scale;
            float red;
            float green;
            float blue;
            float alpha;
        };
        const ImpactLayer layers[4] = {
            {kTexturePinStar, 1.0f,                 0.70f, 0.75f, 1.00f, effect.AlphaTarget},
            {kTextureSpark,   1.0f,                 0.70f, 0.75f, 1.00f, effect.AlphaTarget},
            {kTextureBowStar, effect.Gravity * 2.0f,0.70f, 0.75f, 0.85f, effect.Alpha},
            {kTextureDamageMono,effect.Gravity*3.5f,0.70f, 0.60f, 0.80f, effect.Alpha}
        };
        const float savedScale = effect.Scale;
        for (int index = 0; index < 4; ++index)
        {
            const ImpactLayer& layer = layers[index];
            if (layer.scale <= 0.0f || layer.alpha <= 0.0f)
                continue;
            effect.Scale = layer.scale * kGroundScale;
            if (!Calc_RenderObject(&effect, false, 0, 0))
                continue;
            Vector(layer.red * layer.alpha, layer.green * layer.alpha,
                layer.blue * layer.alpha, model.BodyLight);
            model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
                layer.alpha, -2, layer.alpha,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
                RainingArrowTextureId(layer.textureSlot));
        }
        effect.Scale = savedScale;
    }
    return true;
}

}}
