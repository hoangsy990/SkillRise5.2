#include "stdafx.h"
#include "GrowLancerEffectRuntime.h"
#include "GrowLancerTick.h"
#include "../../../GrowLancer/compat/MagicQuantumSequence.h"
#include "../../../GrowLancer/compat/MagicQuantumClock.h"
#include "GrowLancerRuntimeQA.h"
#include "GrowLancerResources.h"
#include "ZzzBMD.h"
#include "ZzzAI.h"
#include "ZzzCharacter.h"
#include "ZzzInfomation.h"
#include "CharacterManager.h"
#include "ZzzEffect.h"
#include "ZzzLodTerrain.h"
#include "GrowLancerTerrainAdapter.h"
#include "GrowLancerSpriteAdapter.h"
#include "GrowLancerFireParticle.h"
#include "GrowLancerBrecheGround.h"
#include "GrowLancerSpinCross.h"
#include "GrowLancerSpinFlare.h"
#include "GrowLancerSpinGround.h"
#include "GrowLancerCircleShiny.h"
#include "GrowLancerShiningMesh.h"
#include "GrowLancerHarshWind.h"
#include "GrowLancerMagicPinTick.h"
#include "GrowLancerWrathParticle.h"
#include "GrowLancerWrathGroundPulse.h"
#include "GrowLancerWrathSprites.h"
#include "GrowLancerWrathPersistentTick.h"
#include "GrowLancerCirclePersistent.h"
#include "GrowLancerWrathEmission.h"
#include "GrowLancerObsidianTick.h"
#include "GrowLancerClashTick.h"
#include "../../../GrowLancer/compat/S21CastAnimationSpeed.h"
#include "ZzzObject.h"
#include "ZzzOpenglUtil.h"
#include "SkillEffectMgr.h"
#include <cstdint>
#include "../../../GrowLancer/compat/AnimationSampleBatch.h"
#include "../../../GrowLancer/compat/AnimationSampleSlot.h"
#include "../../../GrowLancer/compat/AnimationActorIdentity.h"

namespace rise { namespace growlancer {
namespace {
AnimationSampleCursor gPrimaryAnimationCursors[MAX_EFFECTS];
AnimationSampleCursor gSecondaryAnimationCursors[MAX_SKILL_EFFECTS];
AnimationActorIdentity<uintptr_t> gAnimationActorIdentity;
AnimationSampleBatch<1> gAnimationObservation;
unsigned gAnimationObservationSerial = 0;
bool gAnimationObservationArmed = false;
MagicQuantumClock gMagicClock;
bool gMagicClockStarted = false;
bool gMagicFrameOwned = false;
bool gMagicFrameRan = false;
unsigned long long gMagicPendingBefore = 0, gMagicCompleted = 0;

class ScopedMeshTriangles
{
public:
    ScopedMeshTriangles(Mesh_t& mesh, Triangle_t* triangles, short count)
        : m_Mesh(mesh), m_OriginalTriangles(mesh.Triangles),
          m_OriginalCount(mesh.NumTriangles)
    {
        m_Mesh.Triangles = triangles;
        m_Mesh.NumTriangles = count;
    }

    ~ScopedMeshTriangles()
    {
        m_Mesh.Triangles = m_OriginalTriangles;
        m_Mesh.NumTriangles = m_OriginalCount;
    }

private:
    ScopedMeshTriangles(const ScopedMeshTriangles&);
    ScopedMeshTriangles& operator=(const ScopedMeshTriangles&);

    Mesh_t& m_Mesh;
    Triangle_t* m_OriginalTriangles;
    short m_OriginalCount;
};

enum CarrierKind
{
    kCarrierSpinGround = 1,
    kCarrierFlare = 2,
    kCarrierHole = 3,
    kCarrierCircleShiny = 10
};

bool ResolveTarget(short index, OBJECT*& target)
{
    if (index < 0 || index >= MAX_CHARACTERS_CLIENT)
        return false;
    target = &CharactersClient[index].Object;
    return target->Live;
}

bool ClashDirection(const OBJECT& owner, short targetIndex, vec3_t direction)
{
    OBJECT* target = 0;
    if (!ResolveTarget(targetIndex, target))
        return false;
    VectorSubtract(target->Position, owner.Position, direction);
    const float length = VectorLength(direction);
    // S21 D308C3 keeps XYZ and zeros tiny vectors without rejecting the cast.
    if (length < 0.000001f)
        Vector(0.0f, 0.0f, 0.0f, direction);
    else
        VectorNormalize(direction);
    return true;
}

float SymmetricAlpha(float lifetime, float maximumLifetime, float peak)
{
    const float half = maximumLifetime * 0.5f;
    const float elapsed = maximumLifetime - lifetime;
    float alpha = elapsed <= half ? elapsed / half : lifetime / half;
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    return alpha * peak;
}

bool CanPlayAction(const OBJECT& actor, unsigned short action)
{
    return Models && actor.Type >= 0 &&
        actor.Type < RuntimeModelLimit(MAX_MODELS) &&
        Models[actor.Type].NumActions > action;
}

bool PrepareFixedPlayerAction(const OBJECT& actor, unsigned short action)
{
    // Only the isolated merged player clips185..194 ->284..293.
    // These selected source actions have constant speed, independent of stats.
    if (actor.Type != MODEL_PLAYER || action < 284 || action > 293 ||
        !CanPlayAction(actor, action) || !Models[MODEL_PLAYER].Actions)
        return false;
    const float speed = S21FixedCastAnimationSpeed(action - 284 + 185);
    if (speed <= 0.0f)
        return false;
    Models[MODEL_PLAYER].Actions[action].PlaySpeed = speed;
    return true;
}

bool PrepareLocalQADynamicAction(const OBJECT& actor, unsigned short action)
{
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    // Owner-authorized local DK visual testing only. Final GL/remote inputs,
    // advanced bonus slots and Tornado delivery are separate open contracts.
    // Native Attribute already includes ability/item/buff contributions.
    // Never replay S21 flag01/08 +20 or truncate the native DWORD to WORD.
    if (!Hero || &actor != &Hero->Object || actor.Type != MODEL_PLAYER ||
        !CharacterMachine || CharacterAttribute != &CharacterMachine->Character ||
        gCharacterManager.GetBaseClass(CharacterAttribute->Class) != CLASS_KNIGHT ||
        action < 284 || action > 289 || action == 286 ||
        !CanPlayAction(actor, action) || !Models[MODEL_PLAYER].Actions)
        return false;
    const float nativeSpeed = static_cast<float>(CharacterAttribute->AttackSpeed);
    const float adjusted = nativeSpeed > S21GrowLancerInitialSpeedLimit ?
        S21GrowLancerInitialSpeedLimit : nativeSpeed;
    Models[MODEL_PLAYER].Actions[action].PlaySpeed =
        S21CastAnimationSpeed(action - 284 + 185, adjusted);
    return true;
#else
    // Do not run with an uninitialized speed or borrow the local hero's stats
    // for another actor while the final input/ownership contract is unproven.
    (void)actor;
    (void)action;
    return false;
#endif
}

void TransformFromObject(const OBJECT& object, float x, float y, float z,
    vec3_t result)
{
    vec3_t local;
    vec3_t rotated;
    float matrix[3][4];
    Vector(x, y, z, local);
    AngleMatrix(object.Angle, matrix);
    VectorRotate(local, matrix, rotated);
    VectorAdd(object.Position, rotated, result);
}

void EmitHarshStrikeBlur(OBJECT& effect, float animationFactor)
{
    OBJECT* owner = effect.Owner;
    if (!owner || !Models || owner->Type < 0 ||
        owner->Type >= RuntimeModelLimit(MAX_MODELS))
        return;

    BMD& ownerModel = Models[owner->Type];
    const unsigned short action = owner->CurrentAction;
    if (ownerModel.NumBones <= 8 || ownerModel.NumActions <= action)
        return;

    ownerModel.BodyScale = owner->Scale;
    ownerModel.CurrentAction = action;
    VectorCopy(owner->Angle, ownerModel.BodyAngle);
    VectorCopy(owner->Position, ownerModel.BodyOrigin);

    // SS21 0x15773BD..0x1577847 first evaluates the caster model, then
    // evaluates the deliberately empty type-1504 controller into the same
    // BoneTransform buffer. BMD::Animation returns immediately for that empty
    // record, so bones 8/5 below are the caster bones. The controller's
    // BodyScale/BodyOrigin are nevertheless used by TransformPosition.
    const float actionSpeed = ownerModel.Actions[action].PlaySpeed * animationFactor;
    const float sampleStep = actionSpeed / 6.0f;
    float sampleFrame = owner->AnimationFrame - actionSpeed;
    vec3_t localZero;
    vec3_t localOffset;
    vec3_t rotatedOffset;
    vec3_t start;
    vec3_t end;
    vec3_t light;
    float angleMatrix[3][4];
    Vector(0.0f, 0.0f, 0.0f, localZero);
    Vector(-30.0f, 0.0f, 100.0f, localOffset);
    AngleMatrix(effect.Angle, angleMatrix);
    VectorRotate(localOffset, angleMatrix, rotatedOffset);
    Vector(1.0f, 1.0f, 1.0f, light);

    for (int sample = 0; sample < 6; ++sample)
    {
        if (sampleFrame >= 2.0f && sampleFrame <= 4.0f)
        {
            // S21 157748F..15774B3 gates animation as well as blur output.
            ownerModel.Animation(BoneTransform, sampleFrame,
                static_cast<float>(static_cast<int>(sampleFrame) - 1),
                owner->PriorAction, owner->Angle, owner->HeadAngle, false, false);
            VectorTransform(localZero, BoneTransform[8], start);
            VectorTransform(localZero, BoneTransform[5], end);
            VectorScale(start, effect.Scale, start);
            VectorScale(end, effect.Scale, end);
            VectorAdd(start, effect.Position, start);
            VectorAdd(end, effect.Position, end);
            VectorAdd(start, rotatedOffset, start);
            VectorAdd(end, rotatedOffset, end);
            CreateObjectBlur(&effect, start, end, light, 11, false, 3, -1);
        }
        sampleFrame += sampleStep;
    }
}

void EmitSpinStepWeaponBlur(OBJECT& effect, float animationFactor)
{
    OBJECT* owner = effect.Owner;
    if (!owner || !Models || owner->Type < 0 ||
        owner->Type >= RuntimeModelLimit(MAX_MODELS))
        return;
    if (!EnsureSpinMotionBlurBitmap())
        return;

    BMD& ownerModel = Models[owner->Type];
    const unsigned short action = owner->CurrentAction;
    if (ownerModel.NumBones <= 33 || ownerModel.NumActions <= action)
        return;

    ownerModel.BodyScale = owner->Scale;
    ownerModel.CurrentAction = action;
    VectorCopy(owner->Angle, ownerModel.BodyAngle);
    VectorCopy(owner->Position, ownerModel.BodyOrigin);

    // Exact S21 0x157C2A8..0x157CA4A weapon ribbon.  Three samples span
    // action frames 0.0..4.5 on caster bone 33.  The ninth S21 blur argument
    // is render style 1; the SS6 pool adapter preserves legacy style 0 for
    // every existing call and rotates UVs only for this ribbon.
    const float actionSpeed = ownerModel.Actions[action].PlaySpeed * animationFactor;
    const float sampleStep = actionSpeed / 3.0f;
    float sampleFrame = owner->AnimationFrame - actionSpeed;
    vec3_t localStart;
    vec3_t localEnd;
    vec3_t localOffset;
    vec3_t rotatedOffset;
    vec3_t start;
    vec3_t end;
    vec3_t light;
    float angleMatrix[3][4];
    Vector(0.0f, 0.0f, 0.0f, localStart);
    Vector(0.0f, -180.0f, 20.0f, localEnd);
    Vector(0.0f, -10.0f, 0.0f, localOffset);
    AngleMatrix(effect.Angle, angleMatrix);
    VectorRotate(localOffset, angleMatrix, rotatedOffset);
    Vector(1.0f, 1.0f, 1.0f, light);

    for (int sample = 0; sample < 3; ++sample)
    {
        ownerModel.Animation(BoneTransform, sampleFrame,
            static_cast<float>(static_cast<int>(sampleFrame) - 1),
            owner->PriorAction, owner->Angle, owner->HeadAngle, false, false);
        if (sampleFrame >= 0.0f && sampleFrame <= 4.5f)
        {
            ownerModel.TransformPosition(BoneTransform[33], localStart,
                start, true);
            ownerModel.TransformPosition(BoneTransform[33], localEnd,
                end, true);
            VectorAdd(start, rotatedOffset, start);
            VectorAdd(end, rotatedOffset, end);
            CreateObjectBlur(&effect, start, end, light, 1, false, 0, -1, 1);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
            // Diagnostic only: prove that the S21 style-1 weapon ribbon
            // reaches the native blur allocator for this sampled frame.
            RecordSpinRenderQA(effect, "weapon-blur-submit", 1, 0, 33, 0,
                1);
#endif
        }
        sampleFrame += sampleStep;
    }
}

void CreateCarrier(int bitmap, const vec3_t position, const vec3_t angle,
    const vec3_t light, int kind, float scale, OBJECT* owner = 0)
{
    CreateEffect(kSpriteCarrierModel, const_cast<float*>(position),
        const_cast<float*>(angle), const_cast<float*>(light), kind, owner, -1,
        static_cast<WORD>(bitmap), 0, 0, scale);
}

void CreateSpinCross(const vec3_t position, const vec3_t angle,
    const vec3_t light, float scale, OBJECT* owner)
{
    CreateEffect(kSpinCrossModel, const_cast<float*>(position),
        const_cast<float*>(angle), const_cast<float*>(light), 0, owner, -1,
        0, kSpinStepSkill, 0, scale);
}

void EmitSpinCrosses(OBJECT& effect, float scale)
{
    vec3_t light;
    Vector(0.2f, 0.6f, 1.0f, light);
    for (int i = 0; i < 3; ++i)
    {
        // Three identical input angles at157CC1E/157CC7A/157CCD6.
        // The cross constructor sets absolute Z=120; no invented fan spread.
        CreateSpinCross(effect.Position, effect.Angle, light, scale,
            effect.Owner);
    }
}

void EmitSpinCastFanout(OBJECT& target)
{
    vec3_t light;
    Vector(.2f, .6f, 1.f, light);
    for (int i = 0; i < 3; ++i)
        CreateSpinCross(target.Position, target.Angle, light, 17.f, &target);
    Vector(0.1f, 0.15f, 0.4f, light);
    // S21 157CD44 passes null owner for ground, unlike the three crosses.
    CreateCarrier(kSpinGroundBitmap, target.Position, target.Angle, light,
        kCarrierSpinGround, 5.0f, 0);
}

void EmitSpinHitBurst(OBJECT& effect)
{
    vec3_t light;
    Vector(0.2f, 0.6f, 1.0f, light);
    CreateCarrier(kFlareBlueBitmap, effect.Position, effect.Angle, light,
        kCarrierFlare, 3.5f, effect.Owner);
    for (int i = 0; i < 5; ++i)
        CreateParticle(kFlareBlueBitmap, effect.Position, effect.Angle, light,
            4, 0.2f, effect.Owner);
    EmitSpinCrosses(effect, 12.0f);
}

void EmitSpinHoles(OBJECT& effect, float firstScale, float secondScale)
{
    vec3_t light;
    // S21 157CD85..157CDCA and157D247..157D28F both supply white.
    Vector(1.f, 1.f, 1.f, light);
    CreateCarrier(kHoleBitmap, effect.Position, effect.Angle, light,
        kCarrierHole, firstScale, effect.Owner);
    CreateCarrier(kHoleBitmap, effect.Position, effect.Angle, light,
        kCarrierHole, secondScale, effect.Owner);
}

void EmitMagicHitTick(OBJECT& effect)
{
    vec3_t position;
    vec3_t light;
    // SS21 0x157564D computes one random point for the entire six-particle
    // burst; only the 2.5/3.5 scale choice is randomized per child.
    VectorCopy(effect.Position, position);
    position[0] += static_cast<float>(rand() % 41 - 20);
    position[1] += static_cast<float>(rand() % 41 - 20);
    position[2] += static_cast<float>(rand() % 41 - 20);
    Vector(0.6f, 0.7f, 1.0f, light);
    for (int i = 0; i < 6; ++i)
    {
        CreateMagicPinParticle(BITMAP_CLUD64, position, effect.Angle, light, 19,
            (rand() % 2) ? 2.5f : 3.5f, 0);
    }
    Vector(0.3f, 0.3f, 0.5f, light);
    for (int i = 0; i < 2; ++i)
        CreateMagicPinParticle(kShockwave2Bitmap, effect.Position, effect.Angle, light,
            1, 0.69f, &effect);
}

void CreateShiningModel(int type, const vec3_t position, OBJECT& controller,
    int subtype = 0, float scale = 0.0f)
{
    CreateEffect(type, const_cast<float*>(position), controller.Angle,
        controller.Light, subtype, controller.Owner, -1, 0,
        kShiningPeakSkill, 0, scale);
}

void EmitShiningXsuper(OBJECT& controller, float x, float y, float z,
    int randomX, int biasX, int randomY, int biasY,
    int randomZ, int biasZ)
{
    vec3_t position;
    vec3_t light;
    TransformFromObject(controller, x, y, z, position);
    if (randomX) position[0] += static_cast<float>(rand() % randomX - biasX);
    if (randomY) position[1] += static_cast<float>(rand() % randomY - biasY);
    if (randomZ) position[2] += static_cast<float>(rand() % randomZ - biasZ);
    Vector(1.0f, 1.0f, 1.0f, light);
    CreateParticle(kXsuper0001Bitmap, position, controller.Angle, light,
        1, 1.8f, NULL);
}

void EmitCirclePair(OBJECT& controller, const vec3_t position)
{
    vec3_t shinyLight;
    vec3_t flareLight;
    Vector(0.3f, 0.7f, 1.0f, shinyLight);
    Vector(0.25f, 0.18f, 1.0f, flareLight);
    CreateCarrier(kCircleShinyBitmap, position, controller.Angle, shinyLight,
        kCarrierCircleShiny, 1.0f, &controller);
    const float rotation =
        static_cast<float>(static_cast<int>(WorldTime) % 180);
    CreateSprite(BITMAP_LIGHT, const_cast<float*>(position), 10.0f,
        flareLight, &controller, rotation, 0);
}

void PlaceClashChild(OBJECT& effect, float offset, float height)
{
    if (!effect.Owner)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    vec3_t direction;
    if (!ClashDirection(*effect.Owner, effect.m_sTargetIndex, direction))
    {
        effect.LifeTime = 0.0f;
        return;
    }
    VectorScale(direction, offset, effect.Direction);
    // Retain constructor input position; stored direction is applied on update.
    effect.Position[2] += height;
}

void UpdateClashChildFromStoredDirection(OBJECT& effect, float height)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0.0f;
        return;
    }
    // SS21 child constructors recover the target vector once into +0xF8.
    // Their updates only add that stored vector to the current owner position.
    ClashStoredPosition(effect.Owner->Position, effect.Direction, height, effect.Position);
}

}

