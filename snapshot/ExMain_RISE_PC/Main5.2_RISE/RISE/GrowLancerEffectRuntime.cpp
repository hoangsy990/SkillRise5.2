#include "stdafx.h"
#include "GrowLancerEffectRuntime.h"
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
#include "GrowLancerObsidianTick.h"
#include "GrowLancerClashTick.h"
#include "../../../GrowLancer/compat/S21CastAnimationSpeed.h"
#include "ZzzObject.h"
#include "ZzzOpenglUtil.h"

namespace rise { namespace growlancer {
namespace {

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
        CreateParticle(BITMAP_CLUD64, position, effect.Angle, light, 19,
            (rand() % 2) ? 2.5f : 3.5f, 0);
    }
    Vector(0.3f, 0.3f, 0.5f, light);
    for (int i = 0; i < 2; ++i)
        CreateParticle(kShockwave2Bitmap, effect.Position, effect.Angle, light,
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
    return type >= kBrecheControllerModel && type <= kBrecheEmitterModel;
}

void InitializeEffect(OBJECT& effect)
{
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 1.0f;
    switch (effect.Type)
    {
    case kBrecheControllerModel:
    {
        effect.Timer = 0.f;
        effect.LifeTime = 20.f;
        if (effect.SubType != 1 || !effect.Owner)
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
        CreateParticle(BITMAP_ENERGY, position, effect.Angle, light,
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

void UpdateEffect(OBJECT& effect, float animationFactor)
{
    switch (effect.Type)
    {
    case kBrecheControllerModel:
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
            CreateSprite(BITMAP_LIGHT, position, 4.f, light, &owner, 0.f);
            Vector(1.f, 1.f, 1.f, light);
            const float scale = (rand() % 5 + 13) * .1f;
            const int variant = rand() % 3;
            CreateBrecheFireParticle(variant, position, effect.Angle, light, scale);
        }
        break;
    }
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
                if (!ResolveTarget(effect.m_sTargetIndex, target))
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

bool RenderEffect(OBJECT& effect)
{
    if (IsBrecheEffectType(effect.Type))
    {
        if (effect.Type == kBrecheControllerModel || effect.Type == kBrecheEmitterModel)
            return true;
        vec3_t light;
        VectorScale(effect.Light, effect.Alpha, light);
        const bool marks = effect.Type == kBrecheLightMarksModel;
        const int bitmap = marks ? kBrecheLightMarksBitmap :
            (effect.Type == kBrecheTwilight02Model ? kBrecheTwilight02Bitmap : kBrecheTwilight01Bitmap);
        RenderGrowLancerTerrainBitmap(bitmap, effect.Position[0], effect.Position[1],
            effect.Scale, effect.Scale, light, effect.Angle[2],
            marks ? 1.f : effect.Alpha, 5.f, !marks);
        return true;
    }
    if (effect.Type == kWrathBrokenBitmap)
    {
        // SS21 0x15A0B63..0x15A0C0F delegates the effect object to its
        // terrain-alpha bitmap renderer with rotation 0, alpha 1, height 5.
        RenderTerrainAlphaBitmap(effect.Type, effect.Position[0],
            effect.Position[1], effect.Scale, effect.Scale, effect.Light,
            0.0f, 1.0f, 5.0f);
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
        if (!EnsureModel(effect.Type) ||
            !Calc_RenderObject(&effect, false, 0, 0))
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
        model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
            0, effect.Type == kMagicPin01Model ? 1.0f : effect.Alpha,
            zeroMagicUv ? 0.0f : effect.BlendMeshTexCoordU,
            zeroMagicUv ? 0.0f : effect.BlendMeshTexCoordV, -1);
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
    if (!EnsureModel(effect.Type) || !Calc_RenderObject(&effect, false, 0, 0))
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
    caster.CurrentAction = 290;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    CreateEffect(kObsidianRotatingModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kObsidianSkill);
    CreateEffect(kObsidianAuraModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kObsidianSkill);
    PlayBuffer(kObsidianSound, &caster);
}

void CreateSpinStepRoot(OBJECT& caster, short targetIndex)
{
    if (!PrepareLocalQADynamicAction(caster, 285))
        return;
    caster.CurrentAction = 285;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    CreateEffect(kSpinControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kSpinStepSkill, 0, caster.Scale, targetIndex);
    PlayBuffer(kSpinStepCastSound, &caster);
}

void CreateSpinStepHit(OBJECT& target)
{
    CreateEffect(kSpinControllerModel, target.Position, target.Angle,
        target.Light, 2, &target, -1, 0, kSpinStepExplosionSkill);
    PlayBuffer(kSpinStepHitSound, &target);
}

void CreateMagicPinRoots(OBJECT& caster)
{
    if (!PrepareLocalQADynamicAction(caster, 287))
        return;
    caster.CurrentAction = 287;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    CreateEffect(kMagicPinControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kMagicPinSkill);
    CreateEffect(kMagicPinRootModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kMagicPinSkill);
    PlayBuffer(kMagicPinCastSound, &caster);
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
    caster.CurrentAction = 284;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    CreateEffect(kHarshStrikeControllerModel, caster.Position, caster.Angle,
        caster.Light, 0, &caster, -1, 0, kHarshStrikeSkill);
    PlayBuffer(kHarshStrikeSound, &caster);
}

void CreateShiningPeakRoots(OBJECT& caster)
{
    if (!PrepareLocalQADynamicAction(caster, 288))
        return;
    caster.CurrentAction = 288;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
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
    caster.CurrentAction = 291;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
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
    caster.CurrentAction = 289;
    caster.AnimationFrame = 0.0f;
    caster.PriorAnimationFrame = 0.0f;
    PlayBuffer(kBrecheSound, &caster);
}

void CreateBrecheHit(OBJECT& target)
{
    if (!target.Live || !EnsureBrecheBitmaps())
        return;
    CreateEffect(kBrecheControllerModel, target.Position, target.Angle,
        target.Light, 1, &target);
}

}}
