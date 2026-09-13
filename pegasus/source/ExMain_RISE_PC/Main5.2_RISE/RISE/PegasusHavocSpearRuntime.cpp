#include "stdafx.h"
#include "PegasusHavocSpearRuntime.h"
#include "PegasusHavocSpearMath.h"
#include "PegasusTextureIds.h"
#include "DSPlaySound.h"
#include "PegasusSoundRuntime.h"
#include "PegasusRuntimeTrace.h"
#include "ZzzBMD.h"
#include "ZzzObject.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "GlobalBitmap.h"
#include "ZzzLodTerrain.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"

namespace rise { namespace pegasus {
namespace {
int g_havocAlphaLineBitmap = BITMAP_UNKNOWN;
int g_havocGroundCrackBitmap = BITMAP_UNKNOWN;

bool EnsureHavocOverrideTextures()
{
    // Pegasus loader registrations 0x86E2BF..0x86E2F7. Use the native
    // path-keyed loader instead of copying source numeric IDs 0x7F17/0x7F18,
    // which are not stable in RISE and can collide with custom textures.
    if (g_havocAlphaLineBitmap == BITMAP_UNKNOWN)
        g_havocAlphaLineBitmap = Bitmaps.LoadImageFile(
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\alpha_line2mono3.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (g_havocGroundCrackBitmap == BITMAP_UNKNOWN)
        g_havocGroundCrackBitmap = Bitmaps.LoadImageFile(
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\ground_crack_light02.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    const bool somasiLoaded = Bitmaps.FindTexture(kHavocSomasiJointBitmap) ||
        Bitmaps.LoadImageFile(kHavocSomasiJointBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\somasi01mono_R.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    const bool adLoaded = Bitmaps.FindTexture(kHavocAdJointBitmap) ||
        Bitmaps.LoadImageFile(kHavocAdJointBitmap,
            "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\ad001.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE);
    return g_havocAlphaLineBitmap != BITMAP_UNKNOWN &&
        g_havocGroundCrackBitmap != BITMAP_UNKNOWN && somasiLoaded && adLoaded;
}

const char* Directory(int modelId)
{
    return (modelId == kHavocNovaModel || modelId == kHavocLineModel)
        ? "Data\\RISE\\Pegasus\\Recovered\\Data\\Skill\\"
        : "Data\\RISE\\Pegasus\\Recovered\\Data\\Effect\\";
}
const char* Filename(int modelId)
{
    switch (modelId)
    {
    case kHavocNovaModel: return "Nova_arrow_sim02.bmd";
    case kHavocLineModel: return "Magum_line.bmd";
    case kHavocShockwaveModel: return "Shorkwave_gun01.bmd";
    default: return "knight_plancrack_a.bmd";
    }
}

float HavocRandomRange(float minimum, float maximum)
{
    // Pegasus 0x666E16: inclusive 0..1000 interpolation.
    return static_cast<float>(rand() % 1001) * 0.001f *
        (maximum - minimum) + minimum;
}

void CreateHavocRootJoints(OBJECT& owner)
{
    vec3_t origin, target, angle, light;
    VectorCopy(owner.Position, origin);
    Vector(0.0f, 0.0f, 0.0f, angle);
    for (int index = 0; index < 6; ++index)
    {
        const HavocRootJointSpec& spec = kHavocRootJointSpecs[index];
        const float degrees = HavocRandomRange(-30.0f, 30.0f) +
            static_cast<float>(index) * 60.0f;
        const float radians = degrees * 3.14159265358979323846f / 180.0f;
        target[0] = origin[0] + sinf(radians) * 300.0f;
        target[1] = origin[1] - cosf(radians) * 300.0f;
        target[2] = origin[2] + HavocRandomRange(20.0f, 90.0f);
        Vector(spec.red, spec.green, spec.blue, light);
        const int bitmap = spec.bitmapRole == kHavocAdJointRole
            ? kHavocAdJointBitmap : kHavocSomasiJointBitmap;
        CreateJoint(bitmap, origin, target, angle, spec.subtype, &owner,
            HavocRootJointScale(spec), owner.PKKey, kHavocSpearSkill,
            owner.m_bySkillSerialNum, -1, light, -1);
    }
}

void CreateHavocImpactBurst(const vec3_t origin, OBJECT& owner,
    float scaleFactor, bool createEnd)
{
    // Pegasus 0x6669B7..0x666BDC: four randomized lightning particles,
    // then the terrain shockwave and optional plan-crack end layer.
    vec3_t position, angle, light;
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(0.6f, 0.85f, 1.0f, light);
    const int xyRange = max(2, static_cast<int>(scaleFactor * 45.0f));
    const int zRange = max(1, static_cast<int>(scaleFactor * 55.0f));
    for (int n = 0; n < 4; ++n)
    {
        position[0] = origin[0] + static_cast<float>((rand() % xyRange) - (xyRange / 2));
        position[1] = origin[1] + static_cast<float>((rand() % xyRange) - (xyRange / 2));
        position[2] = origin[2] + static_cast<float>(rand() % zRange);
        CreateParticle(BITMAP_LIGHTNING_MEGA1 + (rand() % 3), position,
            angle, light, 0, static_cast<float>((rand() % 35) + 35) * 0.01f * scaleFactor,
            &owner, -32768, true);
    }

    VectorCopy(origin, position);
    position[2] = RequestTerrainHeight(position[0], position[1]) + 5.0f;
    Vector(0.6f, 0.85f, 1.0f, light);
    CreateEffect(kHavocShockwaveModel, position, angle, light,
        0, &owner, -1, kHavocSpearSkill, 0, 0, 1.6f);
    if (createEnd)
    {
        position[2] = RequestTerrainHeight(position[0], position[1]) + 3.0f;
        Vector(0.55f, 0.8f, 1.0f, light);
        CreateEffect(kHavocEndModel, position, angle, light,
            0, &owner, -1, kHavocSpearSkill, 0, 0, scaleFactor * 0.85f);
    }
}

void CreateHavocRootBurst(OBJECT& root)
{
    vec3_t position, angle, light;
    VectorCopy(root.Owner->Position, position);
    VectorCopy(root.Owner->Angle, angle);
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kHavocLineModel, position, angle, light,
        0, root.Owner, -1, kHavocSpearSkill, 0, 0, 0.7f);
    CreateHavocRootJoints(*root.Owner);
    position[2] = RequestTerrainHeight(position[0], position[1]);
    // Pegasus 0x6670D0..0x667116: zero angle and skill2013. Losing
    // SkillIndex disables the alpha_line2mono3 second pass at0x66660B.
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(0.55f, 0.8f, 1.0f, light);
    CreateEffect(kHavocShockwaveModel, position, angle, light,
        0, root.Owner, -1, kHavocSpearSkill, 0, 0, 8.0f);
    PlayBuffer(kHavocSpearSound, nullptr, false);
    PlayBuffer(kHavocSpearExplosionSound, nullptr, false);
}

void CreateHavocOddTickBurst(OBJECT& root)
{
    vec3_t position, angle, light;
    position[0] = root.Owner->Position[0] + static_cast<float>((rand() % 700) - 350);
    position[1] = root.Owner->Position[1] + static_cast<float>((rand() % 700) - 350);
    position[2] = RequestTerrainHeight(position[0], position[1]);
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(0.7f, 0.9f, 1.0f, light);
    const float scale = static_cast<float>((rand() % 9) + 11) * 0.02f;
    CreateEffect(kHavocNovaModel, position, angle, light,
        0, root.Owner, -1, kHavocSpearSkill, 0, 0, scale);
    CreateHavocImpactBurst(position, *root.Owner, 1.0f, true);
}
}

bool EnsureHavocSpearModel(int modelId)
{
    if (!EnsureHavocOverrideTextures()) return false;
    if (!Models || modelId < kHavocFirstModel || modelId > kHavocLastModel) return false;
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

void CreateHavocSpearRoot(OBJECT& target, int skill)
{
    // Pegasus 0x666CB6 uses the same caster object for SetAction and 0x27A5.
    // The caller must pass caster, not the attacked target.
    vec3_t light;
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kHavocRootModel, target.Position, target.Angle, light,
        0, &target, -1, static_cast<WORD>(skill), 0);
}

void CreateHavocSpearTargetImpact(OBJECT& target, int ordinal, int skill)
{
    vec3_t position, angle, light;
    VectorCopy(target.Position, position);
    position[2] = RequestTerrainHeight(position[0], position[1]);
    Vector(0.0f, 0.0f, 0.0f, angle);
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateEffect(kHavocImpactModel, target.Position, target.Angle, light,
        9, &target, -1, static_cast<WORD>(skill), 0);
    const float scaleFactor = HavocTargetScale(target.Scale);
    Vector(0.7f, 0.9f, 1.0f, light);
    CreateEffect(kHavocNovaModel, position, angle, light,
        0, &target, -1, static_cast<WORD>(skill), 0, 0,
        static_cast<float>((rand() % 9) + 11) * 0.02f * scaleFactor);
    CreateHavocImpactBurst(position, target, scaleFactor, true);
    PlayBuffer((ordinal & 1) ? kHavocSpearHit2Sound : kHavocSpearHit1Sound,
        nullptr, false);
}

void InitializeHavocSpear(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kHavocNovaModel:
        // Pegasus 0x665E4B..0x665EB8.
        effect.LifeTime = kHavocNovaLifetime;
        effect.Distance = kHavocNovaLifetime;
        effect.Angle[2] = static_cast<float>(rand() % 360);
        effect.Scale += static_cast<float>(rand() % 10) * 0.025f;
        effect.Position[2] = HavocNovaHeight(effect.Position[2], effect.Scale,
            kHavocNovaLifetime);
        break;
    case kHavocLineModel:
        effect.LifeTime = kHavocLineLifetime;
        effect.Distance = kHavocLineLifetime;
        break;
    case kHavocShockwaveModel:
        // Distance retains the constructor scale and drives expansion.
        effect.LifeTime = kHavocShockwaveLifetime;
        effect.Distance = effect.Scale;
        break;
    case kHavocRootModel:
        effect.LifeTime = 15.0f;
        effect.Alpha = 0.0f;
        effect.Timer = 0.0f;
        break;
    case kHavocImpactModel: effect.LifeTime = 15.0f; effect.Alpha = 0.0f; break;
    case kHavocEndModel:
        effect.LifeTime = kHavocEndLifetime;
        effect.Distance = kHavocEndLifetime;
        effect.Angle[2] = static_cast<float>(rand() % 360);
        effect.Scale += static_cast<float>(rand() % 5) * 0.06f;
        break;
    }
}

void UpdateHavocSpear(OBJECT& effect, float animationFactor)
{
    if (effect.Type == kHavocNovaModel)
    {
        // Pegasus 0x6662A0..0x666365: terrain-relative rise envelope plus
        // lifetime alpha. Keep the source 234 model-height correction.
        const float initial = effect.Distance > 0.0f ? effect.Distance : 1.0f;
        const float terrain = RequestTerrainHeight(effect.Position[0], effect.Position[1]);
        effect.Position[2] = HavocNovaHeight(terrain, effect.Scale,
            effect.LifeTime, initial);
        effect.Alpha = HavocLifetimeAlpha(effect.LifeTime, initial);
        effect.BlendMeshLight = effect.Alpha;
    }
    else if (effect.Type == kHavocLineModel)
    {
        const float initial = effect.Distance > 0.0f ? effect.Distance : 1.0f;
        effect.Alpha = HavocLifetimeAlpha(effect.LifeTime, initial);
        effect.BlendMeshLight = effect.Alpha;
    }
    else if (effect.Type == kHavocShockwaveModel)
    {
        effect.Scale = HavocShockwaveScaleStep(effect.Scale, effect.Distance,
            animationFactor);
        effect.Alpha = HavocLifetimeAlpha(effect.LifeTime,
            kHavocShockwaveLifetime);
        effect.BlendMeshLight = effect.Alpha;
        effect.Angle[2] += 6.0f * animationFactor;
    }
    else if (effect.Type == kHavocEndModel)
    {
        effect.Alpha = HavocEndAlpha(effect.LifeTime);
        effect.BlendMeshLight = effect.Alpha;
    }
    else if (effect.Type == kHavocRootModel)
    {
        // Pegasus 0x666129 checks only for a stored owner pointer. The burst
        // helpers read that actor's current position, while the invisible root
        // itself remains at its cast position. Owner::Live is not a lifetime
        // gate for already-created Havoc layers.
        if (!effect.Owner)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        const int previousTick = static_cast<int>(effect.Timer);
        effect.Timer += animationFactor;
        const int currentTick = static_cast<int>(effect.Timer);
        // Pegasus 0x666162..0x6661AA calls the radial root burst when tick 1
        // is first crossed, then deliberately falls through and also calls
        // the shared odd-tick Nova/explosion child at tick 1. The previous
        // adapter started this loop at 2 and therefore omitted that first
        // visible Nova child.
        for (int tick = max(previousTick + 1, 1);
            tick <= currentTick && tick <= kHavocRootLastTick; ++tick)
        {
            if (HavocCreatesRootBurstAtTick(tick))
                CreateHavocRootBurst(effect);
            if (HavocCreatesOddBurstAtTick(tick))
                CreateHavocOddTickBurst(effect);
        }
    }
    else if (effect.Type == kHavocImpactModel)
    {
        // Pegasus 0x665FFB..0x66611D: the invisible target orchestrator
        // follows its owner and emits exactly two native mega-lightnings per
        // update from an 80x80x120 box.
        if (!effect.Owner)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        VectorCopy(effect.Owner->Position, effect.Position);
        vec3_t position, angle, light;
        Vector(0.0f, 0.0f, 0.0f, angle);
        Vector(0.2f, 0.2f, 1.0f, light);
        for (int n = 0; n < 2; ++n)
        {
            position[0] = effect.Position[0] + static_cast<float>((rand() % 80) - 40);
            position[1] = effect.Position[1] + static_cast<float>((rand() % 80) - 40);
            position[2] = effect.Position[2] + static_cast<float>(rand() % 120);
            const float scale = static_cast<float>((rand() % 70) + 22) * 0.01f;
            CreateParticle(BITMAP_LIGHTNING_MEGA1 + (rand() % 3), position,
                angle, light, 0, scale, effect.Owner, -32768, true);
        }
    }
}

bool RenderHavocSpear(OBJECT& effect)
{
#if defined(PEGASUS_ISOLATED_TEST)
    static unsigned int reported[kHavocLastModel - kHavocFirstModel + 1] = {};
    const int traceSlot = effect.Type - kHavocFirstModel;
    const bool traceSlotValid = traceSlot >= 0 &&
        traceSlot <= kHavocLastModel - kHavocFirstModel;
#endif
    // Pegasus 0x6663FE..0x666412: 0x27A5/0x27A6 are orchestration-only.
    if (effect.Type == kHavocRootModel || effect.Type == kHavocImpactModel)
        return true;
    if (!EnsureHavocSpearModel(effect.Type))
    {
#if defined(PEGASUS_ISOLATED_TEST)
        if (traceSlotValid && !(reported[traceSlot] & 1))
        {
            reported[traceSlot] |= 1;
            TraceRuntime("HAVOC_RENDER_MODEL", effect.Type, effect.SubType, "LOAD_FAILED");
        }
#endif
        return false;
    }
    BMD& model = Models[effect.Type];
    // Pegasus 0x66664F..0x666684 advances Magum_line at speed 1.0 and
    // 0x66653D..0x666567 advances Shorkwave_gun01 at speed 0.3 before pose
    // calculation. Remapped dynamic IDs are outside RISE's generic BMD loop.
    float playSpeed = 0.0f;
    if (effect.Type == kHavocLineModel)
        playSpeed = kHavocLinePlaySpeed;
    else if (effect.Type == kHavocShockwaveModel)
        playSpeed = kHavocShockwavePlaySpeed;
    if (playSpeed > 0.0f)
    {
        model.CurrentAction = effect.CurrentAction;
        model.PlayAnimation(&effect.AnimationFrame,
            &effect.PriorAnimationFrame, &effect.PriorAction, playSpeed,
            effect.Position, effect.Angle);
        TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,
            effect.AnimationFrame);
    }
    if (!Calc_RenderObject(&effect, false, 0, 0))
    {
#if defined(PEGASUS_ISOLATED_TEST)
        if (traceSlotValid && !(reported[traceSlot] & 2))
        {
            reported[traceSlot] |= 2;
            TraceRuntime("HAVOC_RENDER_POSE", effect.Type, effect.SubType, "REJECTED");
        }
#endif
        return false;
    }
#if defined(PEGASUS_ISOLATED_TEST)
    if (traceSlotValid && !(reported[traceSlot] & 4))
    {
        reported[traceSlot] |= 4;
        char detail[256];
        _snprintf_s(detail, sizeof(detail), _TRUNCATE,
            "DRAW meshes=%d bones=%d actions=%d frame=%.3f scale=%.3f alpha=%.3f life=%.3f pos=%.1f,%.1f,%.1f",
            model.NumMeshs, model.NumBones, model.NumActions, effect.AnimationFrame,
            effect.Scale, effect.Alpha, effect.LifeTime,
            effect.Position[0], effect.Position[1], effect.Position[2]);
        TraceRuntime("HAVOC_RENDER_INPUT", effect.Type, effect.SubType, detail);
    }
#endif
    // Exact type branches from Pegasus 0x6663F7..0x66683D. These are not a
    // shared all-mesh bright fallback: each BMD has its own mesh, color and
    // texture-override contract.
    switch (effect.Type)
    {
    case kHavocNovaModel: // 0x27A2
        VectorCopy(effect.Light, model.BodyLight);
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.Alpha, -2, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        if (model.NumMeshs > 1)
        {
            model.RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT,
                effect.Alpha, -2, effect.Alpha,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
            model.RenderMesh(1, RENDER_CHROME | RENDER_BRIGHT,
                effect.Alpha, -2, effect.Alpha,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
                BITMAP_CHROME);
        }
        break;
    case kHavocLineModel: // 0x27A3
        Vector(0.15f, 0.5f, 0.7f, model.BodyLight);
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.Alpha, -2, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        Vector(0.15f, 0.15f, 0.6f, model.BodyLight);
        // 666747 jumps to666830, which pushes mesh1 (not the prior mesh0).
        model.RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.Alpha, -2, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        break;
    case kHavocShockwaveModel: // 0x27A4
        model.BodyLight[0] = effect.Light[0] * effect.Alpha;
        model.BodyLight[1] = effect.Light[1] * effect.Alpha;
        model.BodyLight[2] = effect.Light[2] * effect.Alpha;
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.Alpha, -2, 1.0f,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        if (effect.Skill == kHavocSpearSkill)
            model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
                effect.Alpha, -2, 1.0f,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
                g_havocAlphaLineBitmap);
        break;
    case kHavocEndModel: // 0x27A7
        model.BodyLight[0] = effect.Light[0] * effect.Alpha;
        model.BodyLight[1] = effect.Light[1] * effect.Alpha;
        model.BodyLight[2] = effect.Light[2] * effect.Alpha;
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
            effect.Alpha, -2, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
            g_havocGroundCrackBitmap);
        break;
    default:
        return false;
    }
    return true;
}

}}