static AnimationSampleCursor* FindAnimationSampleCursor(const OBJECT* effect)
{
    if (!effect) return nullptr;
    unsigned index = 0;
    const uintptr_t address = reinterpret_cast<uintptr_t>(effect);
    if (AnimationSampleSlot(address, reinterpret_cast<uintptr_t>(&Effects[0]),
        static_cast<uintptr_t>(sizeof(OBJECT)), MAX_EFFECTS, index))
    {
        return &gPrimaryAnimationCursors[index];
    }
    if (AnimationSampleSlot(address, reinterpret_cast<uintptr_t>(g_SkillEffects.GetEffect(0)),
        static_cast<uintptr_t>(sizeof(OBJECT)), MAX_SKILL_EFFECTS, index))
    {
        return &gSecondaryAnimationCursors[index];
    }
    return nullptr;
}

bool ResetEffectAnimationSamples(OBJECT* effect)
{
    AnimationSampleCursor* cursor = FindAnimationSampleCursor(effect);
    if (!cursor) return false;
    cursor->Reset();
    return true;
}

void ResetAllEffectAnimationSamples()
{
    gMagicClock.Reset();
    gMagicClockStarted = false;
    gMagicFrameOwned = false;
    gMagicFrameRan = false;
    gAnimationActorIdentity.Invalidate();
    gAnimationObservation.Invalidate();
    gAnimationObservationSerial = 0;
    gAnimationObservationArmed = false;
    for (auto& cursor : gPrimaryAnimationCursors) cursor.Reset();
    for (auto& cursor : gSecondaryAnimationCursors) cursor.Reset();
}

void RetireCharacterAnimationSamples(const OBJECT* actor)
{
    if (gAnimationActorIdentity.Matches(reinterpret_cast<uintptr_t>(actor)))
        ResetAllEffectAnimationSamples();
}

void PublishCharacterAnimationObservation(const OBJECT& actor)
{
    const uintptr_t token = reinterpret_cast<uintptr_t>(&actor);
    if (!gAnimationObservationArmed || !gAnimationActorIdentity.Matches(token)) return;
    if (!actor.Live || !Hero || &actor != &Hero->Object ||
        actor.Type != MODEL_PLAYER || SceneFlag != MAIN_SCENE ||
        gAnimationObservationSerial == 0xffffffffu)
    {
        ResetAllEffectAnimationSamples();
        return;
    }
    const unsigned generation = gAnimationActorIdentity.Bind(token);
    // One ACTUAL native animation call, possibly fractional. Do not replay
    // this sample as multiple source40ms ticks or fabricate skipped frames.
    if (!generation || !gAnimationObservation.Begin(generation,
        ++gAnimationObservationSerial, actor.CurrentAction) ||
        !gAnimationObservation.AppendStep(actor.CurrentAction, actor.AnimationFrame) ||
        !gAnimationObservation.Publish())
    {
        ResetAllEffectAnimationSamples();
        return;
    }
    // Preserve the first observed exit step, then stop collecting idle frames.
    if (actor.CurrentAction != 287) gAnimationObservationArmed = false;
}

bool ReadCharacterAnimationObservation(const OBJECT& controller,
    unsigned short& action, float& frame)
{
    if (!controller.Live || controller.Type != kMagicPinControllerModel ||
        controller.SubType != 0 || !Hero || controller.Owner != &Hero->Object ||
        !Hero->Object.Live || SceneFlag != MAIN_SCENE || !gAnimationObservationSerial)
        return false;
    const uintptr_t token = reinterpret_cast<uintptr_t>(controller.Owner);
    if (!gAnimationActorIdentity.Matches(token)) return false;
    AnimationSampleCursor* cursor = FindAnimationSampleCursor(&controller);
    if (!cursor) return false;
    const unsigned generation = gAnimationActorIdentity.Bind(token);
    // Bind rejects duplicate batches without rewinding its existing cursor.
    cursor->Bind(generation, gAnimationObservationSerial);
    return cursor->Next(gAnimationObservation, action, frame);
}

bool IsEffectType(int type)
{
    return (type >= kFirstModel && type <= kLastModel) ||
        type == kWrathBrokenBitmap;
}

bool IsSpinFlareEffect(const OBJECT& effect)
{
    // Hole8003/sub0 shares the audited flare7FDD/sub0 tick envelope.
    return effect.Type == kSpriteCarrierModel &&
        (effect.SubType == kCarrierFlare || effect.SubType == kCarrierHole);
}

bool IsSpinGroundEffect(const OBJECT& effect)
{
    return effect.Type == kSpriteCarrierModel && effect.SubType == kCarrierSpinGround;
}

bool IsCircleShinyEffect(const OBJECT& effect)
{
    return effect.Type == kSpriteCarrierModel && effect.SubType == kCarrierCircleShiny;
}

bool IsBrecheEffectType(int type)
{
    return type >= kBrecheControllerModel && type <= kBrecheOwnerTwilight01Model;
}

