#include "stdafx.h"
#include "SlayerSkillResources.h"

#include "ZzzBMD.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "DSPlaySound.h"
#include "../../../ZzzInfomation.h"
#include "../../../ZzzObject.h"
#include "../../../ZzzTexture.h"
#include "../shared/SlayerSkillContractData.h"
#ifdef RISE_SLAYER_RUNTIME_QA
#include "../../SlayerRuntimeQA.h"
#endif

#include <cmath>
#include <string>

namespace rise { namespace slayer {

namespace {

struct ModelRow
{
    int id;
    const char* filename;
};

const char* const kEffectDirectory = "Data\\RISE\\Slayer\\Effect\\";

const ModelRow kModels[] = {
    // 292 Sword Inertia is dispatched through S21 roots 0x68A and 0x689.
    // The S21-exclusive Slayer sword family provides the renderable sword
    // plus its one-mesh ring/aura child.
    {kSwordInertiaModel, "ak_skill_sword.bmd"},
    {kSwordInertiaAuraModel, "ak_skill_sword_s01.bmd"},
    {kBatFlockModel, "Bat_van01.bmd"},
    // 293 Bat Flock keeps a separate trail carrier in the 5.2 overlay. The
    // direct S21 dispatcher evidence is limited to roots 0x682/0x683/0x684.
    {kBatFlockTrailModel, "van_object02_skill.bmd"},
    // 294 Pierce root 0x679 and 295 Detection root 0x692 use isolated 5.2
    // carrier models; disk asset identity is pinned separately by hashes.
    {kPierceImpactModel, "Van_object04_skill.bmd"},
    // Detection's extra mark/impact carriers stay separate in 5.2 even
    // though the direct 295 handler only proves root 0x692.
    {kDetectionMarkModel, "van_object03_skill.bmd"},
    {kDetectionImpactModel, "Van_object04_skill.bmd"},
    // A zero-mesh carrier is retained for the isolated Pierce animation; the
    // dump does not establish it as a direct child of handler 0x12D1209.
    {kPierceSwordLineModel, "van_swordline01.bmd"}
};

const ModelRow* FindModelRow(int modelId)
{
    for (unsigned i = 0; i < sizeof(kModels) / sizeof(kModels[0]); ++i)
        if (kModels[i].id == modelId)
            return &kModels[i];
    return 0;
}

bool IsKnownModel(int modelId)
{
    return modelId >= kFirstModel && modelId <= kLastModel &&
        FindModelRow(modelId) != 0;
}

bool IsController(int type)
{
    return type >= kSwordInertiaController && type <= kLastEffect;
}

float InitialLife(const OBJECT& effect)
{
    switch (effect.Type)
    {
    case kSwordInertiaModel:
    case kSwordInertiaAuraModel: return 30.f;
    case kSwordInertiaController:
    case kPierceMotionController: return 99.f;
    case kDemolishController: return 60.f;
    case kSwordFlightController: return 30.f;
    case kBatFlockModel: return effect.SubType == 0 ? 40.f :
        (effect.SubType == 3 ? 10.f : 50.f);
    case kBatFlockTrailModel: return 20.f;
    case kBatFlockController:
    case kBatFlockTargetController:
    case kBatFlockDotController: return 10.f;
    case kBatFlockOrbitController: return 60.f;
    case kPierceSwordController:
    case kPierceBurstController: return 30.f;
    case kPierceFlashController: return 4.f;
    case kPierceFanController: return 1.f;
    case kPierceBatController: return 10.f;
    case kPierceImpactModel:
    case kDetectionImpactModel: return effect.SubType == 1 ? 35.f : 40.f;
    case kDetectionMarkModel: return effect.SubType == 1 ? 40.f : 35.f;
    case kPierceSwordLineModel:
    case kPierceController:
    case kDetectionController:
    case kDetectionChildController: return 30.f;
    default: return 20.f;
    }
}

void SpawnChild(int type, const OBJECT& source, OBJECT* owner, int subtype,
    float scale)
{
    vec3_t position, angle, light;
    VectorCopy(source.Position, position);
    VectorCopy(source.Angle, angle);
    VectorCopy(source.Light, light);
    CreateEffect(type, position, angle, light, subtype, owner, -1,
        source.Skill, 0, 0, scale, source.m_sTargetIndex);
}

OBJECT* ResolveTarget(short targetIndex)
{
    if (targetIndex < 0 || targetIndex >= MAX_CHARACTERS_CLIENT)
        return 0;
    OBJECT* target = &CharactersClient[targetIndex].Object;
    return target->Live ? target : 0;
}

void AttachToTarget(OBJECT& effect)
{
    OBJECT* target = ResolveTarget(effect.m_sTargetIndex);
    if (target)
        VectorCopy(target->Position, effect.Position);
}

void MoveBetween(OBJECT& effect, OBJECT* start, OBJECT* finish,
    float progress)
{
    if (!start || !finish)
        return;
    if (progress < 0.f) progress = 0.f;
    if (progress > 1.f) progress = 1.f;
    effect.Position[0] = start->Position[0] +
        (finish->Position[0] - start->Position[0]) * progress;
    effect.Position[1] = start->Position[1] +
        (finish->Position[1] - start->Position[1]) * progress;
    effect.Position[2] = start->Position[2] +
        (finish->Position[2] - start->Position[2]) * progress;
}

float Clamp01(float value)
{
    if (value < 0.f) return 0.f;
    if (value > 1.f) return 1.f;
    return value;
}

#ifdef RISE_SLAYER_RUNTIME_QA
unsigned gRenderSamples[8] = {};

const char* SkillNameForModel(int modelId)
{
    switch (modelId)
    {
    case kSwordInertiaModel: return "Sword Inertia";
    case kSwordInertiaAuraModel: return "Sword Inertia";
    case kBatFlockModel: return "Bat Flock";
    case kBatFlockTrailModel: return "Bat Flock";
    case kPierceImpactModel:
    case kPierceSwordLineModel: return "Pierce Attack";
    case kDetectionMarkModel:
    case kDetectionImpactModel: return "Detection";
    default: return "unknown";
    }
}

void LogModelRender(const OBJECT& effect, const BMD& model)
{
    const unsigned slot = static_cast<unsigned>(effect.Type - kFirstModel);
    if (slot >= sizeof(gRenderSamples) / sizeof(gRenderSamples[0]) ||
        gRenderSamples[slot] >= 128)
        return;
    ++gRenderSamples[slot];
    char line[384];
    sprintf_s(line, sizeof(line),
        "native-render-submit skill=%s skillId=%d type=%d subtype=%d "
        "model=%s live=%d visible=%d life=%.3f alpha=%.3f scale=%.3f "
        "ownerLive=%d modelReady=1 meshes=%d bones=%d actions=%d",
        SkillNameForModel(effect.Type), static_cast<int>(effect.Skill),
        effect.Type, effect.SubType, model.Name, effect.Live ? 1 : 0,
        effect.Visible ? 1 : 0, static_cast<double>(effect.LifeTime),
        static_cast<double>(effect.Alpha), static_cast<double>(effect.Scale),
        effect.Owner && effect.Owner->Live ? 1 : 0, model.NumMeshs,
        model.NumBones, model.NumActions);
    rise::slayerqa::AppendRuntimeQALog(line);
}
#endif

}

bool IsEffectType(int type)
{
#ifdef RISE_SLAYER_PORT
    return IsKnownModel(type) || IsController(type);
#else
    (void)type;
    return false;
#endif
}

bool RequiresModel(int type)
{
#ifdef RISE_SLAYER_PORT
    return IsKnownModel(type);
#else
    (void)type;
    return false;
#endif
}

bool EnsureModel(int modelId)
{
#ifdef RISE_SLAYER_PORT
    if (!Models || !IsKnownModel(modelId))
        return false;
    BMD& model = Models[modelId];
    if (model.NumBones > 0 && model.NumActions > 0)
        return true;
    const ModelRow* row = FindModelRow(modelId);
    if (!row)
        return false;
    char directory[MAX_PATH] = {};
    strcpy_s(directory, kEffectDirectory);
    char filename[MAX_PATH] = {};
    strcpy_s(filename, row->filename);
    model.m_iBMDSeqID = modelId;
    if (!model.Open2(directory, filename))
        return false;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        const std::string texturePath = std::string(directory) +
            model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(texturePath,
            GL_LINEAR, GL_REPEAT);
        if (model.IndexTexture[mesh] == BITMAP_UNKNOWN)
        {
            model.Release();
            return false;
        }
    }
    return model.NumBones > 0 && model.NumActions > 0;
#else
    (void)modelId;
    return false;
#endif
}

void InitializeEffect(OBJECT& effect)
{
#ifdef RISE_SLAYER_PORT
    if (!IsEffectType(effect.Type))
        return;
    effect.CurrentAction = 0;
    effect.AnimationFrame = 0.f;
    effect.PriorAnimationFrame = 0.f;
    effect.Velocity = 0.4f;
    effect.Timer = 0.f;
    effect.StartPosition[0] = effect.Position[0];
    effect.StartPosition[1] = effect.Position[1];
    effect.StartPosition[2] = effect.Position[2];
    effect.LifeTime = InitialLife(effect);
    if (effect.Type == kDetectionMarkModel ||
        effect.Type == kDetectionImpactModel)
        effect.Scale = effect.Scale <= 0.f ? 1.f : effect.Scale;

    // Root creation is kept on the controller initializer.  The numeric S21
    // root ids are evidence labels; 5.2 carrier children remain isolated and
    // are not presented as recovered machine-code children.
    switch (effect.Type)
    {
    case kSwordInertiaController:
        // S21 292 handler 0x12D0F0F emits roots 0x68A and 0x689, then the
        // native animation gates launch the three returning sword lanes.
        break;
    case kSwordFlightController:
        // Keep movement, collision ownership and triangles in separate
        // records in the 5.2 carrier graph.
        SpawnChild(kSwordInertiaModel, effect, &effect, effect.SubType, 0.72f);
        SpawnChild(kSwordInertiaAuraModel, effect, &effect, effect.SubType, 1.05f);
        break;
    case kBatFlockController:
        // S21 293 handler 0x12D10A0 creates roots 0x682/0x683/0x684.
        SpawnChild(kBatFlockTrailModel, effect, effect.Owner, 0, 0.70f);
        break;
    case kPierceController:
    {
        vec3_t flare, ring, white;
        Vector(0.6f, 0.6f, 0.6f, flare);
        Vector(0.05f, 0.01f, 0.25f, ring);
        Vector(1.f, 1.f, 1.f, white);
        CreateParticle(kFlare01RedBitmap, effect.Position, effect.Angle,
            flare, 10, 2.5f, &effect);
        CreateParticle(kRingOfGradation2Bitmap, effect.Position, effect.Angle,
            ring, 0, 3.75f, &effect);
        CreateParticle(kEnemyRing01Bitmap, effect.Position, effect.Angle,
            white, 0, 1.2f, &effect);
        break;
    }
    case kPierceSwordController:
        // Isolated 5.2 carrier for the Pierce sword-line animation.
        SpawnChild(kPierceSwordLineModel, effect, effect.Owner, 3, 1.f);
        break;
    case kPierceBurstController:
    {
        // Isolated Pierce contact carrier. Preserve each bitmap family
        // instead of collapsing it into the impact BMD.
        vec3_t white, violet, orange;
        Vector(1.f, 1.f, 1.f, white);
        Vector(0.35f, 0.10f, 0.65f, violet);
        Vector(1.f, 0.30f, 0.05f, orange);
        CreateParticle(kAlphaRingX256Bitmap, effect.Position, effect.Angle,
            violet, 0, 1.5f, &effect);
        CreateParticle(kDamage1MonoBitmap, effect.Position, effect.Angle,
            white, 0, 1.f, &effect);
        CreateParticle(kFlare01RedBitmap, effect.Position, effect.Angle,
            orange, 1, 1.2f, &effect);
        CreateParticle(kPinStar02RedBitmap, effect.Position, effect.Angle,
            orange, 2, 1.f, &effect);
        break;
    }
    case kPierceFanController:
    {
        // Isolated one-tick fan-out carrier.
        vec3_t white;
        Vector(1.f, 1.f, 1.f, white);
        CreateParticle(kAlphaRingX256Bitmap, effect.Position, effect.Angle,
            white, 0, 0.8f, &effect);
        CreateParticle(kAlphaRingX256Bitmap, effect.Position, effect.Angle,
            white, 1, 0.8f, &effect);
        CreateParticle(kDamage1MonoBitmap, effect.Position, effect.Angle,
            white, 0, 1.f, &effect);
        break;
    }
    case kPierceBatController:
        // 0x686 initializer creates two long-lived 0x678/subtype3 bats.
        SpawnChild(kBatFlockModel, effect, &effect, 3, 0.8f);
        SpawnChild(kBatFlockModel, effect, &effect, 3, 0.8f);
        break;
    case kDetectionController:
    {
        vec3_t white, warm;
        Vector(1.f, 1.f, 1.f, white);
        Vector(0.70f, 0.25f, 0.05f, warm);
        SpawnChild(kDetectionChildController, effect, &effect, 0, 1.f);
        SpawnChild(kDetectionImpactModel, effect, &effect, 0, 1.f);
        CreateParticle(kMagicGround12Bitmap, effect.Position, effect.Angle,
            white, 4, 1.f, &effect);
        CreateParticle(kMagicGround12Bitmap, effect.Position, effect.Angle,
            white, 5, 1.f, &effect);
        CreateParticle(BITMAP_MMsSs2_r, effect.Position, effect.Angle,
            white, 2, 2.5f, &effect);
        CreateParticle(kRingOfGradation2Bitmap, effect.Position, effect.Angle,
            warm, 0, 6.f, &effect);
        break;
    }
    case kDemolishController:
    {
        // S21 297 handler 0x12D1443 emits root 0x695 and uses the 0x511
        // cast family.  The client-side root is a persistent self aura; the
        // gold/yellow sprite pair is copied from the S21 Effect directory.
        vec3_t gold, yellow;
        Vector(1.f, 0.62f, 0.08f, gold);
        Vector(1.f, 0.90f, 0.20f, yellow);
        CreateParticle(kFireHik01GoldBitmap, effect.Position, effect.Angle,
            gold, 0, 2.2f, &effect);
        CreateParticle(kFlare01YellowBitmap, effect.Position, effect.Angle,
            yellow, 0, 1.5f, &effect);
        CreateParticle(kRingOfGradation2Bitmap, effect.Position, effect.Angle,
            gold, 0, 4.5f, &effect);
        break;
    }
    default:
        break;
    }
#else
    (void)effect;
#endif
}

void UpdateEffect(OBJECT& effect, float animationFactor)
{
#ifdef RISE_SLAYER_PORT
    if (!IsEffectType(effect.Type))
        return;
    OBJECT* owner = effect.Owner;
    OBJECT* target = ResolveTarget(effect.m_sTargetIndex);
    const float initialLife = InitialLife(effect);
    const float elapsed = initialLife - effect.LifeTime;

    if (IsController(effect.Type))
    {
        switch (effect.Type)
        {
        case kSwordInertiaController:
            if (owner && owner->Live)
            {
                VectorCopy(owner->Position, effect.StartPosition);
                // S21 0x15411E2/0x15413C0 gate two launches at frame 4;
                // 0x15418B3 gates the third launch at frame 7.
                if (effect.Timer < 1.f && owner->AnimationFrame >= 4.f)
                {
                    for (int lane = 0; lane < 2; ++lane)
                    {
                        OBJECT laneSource = effect;
                        laneSource.Angle[2] += lane == 0 ? -14.f : 0.f;
                        SpawnChild(kSwordFlightController, laneSource, owner,
                            lane, 1.f);
                    }
                    effect.Timer = 1.f;
                }
                if (effect.Timer < 2.f && owner->AnimationFrame >= 7.f)
                {
                    OBJECT laneSource = effect;
                    laneSource.Angle[2] += 14.f;
                    SpawnChild(kSwordFlightController, laneSource, owner, 2, 1.f);
                    effect.Timer = 2.f;
                }
            }
            break;
        case kSwordFlightController:
            if (owner && owner->Live)
            {
                const float normalized = Clamp01(elapsed / initialLife);
                const float travel = normalized <= 0.5f ? normalized * 2.f :
                    (1.f - normalized) * 2.f;
                vec3_t endpoint;
                if (target && target != owner)
                {
                    VectorCopy(target->Position, endpoint);
                }
                else
                {
                    const float heading = (effect.Angle[2] - 90.f) *
                        0.01745329252f;
                    endpoint[0] = effect.StartPosition[0] + cosf(heading) * 650.f;
                    endpoint[1] = effect.StartPosition[1] + sinf(heading) * 650.f;
                    endpoint[2] = effect.StartPosition[2];
                }
                const float fan = static_cast<float>(effect.SubType - 1) * 75.f;
                const float side = effect.Angle[2] * 0.01745329252f;
                effect.Position[0] = effect.StartPosition[0] +
                    (endpoint[0] - effect.StartPosition[0]) * travel +
                    cosf(side) * fan;
                effect.Position[1] = effect.StartPosition[1] +
                    (endpoint[1] - effect.StartPosition[1]) * travel +
                    sinf(side) * fan;
                effect.Position[2] = effect.StartPosition[2] +
                    (endpoint[2] - effect.StartPosition[2]) * travel +
                    sinf(travel * 3.14159265f) * 35.f;
                effect.Angle[0] = normalized <= 0.5f ? 0.f : 180.f;
                effect.Angle[2] += 18.f * animationFactor;
            }
            break;
        case kBatFlockController:
            // 0x682 emits on LifeTime%5 == 0 or a 1/20 random pulse.
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f)
            {
                effect.Timer -= 1.f;
                const int life = static_cast<int>(effect.LifeTime);
                if ((life > 0 && life % 5 == 0) || rand() % 20 == 0)
                {
                    OBJECT pulse = effect;
                    pulse.Position[0] += static_cast<float>((rand() % 201) - 100);
                    pulse.Position[1] += static_cast<float>((rand() % 201) - 100);
                    pulse.Position[2] += static_cast<float>((rand() % 31) + 75);
                    Vector(1.f, 1.f, 1.f, pulse.Light);
                    SpawnChild(kBatFlockModel, pulse, effect.Owner, 2, 2.5f);
                    vec3_t half, gray;
                    Vector(0.5f, 0.5f, 0.5f, half);
                    Vector(0.65f, 0.65f, 0.65f, gray);
                    CreateParticle(kPinStar02RedBitmap, pulse.Position,
                        pulse.Angle, half, 0, 0.8f, &effect);
                    CreateParticle(kEmpact01Bitmap, pulse.Position,
                        pulse.Angle, gray, 0, 0.65f, &effect);
                }
            }
            break;
        case kBatFlockTargetController:
            if (target)
            {
                VectorCopy(target->Position, effect.Position);
                effect.Position[2] += 35.f;
                effect.Timer += animationFactor;
                while (effect.Timer >= 1.f)
                {
                    effect.Timer -= 1.f;
                    if (elapsed >= 1.f && rand() % 15 != 0)
                        continue;
                    vec3_t red;
                    Vector(1.f, 0.2f, 0.2f, red);
                    CreateParticle(kGhostMark02RedBitmap, effect.Position,
                        effect.Angle, red, 1, 0.2f, target);
                }
            }
            break;
        case kBatFlockDotController:
            if (target)
            {
                VectorCopy(target->Position, effect.Position);
                effect.Position[2] += 35.f;
                effect.Timer += animationFactor;
                while (effect.Timer >= 1.f)
                {
                    effect.Timer -= 1.f;
                    if (rand() % 15 == 0)
                        SpawnChild(kBatFlockOrbitController, effect,
                            effect.Owner, 0, 1.f);
                }
            }
            break;
        case kBatFlockOrbitController:
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f)
            {
                effect.Timer -= 1.f;
                const int phase = static_cast<int>(effect.LifeTime) % 6;
                effect.Position[2] += phase < 3 ? 8.f : -8.f;
                vec3_t magenta;
                Vector(0.7f, 0.15f, 0.7f, magenta);
                for (int ordinal = 0; ordinal < 3; ++ordinal)
                {
                    vec3_t p;
                    VectorCopy(effect.Position, p);
                    p[ordinal % 2] += static_cast<float>((rand() % 31) - 15);
                    CreateParticle(kFireHik01MagentaBitmap, p, effect.Angle,
                        magenta, 0, 0.5f, &effect);
                }
            }
            break;
        case kPierceController:
            if (target)
            {
                VectorCopy(target->Position, effect.Position);
                effect.Position[2] += 35.f;
            }
            // 294 root 0x679 is gated by the owner animation before the
            // isolated Pierce carrier is emitted.
            if (effect.Timer < 1.f && owner && owner->Live &&
                owner->AnimationFrame >= 3.5f)
            {
                SpawnChild(kPierceImpactModel, effect, &effect, 0, 1.f);
                effect.Timer = 1.f;
            }
            break;
        case kPierceMotionController:
            if (target)
            {
                VectorCopy(target->Position, effect.Position);
                effect.Position[2] += 35.f;
            }
            // Legacy 5.2 supplies one resolved target; keep the subordinate
            // visual carriers separate so their ownership/timing is explicit.
            if (effect.Timer < 1.f && target)
            {
                SpawnChild(kPierceBurstController, effect, &effect, 0, 1.f);
                SpawnChild(kPierceSwordController, effect, &effect, 0, 1.f);
                SpawnChild(kPierceSwordController, effect, &effect, 1, 1.f);
                SpawnChild(kPierceBatController, effect, &effect, 0, 1.f);
                SpawnChild(kPierceFanController, effect, &effect, 0, 1.f);
                SpawnChild(kPierceFanController, effect, &effect, 1, 1.f);
                SpawnChild(kPierceFlashController, effect, &effect, 0, 1.f);
                PlayImpactSound(kPierceAttack, target, 1);
                PlayImpactSound(kPierceAttack, target, 2);
                effect.Timer = 1.f;
            }
            break;
        case kPierceSwordController:
        case kPierceBurstController:
            AttachToTarget(effect);
            break;
        case kPierceFlashController:
            AttachToTarget(effect);
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f)
            {
                effect.Timer -= 1.f;
                vec3_t white, position;
                Vector(1.f, 1.f, 1.f, white);
                VectorCopy(effect.Position, position);
                position[0] += static_cast<float>((rand() % 31) - 15);
                position[1] += static_cast<float>((rand() % 31) - 15);
                CreateParticle(kGroundStarBitmap, position, effect.Angle,
                    white, 0, 1.f, &effect);
            }
            break;
        case kPierceFanController:
            AttachToTarget(effect);
            break;
        case kPierceBatController:
            AttachToTarget(effect);
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f)
            {
                effect.Timer -= 1.f;
                vec3_t white;
                Vector(1.f, 1.f, 1.f, white);
                CreateParticle(kEmpact01Bitmap, effect.Position, effect.Angle,
                    white, 0, 0.65f, &effect);
                CreateParticle(kGhostMark02Bitmap, effect.Position, effect.Angle,
                    white, 0, 0.2f, &effect);
            }
            break;
        case kDetectionController:
            if (owner && owner->Live)
            {
                VectorCopy(owner->Position, effect.Position);
                effect.Position[2] += 110.f;
            }
            // Detection remains tied to the caster action while the isolated
            // compatibility carrier emits its authored sprite family.
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f && owner && owner->Live &&
                owner->CurrentAction == kDetectionAction)
            {
                effect.Timer -= 1.f;
                for (int ordinal = 0; ordinal < 4; ++ordinal)
                {
                    vec3_t position, angle, light;
                    VectorCopy(effect.Position, position);
                    VectorCopy(effect.Angle, angle);
                    position[0] += static_cast<float>((rand() % 61) - 30);
                    position[1] += static_cast<float>((rand() % 61) - 30);
                    if ((rand() & 1) == 0)
                    {
                        Vector(0.4f, 0.45f, 1.f, light);
                        CreateParticle(kSmoke01Bitmap, position, angle, light,
                            0x77, 2.5f, &effect);
                    }
                    else
                    {
                        Vector(0.15f, 0.18f, 1.f, light);
                        CreateParticle(kSmokeLines01Bitmap + rand() % 3,
                            position, angle, light, 0x0D, 2.f, &effect);
                    }
                }
                OBJECT wave = effect;
                wave.Angle[2] = (static_cast<int>(effect.LifeTime) & 1) != 0 ?
                    -static_cast<float>(rand() % 101) :
                    static_cast<float>(rand() % 101);
                SpawnChild(kDetectionMarkModel, wave, &effect, 0, 0.2f);
            }
            break;
        case kDetectionChildController:
            if (owner && owner->Live)
            {
                VectorCopy(owner->Position, effect.Position);
                effect.Position[2] += 100.f;
                effect.Timer += animationFactor;
                while (effect.Timer >= 1.f)
                {
                    effect.Timer -= 1.f;
                    vec3_t blue;
                    Vector(0.35f, 0.65f, 1.f, blue);
                    OBJECT mark = effect;
                    VectorCopy(blue, mark.Light);
                    // Keep Detection's 5.2 mark/impact carriers separate from
                    // Bat Flock. The direct S21 evidence proves root 0x692;
                    // these child records are an isolated compatibility layer.
                    SpawnChild(kDetectionMarkModel, mark, &effect, 1, 1.0f);
                    if ((rand() & 1) == 0)
                        SpawnChild(kDetectionImpactModel, mark, &effect, 1,
                            0.8f);
                }
            }
            break;
        case kDemolishController:
            if (owner && owner->Live)
            {
                VectorCopy(owner->Position, effect.Position);
                effect.Position[2] += 100.f;
                effect.Timer += animationFactor;
                // S21 root 0x695 is a self buff, so keep the carrier alive for
                // the 60-second server effect and pulse only its authored
                // gold/yellow sprite family.
                while (effect.Timer >= 5.f)
                {
                    effect.Timer -= 5.f;
                    vec3_t gold, yellow;
                    Vector(1.f, 0.62f, 0.08f, gold);
                    Vector(1.f, 0.90f, 0.20f, yellow);
                    CreateParticle(kFireHik01GoldBitmap, effect.Position,
                        effect.Angle, gold, 0, 1.4f, &effect);
                    CreateParticle(kFlare01YellowBitmap, effect.Position,
                        effect.Angle, yellow, 0, 0.8f, &effect);
                }
            }
            break;
        default:
            break;
        }
        return;
    }

    if ((effect.Type == kSwordInertiaModel ||
         effect.Type == kSwordInertiaAuraModel) && owner)
    {
        VectorCopy(owner->Position, effect.Position);
        VectorCopy(owner->Angle, effect.Angle);
        effect.Angle[2] += 18.f * animationFactor;
        const float normalized = Clamp01(elapsed / initialLife);
        effect.Alpha = normalized < 0.9f ? 1.f : Clamp01((1.f - normalized) * 10.f);
    }
    else if (effect.Type == kPierceImpactModel ||
             effect.Type == kPierceSwordLineModel)
    {
        AttachToTarget(effect);
        effect.Angle[2] += 18.f * animationFactor;
    }
    else if (effect.Type == kBatFlockModel)
    {
        AttachToTarget(effect);
        effect.Angle[2] += 8.f * animationFactor;
    }
    else if (effect.Type == kBatFlockTrailModel)
    {
        const float halfLife = initialLife * 0.5f;
        const float delta = 1.f / (halfLife > 0.f ? halfLife : 1.f);
        effect.Scale += (effect.LifeTime > halfLife ? delta : -delta) *
            animationFactor;
        effect.Position[1] += (effect.Position[1] >= effect.StartPosition[1] ?
            -1.f : 1.f) * animationFactor;
    }
    else if (effect.Type == kDetectionImpactModel)
    {
        const float halfLife = initialLife * 0.5f;
        const float delta = (effect.SubType == 1 ? 0.5f : 1.f) /
            (halfLife > 0.f ? halfLife : 1.f);
        effect.Scale += (effect.LifeTime > halfLife ? delta : -delta) *
            animationFactor;
        if (effect.SubType == 1)
            effect.Angle[0] += 0.02f * animationFactor;
    }
    else if (effect.Type == kDetectionMarkModel)
    {
        if (effect.SubType == 0)
        {
            const float quarter = initialLife * 0.25f;
            if (effect.LifeTime > initialLife - quarter)
                effect.Scale += animationFactor / quarter;
            else if (effect.LifeTime < quarter)
                effect.Scale -= animationFactor / quarter;
            effect.Angle[0] += 0.025f * animationFactor;
            effect.Position[2] += (effect.Position[2] >= effect.StartPosition[2] ?
                -3.f : 3.f) * animationFactor;
        }
        else
        {
            const float half = initialLife * 0.5f;
            effect.Scale += (effect.LifeTime > half ? 1.f : -1.f) *
                animationFactor / half;
        }
    }
    effect.AnimationFrame += effect.Velocity * animationFactor;
    BMD& model = Models[effect.Type];
    if (model.NumActions > 0 && effect.AnimationFrame >=
        model.Actions[0].NumAnimationKeys)
        effect.AnimationFrame = 0.f;
