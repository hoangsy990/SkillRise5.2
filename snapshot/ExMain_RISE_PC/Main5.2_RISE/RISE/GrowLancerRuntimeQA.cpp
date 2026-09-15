#include "stdafx.h"
#include "GrowLancerRuntimeQA.h"

#ifdef RISE_GROW_LANCER_RUNTIME_QA

#include "GrowLancerEffectRuntime.h"
#include "GrowLancerResources.h"
#include "GrowLancerCirclePersistent.h"
#include "GrowLancerCirclePreviewGate.h"
#include "MapManager.h"
#include "ZzzBMD.h"
#include "ZzzTexture.h"
#include "ZzzCharacter.h"
#include "CharacterManager.h"
#include "ZzzInfomation.h"
#include "ZzzEffect.h"
#include "ZzzAI.h"
#include "SkillEffectMgr.h"
#include "ZzzOpenglUtil.h"
#include "ZzzLodTerrain.h"
#include "NewUICommon.h"
#include "NewUISystem.h"
#include "_GlobalFunctions.h"

extern int SelectedCharacter;
extern float BoneScale;

namespace rise { namespace growlancer {
namespace {

int gRuntimeQASkill = 0;
bool gRuntimeQAIconPanelVisible = false;
struct ShiningSample { float frame, factor, life; int stage; unsigned short action; };
ShiningSample gShiningSamples[512];
unsigned int gShiningSampleCount = 0;
unsigned int gShiningDropped = 0;
bool gShiningArmed = false;
struct ControllerSample
{
    const OBJECT* slot;
    float frame, factor, life;
    float priorFrame, actionSpeed;
    float observedFrame;
    unsigned short observedAction;
    bool observationAvailable;
    int stage, action, target;
    DWORD elapsed;
};
ControllerSample gControllerSamples[512];
unsigned int gControllerCount = 0, gControllerDropped = 0;
int gControllerSkill = 0;
struct WrathGroundSample
{
    int blend, source, destination, texture, expectedTexture;
    float scale, life, light[3];
    DWORD elapsed;
};
WrathGroundSample gWrathGroundSamples[32];
unsigned gWrathGroundCount = 0;
struct WrathGroundPixelPoint
{
    bool valid;
    int x, y;
    unsigned char before[4], after[4]; // RGBA avoids RGB 4-byte pack padding
};
struct WrathGroundPixelSample
{
    GLuint query;
    unsigned samples;
    int centerX, centerY, blendSrc, blendDst, boundTexture;
    int readBuffer, drawBuffer;
    DWORD elapsed;
    WrathGroundPixelPoint points[5];
};
WrathGroundPixelSample gWrathGroundPixels[4];
unsigned gWrathGroundPixelCount = 0;
struct WrathBuffVisualSample
{
    const char* stage;
    int variant;
    int attachment;
    int bone;
    unsigned action;
    float frame;
    DWORD elapsed;
};
// One bounded sequence: eleven lightmarks, two scatter marks, two bone flares
// and four blue mono particles (19 records). A later frame must not flood the
// QA log, but the bound must include the post-buff owner tail we need to see.
WrathBuffVisualSample gWrathBuffVisualSamples[32];
unsigned gWrathBuffVisualCount = 0;
struct MagicFrameSample
{
    bool owned, ran;
    unsigned long long before, after, completed;
    unsigned action;
    float frame;
    DWORD elapsed;
};
MagicFrameSample gMagicFrames[512];
unsigned gMagicFrameCount = 0, gMagicFrameDropped = 0;
struct SpinOutcome { int outcome, type, subtype, target; float frame; DWORD elapsed; };
SpinOutcome gSpinOutcomes[64];
unsigned gSpinOutcomeCount = 0, gSpinOutcomeDropped = 0;
unsigned gSpinRenderCount = 0;
unsigned gSpinCrossProbeCount = 0;
unsigned gSpinBlurDrawCount = 0;
unsigned gCircleEmissionCount = 0;
unsigned gTargetSkillRenderCount = 0;
unsigned gMagicRasterProbeCount[2] = {};
unsigned gBrecheCasterRenderCount = 0;
DWORD gControllerStart = 0;
const OBJECT* gCirclePreviewActor = nullptr;
DWORD gCirclePreviewUntil = 0;
int gCirclePreviewWorld = -1;

const int kRuntimeQASkills[] =
{
    271, 272, 273, 274, 275, 276, 277, 278, 279
};

const char* SkillName(int id)
{
    switch (id)
    {
    case 271: return "Spin Step";
    case 272: return "Circle Shield";
    case 273: return "Obsidian";
    case 274: return "Magic Pin";
    case 275: return "Clash";
    case 276: return "Harsh Strike";
    case 277: return "Shining Peak";
    case 278: return "Wrath";
    case 279: return "Breche";
    default: return "unknown";
    }
}

// Dispatch diagnostics only: void Create* APIs cannot prove allocation or parity.
void AppendQALog(const char* message)
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    char line[640];
    sprintf_s(line, sizeof(line),
        "%04u-%02u-%02uT%02u:%02u:%02u.%03uZ pid=%lu %s\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds, GetCurrentProcessId(), message);
    OutputDebugStringA(line);
    FILE* file = 0;
    if (fopen_s(&file, "GrowLancerRuntimeQA.log", "a") == 0 && file)
    {
        fputs(line, file);
        fclose(file);
    }
}

void WriteQALog(const char* eventName, int skill, int target)
{
    char message[256];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=%s skill=%d name=%s target=%d",
        eventName, skill, SkillName(skill), target);
    AppendQALog(message);
}

bool IsGrowLancerParticle(int type)
{
    return type == kFlareBlueBitmap || type == kShockwave2Bitmap ||
        type == kWrathComboBitmap || type == kWrathAlphaLightBitmap ||
        type == kXsuper0001Bitmap ||
        (type >= kXsuper0002Bitmap && type <= kXsuper0006Bitmap);
}

void WriteCastInputSnapshot(const char* phase, int skill)
{
    // Observations only. Do not trigger recalculation or read potentially
    // uninitialized PlaySpeed for the still-unmapped dynamic actions.
    if (!Hero || !CharacterAttribute)
        return;
    char message[384];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=cast-input phase=%s skill=%d class=%u attackSpeed=%lu magicSpeed=%lu ability=%u action=%u frame=%.6f attributeAliasesMachine=%d",
        phase, skill, static_cast<unsigned int>(CharacterAttribute->Class),
        static_cast<unsigned long>(CharacterAttribute->AttackSpeed),
        static_cast<unsigned long>(CharacterAttribute->MagicSpeed),
        static_cast<unsigned int>(CharacterAttribute->Ability),
        static_cast<unsigned int>(Hero->Object.CurrentAction),
        static_cast<double>(Hero->Object.AnimationFrame),
        CharacterMachine && CharacterAttribute == &CharacterMachine->Character ? 1 : 0);
    AppendQALog(message);
}

void WritePoolSnapshot(const char* reason)
{
    int liveEffects = 0;
    int growLancerEffects = 0;
    for (int i = 0; i < MAX_EFFECTS; ++i)
    {
        if (!Effects[i].Live)
            continue;
        ++liveEffects;
        if (IsEffectType(Effects[i].Type))
            ++growLancerEffects;
    }
    for (int i = 0; i < g_SkillEffects.GetSize(); ++i)
    {
        OBJECT* effect = g_SkillEffects.GetEffect(i);
        if (!effect || !effect->Live)
            continue;
        ++liveEffects;
        if (IsEffectType(effect->Type))
            ++growLancerEffects;
    }

    int liveParticles = 0;
    int growLancerParticles = 0;
    int circleUpperArmParticles = 0;
    // Shared native textures cannot establish Grow Lancer ownership, but
    // must remain visible when checking a quiet-scene before/after baseline.
    int sharedEnergyParticles = 0;
    int sharedCloudParticles = 0;
    for (int i = 0; i < MAX_PARTICLES; ++i)
    {
        if (!Particles[i].Live)
            continue;
        ++liveParticles;
        if (Particles[i].Type == BITMAP_ENERGY)
            ++sharedEnergyParticles;
        if (Particles[i].Type == BITMAP_CLUD64)
            ++sharedCloudParticles;
        if (IsGrowLancerParticle(static_cast<int>(Particles[i].Type)))
            ++growLancerParticles;
        if (Particles[i].Type == kCircleUpperArmMonoBitmap &&
            Particles[i].SubType == kCircleMonoSubType)
            ++circleUpperArmParticles;
    }

    int liveJoints = 0;
    int growLancerJoints = 0;
    int circleContactJoints = 0;
    for (int i = 0; i < MAX_JOINTS; ++i)
    {
        if (!Joints[i].Live)
            continue;
        ++liveJoints;
        if (Hero && Joints[i].Target == &Hero->Object &&
            (Joints[i].Type == BITMAP_PIN_LIGHT || Joints[i].Type == BITMAP_FORCEPILLAR))
            ++growLancerJoints;
        if (Joints[i].Type == BITMAP_FORCEPILLAR && Joints[i].SubType == 3)
            ++circleContactJoints;
    }

    char line[384];
    sprintf_s(line, sizeof(line),
        "GrowLancerQA event=pool reason=%s effects=%d glEffects=%d particles=%d glParticles=%d circleMono=%d sharedEnergy=%d sharedCloud=%d joints=%d glJoints=%d circleContact=%d fps=%.2f avg=%.2f",
        reason, liveEffects, growLancerEffects, liveParticles,
        growLancerParticles, circleUpperArmParticles, sharedEnergyParticles,
        sharedCloudParticles, liveJoints, growLancerJoints,
        circleContactJoints, FPS, FPS_AVG);
    AppendQALog(line);
}

OBJECT* SelectedTarget()
{
    if (SelectedCharacter < 0 || SelectedCharacter >= MAX_CHARACTERS_CLIENT)
        return 0;
    OBJECT* target = &CharactersClient[SelectedCharacter].Object;
    return target->Live ? target : 0;
}

void FlushShiningSamples()
{
    if (!gShiningArmed)
        return;
    gShiningArmed = false;
    char message[384];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=shining-samples count=%u dropped=%u phase=before-controller-update",
        gShiningSampleCount, gShiningDropped);
    AppendQALog(message);
    for (unsigned int i = 0; i < gShiningSampleCount; ++i)
    {
        const ShiningSample& s = gShiningSamples[i];
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=shining-sample index=%u action=%u frame=%.9f factor=%.9f stage=%d life=%.6f",
            i, static_cast<unsigned int>(s.action), static_cast<double>(s.frame),
            static_cast<double>(s.factor), s.stage, static_cast<double>(s.life));
        AppendQALog(message);
    }
}