void InitializeEffect(OBJECT& effect)
{
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    if (effect.Type == kSpinControllerModel || effect.Type == kSpinCrossModel ||
        (effect.Type == kSpriteCarrierModel && effect.SubType == kCarrierSpinGround))
        RecordSpinOutcomeQA(effect, 2, effect.m_sTargetIndex);
#endif
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kBrecheControllerModel:
    {
        effect.Timer = 0.f;
        effect.LifeTime = 20.f;
        if (!effect.Owner)
        {
            effect.LifeTime = 0.f;
            break;
        }
        if (effect.SubType == 0)
        {
            // S21 5FD/subtype0 is the caster-side control record.  It does
            // not create the five receive children below; its primary
            // update producer owns the pin/ring/wind layers separately.
            effect.Alpha = 1.f;
            effect.AttackPoint[0] = 0;
            break;
        }
        if (effect.SubType != 1)
        {
            effect.LifeTime = 0.f;
            break;
        }
        // S21 1482645..1482818: children inherit root.Owner, NOT &root.
        vec3_t red, gold;
        Vector(1.f, .07f, .07f, red);
        Vector(1.f, .7f, .2f, gold);
        CreateEffect(kBrecheLightMarksModel, effect.Position, effect.Angle,
            red, 0, effect.Owner, -1, 0, 0, 0, 2.f);
        CreateEffect(kBrecheTwilight02Model, effect.Position, effect.Angle,
            red, 0, effect.Owner, -1, 0, 0, 0, 2.5f);
        CreateEffect(kBrecheTwilight01Model, effect.Position, effect.Angle,
            gold, 14, effect.Owner, -1, 0, 0, 0, 0.f);
        CreateEffect(kBrecheTwilight01Model, effect.Position, effect.Angle,
            gold, 14, effect.Owner, -1, 0, 0, 0, .2f);
        CreateEffect(kBrecheEmitterModel, effect.Position, effect.Angle,
            effect.Light, 15, effect.Owner, -1, 0, 0, 0, 0.f);
        break;
    }
    case kBrecheOwnerRingModel:
    case kBrecheOwnerLightMarksModel:
    case kBrecheOwnerFireRingModel:
    case kBrecheOwnerWindModel:
    case kBrecheOwnerTwilight02Model:
    case kBrecheOwnerTwilight01Model:
        // These are emitted by the S21 5FD/subtype0 primary handler, not by
        // the target receive root.  Their exact S21 records are separate
        // pool entries; keep the ownership explicit in the RISE carrier.
        effect.Timer = 0.f;
        effect.LifeTime = effect.Type == kBrecheOwnerFireRingModel ? 12.f : 20.f;
        effect.Alpha = effect.Type == kBrecheOwnerWindModel ? .9f :
            (effect.Type == kBrecheOwnerLightMarksModel ? 0.f :
            ((effect.Type == kBrecheOwnerTwilight01Model ||
              effect.Type == kBrecheOwnerTwilight02Model) ? .1f :
             (effect.Type == kBrecheOwnerFireRingModel ? 0.f : 1.f)));
        // S21's common object reset initializes +0x74 (BlendMesh) to zero.
        // wind_foce has exactly one mesh whose texture slot is also zero, so
        // RenderBody flag 2 enters the native blend-mesh branch.  Preserve
        // that selected material contract explicitly; the generic private
        // carrier default of -2 is broader than the S21 object state.
        if (effect.Type == kBrecheOwnerWindModel)
            effect.BlendMesh = 0;
        break;
    case kBrecheLightMarksModel:
    case kBrecheTwilight02Model:
    case kBrecheTwilight01Model:
    case kBrecheEmitterModel:
        effect.Timer = 0.f;
        effect.LifeTime = 20.f;
        effect.Alpha = effect.Type == kBrecheLightMarksModel ? 0.f :
            (effect.Type == kBrecheEmitterModel ? 1.f : .1f);
        break;
    case kWrathBrokenBitmap:
        // Exact SS21 0x1480FD7..0x148107A (effect 0x81E3).
        effect.LifeTime = 30.0f;
        effect.Timer = 0.0f;
        effect.Scale = 4.0f;
        effect.Alpha = 1.0f;
        Vector(1.0f, 1.0f, 1.0f, effect.Light);
        break;
    case kSpriteCarrierModel:
        if (effect.SubType == kCarrierFlare || effect.SubType == kCarrierHole)
        {
            InitializeSpinFlare(effect, rand() % 180);
        }
        else if (effect.SubType == kCarrierSpinGround)
        {
            InitializeSpinGround(effect);
        }
        else if (effect.SubType == kCarrierCircleShiny)
        {
            // Exact S21 Shiny04 subtype-0 constructor 0x147FF27..0x147FFC6.
            effect.LifeTime = 15.0f;
            effect.Timer = 0.f;
            effect.Scale = 3.0f;
            effect.Alpha = 0.0f;
            effect.Angle[0] = 0.0f;
            effect.Angle[1] = 0.0f;
            effect.Angle[2] = static_cast<float>(static_cast<int>(WorldTime) % 360);
        }
        else
        {
            effect.LifeTime = 10.0f;
            effect.Alpha = 1.0f;
            effect.Angle[2] += static_cast<float>(rand() % 360);
        }
        break;
    case kSpinControllerModel:
        effect.LifeTime = effect.SubType == 0 ? 32.0f :
            (effect.SubType == 1 ? 10.0f : 15.0f);
        effect.AttackPoint[0] = 0;
        effect.AttackPoint[1] = 0;
        if (effect.SubType == 1 || effect.SubType == 2)
        {
            effect.Angle[2] = static_cast<float>(rand() % 180);
            effect.Alpha = 0.f;
        }
        if (effect.SubType == 1)
        {
            vec3_t light;
            // S21 1481A61..1481BC8 supplies white, despite flareBlue filename.
            Vector(1.f, 1.f, 1.f, light);
            CreateCarrier(kFlareBlueBitmap, effect.Position, effect.Angle,
                light, kCarrierFlare, 5.0f, effect.Owner);
            for (int i = 0; i < 5; ++i)
                CreateParticle(kFlareBlueBitmap, effect.Position,
                    effect.Angle, light, 4, 0.2f, effect.Owner);
        }
        break;
    case kSpinCrossModel:
        effect.Timer = 0.f;
        effect.LifeTime = 10.0f;
        effect.Distance = effect.Scale;
        effect.Scale *= 0.5f;
        effect.Alpha = 1.0f;
        // S21 1481CED..1481CF5 stores a constant, not angle + constant.
        effect.Angle[2] = 120.0f;
        break;
    case kMagicPinControllerModel:
        effect.LifeTime = effect.SubType == 0 ? 75.0f : 8.0f;
        effect.Timer = 0.0f;
        if (effect.SubType == 0) effect.Scale = 1.0f;
        effect.AttackPoint[0] = 0;
        effect.AttackPoint[1] = 0;
        break;
    case kMagicPin01Model:
        effect.LifeTime = 10.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.7f;
        effect.Alpha = 0.0f;
        break;
    case kMagicPin03Model:
    {
        effect.LifeTime = 10.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.85f;
        effect.Alpha = 0.0f;
        if (effect.SubType == 0)
        {
            effect.Position[0] += static_cast<float>(rand() % 50 - 25);
            effect.Position[1] += static_cast<float>(rand() % 50 - 25);
            effect.Position[2] += static_cast<float>(rand() % 50 - 25);
        }
        vec3_t position;
        vec3_t light;
        TransformFromObject(effect, 0.0f, -330.0f, 95.0f, position);
        Vector(0.48f, 0.73f, 1.0f, light);
        CreateMagicPinParticle(BITMAP_ENERGY, position, effect.Angle, light,
            effect.SubType == 0 ? 9 : 10, 1.0f, NULL);
        TransformFromObject(effect, 0.0f, 0.0f, -30.0f, position);
        CreateEffect(kMagicPinAuxModel, position, effect.Angle, light,
            0, &effect, -1, 0, kMagicPinSkill);
        break;
    }
    case kMagicPinRootModel:
        effect.LifeTime = 14.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.7f;
        effect.Alpha = 1.0f;
        effect.Angle[2] += 0.1f;
        break;
    case kMagicPinAuxModel:
        effect.LifeTime = 10.0f;
        effect.Timer = 0.0f;
        effect.Scale = 1.2f;
        effect.Alpha = 1.0f;
        break;
    case kHarshStrikeControllerModel:
        effect.LifeTime = 40.0f;
        effect.Timer = 0.0f;
        effect.Scale = 3.8f;
        effect.AttackPoint[0] = 0;
        if (effect.Owner && Models && effect.Owner->Type >= 0 &&
            effect.Owner->Type < RuntimeModelLimit(MAX_MODELS) &&
            Models[effect.Owner->Type].NumActions > 284)
        {
            effect.Velocity = Models[effect.Owner->Type].Actions[284].PlaySpeed;
        }
        break;
    case kHarshWind01Model:
        effect.LifeTime = 8.0f;
        effect.Timer = 0.0f;
        effect.Scale = 1.0f;
        effect.Angle[0] -= 90.0f;
        effect.Alpha = 1.0f;
        Vector(1.0f, 1.0f, 1.0f, effect.Light);
        break;
    case kHarshWind02Model:
        effect.LifeTime = 7.0f;
        effect.Timer = 0.0f;
        effect.Scale = 1.0f;
        effect.Angle[0] -= 90.0f;
        effect.Alpha = 1.0f;
        Vector(1.0f, 0.6f, 0.2f, effect.Light);
        break;
    case kShiningPeakControllerModel:
        effect.LifeTime = effect.SubType == 0 ? 100.0f : 60.0f;
        effect.Scale = effect.SubType == 0 ? 0.1f : 1.0f;
        effect.AttackPoint[0] = 0;
        if (effect.SubType == 1)
        {
            vec3_t position;
            vec3_t targetPosition;
            vec3_t angle;
            vec3_t light;
            TransformFromObject(effect, 0.0f, 0.0f, 35.0f, position);
            VectorCopy(effect.Position, targetPosition);
            Vector(0.65f, 0.8f, 1.0f, light);

            // SS21 0x1481637..0x14817F9 calls the joint allocator three
            // times.  Its first 13 arguments map directly to SS6 CreateJoint;
            // the fourteenth SS21-only argument is zero in every call.
            VectorCopy(effect.Angle, angle);
            angle[1] += static_cast<float>(rand() % 40 - 20);
            CreateJoint(BITMAP_PIN_LIGHT, position, targetPosition, angle, 2,
                effect.Owner, 12.0f, -1, 0, 0, -1, 0, -1);

            position[2] += 80.0f;
            VectorCopy(effect.Angle, angle);
            angle[1] = static_cast<float>(rand() % 360);
            CreateJoint(BITMAP_PIN_LIGHT, position, targetPosition, angle, 3,
                effect.Owner, 12.0f, -1, 0, 0, -1, 0, -1);

            VectorCopy(effect.Angle, angle);
            angle[1] = -static_cast<float>(rand() % 360);
            CreateJoint(BITMAP_PIN_LIGHT, position, targetPosition, angle, 4,
                effect.Owner, 12.0f, -1, 0, 0, -1, 0, -1);
        }
        break;
    case kShiningPeakBodyModel:
        effect.LifeTime = 12.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.9f;
        effect.Alpha = 0.2f;
        break;
    case kShiningPeakPinModel:
        effect.LifeTime = 8.0f;
        effect.Timer = 0.0f;
        effect.Scale = 1.8f;
        effect.Alpha = 0.0f;
        Vector(0.65f, 0.8f, 1.0f, effect.Light);
        break;
    case kShiningPeakSpinModel:
        effect.LifeTime = 20.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.8f;
        effect.Alpha = 0.0f;
        break;
    case kShiningPeakShockwaveModel:
        // SS21 0x14700FE..0x1470205, MODEL_SHOCKWAVE01 subtype 6.
        // CreateEffect has already copied the caller-provided scale.
        effect.LifeTime = 6.0f;
        effect.Timer = 0.0f;
        effect.Alpha = 0.85f;
        effect.Angle[1] -= 90.0f;
        effect.Angle[2] -= 90.0f;
        Vector(0.2f, 0.3f, 1.0f, effect.Light);
        break;
    case kWrathControllerModel:
        effect.LifeTime = 70.0f;
        effect.Timer = 0.0f;
        effect.Scale = 1.0f;
        effect.AttackPoint[0] = 0;
        break;
    case kWrathTravelModel:
        effect.LifeTime = 20.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.6f;
        effect.Alpha = 0.6f;
        break;
    case kWrathAuraModel:
        effect.LifeTime = 30.0f;
        effect.Timer = 0.0f;
        effect.Scale = effect.SubType == 0 ? 0.95f : 0.7f;
        effect.Alpha = 0.0f;
        effect.BlendMeshTexCoordU = effect.SubType == 1 ? 0.25f : 0.0f;
        effect.BlendMeshTexCoordV = -0.7f;
        break;
    case kCircleShieldControllerModel:
        effect.LifeTime = 30.0f;
        effect.Timer = 0.f;
        if (effect.SubType == 0)
        {
            vec3_t basePosition;
            vec3_t local;
            vec3_t rotated;
            vec3_t position;
            vec3_t randomAngle;
            float matrix[3][4];
            TransformFromObject(effect, 0.0f, -50.0f, 0.0f, basePosition);
            Vector(0.0f, -300.0f, 0.0f, local);
            for (int i = 0; i < 20; ++i)
            {
                Vector(static_cast<float>(rand() % 90), 0.0f,
                    static_cast<float>(rand() % 360), randomAngle);
                AngleMatrix(randomAngle, matrix);
                VectorRotate(local, matrix, rotated);
                VectorSubtract(basePosition, rotated, position);
                position[2] += 120.0f;
                CreateJoint(BITMAP_FORCEPILLAR, position, basePosition,
                    randomAngle, 2, effect.Owner, 20.0f, -1, 0, 0, -1,
                    0, -1);
            }
        }
        break;
    case kCircleShieldModel:
        effect.LifeTime = 10.0f;
        effect.Timer = 0.f;
        effect.Scale = 1.0f;
        effect.Angle[1] += 70.0f;
        break;
    case kObsidianRotatingModel:
        effect.LifeTime = 15.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.85f;
        effect.Alpha = 0.0f;
        effect.Angle[2] += 20.0f;
        effect.BlendMeshTexCoordU = 0.0f;
        effect.BlendMeshTexCoordV = -0.45f;
        break;
    case kObsidianAuraModel:
    {
        effect.LifeTime = 20.0f;
        effect.Timer = 0.0f;
        effect.Scale = 0.7f;
        effect.Alpha = 0.0f;
        effect.Angle[2] += 0.1f;
        vec3_t local;
        vec3_t rotated;
        vec3_t position;
        vec3_t light;
        float matrix[3][4];
        Vector(-10.0f, 0.0f, 170.0f, local);
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        VectorAdd(effect.Position, rotated, position);
        Vector(0.5f, 0.95f, 1.0f, light);
        CreateParticle(BITMAP_ENERGY, position, effect.Angle, light, 8,
            2.0f, NULL);
        break;
    }
    case kClashControllerModel:
        effect.LifeTime = 30.0f;
        effect.Timer = 0.0f;
        effect.AttackPoint[0] = 0;
        break;
    case kClashFrontModel:
        effect.LifeTime = 27.0f;
        effect.Timer = 0.0f;
        effect.Alpha = 0.0f;
        PlaceClashChild(effect, 30.0f, 150.0f);
        break;
    case kClashRearModel:
        effect.LifeTime = 10.0f;
        effect.Timer = 0.0f;
        effect.Alpha = 0.0f;
        PlaceClashChild(effect, -50.0f, 0.0f);
        break;
    default:
        // Other statically recovered types are registered but remain
        // fail-closed until their exact initializer is wired.
        effect.LifeTime = 0.0f;
        break;
    }
}

void BeginMagicPinFrame()
{
    gMagicPendingBefore = gMagicCompleted = 0;
    gMagicFrameOwned = false;
    gMagicFrameRan = false;
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    // Explicit isolated-process opt-in; never enabled in a normal client build.
    static const bool enabled = []()
    {
        char value[2] = {};
        return GetEnvironmentVariableA("RISE_GL_MAGIC_FIXED_TICK_QA", value, 2) == 1 &&
            value[0] == '1';
    }();
    if (!enabled) return;
    // The opt-in clock belongs to an active Magic cast or its live model
    // records. Do not replay an unrelated SS6 action after Magic has cleaned
    // up; target-owned internal 281 can still keep its own controller alive.
    const auto hasLiveMagicModel = []()
    {
        if (!Effects) return false;
        for (int i = 0; i < MAX_EFFECTS; ++i)
        {
            const OBJECT& effect = Effects[i];
            if (effect.Live && (effect.Type == kMagicPinControllerModel ||
                effect.Type == kMagicPin01Model ||
                effect.Type == kMagicPin03Model ||
                effect.Type == kMagicPinRootModel ||
                effect.Type == kMagicPinAuxModel)) return true;
        }
        return false;
    };
    if (!Hero || !Hero->Object.Live || Hero->Object.Type != MODEL_PLAYER ||
        SceneFlag != MAIN_SCENE ||
        !gAnimationActorIdentity.Matches(reinterpret_cast<uintptr_t>(&Hero->Object)) ||
        (Hero->Object.CurrentAction != 287 && !hasLiveMagicModel()))
    {
        gMagicClock.Reset();
        gMagicClockStarted = false;
        return;
    }
    const unsigned now = GetTickCount();
    if (!gMagicClockStarted)
    {
        gMagicClock.Start(now);
        gMagicClockStarted = true;
    }
    if (gMagicClock.Observe(now))
    {
        gMagicFrameOwned = true;
        gMagicPendingBefore = gMagicClock.PendingMilliseconds();
    }
#endif
}

bool MagicPinFrameOwnsActor(const OBJECT& actor)
{
    return gMagicFrameOwned && Hero && &actor == &Hero->Object &&
        actor.CurrentAction == 287;
}

bool MagicPinFrameOwnsParticles() { return gMagicFrameOwned; }

bool MagicPinFrameOwnsModel(const OBJECT& effect)
{
    return gMagicFrameOwned && (effect.Type == kMagicPinControllerModel ||
        effect.Type == kMagicPin01Model || effect.Type == kMagicPin03Model ||
        effect.Type == kMagicPinRootModel || effect.Type == kMagicPinAuxModel);
}

