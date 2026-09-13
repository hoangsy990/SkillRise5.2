#include "stdafx.h"
#include "PegasusSwordBlowRuntime.h"
#include "PegasusSwordBlowMath.h"
#include "ZzzBMD.h"
#include "ZzzAI.h"
#include "ZzzObject.h"
#include "ZzzEffect.h"
#include "ZzzCharacter.h"
#include "GlobalBitmap.h"
#include "PegasusRuntimeTrace.h"
#include "../../../Shared/PegasusHighSkillCatalogData.h"
#include "../../../Shared/PegasusSwordBlowProtocol.h"
#include <cmath>

namespace rise { namespace pegasus {

namespace {
struct SwordBlowFlightState
{
    OBJECT* owner;
    DWORD startedTick;
    float groundZ;
    float groundPitch;
    vec3_t targetPosition;
    DWORD lastCastTick;
    DWORD landingTick;
    unsigned short priorAction;
    int phase;
    bool rootCreated;
    bool active;
};

SwordBlowFlightState g_SwordBlowFlights[32] = {};

SwordBlowFlightState* FindSwordBlowFlight(OBJECT* owner, bool allocate)
{
    SwordBlowFlightState* freeSlot = nullptr;
    for (SwordBlowFlightState& state : g_SwordBlowFlights)
    {
        if (state.active && state.owner == owner)
            return &state;
        if (!state.active && !freeSlot)
            freeSlot = &state;
    }
    return allocate ? freeSlot : nullptr;
}

float RandomRange(float minimum, float maximum, int precision = 1)
{
    // Pegasus 0x679014: inclusive discrete range at the requested precision.
    if (precision < 1) precision = 1;
    const int steps = static_cast<int>((maximum - minimum) * precision) + 1;
    if (steps <= 1) return minimum;
    return minimum + static_cast<float>(rand() % steps) /
        static_cast<float>(precision);
}

bool EnsureSwordBlowArchangelWingModelInternal()
{
    if (!Models) return false;
    BMD& model = Models[kSwordBlowArchangelWingModel];
    if (model.NumMeshs > 3 && model.Meshs && model.IndexTexture) return true;
    char directory[] = "Data\\Npc\\";
    char filename[] = "BloodCastle01.bmd";
    model.m_iBMDSeqID = kSwordBlowArchangelWingModel;
    if (!model.Open2(directory, filename)) return false;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        const std::string texturePath = std::string(directory) +
            model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(texturePath,
            GL_LINEAR, GL_CLAMP_TO_EDGE);
        if (model.IndexTexture[mesh] == BITMAP_UNKNOWN)
        {
            model.Release();
            return false;
        }
    }
#ifdef jdk_shader_local330
    OGL330MODEL::ConvertOldMeshToVaoMesh(kSwordBlowArchangelWingModel);
#endif
    return model.NumMeshs > 3;
}

bool RenderSwordBlowArchangelWingsInternal(OBJECT& effect)
{
    if (!EnsureSwordBlowArchangelWingModelInternal()) return false;
    BMD& model = Models[kSwordBlowArchangelWingModel];
    if (!effect.Owner || !effect.Owner->Live) return false;

    // Evaluate the actual skinned BloodCastle mesh first.  The complete wing
    // plane belongs to bone 45, so a raw vertex centroid is not a valid model
    // origin.  Move the evaluated root onto the same player-back attachment
    // used by native level-1/2 wings, then evaluate once more for rendering.
    if (!Calc_RenderObject(&effect, false, 0, 0)) return false;
    vec3_t localRoot, renderedRoot, backOffset, backAnchor;
    Vector(kSwordBlowWingRootX, kSwordBlowWingRootY,
        kSwordBlowWingRootZ, localRoot);
    model.TransformPosition(BoneTransform[kSwordBlowWingModelBone],
        localRoot, renderedRoot, true);
    Vector(0.0f, 0.0f, 15.0f, backOffset);
    if (Models && effect.Owner->Type >= 0 && effect.Owner->BoneTransform &&
        Models[effect.Owner->Type].NumBones > 47)
    {
        Models[effect.Owner->Type].TransformByObjectBone(backAnchor,
            effect.Owner, 47, backOffset);
    }
    else
    {
        VectorCopy(effect.Owner->Position, backAnchor);
        backAnchor[2] += 75.0f;
    }
    effect.Position[0] += backAnchor[0] - renderedRoot[0];
    effect.Position[1] += backAnchor[1] - renderedRoot[1];
    effect.Position[2] += backAnchor[2] - renderedRoot[2];
    if (!Calc_RenderObject(&effect, false, 0, 0)) return false;
    Vector(0.65f, 1.0f, 0.75f, model.BodyLight);
    // Mesh 3 is the visible Blood Castle wing plane. It is recentered and
    // scaled by UpdateSwordBlowArchangelWings; the rejected V1 placed its raw
    // 276-unit model origin at the player's feet.
    model.RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,
        3, effect.Alpha, effect.BlendMeshTexCoordU,
        effect.BlendMeshTexCoordV, -1);
    return true;
}

void TransformLocal(const OBJECT& effect, const vec3_t local, vec3_t world)
{
    float matrix[3][4];
    vec3_t displacement;
    AngleMatrix(effect.Angle, matrix);
    VectorRotate(local, matrix, displacement);
    VectorAdd(effect.Position, displacement, world);
}

void NormalizeDirection(vec3_t direction)
{
    const float length = std::sqrt(direction[0] * direction[0] +
        direction[1] * direction[1] + direction[2] * direction[2]);
    if (length > 0.000001f)
    {
        direction[0] /= length;
        direction[1] /= length;
        direction[2] /= length;
    }
}

bool RandomOwnerBonePosition(OBJECT& owner, const vec3_t relative, vec3_t world)
{
    if (!Models || owner.Type < 0)
        return false;
    BMD& model = Models[owner.Type];
    if (model.NumBones <= 0)
        return false;
    // Pegasus 0x678F61 selects 0 when there is one bone and otherwise a
    // random bone in [0, NumBones-2], deliberately excluding the last entry.
    const int bone = model.NumBones <= 1 ? 0 : rand() % (model.NumBones - 1);
    model.TransformByObjectBone(world, &owner, bone,
        const_cast<float*>(relative));
    return true;
}
}