void FlushControllerSamples()
{
    if (!gControllerSkill) return;
    char message[512];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=controller-samples skill=%d count=%u dropped=%u phase=before-update",
        gControllerSkill, gControllerCount, gControllerDropped);
    AppendQALog(message);
    for (unsigned int i = 0; i < gControllerCount; ++i)
    {
        const ControllerSample& s = gControllerSamples[i];
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=controller-sample skill=%d index=%u slot=%p ms=%lu action=%d frame=%.9f stage=%d life=%.6f factor=%.9f target=%d priorFrame=%.9f actionSpeed=%.9f observationAvailable=%d observedAction=%u observedFrame=%.9f",
            gControllerSkill, i, static_cast<const void*>(s.slot), s.elapsed,
            s.action, static_cast<double>(s.frame), s.stage,
            static_cast<double>(s.life), static_cast<double>(s.factor), s.target,
            static_cast<double>(s.priorFrame), static_cast<double>(s.actionSpeed),
            s.observationAvailable ? 1 : 0, static_cast<unsigned>(s.observedAction),
            static_cast<double>(s.observedFrame));
        AppendQALog(message);
    }
    if (gControllerSkill == 271)
    {
        sprintf_s(message, sizeof(message), "GrowLancerQA event=spin-outcomes count=%u dropped=%u",
            gSpinOutcomeCount, gSpinOutcomeDropped);
        AppendQALog(message);
        for (unsigned i = 0; i < gSpinOutcomeCount; ++i)
        {
            const SpinOutcome& s = gSpinOutcomes[i];
            sprintf_s(message, sizeof(message),
                "GrowLancerQA event=spin-outcome outcome=%d type=%d subtype=%d target=%d frame=%.9f ms=%lu",
                s.outcome,s.type,s.subtype,s.target,static_cast<double>(s.frame),s.elapsed);
            AppendQALog(message);
        }
    }
    if (gControllerSkill == 278)
    {
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=wrath-ground-samples count=%u limit=32 phase=after-terrain-call",
            gWrathGroundCount);
        AppendQALog(message);
        for (unsigned i = 0; i < gWrathGroundCount; ++i)
        {
            const WrathGroundSample& s = gWrathGroundSamples[i];
            sprintf_s(message, sizeof(message),
                "GrowLancerQA event=wrath-ground index=%u ms=%lu blend=%d src=%d dst=%d texture=%d expectedTexture=%d matched=%d scale=%.6f life=%.6f rgb=%.6f,%.6f,%.6f",
                i, s.elapsed, s.blend, s.source, s.destination, s.texture,
                s.expectedTexture,
                s.expectedTexture != 0 && s.texture == s.expectedTexture ? 1 : 0,
                static_cast<double>(s.scale), static_cast<double>(s.life),
                static_cast<double>(s.light[0]), static_cast<double>(s.light[1]), static_cast<double>(s.light[2]));
            AppendQALog(message);
        }
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=wrath-ground-pixel-summary count=%u limit=4 phase=around-terrain-call",
            gWrathGroundPixelCount);
        AppendQALog(message);
        for (unsigned i = 0; i < gWrathGroundPixelCount; ++i)
        {
            const WrathGroundPixelSample& s = gWrathGroundPixels[i];
            sprintf_s(message, sizeof(message),
                "GrowLancerQA event=wrath-ground-pixel-draw index=%u ms=%lu samples=%u center=%d,%d src=%d dst=%d texture=%d readBuffer=%d drawBuffer=%d",
                i, s.elapsed, s.samples, s.centerX, s.centerY,
                s.blendSrc, s.blendDst, s.boundTexture,
                s.readBuffer, s.drawBuffer);
            AppendQALog(message);
            for (unsigned p = 0; p < 5; ++p)
            {
                const WrathGroundPixelPoint& point = s.points[p];
                const bool darkened = point.valid &&
                    (point.after[0] < point.before[0] ||
                     point.after[1] < point.before[1] ||
                     point.after[2] < point.before[2]);
                sprintf_s(message, sizeof(message),
                    "GrowLancerQA event=wrath-ground-pixel index=%u point=%u valid=%d x=%d y=%d before=%u,%u,%u after=%u,%u,%u darkened=%d",
                    i, p, point.valid ? 1 : 0, point.x, point.y,
                    point.before[0], point.before[1], point.before[2],
                    point.after[0], point.after[1], point.after[2],
                    darkened ? 1 : 0);
                AppendQALog(message);
            }
        }
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=wrath-buff-visual-summary count=%u limit=32 "
            "phase=first-owner-sequence",
            gWrathBuffVisualCount);
        AppendQALog(message);
        for (unsigned i = 0; i < gWrathBuffVisualCount; ++i)
        {
            const WrathBuffVisualSample& s = gWrathBuffVisualSamples[i];
            sprintf_s(message, sizeof(message),
                "GrowLancerQA event=wrath-buff-visual index=%u stage=%s "
                "variant=%d attachment=%d bone=%d action=%u frame=%.9f ms=%lu",
                i, s.stage, s.variant, s.attachment, s.bone, s.action,
                static_cast<double>(s.frame), s.elapsed);
            AppendQALog(message);
        }
    }
    if (gControllerSkill == 274)
    {
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=magic-frames count=%u dropped=%u phase=after-particles",
            gMagicFrameCount, gMagicFrameDropped);
        AppendQALog(message);
        for (unsigned i = 0; i < gMagicFrameCount; ++i)
        {
            const MagicFrameSample& s = gMagicFrames[i];
            sprintf_s(message, sizeof(message),
                "GrowLancerQA event=magic-frame index=%u ms=%lu owned=%d ran=%d pendingBefore=%llu pendingAfter=%llu completed=%llu action=%u frame=%.9f",
                i, s.elapsed, s.owned ? 1 : 0, s.ran ? 1 : 0,
                s.before, s.after, s.completed, s.action, static_cast<double>(s.frame));
            AppendQALog(message);
        }
    }
    if (gControllerSkill == 275 || gControllerSkill == 279)
    {
        sprintf_s(message, sizeof(message),
            "GrowLancerQA event=target-skill-render-summary skill=%d count=%u limit=256 phase=before-update",
            gControllerSkill, gTargetSkillRenderCount);
        AppendQALog(message);
    }
    gControllerSkill = 0;
}

void CastCurrentSkill()
{
    const int skill = kRuntimeQASkills[gRuntimeQASkill];
    if (!Hero || !Hero->Object.Live)
    {
        WriteQALog("cast-rejected-no-caster", skill, -1);
        return;
    }
    OBJECT& caster = Hero->Object;
    // This isolated F7 fixture is the owner-approved temporary DK preview,
    // not a replacement for the still-open native Grow Lancer class. A newer
    // class byte such as 30 can otherwise silently fail PrepareLocalQAAction
    // while this wrapper misleadingly logs root-dispatched success.
    const int sourceClass = CharacterAttribute ? CharacterAttribute->Class : -1;
    const int baseClass = CharacterAttribute ?
        gCharacterManager.GetBaseClass(sourceClass) : -1;
    if (!CharacterMachine ||
        CharacterAttribute != &CharacterMachine->Character ||
        baseClass != CLASS_KNIGHT)
    {
        if (g_pChatListBox)
            g_pChatListBox->AddText("", "Grow Lancer QA F7: use a Dark Knight test character",
                SEASON3B::TYPE_ERROR_MESSAGE);
        char message[180] = {};
        sprintf_s(message,
            "GrowLancerQA event=cast-rejected-class skill=%d sourceClass=%d baseClass=%d requiredBase=%d",
            skill, sourceClass, baseClass, CLASS_KNIGHT);
        AppendQALog(message);
        return;
    }
    // Spin's source frame-gated fanout resolves a retained target index.
    // A targetless QA cast cannot exercise that chain; do not report it as
    // root-dispatched success or arm samples for an invalid fixture.
    if ((skill == 271 || skill == 275 || skill == 279) && !SelectedTarget())
    {
        if (skill == 279)
            g_pChatListBox->AddText("", "Breche: select a target before F7", SEASON3B::TYPE_ERROR_MESSAGE);
        WriteQALog("cast-rejected-no-target", skill, -1);
        return;
    }
    if ((skill == 271 && caster.CurrentAction == 285 && caster.AnimationFrame > 0.01f) ||
        (skill == 274 && caster.CurrentAction == 287 && caster.AnimationFrame > 0.01f))
    {
        // Native skill dispatch does not accept a second cast while the same
        // action is still running.  Replaying the QA action would reset Spin's
        // 4.7..6.5 fanout window (or leave Magic Pin past 5.2/7.0/10.0), which
        // was the observed intermittent visual result.  Fail closed and let
        // the existing action finish before accepting the next cast.
        const char* message = skill == 271 ?
            "Spin Step: wait for the current action to finish" :
            "Magic Pin: wait for the current action to finish";
        g_pChatListBox->AddText("", message, SEASON3B::TYPE_ERROR_MESSAGE);
        WriteQALog("cast-rejected-action-busy", skill, -1);
        return;
    }
    // Flush outside controller updates. Capture one isolated F7 cast at a time.
    FlushShiningSamples();
    FlushControllerSamples();
    const int sampleSkill = skill;
    if (sampleSkill == 271 || sampleSkill == 274 || sampleSkill == 275 ||
        sampleSkill == 276 || sampleSkill == 278 || sampleSkill == 279)
    {
        gControllerCount = gControllerDropped = 0;
        gMagicFrameCount = gMagicFrameDropped = 0;
        gWrathGroundCount = 0;
        gWrathGroundPixelCount = 0;
        gWrathBuffVisualCount = 0;
        gSpinOutcomeCount = gSpinOutcomeDropped = 0;
        gSpinRenderCount = 0;
        gSpinCrossProbeCount = 0;
        gSpinBlurDrawCount = 0;
        gTargetSkillRenderCount = 0;
        gMagicRasterProbeCount[0] = gMagicRasterProbeCount[1] = 0;
        ResetBrecheCasterRenderQA();
        gControllerStart = GetTickCount();
        gControllerSkill = sampleSkill;
    }
    if (kRuntimeQASkills[gRuntimeQASkill] == 277)
    {
        gShiningSampleCount = gShiningDropped = 0;
        gShiningArmed = true;
    }
    WriteCastInputSnapshot("before-dispatch", kRuntimeQASkills[gRuntimeQASkill]);
    bool actionPrepared = true;
    switch (kRuntimeQASkills[gRuntimeQASkill])
    {
    case 271: actionPrepared = CreateSpinStepRoot(caster, static_cast<short>(SelectedCharacter)); break;
    case 272:
        CreateCircleShieldRoot(caster);
        gCircleEmissionCount = 0;
        // Source shoulder emission is post-buff, not the attack root.  The
        // current GS deliberately does not grant an unproven S21 buff, so
        // this owner-only QA probe is armed by F7 for three seconds.  Keep
        // buff state and all server packets untouched.
        gCirclePreviewActor = &caster;
        gCirclePreviewWorld = gMapManager.WorldActive;
        gCirclePreviewUntil = GetTickCount() + 3000;
        if (g_pChatListBox)
            g_pChatListBox->AddText("", "Circle Shield QA: shoulder preview 3s (visual only)",
                SEASON3B::TYPE_SYSTEM_MESSAGE);
        WriteQALog("circle-shoulder-preview-armed-visual-only", skill, -1);
        break;
    case 273: CreateObsidianRoots(caster); break;
    case 274: actionPrepared = CreateMagicPinRoots(caster); break;
    case 275: CreateClashRoot(caster, static_cast<short>(SelectedCharacter)); break;
    case 276: CreateHarshStrikeRoot(caster); break;
    case 277: CreateShiningPeakRoots(caster); break;
    case 278: CreateWrathRoot(caster); break;
    case 279:
        // Native ReceiveMagic faces the source toward the resolved target
        // before dispatching action 190.  F7 bypasses that packet path, so it
        // must reproduce the same named-field behavior before the owner
        // controller snapshots caster.Angle.  Without this, the verified
        // (0,-200,0) owner offset follows a stale movement angle and appears
        // sideways from the selected target.
        if (OBJECT* target = SelectedTarget())
            caster.Angle[2] = CreateAngle2D(caster.Position, target->Position);
        CreateBrecheAction(caster);
        // F7 is a visual-only fixture and sends no skill packet.  SetAction()
        // already enters the native character animation path, so do not arm
        // CHARACTER::Skill/AttackTime/TargetCharacter as if a server-approved
        // cast had occurred.  Leaving those fields modified while connected
        // to an unrelated GS creates a client-only attack state with no
        // authoritative reply and can interfere with subsequent movement QA.
        if (caster.CurrentAction == 289)
            WriteQALog("breche-caster-action-local-only", 279, SelectedCharacter);
        // QA convenience only: the real receive path remains in
        // WSclient::ReceiveMagic.  When a live target is selected, F7 also
        // emits the resolved-target contact root so Breche can be inspected
        // without entering Siege or hand-driving F9.  This is not a server
        // packet shortcut and is compiled only into the isolated QA client.
        if (OBJECT* target = SelectedTarget())
        {
            CreateBrecheHit(*target);
            WriteQALog("breche-contact-qa-auto", 279, SelectedCharacter);

            // Never replay the target contact root on the caster.  S21 has a
            // separate owner-side presentation path; until its producer,
            // asset and attachment are pinned, this QA path remains
            // target-only.
        }
        break;
    }
    if (!actionPrepared)
    {
        gControllerSkill = 0;
        if (g_pChatListBox)
            g_pChatListBox->AddText("", "Grow Lancer QA: action/model unavailable; cast not started",
                SEASON3B::TYPE_ERROR_MESSAGE);
        WriteQALog("cast-rejected-action-model-unavailable", skill,
            SelectedTarget() ? SelectedCharacter : -1);
        return;
    }
    WriteCastInputSnapshot("after-dispatch", kRuntimeQASkills[gRuntimeQASkill]);
    WriteQALog(kRuntimeQASkills[gRuntimeQASkill] == 279 ?
        "action-dispatched-visual-chain-unverified" : "root-dispatched",
        kRuntimeQASkills[gRuntimeQASkill],
        SelectedTarget() ? SelectedCharacter : -1);
}

void EmitCurrentContact()
{
    OBJECT* target = SelectedTarget();
    if (!target)
    {
        WriteQALog("contact-rejected-no-target", kRuntimeQASkills[gRuntimeQASkill], -1);
        return;
    }
    const int skill = kRuntimeQASkills[gRuntimeQASkill];
    if (skill == 271)
        CreateSpinStepHit(*target);
    else if (skill == 272)
    {
        if (!Hero || !Hero->Object.Live || target == &Hero->Object)
        {
            WriteQALog("contact-rejected-no-distinct-caster", skill, SelectedCharacter);
            return;
        }
        // Explicit isolated visual fixture, NOT a received buff/damage packet.
        // Preserve receiver actor order: first=Hero, second=selected target.
        CreateCircleShieldContact(Hero->Object, *target);
        WriteQALog("circle-contact-qa-only-first-hero-second-selected", skill, SelectedCharacter);
        return;
    }
    else if (skill == 274)
        CreateMagicPinHit(*target);
    else if (skill == 279)
        CreateBrecheHit(*target);
    else
    {
        WriteQALog("contact-unsupported", skill, SelectedCharacter);
        return;
    }
    WriteQALog("contact-dispatched", skill, SelectedCharacter);
}

void ToggleSkillIconQA()
{
	gRuntimeQAIconPanelVisible = !gRuntimeQAIconPanelVisible;
	WriteQALog(gRuntimeQAIconPanelVisible ? "icon-panel-on" : "icon-panel-off", 0, -1);
}

void ToggleWrathBuffFixtureQA()
{
    if (!Hero || !Hero->Object.Live)
    {
        WriteQALog("wrath-buff-fixture-rejected-no-caster", 278, -1);
        return;
    }
    OBJECT& caster = Hero->Object;
    const eBuffState buff = static_cast<eBuffState>(424);
    if (g_isCharacterBuff((&caster), buff))
    {
        // Flush the bounded observation before removing the native buff so the
        // final owner-bound frame and the full post-buff order are retained.
        WritePoolSnapshot("wrath-buff-before-off");
        g_CharacterUnRegisterBuff((&caster), buff);
        WriteQALog("wrath-buff-fixture-off", 278, 424);
        FlushControllerSamples();
        WritePoolSnapshot("wrath-buff-after-off");
    }
    else
    {
        // Isolated QA convenience only: no packet, learning, persistence or
        // server authority is implied. The ID is the recovered base Wrath
        // buff gate; production builds do not compile this handler.
        FlushControllerSamples();
        gControllerCount = gControllerDropped = 0;
        gWrathGroundCount = 0;
        gWrathGroundPixelCount = 0;
        gWrathBuffVisualCount = 0;
        gTargetSkillRenderCount = 0;
        gControllerStart = GetTickCount();
        gControllerSkill = 278;
        g_CharacterRegisterBuff((&caster), buff);
        WriteQALog("wrath-buff-fixture-on", 278, 424);
        WritePoolSnapshot("wrath-buff-on");
    }
}

}