void RunMagicPinFrame()
{
    if (!gMagicFrameOwned || gMagicFrameRan || !Hero) return;
    gMagicFrameRan = true;
    // Consume only completed batches; no conversion through the capped FPS factor.
    while (gMagicClock.Due())
    {
        const unsigned ticks = gMagicClock.Due() > 0xffffffffull ?
            0xffffffffu : static_cast<unsigned>(gMagicClock.Due());
        const unsigned completed = RunMagicPinQuanta(Hero->Object, ticks);
        gMagicCompleted += completed;
        if (!gMagicClock.Consume(completed) || completed != ticks) break;
    }
}

void EndMagicPinFrame()
{
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    RecordMagicFrameQA(gMagicFrameOwned, gMagicFrameRan, gMagicPendingBefore,
        gMagicClock.PendingMilliseconds(), gMagicCompleted);
#endif
    gMagicFrameOwned = false;
}

unsigned RunMagicPinQuanta(OBJECT& actor, unsigned ticks)
{
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    auto valid = [&actor]()
    {
        return Hero && &actor == &Hero->Object && actor.Live &&
            actor.Type == MODEL_PLAYER && SceneFlag == MAIN_SCENE &&
            gAnimationActorIdentity.Matches(reinterpret_cast<uintptr_t>(&actor));
    };
    auto animation = [&actor]() { StepMagicPinActor(actor); };
    auto models = []() { StepMagicPinModels(); };
    auto particles = []() { StepMagicPinParticles(); };
    return RunMagicQuantumSequence(ticks, valid, animation, models, particles);
#else
    (void)actor;
    (void)ticks;
    return 0;
#endif
}

void StepMagicPinModels()
{
    auto select = [](const OBJECT& effect)
    {
        return effect.Type == kMagicPinControllerModel ||
            effect.Type == kMagicPin01Model || effect.Type == kMagicPin03Model ||
            effect.Type == kMagicPinRootModel || effect.Type == kMagicPinAuxModel;
    };
    auto update = [](OBJECT& effect, float factor) { UpdateEffect(effect, factor); };
    auto destroy = [](OBJECT* effect) { EffectDestructor(effect); };
    StepPrimaryModelPhase(Effects, MAX_EFFECTS, select, update, destroy);
}

namespace
{
void UpdateBrecheOwnerControllerPosition(OBJECT& effect)
{
    if (!effect.Owner || !effect.Owner->Live)
        return;

    // S21 0x15354EA..0x1535561 rotates the controller-local vector
    // (0,-200,0) by the controller Angle and writes the result directly to
    // the controller Position (D3189D's third argument).  It then adds the
    // owner snapshot that the generic allocator keeps at +0x1D0.  In native
    // RISE terms that is owner world position plus the rotated local offset;
    // do not add the owner position twice or copy the S21 offset literally.
    vec3_t localOffset, rotatedOffset;
    float matrix[3][4];
    Vector(0.f, -200.f, 0.f, localOffset);
    AngleMatrix(effect.Angle, matrix);
    VectorRotate(localOffset, matrix, rotatedOffset);
    VectorAdd(effect.Owner->Position, rotatedOffset, effect.Position);
}

void SpawnBrecheOwnerPinLights(const OBJECT& effect)
{
    // S21 0x1535950..0x1535BDB runs ten iterations.  The call at 0x1535BC9
    // is the extended CreateJoint family with type 0x8073, subtype 5,
    // target NULL and scale 6.0.  RISE's native CreateJoint has the same
    // semantic fields; its S21-only trailing argument is intentionally not
    // copied.
    vec3_t color;
    Vector(1.f, .7f, .15f, color);
    for (int i = 0; i < 10; ++i)
    {
        vec3_t position;
        VectorCopy(effect.Position, position);
        position[0] += static_cast<float>(rand() % 101 - 50);
        position[1] += static_cast<float>(rand() % 101 - 50);
        vec3_t angle;
        VectorCopy(effect.Angle, angle);
        // S21 calls 0x1267C3C with [-30, 30] for local X, then adds the
        // controller angle.  Keep the full signed range; the previous
        // rand()%31 expression only produced [-30, 0] and biased the joint.
        angle[0] += static_cast<float>(rand() % 61 - 30);
        angle[2] += static_cast<float>(rand() % 21 + 70);
        CreateJoint(BITMAP_PIN_LIGHT, position, position, angle, 5, NULL,
            6.f, -1, 0, 0, -1, color, -1);
    }
}

void SpawnBrecheOwnerParticles(const OBJECT& effect)
{
    // The three source branches are the same 8084/0, 806E/4 and 8085/0
    // family used by the receive emitter.  They have no owner pointer in
    // S21.  The caller passes the controller Position directly (there is no
    // caller-side random XYZ offset); the only random scalar is the helper's
    // integer 1..2 result (0x1267C3C, divisor 1).  Keep the native particle
    // pool and its private Breche tick mode.
    vec3_t light;
    Vector(.4f, .4f, .4f, light);
    for (int i = 0; i < 3; ++i)
    {
        vec3_t position;
        VectorCopy(effect.Position, position);
        const float scale = static_cast<float>(rand() % 2 + 1);
        CreateBrecheFireParticle(rand() % 3, position,
            const_cast<float*>(effect.Angle), light, scale);
    }
}

void CreateBrecheOwnerTimedLayers(OBJECT& effect)
{
    if (!effect.Owner || !effect.Owner->Live)
        return;

    // The S21 timed calls read both Angle and Position from the owner pointer
    // (0x34C + 0x164 / +0x158), except the fire/twilight calls whose equivalent
    // position is the controller's allocator snapshot at +0x1D0.  Both sources
    // are the live caster position here; none of these visible ground layers
    // uses the controller's forward -200 carrier offset.
    const OBJECT& owner = *effect.Owner;
    vec3_t ownerAngle;
    vec3_t ownerPosition;
    VectorCopy(owner.Angle, ownerAngle);
    VectorCopy(owner.Position, ownerPosition);
    vec3_t red;
    Vector(1.f, .1f, .1f, red);
    if (effect.LifeTime == 17.f)
    {
        // S21 0x1535C80 (0x80BC/sub1, scale 5.5) followed by
        // 0x1535CEA (0x81EC/sub0, scale 6.0).
        CreateEffect(kBrecheOwnerRingModel, ownerPosition, ownerAngle,
            red, 1, effect.Owner, -1, 0, kBrecheSkill, 0, 5.5f);
        CreateEffect(kBrecheOwnerLightMarksModel, ownerPosition, ownerAngle,
            red, 0, effect.Owner, -1, 0, kBrecheSkill, 0, 6.f);
    }
    if (effect.LifeTime == 7.f)
    {
        vec3_t white;
        Vector(1.f, 1.f, 1.f, white);
        // S21 0x1535DB1: 0x81EB/sub0, scale 6.5.
        // S21 passes the generic owner snapshot (+0x1D0) here, not the
        // controller Position after its -200 local offset was applied.
        CreateEffect(kBrecheOwnerFireRingModel, ownerPosition, ownerAngle,
            white, 0, effect.Owner, -1, 0, kBrecheSkill, 0, 6.5f);
    }
    if (effect.LifeTime == 15.f)
    {
        // S21 0x15360DB: 0x809F/sub2, scale 3.0, and two 0x809E/sub13
        // records with supplied scale 0 (native constructor default 0.9).
        // The two twilight records likewise read +0x1D0 (owner world
        // position); the ring/marks above intentionally use effect.Position.
        CreateEffect(kBrecheOwnerTwilight02Model, ownerPosition, ownerAngle,
            red, 2, effect.Owner, -1, kBrecheSkill, 0, 0, 3.f);
        CreateEffect(kBrecheOwnerTwilight01Model, ownerPosition, ownerAngle,
            red, 13, effect.Owner, -1, kBrecheSkill, 0, 0, 0.f);
        CreateEffect(kBrecheOwnerTwilight01Model, ownerPosition, ownerAngle,
            red, 13, effect.Owner, -1, kBrecheSkill, 0, 0, 0.f);
    }
    if (effect.LifeTime == 7.f)
    {
        // S21 0x1535DCB..0x1535FC7 copies the allocator owner snapshot,
        // subtracts exactly 5.0 from local Y, and creates AD9/subtype6 once
        // inside the same remaining-life-7 gate.  Creating it every tick at
        // the -200 controller position produced the oversized, displaced
        // white arc observed in the 5.2 QA client.
        vec3_t windPosition;
        vec3_t windLight;
        VectorCopy(ownerPosition, windPosition);
        windPosition[1] -= 5.f;
        // S21 0x1535E57..0x1535F18 builds (1,.2,.2), then multiplies
        // every component by the exact 3.0 constant before the AD9 call.
        Vector(3.f, .6f, .6f, windLight);
        CreateEffect(kBrecheOwnerWindModel, windPosition, ownerAngle,
            windLight, 6, effect.Owner, -1, kBrecheSkill, 0, 0, 3.f);
    }
}

void UpdateBrecheOwnerTick(OBJECT& effect)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0.f;
        return;
    }

    // 0x15356B2..0x15357D4: three random particle branches per update.
    SpawnBrecheOwnerParticles(effect);
    // The first particle loop runs before this S21 controller-position
    // rewrite; all pin/timed layers below consume the rewritten position.
    UpdateBrecheOwnerControllerPosition(effect);
    SpawnBrecheOwnerPinLights(effect);
    CreateBrecheOwnerTimedLayers(effect);
}
}