bool EnsureSwordBlowArchangelWingModel()
{
    return EnsureSwordBlowArchangelWingModelInternal();
}

bool RenderSwordBlowArchangelWings(OBJECT& effect)
{
    return RenderSwordBlowArchangelWingsInternal(effect);
}

bool InstallSwordBlowFlightAction()
{
    if (!Models) return false;
    BMD& player = Models[MODEL_PLAYER];
    if (player.NumActions > PLAYER_ATTACK_ONETOONE_FLY)
        return true;
    if (player.NumActions != PLAYER_ATTACK_ONETOONE_FLY ||
        PLAYER_ATTACK_ONETOONE >= player.NumActions || !player.Actions ||
        !player.Bones)
        return false;

    constexpr int kFlightKeys = 14;
    constexpr int kSourceFrames[kFlightKeys] = {
        0, 1, 2, 2, 3, 4, 5, 5, 5, 5, 6, 2, 1, 0
    };
    const int oldCount = player.NumActions;
    Action_t* actions = new Action_t[oldCount + 1];
    memcpy(actions, player.Actions, oldCount * sizeof(Action_t));
    Action_t& flight = actions[PLAYER_ATTACK_ONETOONE_FLY];
    flight.Loop = false;
    const int sourceKeysForSpeed =
        player.Actions[PLAYER_ATTACK_ONETOONE].NumAnimationKeys;
    flight.PlaySpeed = SwordBlowFlightPlaySpeed(
        player.Actions[PLAYER_ATTACK_ONETOONE].PlaySpeed,
        sourceKeysForSpeed, kFlightKeys);
    flight.NumAnimationKeys = kFlightKeys;
    flight.LockPositions = false;
    flight.Positions = nullptr;

    for (int bone = 0; bone < player.NumBones; ++bone)
    {
        Bone_t& sourceBone = player.Bones[bone];
        if (sourceBone.Dummy) continue;
        BoneMatrix_t* matrices = new BoneMatrix_t[oldCount + 1];
        memcpy(matrices, sourceBone.BoneMatrixes,
            oldCount * sizeof(BoneMatrix_t));
        BoneMatrix_t& destination = matrices[PLAYER_ATTACK_ONETOONE_FLY];
        destination.Position = new vec3_t[kFlightKeys];
        destination.Rotation = new vec3_t[kFlightKeys];
        destination.Quaternion = new vec4_t[kFlightKeys];
        const BoneMatrix_t& source =
            sourceBone.BoneMatrixes[PLAYER_ATTACK_ONETOONE];
        const int sourceKeys =
            player.Actions[PLAYER_ATTACK_ONETOONE].NumAnimationKeys;
        for (int key = 0; key < kFlightKeys; ++key)
        {
            int sourceKey = kSourceFrames[key];
            if (sourceKey >= sourceKeys) sourceKey = sourceKeys - 1;
            VectorCopy(source.Position[sourceKey], destination.Position[key]);
            VectorCopy(source.Rotation[sourceKey], destination.Rotation[key]);
            // The new clip has a distinct crouch and tucked airborne pose.
            // These are bone-channel edits, not an OBJECT-angle overlay.
            if ((bone == 3 || bone == 10) && (key == 1 || key == 2))
                destination.Rotation[key][2] -= 12.0f * 3.14159265f / 180.0f;
            if ((bone == 4 || bone == 11) && key >= 4 && key <= 8)
                destination.Rotation[key][2] -= 18.0f * 3.14159265f / 180.0f;
            if ((bone == 17 || bone == 18) && key >= 5 && key <= 9)
                destination.Rotation[key][0] -= 16.0f * 3.14159265f / 180.0f;
            AngleQuaternion(destination.Rotation[key],
                destination.Quaternion[key]);
        }
        delete[] sourceBone.BoneMatrixes;
        sourceBone.BoneMatrixes = matrices;
    }
    delete[] player.Actions;
    player.Actions = actions;
    player.NumActions = oldCount + 1;
    TraceRuntime("SWORD_BLOW_FLY_ACTION", PLAYER_ATTACK_ONETOONE,
        PLAYER_ATTACK_ONETOONE_FLY, "INSTALLED_14_KEYS");
    return true;
}

void SynchronizeSwordBlowFlightActionSpeed()
{
    if (!Models) return;
    BMD& player = Models[MODEL_PLAYER];
    if (!player.Actions || player.NumActions <= PLAYER_ATTACK_ONETOONE_FLY ||
        player.NumActions <= PLAYER_ATTACK_ONETOONE)
        return;
    Action_t& source = player.Actions[PLAYER_ATTACK_ONETOONE];
    Action_t& flight = player.Actions[PLAYER_ATTACK_ONETOONE_FLY];
    flight.PlaySpeed = SwordBlowFlightPlaySpeed(source.PlaySpeed,
        source.NumAnimationKeys, flight.NumAnimationKeys);
}