void RecordMapEffectTeardownQA(const char* phase)
{
    // Count native pool slots before/after map teardown without dereferencing
    // retired world objects. The diagnostic does not change effect ownership.
    WritePoolSnapshot(phase ? phase : "map-teardown");
    char message[160];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=map-style1-blurs phase=%s live=%d",
        phase ? phase : "map-teardown", CountStyleOneObjectBlursQA());
    AppendQALog(message);
}

int RuntimeQASelectedSkillId()
{
    return kRuntimeQASkills[gRuntimeQASkill];
}

const char* RuntimeQASelectedSkillName()
{
    return SkillName(RuntimeQASelectedSkillId());
}

int RuntimeQASelectedTargetIndex()
{
    return SelectedTarget() ? SelectedCharacter : -1;
}

bool RuntimeQASelectedTargetLive()
{
    return SelectedTarget() != 0;
}

unsigned BeginWrathGroundPixelsQA(const OBJECT& effect)
{
    if (gControllerSkill != 278 || effect.Type != kWrathBrokenBitmap ||
        gWrathGroundPixelCount >= 4 || !wglGetCurrentContext()) return 0;
    char enabled[4] = {};
    if (GetEnvironmentVariableA("RISE_GL_WRATH_PIXEL_QA", enabled,
        sizeof(enabled)) != 1 || enabled[0] != '1') return 0;
    GLint currentQuery = 0;
    glGetQueryiv(GL_SAMPLES_PASSED, GL_CURRENT_QUERY, &currentQuery);
    if (currentQuery) return 0;
    GLint viewport[4] = {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    if (viewport[2] <= 0 || viewport[3] <= 0) return 0;
    vec3_t ground = {effect.Position[0], effect.Position[1],
        RequestTerrainHeight(effect.Position[0], effect.Position[1]) + 5.0f};
    // Native TransformPosition divides by PerspectiveX/Y and camera Z.
    // Check its exact camera-space input before calling the projection.
    if (PerspectiveX <= 0.000001f || PerspectiveY <= 0.000001f)
        return 0;
    vec3_t fromCamera = {};
    vec3_t cameraSpace = {};
    VectorSubtract(ground, CameraPosition, fromCamera);
    VectorRotate(fromCamera, CameraMatrix, cameraSpace);
    if (cameraSpace[2] >= -1.0f) return 0;
    int centerX = 0, centerY = 0;
    TransformPosition(ground, cameraSpace, &centerX, &centerY);
    if (centerX < viewport[0] ||
        centerY < viewport[1] || centerX >= viewport[0] + viewport[2] ||
        centerY >= viewport[1] + viewport[3]) return 0;
    GLuint query = 0;
    glGenQueries(1, &query);
    if (!query) return 0;
    WrathGroundPixelSample& sample = gWrathGroundPixels[gWrathGroundPixelCount];
    sample = {};
    sample.query = query;
    sample.centerX = centerX;
    sample.centerY = centerY;
    sample.elapsed = GetTickCount() - gControllerStart;
    glGetIntegerv(GL_BLEND_SRC, &sample.blendSrc);
    glGetIntegerv(GL_BLEND_DST, &sample.blendDst);
    glGetIntegerv(GL_READ_BUFFER, &sample.readBuffer);
    glGetIntegerv(GL_DRAW_BUFFER, &sample.drawBuffer);
    const int offsets[5][2] = {{0, 0}, {32, 0}, {-32, 0}, {0, 32}, {0, -32}};
    for (unsigned pointIndex = 0; pointIndex < 5; ++pointIndex)
    {
        WrathGroundPixelPoint& point = sample.points[pointIndex];
        point.x = centerX + offsets[pointIndex][0];
        point.y = centerY + offsets[pointIndex][1];
        point.valid = point.x >= viewport[0] && point.y >= viewport[1] &&
            point.x < viewport[0] + viewport[2] &&
            point.y < viewport[1] + viewport[3];
        if (point.valid)
            glReadPixels(point.x, point.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                point.before);
    }
    glBeginQuery(GL_SAMPLES_PASSED, query);
    ++gWrathGroundPixelCount;
    return gWrathGroundPixelCount; // one-based bounded token, not a GL handle
}

void EndWrathGroundPixelsQA(unsigned token)
{
    if (!token || token > gWrathGroundPixelCount || token > 4) return;
    WrathGroundPixelSample& sample = gWrathGroundPixels[token - 1];
    if (!sample.query) return;
    glEndQuery(GL_SAMPLES_PASSED);
    glGetQueryObjectuiv(sample.query, GL_QUERY_RESULT, &sample.samples);
    glDeleteQueries(1, &sample.query);
    sample.query = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &sample.boundTexture);
    for (WrathGroundPixelPoint& point : sample.points)
        if (point.valid)
            glReadPixels(point.x, point.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                point.after);
}

void RecordWrathGroundQA(const OBJECT& effect)
{
    // Bounded diagnostic readback only during an explicitly armed Wrath QA cast.
    if (gControllerSkill != 278 || effect.Type != kWrathBrokenBitmap ||
        gWrathGroundCount >= 32) return;
    WrathGroundSample& sample = gWrathGroundSamples[gWrathGroundCount++];
    sample.blend = glIsEnabled(GL_BLEND) ? 1 : 0;
    glGetIntegerv(GL_BLEND_SRC, &sample.source);
    glGetIntegerv(GL_BLEND_DST, &sample.destination);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &sample.texture);
    const BITMAP_t* expected = Bitmaps.FindTexture(effect.Type);
    sample.expectedTexture = expected ?
        static_cast<int>(expected->TextureNumber) : 0;
    sample.scale = effect.Scale;
    sample.life = effect.LifeTime;
    for (int axis = 0; axis < 3; ++axis) sample.light[axis] = effect.Light[axis];
    sample.elapsed = GetTickCount() - gControllerStart;
}

void RecordWrathBuffVisualQA(const OBJECT& caster, const char* stage,
    int variant, int attachment, int bone)
{
    // This is deliberately owner-only and bounded to the first sequence of
    // the explicitly armed F7 Wrath QA cast. It observes the existing native
    // submissions; it never creates a buff, particle or sprite.
    if (gControllerSkill != 278 || !Hero || &caster != &Hero->Object ||
        !caster.Live || !stage || gWrathBuffVisualCount >= 32)
        return;
    WrathBuffVisualSample& sample =
        gWrathBuffVisualSamples[gWrathBuffVisualCount++];
    sample.stage = stage;
    sample.variant = variant;
    sample.attachment = attachment;
    sample.bone = bone;
    sample.action = caster.CurrentAction;
    sample.frame = caster.AnimationFrame;
    sample.elapsed = GetTickCount() - gControllerStart;
}

void RecordMagicFrameQA(bool owned, bool ran, unsigned long long before,
    unsigned long long after, unsigned long long completed)
{
    if (gControllerSkill != 274 || !Hero) return;
    if (gMagicFrameCount >= 512)
    {
        if (gMagicFrameDropped != 0xffffffffu) ++gMagicFrameDropped;
        return;
    }
    MagicFrameSample& sample = gMagicFrames[gMagicFrameCount++];
    sample.owned = owned;
    sample.ran = ran;
    sample.before = before;
    sample.after = after;
    sample.completed = completed;
    sample.action = Hero->Object.CurrentAction;
    sample.frame = Hero->Object.AnimationFrame;
    sample.elapsed = GetTickCount() - gControllerStart;
}

void RecordControllerQASample(const OBJECT& controller, float factor)
{
    if (!gControllerSkill || !Hero || controller.Owner != &Hero->Object ||
        controller.SubType != 0) return;
    const int skill = controller.Type == kSpinControllerModel ? 271 :
        controller.Type == kMagicPinControllerModel ? 274 :
        controller.Type == kHarshStrikeControllerModel ? 276 : 0;
    if (skill != gControllerSkill) return;
    if (gControllerCount >= 512)
    {
        if (gControllerDropped != 0xffffffffu) ++gControllerDropped;
        return;
    }
    ControllerSample& s = gControllerSamples[gControllerCount++];
    s.slot = &controller;
    s.elapsed = GetTickCount() - gControllerStart;
    s.action = Hero->Object.CurrentAction;
    s.frame = Hero->Object.AnimationFrame;
    s.priorFrame = Hero->Object.PriorAnimationFrame;
    // Snapshot the registered speed; -1 means unavailable, not a guessed speed.
    // This is not the final post-modifier animation increment or a GPU timestamp.
    s.actionSpeed = -1.0f;
    if (Models && Hero->Object.Type == MODEL_PLAYER && s.action >= 0 &&
        s.action < Models[MODEL_PLAYER].NumActions && Models[MODEL_PLAYER].Actions)
        s.actionSpeed = Models[MODEL_PLAYER].Actions[s.action].PlaySpeed;
    s.stage = controller.AttackPoint[0];
    s.life = controller.LifeTime;
    s.factor = factor;
    s.target = controller.m_sTargetIndex;
    s.observedFrame = -1.f;
    s.observedAction = 0;
    s.observationAvailable = skill == 274 && ReadCharacterAnimationObservation(
        controller, s.observedAction, s.observedFrame);
}

void RecordSpinOutcomeQA(const OBJECT& effect, int outcome, int targetIndex)
{
    if (gControllerSkill != 271 || !Hero || !Hero->Object.Live) return;
    if (gSpinOutcomeCount >= 64)
    {
        if (gSpinOutcomeDropped != 0xffffffffu) ++gSpinOutcomeDropped;
        return;
    }
    gSpinOutcomes[gSpinOutcomeCount++] = {outcome,effect.Type,effect.SubType,
        targetIndex,Hero->Object.AnimationFrame,GetTickCount()-gControllerStart};
}

void RecordSpinRenderQA(const OBJECT& effect, const char* stage, int result,
    int meshCount, int boneCount, int actionCount, int textureId)
{
    if (gControllerSkill != 271 || !Hero || !Hero->Object.Live ||
        !stage || gSpinRenderCount >= 128)
        return;
    // Keep the render telemetry bounded. Render calls are per-frame, so only
    // the first 128 observations are written; this is enough
    // to distinguish cull, EnsureModel, Calc_RenderObject and submission.
    char message[640];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=spin-render stage=%s result=%d type=%d subtype=%d live=%d visible=%d target=%d life=%.6f alpha=%.6f scale=%.6f pos=%.2f,%.2f,%.2f mesh=%d bones=%d actions=%d texture=%d ms=%lu",
        stage, result, effect.Type, effect.SubType, effect.Live ? 1 : 0,
        effect.Visible ? 1 : 0, effect.m_sTargetIndex,
        static_cast<double>(effect.LifeTime), static_cast<double>(effect.Alpha),
        static_cast<double>(effect.Scale), static_cast<double>(effect.Position[0]),
        static_cast<double>(effect.Position[1]), static_cast<double>(effect.Position[2]),
        meshCount, boneCount, actionCount, textureId,
        GetTickCount() - gControllerStart);
    AppendQALog(message);
    ++gSpinRenderCount;
}

void RecordSpinBlurDrawQA(const OBJECT* owner, int segments, int life,
    int textureId, const float* head1, const float* head2,
    const float* tail1, const float* tail2)
{
    if (gControllerSkill != 271 || !Hero || !Hero->Object.Live ||
        !owner || !owner->Live || owner->Owner != &Hero->Object ||
        owner->Type != kSpinControllerModel || owner->SubType != 0 ||
        segments < 1 || !head1 || !head2 || !tail1 || !tail2 ||
        gSpinBlurDrawCount >= 128)
        return;
    const bool textureRegistered = Bitmaps.FindTexture(textureId) != NULL;
    char message[512];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=spin-blur-draw segments=%d life=%d texture=%d registered=%d owner=%p action=%u frame=%.6f ms=%lu head1=%.2f,%.2f,%.2f head2=%.2f,%.2f,%.2f tail1=%.2f,%.2f,%.2f tail2=%.2f,%.2f,%.2f",
        segments, life, textureId, textureRegistered ? 1 : 0,
        static_cast<const void*>(owner), Hero->Object.CurrentAction,
        static_cast<double>(Hero->Object.AnimationFrame),
        GetTickCount() - gControllerStart,
        static_cast<double>(head1[0]), static_cast<double>(head1[1]), static_cast<double>(head1[2]),
        static_cast<double>(head2[0]), static_cast<double>(head2[1]), static_cast<double>(head2[2]),
        static_cast<double>(tail1[0]), static_cast<double>(tail1[1]), static_cast<double>(tail1[2]),
        static_cast<double>(tail2[0]), static_cast<double>(tail2[1]), static_cast<double>(tail2[2]));
    AppendQALog(message);
    ++gSpinBlurDrawCount;
}

