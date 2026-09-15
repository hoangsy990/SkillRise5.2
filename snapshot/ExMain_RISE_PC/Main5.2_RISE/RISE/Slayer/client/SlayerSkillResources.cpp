#include "stdafx.h"
#include "SlayerSkillResources.h"

#include "ZzzBMD.h"
#include "ZzzAI.h"
#include "ZzzEffect.h"
#include "ZzzLodTerrain.h"
#include "ZzzCharacter.h"
#include "DSPlaySound.h"
#include "../../../ZzzInfomation.h"
#include "../../../ZzzObject.h"
#include "../../../ZzzTexture.h"
#include "../../../GMHellas.h"
#include "../../../MapManager.h"
#include "../../../wsclientinline.h"
#include "../shared/SlayerSkillContractData.h"
#include "../shared/SlayerPierceFanoutWire.h"
#ifdef RISE_SLAYER_RUNTIME_QA
#include "../../SlayerRuntimeQA.h"
#endif

#include <algorithm>
#include <cmath>
#include <map>
#include <string>
#include <vector>

namespace rise { namespace slayer {

namespace {

// 5.2 OBJECT has no S21 vector field. Keep list-bearing supplemental effects
// in a private, bounded sidecar and erase a slot whenever CreateEffect reuses
// it; the canonical cast roots therefore remain genuinely list-free.
std::map<const OBJECT*, std::vector<short> > gBatFlockTargets;
std::map<const OBJECT*, std::vector<short> > gPierceTargets;
std::map<const OBJECT*, unsigned char> gPierceCastSerial;
std::map<const OBJECT*, short> gPierceCastTargetIndex;
unsigned char gPierceLaneDirection = 0;

float NativeRandomUnitStep(int lower, int upper)
{
    // S21 0x1267C3C truncates bounds to unit steps and takes one random
    // integer modulo (upper - lower + 1). The 5.2 private RNG need not
    // reproduce the protected PRNG seed to retain these discrete ranges.
    return static_cast<float>(lower + rand() % (upper - lower + 1));
}

struct ModelRow
{
    int id;
    const char* filename;
};

const char* const kEffectDirectory = "Data\\RISE\\Slayer\\Effect\\";

const ModelRow kModels[] = {
    // Native 0x678. It is shared by Bat Flock and the two buff child roots.
    {kBatFlockModel, "Bat_van01.bmd"},
    // Native model 0x688, created by root 0x682.
    {kBatFlockTrailModel, "van_object02_skill.bmd"},
    // Native models 0x691 and 0x694.
    {kDetectionMarkModel, "van_object03_skill.bmd"},
    {kDetectionImpactModel, "Van_object04_skill.bmd"},
    // Native 0x5D8 child from Pierce's action-start 0x81CD subtype 2.
    {kPierceMarksCylinderModel, "marks_cylinder.bmd"},
    // Native model 0x681, created by Pierce 0x679 and Sword child 0x68B.
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
    return (type >= kSwordInertiaController && type <= kLastEffect) ||
        type == kPierce81CDController;
}

bool IsBitmapEffect(int type)
{
    return (type >= kFlare01RedEffect && type <= kFlareEffect) ||
        type == kPierce81CEEffect || type == kPierce80BAEffect ||
        type == kPierce8149Effect || type == kPierceShockWaveEffect;
}

float InitialLife(const OBJECT& effect)
{
    switch (effect.Type)
    {
    case kSwordInertiaController:
        return 30.f;
    case kSwordSecondaryController:
    case kPierceController: return 99.f;
    case kDemolishController:
    case kDemolishChildController: return 30.f;
    case kBatFlockModel: return effect.SubType == 0 ? 40.f :
        (effect.SubType == 3 ? 10.f : 50.f);
    case kBatFlockTrailModel: return 20.f;
    case kPierceMarksCylinderModel: return 30.f;
    case kBatFlockController:
    case kBatFlockTargetController:
    case kBatFlockDotController: return 10.f;
    case kBatFlockOrbitController: return 60.f;
    case kBatFlock686Controller: return effect.SubType == 0 ? 10.f : 70.f;
    case kBatFlock687Controller: return 9.f;
    case kPierce67AController:
    case kPierceSwordLineModel:
    case kSword68BController:
    case kSword68DController: return 30.f;
    case kPierce67BController:
    case kPierce67EController:
    case kSword690Controller: return 10.f;
    case kPierce67CController:
    case kSword68EController: return 1.f;
    case kPierce67DController:
    case kSword68FController: return 3.f;
    case kPierce680Controller: return 5.f;
    case kSword68CController: return 4.f;
    case kDetectionImpactModel: return effect.SubType == 1 ? 35.f : 40.f;
    case kDetectionMarkModel: return effect.SubType == 1 ? 40.f : 35.f;
    case kDetectionController:
    case kDetectionChildController: return 30.f;
    case kFlare01RedEffect:
    case kRingOfGradation2Effect: return 30.f;
    case kEnemyRing01Effect: return 40.f;
    case kMagicGround12Effect: return effect.SubType == 2 ? 15.f : 30.f;
    case kPierce81CDController: return 30.f;
    case kPierce81CEEffect: return effect.SubType == 4 ? 20.f : 30.f;
    case kPierce80BAEffect: return 50.f;
    case kPierce8149Effect: return 50.f;
    case kPierceShockWaveEffect: return 15.f;
    case kFlareBlueEffect:
    case kFlareEffect: return 30.f;
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

void SpawnBitmapChild(int type, const OBJECT& source, OBJECT* owner,
    const vec3_t light, int subtype, float scale)
{
    vec3_t position, angle, color;
    VectorCopy(source.Position, position);
    VectorCopy(source.Angle, angle);
    VectorCopy(light, color);
    CreateEffect(type, position, angle, color, subtype, owner, -1,
        source.Skill, 0, 0, scale, source.m_sTargetIndex);
}

OBJECT* ResolveTarget(short targetIndex)
{
    if (targetIndex < 0 || targetIndex >= MAX_CHARACTERS_CLIENT)
        return 0;
    OBJECT* target = &CharactersClient[targetIndex].Object;
    return target->Live ? target : 0;
}

short PickBatFlockTargetIndex(const OBJECT& effect)
{
    std::map<const OBJECT*, std::vector<short> >::const_iterator it =
        gBatFlockTargets.find(&effect);
    if (it == gBatFlockTargets.end() || it->second.empty())
        return -1;
    return it->second[static_cast<unsigned>(rand()) % it->second.size()];
}

OBJECT* PickBatFlockTarget(const OBJECT& effect)
{
    return ResolveTarget(PickBatFlockTargetIndex(effect));
}

short PierceTargetIndexAt(const OBJECT& effect, int ordinal)
{
    std::map<const OBJECT*, std::vector<short> >::const_iterator it =
        gPierceTargets.find(&effect);
    if (it == gPierceTargets.end() || ordinal < 0 ||
        static_cast<unsigned>(ordinal) >= it->second.size())
        return -1;
    return it->second[ordinal];
}

OBJECT* PierceTargetAt(const OBJECT& effect, int ordinal)
{
    return ResolveTarget(PierceTargetIndexAt(effect, ordinal));
}

bool AdvanceSword68BTargetAnimation(OBJECT& effect, OBJECT* target)
{
    // S21 0x1417693 selects the resolved target's BMD and calls
    // 0x132D0CD using its action-0 PlaySpeed. The allocator initializes
    // the effect action/frame to zero at 0x143E7A8/0x143E92D. Use the
    // native 5.2 BMD animation API rather than an invented tick deadline.
    if (!target || !Models || target->Type < 0 ||
        target->Type >= MAX_MODELS)
        return false;
    BMD& model = Models[target->Type];
    if (model.NumActions <= 0 || !model.Actions ||
        effect.CurrentAction < 0 || effect.CurrentAction >= model.NumActions)
        return false;
    const unsigned short savedAction = model.CurrentAction;
    model.CurrentAction = static_cast<unsigned short>(effect.CurrentAction);
    const float speed = model.Actions[model.CurrentAction].PlaySpeed;
    const bool playing = model.PlayAnimation(&effect.AnimationFrame,
        &effect.PriorAnimationFrame, &effect.PriorAction, speed,
        effect.Position, effect.Angle);
    model.CurrentAction = savedAction;
    return playing;
}

int PierceTargetCount(const OBJECT& effect)
{
    std::map<const OBJECT*, std::vector<short> >::const_iterator it =
        gPierceTargets.find(&effect);
    return it == gPierceTargets.end() ? 0 :
        static_cast<int>(it->second.size());
}

bool SendPierceLaneRequest(const OBJECT& effect, short targetSlot,
    const OBJECT& target)
{
    // Native 0x15464BE..0x15466A0 sends once per 0x689 lane only from the
    // local caster. In 5.2 head 0x00 is chat, so use the private F4:E6
    // route and let GS validate the serial/authorized target before damage.
    if (!Hero || effect.Owner != &Hero->Object ||
        !CharactersClient || targetSlot < 0 ||
        targetSlot >= MAX_CHARACTERS_CLIENT)
        return false;
    const auto serial = gPierceCastSerial.find(&effect);
    if (serial == gPierceCastSerial.end())
        return false;
    const int tileX = static_cast<int>(target.Position[0] / 100.f);
    const int tileY = static_cast<int>(target.Position[1] / 100.f);
    if (tileX < 0 || tileX > 255 || tileY < 0 || tileY > 255)
        return false;
    const int targetKey = CharactersClient[targetSlot].Key & 0x7FFF;
    // S21 helper 0x173EA5C advances this packet byte from 1 through 50.
    // It is an outbound sequence byte, not the caster's world yaw.
    gPierceLaneDirection = static_cast<unsigned char>(
        gPierceLaneDirection >= 50 ? 1 : gPierceLaneDirection + 1);
    CStreamPacketEngine packet;
    packet.Init(0xC1, kPierceFanoutHead);
    packet << static_cast<BYTE>(kPierceLaneRequestSub)
        << static_cast<BYTE>(kPierceAttack >> 8)
        << static_cast<BYTE>(kPierceAttack & 0xFF)
        << static_cast<BYTE>(serial->second)
        << static_cast<BYTE>(targetKey >> 8)
        << static_cast<BYTE>(targetKey & 0xFF)
        << static_cast<BYTE>(tileX)
        << static_cast<BYTE>(tileY)
        << static_cast<BYTE>(gPierceLaneDirection);
    if (packet.GetSize() != sizeof(PierceLaneRequestWire))
        return false;
    packet.Send();
    return true;
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

void OffsetByYaw(OBJECT& effect, float yawOffset, float distance,
    float height)
{
    const float yaw = (effect.Angle[2] + yawOffset) * 0.01745329252f;
    effect.Position[0] += -sinf(yaw) * distance;
    effect.Position[1] += cosf(yaw) * distance;
    effect.Position[2] += height;
}

float VectorDistance3(const vec3_t first, const vec3_t second)
{
    const float x = first[0] - second[0];
    const float y = first[1] - second[1];
    const float z = first[2] - second[2];
    return sqrtf(x * x + y * y + z * z);
}

void InitializeSwordFlight(OBJECT& effect, float distanceScale,
    float rangeScale)
{
    VectorCopy(effect.Position, effect.StartPosition);
    vec34_t matrix;
    AngleMatrix(effect.Angle, matrix);
    effect.Direction[0] = matrix[0][0];
    effect.Direction[1] = matrix[0][1];
    effect.Direction[2] = matrix[0][2];

    vec3_t destination;
    if (effect.Owner)
    {
        VectorCopy(effect.Owner->Position, destination);
    }
    else
    {
        VectorCopy(effect.Position, destination);
    }
    destination[0] += effect.Direction[0] * distanceScale;
    destination[1] += effect.Direction[1] * distanceScale;
    destination[2] += effect.Direction[2] * distanceScale;
    effect.Distance = VectorDistance3(effect.Position, destination) *
        rangeScale;
}

void AdvanceSwordFlight(OBJECT& effect, float animationFactor)
{
    const float divisor = InitialLife(effect) + 1.f;
    const float step = effect.Distance / divisor * animationFactor;
    effect.Position[0] += effect.Direction[0] * step;
    effect.Position[1] += effect.Direction[1] * step;
    effect.Position[2] += effect.Direction[2] * step;
    if (VectorDistance3(effect.Position, effect.StartPosition) >
        effect.Distance)
        effect.LifeTime = 0.f;
}

void DirectionFromAngle(OBJECT& effect)
{
    vec34_t matrix;
    AngleMatrix(effect.Angle, matrix);
    effect.Direction[0] = matrix[0][0];
    effect.Direction[1] = matrix[0][1];
    effect.Direction[2] = matrix[0][2];
}

void RandomBatSpread(vec3_t spread, bool upwardZ)
{
    // S21 0x683/0x684: random X/Z direction (Y stays zero), normalized,
    // then each axis independently scaled by [0,160]. 0x684's Z seed
    // alone uses [0,100] instead of [-100,100].
    // Native emits the X sample first, then Z. Vector is an inline
    // function in 5.2; nested call arguments would be evaluated in the
    // compiler's own order and consume the two RNG draws backwards.
    const float x = NativeRandomUnitStep(-100, 100);
    const float z = NativeRandomUnitStep(upwardZ ? 0 : -100, 100);
    Vector(x, 0.f, z, spread);
    VectorNormalize(spread);
    for (int axis = 0; axis < 3; ++axis)
        spread[axis] *= NativeRandomUnitStep(0, 160);
}

void EmitBatTargetJoint(OBJECT& effect, OBJECT* target)
{
    // S21 0x154435F..0x154476C: localized red-joint emitter.
    vec3_t spread;
    RandomBatSpread(spread, false);
    vec3_t position, angle;
    VectorAdd(effect.Position, spread, position);
    position[2] += 90.f;
    VectorCopy(effect.Angle, angle);
    angle[2] += NativeRandomUnitStep(-15, 15);
    CreateJoint(kGhostMark02RedBitmap, position, position, angle, 1,
        target, 20.f, -1, 0, 0, -1, 0, -1);
}

void EmitBatOrbitChild(OBJECT& effect)
{
    // S21 0x1544771..0x1544ADA emits 0x685 from a spread root position,
    // elevated 90 Z, with root light, null owner and zero incoming scale.
    vec3_t spread;
    RandomBatSpread(spread, true);
    OBJECT pulse = effect;
    VectorAdd(effect.Position, spread, pulse.Position);
    pulse.Position[2] += 90.f;
    pulse.Angle[2] += NativeRandomUnitStep(-15, 15);
    SpawnChild(kBatFlockOrbitController, pulse, 0, 0, 0.f);
}

void EmitBatMainPulse(OBJECT& effect)
{
    // S21 0x1543C4C..0x15442B6: rotate a randomized X/Z spread by the
    // root angle, then lift the authored model/particle launch by 90 Z.
    vec3_t spread;
    const float x = NativeRandomUnitStep(-100, 100);
    const float z = NativeRandomUnitStep(-100, 100);
    Vector(x, 0.f, z, spread);
    VectorNormalize(spread);
    for (int axis = 0; axis < 3; ++axis)
        spread[axis] *= NativeRandomUnitStep(40, 90);
    vec34_t matrix;
    AngleMatrix(effect.Angle, matrix);
    vec3_t rotated;
    VectorRotate(spread, matrix, rotated);
    OBJECT pulse = effect;
    VectorAdd(effect.Position, rotated, pulse.Position);
    pulse.Position[2] += 90.f;
    pulse.Angle[2] += NativeRandomUnitStep(-15, 15);
    Vector(1.f, 1.f, 1.f, pulse.Light);
    // Native 0x678 child owner is the immediate 0x682 root, not caster.
    // S21 selects independently from this 0x682 root's list for each pulse.
    // A canonical cast root with an empty list passes -1 to the child.
    pulse.m_sTargetIndex = PickBatFlockTargetIndex(effect);
    SpawnChild(kBatFlockModel, pulse, &effect, 2, 2.5f);

    const float particleScale = NativeRandomUnitStep(80, 100) * 0.01f;
    vec3_t pinLight, impactLight;
    Vector(0.5f, 0.5f, 0.5f, pinLight);
    Vector(0.65f, 0.65f, 0.65f, impactLight);
    // Native particle allocator takes the root angle, not the perturbed
    // model angle, for both 0x810B and 0x8070 records.
    CreateParticle(kPinStar02RedBitmap, pulse.Position, effect.Angle,
        pinLight, 0, 2.f * particleScale, &effect);
    CreateParticle(kEmpact01Bitmap, pulse.Position, effect.Angle,
        impactLight, 0, 0.26f * particleScale, &effect);
}

#ifdef RISE_SLAYER_RUNTIME_QA
unsigned gRenderSamples[8] = {};

const char* SkillNameForModel(const OBJECT& effect)
{
    switch (effect.Type)
    {
    case kBatFlockModel: return "Bat Flock";
    case kBatFlockTrailModel: return "Bat Flock";
    case kPierceSwordLineModel: return "Pierce Attack";
    case kDetectionMarkModel:
    case kDetectionImpactModel:
        return effect.Skill == kDemolish ? "Demolish" : "Detection";
    default: return "unknown";
    }
}

void LogModelRender(const OBJECT& effect, const BMD& model, int renderFlags)
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
        "ownerLive=%d modelReady=1 meshes=%d bones=%d actions=%d pass=%s",
        SkillNameForModel(effect), static_cast<int>(effect.Skill),
        effect.Type, effect.SubType, model.Name, effect.Live ? 1 : 0,
        effect.Visible ? 1 : 0, static_cast<double>(effect.LifeTime),
        static_cast<double>(effect.Alpha), static_cast<double>(effect.Scale),
        effect.Owner && effect.Owner->Live ? 1 : 0, model.NumMeshs,
        model.NumBones, model.NumActions,
        (renderFlags & RENDER_BRIGHT) ? "additive" :
            (effect.Alpha < 0.99f ? "textured-alpha" : "textured-opaque"));
    rise::slayerqa::AppendRuntimeQALog(line);
}
#endif

}

void SetBatFlockTargets(OBJECT& effect, const short* targetIndexes, int count)
{
    std::vector<short>& list = gBatFlockTargets[&effect];
    list.clear();
    if (!targetIndexes || count <= 0)
        return;
    for (int i = 0; i < count && i < 10; ++i)
    {
        const short targetIndex = targetIndexes[i];
        if (targetIndex >= 0 &&
            std::find(list.begin(), list.end(), targetIndex) == list.end())
            list.push_back(targetIndex);
    }
}

void SetPierceTargets(OBJECT& effect, const short* targetIndexes, int count,
    unsigned char castSerial)
{
    std::vector<short>& list = gPierceTargets[&effect];
    list.clear();
    gPierceCastSerial.erase(&effect);
    gPierceCastTargetIndex.erase(&effect);
    if (!targetIndexes || count <= 0)
        return;
    gPierceCastSerial[&effect] = castSerial;
    for (int i = 0; i < count && i < 10; ++i)
    {
        const short index = targetIndexes[i];
        if (index >= 0 &&
            std::find(list.begin(), list.end(), index) == list.end())
            list.push_back(index);
    }
    if (!list.empty())
        gPierceCastTargetIndex[&effect] = list.front();
}

bool IsEffectType(int type)
{
#ifdef RISE_SLAYER_PORT
    return IsKnownModel(type) || IsController(type) || IsBitmapEffect(type);
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
        if (modelId == kDetectionMarkModel)
        {
            // Only the S21 0x691 silver-mark JPEG carries the black color
            // key. Convert its loaded private texture to RGBA in memory;
            // leave the BMD and OZJ files byte-for-byte intact.
            if (_stricmp(model.Textures[mesh].FileName,
                    "Elite_monster_ground02.JPG") != 0 ||
                !Bitmaps.ApplySlayerBlackKeyAlpha(model.IndexTexture[mesh]))
            {
                model.Release();
                return false;
            }
        }
        else if (modelId == kDetectionImpactModel)
        {
            // The S21 0x694 model has two RGB ring materials with authored
            // black fields. Key only these imported private textures, so
            // 5.2's ordinary textured-alpha pass can blend the empty field.
            const char* material = model.Textures[mesh].FileName;
            const bool isArk = _stricmp(material, "ark.JPG") == 0;
            const bool isEmpact = _stricmp(material, "empact01.JPG") == 0;
            if ((!isArk && !isEmpact) ||
                !Bitmaps.ApplySlayerBlackKeyAlpha(model.IndexTexture[mesh],
                    isArk ? 48 : 16))
            {
                model.Release();
                return false;
            }
        }
    }
    return model.NumBones > 0 && model.NumActions > 0;
#else
    (void)modelId;
    return false;
#endif
}

void InitializeEffect(OBJECT& effect, float incomingScale)
{
    // Effect-pool pointers are recycled; never inherit a previous cast's list.
    gBatFlockTargets.erase(&effect);
    gPierceTargets.erase(&effect);
    gPierceCastSerial.erase(&effect);
    gPierceCastTargetIndex.erase(&effect);
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
    if (IsBitmapEffect(effect.Type))
        effect.Alpha = effect.Type == kMagicGround12Effect ? 1.f : 0.f;
    // Native CreateEffect first normalizes non-positive incoming scale to
    // 0.9, but some subtype initializers then overwrite OBJECT+0xA0 with
    // the original argument. Keep both values separate for those branches.
    // Root creation is kept on the controller initializer. Every child below
    // corresponds to a decoded S21 secondary-pool code or bitmap id.
    switch (effect.Type)
    {
    case kFlareBlueEffect:
        if (effect.SubType == 2)
        {
            // S21 0x1481E34..0x1481E91: 30 ticks, original child scale,
            // zero Angle Z and alpha. The high-code updater below makes it
            // visible; leaving this node at alpha zero hid Detection's flare.
            effect.Scale = incomingScale;
            effect.Angle[2] = 0.f;
            effect.Alpha = 0.f;
        }
        break;
    case kPierce81CDController:
    {
        // Native 0x147D373 subtype 2 creates its children in call order.
        // All ten direct children now follow their native call order;
        // the subtype-7 particle fanout is emitted by their update path.
        if (effect.SubType != 2 || !effect.Owner)
        {
            effect.LifeTime = 0.f;
            break;
        }
        vec3_t mark;
        Vector(1.f, 0.4f, 0.95f, mark);
        for (int subtype = 3; subtype <= 5; ++subtype)
            SpawnBitmapChild(kPierce81CEEffect, effect, effect.Owner,
                mark, subtype, 0.f);
        vec3_t flare;
        Vector(0.2f, 0.f, 1.f, flare);
        SpawnBitmapChild(kPierce80BAEffect, effect, effect.Owner,
            flare, 6, 7.f);
        vec3_t star;
        Vector(0.9f, 0.f, 1.f, star);
        SpawnBitmapChild(kPierce8149Effect, effect, effect.Owner,
            star, 2, 1.7f);
        vec3_t ground;
        Vector(0.8f, 0.6f, 1.f, ground);
        SpawnBitmapChild(kMagicGround12Effect, effect, effect.Owner,
            ground, 2, 0.f);
        OBJECT cylinder = effect;
        Vector(0.8f, 0.5f, 1.f, cylinder.Light);
        SpawnChild(kPierceMarksCylinderModel, cylinder, effect.Owner, 1, 0.f);
        // S21 0x147D9D3..0x147DEBF: three subtype-7 flares use a fixed
        // 180-degree X rotation for their authored offsets, not actor yaw.
        // Each flare owns three particles per update in the separate pool.
        const float offsets[3][3] = {
            {-126.71f, 73.69f, 0.f},
            {134.72f, 84.32f, 0.f},
            {0.f, -144.92f, 0.f}
        };
        vec3_t turn, flareLight;
        float matrix[3][4];
        Vector(180.f, 0.f, 0.f, turn);
        Vector(0.38f, 0.2f, 1.f, flareLight);
        AngleMatrix(turn, matrix);
        for (int lane = 0; lane < 3; ++lane)
        {
            vec3_t offset, rotated;
            Vector(offsets[lane][0], offsets[lane][1], offsets[lane][2],
                offset);
            VectorRotate(offset, matrix, rotated);
            OBJECT flareLane = effect;
            for (int axis = 0; axis < 3; ++axis)
                flareLane.Position[axis] += rotated[axis];
            SpawnBitmapChild(kPierce80BAEffect, flareLane, effect.Owner,
                flareLight, 7, 2.f);
        }
        break;
    }
    case kPierce81CEEffect:
        // Native 0x147E153 subtypes 3/4/5 set 0xB4 to the current S21
        // millisecond clock. Their updater refreshes LifeTime every frame
        // until its elapsed-time cutoff, so an ordinary 30-tick effect dies
        // far too early in 5.2.
        effect.Timer = WorldTime;
        effect.Scale = effect.SubType == 3 ? 2.86f : 4.55f;
        effect.Alpha = 1.f;
        break;
    case kPierce80BAEffect:
        // S21 0x1471BDD: subtypes 6/7 copy the incoming scale and light,
        // start at life 50, and stamp the current millisecond clock.
        if (effect.SubType != 6 && effect.SubType != 7)
        {
            effect.LifeTime = 0.f;
            break;
        }
        effect.Timer = WorldTime;
        effect.Scale = incomingScale;
        effect.Alpha = 1.f;
        break;
    case kPierce8149Effect:
        // Native 0x147637E: subtype 2 snapshots the millisecond clock,
        // sets life 50 and alpha .9. The parent supplies scale 1.7.
        if (effect.SubType != 2)
        {
            effect.LifeTime = 0.f;
            break;
        }
        effect.Timer = WorldTime;
        effect.Scale = incomingScale;
        effect.Alpha = 0.9f;
        break;
    case kPierceShockWaveEffect:
        // S21 0x145CBEA: the nested 0x8012 subtype-17 child has no
        // owner, life 15, alpha 1 and an authored scale 4, regardless of
        // the zero incoming scale passed by 0x81CF subtype 2.
        if (effect.SubType != 17)
        {
            effect.LifeTime = 0.f;
            break;
        }
        effect.Scale = 4.f;
        effect.Alpha = 1.f;
        break;
    case kPierceMarksCylinderModel:
        // S21 0x147ED07: subtype 1 shares the thirty-tick model setup
        // with subtype 0, but only subtype 0 has a six-second refresh
        // updater. This direct Pierce child naturally expires at 30 ticks.
        if (effect.SubType != 1)
        {
            effect.LifeTime = 0.f;
            break;
        }
        effect.Scale = 1.f;
        effect.Alpha = 1.f;
        effect.Timer = WorldTime;
        break;
    case kPierceController:
        // Native 0x679 subtype zero initializes EFFECT+0xBC from player
        // action 0xE0 at 0x148EE0F..0x148EE29. Its update compares the
        // controller's own animation frame with 4 and 7, so the generic
        // .4 tick velocity shifts both authored flank emission times.
        if (!Models || Models[MODEL_PLAYER].NumActions <=
            kSwordInertiaAction)
        {
            effect.LifeTime = 0.f;
            break;
        }
        if (CharacterAttribute)
            ApplyPlayerActionSpeeds(CharacterAttribute->AttackSpeed);
        effect.Velocity = Models[MODEL_PLAYER].Actions[
            kSwordInertiaAction].PlaySpeed;
        break;
    case kPierceSwordLineModel:
    {
        // S21 0x148FFEE/0x149015B/0x14902C2 copies E0/E1/E2 action
        // speed for modes 0..2. Mode 3 at 0x1490304..0x14903E2 copies
        // its owner's current action speed instead. All four modes write
        // both swordline model action 0 and EFFECT+0xBC.
        if (!effect.Owner || !Models ||
            Models[MODEL_PLAYER].NumActions <= kSwordInertiaAction)
        {
            effect.LifeTime = 0.f;
            break;
        }
        if (CharacterAttribute)
            ApplyPlayerActionSpeeds(CharacterAttribute->AttackSpeed);
        const float attackSpeedTerm = CharacterAttribute ?
            CharacterAttribute->AttackSpeed * 0.002f : 0.f;
        if (effect.SubType == 0)
            effect.Velocity = Models[MODEL_PLAYER].Actions[
                kSwordInertiaAction].PlaySpeed;
        else if (effect.SubType == 1 || effect.SubType == 2)
            effect.Velocity = 0.40f + attackSpeedTerm;
        else if (effect.Owner->CurrentAction >= 0 &&
            effect.Owner->CurrentAction <
                Models[MODEL_PLAYER].NumActions)
            effect.Velocity = Models[MODEL_PLAYER].Actions[
                effect.Owner->CurrentAction].PlaySpeed;
        else
        {
            effect.LifeTime = 0.f;
            break;
        }
        if (EnsureModel(kPierceSwordLineModel))
            Models[kPierceSwordLineModel].Actions[0].PlaySpeed =
                effect.Velocity;
        else
        {
            effect.LifeTime = 0.f;
            break;
        }
        effect.Alpha = 1.f;
        // Native 0x681 modes 1/2 perturb Angle Y before the seven-key
        // blur animation; mode 0 and mode 3 keep the incoming angle.
        if (effect.SubType == 1 || effect.SubType == 2)
            effect.Angle[1] += (effect.SubType == 1 ? 140.f : 60.f) +
                static_cast<float>(rand()) * (20.f / RAND_MAX);
        break;
    }
    case kBatFlockModel:
    {
        // Native 0x678 uses the caller scale multiplied by a random .60..1.20
        // envelope for all five Slayer subtypes.
        effect.Scale *= NativeRandomUnitStep(60, 120) * 0.01f;
        effect.Alpha = 1.f;
        if (effect.SubType == 1 || effect.SubType == 4)
        {
            effect.Angle[0] = NativeRandomUnitStep(-90, 90);
            effect.Angle[1] = NativeRandomUnitStep(-90, 90);
            effect.Angle[2] = NativeRandomUnitStep(0, 360);
        }
        DirectionFromAngle(effect);
        switch (effect.SubType)
        {
        case 0: effect.Gravity = NativeRandomUnitStep(50, 100) * 0.05f; break;
        case 1: effect.Gravity = NativeRandomUnitStep(50, 100) * 0.025f; break;
        case 2: effect.Gravity = NativeRandomUnitStep(80, 100) * 0.5f; break;
        case 3:
            effect.Gravity = 30.f;
            if (effect.Owner)
                effect.Distance = VectorDistance3(effect.Position,
                    effect.Owner->Position);
            break;
        case 4: effect.Gravity = NativeRandomUnitStep(50, 100) * 0.025f; break;
        default: break;
        }
        if (effect.SubType == 1)
        {
            // Native 0x148E895..0x148E907 allocates the force_Pillar
            // joint-pool subtype 6, owned by this bat, at 40 * bat scale.
            CreateJoint(BITMAP_FORCEPILLAR, effect.Position,
                effect.Position, effect.Angle, 6, &effect,
                40.f * effect.Scale);
        }
        break;
    }
    case kBatFlockTrailModel:
        // Native 0x1490F57 copies incoming 0.85 to +0xA0 scale, then
        // 0x1490F6A clears xmm0 before writing zero to +0xDC alpha.
        // The faded trail must emerge from zero in the updater.
        if (effect.SubType == 0)
        {
            effect.Scale = incomingScale;
            effect.Alpha = 0.f;
        }
        break;
    case kSwordInertiaController:
    {
        // Native 0x68A initializer (0x1491051), including both subtypes.
        vec3_t flare, ring, white;
        Vector(0.6f, 0.6f, 0.6f, flare);
        Vector(effect.SubType == 0 ? 0.05f : 0.10f, 0.01f,
            effect.SubType == 0 ? 0.25f : 0.01f, ring);
        Vector(1.f, 1.f, 1.f, white);
        SpawnBitmapChild(kFlare01RedEffect, effect, 0, flare,
            0x0A, 2.5f);
        SpawnBitmapChild(kRingOfGradation2Effect, effect, 0, ring,
            0, 3.75f);
        if (effect.SubType == 0)
            SpawnBitmapChild(kEnemyRing01Effect, effect, 0, white,
                0, 1.2f);
        break;
    }
    case kSwordSecondaryController:
        // Native 0x689 only snapshots the source origin (+100 Z); its
        // subordinate graph is emitted by the per-frame case below.
        effect.StartPosition[2] += 100.f;
        break;
    case kBatFlockController:
        // Native 0x682 subtype 0 creates model 0x688; the packet-born
        // subtype 1 exits its initializer without this child/light override.
        if (effect.SubType == 0)
        {
            Vector(0.9f, 0.9f, 0.9f, effect.Light);
            SpawnChild(kBatFlockTrailModel, effect, effect.Owner, 0, 0.85f);
        }
        break;
    case kBatFlockOrbitController:
        // Native 0x685: 60 ticks, 40..50 scale and a signed 1..15 degree
        // step.  Gravity is private scratch state for that recovered step.
        effect.Scale = (80.f + static_cast<float>(rand() % 21)) * 0.5f;
        effect.Gravity = static_cast<float>((rand() % 31) - 15);
        if (effect.Gravity == 0.f)
            effect.Gravity = 1.f;
        break;
    case kBatFlock686Controller:
        // Native 0x686 loops exactly three times in both mode branches.
        for (int ordinal = 0; ordinal < 3; ++ordinal)
            SpawnChild(kBatFlockModel, effect, &effect, 3, 2.3f);
        break;
    case kSword68BController:
    {
        // Native 0x68B copies Position/Angle, raises Position Z by 100 and
        // rotates Angle Y by one of 0/180/90.  The earlier reconstruction
        // incorrectly moved Position X, which produced the detached line.
        OBJECT swordLine = effect;
        const int lane = rand() % 3;
        if (lane == 1)
            swordLine.Angle[1] += 180.f;
        else if (lane == 2)
            swordLine.Angle[1] += 90.f;
        swordLine.Position[2] += 100.f;
        SpawnChild(kPierceSwordLineModel, swordLine, &effect, 3, 1.2f);
        vec34_t matrix;
        AngleMatrix(effect.Angle, matrix);
        effect.Direction[0] = matrix[0][0];
        effect.Direction[1] = matrix[0][1];
        effect.Direction[2] = matrix[0][2];
        break;
    }
    case kSword68DController:
    {
        // Native 0x68D base branch (owner does not have S21 master skill
        // 0x13B): 82F5/.2, 82F8/.45 and 80BA/subtype1/scale2.  The 0x8101
        // fourth sprite belongs only to the unavailable 0x13B master branch,
        // so importing it into the 5.2 base skill would be incorrect.
        vec3_t white, blue;
        Vector(1.f, 1.f, 1.f, white);
        Vector(0.6f, 0.85f, 1.f, blue);
        CreateParticle(kAlphaRingX256Bitmap, effect.Position, effect.Angle,
            white, 0, 0.2f, &effect);
        CreateParticle(kDamage1MonoBitmap, effect.Position, effect.Angle,
            blue, 0, 0.45f, &effect);
        CreateParticle(kFlare01RedBitmap, effect.Position, effect.Angle,
            white, 1, 2.f, &effect);
        break;
    }
    case kDetectionMarkModel:
        // 0x1492259 writes raw incoming scale to +0xA0; 0x149226C
        // zeroes xmm0 before +0xDC alpha. Subtype 1 exits at 0x1492277.
        // Both Sword's scale-zero mark and buff's scale-0.2 mark start
        // invisible and gain alpha through their own updater.
        if (effect.SubType == 0)
        {
            effect.Scale = incomingScale;
            effect.Alpha = 0.f;
        }
        break;
    case kDetectionImpactModel:
        // Native 0x14926CC/0x1492797 write raw scale to +0xA0, then
        // xorps at 0x14926DF/0x14927AA sets +0xDC alpha to zero in
        // both subtypes. The earlier filtered disassembly hid xorps.
        effect.Scale = incomingScale;
        effect.Alpha = 0.f;
        effect.Position[2] += 50.f;
        Vector(0.f, 0.f, 0.f, effect.Angle);
        break;
    case kSword68FController:
        // Native 0x68F has two three-tick projectiles.  Both begin at alpha
        // .3; subtype 0 flies 50 units backwards, subtype 1 uses the shorter
        // 10-unit vector and one-tenth stopping range.
        effect.Alpha = 0.3f;
        InitializeSwordFlight(effect, effect.SubType == 0 ? -50.f : -10.f,
            effect.SubType == 0 ? 1.f : 0.1f);
        break;
    case kSword690Controller:
        // Native 0x690 stores its launch origin and travel distance once;
        // update consumes Distance/(InitialLife+1) each tick.
        InitializeSwordFlight(effect, 1.f, 1.f);
        break;
    case kDetectionController:
    {
        // Exact 0x1492364 initializer graph for native root 0x692.
        vec3_t white, blue, warm;
        Vector(1.f, 1.f, 1.f, white);
        Vector(0.5f, 0.5f, 1.f, blue);
        Vector(0.70f, 0.25f, 0.05f, warm);
        SpawnChild(kDetectionChildController, effect, effect.Owner, 0, 0.f);
        SpawnChild(kDetectionImpactModel, effect, &effect, 0, 1.f);
        SpawnBitmapChild(kMagicGround12Effect, effect, &effect, white,
            4, 0.f);
        SpawnBitmapChild(kMagicGround12Effect, effect, &effect, white,
            5, 0.f);
        SpawnBitmapChild(kFlareBlueEffect, effect, 0, blue,
            2, 2.5f);
        SpawnBitmapChild(kRingOfGradation2Effect, effect, 0, warm,
            0, 6.f);
        break;
    }
    case kDemolishController:
    {
        // Exact 0x149289F initializer graph for native root 0x695.
        vec3_t white, ring;
        Vector(1.f, 1.f, 1.f, white);
        Vector(1.f, 0.4f, 0.1f, ring);
        SpawnChild(kDemolishChildController, effect, effect.Owner, 0, 0.f);
        SpawnChild(kDetectionImpactModel, effect, &effect, 0, 1.f);
        SpawnChild(kDetectionImpactModel, effect, &effect, 1, 0.5f);
        SpawnBitmapChild(kFlareEffect, effect, 0, white, 8, 8.f);
        SpawnBitmapChild(kRingOfGradation2Effect, effect, 0, ring,
            0, 6.f);
        break;
    }
    case kMagicGround12Effect:
        if (effect.SubType == 2)
        {
            // S21 0x147E7D4: subtype 2 overrides the allocator's .9
            // fallback with scale 1, alpha 1 and a 15-tick envelope.
            // It additionally creates 0x8012 subtype 17; that nested
            // effect remains separate until its map/render path is ported.
            effect.Scale = 1.f;
            effect.Alpha = 1.f;
            effect.Timer = WorldTime;
            vec3_t shockLight;
            Vector(1.f, 0.28f, 0.95f, shockLight);
            SpawnBitmapChild(kPierceShockWaveEffect, effect, 0,
                shockLight, 17, 0.f);
        }
        // 0x147EA2A/0x147EA83: buff-ring subtypes 4/5 copy raw scale zero
        // back over the allocator fallback, then expand by .3/1 per tick.
        else if (effect.SubType == 4 || effect.SubType == 5)
            effect.Scale = incomingScale;
        break;
    default:
        break;
    }
#else
    (void)effect;
    (void)incomingScale;
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

    if (IsBitmapEffect(effect.Type))
    {
        // Bitmap effects have separate low/high-code S21 update dispatchers.
        // The high-code 0x7FDD subtype-2 branch is at 0x157D57E; four
        // other root nodes use half-life triangles in the low-code switch.
        switch (effect.Type)
        {
        case kPierceShockWaveEffect:
            // S21 0x14FD824: subtype 17 expands .5 per tick and
            // derives object alpha from remainingLife/15. No clock
            // refresh or owner-follow rule is present in that branch.
            effect.Scale += 0.5f * animationFactor;
            effect.Alpha = effect.LifeTime / 15.f;
            break;
        case kPierce8149Effect:
            // 0x152BA06: unlike 0x81CE, life refreshes only near zero.
            // Its S21 clock still cuts the sprite object off at 6000 ms.
            if (effect.LifeTime <= 2.f)
                effect.LifeTime = 50.f;
            if (WorldTime - effect.Timer > 6000.f)
            {
                effect.LifeTime = 0.f;
                effect.Timer = 0.f;
            }
            break;
        case kPierce80BAEffect:
            // 0x151F08A / 0x151F2F9 refresh to 30 until six seconds.
            // S21 0x151F0D0: each subtype-7 flare emits exactly three
            // particle children every update, in native call order.
            if (effect.SubType == 7)
            {
                vec3_t smokeLight;
                if (rand() % 4 == 0)
                    Vector(0.5f, 1.f, 0.f, smokeLight);
                else
                    Vector(0.6f, 0.2f, 1.f, smokeLight);
                CreateParticle(kSmokeLines01Bitmap + rand() % 3,
                    effect.Position, effect.Angle, smokeLight, 9, 1.3f,
                    &effect);
                vec3_t cloudLight;
                Vector(0.2f, 0.2f, 1.f, cloudLight);
                CreateParticleTexture(BITMAP_CLUD64, kClud64Bitmap,
                    effect.Position, effect.Angle, cloudLight, 17, 2.3f,
                    &effect);
                Vector(0.8f, 0.8f, 1.f, cloudLight);
                CreateParticleTexture(BITMAP_CLUD64, kClud64Bitmap,
                    effect.Position, effect.Angle, cloudLight, 17, 1.3f,
                    &effect);
            }
            effect.LifeTime = 30.f;
            if (WorldTime - effect.Timer > 6000.f)
            {
                effect.LifeTime = 0.f;
                effect.Timer = 0.f;
            }
            break;
        case kPierce81CEEffect:
            if (effect.SubType == 4)
            {
                // 0x1534339: LifeTime/20 drives alpha; scale grows from
                // 4.35 by .05 for each elapsed tick, then life refreshes.
                effect.Scale = (20.f - effect.LifeTime) * 0.05f + 4.35f;
                effect.Alpha = effect.LifeTime / 20.f;
                if (effect.LifeTime <= 1.f)
                    effect.LifeTime = 20.f;
            }
            else
            {
                // 0x1534250/0x1534422: subtype 3/5 retain 30 ticks
                // while their authored millisecond window is still open.
                effect.LifeTime = 30.f;
            }
            if (WorldTime - effect.Timer > 6000.f)
            {
                effect.LifeTime = 0.f;
                effect.Timer = 0.f;
            }
            break;
        case kFlare01RedEffect: // 0x80BA subtype A, update 0x151F416
        case kRingOfGradation2Effect: // 0x82F6, update 0x1548E75
        case kEnemyRing01Effect: // 0x82F7, update 0x15484D8
        case kFlareEffect: // 0x7F78 subtype 8, update 0x14FF2CE
            {
                const float half = static_cast<float>(
                    static_cast<int>(initialLife) / 2);
                if (half > 0.f)
                    effect.Alpha += (effect.LifeTime > half ? 1.f : -1.f) *
                        animationFactor / half;
                if (effect.Type == kFlare01RedEffect)
                    effect.Angle[2] += animationFactor;
            }
            break;
        case kFlareBlueEffect: // 0x7FDD subtype 2, update 0x157D57E
            if (effect.SubType == 2)
            {
                const float half = static_cast<float>(
                    static_cast<int>(initialLife) / 2);
                if (half > 0.f)
                    effect.Alpha += (effect.LifeTime > half ? 1.f : -1.f) /
                        half;
                // 0x157D5F9..0x157D627 adds native 5 degrees per tick.
                effect.Angle[2] += 5.f;
            }
            break;
        case kMagicGround12Effect:
            if (effect.SubType == 2)
            {
                // Native 0x1534735: scale expands .15 per tick and alpha
                // reads remainingLife/15; at one tick it repeats its ring
                // until the independent 6000-ms S21 clock expires.
                effect.Scale += 0.15f * animationFactor;
                effect.Alpha = effect.LifeTime / 15.f;
                if (effect.LifeTime <= 1.f)
                {
                    effect.LifeTime = 15.f;
                    effect.Scale = 1.f;
                }
                if (WorldTime - effect.Timer > 6000.f)
                {
                    effect.LifeTime = 0.f;
                    effect.Timer = 0.f;
                }
            }
            else
            {
                // 0x81CF buff subtypes 4/5, update 0x1534839.
                effect.Alpha -= animationFactor / initialLife;
                effect.Scale += (effect.SubType == 4 ? 0.3f : 1.f) *
                    animationFactor;
            }
            break;
        default: break;
        }
        if (effect.Alpha < 0.f) effect.Alpha = 0.f;
        if (effect.Alpha > 1.f) effect.Alpha = 1.f;
        return;
    }

    if (IsController(effect.Type))
    {
        if (effect.Type == kSwordSecondaryController &&
            effect.Skill == kPierceAttack && PierceTargetCount(effect) > 0)
        {
            // Native 0x689 copies the caster's cast-target key (+0x3E)
            // into EFFECT+0x364 at construction, then 0x1545A39 checks
            // that fixed actor on every update before walking +0x1DC.
            // The 5.2 fanout deliberately puts the clicked victim first;
            // keep it in a separate sidecar because m_sTargetIndex changes
            // to the active lane and is inherited by its 0x68B children.
            const auto original = gPierceCastTargetIndex.find(&effect);
            if (original == gPierceCastTargetIndex.end() ||
                !ResolveTarget(original->second))
            {
                effect.LifeTime = 0.f;
                return;
            }
        }
        switch (effect.Type)
        {
        case kSwordInertiaController:
            // Native 0x68A update has two subtype branches. Both emit the
            // 0x7F5A/0x807E..0x8080 smoke family and model 0x691; there is no
            // boomerang interpolation in this case.
            if (owner && owner->Live &&
                (owner->CurrentAction == kSwordInertiaAction ||
                 owner->CurrentAction == kPierceAttackAction))
            {
                // S21 0x1546793..0x1546803 uses the owning actor's
                // animation frame, not the controller lifetime, for a
                // 0..3.5..7 frame opacity triangle (constant 0x1B4ED34).
                // Both ordinary Sword and base Pierce use subtype zero.
                const float frame = owner->AnimationFrame;
                effect.Alpha = Clamp01(frame <= 3.5f ?
                    frame / 3.5f : 1.f - (frame - 3.5f) / 3.5f);
                VectorCopy(owner->Position, effect.Position);
                for (int ordinal = 0; ordinal < 2; ++ordinal)
                {
                    vec3_t position, angle, light;
                    VectorCopy(effect.Position, position);
                    VectorCopy(effect.Angle, angle);
                    position[0] += static_cast<float>((rand() % 61) - 30);
                    position[1] += static_cast<float>((rand() % 61) - 30);
                    if ((rand() & 1) == 0)
                    {
                        Vector(1.f, 0.1f, 0.05f, light);
                        CreateParticle(kSmoke01Bitmap, position, angle, light,
                            0x77, 2.f, &effect);
                    }
                    else
                    {
                        Vector(1.f, 0.25f, 0.05f, light);
                        CreateParticle(kSmokeLines01Bitmap + rand() % 3,
                            position, angle, light, 0x0D, 1.8f, &effect);
                    }
                }
                // S21 0x1546BEA..0x1546D75 creates model 0x691 only
                // when the independent random sample is divisible by six.
                // A child on every frame accumulated opaque geometry.
                if (rand() % 6 == 0)
                {
                    // The 0x1546CC1..0x1546CED argument window copies the
                    // root angles, then replaces Z with a 0..360 sample.
                    OBJECT markAnchor = effect;
                    markAnchor.Angle[2] = NativeRandomUnitStep(0, 360);
                    SpawnChild(kDetectionMarkModel, markAnchor, &effect,
                        0, 0.f);
                }
            }
            break;
        case kSwordSecondaryController:
        {
            const bool pierceList = effect.Skill == kPierceAttack &&
                PierceTargetCount(effect) > 0;
            if (pierceList && effect.SubType == 2)
            {
                // Native 0x15466CF pops the completed 0x689 list key when
                // its 0x68B child has set root state 2 at 0x154759F.
                ++effect.CurrentAction;
                effect.SubType = 0;
                effect.m_sTargetIndex = -1;
            }
            if (pierceList && effect.CurrentAction >=
                PierceTargetCount(effect))
            {
                effect.LifeTime = 0.f;
                break;
            }
            if (owner && owner->Live &&
                (pierceList ?
                    effect.SubType == 0 :
                    effect.Timer < 1.f))
            {
                // Native 0x1545A08 walks the list on supplemental 0x126
                // roots; base Sword remains a single-target one-shot. The
                // private 5.2 sidecar supplies that list instead of adding
                // S21 vector fields to OBJECT. It anchors 0x68D at
                // target+100Z, backs the
                // launch point 100 units along the caster-to-target yaw,
                // emits one 0x68B (not both subtypes), one 0x68E/subtype 0,
                // then raises the 0x68C endpoint another 100 units.  0x686
                // belongs only to the separate 0x817 master-skill branch.
                OBJECT* laneTarget = pierceList ?
                    PierceTargetAt(effect, effect.CurrentAction) : target;
                if (pierceList && !laneTarget)
                {
                    // S21 removes an unresolved list key; never emit a
                    // false hit at the caster's creation position.
                    ++effect.CurrentAction;
                    break;
                }
                if (pierceList)
                    effect.m_sTargetIndex = PierceTargetIndexAt(effect,
                        effect.CurrentAction);
                OBJECT anchor = effect;
                if (laneTarget)
                    VectorCopy(laneTarget->Position, anchor.Position);
                anchor.Position[2] += 100.f;
                anchor.Angle[0] = 0.f;
                anchor.Angle[1] = 0.f;
                anchor.Angle[2] = CreateAngle(effect.StartPosition[0],
                    effect.StartPosition[1], anchor.Position[0],
                    anchor.Position[1]);
                SpawnChild(kSword68DController, anchor, &effect, 0, 0.f);

                OBJECT launch = anchor;
                // S21 0x1545D43..0x1545E78 rotates (0,-1,0) by the
                // target yaw, scales that forward vector by 100, then
                // SUBTRACTS it from the target position. The old plus
                // signs launched the sword beyond the target instead.
                vec3_t nativeForward, rotatedForward;
                vec34_t launchMatrix;
                Vector(0.f, -1.f, 0.f, nativeForward);
                AngleMatrix(launch.Angle, launchMatrix);
                VectorRotate(nativeForward, launchMatrix,
                    rotatedForward);
                launch.Position[0] -= rotatedForward[0] * 100.f;
                launch.Position[1] -= rotatedForward[1] * 100.f;
                launch.Position[2] -= rotatedForward[2] * 100.f;
                // S21 0x1545EA1/0x1545EF8/0x154605F reads the owning
                // 0x689 EFFECT+0xA0 scale for 0x68B/0x68E creation, not
                // the caster actor scale. The list-bearing receive creates
                // that 0x689 with incoming scale zero at 0x12A7A43.
                const float sourceScale = effect.Scale;
                SpawnChild(kSword68BController, launch, &effect, 0,
                    sourceScale);
                SpawnChild(kSword68EController, launch, &effect, 0,
                    sourceScale);

                launch.Position[2] += 100.f;
                SpawnChild(kSword68CController, launch, &effect, 0, 0.f);
                if (pierceList)
                {
                    SendPierceLaneRequest(effect, effect.m_sTargetIndex,
                        *laneTarget);
                    effect.SubType = 1;
                }
                else
                    effect.Timer = 1.f;
            }
            break;
        }
        case kSword68BController:
            // Native subtype 1 receives a final five-tick 30-unit direction
            // push before the engine homing/collision test.
            if (effect.SubType == 1 && effect.LifeTime > initialLife - 5.f)
            {
                effect.Position[0] += effect.Direction[0] * 30.f *
                    animationFactor;
                effect.Position[1] += effect.Direction[1] * 30.f *
                    animationFactor;
                effect.Position[2] += effect.Direction[2] * 30.f *
                    animationFactor;
            }
            // Native helper 0x1417693 aborts when its target/model transform
            // cannot be resolved.  Preserve the target-liveness side effect;
            // its remaining work is bone preparation rather than movement.
            if (!AdvanceSword68BTargetAnimation(effect, target))
            {
                // 0x154759F signals state 2 on the owning 0x689 root when
                // the target model/action helper returns false.
                if (owner && owner->Type == kSwordSecondaryController &&
                    owner->Skill == kPierceAttack)
                    owner->SubType = 2;
                effect.LifeTime = 0.f;
            }
            break;
        case kSword68EController:
            if (effect.Timer < 1.f)
            {
                SpawnChild(kSword68FController, effect, &effect, 0, 1.f);
                SpawnChild(kSword68FController, effect, &effect, 1, 1.f);
                SpawnChild(kSword690Controller, effect, &effect, 0, 1.f);
                effect.Timer = 1.f;
            }
            break;
        case kSword68FController:
            AdvanceSwordFlight(effect, animationFactor);
            effect.Alpha = initialLife > 0.f ?
                (effect.LifeTime / initialLife) * 0.5f : 0.f;
            break;
        case kSword690Controller:
        {
            AdvanceSwordFlight(effect, animationFactor);
            // Native 0x690 emits two independently randomized smoke samples
            // around the projectile after advancing it.
            for (int ordinal = 0; ordinal < 2; ++ordinal)
            {
                vec3_t position, angle, light;
                VectorCopy(effect.Position, position);
                VectorCopy(effect.Angle, angle);
                const float radius = 30.f;
                const float yaw = static_cast<float>(rand() % 360) *
                    0.01745329252f;
                position[0] += cosf(yaw) * radius;
                position[1] += sinf(yaw) * radius;
                position[2] += 100.f;
                if (ordinal == 0)
                {
                    Vector(0.8f, 0.05f, 0.05f, light);
                    CreateParticle(kSmoke01Bitmap, position, angle, light,
                        0x76, 2.5f, &effect);
                }
                else
                {
                    Vector(1.f, 0.2f, 0.25f, light);
                    CreateParticle(kSmokeLines01Bitmap + rand() % 3,
                        position, angle, light, 0x0C, 3.f, &effect);
                }
            }
            break;
        }
        case kBatFlockController:
            // 0x682 emits on LifeTime%5 == 0 or a 1/20 random pulse.
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f)
            {
                effect.Timer -= 1.f;
                const int life = static_cast<int>(effect.LifeTime);
                if ((life > 0 && life % 5 == 0) || rand() % 20 == 0)
                    EmitBatMainPulse(effect);
            }
            break;
        case kBatFlockTargetController:
        {
            // S21 0x15442BB seeds once at InitialLife, then gates at
            // random 1/15. The root remains at its caster creation point;
            // the chosen target is only the joint owner.
            if (effect.LifeTime >= initialLife)
            {
                EmitBatTargetJoint(effect, PickBatFlockTarget(effect));
            }
            else
            {
                effect.Timer += animationFactor;
                while (effect.Timer >= 1.f)
                {
                    effect.Timer -= 1.f;
                    if (rand() % 15 == 0)
                        EmitBatTargetJoint(effect, PickBatFlockTarget(effect));
                }
            }
            break;
        }
        case kBatFlockDotController:
            // S21 0x684 does not attach its root to the selected target.
            // It independently gates a localized 0x685 spawn at 1/15.
            effect.Timer += animationFactor;
            while (effect.Timer >= 1.f)
            {
                effect.Timer -= 1.f;
                if (rand() % 15 == 0)
                    EmitBatOrbitChild(effect);
            }
            break;
        case kBatFlockOrbitController:
            // Native 0x1544ADF rotates by its initializer's signed step,
            // advances scale/3 along the rotated direction, then emits three
            // sequential 0x82E8 particles with light (1,.8,.2).
            effect.Angle[2] +=
                (static_cast<int>(effect.LifeTime) % 6 < 3 ? 1.f : -1.f) *
                effect.Gravity;
            vec34_t matrix;
            AngleMatrix(effect.Angle, matrix);
            vec3_t step, light;
            step[0] = matrix[0][0] * (effect.Scale / 3.f);
            step[1] = matrix[0][1] * (effect.Scale / 3.f);
            step[2] = matrix[0][2] * (effect.Scale / 3.f);
            Vector(1.f, 0.8f, 0.2f, light);
            for (int ordinal = 0; ordinal < 3; ++ordinal)
            {
                effect.Position[0] += step[0];
                effect.Position[1] += step[1];
                effect.Position[2] += step[2];
                CreateParticle(kFireHik01MagentaBitmap, effect.Position,
                    effect.Angle, light, 0, 1.f, &effect);
            }
            break;
        case kBatFlock686Controller:
        {
            // Native subtype 1 emits a randomized burst on 1/15 frames;
            // subtype 0 performs the same burst every frame.  Both add a
            // sparse 0x82EF joint on a separate 1/8 gate.
            if (effect.SubType == 0 || rand() % 15 == 0)
            {
                OBJECT burst = effect;
                burst.Position[0] += static_cast<float>((rand() % 101) - 50);
                burst.Position[1] += static_cast<float>((rand() % 101) - 50);
                burst.Position[2] += static_cast<float>((rand() % 101) + 50);
                const float scale = static_cast<float>(60 + rand() % 61) *
                    0.01f;
                Vector(1.f, 1.f, 1.f, burst.Light);
                SpawnChild(kBatFlock687Controller, burst, &effect, 0, scale);
                CreateParticle(kEmpact01Bitmap, burst.Position, burst.Angle,
                    burst.Light, 0, scale * 0.5f, &effect);
            }
            if (rand() % 8 == 0)
            {
                vec3_t jointPosition;
                VectorCopy(effect.Position, jointPosition);
                jointPosition[2] += 100.f;
                CreateJoint(kGhostMark02Bitmap, jointPosition, jointPosition,
                    effect.Angle, 0, &effect, 10.f, -1, 0, 0, -1, 0, -1);
            }
            if (effect.SubType == 0)
                ++effect.LifeTime;
            break;
        }
        case kBatFlock687Controller:
        {
            vec3_t white;
            Vector(1.f, 1.f, 1.f, white);
            CreateParticle(kWaterBoardRedBitmap, effect.Position, effect.Angle,
                white, 0, effect.Scale * 0.6f, &effect);
            CreateParticle(kWaterWallBitmap, effect.Position, effect.Angle,
                white, 0, effect.Scale * 0.8f, &effect);
            break;
        }
        case kPierceController:
            // S21 0x154113C..0x1541167 removes 0x679 when the owner
            // handle is absent or no longer live; do not keep a stale
            // 99-tick controller in the recycled 5.2 effect pool.
            if (!owner || !owner->Live)
            {
                effect.LifeTime = 0.f;
                break;
            }
            // Native 0x154117D restores the root's launch position from its
            // own saved vector. 0x15411DF reads the root animation frame
            // (+0xD0), not the owning player's frame.
            VectorCopy(effect.StartPosition, effect.Position);
            // 0x15411E2 compares that effect frame against exactly 4.0.
            // At that frame the center 0x67B/0x681 pair and the +90-degree
            // 0x67A/0x67C/0x681 group are both created.
            if (owner && owner->Live && effect.Timer < 1.f &&
                effect.AnimationFrame >= 4.f)
            {
                SpawnChild(kPierce67BController, effect, &effect, 0, 1.f);
                SpawnChild(kPierceSwordLineModel, effect, owner, 0, 1.f);
                OBJECT flank = effect;
                OffsetByYaw(flank, 90.f, 200.f, 0.f);
                const int subtype = rand() & 1;
                SpawnChild(kPierce67AController, flank, &effect, subtype,
                    owner->Scale);
                SpawnChild(kPierce67CController, flank, &effect, 0,
                    owner->Scale);
                flank.Position[2] += 50.f;
                SpawnChild(kPierceSwordLineModel, flank, owner, subtype + 1,
                    1.f);
                effect.Timer = 1.f;
            }
            // 0x15418B3 compares the same effect frame against exactly 7.0
            // and emits the mirrored
            // -90-degree group once.
            if (owner && owner->Live && effect.Timer < 2.f &&
                effect.AnimationFrame >= 7.f)
            {
                OBJECT flank = effect;
                OffsetByYaw(flank, -90.f, 200.f, 0.f);
                const int subtype = rand() & 1;
                SpawnChild(kPierce67AController, flank, &effect, subtype,
                    owner->Scale);
                SpawnChild(kPierce67CController, flank, &effect, 0,
                    owner->Scale);
                flank.Position[2] += 50.f;
                SpawnChild(kPierceSwordLineModel, flank, owner, subtype + 1,
                    1.f);
                effect.Timer = 2.f;
            }
            break;
        case kPierce67AController:
            if (effect.Timer < 1.f)
            {
                SpawnChild(kPierce67BController, effect, &effect, 0, 1.f);
                SpawnChild(kPierce67BController, effect, &effect, 1, 1.f);
                effect.Timer = 1.f;
            }
            break;
        case kPierce67BController:
            if (effect.Timer < 1.f)
            {
                SpawnChild(kPierce680Controller, effect, &effect, 0, 1.f);
                effect.Timer = 1.f;
            }
            CreateSprite(kImpack03Bitmap, effect.Position, 1.5f,
                effect.Light, &effect, 0.f, 4);
            CreateSprite(kPinStarBitmap, effect.Position, 1.f,
                effect.Light, &effect, 0.f, 4);
            break;
        case kPierce67CController:
            if (effect.Timer < 1.f)
            {
                SpawnChild(kPierce67DController, effect, &effect, 0, 1.f);
                SpawnChild(kPierce67EController, effect, &effect, 0, 1.f);
                effect.Timer = 1.f;
            }
            break;
        case kPierce67DController:
        case kPierce680Controller:
            break;
        case kPierce67EController:
            CreateParticle(kJujugBitmap, effect.Position, effect.Angle,
                effect.Light, 0, 1.f, &effect);
            break;
        case kSword68CController:
        {
            // Native 0x15475B8 does not attach 0x68C to the selected target.
            // It emits 0x8149 at the endpoint supplied by root 0x689, with a
            // random +/-50 position cube, +/-90 pitch/yaw and a signed
            // 80..100-degree roll. The authored scale is 24.
            vec3_t position, angle, light;
            VectorCopy(effect.Position, position);
            position[0] += static_cast<float>((rand() % 101) - 50);
            position[1] += static_cast<float>((rand() % 101) - 50);
            position[2] += static_cast<float>((rand() % 101) - 50);
            angle[0] = static_cast<float>((rand() % 181) - 90);
            angle[1] = static_cast<float>((rand() % 181) - 90);
            angle[2] = effect.Angle[2] + ((rand() & 1) ? 1.f : -1.f) *
                static_cast<float>(80 + rand() % 21);
            Vector(1.f, 0.1f, 0.3f, light);
            CreateParticle(kGroundStarBitmap, position, angle, light,
                0, 24.f, &effect);
            break;
        }
        case kDetectionController:
        case kDemolishController:
            if (!owner || !owner->Live)
            {
                effect.LifeTime = 0.f;
                break;
            }
            // Native 0x154856F/0x1548F0C run only during the paired source
            // action and emit four randomized smoke records per frame.
            if (owner->CurrentAction != (effect.Type == kDetectionController ?
                    kDetectionAction : kDemolishAction))
                break;
            // S21 0x15486A4/0x1549041 builds every smoke position from
            // the root's saved +0x1D0 launch vector. The root does not
            // follow a moving caster during its action.
            VectorCopy(effect.StartPosition, effect.Position);
            for (int ordinal = 0; ordinal < 4; ++ordinal)
            {
                vec3_t position, angle, light;
                VectorCopy(effect.Position, position);
                VectorCopy(effect.Angle, angle);
                // 0x1548600/0x1548F9D select the particle family first.
                // Both paired branches then call native Random(-30,30,1)
                // independently for X and Y, not a circular yaw/radius.
                const bool smoke01 = (rand() & 1) == 0;
                position[0] += static_cast<float>((rand() % 61) - 30);
                position[1] += static_cast<float>((rand() % 61) - 30);
                if (smoke01)
                {
                    if (effect.Type == kDetectionController)
                        Vector(0.4f, 0.45f, 1.f, light);
                    else
                        Vector(1.f, 0.35f, 0.05f, light);
                    CreateParticle(kSmoke01Bitmap, position, angle, light,
                        0x77, 2.5f, &effect);
                }
                else
                {
                    if (effect.Type == kDetectionController)
                        Vector(0.15f, 0.18f, 1.f, light);
                    else
                        Vector(1.f, 0.5f, 0.15f, light);
                    CreateParticle(kSmokeLines01Bitmap + rand() % 3,
                        position, angle, light, 0x0D, 2.f, &effect);
                }
            }
            {
                OBJECT wave = effect;
                // 0x1548A6D/0x154940A: even life uses inclusive
                // Random(0,360,1); odd life negates Random(1,360,1).
                wave.Angle[2] = (static_cast<int>(effect.LifeTime) & 1) == 0 ?
                    static_cast<float>(rand() % 361) :
                    -static_cast<float>(1 + rand() % 360);
                Vector(1.f, 1.f, 1.f, wave.Light);
                SpawnChild(kDetectionMarkModel, wave, &effect, 0, 0.2f);
            }
            break;
        case kDetectionChildController:
        case kDemolishChildController:
            if (!owner || !owner->Live)
            {
                effect.LifeTime = 0.f;
                break;
            }
            {
                OBJECT bat = effect;
                // Native 0x693/0x696 also start their 0x678 children at
                // their own saved +0x1D0 cast position, then add 100 Z.
                VectorCopy(effect.StartPosition, bat.Position);
                bat.Position[2] += 100.f;
                Vector(0.5f, 0.8f, 1.f, bat.Light);
                const int subtype = effect.Type == kDetectionChildController ?
                    1 : 4;
                const float scale = effect.Type == kDetectionChildController ?
                    1.5f : 2.f;
                SpawnChild(kBatFlockModel, bat, &effect, subtype, scale);
                if ((rand() & 1) == 0)
                    SpawnChild(kBatFlockModel, bat, &effect, subtype, scale);
            }
            break;
        default:
            break;
        }
        return;
    }