bool EnsureSwordBlowModel(int modelId)
{
    static const char* const names[] = {
        "blood_stomsword_big2.bmd", "blood_stomsword_small2.bmd",
        "blood_stomsword_plane02.bmd", "Plan_01.bmd", "multishot01_k.bmd",
        "multishot02_k.bmd", "multishot03_k.bmd",
        "blood_stomsword_plane02.bmd", "blood_stomsword_plane02.bmd"
    };
    static_assert(MODEL_PEGASUS_SWORD_BLOW_TARGET2 - MODEL_PEGASUS_SWORD_BLOW_BIG == 8,
        "Pegasus model range must match registration order");
    if (!Bitmaps.FindTexture(kSwordBlowLightmarksBitmap) &&
        !Bitmaps.LoadImageFile(kSwordBlowLightmarksBitmap,
            "Data\\RISE\\Pegasus\\SwordBlow\\Effect\\lightmarks.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE)) return false;
    if (!Bitmaps.FindTexture(kSwordBlowFlare01Bitmap) &&
        !Bitmaps.LoadImageFile(kSwordBlowFlare01Bitmap,
            "Data\\RISE\\Pegasus\\SwordBlow\\Effect\\flare01.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE)) return false;
    if (!Bitmaps.FindTexture(kSwordBlowGroundSmokeBitmap) &&
        !Bitmaps.LoadImageFile(kSwordBlowGroundSmokeBitmap,
            "Data\\RISE\\Pegasus\\SwordBlow\\Effect\\ground_smoke_dark.tga",
            GL_LINEAR, GL_CLAMP_TO_EDGE)) return false;
    if (!Bitmaps.FindTexture(kSwordBlowSparkBitmap) &&
        !Bitmaps.LoadImageFile(kSwordBlowSparkBitmap,
            "Data\\RISE\\Pegasus\\SwordBlow\\Effect\\Spark03.jpg",
            GL_LINEAR, GL_CLAMP_TO_EDGE)) return false;
    if (!Models || modelId < MODEL_PEGASUS_SWORD_BLOW_BIG ||
        modelId > MODEL_PEGASUS_SWORD_BLOW_TARGET2) return false;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.Meshs && model.IndexTexture) return true;
    char directory[] = "Data\\RISE\\Pegasus\\SwordBlow\\Effect\\";
    char filename[64];
    strcpy_s(filename, names[modelId - MODEL_PEGASUS_SWORD_BLOW_BIG]);
    model.m_iBMDSeqID = modelId;
    if (!model.Open2(directory, filename)) return false;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh) {
        const std::string texturePath = std::string(directory) + model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(texturePath, GL_NEAREST, GL_REPEAT);
        if (model.IndexTexture[mesh] == BITMAP_UNKNOWN) {
            // Native bitmap cache owns any textures already loaded.
            model.Release();
            return false;
        }
    }
#ifdef jdk_shader_local330
    OGL330MODEL::ConvertOldMeshToVaoMesh(modelId);
#endif
    return model.NumMeshs > 0;
}

void CreateSwordBlowCasterRoot(CHARACTER& caster, bool flyVariant)
{
    OBJECT& owner = caster.Object;
    vec3_t position;
    VectorCopy(owner.Position, position);
    if (Models && owner.Type >= 0)
    {
        BMD& model = Models[owner.Type];
        const int linkBone = caster.Weapon[0].LinkBone;
        if (caster.Weapon[0].Type != -1 && linkBone >= 0 && linkBone < model.NumBones)
        {
            vec3_t bonePosition;
            model.TransformByObjectBone(bonePosition, &owner, linkBone);
            // Pegasus caster_root retains actor X/Y and replaces only Z.
            position[2] = bonePosition[2];
        }
    }
    vec3_t castAngle;
    VectorCopy(owner.Angle, castAngle);
    SwordBlowFlightState* state = flyVariant
        ? FindSwordBlowFlight(&owner, false) : nullptr;
    if (state && state->active)
    {
        castAngle[0] = SwordBlowChildPitchDegrees(position[2],
            state->targetPosition[2]);
        castAngle[1] = 0.0f;
        castAngle[2] = CreateAngle(position[0], position[1],
            state->targetPosition[0], state->targetPosition[1]);
    }
    CreateEffect(MODEL_PEGASUS_SWORD_BLOW_BIG, position, castAngle,
        owner.Light, flyVariant ? 2 : 0, &owner);
}

bool StartSwordBlowFlight(CHARACTER& caster, OBJECT& target)
{
    OBJECT& owner = caster.Object;
    SwordBlowFlightState* state = FindSwordBlowFlight(&owner, true);
    if (!state) return false;
    if (state->active)
    {
        state->lastCastTick = GetTickCount();
        state->landingTick = 0;
        VectorCopy(target.Position, state->targetPosition);
        state->targetPosition[2] += 35.0f;
        owner.Angle[2] = CreateAngle2D(owner.Position, state->targetPosition);
        if (state->phase >= 2)
        {
            owner.AnimationFrame = kSwordBlowFlyThrustStartFrame;
            CreateSwordBlowCasterRoot(caster, true);
        }
        TraceRuntime("SWORD_BLOW_FLY", caster.Key,
            PLAYER_ATTACK_ONETOONE_FLY, "RETRIGGER_HELD_AIRBORNE");
        return false;
    }
    state->groundZ = owner.Position[2];
    state->groundPitch = owner.Angle[0];
    VectorCopy(target.Position, state->targetPosition);
    state->targetPosition[2] += 35.0f;
    state->owner = &owner;
    state->startedTick = GetTickCount();
    state->lastCastTick = state->startedTick;
    state->landingTick = 0;
    state->priorAction = owner.CurrentAction;
    state->phase = -1;
    state->rootCreated = false;
    state->active = true;
    owner.Angle[2] = CreateAngle2D(owner.Position, state->targetPosition);
    SetAction(&owner, PLAYER_ATTACK_ONETOONE_FLY);
    DeleteEffect(kSwordBlowArchangelWingModel, &owner);
    vec3_t light;
    Vector(kSwordBlowPlanLight.red, kSwordBlowPlanLight.green,
        kSwordBlowPlanLight.blue, light);
    CreateEffect(kSwordBlowArchangelWingModel, owner.Position, owner.Angle,
        light, 0, &owner);
    TraceRuntime("SWORD_BLOW_FLY", caster.Key,
        PLAYER_ATTACK_ONETOONE_FLY, "START");
    return true;
}

void UpdateSwordBlowFlight(CHARACTER& caster)
{
    OBJECT& owner = caster.Object;
    SwordBlowFlightState* state = FindSwordBlowFlight(&owner, false);
    if (!state || !state->active) return;
    if (!owner.Live)
    {
        state->active = false;
        return;
    }
    const DWORD now = GetTickCount();
    const DWORD elapsed = now - state->startedTick;
    const DWORD takeoffMs = static_cast<DWORD>(
        kSwordBlowFlightTakeoffEnd * kSwordBlowFlightDurationMs);
    float height = kSwordBlowFlightHeight;
    float progress = static_cast<float>(elapsed) /
        static_cast<float>(kSwordBlowFlightDurationMs);
    if (elapsed < takeoffMs)
        height = SwordBlowFlightHeightAt(progress);
    else
    {
        if (state->landingTick == 0 &&
            now - state->lastCastTick >= kSwordBlowFlightHoldAfterCastMs)
            state->landingTick = now;
        if (state->landingTick != 0)
        {
            const DWORD landingElapsed = now - state->landingTick;
            if (landingElapsed >= kSwordBlowFlightLandingDurationMs)
            {
                owner.Position[2] = state->groundZ;
                owner.Angle[0] = state->groundPitch;
                DeleteEffect(kSwordBlowArchangelWingModel, &owner);
                SetAction(&owner, state->priorAction < MAX_PLAYER_ACTION
                    ? state->priorAction : PLAYER_STOP_MALE);
                state->active = false;
                TraceRuntime("SWORD_BLOW_FLY", caster.Key, 4, "COMPLETE");
                return;
            }
            height = kSwordBlowFlightHeight * (1.0f - SwordBlowSmoothStep(
                static_cast<float>(landingElapsed) /
                static_cast<float>(kSwordBlowFlightLandingDurationMs)));
        }
    }
    owner.Position[2] = state->groundZ + height;
    owner.Angle[0] = state->groundPitch;
    if (elapsed >= takeoffMs && state->landingTick == 0)
    {
        const float dx = state->targetPosition[0] - owner.Position[0];
        const float dy = state->targetPosition[1] - owner.Position[1];
        const float horizontal = std::sqrt(dx * dx + dy * dy);
        float divePitch = std::atan2(owner.Position[2] -
            state->targetPosition[2], horizontal > 1.0f ? horizontal : 1.0f) *
            180.0f / 3.14159265f;
        if (divePitch < 18.0f) divePitch = 18.0f;
        if (divePitch > 58.0f) divePitch = 58.0f;
        const DWORD sinceCast = now - state->lastCastTick;
        float weight = 1.0f - SwordBlowSmoothStep(
            static_cast<float>(sinceCast) / 650.0f);
        owner.Angle[0] = state->groundPitch + divePitch * weight;
    }
    if (owner.CurrentAction != PLAYER_ATTACK_ONETOONE_FLY)
        SetAction(&owner, PLAYER_ATTACK_ONETOONE_FLY);
    const int phase = state->landingTick != 0 ? 3 :
        progress < kSwordBlowFlightCrouchEnd ? 0 :
        elapsed < takeoffMs ? 1 : 2;
    if (phase != state->phase)
    {
        state->phase = phase;
        if (phase == 2 && !state->rootCreated)
        {
            state->rootCreated = true;
            CreateSwordBlowCasterRoot(caster, true);
        }
        TraceRuntime("SWORD_BLOW_FLY", caster.Key, phase,
            phase == 0 ? "CROUCH" : phase == 1 ? "TAKEOFF" :
            phase == 2 ? "DIVE_THRUST" : "LANDING");
    }
}

bool RenderSwordBlowModel(OBJECT& effect)
{
    if (!EnsureSwordBlowModel(effect.Type)) return false;
    BMD& model = Models[effect.Type];
    // Pegasus 0x6795EE..0x67965A advances every 0x27B5..0x27BD carrier with
    // effect velocity / 6 before drawing it. This is visually relevant to
    // the 5/10-key multishot BMDs even though the sword meshes have one key.
    model.CurrentAction = effect.CurrentAction;
    model.PlayAnimation(&effect.AnimationFrame,
        &effect.PriorAnimationFrame, &effect.PriorAction,
        effect.Velocity / kSwordBlowModelPlayDivisor,
        effect.Position, effect.Angle);
    TraceRuntimeModelRangeAnimationOnce("SWORD_BLOW_BMD_FRAME", effect.Type,
        MODEL_PEGASUS_SWORD_BLOW_BIG, MODEL_PEGASUS_SWORD_BLOW_TARGET2,
        effect.AnimationFrame);
    if (!Calc_RenderObject(&effect, false, 0, 0)) return false;
    VectorCopy(effect.Light, model.BodyLight);
    // The SS21 trace proves that neutral 0.2 is constructor/object state, not
    // the final additive material colour. Its split contract submits mesh 0
    // once with unit blend light and then submits mesh 1 through the bright
    // path. Feeding alpha into the first blend multiplier and drawing mesh 0
    // again in white produces the silver lanes rejected by owner comparison.
    const bool flySmallSword = effect.Type == MODEL_PEGASUS_SWORD_BLOW_SMALL &&
        effect.SubType == 2;
    const float basePassLight = flySmallSword ? 0.42f : 1.0f;
    model.RenderMesh(0, RENDER_TEXTURE, effect.Alpha, 0, basePassLight,
        effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);

    if (effect.Type == MODEL_PEGASUS_SWORD_BLOW_BIG ||
        effect.Type == MODEL_PEGASUS_SWORD_BLOW_SMALL)
    {
        // Pegasus 0x84AF17..0x84B0E4 proves mesh 1 owns the missing tail.
        // The .65 multiplier below comes from the RISE preview compatibility
        // renderer, not a recovered SS21 constant. Visual parity remains open;
        // keep the neutral OBJECT light untouched while measuring the material.
        if (model.NumMeshs > 1)
        {
            // Owner rejected the neutral white/yellow tail on 2026-09-13.
            // Apply the recorded image ratio to material mesh 1 only. A
            // scalar gain cannot tint a monochrome texture. Leave mesh 0,
            // OBJECT light and the original/Fly gain separation untouched.
            if (effect.Type == MODEL_PEGASUS_SWORD_BLOW_SMALL)
            {
                const SwordBlowLight tail = SwordBlowSmallSwordTailLight(effect.Light[2]);
                Vector(tail.red, tail.green, tail.blue, model.BodyLight);
            }
            const float brightPassLight = effect.Type == MODEL_PEGASUS_SWORD_BLOW_SMALL
                ? effect.Alpha * (flySmallSword
                    ? kSwordBlowFlySmallSwordBrightPassTransfer
                    : kSwordBlowSmallSwordBrightPassTransfer)
                : effect.Alpha;
#if defined(PEGASUS_ISOLATED_TEST)
            if (effect.Type == MODEL_PEGASUS_SWORD_BLOW_SMALL && effect.Alpha > 0.0f)
            {
                static bool traced[2][3] = {};
                const int variant = flySmallSword ? 1 : 0;
                const int band = effect.Alpha < 0.33f ? 0 : effect.Alpha < 0.66f ? 1 : 2;
                if (!traced[variant][band])
                {
                    traced[variant][band] = true;
                    char detail[256];
                    _snprintf_s(detail, sizeof(detail), _TRUNCATE,
                        "mesh=1 material=%d texture=%d alpha=%.4f gain=%.4f rgb=%.5f,%.5f,%.5f",
                        model.Meshs[1].Texture, model.IndexTexture[model.Meshs[1].Texture],
                        effect.Alpha, brightPassLight, model.BodyLight[0] * brightPassLight,
                        model.BodyLight[1] * brightPassLight, model.BodyLight[2] * brightPassLight);
                    TraceRuntime("SWORD_BLOW_TAIL_SUBMIT", flySmallSword ? 1502 : 1501, band, detail);
                    const int tailTexture = model.IndexTexture[model.Meshs[1].Texture];
                    const BITMAP_t* tailBitmap = Bitmaps.FindTexture(tailTexture);
                    TraceRuntime("SWORD_BLOW_TAIL_BINDING", tailTexture, band,
                        tailBitmap ? tailBitmap->FileName : "MISSING_BITMAP");
                }
            }
#endif
            model.RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT,
                effect.Alpha, 1, brightPassLight,
                effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV, -1);
#if defined(PEGASUS_ISOLATED_TEST)
            static bool measuredTailState = false;
            if (!measuredTailState && effect.Type == MODEL_PEGASUS_SWORD_BLOW_SMALL &&
                !flySmallSword && effect.Alpha > 0.5f)
            {
                measuredTailState = true;
                GLint program = 0, texture = 0, sourceBlend = 0, destinationBlend = 0;
                GLfloat color[4] = {};
                glGetIntegerv(GL_CURRENT_PROGRAM, &program);
                glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
                glGetIntegerv(GL_BLEND_SRC, &sourceBlend);
                glGetIntegerv(GL_BLEND_DST, &destinationBlend);
                glGetFloatv(GL_CURRENT_COLOR, color);
                char detail[256];
                _snprintf_s(detail, sizeof(detail), _TRUNCATE,
                    "program=%d glTexture=%d blend=%d,%d enabled=%d currentColor=%.5f,%.5f,%.5f,%.5f",
                    program, texture, sourceBlend, destinationBlend,
                    static_cast<int>(glIsEnabled(GL_BLEND)), color[0], color[1], color[2], color[3]);
                TraceRuntime("SWORD_BLOW_TAIL_GL_STATE", 1501, 1, detail);
            }
#endif
            VectorCopy(effect.Light, model.BodyLight);
        }
    }
    if (effect.Type == MODEL_PEGASUS_SWORD_BLOW_BIG)
    {
        // Pegasus 0x677A57 uses each non-dummy bone's first key position as
        // a stable light anchor, reordered (x,-z,y), then applies the effect
        // rotation/scale/translation. It does not read the animated pose.
        vec3_t light, position, local, rotated;
        float matrix[3][4];
        AngleMatrix(effect.Angle, matrix);
        Vector(effect.Alpha * 0.25f, effect.Alpha * 0.6f,
            effect.Alpha * 0.85f, light);
        const int lastBone = model.NumBones < 10 ? model.NumBones : 10;
        for (int bone = 1; bone < lastBone; ++bone)
        {
            if (bone == 2 || bone == 3)
                continue;
            if (!model.Bones || !model.Bones[bone].BoneMatrixes ||
                !model.Bones[bone].BoneMatrixes[0].Position)
                continue;
            const vec3_t& key = *model.Bones[bone].BoneMatrixes[0].Position;
            Vector(key[0], -key[2], key[1], local);
            VectorRotate(local, matrix, rotated);
            position[0] = effect.Position[0] + rotated[0] * effect.Scale;
            position[1] = effect.Position[1] + rotated[1] * effect.Scale;
            position[2] = effect.Position[2] + rotated[2] * effect.Scale;
            const int bitmap = (bone == 9) ? kSwordBlowFlare01Bitmap
                : kSwordBlowLightmarksBitmap;
            const float scale = (bone == 1 || bone == 9) ? 6.0f : 1.5f;
            CreateSprite(bitmap, position, scale, light, &effect);
        }
    }
    return true;
}

void InitializeSwordBlowSmallSword(OBJECT& effect, float lifetime, float speed)
{
    // Pegasus 0x6790AD..0x679141, expressed with native named fields.
    effect.Distance = lifetime;
    effect.LifeTime = lifetime;
    effect.Velocity = speed;
    effect.BlendMesh = -2;
    effect.Alpha = 0.0f;
    effect.BlendMeshLight = 0.0f;
    if (effect.SubType == 2)
    {
        // Fly-only visual upgrade. Original Sword Blow keeps the recovered
        // neutral constructor/material path byte-for-byte.
        Vector(kSwordBlowFlySmallSwordImageLight.red,
            kSwordBlowFlySmallSwordImageLight.green,
            kSwordBlowFlySmallSwordImageLight.blue, effect.Light);
    }
    else
    {
        Vector(kSwordBlowNeutralModelLight.red,
            kSwordBlowNeutralModelLight.green,
            kSwordBlowNeutralModelLight.blue, effect.Light);
    }
}

void InitializeSwordBlowArchangelWings(OBJECT& effect)
{
    effect.Distance = static_cast<float>(kSwordBlowFlightDurationMs);
    effect.LifeTime = 30000;
    effect.m_dwTime = GetTickCount();
    effect.Alpha = 0.0f;
    effect.BlendMesh = -2;
    effect.BlendMeshLight = 0.0f;
    effect.Scale = effect.Owner ? effect.Owner->Scale * 0.82f : 0.82f;
}

void UpdateSwordBlowArchangelWings(OBJECT& effect, float)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0;
        return;
    }
    // Keep the source NPC wing's own bone orientation and follow the player's
    // pose/yaw. The render path resolves its skinned root onto back bone 47.
    VectorCopy(effect.Owner->Angle, effect.Angle);
    effect.Scale = effect.Owner->Scale * 0.82f;
    VectorCopy(effect.Owner->Position, effect.Position);
    SwordBlowFlightState* state = FindSwordBlowFlight(effect.Owner, false);
    if (!state || !state->active)
    {
        effect.LifeTime = 0;
        return;
    }
    float alpha = kSwordBlowFlightWingOpacity;
    const DWORD now = GetTickCount();
    const DWORD fadeIn = now - state->startedTick;
    if (fadeIn < 250)
        alpha *= SwordBlowSmoothStep(static_cast<float>(fadeIn) / 250.0f);
    if (state->landingTick != 0)
        alpha *= 1.0f - SwordBlowSmoothStep(static_cast<float>(
            now - state->landingTick) / kSwordBlowFlightLandingDurationMs);
    effect.Alpha = alpha;
    effect.BlendMeshLight = effect.Alpha;
}