void RecordTargetSkillRenderQA(const OBJECT& effect, int skill,
    const char* stage, int result, int meshCount, int boneCount,
    int actionCount, int textureId)
{
    if ((skill != 274 && skill != 275 && skill != 279) ||
        gControllerSkill != skill ||
        !Hero || !Hero->Object.Live || !stage ||
        gTargetSkillRenderCount >= 256)
        return;
    char message[720];
    const OBJECT* owner = effect.Owner;
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=target-skill-render skill=%d stage=%s result=%d type=%d subtype=%d live=%d visible=%d target=%d owner=%p ownerLive=%d ownerType=%d ownerAction=%u life=%.6f alpha=%.6f scale=%.6f pos=%.2f,%.2f,%.2f mesh=%d bones=%d actions=%d texture=%d ms=%lu angle=%.2f,%.2f,%.2f",
        skill, stage, result, effect.Type, effect.SubType,
        effect.Live ? 1 : 0, effect.Visible ? 1 : 0, effect.m_sTargetIndex,
        static_cast<const void*>(owner), owner && owner->Live ? 1 : 0,
        owner ? owner->Type : -1,
        owner ? static_cast<unsigned>(owner->CurrentAction) : 0u,
        static_cast<double>(effect.LifeTime), static_cast<double>(effect.Alpha),
        static_cast<double>(effect.Scale), static_cast<double>(effect.Position[0]),
        static_cast<double>(effect.Position[1]), static_cast<double>(effect.Position[2]),
        meshCount, boneCount, actionCount, textureId,
        GetTickCount() - gControllerStart,
        static_cast<double>(effect.Angle[0]),
        static_cast<double>(effect.Angle[1]),
        static_cast<double>(effect.Angle[2]));
    AppendQALog(message);
    ++gTargetSkillRenderCount;
}

void RecordMagicMaterialQA(int modelType, unsigned textureId,
    const char* bmdTextureName)
{
    if (gControllerSkill != 274 || !bmdTextureName || !*bmdTextureName)
        return;
    const int types[] = {kMagicPin01Model, kMagicPin03Model,
        kMagicPinRootModel, kMagicPinAuxModel};
    int slot = -1;
    for (int i = 0; i < 4; ++i)
        if (types[i] == modelType) { slot = i; break; }
    if (slot < 0) return;
    static bool recorded[4] = {};
    if (recorded[slot]) return;
    recorded[slot] = true;

    const std::string expected = std::string("Data\\RISE\\GrowLancer\\MagicPin\\") +
        bmdTextureName;
    BITMAP_t* bitmap = Bitmaps.FindTexture(textureId);
    const bool exactPath = bitmap &&
        _stricmp(bitmap->FileName, expected.c_str()) == 0 &&
        Bitmaps.FindTexture(expected) == bitmap;
    const bool gpuAllocated = bitmap && glIsTexture(bitmap->TextureNumber);
    GLint previous = 0, gpuWidth = 0, gpuHeight = 0;
    if (gpuAllocated)
    {
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous);
        glBindTexture(GL_TEXTURE_2D, bitmap->TextureNumber);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &gpuWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &gpuHeight);
        glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previous));
    }
    char message[620] = {};
    sprintf_s(message,
        "GrowLancerQA event=magic-material type=%d id=%u bmdName=%.80s bitmapPath=%.250s exactPath=%d cpuWidth=%.0f cpuHeight=%.0f components=%d gpuAllocated=%d gpuWidth=%d gpuHeight=%d ms=%lu",
        modelType, textureId, bmdTextureName,
        bitmap ? bitmap->FileName : "missing", exactPath ? 1 : 0,
        bitmap ? bitmap->Width : 0.f,
        bitmap ? bitmap->Height : 0.f,
        bitmap ? static_cast<int>(bitmap->Components) : 0,
        gpuAllocated ? 1 : 0, gpuWidth, gpuHeight,
        GetTickCount() - gControllerStart);
    AppendQALog(message);
}

unsigned BeginMagicPinFootSamplesQA(const OBJECT& effect)
{
    const int modelSlot = effect.Type == kMagicPin01Model ? 0 :
        effect.Type == kMagicPinRootModel ? 1 : -1;
    if (gControllerSkill != 274 || modelSlot < 0 ||
        effect.Alpha < .99f || gMagicRasterProbeCount[modelSlot] >= 8)
        return 0;
    char enabled[4] = {};
    if (GetEnvironmentVariableA("RISE_GL_MAGIC_RASTER_QA", enabled,
        sizeof(enabled)) != 1 || enabled[0] != '1')
        return 0;
    GLint active = 0;
    glGetQueryiv(GL_SAMPLES_PASSED, GL_CURRENT_QUERY, &active);
    if (active) return 0; // never nest/interrupt another renderer query
    GLuint query = 0;
    glGenQueries(1, &query);
    if (!query) return 0;
#ifdef jdk_shader_local330
    // RenderMesh may enqueue its draw.  Exclude previously queued meshes
    // before opening this diagnostic query; production batching is untouched.
    if (OGL330::IsShader() && GMMeshShader)
        GMMeshShader->FlushAllMesh();
#endif
    glBeginQuery(GL_SAMPLES_PASSED, query);
    ++gMagicRasterProbeCount[modelSlot];
    return query;
}

void EndMagicPinFootSamplesQA(const OBJECT& effect, unsigned query)
{
    if (!query) return;
    unsigned vao = 0, indices = 0;
    int shaderBackend = 0;
#ifdef jdk_shader_local330
    // Consume the Magic Pin mesh while its query is still open.  Without
    // this flush a successful shader enqueue falsely reads as zero samples.
    shaderBackend = OGL330::IsShader() ? 1 : 0;
    if (shaderBackend && GMMeshShader)
        GMMeshShader->FlushAllMesh();
    const BMD& model = Models[effect.Type];
    if (model.NewMeshes && !model.NewMeshes->empty())
    {
        vao = (*model.NewMeshes)[0].VAO;
        indices = (*model.NewMeshes)[0].IndexCount;
    }
#endif
    glEndQuery(GL_SAMPLES_PASSED);
    GLuint samples = 0;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samples);
    glDeleteQueries(1, &query);
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    const int depthTest = glIsEnabled(GL_DEPTH_TEST) ? 1 : 0;
    const int cullFace = glIsEnabled(GL_CULL_FACE) ? 1 : 0;
    char message[320] = {};
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=magic-raster-probe type=%d candidate=%s alpha=%.4f life=%.2f samples=%u shader=%d vao=%u indices=%u program=%d depth=%d cull=%d scope=render-mesh-only shaderQueueFlushed=1 ms=%lu",
        effect.Type,
        effect.Type == kMagicPin01Model ? "angular-strip" : "annular-root",
        static_cast<double>(effect.Alpha),
        static_cast<double>(effect.LifeTime), samples, shaderBackend,
        vao, indices, program, depthTest, cullFace,
        GetTickCount() - gControllerStart);
    AppendQALog(message);
}

unsigned BeginSpinCrossSamplesQA(const OBJECT& effect)
{
    if (gControllerSkill != 271 || effect.Type != kSpinCrossModel ||
        !effect.Owner || !effect.Owner->Live || effect.Alpha <= 0.3f ||
        gSpinCrossProbeCount >= 12 || !wglGetCurrentContext()) return 0;
    char enabled[4] = {};
    if (GetEnvironmentVariableA("RISE_GL_SPIN_CROSS_RASTER_QA", enabled,
        sizeof(enabled)) != 1 || enabled[0] != '1') return 0;
    GLint active = 0;
    glGetQueryiv(GL_SAMPLES_PASSED, GL_CURRENT_QUERY, &active);
    if (active) return 0;
    GLuint query = 0;
    glGenQueries(1, &query);
    if (!query) return 0;
#ifdef jdk_shader_local330
    if (OGL330::IsShader() && GMMeshShader)
        GMMeshShader->FlushAllMesh();
#endif
    glBeginQuery(GL_SAMPLES_PASSED, query);
    ++gSpinCrossProbeCount;
    return query;
}

void EndSpinCrossSamplesQA(const OBJECT& effect, unsigned query)
{
    if (!query) return;
    unsigned vao = 0, indices = 0;
    int shaderBackend = 0;
#ifdef jdk_shader_local330
    shaderBackend = OGL330::IsShader() ? 1 : 0;
    if (shaderBackend && GMMeshShader)
        GMMeshShader->FlushAllMesh();
    const BMD& model = Models[effect.Type];
    if (model.NewMeshes && !model.NewMeshes->empty())
    {
        vao = (*model.NewMeshes)[0].VAO;
        indices = (*model.NewMeshes)[0].IndexCount;
    }
#endif
    glEndQuery(GL_SAMPLES_PASSED);
    GLuint samples = 0;
    glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samples);
    glDeleteQueries(1, &query);
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    char message[460] = {};
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=spin-cross-raster-probe index=%u type=%d ownerLive=%d pos=%.2f,%.2f,%.2f ownerPos=%.2f,%.2f,%.2f alpha=%.4f scale=%.4f life=%.2f samples=%u shader=%d vao=%u indices=%u program=%d depth=%d cull=%d scope=cross-mesh-only shaderQueueFlushed=1 ms=%lu",
        gSpinCrossProbeCount, effect.Type,
        effect.Owner && effect.Owner->Live ? 1 : 0,
        static_cast<double>(effect.Position[0]),
        static_cast<double>(effect.Position[1]),
        static_cast<double>(effect.Position[2]),
        static_cast<double>(effect.Owner->Position[0]),
        static_cast<double>(effect.Owner->Position[1]),
        static_cast<double>(effect.Owner->Position[2]),
        static_cast<double>(effect.Alpha), static_cast<double>(effect.Scale),
        static_cast<double>(effect.LifeTime), samples, shaderBackend,
        vao, indices, program, glIsEnabled(GL_DEPTH_TEST) ? 1 : 0,
        glIsEnabled(GL_CULL_FACE) ? 1 : 0,
        GetTickCount() - gControllerStart);
    AppendQALog(message);
}

void ResetBrecheCasterRenderQA()
{
    gBrecheCasterRenderCount = 0;
}

void RecordBrecheCasterRenderQA(const OBJECT& actor, const char* stage,
    int bodyPart, int modelType, int meshCount, int boneCount,
    int actionCount, int result)
{
    // Owner-only: remote actors with the same action must not make the local
    // Breche cast look rendered. This is observational QA telemetry only.
    if (gControllerSkill != 279 || !Hero || &actor != &Hero->Object ||
        !actor.Live || !stage || gBrecheCasterRenderCount >= 128)
        return;
    char message[720];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=breche-caster-render stage=%s result=%d bodyPart=%d modelType=%d actorType=%d live=%d alpha=%.6f visible=%d action=%u frame=%.9f priorFrame=%.9f mesh=%d bones=%d actions=%d pos=%.2f,%.2f,%.2f ms=%lu",
        stage, result, bodyPart, modelType, actor.Type, actor.Live ? 1 : 0,
        static_cast<double>(actor.Alpha), actor.Visible ? 1 : 0,
        static_cast<unsigned>(actor.CurrentAction),
        static_cast<double>(actor.AnimationFrame),
        static_cast<double>(actor.PriorAnimationFrame), meshCount, boneCount,
        actionCount, static_cast<double>(actor.Position[0]),
        static_cast<double>(actor.Position[1]), static_cast<double>(actor.Position[2]),
        GetTickCount() - gControllerStart);
    AppendQALog(message);
    ++gBrecheCasterRenderCount;
}

void RecordBrecheOwnerResourceGateQA(bool resourcesReady, int targetIndex,
    bool controllerCreated)
{
    if (gControllerSkill != 279 || !Hero || !Hero->Object.Live)
        return;
    char message[256];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=breche-owner-resource-gate ready=%d controllerCreated=%d target=%d",
        resourcesReady ? 1 : 0, controllerCreated ? 1 : 0, targetIndex);
    AppendQALog(message);
}

void RecordBrecheActionStopQA(const OBJECT& actor, int skill, int attackTime,
    int numAnimationKeys, float playSpeed, bool playResult)
{
    if (gControllerSkill != 279 || !Hero || &actor != &Hero->Object ||
        !actor.Live || gBrecheCasterRenderCount >= 128)
        return;
    char message[320];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=breche-action-step action=%u frame=%.9f skill=%d attackTime=%d keys=%d speed=%.6f play=%d",
        static_cast<unsigned>(actor.CurrentAction),
        static_cast<double>(actor.AnimationFrame), skill, attackTime,
        numAnimationKeys, static_cast<double>(playSpeed), playResult ? 1 : 0);
    AppendQALog(message);
}

void RecordBrecheActionBoundaryQA(const OBJECT& actor, const char* stage,
    unsigned short before, unsigned short after, bool playResult)
{
    if (gControllerSkill != 279 || !Hero || &actor != &Hero->Object ||
        !actor.Live || gBrecheCasterRenderCount >= 128 ||
        (before != 289 && after != 289))
        return;
    char message[320];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=breche-action-boundary stage=%s before=%u after=%u play=%d frame=%.9f skill=%d attackTime=%d",
        stage ? stage : "unknown", static_cast<unsigned>(before),
        static_cast<unsigned>(after), playResult ? 1 : 0,
        static_cast<double>(actor.AnimationFrame), Hero->Skill,
        static_cast<int>(Hero->AttackTime));
    AppendQALog(message);
}

void RecordShiningQASample(const OBJECT& controller, float factor)
{
    if (!gShiningArmed || !Hero || controller.Owner != &Hero->Object ||
        controller.SubType != 0)
        return;
    if (gShiningSampleCount >= 512)
    {
        if (gShiningDropped != 0xffffffffu) ++gShiningDropped;
        return;
    }
    ShiningSample& s = gShiningSamples[gShiningSampleCount++];
    s.frame = Hero->Object.AnimationFrame;
    s.action = Hero->Object.CurrentAction;
    s.factor = factor;
    s.stage = controller.AttackPoint[0];
    s.life = controller.LifeTime;
}