#else
    (void)effect;
    (void)animationFactor;
#endif
}

bool RenderEffect(OBJECT& effect)
{
#ifdef RISE_SLAYER_PORT
    if (IsController(effect.Type))
        return true;
    if (!IsKnownModel(effect.Type) || !EnsureModel(effect.Type) ||
        !Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    // This isolated Pierce carrier has no triangles to submit when its BMD is
    // zero-mesh; no direct S21 child relationship is assumed here.
    if (model.NumMeshs == 0)
        return true;
    model.BodyLight[0] = effect.Light[0] * effect.Alpha;
    model.BodyLight[1] = effect.Light[1] * effect.Alpha;
    model.BodyLight[2] = effect.Light[2] * effect.Alpha;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        // The solid first mesh keeps its authored texture lighting; overlay
        // meshes and one-mesh aura/bat/mark assets use the additive path.
        const bool overlayOnly = effect.Type == kSwordInertiaAuraModel ||
            effect.Type == kBatFlockModel ||
            effect.Type == kDetectionMarkModel;
        const int flags = RENDER_TEXTURE |
            ((overlayOnly || mesh > 0) ? RENDER_BRIGHT : 0);
        model.RenderMesh(mesh, flags, effect.Alpha,
            effect.BlendMesh, effect.BlendMeshLight,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
    }
#ifdef RISE_SLAYER_RUNTIME_QA
    LogModelRender(effect, model);
#endif
    return true;
#else
    (void)effect;
    return false;
#endif
}

bool ApplyCastAction(OBJECT& actor, int skillId)
{
#ifdef RISE_SLAYER_PORT
    int action = -1;
    switch (skillId)
    {
    case kSwordInertia: action = kSwordInertiaAction; break;
    case kBatFlock: action = kBatFlockAction; break;
    case kPierceAttack: action = kPierceAttackAction; break;
    case kDetection: action = kDetectionAction; break;
    case kDemolish: action = kDemolishAction; break;
    default: return false;
    }
    if (!Models || Models[MODEL_PLAYER].NumActions <= action)
        return false;
    actor.CurrentAction = action;
    actor.AnimationFrame = 0.f;
    actor.PriorAnimationFrame = 0.f;
    actor.Velocity = 0.35f;
    return true;
#else
    (void)actor;
    (void)skillId;
    return false;
#endif
}

void LoadSounds()
{
#ifdef RISE_SLAYER_PORT
    Bitmaps.LoadImageFile(kMagicGround12Bitmap,
        "Data\\RISE\\Slayer\\Effect\\magic_ground12.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kRingOfGradation2Bitmap,
        "Data\\RISE\\Slayer\\Effect\\ring_of_gradation2.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kPinStar03Bitmap,
        "Data\\RISE\\Slayer\\Effect\\pin_star03.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kFlare01RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\flare01_red.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kEnemyRing01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\enemy_ring01.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kGhostMark02RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\gostmark02_red.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kSmoke01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smoke01.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kSmokeLines01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smokelines01.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kSmokeLines02Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smokelines02.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kSmokeLines03Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smokelines03.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kPinStar02RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\pin_star02_red.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kFireHik01MagentaBitmap,
        "Data\\RISE\\Slayer\\Effect\\firehik01_magenta.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kEmpact01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\empact01.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kGhostMark02Bitmap,
        "Data\\RISE\\Slayer\\Effect\\gostmark02.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kAlphaRingX256Bitmap,
        "Data\\RISE\\Slayer\\Effect\\alpha_RingX256_1.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kDamage1MonoBitmap,
        "Data\\RISE\\Slayer\\Effect\\Damage1mono.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kGroundStarBitmap,
        "Data\\RISE\\Slayer\\Effect\\ground_star.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kFireHik01GoldBitmap,
        "Data\\RISE\\Slayer\\Effect\\firehik01_gold.OZJ", GL_LINEAR, GL_CLAMP);
    Bitmaps.LoadImageFile(kFlare01YellowBitmap,
        "Data\\RISE\\Slayer\\Effect\\flare01_yellow.OZJ", GL_LINEAR, GL_CLAMP);
    LoadWaveFile(SOUND_SLAYER_SWORD_INERTIA_START,
        "Data\\RISE\\Slayer\\Sound\\SwordInertiastart.wav");
    LoadWaveFile(SOUND_SLAYER_SWORD_INERTIA_ATTACK,
        "Data\\RISE\\Slayer\\Sound\\SwordInertiaAttack.wav");
    LoadWaveFile(SOUND_SLAYER_BAT_FLOCK_START,
        "Data\\RISE\\Slayer\\Sound\\BatFlockstart.wav");
    LoadWaveFile(SOUND_SLAYER_BAT_FLOCK_ATTACK,
        "Data\\RISE\\Slayer\\Sound\\BatFlockAttack.wav");
    LoadWaveFile(SOUND_SLAYER_PIERCE_START,
        "Data\\RISE\\Slayer\\Sound\\FierceAttackstart.wav");
    LoadWaveFile(SOUND_SLAYER_PIERCE_ATTACK_1,
        "Data\\RISE\\Slayer\\Sound\\FierceAttack_Attack_1.wav");
    LoadWaveFile(SOUND_SLAYER_PIERCE_ATTACK_2,
        "Data\\RISE\\Slayer\\Sound\\FierceAttack_Attack_2.wav");
    LoadWaveFile(SOUND_SLAYER_PIERCE_ATTACK_3,
        "Data\\RISE\\Slayer\\Sound\\FierceAttack_Attack_3.wav");
    LoadWaveFile(SOUND_SLAYER_DETECTION,
        "Data\\RISE\\Slayer\\Sound\\Detection_3.wav");
#endif
}

void PlayCastSound(int skillId, OBJECT* actor)
{
#ifdef RISE_SLAYER_PORT
    const int sound = skillId == kSwordInertia ? SOUND_SLAYER_SWORD_INERTIA_START :
        skillId == kBatFlock ? SOUND_SLAYER_BAT_FLOCK_START :
        skillId == kPierceAttack ? SOUND_SLAYER_PIERCE_START :
        (skillId == kDetection || skillId == kDemolish) ?
            SOUND_SLAYER_DETECTION : -1;
    if (sound >= 0)
        PlayBuffer(sound, actor);
#else
    (void)skillId; (void)actor;
#endif
}

void PlayImpactSound(int skillId, OBJECT* actor, int ordinal)
{
#ifdef RISE_SLAYER_PORT
    int sound = -1;
    if (skillId == kSwordInertia) sound = SOUND_SLAYER_SWORD_INERTIA_ATTACK;
    else if (skillId == kBatFlock) sound = SOUND_SLAYER_BAT_FLOCK_ATTACK;
    else if (skillId == kPierceAttack)
        sound = SOUND_SLAYER_PIERCE_ATTACK_1 + (ordinal % 3);
    if (sound >= 0)
        PlayBuffer(sound, actor);
#else
    (void)skillId; (void)actor; (void)ordinal;
#endif
}

void ApplySkillCatalog()
{
    if (!SkillAttribute)
        return;

    static_assert(MAX_SKILLS > kDemolish,
        "5.2 skill capacity must contain the Slayer rows");

    for (unsigned i = 0; i < SkillSeedCount(); ++i)
    {
        const SkillSeed& seed = kSkillSeeds[i];
        SKILL_ATTRIBUTE& value = SkillAttribute[seed.id];
        ZeroMemory(&value, sizeof(value));
        strcpy_s(value.Name, seed.name);
        value.Level = static_cast<WORD>(seed.skillListLevel);
        value.Damage = static_cast<WORD>(seed.damage < 0 ? 0 : seed.damage);
        value.Mana = static_cast<WORD>(seed.mana);
        value.AbilityGuage = static_cast<WORD>(seed.bp);
        value.Distance = static_cast<DWORD>(seed.distance);
        value.Delay = seed.delayMs;
        value.Energy = 0;
        value.Charisma = 0;
        value.MasteryType = 255;
        value.SkillUseType = static_cast<BYTE>(seed.useType);
        value.SkillBrand = 0;
        value.KillCount = 0;
        value.SkillRank = 0;
        value.SkillGroup = 0;
        value.TypeSkill = static_cast<BYTE>(seed.type);
        value.Strength = seed.requiredStrength;
        value.Dexterity = seed.requiredDexterity;
        value.ItemSkill = 0;
        value.IsDamage = static_cast<BYTE>((seed.flags & kMarksMiniMap) != 0 ? 0 : 1);
        value.Magic_Icon = static_cast<WORD>(seed.icon);

        // RequireClass[] is intentionally untouched.  Legacy 5.2 exposes
        // only seven class bytes; writing Slayer's S21 class number 9 here
        // would corrupt the adjacent SkillRank/metadata ABI.
    }
}

}}