void InitializeSwordBlowBigSword(OBJECT& effect)
{
    // Pegasus constructor 0x6790A2 and recovered shared initializer.
    effect.Distance = 15.0f;
    effect.LifeTime = 15;
    effect.Velocity = 30.0f;
    effect.BlendMesh = -2;
    effect.Alpha = 0.0f;
    effect.BlendMeshLight = 0.0f;
    effect.Scale = 1.0f;
    Vector(kSwordBlowNeutralModelLight.red, kSwordBlowNeutralModelLight.green,
        kSwordBlowNeutralModelLight.blue, effect.Light);
}

void InitializeSwordBlowTarget(OBJECT& effect)
{
    // Jump-table entries 0x27BC/0x27BD both land at 0x679230. Lifetime is
    // the sole override; native alpha/blend/light inputs remain intact.
    effect.LifeTime = 15;
}

void InitializeSwordBlowPlane(OBJECT& effect)
{
    // Pegasus 0x679146 supplies the scale rule. Guarded SS21 runtime capture
    // supersedes its old tint and proves neutral (0.2,0.2,0.2) for all 0x6A4.
    Vector(kSwordBlowNeutralModelLight.red, kSwordBlowNeutralModelLight.green,
        kSwordBlowNeutralModelLight.blue, effect.Light);
    effect.Distance = 50.0f;
    effect.LifeTime = 50;
    effect.Scale = (effect.SubType == 1) ? 3.0f : 5.0f;
    effect.Gravity = effect.Scale;
}