int RunBodyTextureProbeQA()
{
    char enabled[4] = {};
    const bool bodyProbe = GetEnvironmentVariableA("RISE_GL_BODY_TEXTURE_QA",
        enabled, sizeof(enabled)) == 1 && enabled[0] == '1';
    char circleEnabled[4] = {};
    const bool circleProbe = GetEnvironmentVariableA("RISE_GL_CIRCLE_BITMAP_QA",
        circleEnabled, sizeof(circleEnabled)) == 1 && circleEnabled[0] == '1';
    char spinEnabled[4] = {};
    const bool spinProbe = GetEnvironmentVariableA("RISE_GL_SPIN_BLUR_BITMAP_QA",
        spinEnabled, sizeof(spinEnabled)) == 1 && spinEnabled[0] == '1';
    char wrathEnabled[4] = {};
    const bool wrathProbe = GetEnvironmentVariableA("RISE_GL_WRATH_MODEL_QA",
        wrathEnabled, sizeof(wrathEnabled)) == 1 && wrathEnabled[0] == '1';
    char privateModelsEnabled[4] = {};
    const bool privateModelsProbe = GetEnvironmentVariableA("RISE_GL_PRIVATE_MODELS_QA",
        privateModelsEnabled, sizeof(privateModelsEnabled)) == 1 &&
        privateModelsEnabled[0] == '1';
    char magicDrawEnabled[4] = {};
    const bool magicDrawProbe = GetEnvironmentVariableA("RISE_GL_MAGIC_NATIVE_DRAW_QA",
        magicDrawEnabled, sizeof(magicDrawEnabled)) == 1 &&
        magicDrawEnabled[0] == '1';
    char spinCrossDrawEnabled[4] = {};
    const bool spinCrossDrawProbe = GetEnvironmentVariableA(
        "RISE_GL_SPIN_CROSS_DRAW_QA", spinCrossDrawEnabled,
        sizeof(spinCrossDrawEnabled)) == 1 && spinCrossDrawEnabled[0] == '1';
    if (!bodyProbe && !circleProbe && !spinProbe && !wrathProbe &&
        !privateModelsProbe && !magicDrawProbe && !spinCrossDrawProbe) return -1;
    if (static_cast<int>(bodyProbe) + static_cast<int>(circleProbe) +
        static_cast<int>(spinProbe) + static_cast<int>(wrathProbe) +
        static_cast<int>(privateModelsProbe) + static_cast<int>(magicDrawProbe) +
        static_cast<int>(spinCrossDrawProbe) != 1)
        return 2;
    if (wglGetCurrentContext()) return 2;
    // Hidden test surface only. Native bitmap decoder/uploader remains unchanged.
    // Same pixel-format contract as CreateOpenglWindow, without gameplay globals,
    // foreground changes, failure dialogs or network initialization.
    struct Context
    {
        HINSTANCE instance = GetModuleHandleW(NULL);
        HWND window = NULL;
        HDC dc = NULL;
        HGLRC rc = NULL;
        ATOM atom = 0;
        ~Context()
        {
            if (rc) { wglMakeCurrent(NULL, NULL); wglDeleteContext(rc); }
            if (dc) ReleaseDC(window, dc);
            if (window) DestroyWindow(window);
            if (atom) UnregisterClassW(L"RISE_GL_BodyTextureProbe", instance);
        }
    } context;
    WNDCLASSW wc = {};
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = context.instance;
    wc.lpszClassName = L"RISE_GL_BodyTextureProbe";
    context.atom = RegisterClassW(&wc);
    if (!context.atom) return 2;
    context.window = CreateWindowExW(0, wc.lpszClassName, L"", WS_POPUP,
        0, 0, 256, 256, NULL, NULL, context.instance, NULL);
    if (!context.window || !(context.dc = GetDC(context.window))) return 2;
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd); pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; pfd.cColorBits = 16; pfd.cDepthBits = 16;
    const int format = ChoosePixelFormat(context.dc, &pfd);
    if (!format || !SetPixelFormat(context.dc, format, &pfd)) return 2;
    context.rc = wglCreateContext(context.dc);
    if (!context.rc || !wglMakeCurrent(context.dc, context.rc)) return 2;
    char driverLine[512] = {};
    sprintf_s(driverLine, "event=body-texture-driver vendor=%.100s renderer=%.150s version=%.100s",
        reinterpret_cast<const char*>(glGetString(GL_VENDOR)),
        reinterpret_cast<const char*>(glGetString(GL_RENDERER)),
        reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    AppendQALog(driverLine);
    bool shaderProbe = false;
#ifdef jdk_shader_local330
    struct ShaderLifetime
    {
        bool initialized = false;
        ~ShaderLifetime() { if (initialized) OGL330::Release(); }
    } shaderLifetime; // released before the hidden WGL context
    char shaderEnabled[4] = {};
    shaderProbe = wrathProbe || privateModelsProbe || magicDrawProbe ||
        spinCrossDrawProbe ||
        (GetEnvironmentVariableA("RISE_GL_BODY_GPU_QA",
        shaderEnabled, sizeof(shaderEnabled)) == 1 && shaderEnabled[0] == '1');
    if (shaderProbe)
    {
        if (glewInit() != GLEW_OK) return 2;
        OGL330::Init();
        shaderLifetime.initialized = true;
        AppendQALog(OGL330::IsShader() ? "event=body-shader enabled=1" : "event=body-shader enabled=0");
        if (!OGL330::IsShader()) return 2;
    }
    std::vector<GLuint> bodyVaos, bodyBuffers;
#endif
    if (wrathProbe)
    {
        if (Models) return 2; // never substitute the active gameplay table
        const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
        const size_t countBefore = Bitmaps.GetNumberOfTexture();
        bool valid = true;
        {
        struct ModelTable
        {
            ModelTable() { Models = new BMD[RuntimeModelLimit(MAX_MODELS)]; }
            ~ModelTable() { delete[] Models; Models = NULL; }
        } modelTable;
        struct Row { int type, bones, width, height, components; const char* material; };
        const Row rows[] = {
            {kWrathAuraModel, 1, 256, 128, 3, "alpha_line.jpg"},
            {kWrathTravelModel, 2, 64, 64, 4, "dust01.tga"}
        };
        for (const Row& row : rows)
        {
            const bool opened = EnsureModel(row.type);
            BMD& model = Models[row.type];
            const bool material = opened && model.NumMeshs == 1 &&
                model.NumBones == row.bones && model.NumActions == 1 &&
                _stricmp(model.Textures[0].FileName, row.material) == 0;
            BITMAP_t* bitmap = material ?
                Bitmaps.FindTexture(model.IndexTexture[0]) : NULL;
            GLint gpuWidth = 0, gpuHeight = 0;
            bool gpuMatches = false;
            if (bitmap && bitmap->Buffer && bitmap->Components == row.components)
            {
                glBindTexture(GL_TEXTURE_2D, bitmap->TextureNumber);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,
                    &gpuWidth);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT,
                    &gpuHeight);
                if (gpuWidth == row.width && gpuHeight == row.height)
                {
                    std::vector<unsigned char> pixels(static_cast<size_t>(gpuWidth) *
                        gpuHeight * row.components);
                    glPixelStorei(GL_PACK_ALIGNMENT, 1);
                    glGetTexImage(GL_TEXTURE_2D, 0,
                        row.components == 4 ? GL_RGBA : GL_RGB,
                        GL_UNSIGNED_BYTE, pixels.data());
                    gpuMatches = memcmp(pixels.data(), bitmap->Buffer,
                        pixels.size()) == 0;
                }
            }
            const GLenum error = glGetError();
            bool shaderUploaded = false;
#ifdef jdk_shader_local330
            if (model.NewMeshes && !model.NewMeshes->empty())
            {
                const VAOMesh& mesh = (*model.NewMeshes)[0];
                shaderUploaded = mesh.VAO != 0 && mesh.VBO != 0 &&
                    mesh.IBO != 0 && mesh.IndexCount > 0;
            }
#endif
            char line[320] = {};
            sprintf_s(line,
                "event=wrath-native-model-material type=%d expected=%s opened=%d material=%d meshes=%d bones=%d actions=%d bitmap=%d components=%d gpuWidth=%d gpuHeight=%d cpuGpuEqual=%d shaderUploaded=%d glError=%u",
                row.type, row.material, opened ? 1 : 0, material ? 1 : 0,
                model.NumMeshs, model.NumBones, model.NumActions,
                bitmap ? 1 : 0, bitmap ? bitmap->Components : 0,
                gpuWidth, gpuHeight, gpuMatches ? 1 : 0,
                shaderUploaded ? 1 : 0,
                static_cast<unsigned>(error));
            AppendQALog(line);
            valid = valid && material && bitmap && gpuMatches &&
                shaderUploaded && error == GL_NO_ERROR;
        }
        // Native BMD destructors release GPU meshes and their material bitmap
        // references. Scope the table so this happens before bitmap accounting.
        }
        const bool released = bytesBefore == Bitmaps.GetUsedTextureMemory() &&
            countBefore == Bitmaps.GetNumberOfTexture() && Models == NULL;
        char line[160] = {};
        sprintf_s(line, "event=wrath-native-model-release stable=%d",
            released ? 1 : 0);
        AppendQALog(line);
        return valid && released ? 0 : 1;
    }
    if (privateModelsProbe)
    {
        if (Models) return 2;
        const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
        const size_t countBefore = Bitmaps.GetNumberOfTexture();
        bool valid = true;
        int openedCount = 0;
        int uploadedCount = 0;
        {
            struct ModelTable
            {
                ModelTable() { Models = new BMD[RuntimeModelLimit(MAX_MODELS)]; }
                ~ModelTable() { delete[] Models; Models = NULL; }
            } modelTable;
            // Registered visible tail only; synthetic/controller slots and
            // production model table are never touched by this offline probe.
            for (int type = kFirstModel; type <= kLastVisibleModel; ++type)
            {
                const bool opened = EnsureModel(type);
                BMD& model = Models[type];
                bool texturesReady = opened;
                bool shaderUploaded = opened && model.NumMeshs > 0;
                int geometryMeshes = 0;
                for (int meshIndex = 0; opened && meshIndex < model.NumMeshs;
                    ++meshIndex)
                {
                    const BITMAP_t* bitmap =
                        Bitmaps.FindTexture(model.IndexTexture[meshIndex]);
                    texturesReady = texturesReady && bitmap && bitmap->TextureNumber &&
                        glIsTexture(bitmap->TextureNumber) == GL_TRUE;
                    const Mesh_t& source = model.Meshs[meshIndex];
                    if (source.NumVertices > 0 && source.NumTriangles > 0)
                    {
                        ++geometryMeshes;
#ifdef jdk_shader_local330
                        const bool uploaded = model.NewMeshes &&
                            model.NewMeshes->size() == static_cast<size_t>(model.NumMeshs) &&
                            (*model.NewMeshes)[meshIndex].VAO != 0 &&
                            (*model.NewMeshes)[meshIndex].VBO != 0 &&
                            (*model.NewMeshes)[meshIndex].IBO != 0 &&
                            (*model.NewMeshes)[meshIndex].IndexCount > 0;
                        shaderUploaded = shaderUploaded && uploaded;
#else
                        shaderUploaded = false;
#endif
                    }
                }
                shaderUploaded = shaderUploaded && geometryMeshes > 0;
                const GLenum error = glGetError();
                char line[224] = {};
                sprintf_s(line,
                    "event=private-model-gpu type=%d opened=%d meshes=%d geometry=%d texturesReady=%d shaderUploaded=%d glError=%u",
                    type, opened ? 1 : 0, model.NumMeshs, geometryMeshes,
                    texturesReady ? 1 : 0, shaderUploaded ? 1 : 0,
                    static_cast<unsigned>(error));
                AppendQALog(line);
                openedCount += opened ? 1 : 0;
                uploadedCount += shaderUploaded ? 1 : 0;
                valid = valid && opened && texturesReady && shaderUploaded &&
                    error == GL_NO_ERROR;
            }
        }
        const bool released = bytesBefore == Bitmaps.GetUsedTextureMemory() &&
            countBefore == Bitmaps.GetNumberOfTexture() && Models == NULL;
        char line[224] = {};
        sprintf_s(line,
            "event=private-model-gpu-summary expected=%d opened=%d uploaded=%d releaseStable=%d",
            kLastVisibleModel - kFirstModel + 1, openedCount, uploadedCount,
            released ? 1 : 0);
        AppendQALog(line);
        return valid && released ? 0 : 1;
    }
    if (magicDrawProbe || spinCrossDrawProbe)
    {
        if (Models || !GMMeshShader) return 2;
        const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
        const size_t countBefore = Bitmaps.GetNumberOfTexture();
        bool valid = true;
        {
            struct ModelTable
            {
                ModelTable() { Models = new BMD[RuntimeModelLimit(MAX_MODELS)]; }
                ~ModelTable() { delete[] Models; Models = NULL; }
            } modelTable;
            glViewport(0, 0, 256, 256);
            glMatrixMode(GL_PROJECTION); glLoadIdentity();
            glOrtho(-250.0, 250.0, -50.0, 450.0, -1000.0, 1000.0);
            glMatrixMode(GL_MODELVIEW); glLoadIdentity();
            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            const int types[] = {spinCrossDrawProbe ? kSpinCrossModel :
                kMagicPin01Model, spinCrossDrawProbe ? -1 : kMagicPinRootModel};
            for (int type : types)
            {
                if (type < 0) continue;
                const bool opened = EnsureModel(type);
                BMD& model = Models[type];
                float pose[MAX_BONES][3][4] = {};
                vec3_t angle = {}, headAngle = {};
                bool meshReady = false;
                if (opened && model.NewMeshes && !model.NewMeshes->empty())
                {
                    const VAOMesh& mesh = (*model.NewMeshes)[0];
                    meshReady = mesh.VAO != 0 && mesh.IndexCount > 0;
                    model.CurrentAction = 0;
                    model.BodyScale = model.RequestScale =
                        spinCrossDrawProbe ? 8.5f : 0.7f;
                    Vector(0.0f, 0.0f, 0.0f, model.BodyOrigin);
                    if (spinCrossDrawProbe)
                        Vector(0.2f, 0.6f, 1.0f, model.BodyLight);
                    else
                        Vector(0.48f, 0.73f, 1.0f, model.BodyLight);
                    model.LightEnable = false;
                }
                const int rotations[] = {0, 90, 180, 270};
                for (int rotation : rotations)
                {
                    unsigned samples = 0;
                    if (meshReady)
                    {
                        angle[2] = spinCrossDrawProbe ? 120.0f :
                            static_cast<float>(rotation);
                        model.Animation(pose, 0.0f, 0.0f, 0, angle,
                            headAngle, false, false);
                        if (spinCrossDrawProbe)
                        {
                            glMatrixMode(GL_MODELVIEW); glLoadIdentity();
                            glRotatef(static_cast<float>(rotation), 0.0f,
                                0.0f, 1.0f); // fixture camera sweep, not caster angle
                            glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                        }
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        GLuint query = 0;
                        glGenQueries(1, &query);
                        if (query)
                        {
                            glBeginQuery(GL_SAMPLES_PASSED, query);
                            GMMeshShader->BeginScope();
                            GMMeshShader->AddBoneTransform(&model, pose, false);
                            // Exact mesh flags/peak alpha and source model
                            // scale; only owner position/camera are fixtures.
                            model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,
                                1.0f, 0, 1.0f, 0.0f, 0.0f, -1);
                            GMMeshShader->EndScope();
                            glEndQuery(GL_SAMPLES_PASSED);
                            glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samples);
                            glDeleteQueries(1, &query);
                        }
                        GMMeshShader->AddBoneTransform(NULL, NULL, false);
                    }
                    const GLenum error = glGetError();
                    unsigned litPixels = 0;
                    if (spinCrossDrawProbe && meshReady && error == GL_NO_ERROR)
                    {
                        unsigned char pixels[256 * 256 * 4] = {};
                        glReadPixels(0, 0, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE,
                            pixels);
                        for (int pixel = 0; pixel < 256 * 256; ++pixel)
                            if (pixels[pixel * 4] || pixels[pixel * 4 + 1] ||
                                pixels[pixel * 4 + 2]) ++litPixels;
                    }
                    char line[320] = {};
                    if (spinCrossDrawProbe)
                        sprintf_s(line,
                            "event=spin-cross-native-fixture-draw type=%d cameraSweep=%d sourceAngle=120 sourceScale=8.5 opened=%d meshReady=%d samples=%u litPixels=%u glError=%u fixturePose=1 gameplayCamera=0",
                            type, rotation, opened ? 1 : 0, meshReady ? 1 : 0,
                            samples, litPixels, static_cast<unsigned>(error));
                    else
                        sprintf_s(line,
                            "event=magic-native-fixture-draw type=%d rotation=%d sourceScale=1 opened=%d meshReady=%d samples=%u glError=%u fixturePose=1 gameplayCamera=0",
                            type, rotation, opened ? 1 : 0, meshReady ? 1 : 0,
                            samples, static_cast<unsigned>(error));
                    AppendQALog(line);
                    valid = valid && opened && meshReady && samples > 0 &&
                        error == GL_NO_ERROR &&
                        (!spinCrossDrawProbe || litPixels > 0);
                }
            }
        }
        const bool released = bytesBefore == Bitmaps.GetUsedTextureMemory() &&
            countBefore == Bitmaps.GetNumberOfTexture() && Models == NULL;
        AppendQALog(spinCrossDrawProbe ?
            (released ? "event=spin-cross-native-fixture-release stable=1" :
                "event=spin-cross-native-fixture-release stable=0") :
            (released ? "event=magic-native-fixture-release stable=1" :
                "event=magic-native-fixture-release stable=0"));
        return valid && released ? 0 : 1;
    }
    if (circleProbe)
    {
        const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
        const size_t countBefore = Bitmaps.GetNumberOfTexture();
        const bool loaded = EnsureCirclePersistentBitmap();
        BITMAP_t* bitmap = Bitmaps.FindTexture(kCircleUpperArmMonoBitmap);
        GLint width = 0, height = 0, minFilter = 0, magFilter = 0;
        GLint wrapS = 0, wrapT = 0;
        bool pixelsMatch = false;
        bool exactPath = false;
        if (bitmap)
        {
            exactPath = _stricmp(bitmap->FileName,
                "Data\\RISE\\GrowLancer\\CircleShield\\firehik_mono01.jpg") == 0;
            glBindTexture(GL_TEXTURE_2D, bitmap->TextureNumber);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
            if (width == 64 && height == 64 && bitmap->Components == 3 &&
                bitmap->Buffer)
            {
                std::vector<unsigned char> pixels(64u * 64u * 3u);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    pixels.data());
                pixelsMatch = memcmp(pixels.data(), bitmap->Buffer,
                    pixels.size()) == 0;
            }
        }
        const GLenum error = glGetError();
        const bool valid = loaded && bitmap && exactPath && pixelsMatch &&
            width == 64 && height == 64 && minFilter == GL_LINEAR &&
            magFilter == GL_LINEAR && wrapS == GL_CLAMP_TO_EDGE &&
            wrapT == GL_CLAMP_TO_EDGE && error == GL_NO_ERROR;
        char line[384] = {};
        sprintf_s(line, "event=circle-native-bitmap loaded=%d exactPath=%d cpuGpuEqual=%d width=%d height=%d components=%d min=%d mag=%d wrapS=%d wrapT=%d glError=%u valid=%d",
            loaded ? 1 : 0, exactPath ? 1 : 0, pixelsMatch ? 1 : 0,
            width, height, bitmap ? bitmap->Components : 0, minFilter,
            magFilter, wrapS, wrapT, static_cast<unsigned>(error),
            valid ? 1 : 0);
        AppendQALog(line);
        if (bitmap) Bitmaps.UnLoadImageFile(kCircleUpperArmMonoBitmap, false);
        const bool released = bytesBefore == Bitmaps.GetUsedTextureMemory() &&
            countBefore == Bitmaps.GetNumberOfTexture();
        sprintf_s(line, "event=circle-native-bitmap-release stable=%d",
            released ? 1 : 0);
        AppendQALog(line);
        return valid && released ? 0 : 1;
    }
    if (spinProbe)
    {
        const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
        const size_t countBefore = Bitmaps.GetNumberOfTexture();
        const bool stockLoaded = Bitmaps.LoadImageFile(BITMAP_BLUR + 1,
            "Data\\Effect\\motion_blur.jpg", GL_NEAREST, GL_CLAMP_TO_EDGE);
        BITMAP_t* stock = Bitmaps.FindTexture(BITMAP_BLUR + 1);
        GLint stockWrapBefore = 0, stockWrapAfter = 0;
        GLint stockFilter = 0, privateWrap = 0, privateFilter = 0;
        GLint width = 0, height = 0;
        if (stock)
        {
            glBindTexture(GL_TEXTURE_2D, stock->TextureNumber);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &stockWrapBefore);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &stockFilter);
        }
        const bool privateLoaded = EnsureSpinMotionBlurBitmap();
        BITMAP_t* privateBitmap = Bitmaps.FindTexture(kSpinMotionBlurBitmap);
        bool samePixels = false, gpuMatches = false, exactPath = false;
        if (privateBitmap)
        {
            exactPath = _stricmp(privateBitmap->FileName,
                "Data\\RISE\\GrowLancer\\SpinStep\\motion_blur.jpg") == 0;
            glBindTexture(GL_TEXTURE_2D, privateBitmap->TextureNumber);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &privateWrap);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &privateFilter);
            if (stock && stock->Buffer && privateBitmap->Buffer &&
                width > 0 && height > 0 && width == static_cast<int>(stock->Width) &&
                height == static_cast<int>(stock->Height) &&
                privateBitmap->Components == 3 && stock->Components == 3)
            {
                const size_t bytes = static_cast<size_t>(width) * height * 3u;
                samePixels = memcmp(stock->Buffer, privateBitmap->Buffer, bytes) == 0;
                std::vector<unsigned char> gpu(bytes);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    gpu.data());
                gpuMatches = memcmp(gpu.data(), privateBitmap->Buffer, bytes) == 0;
            }
        }
        if (stock)
        {
            glBindTexture(GL_TEXTURE_2D, stock->TextureNumber);
            glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &stockWrapAfter);
        }
        const GLenum error = glGetError();
        const bool valid = stockLoaded && privateLoaded && stock && privateBitmap &&
            exactPath && samePixels && gpuMatches && width > 0 && height > 0 &&
            stock->TextureNumber != privateBitmap->TextureNumber &&
            stockWrapBefore == GL_CLAMP_TO_EDGE && stockWrapAfter == GL_CLAMP_TO_EDGE &&
            stockFilter == GL_NEAREST && privateWrap == GL_CLAMP &&
            privateFilter == GL_NEAREST && error == GL_NO_ERROR;
        char line[384] = {};
        sprintf_s(line, "event=spin-native-bitmap stockLoaded=%d privateLoaded=%d exactPath=%d samePixels=%d gpuMatches=%d width=%d height=%d stockWrapBefore=%d stockWrapAfter=%d stockFilter=%d privateWrap=%d privateFilter=%d glError=%u valid=%d",
            stockLoaded ? 1 : 0, privateLoaded ? 1 : 0,
            exactPath ? 1 : 0, samePixels ? 1 : 0, gpuMatches ? 1 : 0,
            width, height, stockWrapBefore, stockWrapAfter, stockFilter,
            privateWrap, privateFilter, static_cast<unsigned>(error), valid ? 1 : 0);
        AppendQALog(line);
        if (privateBitmap) Bitmaps.UnLoadImageFile(kSpinMotionBlurBitmap, false);
        if (stock) Bitmaps.UnLoadImageFile(BITMAP_BLUR + 1, false);
        const bool released = bytesBefore == Bitmaps.GetUsedTextureMemory() &&
            countBefore == Bitmaps.GetNumberOfTexture();
        sprintf_s(line, "event=spin-native-bitmap-release stable=%d",
            released ? 1 : 0);
        AppendQALog(line);
        return valid && released ? 0 : 1;
    }
    const char* files[] = {"hair_Class108.tga", "skin_Class108.jpg", "Skin_Class308.jpg"};
    const DWORD beforeMemory = Bitmaps.GetUsedTextureMemory();
    const size_t beforeCount = Bitmaps.GetNumberOfTexture();
    int result = 0;
    for (unsigned i = 0; i < 3; ++i)
    {
        const std::string path = std::string("D:\\RISE-CrossPlatform\\Source\\_PC_GrowLancer\\ExMain_RISE_PC\\Tests\\GrowLancerBuild\\BodyCandidates\\Data\\RISE\\GrowLancer\\ClassBody\\") + files[i];
        const GLuint id = Bitmaps.LoadImageFile(path, GL_LINEAR, GL_REPEAT);
        BITMAP_t* bitmap = id == BITMAP_UNKNOWN ? NULL : Bitmaps.FindTexture(id);
        GLint width = 0, height = 0;
        bool pixelsMatch = false;
        unsigned alphaMin = 255, alphaMax = 0;
        if (bitmap)
        {
            glBindTexture(GL_TEXTURE_2D, bitmap->TextureNumber);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
            const int expectedSize = i == 0 ? 64 : 256;
            const int components = i == 0 ? 4 : 3;
            if (width == expectedSize && height == expectedSize && bitmap->Buffer
                && bitmap->Components == components)
            {
                std::vector<unsigned char> pixels(static_cast<size_t>(width) * height * components);
                glPixelStorei(GL_PACK_ALIGNMENT, 1);
                glGetTexImage(GL_TEXTURE_2D, 0, components == 4 ? GL_RGBA : GL_RGB,
                    GL_UNSIGNED_BYTE, pixels.data());
                pixelsMatch = memcmp(pixels.data(), bitmap->Buffer, pixels.size()) == 0;
                if (components == 4)
                    for (size_t p = 3; p < pixels.size(); p += 4)
                    {
                        if (pixels[p] < alphaMin) alphaMin = pixels[p];
                        if (pixels[p] > alphaMax) alphaMax = pixels[p];
                    }
            }
        }
        const GLenum error = glGetError();
        const int expected = i == 0 ? 64 : 256;
        if (!bitmap || width != expected || height != expected || error != GL_NO_ERROR
            || !pixelsMatch || (i == 0 && (alphaMin != 0 || alphaMax != 255))) result = 1;
        char line[256] = {};
        sprintf_s(line, "event=body-texture-upload file=%s loaded=%d gpuWidth=%d gpuHeight=%d glError=%u",
            files[i], bitmap ? 1 : 0, width, height, static_cast<unsigned>(error));
        AppendQALog(line);
        sprintf_s(line, "event=body-texture-readback file=%s cpuGpuEqual=%d alphaMin=%u alphaMax=%u hasAlpha=%d",
            files[i], pixelsMatch ? 1 : 0, alphaMin, alphaMax, i == 0 ? 1 : 0);
        AppendQALog(line);
        if (id != BITMAP_UNKNOWN) Bitmaps.UnLoadImageFile(id, false);
    }
    const GLuint missing = Bitmaps.LoadImageFile(
        "D:\\RISE-CrossPlatform\\Source\\_PC_GrowLancer\\ExMain_RISE_PC\\Tests\\GrowLancerBuild\\BodyCandidates\\Data\\RISE\\GrowLancer\\ClassBody\\__missing_body_probe__.jpg",
        GL_LINEAR, GL_REPEAT);
    if (missing != BITMAP_UNKNOWN) { Bitmaps.UnLoadImageFile(missing, false); result = 1; }
    AppendQALog(missing == BITMAP_UNKNOWN ? "event=body-texture-missing rejected=1" : "event=body-texture-missing rejected=0");
    char adapterEnabled[4] = {};
    if (GetEnvironmentVariableA("RISE_GL_BODY_ADAPTER_QA", adapterEnabled, sizeof(adapterEnabled)) == 1
        && adapterEnabled[0] == '1')
    {
        if (Models) return 2; // offline test must not replace a game model table
        struct ModelTable
        {
            ModelTable() { Models = new BMD[RuntimeModelLimit(MAX_MODELS)]; }
            ~ModelTable() { delete[] Models; Models = NULL; }
        } modelTable;
        bool valid = !EnsureClassBodyModel(0, 0, GL_LINEAR, GL_REPEAT)
            && !EnsureClassBodyModel(7, 5, GL_LINEAR, GL_REPEAT);
        char conflictEnabled[4] = {};
        if (GetEnvironmentVariableA("RISE_GL_BODY_SAMPLER_CONFLICT_QA", conflictEnabled, sizeof(conflictEnabled)) == 1
            && conflictEnabled[0] == '1')
        {
            const GLuint preloaded = Bitmaps.LoadImageFile(
                "Data\\RISE\\GrowLancer\\ClassBody\\skin_Class108.jpg", GL_NEAREST, GL_REPEAT);
            BITMAP_t* preloadedBitmap = preloaded == BITMAP_UNKNOWN ? NULL : Bitmaps.FindTexture(preloaded);
            const BYTE refBefore = preloadedBitmap ? preloadedBitmap->Ref : 0;
            const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
            const size_t countBefore = Bitmaps.GetNumberOfTexture();
            const bool rejected = preloaded != BITMAP_UNKNOWN
                && !EnsureClassBodyModel(7, 1, GL_LINEAR, GL_REPEAT);
            BMD& conflictModel = Models[ClassBodyModelId(1, MAX_MODELS)];
            const bool reset = conflictModel.NumMeshs == 0 && !conflictModel.m_bCompletedAlloc;
            BITMAP_t* afterBitmap = preloaded == BITMAP_UNKNOWN ? NULL : Bitmaps.FindTexture(preloaded);
            const bool refPreserved = afterBitmap && afterBitmap->Ref == refBefore;
            const bool countersStable = bytesBefore == Bitmaps.GetUsedTextureMemory()
                && countBefore == Bitmaps.GetNumberOfTexture();
            if (preloaded != BITMAP_UNKNOWN) Bitmaps.UnLoadImageFile(preloaded, false);
            const bool retry = preloaded != BITMAP_UNKNOWN
                && EnsureClassBodyModel(7, 1, GL_LINEAR, GL_REPEAT);
            char line[256] = {};
            sprintf_s(line, "event=body-sampler-conflict preloaded=%d rejected=%d reset=%d refPreserved=%d countersStable=%d retry=%d",
                preloaded != BITMAP_UNKNOWN, rejected, reset, refPreserved, countersStable, retry);
            AppendQALog(line);
            valid = valid && preloaded != BITMAP_UNKNOWN && rejected && reset && refPreserved
                && countersStable && retry;
        }
        char rollbackEnabled[4] = {};
        if (GetEnvironmentVariableA("RISE_GL_BODY_ROLLBACK_QA", rollbackEnabled, sizeof(rollbackEnabled)) == 1
            && rollbackEnabled[0] == '1')
        {
            char originalDirectory[MAX_PATH] = {};
            const DWORD directoryLength = GetCurrentDirectoryA(MAX_PATH, originalDirectory);
            if (!directoryLength || directoryLength >= MAX_PATH) return 2;
            const bool entered = SetCurrentDirectoryA("D:\\RISE-CrossPlatform\\Source\\_PC_GrowLancer\\ExMain_RISE_PC\\Tests\\GrowLancerBuild\\BodyRollbackFixture") != FALSE;
            if (!entered) return 2;
            const DWORD rollbackBytes = Bitmaps.GetUsedTextureMemory();
            const size_t rollbackCount = Bitmaps.GetNumberOfTexture();
            const GLuint first = Bitmaps.LoadImageFile("Data\\RISE\\GrowLancer\\ClassBody\\hair_Class108.tga", GL_LINEAR, GL_REPEAT);
            const bool firstPresent = first != BITMAP_UNKNOWN;
            if (firstPresent) Bitmaps.UnLoadImageFile(first, false);
            const bool secondAbsent = GetFileAttributesA("Data\\RISE\\GrowLancer\\ClassBody\\skin_Class108.OZJ") == INVALID_FILE_ATTRIBUTES;
            const bool rejected = !EnsureClassBodyModel(7, 0, GL_LINEAR, GL_REPEAT);
            BMD& head = Models[ClassBodyModelId(0, MAX_MODELS)];
            const bool reset = head.NumMeshs == 0 && !head.m_bCompletedAlloc;
            const bool refsRestored = rollbackBytes == Bitmaps.GetUsedTextureMemory()
                && rollbackCount == Bitmaps.GetNumberOfTexture();
            if (!SetCurrentDirectoryA(originalDirectory)) return 2;
            char line[256] = {};
            sprintf_s(line, "event=body-adapter-rollback firstPresent=%d secondAbsent=%d rejected=%d reset=%d refsRestored=%d",
                firstPresent, secondAbsent, rejected, reset, refsRestored);
            AppendQALog(line);
            valid = valid && firstPresent && secondAbsent && rejected && reset && refsRestored;
        }
        // Part zero below retries the SAME native slot after rollback, now using
        // the complete private runtime directory. No fixture asset is renamed.
        for (unsigned part = 0; part < 5; ++part)
        {
#ifdef jdk_shader_local330
            struct CallerBindings
            {
                GLuint vao = 0, buffers[2] = {};
                GLint previousVao = 0, previousArray = 0, previousElement = 0;
                void Init()
                {
                    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &previousVao);
                    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &previousArray);
                    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &previousElement);
                    glGenVertexArrays(1, &vao); glGenBuffers(2, buffers);
                    glBindVertexArray(vao);
                    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
                }
                bool Matches() const
                {
                    GLint v = 0, a = 0, e = 0;
                    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &v);
                    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &a);
                    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &e);
                    return vao && buffers[0] && buffers[1] && v == vao && a == buffers[0] && e == buffers[1];
                }
                ~CallerBindings()
                {
                    if (!vao) return;
                    glBindVertexArray(static_cast<GLuint>(previousVao));
                    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(previousArray));
                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(previousElement));
                    glDeleteBuffers(2, buffers); glDeleteVertexArrays(1, &vao);
                }
            } callerBindings;
            if (shaderProbe) callerBindings.Init();
