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
    // 0x679 resolves resource 0xE0.  The S21-exclusive Slayer sword family
    // provides the renderable sword plus its one-mesh ring/aura child.
    {kSwordInertiaModel, "ak_skill_sword.bmd"},
    {kSwordInertiaAuraModel, "ak_skill_sword_s01.bmd"},
    {kBatFlockModel, "Bat_van01.bmd"},
    // 0x682 fans out to the registered 0x688 model in S21.
    {kBatFlockTrailModel, "van_object02_skill.bmd"},
    // 0x68A and 0x692 both reach the registered 0x694 impact model.
    {kPierceImpactModel, "Van_object04_skill.bmd"},
    // 0x692 reaches 0x693, whose adjacent registered model is 0x691.
    {kDetectionMarkModel, "van_object03_skill.bmd"},
    {kDetectionImpactModel, "Van_object04_skill.bmd"}
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
unsigned gRenderSamples[7] = {};

const char* SkillNameForModel(int modelId)
{
    switch (modelId)
    {
    case kSwordInertiaModel: return "Sword Inertia";
    case kSwordInertiaAuraModel: return "Sword Inertia";
    case kBatFlockModel: return "Bat Flock";
    case kBatFlockTrailModel: return "Bat Flock";
    case kPierceImpactModel: return "Pierce Attack";
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
    if (model.NumMeshs > 0 && model.NumBones > 0 && model.NumActions > 0)
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
    return model.NumMeshs > 0 && model.NumBones > 0 && model.NumActions > 0;
#else
    (void)modelId;
    return false;
#endif
}

void InitializeEffect(OBJECT& effect)
{
#ifdef RISE_SLAYER_PORT
    if (!IsKnownModel(effect.Type))
        return;
    effect.CurrentAction = 0;
    effect.AnimationFrame = 0.f;
    effect.PriorAnimationFrame = 0.f;
    effect.Velocity = 0.4f;
    effect.StartPosition[0] = effect.Position[0];
    effect.StartPosition[1] = effect.Position[1];
    effect.StartPosition[2] = effect.Position[2];
    switch (effect.Type)
    {
    case kSwordInertiaModel:
    case kSwordInertiaAuraModel: effect.LifeTime = 99.f; break;
    case kBatFlockModel:
    case kBatFlockTrailModel: effect.LifeTime = 10.f; break;
    case kPierceImpactModel: effect.LifeTime = 30.f; break;
    case kDetectionMarkModel:
    case kDetectionImpactModel: effect.LifeTime = 30.f; break;
    default: effect.LifeTime = 20.f; break;
    }
    if (effect.Type == kDetectionMarkModel ||
        effect.Type == kDetectionImpactModel)
        effect.Scale = effect.Scale <= 0.f ? 1.f : effect.Scale;
#else
    (void)effect;
#endif
}

void UpdateEffect(OBJECT& effect, float animationFactor)
{
#ifdef RISE_SLAYER_PORT
    if (!IsKnownModel(effect.Type))
        return;
    OBJECT* owner = effect.Owner;
    OBJECT* target = ResolveTarget(effect.m_sTargetIndex);
    const float initialLife = (effect.Type == kSwordInertiaModel ||
        effect.Type == kSwordInertiaAuraModel) ? 99.f :
        ((effect.Type == kBatFlockModel ||
          effect.Type == kBatFlockTrailModel) ? 10.f : 30.f);
    const float elapsed = initialLife - effect.LifeTime;
    if ((effect.Type == kSwordInertiaModel ||
         effect.Type == kSwordInertiaAuraModel) && owner)
    {
        const float normalized = Clamp01(elapsed / 99.f);
        const float travel = normalized <= 0.5f ? normalized * 2.f :
            (1.f - normalized) * 2.f;
        vec3_t endpoint;
        if (target && target != owner)
        {
            VectorCopy(target->Position, endpoint);
        }
        else
        {
            const float radians = (effect.Angle[2] - 90.f) * 0.01745329252f;
            endpoint[0] = effect.StartPosition[0] + cosf(radians) * 650.f;
            endpoint[1] = effect.StartPosition[1] + sinf(radians) * 650.f;
            endpoint[2] = effect.StartPosition[2];
        }
        const float fan = static_cast<float>(effect.SubType - 1) * 75.f;
        const float radians = effect.Angle[2] * 0.01745329252f;
        effect.Position[0] = effect.StartPosition[0] +
            (endpoint[0] - effect.StartPosition[0]) * travel + cosf(radians) * fan;
        effect.Position[1] = effect.StartPosition[1] +
            (endpoint[1] - effect.StartPosition[1]) * travel + sinf(radians) * fan;
        effect.Position[2] = effect.StartPosition[2] +
            (endpoint[2] - effect.StartPosition[2]) * travel + sinf(travel * 3.14159265f) * 35.f;
        effect.Angle[0] = normalized <= 0.5f ? 0.f : 180.f;
        effect.Angle[2] += 18.f * animationFactor;
        effect.Alpha = normalized < 0.9f ? 1.f : Clamp01((1.f - normalized) * 10.f);
    }
    else if (effect.Type == kPierceImpactModel)
    {
        AttachToTarget(effect);
        effect.Angle[2] += 18.f * animationFactor;
    }
    else if (effect.Type == kBatFlockModel ||
             effect.Type == kBatFlockTrailModel)
    {
        AttachToTarget(effect);
        effect.Angle[2] += 8.f * animationFactor;
    }
    else if ((effect.Type == kDetectionMarkModel ||
              effect.Type == kDetectionImpactModel) && owner)
    {
        VectorCopy(owner->Position, effect.Position);
        effect.Scale += 0.035f * animationFactor;
        effect.Alpha = Clamp01(effect.LifeTime / 24.f);
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
    if (!IsKnownModel(effect.Type) || !EnsureModel(effect.Type) ||
        !Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    model.BodyLight[0] = effect.Light[0] * effect.Alpha;
    model.BodyLight[1] = effect.Light[1] * effect.Alpha;
    model.BodyLight[2] = effect.Light[2] * effect.Alpha;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
        model.RenderMesh(mesh, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
            effect.BlendMesh, effect.BlendMeshLight,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
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
        skillId == kDetection ? SOUND_SLAYER_DETECTION : -1;
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

    static_assert(MAX_SKILLS > kDetection,
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