void InitializeSwordBlowRing(OBJECT& effect)
{
    effect.Angle[0] = RandomRange(0.0f, 360.0f);
    effect.Angle[1] = RandomRange(0.0f, 360.0f);
    effect.Angle[2] = RandomRange(0.0f, 360.0f);
    Vector(kSwordBlowPlanLight.red, kSwordBlowPlanLight.green,
        kSwordBlowPlanLight.blue, effect.Light);
    effect.BlendMesh = -2;
    effect.BlendMeshLight = effect.Alpha;
    effect.LifeTime = 15;
}

void InitializeSwordBlowMultishot(OBJECT& effect)
{
    vec3_t local, world;
    Vector(-10.0f, -60.0f, 0.0f, local);
    TransformLocal(effect, local, world);
    VectorCopy(world, effect.Position);
    effect.Velocity = 0.8f;
    effect.Gravity = 0.0f;
    effect.BlendMesh = -2;
    effect.LifeTime = 16;
}

void CreateSwordBlowChildren(OBJECT& effect)
{
    if (!effect.Owner || !effect.Owner->Live)
    {
        effect.LifeTime = 0;
        return;
    }
    const bool flyVariant = effect.SubType == 2;
    const bool hasSwordWrath = g_isCharacterBuff(effect.Owner,
        static_cast<eBuffState>(kSwordWrathNativeEffect));
    const SwordBlowSpread spread = GetSwordBlowSpread(hasSwordWrath);
    for (int n = 0; n < spread.childCount; ++n)
    {
        vec3_t localStart, worldStart, localEnd, worldEnd, childAngle;
        Vector(RandomRange(-spread.lateralExtent, spread.lateralExtent),
            RandomRange(kSwordBlowChildStartForwardMin,
                kSwordBlowChildStartForwardMax),
            RandomRange(kSwordBlowChildStartHeightMin,
                kSwordBlowChildStartHeightMax), localStart);
        TransformLocal(effect, localStart, worldStart);
        SwordBlowFlightState* state = flyVariant
            ? FindSwordBlowFlight(effect.Owner, false) : nullptr;
        if (state && state->active)
        {
            VectorCopy(state->targetPosition, worldEnd);
        }
        else
        {
            Vector(0.0f, kSwordBlowChildEndForward,
                kSwordBlowChildEndHeight, localEnd);
            TransformLocal(effect, localEnd, worldEnd);
        }
        // Pegasus 0x677E19..0x677E84 computes startZ-endZ before asin.
        // Reversing this subtraction mirrors the child's pitch.
        Vector(SwordBlowChildPitchDegrees(worldStart[2], worldEnd[2]), 0.0f,
            CreateAngle(worldStart[0], worldStart[1], worldEnd[0], worldEnd[1]), childAngle);
        CreateEffect(MODEL_PEGASUS_SWORD_BLOW_SMALL, worldStart, childAngle,
            effect.Light, flyVariant ? 2 : 1, effect.Owner, -1, 0, 0, 0,
            RandomRange(0.4f, 0.7f, 10));
    }

    // The recovered child helper initializes and advances the large root only
    // after creating the small swords, so they retain the caster light.
    InitializeSwordBlowBigSword(effect);
    vec3_t localAdvance, worldAdvance;
    Vector(0.0f, 200.0f, 0.0f, localAdvance);
    TransformLocal(effect, localAdvance, worldAdvance);
    VectorCopy(worldAdvance, effect.Position);

    vec3_t localPlane, worldPlane;
    Vector(0.0f, -200.0f, 0.0f, localPlane);
    TransformLocal(effect, localPlane, worldPlane);
    for (int subtype = 0; subtype < 2; ++subtype)
        CreateEffect(MODEL_PEGASUS_SWORD_BLOW_PLANE, worldPlane, effect.Angle,
            effect.Light, subtype, effect.Owner);
}