void UpdateEffect(OBJECT& effect, float animationFactor)
{
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    RecordControllerQASample(effect, animationFactor);
#endif
    switch (effect.Type)
    {
    case kBrecheControllerModel:
        if (effect.SubType == 0)
            UpdateBrecheOwnerTick(effect);
        // Empty cached S21 model: no mesh/action; native tick owns expiry.
        break;
    case kBrecheLightMarksModel:
        UpdateBrecheGroundTick(effect, kBrecheMarks);
        break;
    case kBrecheTwilight02Model:
    case kBrecheTwilight01Model:
        UpdateBrecheGroundTick(effect, effect.Type == kBrecheTwilight01Model
            ? kBrecheTwilight01 : kBrecheTwilight02);
        break;
    case kBrecheEmitterModel:
    {
        if (!effect.Owner)
        {
            effect.Live = false;
            break;
        }
        OBJECT& owner = *effect.Owner;
        if (!Models || owner.Type < 0 || owner.Type >= RuntimeModelLimit(MAX_MODELS))
            break;
        BMD& model = Models[owner.Type];
        // Compatibility guard: never reproduce S21's uninitialized position
        // on a model with no bones. Preserve native cached pose/BodyScale.
        if (model.NumBones <= 0 || model.NumBones > MAX_BONES)
            break;
        for (int emission = 0; emission < 2; ++emission)
        {
            vec3_t position, light;
            model.TransformByObjectBone(position, &owner, rand() % model.NumBones);
            Vector(1.f, .2f, 0.f, light);
            CreateBrecheSprite(BITMAP_LIGHT, position, 4.f, light, &owner, 0.f);
            Vector(1.f, 1.f, 1.f, light);
            const float scale = (rand() % 5 + 13) * .1f;
            const int variant = rand() % 3;
            CreateBrecheFireParticle(variant, position, effect.Angle, light, scale);
        }
        break;
    }
    case kBrecheOwnerRingModel:
        // Exact S21 0x80BC/subtype1 branch (0x1518760..0x1518819):
        // retain the ground position, but contract both the quad and its RGB
        // by 1.1 every native tick.  Omitting this made the imported ring look
        // like one frozen frame even though the carrier was still alive.
        effect.Scale /= 1.1f;
        effect.Light[0] /= 1.1f;
        effect.Light[1] /= 1.1f;
        effect.Light[2] /= 1.1f;
        break;
    case kBrecheOwnerFireRingModel:
        // Exact S21 0x81EB/subtype0 branch (0x15385D2..0x153868B):
        // constructor fixes life/max-life to 12 and alpha to zero. Rotate
        // 15 degrees and apply +/- 1/(12*0.5), i.e. one sixth per tick.
        effect.Angle[2] += 15.f;
        effect.Alpha += effect.LifeTime > 6.f ? (1.f / 6.f) : -(1.f / 6.f);
        break;
    case kBrecheOwnerLightMarksModel:
        UpdateBrecheGroundTick(effect, kBrecheMarks);
        break;
    case kBrecheOwnerTwilight02Model:
        // Exact S21 809F/sub2 branch: scale -0.1 and Angle.z +15.
        effect.Scale -= .1f;
        effect.Angle[2] += 15.f;
        break;
    case kBrecheOwnerTwilight01Model:
        // Exact S21 809E/sub13 branch for skill 0x117: scale +0.5,
        // Angle.z +15 and the shared +/-0.1 alpha envelope.
        effect.Scale += .5f;
        effect.Angle[2] += 15.f;
        effect.Alpha += effect.LifeTime >= 11.f ? .1f :
            (effect.LifeTime <= 10.f ? -.1f : 0.f);
        break;
    case kBrecheOwnerWindModel:
        // Exact S21 0xAD9/subtype6 branch (0x151D307..0x151D459): the
        // owner-side wind rotates rapidly, fades by 0.01 and compounds the
        // stored RGB by the current alpha.  This remains owner-local and has
        // no target pointer to follow.
        effect.Angle[2] -= 50.f;
        effect.Alpha -= .01f;
        effect.Light[0] *= effect.Alpha;
        effect.Light[1] *= effect.Alpha;
        effect.Light[2] *= effect.Alpha;
        break;
    case kWrathBrokenBitmap:
        // Exact SS21 0x1579AB4..0x1579BBD. The source mutates the stored
        // light rather than applying alpha only at render time. MoveEffect
        // invokes this branch once per accumulated native tick.
        if (effect.LifeTime < 11.0f)
            effect.Alpha -= 0.11f * animationFactor;
        effect.Light[0] *= effect.Alpha;
        effect.Light[1] *= effect.Alpha;
        effect.Light[2] *= effect.Alpha;
        break;
    case kSpriteCarrierModel:
        if (effect.SubType == kCarrierFlare || effect.SubType == kCarrierHole)
        {
            UpdateSpinFlareTick(effect);
        }
        else if (effect.SubType == kCarrierSpinGround)
        {
            UpdateSpinGroundTick(effect);
        }
        else if (effect.SubType == kCarrierCircleShiny)
        {
            UpdateCircleShinyTick(effect);
        }
        else
        {
            effect.Alpha = SymmetricAlpha(effect.LifeTime, 10.0f, 1.0f);
        }
        break;
    case kSpinControllerModel:
        if (effect.SubType == 0)
        {
            if (!effect.Owner || !effect.Owner->Live ||
                effect.Owner->CurrentAction != 285)
            {
                effect.LifeTime = 0.0f;
                return;
            }
            EmitSpinStepWeaponBlur(effect, animationFactor);
            const float frame = effect.Owner->AnimationFrame;
            if (effect.AttackPoint[0] == 0 && frame > 4.7f && frame <= 6.5f)
            {
                effect.AttackPoint[0] = 1;
                OBJECT* target = 0;
                const bool targetResolved = ResolveTarget(effect.m_sTargetIndex, target);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
                RecordSpinOutcomeQA(effect, targetResolved ? 1 : 0, effect.m_sTargetIndex);
#endif
                if (!targetResolved)
                    break;
                CreateEffect(kSpinControllerModel, target->Position,
                    target->Angle, target->Light, 1, target,
                    -1, 0, kSpinStepSkill);
                EmitSpinCastFanout(*target);
            }
        }
        else if (effect.SubType == 1 && effect.LifeTime <= 5.0f &&
            effect.AttackPoint[0] == 0)
        {
            EmitSpinHoles(effect, 2.0f, 3.0f);
            effect.AttackPoint[0] = 1;
        }
        else if (effect.SubType == 2)
        {
            if (effect.LifeTime <= 10.0f && effect.AttackPoint[0] == 0)
            {
                EmitSpinHitBurst(effect);
                effect.AttackPoint[0] = 1;
            }
            if (effect.LifeTime <= 5.0f && effect.AttackPoint[1] == 0)
            {
                EmitSpinHoles(effect, 1.4f, 2.1f);
                effect.AttackPoint[1] = 1;
            }
        }
        break;
    case kSpinCrossModel:
        UpdateSpinCrossTick(effect);
        break;
    case kMagicPinControllerModel:
        if (effect.SubType == 1)
        {
            if (MagicPinHitDue(effect.LifeTime))
                EmitMagicHitTick(effect);
            break;
        }
        if (!effect.Owner || !effect.Owner->Live ||
            effect.Owner->CurrentAction != 287)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        {
            const float frame = effect.Owner->AnimationFrame;
            vec3_t position;
            if (effect.AttackPoint[0] == 0 && MagicPinStageDue(0, frame))
            {
                ++effect.AttackPoint[0];
                // S21 1578334/1578395: controller angle/light/ownership,
                // not the current caster transform. Owner supplies frame only.
                CreateEffect(kMagicPin01Model, effect.Position,
                    effect.Angle,
                    effect.Light, 0, &effect, -1, 0, kMagicPinSkill);
                TransformFromObject(effect, 0.0f, 20.0f, 30.0f,
                    position);
                CreateEffect(kMagicPin03Model, position, effect.Angle,
                    effect.Light, 0, &effect, -1, 0, kMagicPinSkill);
                VectorCopy(position, effect.StartPosition);
            }
            if (effect.AttackPoint[0] == 1 && MagicPinStageDue(1, frame))
            {
                ++effect.AttackPoint[0];
                CreateEffect(kMagicPin03Model, effect.StartPosition,
                    effect.Angle,
                    effect.Light, 0, &effect, -1, 0, kMagicPinSkill);
            }
            if (effect.AttackPoint[0] == 2 && MagicPinStageDue(2, frame))
            {
                ++effect.AttackPoint[0];
                CreateEffect(kMagicPin03Model, effect.StartPosition,
                    effect.Angle,
                    effect.Light, 1, &effect, -1, 0, kMagicPinSkill);
            }
        }
        break;
    case kMagicPin01Model:
        // S21 1578657..15786D2: incremental alpha, maxLife10 / 2.
        // Life10 starts at .2, life6 peaks1; life0 ends at -.2, no clamp.
        UpdateMagicPin01Tick(effect);
        break;
    case kMagicPin03Model:
        UpdateMagicPin03Tick(effect);
        {
            vec3_t local;
            vec3_t rotated;
            float matrix[3][4];
            Vector(0.0f, 1.0f * animationFactor, 0.0f, local);
            AngleMatrix(effect.Angle, matrix);
            VectorRotate(local, matrix, rotated);
            VectorAdd(effect.Position, rotated, effect.Position);
        }
        break;
    case kMagicPinRootModel:
        UpdateMagicPinRootTick(effect);
        break;
    case kMagicPinAuxModel:
        // 157890B -> 96A03C -> CRT sin(double), rounded to float;
        // argument30 is radians (x87 fallback1A36DAD fsin), not degrees.
        UpdateMagicPinAuxTick(effect);
        break;
    case kHarshStrikeControllerModel:
        if (!effect.Owner || !effect.Owner->Live ||
            effect.Owner->CurrentAction != 284)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        {
            const float frame = effect.Owner->AnimationFrame;
            vec3_t position;
            EmitHarshStrikeBlur(effect, animationFactor);
            if (effect.AttackPoint[0] == 0 && frame >= 5.2f && frame <= 5.8f)
            {
                TransformFromObject(effect, 0.0f, -120.0f, 90.0f, position);
                CreateEffect(kHarshWind01Model, position, effect.Angle,
                    effect.Light, 0, 0, -1, 0, kHarshStrikeSkill);
                TransformFromObject(effect, 0.0f, -180.0f, 90.0f, position);
                CreateEffect(kHarshWind02Model, position, effect.Angle,
                    effect.Light, 0, 0, -1, 0, kHarshStrikeSkill);
                effect.AttackPoint[0] = 1;
            }
            if (effect.AttackPoint[0] == 1 && frame >= 5.3f && frame <= 6.0f)
            {
                TransformFromObject(effect, 0.0f, -380.0f, 90.0f, position);
                CreateEffect(kHarshWind02Model, position, effect.Angle,
                    effect.Light, 0, 0, -1, 0, kHarshStrikeSkill);
                effect.AttackPoint[0] = 2;
            }
        }
        break;
    case kHarshWind01Model:
    {
        vec3_t local;
        vec3_t rotated;
        float matrix[3][4];
        Vector(0.0f, 0.0f, -20.0f * animationFactor, local);
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        VectorAdd(effect.Position, rotated, effect.Position);
        UpdateHarshWind01Tick(effect);
        break;
    }
    case kHarshWind02Model:
    {
        vec3_t local;
        vec3_t rotated;
        float matrix[3][4];
        Vector(0.0f, 0.0f, 8.0f * animationFactor, local);
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        VectorAdd(effect.Position, rotated, effect.Position);
        UpdateHarshWind02Tick(effect);
        break;
    }
    case kShiningPeakControllerModel:
        if (effect.SubType != 0)
            break;
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        RecordShiningQASample(effect, animationFactor);
#endif
        if (!effect.Owner || !effect.Owner->Live ||
            effect.Owner->CurrentAction != 288)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        {
            const float frame = effect.Owner->AnimationFrame;
            vec3_t position;
            if (effect.AttackPoint[0] == 0 && frame >= 3.3f && frame <= 3.8f)
            {
                EmitShiningXsuper(effect, 0.0f, -50.0f, 110.0f,
                    100, 50, 100, 50, 0, 0);
                TransformFromObject(effect, 0.0f, 150.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakPinModel, position, effect);
                CreateShiningModel(kShiningPeakPinModel, position, effect);
                effect.AttackPoint[0] = 1;
            }
            if (effect.AttackPoint[0] == 1 && frame >= 5.0f && frame <= 5.6f)
            {
                EmitShiningXsuper(effect, 0.0f, 0.0f, 110.0f,
                    100, 50, 100, 50, 0, 0);
                TransformFromObject(effect, 0.0f, -220.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakBodyModel, position, effect, 0);
                CreateShiningModel(kShiningPeakBodyModel, position, effect, 1);
                TransformFromObject(effect, 0.0f, 150.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakPinModel, position, effect);
                CreateShiningModel(kShiningPeakPinModel, position, effect);
                TransformFromObject(effect, 0.0f, -100.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakSpinModel, position, effect);
                vec3_t light;
                Vector(0.65f, 0.8f, 1.0f, light);
                CreateParticle(kFlareBlueBitmap, position, effect.Angle, light,
                    5, 2.5f, 0);
                CreateParticle(kFlareBlueBitmap, position, effect.Angle, light,
                    5, 2.5f, 0);
                effect.AttackPoint[0] = 2;
            }
            if (effect.AttackPoint[0] == 2 && frame >= 5.8f && frame <= 6.6f)
            {
                TransformFromObject(effect, 0.0f, -350.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakShockwaveModel, position,
                    effect, 6, 0.3f);
                TransformFromObject(effect, 0.0f, 0.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakShockwaveModel, position,
                    effect, 6, 1.05f);
                EmitShiningXsuper(effect, 0.0f, -350.0f, 110.0f,
                    200, 150, 0, 0, 200, 100);
                TransformFromObject(effect, 0.0f, 150.0f, 90.0f, position);
                CreateShiningModel(kShiningPeakPinModel, position, effect);
                CreateShiningModel(kShiningPeakPinModel, position, effect);
                effect.AttackPoint[0] = 3;
            }
            if (effect.AttackPoint[0] == 3 && frame >= 6.8f && frame <= 7.6f)
            {
                for (int i = 0; i < 3; ++i)
                    EmitShiningXsuper(effect, 0.0f, -450.0f, 110.0f,
                        100, 50, 60, 30, 100, 50);
                TransformFromObject(effect, 0.0f, -120.0f, 110.0f, position);
                vec3_t light;
                Vector(3.0f, 3.0f, 3.0f, light);
                for (int i = 0; i < 4; ++i)
                {
                    vec3_t randomPosition;
                    VectorCopy(position, randomPosition);
                    randomPosition[0] += static_cast<float>(rand() % 50 - 25);
                    randomPosition[1] += static_cast<float>(rand() % 50 - 25);
                    randomPosition[2] += static_cast<float>(rand() % 50 - 25);
                    CreateParticle(BITMAP_CLUD64, randomPosition, effect.Angle,
                        light, 19, 2.0f, 0);
                }
                effect.AttackPoint[0] = 4;
            }
        }
        break;
    case kShiningPeakBodyModel:
        UpdateShiningBodyTick(effect);
        break;
    case kShiningPeakPinModel:
    {
        UpdateShiningPinTick(effect);
        vec3_t local;
        vec3_t rotated;
        float matrix[3][4];
        Vector(0.0f, -50.0f, 0.0f, local);
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        VectorAdd(effect.Position, rotated, effect.Position);
        break;
    }
    case kShiningPeakSpinModel:
        UpdateShiningSpinTick(effect);
        break;
    case kShiningPeakShockwaveModel:
        // SS21 routes 0xAD6 through its default movement helper with the
        // rotate-direction flag. CreateEffect leaves Direction at zero, so
        // position, scale, alpha and light stay unchanged; life6..0 gives
        // seven update/expiry visits, not six decrement-then-kill visits.
        break;
    case kWrathControllerModel:
        if (!effect.Owner || !effect.Owner->Live)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        {
            const float frame = effect.Owner->AnimationFrame;
            vec3_t position;
            vec3_t light;
            if (effect.AttackPoint[0] == 0 && WrathStageDue(0, frame))
            {
                TransformFromObject(effect, 0.0f, -70.0f, 250.0f, position);
                Vector(1.0f, 1.0f, 1.0f, light);
                CreateParticle(kWrathComboBitmap, position, effect.Angle,
                    light, 0, 1.0f, NULL);
                effect.AttackPoint[0] = 1;
            }
            if (effect.AttackPoint[0] == 1 && WrathStageDue(1, frame))
            {
                TransformFromObject(effect, 0.0f, -80.0f, 30.0f, position);
                CreateEffect(kWrathAuraModel, position, effect.Angle,
                    effect.Light, 0, effect.Owner, -1, 0, kWrathSkill);
                effect.AttackPoint[0] = 2;
            }
            if (effect.AttackPoint[0] == 2 && WrathStageDue(2, frame))
            {
                TransformFromObject(effect, 0.0f, -30.0f, -40.0f, position);
                CreateEffect(kWrathTravelModel, position, effect.Angle,
                    effect.Light, 0, effect.Owner, -1, 0, kWrathSkill);
                effect.AttackPoint[0] = 3;
            }
            if (effect.AttackPoint[0] == 3 && WrathStageDue(3, frame))
            {
                Vector(1.0f, 1.0f, 1.0f, light);
                TransformFromObject(effect, 0.0f, -80.0f, 220.0f, position);
                CreateParticle(kWrathAlphaLightBitmap, position, effect.Angle,
                    light, 0, 1.0f, NULL);
                TransformFromObject(effect, 0.0f, -80.0f, 5.0f, position);
                CreateEffect(kWrathBrokenBitmap, position, effect.Angle,
                    effect.Light, 0, NULL);
                effect.AttackPoint[0] = 4;
            }
        }
        break;
    case kWrathTravelModel:
    {
        // S21 0x1579227 subtracts without a lower clamp; shared tail also
        // leaves alpha untouched. MoveEffect calls this per whole tick.
        effect.Alpha -= 0.065f * animationFactor;
        vec3_t local;
        vec3_t rotated;
        float matrix[3][4];
        Vector(0.0f, 0.0f, -65.0f * animationFactor, local);
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        VectorAdd(effect.Position, rotated, effect.Position);
        break;
    }
    case kWrathAuraModel:
    {
        effect.Angle[2] += 1.5f * animationFactor;
        if (effect.SubType == 0)
        {
            UpdateWrathAuraScalarTick(effect);
        }
        else if (effect.SubType == 1)
        {
            if (!effect.Owner || !effect.Owner->Live)
            {
                effect.LifeTime = 0.0f;
                return;
            }
            VectorCopy(effect.Owner->Position, effect.Position);
            UpdateWrathAuraScalarTick(effect);
        }
        if (effect.LifeTime == 1.0f && effect.AttackPoint[0] == 0)
        {
            bool continueAura = effect.SubType == 0;
            if (effect.SubType == 1 && effect.Owner)
                continueAura = effect.Owner->m_BuffMap.isBuff(
                    static_cast<eBuffState>(424)) ||
                    effect.Owner->m_BuffMap.isBuff(static_cast<eBuffState>(425));
            if (continueAura)
            {
                vec3_t light;
                // S21 0x15795A7 supplies fixed light only for subtype 0.
                // Recurring subtype 1 at 0x15798B0 passes its stored light.
                if (effect.SubType == 1)
                {
                    VectorCopy(effect.Light, light);
                }
                else
                {
                    Vector(0.65f, 0.75f, 1.0f, light);
                }
                CreateEffect(kWrathAuraModel, effect.Position, effect.Angle,
                    light, 1, effect.Owner, -1, 0, kWrathSkill);
            }
            effect.AttackPoint[0] = 1;
        }
        break;
    }
    case kCircleShieldControllerModel:
    {
        if (!effect.Owner || !effect.Owner->Live)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        if (CirclePairTick(effect.LifeTime))
        {
            vec3_t position;
            TransformFromObject(effect, 0.0f, -60.0f, 100.0f, position);
            EmitCirclePair(effect, position);
        }
        if (CircleChildTick(effect.LifeTime) &&
            effect.Owner->BoneTransform &&
            CanPlayAction(*effect.Owner, effect.Owner->CurrentAction) &&
            Models[effect.Owner->Type].NumBones > 42)
        {
            vec3_t local;
            vec3_t position;
            Vector(0.0f, 0.0f, 0.0f, local);
            Models[effect.Owner->Type].TransformPosition(
                effect.Owner->BoneTransform[42], local, position, false);
            // S21 resolves bone42 but child position is the separately
            // transformed controller-local (0,-50,110), not the bone result.
            TransformFromObject(effect, 0.f, -50.f, 110.f, position);
            vec3_t light;
            Vector(0.45f, 0.35f, 1.0f, light);
            CreateEffect(kCircleShieldModel, position, effect.Angle, light,
                effect.SubType, &effect, -1, 0, kCircleShieldSkill);
        }
        break;
    }
    case kCircleShieldModel:
    {
        vec3_t local;
        vec3_t rotated;
        float matrix[3][4];
        Vector(0.0f, -3.0f * animationFactor, 0.0f, local);
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        VectorAdd(effect.Position, rotated, effect.Position);
        effect.Scale -= 0.05f * animationFactor;
        break;
    }
    case kObsidianRotatingModel:
        UpdateObsidianRotatingTick(effect);
        break;
    case kObsidianAuraModel:
        if (!effect.Owner || !effect.Owner->Live ||
            effect.Owner->CurrentAction != 290)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        UpdateObsidianAuraTick(effect);
        break;
    case kClashControllerModel:
        if (!effect.Owner || !effect.Owner->Live ||
            effect.Owner->CurrentAction != 293)
        {
            effect.LifeTime = 0.0f;
            return;
        }
        // S21 missing owner model skips emission via the common lifetime tail.
        // The native adapter also needs its mapped action after model changes.
        if (!CanPlayAction(*effect.Owner, 293))
            return;
        if (ClashEmissionDue(effect.AttackPoint[0], effect.Owner->AnimationFrame))
        {
            vec3_t direction;
            if (!ClashDirection(*effect.Owner, effect.m_sTargetIndex, direction))
            {
                // S21 missing target skips this tick; retry within frame window.
                return;
            }
            vec3_t position;
            vec3_t light;
            Vector(1.0f, 1.5f, 1.5f, light);
            // S21157DD8B..157DE2F overwrites the provisional -50 vector
            // with caster + unit direction for the rear constructor input.
            ClashInitialPosition(effect.Owner->Position, direction, false, position);
            CreateEffect(kClashRearModel, position, effect.Owner->Angle, light,
                0, effect.Owner, -1, 0, kClashSkill, 0, 0.75f,
                effect.m_sTargetIndex);
            ClashInitialPosition(effect.Owner->Position, direction, true, position);
            CreateEffect(kClashFrontModel, position, effect.Owner->Angle, light,
                0, effect.Owner, -1, 0, kClashSkill, 0, 0.75f,
                effect.m_sTargetIndex);
            effect.AttackPoint[0] = 1;
        }
        break;
    case kClashFrontModel:
        UpdateClashChildFromStoredDirection(effect, 150.0f);
        UpdateClashFrontScalarTick(effect);
        break;
    case kClashRearModel:
        UpdateClashChildFromStoredDirection(effect, 0.0f);
        UpdateClashRearScalarTick(effect);
        break;
    }
}