#endif
            const bool loaded = EnsureClassBodyModel(7, part, GL_LINEAR, GL_REPEAT);
#ifdef jdk_shader_local330
            const bool freshBindings = !shaderProbe || callerBindings.Matches();
            std::vector<GLuint> cachedHandles;
            BMD& bodyModel = Models[ClassBodyModelId(part, MAX_MODELS)];
            if (shaderProbe && loaded && bodyModel.NewMeshes)
            {
                bool sizesMatch = true;
                GLint oldArray = 0;
                glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldArray);
                for (size_t mesh = 0; mesh < bodyModel.NewMeshes->size(); ++mesh)
                {
                    const VAOMesh& gpu = (*bodyModel.NewMeshes)[mesh];
                    bodyVaos.push_back(gpu.VAO);
                    bodyBuffers.push_back(gpu.VBO); bodyBuffers.push_back(gpu.IBO);
                    cachedHandles.push_back(gpu.VAO); cachedHandles.push_back(gpu.VBO); cachedHandles.push_back(gpu.IBO);
                    GLint vertexBytes = 0, indexBytes = 0;
                    glBindBuffer(GL_ARRAY_BUFFER, gpu.VBO);
                    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vertexBytes);
                    glBindBuffer(GL_ARRAY_BUFFER, gpu.IBO);
                    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &indexBytes);
                    sizesMatch = sizesMatch && vertexBytes == gpu.VertexCount * sizeof(VertexBMD)
                        && indexBytes == gpu.IndexCount * sizeof(GLuint);
                }
                glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(oldArray));
                valid = valid && sizesMatch;
                AppendQALog(sizesMatch ? "event=body-gpu sizesMatch=1" : "event=body-gpu sizesMatch=0");
            }