    if (effect.Type == kPierceSwordLineModel)
    {
        // Native 0x154371B: 0x681 is a zero-mesh, four-bone animation
        // controller.  Bones 3 and 1 are the two ribbon endpoints. Subtypes
        // 0..2 select S21 bitmap 0x82EC; subtype 3 selects 0x82F4.
        if (!owner || !owner->Live || !Models ||
            !EnsureModel(kPierceSwordLineModel))
        {
            effect.LifeTime = 0.f;
            return;
        }
        BMD& swordLine = Models[kPierceSwordLineModel];
        if (swordLine.NumBones >= 4 && swordLine.NumActions > 0)
        {
            swordLine.CurrentAction = 0;
            VectorCopy(effect.Position, swordLine.BodyOrigin);
            swordLine.BodyScale = effect.Scale;
            // 0x681's BMD has seven keys. Native iterates every key on each
            // update and divides the source action speed by that exact count.
            const float actionSpeed = effect.Velocity;
            const int samples = swordLine.Actions[0].NumAnimationKeys;
            if (samples <= 0 || actionSpeed <= 0.f)
                return;
            // S21 0x1543882: (initialLife - remainingLife + 1) * speed
            // minus one speed step. The 5.2 lifetime already incorporates
            // animationFactor, so multiplying speed by it again drifts at
            // lower frame rates.
            float frame = (initialLife - effect.LifeTime) * actionSpeed;
            for (int sample = 0; sample < samples; ++sample)
            {
                if (frame > 5.5f)
                    break;
                swordLine.Animation(BoneTransform, frame,
                    effect.PriorAnimationFrame, effect.PriorAction,
                    effect.Angle, effect.HeadAngle);
                vec3_t relative, start, finish, light;
                Vector(0.f, 0.f, 0.f, relative);
                swordLine.TransformPosition(BoneTransform[3], relative,
                    start, false);
                swordLine.TransformPosition(BoneTransform[1], relative,
                    finish, false);
                // S21 0x1543A79: modes 0..2 stay fully lit through frame 5,
                // then use 1 - frame/keyCount; mode 3 keeps its initialized
                // OBJECT+0xDC alpha unchanged.
                const float alpha = effect.SubType == 3 ? effect.Alpha :
                    (frame <= 5.f ? 1.f :
                        Clamp01(1.f - frame / static_cast<float>(samples)));
                if (effect.SubType != 3)
                    effect.Alpha = alpha;
                Vector(alpha, alpha, alpha, light);
                CreateObjectBlurBitmap(&effect, start, finish, light,
                    effect.SubType == 3 ? kBlur02MonoLongVan2Bitmap :
                    kBlur02MonoLongVanBitmap, false, 0, 100);
                frame += actionSpeed / static_cast<float>(samples);
            }
        }
    }
    else if (effect.Type == kBatFlockModel)
    {
        // Native 0x678 has five distinct motion branches; attaching every bat
        // to the target collapses the flock into the stationary placeholder
        // seen in the earlier preview.
        if (effect.SubType == 0)
        {
            const float third = initialLife / 3.f;
            if (effect.LifeTime < third && third > 0.f)
                effect.Alpha -= animationFactor / third;
            effect.Scale -= 0.005f * animationFactor;
            effect.Angle[2] += NativeRandomUnitStep(-10, 10) *
                animationFactor;
            DirectionFromAngle(effect);
            effect.Position[0] += effect.Direction[0] * effect.Gravity *
                animationFactor;
            effect.Position[1] += effect.Direction[1] * effect.Gravity *
                animationFactor;
            effect.Position[2] += effect.Direction[2] * effect.Gravity *
                animationFactor;
        }
        else if (effect.SubType == 1 || effect.SubType == 4)
        {
            const float acceleration = (initialLife - effect.LifeTime) *
                effect.Gravity * animationFactor;
            effect.Position[0] += effect.Direction[0] * acceleration;
            effect.Position[1] += effect.Direction[1] * acceleration;
            effect.Position[2] += effect.Direction[2] * acceleration;
        }
        else if (effect.SubType == 2)
        {
            const float third = initialLife / 3.f;
            if (effect.LifeTime < third && third > 0.f)
                effect.Alpha -= animationFactor / third;
            // S21 0x154064B..0x154091B checks the real target distance,
            // then aims from the bat XY projected onto terrain height.
            // A missing target retains direction and does not freeze flight.
            if (target)
            {
                if (VectorDistance3(effect.Position, target->Position) <= 50.f)
                    effect.LifeTime = 0.f;
                else
                {
                    vec3_t terrainOrigin;
                    VectorCopy(effect.Position, terrainOrigin);
                    terrainOrigin[2] = RequestTerrainHeight(terrainOrigin[0],
                        terrainOrigin[1]);
                    VectorSubtract(target->Position, terrainOrigin,
                        effect.Direction);
                    VectorNormalize(effect.Direction);
                    effect.Angle[2] = CreateAngle2D(terrainOrigin,
                        target->Position);
                }
            }
            if (effect.LifeTime > 0.f)
            {
                effect.Position[0] += effect.Direction[0] * effect.Gravity *
                    animationFactor;
                effect.Position[1] += effect.Direction[1] * effect.Gravity *
                    animationFactor;
                effect.Position[2] += effect.Direction[2] * effect.Gravity *
                    animationFactor;
                // Native 0x1540ADE: independent half-rate 0x8020 particle,
                // resolved from the S21 loader to Effect\\WATERFALL4.jpg.
                if ((rand() & 1) == 0)
                {
                    vec3_t trailLight;
                    Vector(0.5f, 0.05f, 0.f, trailLight);
                    CreateParticle(kWaterFall4Bitmap, effect.Position,
                        effect.Angle, trailLight, 2, 2.f, &effect);
                }
            }
        }
        else if (effect.SubType == 3 && owner && owner->Live)
        {
            vec3_t destination;
            VectorCopy(owner->Position, destination);
            destination[2] += 120.f;
            VectorSubtract(destination, effect.Position, effect.Direction);
            const float distance = VectorNormalize(effect.Direction);
            if (distance > 150.f)
                effect.Angle[2] = CreateAngle2D(effect.Position, destination);
            effect.Position[0] += effect.Direction[0] * effect.Gravity *
                animationFactor;
            effect.Position[1] += effect.Direction[1] * effect.Gravity *
                animationFactor;
            effect.Position[2] += effect.Direction[2] * effect.Gravity *
                animationFactor;
            effect.Distance = VectorDistance3(effect.Position, destination);
            if (effect.Distance > 300.f)
                effect.LifeTime = 0.f;
            ++effect.LifeTime;
        }
    }
    else if (effect.Type == kBatFlockTrailModel)
    {
        const float halfLife = initialLife * 0.5f;
        const float delta = 1.f / (halfLife > 0.f ? halfLife : 1.f);
        effect.Alpha += (effect.LifeTime > halfLife ? delta : -delta) *
            animationFactor;
        effect.Angle[1] += (effect.Angle[1] >= 0.f ?
            1.f : -1.f) * animationFactor;
    }
    else if (effect.Type == kDetectionImpactModel)
    {
        const float halfLife = initialLife * 0.5f;
        const float delta = (effect.SubType == 1 ? 0.5f : 1.f) /
            (halfLife > 0.f ? halfLife : 1.f);
        effect.Alpha += (effect.LifeTime > halfLife ? delta : -delta) *
            animationFactor;
        if (effect.SubType == 1)
            effect.Scale += 0.02f * animationFactor;
    }
    else if (effect.Type == kDetectionMarkModel)
    {
        if (effect.SubType == 0)
        {
            const float quarter = initialLife * 0.25f;
            if (effect.LifeTime > initialLife - quarter)
                effect.Alpha += animationFactor / quarter;
            else if (effect.LifeTime < quarter)
                effect.Alpha -= animationFactor / quarter;
            effect.Scale += 0.025f * animationFactor;
            effect.Angle[2] += (effect.Angle[2] >= 0.f ? 3.f : -3.f) *
                animationFactor;
        }
        else
        {
            const float half = initialLife * 0.5f;
            effect.Alpha += (effect.LifeTime > half ? 1.f : -1.f) *
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
    if (effect.Type == kPierceShockWaveEffect)
    {
        // S21 0x159D2BE gates 0x8012 water submission to Kalima
        // 24..29 and Lost Kalima 36; all other maps have no draw.
        if (!gMapManager.InHellas() && !gMapManager.InHiddenHellas())
            return true;
        if (!Bitmaps.FindTexture(kShockWaveBitmap))
            return false;
        // Its nine-argument 0xE2BD4D path matches the existing 5.2
        // Kalima ShockWave water-terrain adapter. Native render uses the
        // incoming light directly; the subtype's computed Alpha is not
        // multiplied into that light here.
        DisableDepthMask();
        RenderWaterTerrain(kShockWaveBitmap, effect.Position[0],
            effect.Position[1], effect.Scale, effect.Scale, effect.Light,
            -effect.Angle[2], 1.f, 0.f);
        EnableDepthMask();
        return true;
    }
    if (effect.Type == kPierce8149Effect)
    {
        // Native 0x15B145C calls sprite allocator 0x172760A for subtype 2.
        // It submits the object's light/scale and owns a one-frame sprite;
        // this is not the terrain-alpha path used by the other bitmap nodes.
        if (!Bitmaps.FindTexture(kGroundStarBitmap))
            return false;
        CreateSprite(kGroundStarBitmap, effect.Position, effect.Scale,
            effect.Light, &effect, 0.f, 0);
        return true;
    }
    if (IsBitmapEffect(effect.Type))
    {
        int bitmap = 0;
        switch (effect.Type)
        {
        case kFlare01RedEffect: bitmap = kFlare01RedBitmap; break;
        case kRingOfGradation2Effect: bitmap = kRingOfGradation2Bitmap; break;
        case kEnemyRing01Effect: bitmap = kEnemyRing01Bitmap; break;
        case kMagicGround12Effect: bitmap = kMagicGround12Bitmap; break;
        case kFlareBlueEffect: bitmap = kFlareBlueBitmap; break;
        case kFlareEffect: bitmap = kFlareBitmap; break;
        case kPierce81CEEffect:
            // 0x15AE502 uses bitmap 0x81CE for subtype 3;
            // 0x15AE675/0x15AE740 use 0x81CD for subtypes 4/5.
            bitmap = effect.SubType == 3 ? kMarksM04Bitmap :
                kMarksM03Bitmap;
            break;
        case kPierce80BAEffect: bitmap = kFlare01Bitmap; break;
        default: return false;
        }
        // A failed registration must never submit the black fallback
        // texture from CGlobalBitmap::GetTexture.
        if (!Bitmaps.FindTexture(bitmap))
            return false;
        vec3_t light;
        if (effect.Type == kPierce80BAEffect)
        {
            // 0x15A925E: mode 6/7 submits 0x7EF7 with a white time wave,
            // not the incoming blue light used during child construction.
            const float wave = (sinf(WorldTime * 0.005f) + 1.f) *
                0.25f + 0.2f;
            Vector(wave, wave, wave, light);
        }
        else
            Vector(effect.Light[0] * effect.Alpha,
                effect.Light[1] * effect.Alpha,
                effect.Light[2] * effect.Alpha, light);
        // Each imported bitmap-object subtype reaches native 0x1765DF1,
        // the terrain-tile alpha renderer. None is a billboard sprite.
        const float rotation = effect.Type == kPierce80BAEffect ?
            effect.Angle[0] : effect.Type == kRingOfGradation2Effect ||
            effect.Type == kMagicGround12Effect ||
            effect.Type == kPierce81CEEffect ? 0.f : effect.Angle[2];
        // RenderEffects invokes this branch in its ordinary opaque/model pass.
        // RenderTerrainAlphaBitmap only binds and submits textured tiles; it
        // never enables blending. These S21 OZJ/JPEG effect images have black
        // backgrounds, so submitting them opaque paints black terrain squares.
        // Keep the additive bitmap pass scoped to this node; model objects
        // continue through their authored RenderBody material path below.
        EnableAlphaBlend();
        RenderTerrainAlphaBitmap(bitmap, effect.Position[0],
            effect.Position[1], effect.Scale, effect.Scale, light,
            rotation, 1.f, 5.f);
        DisableAlphaBlend();
        return true;
    }
    if (IsController(effect.Type))
        return true;
    if (!IsKnownModel(effect.Type) || !EnsureModel(effect.Type) ||
        !Calc_RenderObject(&effect, false, 0, 0))
        return false;
    BMD& model = Models[effect.Type];
    // Native 0x681 (van_swordline01) is an animation/position controller and
    // intentionally has no triangles in the supplied S21 asset.
    if (model.NumMeshs == 0)
        return true;
    // Native 0x691 and 0x694 use the generic object Calc/Draw path. In 5.2,
    // RENDER_BRIGHT selects GL_ONE/GL_ONE and ignores the per-instance Alpha
    // from both models' native fade curves. The per-frame 0x691 children then
    // sum into the white block seen in QA. Keep both fading buff models in
    // the ordinary textured alpha path, while luminous bat/trail models use
    // their additive material pass.
    const int renderFlags = effect.Type == kDetectionMarkModel ||
        effect.Type == kDetectionImpactModel ||
        effect.Type == kPierceMarksCylinderModel ? RENDER_TEXTURE :
        (RENDER_TEXTURE | RENDER_BRIGHT);
    model.RenderBody(renderFlags, effect.Alpha,
        effect.BlendMesh, effect.BlendMeshLight,
        effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
        effect.HiddenMesh);
#ifdef RISE_SLAYER_RUNTIME_QA
    LogModelRender(effect, model, renderFlags);
#endif
    return true;
#else
    (void)effect;
    return false;
#endif
}

void ApplyPlayerActionSpeeds(int attackSpeed)
{
#ifdef RISE_SLAYER_PORT
    if (!Models || Models[MODEL_PLAYER].NumActions <= kDemolishAction)
        return;
    // S21 player action speed setter 0x140A401..0x140A62D reads an
    // AttackSpeed1 term formed at 0x1408881 by AttackSpeed * 0.002.
    const float term = attackSpeed * 0.002f;
    Models[MODEL_PLAYER].Actions[kSwordInertiaAction].PlaySpeed = 0.43f + term;
    Models[MODEL_PLAYER].Actions[kBatFlockAction].PlaySpeed = 0.40f + term;
    Models[MODEL_PLAYER].Actions[kPierceAttackAction].PlaySpeed = 0.40f + term;
    Models[MODEL_PLAYER].Actions[kDetectionAction].PlaySpeed = 0.10f + term;
    Models[MODEL_PLAYER].Actions[kDemolishAction].PlaySpeed = 0.10f + term;
#else
    (void)attackSpeed;
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
    if (CharacterAttribute)
        ApplyPlayerActionSpeeds(CharacterAttribute->AttackSpeed);
    actor.CurrentAction = action;
    actor.AnimationFrame = 0.f;
    actor.PriorAnimationFrame = 0.f;
    actor.Velocity = Models[MODEL_PLAYER].Actions[action].PlaySpeed;
    if (skillId == kPierceAttack)
    {
        // S21 one-shot action initializer 0x128A082 -> 0x128BE14 writes
        // actor Position Z += 5 before its subtype-2 0x81CD child. This is
        // a visual lift, not evidence of the unrecovered XY rush/return.
        // The local 0x19 acknowledgment is consumed by the pending-graph
        // receive guard, so it does not run this action initializer twice.
        actor.Position[2] += 5.f;
        vec3_t position, angle, light;
        VectorCopy(actor.Position, position);
        VectorCopy(actor.Angle, angle);
        VectorCopy(actor.Light, light);
        CreateEffect(kPierce81CDController, position, angle, light, 2,
            &actor, -1, kPierceAttack, 0, 0, 0.f, actor.m_sTargetIndex);
    }
    return true;
#else
    (void)actor;
    (void)skillId;
    return false;
#endif
}

namespace {
bool RegisterSlayerBitmap(int id, const char* virtualPath,
    GLuint filter, GLuint wrap)
{
    const bool loaded = Bitmaps.LoadImageFile(id, virtualPath, filter, wrap);
#ifdef RISE_SLAYER_RUNTIME_QA
    if (!loaded)
    {
        char line[384];
        sprintf_s(line, sizeof(line),
            "SlayerQA bitmap-load-failed id=%d logical=%s", id,
            virtualPath);
        rise::slayerqa::AppendRuntimeQALog(line);
    }
#endif
    return loaded;
}
}

void LoadSounds()
{
#ifdef RISE_SLAYER_PORT
    RegisterSlayerBitmap(kMagicGround12Bitmap,
        "Data\\RISE\\Slayer\\Effect\\magic_ground12.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kRingOfGradation2Bitmap,
        "Data\\RISE\\Slayer\\Effect\\ring_of_gradation2.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kPinStar03Bitmap,
        "Data\\RISE\\Slayer\\Effect\\pin_star03.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kFlare01RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\flare01_red.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kEnemyRing01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\enemy_ring01.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kGhostMark02RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\gostmark02_red.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kSmoke01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smoke01.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kSmokeLines01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smokelines01.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kSmokeLines02Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smokelines02.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kSmokeLines03Bitmap,
        "Data\\RISE\\Slayer\\Effect\\smokelines03.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kPinStar02RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\pin_star02_red.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kFireHik01MagentaBitmap,
        "Data\\RISE\\Slayer\\Effect\\firehik01_magenta.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kEmpact01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\empact01.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kGhostMark02Bitmap,
        "Data\\RISE\\Slayer\\Effect\\gostmark02.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kAlphaRingX256Bitmap,
        "Data\\RISE\\Slayer\\Effect\\alpha_RingX256_1.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kDamage1MonoBitmap,
        "Data\\RISE\\Slayer\\Effect\\Damage1mono.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kGroundStarBitmap,
        "Data\\RISE\\Slayer\\Effect\\ground_star.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kFlareBlueBitmap,
        "Data\\RISE\\Slayer\\Effect\\flareBlue.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kFlareBitmap,
        "Data\\RISE\\Slayer\\Effect\\Flare.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kWaterBoardRedBitmap,
        "Data\\RISE\\Slayer\\Effect\\water_board_red.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kWaterWallBitmap,
        "Data\\RISE\\Slayer\\Effect\\water_wall.tga", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kSmoke01StrongBitmap,
        "Data\\RISE\\Slayer\\Effect\\smoke01_strong.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kBlur02MonoLongVan2Bitmap,
        "Data\\RISE\\Slayer\\Effect\\blur02_mono_long_van2.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kBlur02MonoLongVanBitmap,
        "Data\\RISE\\Slayer\\Effect\\blur02_mono_long_van.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kMarksM04Bitmap,
        "Data\\RISE\\Slayer\\Effect\\marks_m04.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kMarksM03Bitmap,
        "Data\\RISE\\Slayer\\Effect\\marks_m03.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kFlare01Bitmap,
        "Data\\RISE\\Slayer\\Effect\\flare01.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kShockWaveBitmap,
        "Data\\RISE\\Slayer\\Effect\\ShockWave.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kClud64Bitmap,
        "Data\\RISE\\Slayer\\Effect\\Clud64.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kBetGrilsShot2RedBitmap,
        "Data\\RISE\\Slayer\\Effect\\bet_grilsshot2red.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kImpack03Bitmap,
        "Data\\RISE\\Slayer\\Effect\\Impack03.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kPinStarBitmap,
        "Data\\RISE\\Slayer\\Effect\\pin_star.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kJujugBitmap,
        "Data\\RISE\\Slayer\\Effect\\jujug_R.jpg", GL_LINEAR, GL_CLAMP);
    RegisterSlayerBitmap(kWaterFall4Bitmap,
        "Data\\RISE\\Slayer\\Effect\\waterFall4.jpg", GL_LINEAR, GL_CLAMP);
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