void UpdateSwordBlowSmallSword(OBJECT& effect, float animationFactor)
{
    // Pegasus 0x679334: rotate local (0,-velocity*dt,0), not a common
    // caster direction. Every child keeps its individually constructed angle.
    float matrix[3][4];
    vec3_t local, displacement;
    Vector(0.0f, -effect.Velocity * animationFactor, 0.0f, local);
    AngleMatrix(effect.Angle, matrix);
    VectorRotate(local, matrix, displacement);
    VectorAdd(effect.Position, displacement, effect.Position);
#if defined(PEGASUS_ISOLATED_TEST)
    static bool movementTraced = false;
    if (!movementTraced && effect.Velocity * animationFactor > 0.0f)
    {
        movementTraced = true;
        TraceRuntime("MOVEMENT_MILESTONE", kSwordBlowSkill,
            static_cast<int>(effect.Velocity * animationFactor * 1000.0f),
            "SMALL_SWORD_ADVANCE");
    }
#endif
    effect.Alpha = SwordBlowOpacity(effect.LifeTime, effect.Distance);
    effect.BlendMeshLight = effect.Alpha;
}

void UpdateSwordBlowBigSword(OBJECT& effect, float animationFactor)
{
    float matrix[3][4];
    vec3_t local, displacement;
    Vector(0.0f, -effect.Velocity * animationFactor, 0.0f, local);
    AngleMatrix(effect.Angle, matrix);
    VectorRotate(local, matrix, displacement);
    VectorAdd(effect.Position, displacement, effect.Position);
#if defined(PEGASUS_ISOLATED_TEST)
    static bool movementTraced = false;
    if (!movementTraced && effect.Velocity * animationFactor > 0.0f)
    {
        movementTraced = true;
        TraceRuntime("MOVEMENT_MILESTONE", kSwordBlowSkill,
            static_cast<int>(effect.Velocity * animationFactor * 1000.0f),
            "BIG_SWORD_ADVANCE");
    }
#endif

    // Pegasus 0x678119..0x67829D: one burst while lifetime crosses 13.
    if ((effect.LifeTime + animationFactor) >= 13.0f && effect.LifeTime <= 13.0f)
    {
        vec3_t local, position, light;
        Vector(0.0f, -400.0f, -10.0f, local);
        TransformLocal(effect, local, position);
        const bool hasSwordWrath = effect.Owner && g_isCharacterBuff(effect.Owner,
            static_cast<eBuffState>(kSwordWrathNativeEffect));
        const float scale = hasSwordWrath ? 0.7f : 1.3f;
        Vector(kSwordBlowMultishot3Light.red, kSwordBlowMultishot3Light.green,
            kSwordBlowMultishot3Light.blue, light);
        for (int n = 0; n < 2; ++n)
            CreateEffect(MODEL_PEGASUS_SWORD_BLOW_MULTISHOT3, position, effect.Angle,
                light, 0, effect.Owner, -1, 0, 0, 0, scale);
        Vector(kSwordBlowMultishotLight.red, kSwordBlowMultishotLight.green,
            kSwordBlowMultishotLight.blue, light);
        for (int n = 0; n < 3; ++n)
            CreateEffect(MODEL_PEGASUS_SWORD_BLOW_MULTISHOT1, position, effect.Angle,
                light, 0, effect.Owner, -1, 0, 0, 0, scale);
        for (int n = 0; n < 2; ++n)
            CreateEffect(MODEL_PEGASUS_SWORD_BLOW_MULTISHOT2, position, effect.Angle,
                light, 0, effect.Owner, -1, 0, 0, 0, scale);
    }

    // Pegasus 0x6782A5..0x678395: a plane on every crossed five-frame mark.
    if (effect.LifeTime < 15.0f &&
        CrossedDescendingCadence(effect.LifeTime, animationFactor, 5.0f))
    {
        vec3_t local, position;
        Vector(0.0f, -200.0f, 0.0f, local);
        TransformLocal(effect, local, position);
        CreateEffect(MODEL_PEGASUS_SWORD_BLOW_PLANE, position, effect.Angle,
            effect.Direction, 1, effect.Owner);
    }
    effect.Alpha = SwordBlowOpacity(effect.LifeTime, effect.Distance);
    effect.BlendMeshLight = effect.Alpha;
}