bool SubmitCirclePersistentVisuals(OBJECT& caster)
{
    // S21 13ECB52..13ECFBC executes in the character-render traversal for
    // status216/221/222.  The caller owns the native buff gate.
    if (!caster.Live || !Models || caster.Type < 0 ||
        caster.Type >= MAX_MODELS || !caster.BoneTransform ||
        !EnsureCirclePersistentBitmap()) {
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        RecordCirclePersistentEmissionQA(caster, false, -1, 0);
#endif
        return false;
    }
    BMD& model = Models[caster.Type];
    if (model.NumBones <= 35) {
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        RecordCirclePersistentEmissionQA(caster, false, model.NumBones, 0);
#endif
        return false;
    }

    const float pulse = CircleFlarePulse(rand());
    vec3_t flareLight = {.5f * pulse, 0.f, pulse};
    vec3_t monoLight = {.3f, .38f, 1.f};
    const float savedScale = model.BodyScale;
    model.BodyScale = caster.Scale;
    const auto bonePosition = [&](int bone, vec3_t& position) {
        model.TransformByObjectBone(position, &caster, bone);
    };
    vec3_t position;
    int monoCreated = 0;
    bonePosition(kCircleFlareBones[0], position); // Head20
    CreateSprite(BITMAP_LIGHT, position, 2.f, flareLight, &caster, 0.f, 0);
    bonePosition(kCircleFlareBones[1], position); // R UpperArm26
    CreateSprite(BITMAP_LIGHT, position, 2.f, flareLight, &caster, 0.f, 0);
    for (int i = 0; i < 2; ++i)
        monoCreated += CreateCircleUpperArmParticle(position, caster.Angle,
            monoLight, .6f) >= 0 ? 1 : 0;
    bonePosition(kCircleFlareBones[2], position); // L UpperArm35
    CreateSprite(BITMAP_LIGHT, position, 2.f, flareLight, &caster, 0.f, 0);
    for (int i = 0; i < 2; ++i)
        monoCreated += CreateCircleUpperArmParticle(position, caster.Angle,
            monoLight, .6f) >= 0 ? 1 : 0;
    bonePosition(kCircleFlareBones[3], position); // R Clavicle25
    CreateSprite(BITMAP_LIGHT, position, 2.f, flareLight, &caster, 0.f, 0);
    bonePosition(kCircleFlareBones[4], position); // L Clavicle34
    CreateSprite(BITMAP_LIGHT, position, 2.f, flareLight, &caster, 0.f, 0);
    model.BodyScale = savedScale;
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    RecordCirclePersistentEmissionQA(caster, true, model.NumBones,
        monoCreated);
#endif
    return true;
}

bool SubmitWrathPersistentSprites(OBJECT& caster, bool boneFlareGroup)
{
    if (!caster.Live || !Models || caster.Type < 0 || caster.Type >= MAX_MODELS ||
        !caster.BoneTransform || !EnsureWrathGroundSpriteBitmaps()) return false;
    BMD& model = Models[caster.Type];
    if (model.NumBones <= (boneFlareGroup ? 38 : 35)) return false;
    const int texture = boneFlareGroup ? kWrathFlare01Bitmap : kWrathLightmarksBitmap;
    const float savedScale = model.BodyScale;
    model.BodyScale = caster.Scale;
    const auto emit = [&](int bone,float z,float scale,float r,float g,float b) {
        vec3_t position,light={r,g,b};
        model.TransformByObjectBone(position,&caster,bone);
        position[2] += z;
        CreateSprite(texture,position,scale,light,&caster,0.f,0);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        if (boneFlareGroup)
            RecordWrathBuffVisualQA(caster, "flare", -1, bone == 29 ? 0 : 1,
                bone);
#endif
    };
    if (boneFlareGroup)
        SubmitWrathBoneFlares(emit);
    else
    {
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        RecordWrathBuffVisualQA(caster, "lightmarks", -1, -1, -1);
#endif
        SubmitWrathLightmarks([](){return rand();},emit);
    }
    model.BodyScale = savedScale;
    return true;
}

bool RenderWrathPersistentGround(const OBJECT& caster, float sampledSourceClock)
{
    if (!caster.Live || !EnsureWrathGroundSpriteBitmaps()) return false;
    const int textures[] = {kWrathLightmarksBitmap,kWrathFlareBlueBitmap,
        kWrathFlareBlueBitmap,kWrathShockwaveBitmap};
    EnableAlphaBlend();
    SubmitWrathGroundLayers(sampledSourceClock,
        [](double angle) { return std::cos(angle); },
        [&](int layer,float size,float red,float green,float blue,float rotation) {
            vec3_t light = {red,green,blue};
            RenderGrowLancerTerrainBitmap(textures[layer],caster.Position[0],
                caster.Position[1],size,size,light,rotation,1.f,5.f,false);
        });
    DisableAlphaBlend();
    return true;
}

bool SubmitWrathPersistentVisuals(OBJECT& caster, float sampledSourceClock)
{
    // S21 entry 13DBA07..13DBA27. The state is Teleport, verified against
    // its alpha-fade writer and native ZzzInterface teleport transition.
    // Strict comparison preserves the source equal/unordered fall-through.
    if (caster.Teleport == TELEPORT && caster.Alpha < 1.e-6f) return false;
    // S21 13EB9DA..13EBA12: membership OR, not one emission per buff.
    // Keep this check before asset loads and RNG/pool submissions. Removal
    // stops new emissions; already allocated children retain native lifetime.
    OBJECT* buffOwner = &caster;
    if (!g_isCharacterBuff(buffOwner, static_cast<eBuffState>(424)) &&
        !g_isCharacterBuff(buffOwner, static_cast<eBuffState>(425))) return false;
    if (!caster.Live || !Models || caster.Type < 0 || caster.Type >= MAX_MODELS ||
        !caster.BoneTransform || Models[caster.Type].NumBones <= 38 ||
        !EnsureWrathGroundSpriteBitmaps() || !EnsureWrathScatterBitmaps() ||
        !EnsureWrathPersistentBitmaps()) return false;
    SubmitWrathPersistentSprites(caster,false);
    RenderWrathPersistentGround(caster,sampledSourceClock);
    BMD& model = Models[caster.Type];
    const float savedScale = model.BodyScale;
    model.BodyScale = caster.Scale;
    SubmitWrathParticleSequence([](){return rand();},
        [&](int variant,int x,int y,float scale) {
            vec3_t position = {caster.Position[0]+x,caster.Position[1]+y,caster.Position[2]};
            vec3_t light = {.6f,.75f,1.f};
            CreateWrathScatterParticle(variant,position,caster.Angle,light,scale);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
            RecordWrathBuffVisualQA(caster, "scatter", variant, -1, -1);
#endif
        },
        [&](){SubmitWrathPersistentSprites(caster,true);},
        [&](int variant,int attachment) {
            vec3_t position,light = {.4f,.6f,1.f};
            model.TransformByObjectBone(position,&caster,attachment == 0 ? 29 : 38);
            CreateWrathPersistentParticle(variant,attachment,position,caster.Angle,
                light,.75f,&caster);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
            RecordWrathBuffVisualQA(caster, "mono", variant, attachment,
                attachment == 0 ? 29 : 38);
#endif
        });
    model.BodyScale = savedScale;
    return true;
}