#endif
            const DWORD cachedBytes = Bitmaps.GetUsedTextureMemory();
            const size_t cachedCount = Bitmaps.GetNumberOfTexture();
            const bool cached = EnsureClassBodyModel(7, part, GL_LINEAR, GL_REPEAT);
            GLint samplerBindingBefore = 0, samplerBindingAfter = 0;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &samplerBindingBefore);
            const bool samplerRejected = !EnsureClassBodyModel(7, part, GL_NEAREST, GL_REPEAT)
                && !EnsureClassBodyModel(7, part, GL_LINEAR, GL_CLAMP_TO_EDGE);
            const bool samplerRetry = EnsureClassBodyModel(7, part, GL_LINEAR, GL_REPEAT);
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &samplerBindingAfter);
            const bool samplerStable = samplerRejected && samplerRetry && samplerBindingBefore == samplerBindingAfter;
            valid = valid && samplerStable;
            AppendQALog(samplerStable ? "event=body-sampler rejectedMismatchAndRetry=1" : "event=body-sampler rejectedMismatchAndRetry=0");
#ifdef jdk_shader_local330
            if (shaderProbe)
            {
                const bool bindingsRestored = freshBindings && callerBindings.Matches();
                valid = valid && bindingsRestored;
                AppendQALog(bindingsRestored ? "event=body-gpu callerBindingsRestored=1" : "event=body-gpu callerBindingsRestored=0");
                std::vector<GLuint> afterHandles;
                if (bodyModel.NewMeshes)
                    for (size_t mesh = 0; mesh < bodyModel.NewMeshes->size(); ++mesh)
                    {
                        const VAOMesh& gpu = (*bodyModel.NewMeshes)[mesh];
                        afterHandles.push_back(gpu.VAO); afterHandles.push_back(gpu.VBO); afterHandles.push_back(gpu.IBO);
                    }
                const bool stable = !cachedHandles.empty() && cachedHandles == afterHandles;
                valid = valid && stable;
                AppendQALog(stable ? "event=body-gpu cacheHandlesStable=1" : "event=body-gpu cacheHandlesStable=0");
            }
#endif
            const bool cacheStable = cachedBytes == Bitmaps.GetUsedTextureMemory()
                && cachedCount == Bitmaps.GetNumberOfTexture();
            valid = valid && loaded && cached && cacheStable;
            char line[192] = {};
            sprintf_s(line, "event=body-adapter part=%u loaded=%d cached=%d countersStable=%d",
                part, loaded ? 1 : 0, cached ? 1 : 0, cacheStable ? 1 : 0);
            AppendQALog(line);
        }