void UpdateSwordBlowPlane(OBJECT& effect, float animationFactor)
{
    float matrix[3][4];
    vec3_t local, displacement;
    Vector(0.0f, -40.0f * animationFactor, 0.0f, local);
    AngleMatrix(effect.Angle, matrix);
    VectorRotate(local, matrix, displacement);
    VectorAdd(effect.Position, displacement, effect.Position);
    effect.Scale -= 0.35f * animationFactor;
    const float divisor = effect.Gravity > 0.0f ? effect.Gravity : 1.0f;
    effect.Alpha = effect.Scale / divisor;
    if (effect.Scale <= 0.0f)
    {
        effect.Scale = 0.0f;
        effect.Alpha = 0.0f;
        effect.LifeTime = 0;
    }
}

void UpdateSwordBlowRing(OBJECT& effect, float animationFactor)
{
    effect.Scale += 0.2f * animationFactor;
    effect.Alpha -= 0.1f * animationFactor;
    if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
    effect.BlendMeshLight = effect.Alpha;
}

void UpdateSwordBlowMultishot(OBJECT& effect, float animationFactor)
{
    switch (effect.Type)
    {
    case MODEL_PEGASUS_SWORD_BLOW_MULTISHOT1:
        effect.Scale += 0.15f * animationFactor;
        effect.Alpha = effect.LifeTime * 0.0625f;
        break;
    case MODEL_PEGASUS_SWORD_BLOW_MULTISHOT2:
        effect.Scale += 0.2f * animationFactor;
        effect.Alpha = effect.LifeTime * 0.0625f;
        break;
    case MODEL_PEGASUS_SWORD_BLOW_MULTISHOT3:
        effect.Scale += 0.35f * animationFactor;
        effect.Alpha = (effect.LifeTime - 4.0f) * 0.0625f;
        if (effect.Alpha < 0.0f) effect.Alpha = 0.0f;
        break;
    }
    effect.BlendMeshLight = effect.Alpha;
}