bool RenderEffect(OBJECT& effect)
{
    if (IsBrecheEffectType(effect.Type))
    {
        if (effect.Type == kBrecheOwnerWindModel)
        {
            if (!EnsureModel(effect.Type) ||
                !Calc_RenderObject(&effect, false, 0, 0))
                return false;
            // S21 dispatch 0x15A136D -> 0x15AD77D sends AD9 through the
            // generic object renderer (0x176D621 -> 0x1887B8B).  Its default
            // branch supplies texture flag 2 and the complete carrier tuple.
            // Common reset leaves BlendMesh=0; wind_foce mesh zero references
            // texture slot zero, selecting the additive blend-mesh branch in
            // both renderers without inventing a RENDER_BRIGHT flag.
            BMD& model = Models[effect.Type];
            model.RenderBody(RENDER_TEXTURE, effect.Alpha, effect.BlendMesh,
                effect.BlendMeshLight, effect.BlendMeshTexCoordU,
                effect.BlendMeshTexCoordV, effect.HiddenMesh, -1);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
            RecordTargetSkillRenderQA(effect, kBrecheSkill,
                "breche-owner-wind-submit", 1, model.NumMeshs,
                model.NumBones, model.NumActions,
                model.NumMeshs > 0 ? static_cast<int>(model.IndexTexture[0]) : 0);
#endif
            return true;
        }
        if (effect.Type == kBrecheControllerModel || effect.Type == kBrecheEmitterModel)
            return true;
        vec3_t light;
        VectorScale(effect.Light, effect.Alpha, light);
        const bool owner = effect.Type == kBrecheOwnerRingModel ||
            effect.Type == kBrecheOwnerLightMarksModel ||
            effect.Type == kBrecheOwnerFireRingModel ||
            effect.Type == kBrecheOwnerTwilight02Model ||
            effect.Type == kBrecheOwnerTwilight01Model;
        const bool marks = effect.Type == kBrecheLightMarksModel ||
            effect.Type == kBrecheOwnerLightMarksModel;
        const int bitmap = effect.Type == kBrecheOwnerRingModel ?
            kBrecheOwnerRingBitmap :
            (effect.Type == kBrecheOwnerFireRingModel ? kBrecheOwnerFireRingBitmap :
             (marks ? kBrecheLightMarksBitmap :
              (effect.Type == kBrecheTwilight02Model ||
               effect.Type == kBrecheOwnerTwilight02Model ?
                  kBrecheTwilight02Bitmap : kBrecheTwilight01Bitmap)));
#ifndef RISE_GROW_LANCER_RUNTIME_QA
        (void)owner;
#endif
        // S21 15AEC85/159D64C/159E00F select ONE/ONE for every Breche
        // terrain layer and restore the normal state after the draw.  The
        // native terrain helper only submits geometry/color; it does not
        // establish this blend mode itself. Keep the state change private to
        // Breche so ordinary SS6 terrain/effects are untouched.
        EnableAlphaBlend();
        RenderGrowLancerTerrainBitmap(bitmap, effect.Position[0], effect.Position[1],
            effect.Scale, effect.Scale, light, effect.Angle[2],
            marks ? 1.f : effect.Alpha, 5.f, !marks);
        DisableAlphaBlend();
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        RecordTargetSkillRenderQA(effect, kBrecheSkill,
            owner ? "breche-owner-submit" : "breche-submit", 1,
            0, 0, 0, bitmap);
#endif
        return true;
    }
    if (effect.Type == kWrathBrokenBitmap)
    {
        // SS21 0x15A0B63..0x15A0C0F delegates the effect object to its
        // terrain-alpha bitmap renderer with rotation 0, alpha 1, height 5.
        // S21 0x15A0B63 explicitly selects ONE/ONE before this JPG ground
        // layer. RenderTerrainAlphaBitmap does not establish blend state.
        EnableAlphaBlend();
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        const unsigned wrathPixelToken = BeginWrathGroundPixelsQA(effect);
#endif
        RenderTerrainAlphaBitmap(effect.Type, effect.Position[0],
            effect.Position[1], effect.Scale, effect.Scale, effect.Light,
            0.0f, 1.0f, 5.0f);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        EndWrathGroundPixelsQA(wrathPixelToken);
        RecordWrathGroundQA(effect);
#endif
        // The native effect loop has no pass-level blend reset. Restore its
        // state after this S21 ONE/ONE terrain draw so subsequent SS6 effects
        // and the next Grow Lancer child do not inherit additive blending.
        DisableAlphaBlend();
        return true;
    }
    if (effect.Type == kSpriteCarrierModel)
    {
        if (effect.SubType == kCarrierSpinGround)
        {
            // S21 15AACE9: Light already scaled in update; do not square alpha.
            const float size = effect.Scale * effect.Distance;
            // Native terrain divides by size; skip the initial zero-area quad.
            if (size <= 0.f)
                return true;
            EnableAlphaBlend();
            RenderGrowLancerTerrainBitmap(effect.Skill, effect.Position[0],
                effect.Position[1], size, size, effect.Light, 0.f, 1.f, 5.f, false);
            DisableAlphaBlend();
#ifdef RISE_GROW_LANCER_RUNTIME_QA
            RecordSpinRenderQA(effect, "ground-submit", 1, 0, 0, 0,
                effect.Skill);
#endif
            return true;
        }
        vec3_t light;
        Vector(effect.Light[0] * effect.Alpha,
            effect.Light[1] * effect.Alpha,
            effect.Light[2] * effect.Alpha, light);
        if (effect.SubType == kCarrierFlare || effect.SubType == kCarrierHole)
        {
            // S21 15ACE16..15ACF70: terrain, not camera-facing sprite.
            // 1765DF1 receives alpha1, height5, terrain heights1, flipV0.
            EnableAlphaBlend();
            RenderGrowLancerTerrainBitmap(effect.Skill, effect.Position[0],
                effect.Position[1], effect.Scale, effect.Scale, light,
                effect.Angle[2], 1.f, 5.f, false);
            DisableAlphaBlend();
            return true;
        }
        CreateSprite(effect.Skill, effect.Position, effect.Scale, light,
            effect.SubType == kCarrierCircleShiny ? &effect : effect.Owner,
            effect.Angle[2]);
        return true;
    }
    if (effect.Type == kClashControllerModel ||
        effect.Type == kSpinControllerModel ||
        effect.Type == kMagicPinControllerModel ||
        effect.Type == kHarshStrikeControllerModel ||
        effect.Type == kShiningPeakControllerModel ||
        effect.Type == kWrathControllerModel ||
        effect.Type == kCircleShieldControllerModel)
        return true;
    if (effect.Type == kSpinCrossModel ||
        effect.Type == kMagicPin01Model ||
        effect.Type == kMagicPin03Model ||
        effect.Type == kMagicPinRootModel ||
        effect.Type == kMagicPinAuxModel)
    {
        const bool modelReady = EnsureModel(effect.Type);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        if (effect.Type == kMagicPin01Model ||
            effect.Type == kMagicPin03Model ||
            effect.Type == kMagicPinRootModel ||
            effect.Type == kMagicPinAuxModel)
        {
            const BMD& diagnosticModel = Models[effect.Type];
            RecordTargetSkillRenderQA(effect, kMagicPinSkill, "magic-ensure",
                modelReady ? 1 : 0, diagnosticModel.NumMeshs,
                diagnosticModel.NumBones, diagnosticModel.NumActions,
                diagnosticModel.NumMeshs > 0 ?
                    static_cast<int>(diagnosticModel.IndexTexture[0]) : 0);
        }
        if (effect.Type == kSpinCrossModel)
        {
            const BMD& diagnosticModel = Models[effect.Type];
            RecordSpinRenderQA(effect, "cross-ensure", modelReady ? 1 : 0,
                diagnosticModel.NumMeshs, diagnosticModel.NumBones,
                diagnosticModel.NumActions,
                diagnosticModel.NumMeshs > 0 ?
                    static_cast<int>(diagnosticModel.IndexTexture[0]) : 0);
        }
#endif
        if (!modelReady)
            return false;
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        if ((effect.Type == kMagicPin01Model ||
             effect.Type == kMagicPin03Model ||
             effect.Type == kMagicPinRootModel ||
             effect.Type == kMagicPinAuxModel) &&
            Models[effect.Type].NumMeshs > 0 &&
            Models[effect.Type].Textures && Models[effect.Type].IndexTexture)
        {
            const BMD& diagnosticModel = Models[effect.Type];
            RecordMagicMaterialQA(effect.Type,
                diagnosticModel.IndexTexture[0],
                diagnosticModel.Textures[0].FileName);
        }
#endif
        const bool transformReady = Calc_RenderObject(&effect, false, 0, 0);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        if (effect.Type == kMagicPin01Model ||
            effect.Type == kMagicPin03Model ||
            effect.Type == kMagicPinRootModel ||
            effect.Type == kMagicPinAuxModel)
        {
            const BMD& diagnosticModel = Models[effect.Type];
            RecordTargetSkillRenderQA(effect, kMagicPinSkill, "magic-calc",
                transformReady ? 1 : 0, diagnosticModel.NumMeshs,
                diagnosticModel.NumBones, diagnosticModel.NumActions,
                diagnosticModel.NumMeshs > 0 ?
                    static_cast<int>(diagnosticModel.IndexTexture[0]) : 0);
        }
        if (effect.Type == kSpinCrossModel)
        {
            const BMD& diagnosticModel = Models[effect.Type];
            RecordSpinRenderQA(effect, "cross-calc", transformReady ? 1 : 0,
                diagnosticModel.NumMeshs, diagnosticModel.NumBones,
                diagnosticModel.NumActions,
                diagnosticModel.NumMeshs > 0 ?
                    static_cast<int>(diagnosticModel.IndexTexture[0]) : 0);
        }
#endif
        if (!transformReady)
            return false;
        BMD& model = Models[effect.Type];
        if (effect.Type == kSpinCrossModel)
        {
            model.BodyLight[0] = effect.Light[0] * effect.Alpha;
            model.BodyLight[1] = effect.Light[1] * effect.Alpha;
            model.BodyLight[2] = effect.Light[2] * effect.Alpha;
        }
        else
        {
            model.BodyLight[0] = 0.48f * effect.Alpha;
            model.BodyLight[1] = 0.73f * effect.Alpha;
            model.BodyLight[2] = effect.Alpha;
        }
        if (effect.Type == kMagicPin03Model ||
            effect.Type == kMagicPinAuxModel)
            effect.BlendMeshTexCoordV += 0.035f;
        const bool zeroMagicUv = effect.Type == kMagicPin01Model ||
            effect.Type == kMagicPinRootModel;
        // A48FEF is literal1 for MagicPin01, not the object's Alpha.
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        const unsigned magicRasterQuery =
            (effect.Type == kMagicPin01Model ||
             effect.Type == kMagicPinRootModel) ?
            BeginMagicPinFootSamplesQA(effect) : 0;
        const unsigned spinCrossRasterQuery = effect.Type == kSpinCrossModel ?
            BeginSpinCrossSamplesQA(effect) : 0;
#endif
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
            0, effect.Type == kMagicPin01Model ? 1.0f : effect.Alpha,
            zeroMagicUv ? 0.0f : effect.BlendMeshTexCoordU,
            zeroMagicUv ? 0.0f : effect.BlendMeshTexCoordV, -1);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        EndMagicPinFootSamplesQA(effect, magicRasterQuery);
        EndSpinCrossSamplesQA(effect, spinCrossRasterQuery);
        if (effect.Type == kMagicPin01Model ||
            effect.Type == kMagicPin03Model ||
            effect.Type == kMagicPinRootModel ||
            effect.Type == kMagicPinAuxModel)
            RecordTargetSkillRenderQA(effect, kMagicPinSkill, "magic-submit",
                1, model.NumMeshs, model.NumBones, model.NumActions,
                model.NumMeshs > 0 ?
                    static_cast<int>(model.IndexTexture[0]) : 0);
        if (effect.Type == kSpinCrossModel)
            RecordSpinRenderQA(effect, "cross-submit", 1, model.NumMeshs,
                model.NumBones, model.NumActions,
                model.NumMeshs > 0 ? static_cast<int>(model.IndexTexture[0]) : 0);
#endif
        return true;
    }
    if (effect.Type == kHarshWind01Model ||
        effect.Type == kHarshWind02Model)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        if (effect.Type == kHarshWind01Model)
        {
            const float colors[4][3] = {
                {1.0f, 0.3f, 0.0f}, {1.0f, 1.0f, 1.0f},
                {1.0f, 1.0f, 1.0f}, {1.0f, 0.95f, 0.25f}
            };
            for (int mesh = 0; mesh < 4; ++mesh)
            {
                // A48821..A48A5A: fixed material RGB, no alpha multiply;
                // alpha is passed separately and both UV offsets are zero.
                VectorCopy(colors[mesh], model.BodyLight);
                model.RenderMesh(mesh, RENDER_TEXTURE | RENDER_BRIGHT,
                    effect.Alpha, mesh, effect.Alpha,
                    0.0f, 0.0f, -1);
            }
        }
        else
        {
            VectorCopy(effect.Light, model.BodyLight);
            model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
                0, 1.0f, effect.BlendMeshTexCoordU,
                effect.BlendMeshTexCoordV, -1);
        }
        return true;
    }
    if (effect.Type == kShiningPeakBodyModel ||
        effect.Type == kShiningPeakPinModel ||
        effect.Type == kShiningPeakSpinModel)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        // Body/spin callbacks A49D77/A49EE5 retain shared BodyLight.
        // Only pin A49DD2 overrides it with object Light * Alpha.
        if (effect.Type == kShiningPeakPinModel)
        {
            model.BodyLight[0] = effect.Light[0] * effect.Alpha;
            model.BodyLight[1] = effect.Light[1] * effect.Alpha;
            model.BodyLight[2] = effect.Light[2] * effect.Alpha;
        }
        int meshCount = effect.Type == kShiningPeakSpinModel ? 3 : 1;
        for (int mesh = 0; mesh < meshCount; ++mesh)
            model.RenderMesh(mesh, RENDER_TEXTURE | RENDER_BRIGHT,
                effect.Alpha, mesh, effect.Alpha,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        return true;
    }
    if (effect.Type == kShiningPeakShockwaveModel)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        // S21 AD6 registration A1CF92 -> A72AD0. Subtype 6 follows
        // A7300C: unscaled Light, mesh 0 twice, flags 0x42, texture -1.
        // Keep this private: Draw_RenderObject cannot dispatch an S21 ID.
        BMD& model = Models[effect.Type];
        VectorCopy(effect.Light, model.BodyLight);
        for (int pass = 0; pass < 2; ++pass)
            model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
                effect.Alpha, 0, effect.BlendMeshLight,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        return true;
    }
    if (effect.Type == kWrathAuraModel || effect.Type == kWrathTravelModel)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        if (effect.Type == kWrathTravelModel)
        {
            // S21 0xA49438/0xA4944C/0xA4945F: fixed RGB, not effect.Light.
            model.BodyLight[0] = 0.37f * effect.Alpha;
            model.BodyLight[1] = 0.57f * effect.Alpha;
            model.BodyLight[2] = effect.Alpha;
        }
        else if (effect.SubType == 0)
        {
            model.BodyLight[0] = effect.Alpha;
            model.BodyLight[1] = effect.Alpha;
            model.BodyLight[2] = effect.Alpha;
        }
        else
        {
            model.BodyLight[0] = effect.Light[0] * effect.Alpha;
            model.BodyLight[1] = effect.Light[1] * effect.Alpha;
            model.BodyLight[2] = effect.Light[2] * effect.Alpha;
        }
        // S21 travel 0xA49527 and aura 0xA49676/0xA4978B use blend mesh 0.
        // Travel passes literal zero UV offsets, unlike scrolling aura.
        const bool travel = effect.Type == kWrathTravelModel;
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
            0, effect.Alpha, travel ? 0.0f : effect.BlendMeshTexCoordU,
            travel ? 0.0f : effect.BlendMeshTexCoordV, -1);
        return true;
    }
    if (effect.Type == kCircleShieldModel)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        VectorCopy(effect.Light, model.BodyLight);
        model.RenderMesh(0, RENDER_TEXTURE, effect.Alpha, -2, effect.Alpha,
            effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        return true;
    }
    if (effect.Type == kObsidianRotatingModel)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        model.BodyLight[0] = 0.48f * effect.Alpha;
        model.BodyLight[1] = effect.Alpha;
        model.BodyLight[2] = effect.Alpha;
        effect.BlendMeshTexCoordU -= 1.0f;
        effect.BlendMeshTexCoordV += 0.075f;
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
            0, effect.Alpha, effect.BlendMeshTexCoordU,
            effect.BlendMeshTexCoordV, -1);
        return true;
    }
    if (effect.Type == kObsidianAuraModel)
    {
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
            return false;
        BMD& model = Models[effect.Type];
        const float colors[2][3] = {
            {0.21f, 0.26f, 1.0f},
            {0.46f, 0.53f, 1.0f}
        };
        for (int mesh = 0; mesh < 2; ++mesh)
        {
            model.BodyLight[0] = colors[mesh][0] * effect.Alpha;
            model.BodyLight[1] = colors[mesh][1] * effect.Alpha;
            model.BodyLight[2] = colors[mesh][2] * effect.Alpha;
            model.RenderMesh(mesh, RENDER_TEXTURE | RENDER_BRIGHT,
                effect.Alpha, mesh, effect.Alpha,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
        }
        return true;
    }
    if (effect.Type != kClashFrontModel && effect.Type != kClashRearModel)
        return true;
    const bool modelReady = EnsureModel(effect.Type);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    {
        const BMD& diagnosticModel = Models[effect.Type];
        RecordTargetSkillRenderQA(effect, kClashSkill, "clash-ensure",
            modelReady ? 1 : 0, diagnosticModel.NumMeshs,
            diagnosticModel.NumBones, diagnosticModel.NumActions,
            diagnosticModel.NumMeshs > 0 ?
                static_cast<int>(diagnosticModel.IndexTexture[0]) : 0);
    }
#endif
    if (!modelReady)
        return false;
    const bool transformReady = Calc_RenderObject(&effect, false, 0, 0);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    {
        const BMD& diagnosticModel = Models[effect.Type];
        RecordTargetSkillRenderQA(effect, kClashSkill, "clash-calc",
            transformReady ? 1 : 0, diagnosticModel.NumMeshs,
            diagnosticModel.NumBones, diagnosticModel.NumActions,
            diagnosticModel.NumMeshs > 0 ?
                static_cast<int>(diagnosticModel.IndexTexture[0]) : 0);
    }
#endif
    if (!transformReady)
        return false;
    if (effect.Type == kClashFrontModel)
    {
        // SS21 renderer 0xA4A1B6..0xA4A1EA scrolls mesh UV, not angles.
        effect.BlendMeshTexCoordU -= 0.05f;
        effect.BlendMeshTexCoordV -= 0.01f;
    }
    BMD& model = Models[effect.Type];
    model.BodyLight[0] = effect.Light[0] * effect.Alpha;
    model.BodyLight[1] = effect.Light[1] * effect.Alpha;
    model.BodyLight[2] = effect.Light[2] * effect.Alpha;
    model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha, 0,
        effect.Alpha, effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,
        -1);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    RecordTargetSkillRenderQA(effect, kClashSkill, "clash-submit", 1,
        model.NumMeshs, model.NumBones, model.NumActions,
        model.NumMeshs > 0 ? static_cast<int>(model.IndexTexture[0]) : 0);