#ifdef jdk_shader_local330
        char drawEnabled[4] = {};
        if (shaderProbe && GetEnvironmentVariableA("RISE_GL_BODY_DRAW_QA", drawEnabled, sizeof(drawEnabled)) == 1
            && drawEnabled[0] == '1' && valid)
        {
            // Diagnostic single-key body pose, NOT a source S21 action/camera.
            // Use the actual native queue and keep each bone array alive until flush.
            glViewport(0, 0, 256, 256);
            glMatrixMode(GL_PROJECTION); glLoadIdentity();
            glOrtho(-250.0, 250.0, -50.0, 450.0, -1000.0, 1000.0);
            glMatrixMode(GL_MODELVIEW); glLoadIdentity(); glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
            for (unsigned part = 0; part < 5; ++part)
            {
                BMD& body = Models[ClassBodyModelId(part, MAX_MODELS)];
                float pose[MAX_BONES][3][4] = {};
                vec3_t angle = {}, headAngle = {};
                body.CurrentAction = 0; body.BodyScale = 1.0f; body.RequestScale = 1.0f;
                Vector(0.0f, 0.0f, 0.0f, body.BodyOrigin);
                Vector(1.0f, 1.0f, 1.0f, body.BodyLight);
                body.LightEnable = false; body.HideSkin = false;
                body.Animation(pose, 0.0f, 0.0f, 0, angle, headAngle, false, false);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                GLuint query = 0, samples = 0;
                glGenQueries(1, &query); glBeginQuery(GL_SAMPLES_PASSED, query);
                GMMeshShader->BeginScope();
                GMMeshShader->AddBoneTransform(&body, pose, false);
                bool accepted = true;
                for (int mesh = 0; mesh < body.NumMeshs; ++mesh)
                    accepted = GMMeshShader->AddMeshCommand(&body, mesh, RENDER_TEXTURE,
                        1.0f, -1, 1.0f, 0.0f, 0.0f, -1) && accepted;
                GMMeshShader->EndScope();
                glEndQuery(GL_SAMPLES_PASSED);
                glGetQueryObjectuiv(query, GL_QUERY_RESULT, &samples); glDeleteQueries(1, &query);
                const GLenum error = glGetError();
                char line[192] = {};
                sprintf_s(line, "event=body-native-draw part=%u accepted=%d samples=%u glError=%u fixturePose=1",
                    part, accepted, samples, error);
                AppendQALog(line);
                valid = valid && accepted && samples > 0 && error == GL_NO_ERROR;
                char ownerEnabled[4] = {};
                if (GetEnvironmentVariableA("RISE_GL_BODY_OWNER_QA", ownerEnabled, sizeof(ownerEnabled)) == 1
                    && ownerEnabled[0] == '1')
                {
                    OBJECT fixture;
                    fixture.BoneTransform = pose; // borrowed; native Destroy does not free it
                    fixture.Scale = 1.0f; fixture.BlendMesh = -1;
                    fixture.BlendMeshLight = 1.0f;
                    fixture.BlendMeshTexCoordU = fixture.BlendMeshTexCoordV = 0.0f;
                    vec3_t fixtureLight = {1.0f, 1.0f, 1.0f};
                    for (unsigned test = 0; test < 4; ++test)
                    {
                        fixture.Alpha = test == 2 ? 0.01f : 1.0f;
                        const float previousScale = BoneScale;
                        BoneScale = 1.75f; // detect accidental global scale retention
                        const DWORD bytesBefore = Bitmaps.GetUsedTextureMemory();
                        const size_t countBefore = Bitmaps.GetNumberOfTexture();
                        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                        GLuint ownerQuery = 0, ownerSamples = 0;
                        glGenQueries(1, &ownerQuery); glBeginQuery(GL_SAMPLES_PASSED, ownerQuery);
                        const auto submitted = SubmitClassBaseBody(7, part, &fixture,
                            test == 3 ? 50 : MAX_BONES, fixtureLight, test == 1, false, GL_LINEAR, GL_REPEAT);
                        glEndQuery(GL_SAMPLES_PASSED);
                        glGetQueryObjectuiv(ownerQuery, GL_QUERY_RESULT, &ownerSamples);
                        glDeleteQueries(1, &ownerQuery);
                        const bool scaleRestored = BoneScale == 1.75f;
                        BoneScale = previousScale;
                        const bool countersStable = bytesBefore == Bitmaps.GetUsedTextureMemory()
                            && countBefore == Bitmaps.GetNumberOfTexture();
                        const auto expected = test == 0 ? BaseBodySubmitResult::Submitted
                            : test == 3 ? BaseBodySubmitResult::Unavailable : BaseBodySubmitResult::Skipped;
                        const GLenum ownerError = glGetError();
                        const bool passed = submitted == expected && scaleRestored && countersStable
                            && (test == 0 ? ownerSamples > 0 : ownerSamples == 0) && ownerError == GL_NO_ERROR;
                        char ownerLine[256] = {};
                        sprintf_s(ownerLine, "event=body-owner-submit part=%u test=%u result=%d samples=%u scaleRestored=%d countersStable=%d glError=%u pass=%d",
                            part, test, static_cast<int>(submitted), ownerSamples, scaleRestored, countersStable, ownerError, passed);
                        AppendQALog(ownerLine);
                        valid = valid && passed;
                    }
                    fixture.BoneTransform = NULL;
                    GMMeshShader->AddBoneTransform(NULL, NULL, false);
                }
            }
            GMMeshShader->AddBoneTransform(NULL, NULL, false);
        }
#endif
        if (!valid) result = 1;
    } // real BMD destructors release only acquired texture refs before WGL teardown
#ifdef jdk_shader_local330
    if (shaderProbe)
    {
        bool deleted = bodyVaos.size() == 6 && bodyBuffers.size() == 12;
        for (size_t i = 0; i < bodyVaos.size(); ++i) deleted = !glIsVertexArray(bodyVaos[i]) && deleted;
        for (size_t i = 0; i < bodyBuffers.size(); ++i) deleted = !glIsBuffer(bodyBuffers[i]) && deleted;
        AppendQALog(deleted ? "event=body-gpu released=1" : "event=body-gpu released=0");
        if (!deleted) result = 1;
    }
#endif
    const bool restored = beforeMemory == Bitmaps.GetUsedTextureMemory()
        && beforeCount == Bitmaps.GetNumberOfTexture();
    AppendQALog(restored ? "event=body-texture-release baselineRestored=1" : "event=body-texture-release baselineRestored=0");
    return restored ? result : 1;
}

int RunLanceLoadProbeQA()
{
    char enabled[4] = {};
    if (GetEnvironmentVariableA("RISE_GL_LANCE_LOAD_QA", enabled, sizeof(enabled)) != 1
        || enabled[0] != '1') return -1;
    // Geometry-only native Open2 check before UI/network. Candidate files are
    // hash-staged elsewhere; this probe does not install item attributes.
    struct LanceRow
    {
        const char* file;
        int meshes, bones, actions;
        const char* textures[3];
    };
    static const LanceRow rows[] = {
        {"RapideLance.bmd", 1, 8, 1, {"RapideLance.jpg"}},
        {"ConmocionLance.bmd", 1, 3, 1, {"ConmocionLance.jpg"}},
        {"PlumaLance.bmd", 3, 22, 1, {"PlumaLance.jpg", "gra.jpg", "flareBlue.jpg"}},
        {"VisLance.bmd", 2, 15, 1, {"VisLance.jpg", "flame_chrom.jpg"}},
        {"PrickleLance.bmd", 2, 5, 1, {"PrickleLance.jpg", "rgb_mix.jpg"}},
        {"AlacranLance.bmd", 1, 5, 1, {"AlacranLance.jpg"}},
        {"bloodangellance01.bmd", 2, 9, 1,
            {"bloodangellance01.jpg", "bloodangellance02.jpg"}},
    };
    char directory[] = "D:\\RISE-CrossPlatform\\Source\\_PC_GrowLancer\\ExMain_RISE_PC\\Tests\\GrowLancerBuild\\ClassItems\\Data\\RISE\\GrowLancer\\ClassItems\\Models\\";
    int result = 0;
    for (unsigned i = 0; i < sizeof(rows) / sizeof(rows[0]); ++i)
    {
        const LanceRow& row = rows[i];
        {
            BMD candidate;
            candidate.m_iBMDSeqID = 0;
            char filename[32] = {};
            strcpy_s(filename, row.file);
            const bool loaded = candidate.Open2(directory, filename);
            bool valid = loaded && candidate.NumMeshs == row.meshes &&
                candidate.NumBones == row.bones && candidate.NumActions == row.actions &&
                candidate.Meshs && candidate.Textures && candidate.IndexTexture;
            if (loaded && candidate.IndexTexture)
                for (int mesh = 0; mesh < candidate.NumMeshs; ++mesh)
                    candidate.IndexTexture[mesh] = BITMAP_SKIN;
            if (valid)
                for (int mesh = 0; mesh < row.meshes; ++mesh)
                    if (candidate.Meshs[mesh].Texture < 0 ||
                        candidate.Meshs[mesh].Texture >= candidate.NumMeshs ||
                        strcmp(candidate.Textures[mesh].FileName, row.textures[mesh]) != 0)
                        valid = false;
            char line[280] = {};
            sprintf_s(line,
                "event=lance-native-load file=%s loaded=%d valid=%d meshes=%d bones=%d actions=%d texturesLoaded=0",
                row.file, loaded ? 1 : 0, valid ? 1 : 0,
                candidate.NumMeshs, candidate.NumBones, candidate.NumActions);
            AppendQALog(line);
            if (!valid) result = 1;
        }
        char line[128] = {};
        sprintf_s(line, "event=lance-native-release-returned file=%s", row.file);
        AppendQALog(line);
    }
    return result;
}

int RunBodyLoadProbeQA()
{
    int result = -1;
    // QA-only and opt-in. Body candidates are not registered/rendered or given
    // texture ownership; preserve the existing production model registry.
    static bool bodyProbeChecked = false;
    if (!bodyProbeChecked)
    {
        bodyProbeChecked = true;
        char enabled[4] = {};
        if (GetEnvironmentVariableA("RISE_GL_BODY_LOAD_QA", enabled, sizeof(enabled)) == 1
            && enabled[0] == '1')
        {
            result = 0;
            const char* names[] = {"HelmClass08.bmd", "ArmorClass08.bmd", "PantClass08.bmd",
                "GloveClass08.bmd", "BootClass08.bmd", "HelmClass308.bmd", "ArmorClass308.bmd",
                "PantClass308.bmd", "GloveClass308.bmd", "BootClass308.bmd"};
            char directory[] = "D:\\RISE-CrossPlatform\\Source\\_PC_GrowLancer\\ExMain_RISE_PC\\Tests\\GrowLancerBuild\\BodyCandidates\\Data\\RISE\\GrowLancer\\ClassBody\\";
            for (unsigned i = 0; i < sizeof(names) / sizeof(names[0]); ++i)
            {
                char filename[32] = {};
                strcpy_s(filename, names[i]);
                {
                    BMD candidate;
                    candidate.m_iBMDSeqID = 0;
                    const bool loaded = candidate.Open2(directory, filename);
                    // Open2 allocates IndexTexture without initializing it.
                    // Release reads mesh.Texture slots and deletes owned bitmaps.
                    // This geometry-only probe acquired NONE: BITMAP_SKIN is the
                    // native Release no-delete sentinel, not a loaded texture.
                    if (loaded)
                        for (int mesh = 0; mesh < candidate.NumMeshs; ++mesh)
                            candidate.IndexTexture[mesh] = BITMAP_SKIN;
                    char line[256] = {};
                    sprintf_s(line, "event=body-native-load file=%s loaded=%d meshes=%d bones=%d actions=%d texturesLoaded=0",
                        filename, loaded ? 1 : 0, candidate.NumMeshs, candidate.NumBones, candidate.NumActions);
                    AppendQALog(line);
                    if (!loaded || candidate.NumBones != 51 || candidate.NumActions != 1
                        || candidate.NumMeshs != ((i % 5) == 0 ? 2 : 1))
                        result = 1;
                }
                char line[128] = {};
                sprintf_s(line, "event=body-native-release-returned file=%s", filename);
                AppendQALog(line);
            }
        }
    }
    return result;
}

bool HandleRuntimeQAHotKey()
{
	if (SEASON3B::IsPress(VK_F12))
	{
		ToggleWrathBuffFixtureQA();
		return true;
	}
    if (SEASON3B::IsPress(VK_F10))
    {
        ToggleSkillIconQA();
        return true;
    }
    if (SEASON3B::IsPress(VK_F11))
    {
        FlushShiningSamples();
        WritePoolSnapshot("manual");
        FlushControllerSamples();
        return true;
    }
    if (SEASON3B::IsPress(VK_F6))
    {
        gRuntimeQASkill = (gRuntimeQASkill + 1) %
            (sizeof(kRuntimeQASkills) / sizeof(kRuntimeQASkills[0]));
        WriteQALog("select", kRuntimeQASkills[gRuntimeQASkill], -1);
        return true;
    }
    if (SEASON3B::IsPress(VK_F7))
    {
        CastCurrentSkill();
        return true;
    }
    if (SEASON3B::IsPress(VK_F9))
    {
        EmitCurrentContact();
        return true;
    }
    return false;
}

void RecordCirclePersistentEmissionQA(const OBJECT& actor, bool ready,
    int boneCount, int monoCreated)
{
    if (gControllerSkill != 272 || !Hero || !Hero->Object.Live ||
        &actor != &Hero->Object || gCircleEmissionCount >= 64)
        return;
    const float dx = actor.Position[0] - CameraPosition[0];
    const float dy = actor.Position[1] - CameraPosition[1];
    char message[260];
    sprintf_s(message, sizeof(message),
        "GrowLancerQA event=circle-persistent-emission ready=%d bones=%d monoCreated=%d monoExpected=4 distance2D=%.1f fps=%.2f avg=%.2f renderEffects=%d renderSkills=%d world=%d",
        ready ? 1 : 0, boneCount, monoCreated,
        sqrtf(dx * dx + dy * dy), static_cast<double>(FPS),
        static_cast<double>(FPS_AVG),
        g_pOption->GetRenderAllEffects() ? 1 : 0,
        g_pOption->GetRenderSkillEffects() ? 1 : 0,
        gMapManager.WorldActive);
    AppendQALog(message);
    ++gCircleEmissionCount;
}

bool IsCirclePersistentPreviewQA(const OBJECT& actor)
{
    // Pointer is compared only with the current live Hero; it is never
    // dereferenced. Map change, actor change, or expiry disarms the fixture.
    if (!CirclePreviewStillArmed(gCirclePreviewActor,
        Hero ? &Hero->Object : nullptr, Hero && Hero->Object.Live,
        gMapManager.WorldActive, gCirclePreviewWorld,
        GetTickCount(), gCirclePreviewUntil))
    {
        gCirclePreviewActor = nullptr;
        return false;
    }
    // Other characters render in the same traversal. They must neither
    // receive the owner probe nor disarm it before Hero's frame is reached.
    return CirclePreviewAppliesTo(gCirclePreviewActor, &actor);
}

bool IsRuntimeQAIconPanelVisible()
{
	return gRuntimeQAIconPanelVisible;
}

}}

#endif
