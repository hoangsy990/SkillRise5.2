#include "stdafx.h"
#include "GrowLancerRuntimeQA.h"

#ifdef RISE_GROW_LANCER_RUNTIME_QA

#include "GrowLancerEffectRuntime.h"
#include "GrowLancerResources.h"
#include "ZzzCharacter.h"
#include "ZzzInfomation.h"
#include "ZzzEffect.h"
#include "SkillEffectMgr.h"
#include "ZzzOpenglUtil.h"
#include "NewUICommon.h"

extern int SelectedCharacter;

namespace rise { namespace growlancer {
namespace {

int gRuntimeQASkill = 0;
bool gRuntimeQAIconPanelVisible = false;
struct ShiningSample { float frame, factor, life; int stage; unsigned short action; };
ShiningSample gShiningSamples[512];
unsigned int gShiningSampleCount = 0;
unsigned int gShiningDropped = 0;
bool gShiningArmed = false;

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
    }

    int liveJoints = 0;
    int growLancerJoints = 0;
    for (int i = 0; i < MAX_JOINTS; ++i)
    {
        if (!Joints[i].Live)
            continue;
        ++liveJoints;
        if (Hero && Joints[i].Target == &Hero->Object &&
            (Joints[i].Type == BITMAP_PIN_LIGHT || Joints[i].Type == BITMAP_FORCEPILLAR))
            ++growLancerJoints;
    }

    char line[320];
    sprintf_s(line, sizeof(line),
        "GrowLancerQA event=pool reason=%s effects=%d glEffects=%d particles=%d glParticles=%d sharedEnergy=%d sharedCloud=%d joints=%d glJoints=%d fps=%.2f avg=%.2f",
        reason, liveEffects, growLancerEffects, liveParticles,
        growLancerParticles, sharedEnergyParticles, sharedCloudParticles,
        liveJoints, growLancerJoints, FPS, FPS_AVG);
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

void CastCurrentSkill()
{
    if (!Hero || !Hero->Object.Live)
    {
        WriteQALog("cast-rejected-no-caster", kRuntimeQASkills[gRuntimeQASkill], -1);
        return;
    }
    OBJECT& caster = Hero->Object;
    // Flush outside controller updates. Capture one isolated F7 cast at a time.
    FlushShiningSamples();
    if (kRuntimeQASkills[gRuntimeQASkill] == 277)
    {
        gShiningSampleCount = gShiningDropped = 0;
        gShiningArmed = true;
    }
    WriteCastInputSnapshot("before-dispatch", kRuntimeQASkills[gRuntimeQASkill]);
    switch (kRuntimeQASkills[gRuntimeQASkill])
    {
    case 271: CreateSpinStepRoot(caster, static_cast<short>(SelectedCharacter)); break;
    case 272: CreateCircleShieldRoot(caster); break;
    case 273: CreateObsidianRoots(caster); break;
    case 274: CreateMagicPinRoots(caster); break;
    case 275:
        if (!SelectedTarget())
        {
            WriteQALog("cast-rejected-no-target", 275, -1);
            return;
        }
        CreateClashRoot(caster, static_cast<short>(SelectedCharacter));
        break;
    case 276: CreateHarshStrikeRoot(caster); break;
    case 277: CreateShiningPeakRoots(caster); break;
    case 278: CreateWrathRoot(caster); break;
    case 279: CreateBrecheAction(caster); break;
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

bool HandleRuntimeQAHotKey()
{
    if (SEASON3B::IsPress(VK_F10))
    {
        ToggleSkillIconQA();
        return true;
    }
    if (SEASON3B::IsPress(VK_F11))
    {
        FlushShiningSamples();
        WritePoolSnapshot("manual");
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

bool IsRuntimeQAIconPanelVisible()
{
	return gRuntimeQAIconPanelVisible;
}

}}

#endif