void UpdateSwordBlowTarget(OBJECT& effect, float animationFactor)
{
    if (!effect.Owner || !effect.Owner->Live)
        return;
    if (effect.Type == MODEL_PEGASUS_SWORD_BLOW_TARGET1)
    {
        vec3_t light, position, direction, local, rotated;
        Vector(0.0f, 0.0f, 0.0f, local);
        if (!RandomOwnerBonePosition(*effect.Owner, local, position))
            VectorCopy(effect.Owner->Position, position);
        VectorCopy(position, effect.Position);
        Vector(kSwordBlowPlanLight.red, kSwordBlowPlanLight.green,
            kSwordBlowPlanLight.blue, light);

        Vector(0.0f, -RandomRange(50.0f, 150.0f), 0.0f, local);
        float matrix[3][4];
        AngleMatrix(effect.Angle, matrix);
        VectorRotate(local, matrix, rotated);
        Vector(-rotated[0], -rotated[1], -RandomRange(0.0f, 100.0f), direction);
        NormalizeDirection(direction);
        CreateParticle(kSwordBlowSparkBitmap, effect.Position, direction, light, 37, 8.0f);

        Vector(RandomRange(-10.0f, 10.0f), RandomRange(-10.0f, 10.0f),
            50.0f, direction);
        NormalizeDirection(direction);
        CreateParticle(kSwordBlowSparkBitmap, effect.Position, direction, light, 36, 8.0f);
        if (CrossedDescendingCadence(effect.LifeTime, animationFactor, 3.0f))
        {
            CreateParticle(kSwordBlowGroundSmokeBitmap, position, effect.Owner->Angle,
                light, 2, 1.2f);
            CreateEffect(MODEL_PEGASUS_SWORD_BLOW_RING, position, effect.Owner->Angle,
                light, 0, effect.Owner, -1, 0, 0, 0, 4.0f);
            Vector(kSwordBlowFlareLight.red, kSwordBlowFlareLight.green,
                kSwordBlowFlareLight.blue, light);
            CreateParticle(BITMAP_LIGHT, position, effect.Owner->Angle, light, 0, 2.5f,
                &effect, -32768, true);
        }
    }
    else
    {
        vec3_t light, position, relative;
        Vector(kSwordBlowLightningLight.red, kSwordBlowLightningLight.green,
            kSwordBlowLightningLight.blue, light);
        for (int n = 0; n < 2; ++n)
        {
            Vector(static_cast<float>((rand() % 100) - 50),
                static_cast<float>((rand() % 100) - 50),
                static_cast<float>((rand() % 100) - 50), relative);
            if (!RandomOwnerBonePosition(*effect.Owner, relative, position))
                VectorAdd(effect.Owner->Position, relative, position);
            VectorCopy(position, effect.Position);
            CreateParticle(BITMAP_LIGHTNING_MEGA1 + (rand() % 3), position,
                effect.Owner->Angle,
                light, 0, static_cast<float>((rand() % 60) + 22) * 0.02f,
                nullptr, -32768, true);
        }
    }
}

}}