#endif
    return true;
}

bool RenderBrecheClippedMesh(BMD& model, int meshIndex, int renderFlags,
    float alpha, int blendMeshIndex, float blendMeshLight,
    float blendMeshTextureCoordU, float zThreshold, int textureIndex)
{
    if (meshIndex < 0 || meshIndex >= model.NumMeshs || model.Meshs == NULL)
        return false;

    Mesh_t& mesh = model.Meshs[meshIndex];
    if (mesh.NumTriangles < 0 || mesh.Triangles == NULL)
        return false;

    std::vector<Triangle_t> visible;
    visible.reserve(mesh.NumTriangles);
    for (int triangleIndex = 0; triangleIndex < mesh.NumTriangles;
         ++triangleIndex)
    {
        const Triangle_t& triangle = mesh.Triangles[triangleIndex];
        if (triangle.Polygon < 3)
            return false;

        const int v0 = triangle.VertexIndex[0];
        const int v1 = triangle.VertexIndex[1];
        const int v2 = triangle.VertexIndex[2];
        if (v0 < 0 || v0 >= mesh.NumVertices ||
            v1 < 0 || v1 >= mesh.NumVertices ||
            v2 < 0 || v2 >= mesh.NumVertices)
            return false;

        // Exact SS21 0x01333847..0x013338D7 predicate.  Equality retains the
        // triangle; there is deliberately no geometric plane intersection.
        if (zThreshold > VertexTransform[meshIndex][v0][2] &&
            zThreshold > VertexTransform[meshIndex][v1][2] &&
            zThreshold > VertexTransform[meshIndex][v2][2])
            continue;

        visible.push_back(triangle);
    }

    if (visible.empty())
        return true;

    ScopedMeshTriangles filtered(mesh, &visible[0],
        static_cast<short>(visible.size()));
    model.RenderMesh(meshIndex, renderFlags, alpha, blendMeshIndex,
        blendMeshLight, blendMeshTextureCoordU, zThreshold, textureIndex);
    return true;
}

void CreateClashRoot(OBJECT& caster, short targetIndex)
{
    OBJECT* target = 0;
    if (!ResolveTarget(targetIndex, target) || !PrepareFixedPlayerAction(caster, 293))
        return;
    caster.CurrentAction = 293;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    CreateEffect(kClashControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kClashSkill, 0, 0.75f, targetIndex);
    PlayBuffer(kClashSound, &caster);
}

void CreateObsidianRoots(OBJECT& caster)
{
    if (!PrepareFixedPlayerAction(caster, 290))
        return;
    // S21 10E46D5 -> 1327DE8 -> 1327D72 selects source action191
    // unless the verified auxiliary predicate selects alternate317. The
    // common setter preserves outgoing action/frame on a transition and
    // leaves an identical action alone. Native SetAction(true) maps only
    // the proven191->290 branch; native317 compatibility remains closed.
    SetAction(&caster, 290, true);
    CreateEffect(kObsidianRotatingModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kObsidianSkill);
    CreateEffect(kObsidianAuraModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kObsidianSkill);
    PlayBuffer(kObsidianSound, &caster);
}

bool CreateSpinStepRoot(OBJECT& caster, short targetIndex)
{
    if (!PrepareLocalQADynamicAction(caster, 285))
        return false;
    // S21 local action186 calls 1327DE8; the decoded transition preserves
    // prior action/frame and does not restart an identical action. Native
    // SetAction(...,true) is the proven SS6 equivalent (already used for Pin).
    SetAction(&caster, 285, true);
    CreateEffect(kSpinControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kSpinStepSkill, 0, caster.Scale, targetIndex);
    PlayBuffer(kSpinStepCastSound, &caster);
    return true;
}

void CreateSpinStepHit(OBJECT& target)
{
    CreateEffect(kSpinControllerModel, target.Position, target.Angle,
        target.Light, 2, &target, -1, 0, kSpinStepExplosionSkill);
    PlayBuffer(kSpinStepHitSound, &target);
}

bool CreateMagicPinRoots(OBJECT& caster)
{
    if (!PrepareLocalQADynamicAction(caster, 287))
        return false;
    // Bind only the approved private local cast. No remote actor borrowing.
    // This establishes lifecycle identity, not animation samples or tick timing.
    if (!gAnimationActorIdentity.Matches(reinterpret_cast<uintptr_t>(&caster)))
    {
        ResetAllEffectAnimationSamples();
        gAnimationActorIdentity.Bind(reinterpret_cast<uintptr_t>(&caster));
    }
    gAnimationObservationArmed =
        gAnimationActorIdentity.Matches(reinterpret_cast<uintptr_t>(&caster));
    // S21 1327D72 preserves the previous frame/action on transition and
    // does not restart an identical action. Use the native equivalent.
    SetAction(&caster, 287, true);
    CreateEffect(kMagicPinControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kMagicPinSkill);
    CreateEffect(kMagicPinRootModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kMagicPinSkill);
    PlayBuffer(kMagicPinCastSound, &caster);
    return true;
}

void CreateMagicPinHit(OBJECT& target)
{
    vec3_t position;
    VectorCopy(target.Position, position);
    position[2] += 140.0f;
    CreateEffect(kMagicPinControllerModel, position, target.Angle,
        target.Light, 1, &target, -1, 0, kMagicPinExplosionSkill);
    PlayBuffer(kMagicPinHitSound, &target);
}

void CreateHarshStrikeRoot(OBJECT& caster)
{
    if (!PrepareLocalQADynamicAction(caster, 284))
        return;
    // S21 local/receive action185 both call the preserving 1327DE8 setter
    // before controller5E0; native SetAction maps the private action284.
    SetAction(&caster, 284, true);
    CreateEffect(kHarshStrikeControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kHarshStrikeSkill);
    PlayBuffer(kHarshStrikeSound, &caster);
}

void CreateShiningPeakRoots(OBJECT& caster)
{
    if (!PrepareLocalQADynamicAction(caster, 288))
        return;
    // S21 local/receive action189 uses 1327DE8 before roots5F6/5F3.
    SetAction(&caster, 288, true);
    CreateEffect(kShiningPeakControllerModel, caster.Position, caster.Angle,
        caster.Light, 1, &caster, -1, 0, kShiningPeakSkill);
    CreateEffect(kShiningPeakControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kShiningPeakSkill);
    PlayBuffer(kShiningPeakSound, &caster);
}

void CreateWrathRoot(OBJECT& caster)
{
    if (!PrepareFixedPlayerAction(caster, 291))
        return;
    // S21 selects primary192 or conditional auxiliary321 through 1327DE8.
    // Only primary192->private291 is mapped; auxiliary321 stays unported.
    SetAction(&caster, 291, true);
    CreateEffect(kWrathControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kWrathSkill);
    PlayBuffer(kWrathSound, &caster);
}

void CreateCircleShieldContact(OBJECT& firstActor, OBJECT& secondActor)
{
    if (!firstActor.Live || !secondActor.Live)
        return;
    // S21 buff216 receiver129F04C..129F60C. No action reset, sound,
    // resource deduction or guessed server packet belongs in this adapter.
    vec3_t angle;
    Vector(0.f, 0.f, CreateAngle(secondActor.Position[0], secondActor.Position[1],
        firstActor.Position[0], firstActor.Position[1]), angle);
    CreateEffect(kCircleShieldControllerModel, secondActor.Position, angle,
        secondActor.Light, 1, &secondActor, -1, 0, 0);

    vec3_t start;
    VectorCopy(firstActor.Position, start);
    start[2] += 100.f;
    VectorCopy(firstActor.Angle, angle);
    angle[2] -= 135.f;
    CreateJoint(BITMAP_FORCEPILLAR, start, secondActor.Position, angle,
        3, &secondActor, 40.f, -1, 0, 0, -1, 0, -1);

    float matrix[3][4];
    AngleMatrix(firstActor.Angle, matrix);
    vec3_t local, rotated;
    Vector(100.f, 50.f, 0.f, local);
    VectorRotate(local, matrix, rotated);
    VectorAdd(firstActor.Position, rotated, start);
    VectorCopy(firstActor.Angle, angle);
    angle[2] -= 90.f;
    CreateJoint(BITMAP_FORCEPILLAR, start, secondActor.Position, angle,
        3, &secondActor, 40.f, -1, 0, 0, -1, 0, -1);

    Vector(-100.f, 50.f, 0.f, local);
    VectorRotate(local, matrix, rotated);
    VectorAdd(firstActor.Position, rotated, start);
    VectorCopy(firstActor.Angle, angle);
    angle[2] += 135.f;
    CreateJoint(BITMAP_FORCEPILLAR, start, secondActor.Position, angle,
        3, &secondActor, 40.f, -1, 0, 0, -1, 0, -1);
}

void RetireCircleCharacterEffects(OBJECT& retiredCharacter)
{
    if (retiredCharacter.Live)
        return;
    // Controller borrows character bones/action. Do not let a reused character
    // slot satisfy its Live guard. Detached children retain their own lifetime.
    DeleteEffect(kCircleShieldControllerModel, &retiredCharacter, -1);
}

void CreateCircleShieldRoot(OBJECT& caster)
{
    if (!PrepareFixedPlayerAction(caster, 286))
        return;
    caster.CurrentAction = 286;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    CreateEffect(kCircleShieldControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kCircleShieldSkill);
    PlayBuffer(kCircleShieldSound, &caster);
}

void CreateBrecheAction(OBJECT& caster)
{
    // Start only the mapped action/sound. The prior equipment-pass evidence
    // was an item-index switch, NOT an action190 switch. Its attribution to
    // Breche is retracted. The recovered receive/contact chain is separate:
    // CreateBrecheHit takes the target, never the local caster as fallback.
    if (!PrepareLocalQADynamicAction(caster, 289))
        return;
    // The recovered S21 local branch reaches the native action setter
    // (1327DE8), not a raw OBJECT field overwrite. Keep the outgoing clip
    // and frame so the player renderer can blend into the appended S21 clip
    // exactly like ReceiveMagic does for remote actors.
    SetAction(&caster, 289, true);
    PlayBuffer(kBrecheSound, &caster);

    // S21 per-character producer 0x142B21D creates a separate 5FD/subtype0
    // control record on the casting character.  It is intentionally not the
    // target receive root created by CreateBrecheHit.
    const bool ownerResourcesReady = EnsureBrecheBitmaps();
#ifdef RISE_GROW_LANCER_RUNTIME_QA
    // Keep the owner-side resource gate observable.  A missing private OZJ
    // asset must never be mistaken for a missing action or target contact.
    RecordBrecheOwnerResourceGateQA(ownerResourcesReady,
        RuntimeQASelectedTargetIndex(), false);
#endif
    if (ownerResourcesReady)
    {
        CreateEffect(kBrecheControllerModel, caster.Position, caster.Angle,
            caster.Light, 0, &caster, -1, kBrecheSkill, 0, 0, 10.f);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
        RecordBrecheOwnerResourceGateQA(true, RuntimeQASelectedTargetIndex(), true);
#endif
    }
}

void CreateBrecheHit(OBJECT& target)
{
    if (!target.Live || !EnsureBrecheBitmaps())
        return;
    CreateEffect(kBrecheControllerModel, target.Position, target.Angle,
        target.Light, 1, &target);
}

}}
