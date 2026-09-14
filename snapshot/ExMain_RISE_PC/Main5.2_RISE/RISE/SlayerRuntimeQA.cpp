#include "stdafx.h"
#include "SlayerRuntimeQA.h"

#ifdef RISE_SLAYER_RUNTIME_QA

#include "Slayer/client/SlayerNativeRuntime.h"
#include "Slayer/client/SlayerSkillResources.h"
#include "Slayer/shared/SlayerSkillContractData.h"
#include "ZzzCharacter.h"
#include "ZzzEffect.h"
#include "ZzzInfomation.h"
#include "NewUICommon.h"

extern int SelectedCharacter;

namespace rise { namespace slayerqa {
namespace {

const int kRuntimeQASkills[] = { 292, 293, 294, 295, 297 };
const int kRuntimeQASkillCount = static_cast<int>(
    sizeof(kRuntimeQASkills) / sizeof(kRuntimeQASkills[0]));
int gRuntimeQASkill = 0;
bool gRuntimeQAIconPanelVisible = false;
bool gCatalogProbeLogged = false;
unsigned gLastEventCount = 0;
char gLastResult[64] = "not-run";
std::uint64_t gVirtualNowMs = 0;
int gAutoSequenceStep = -1;
DWORD gAutoSequenceNextTick = 0;
bool gAutoSequenceComplete = false;

const char* SkillName(int id)
{
    switch (id)
    {
    case slayer::kSwordInertia: return "Sword Inertia";
    case slayer::kBatFlock: return "Bat Flock";
    case slayer::kPierceAttack: return "Pierce Attack";
    case slayer::kDetection: return "Detection";
    case slayer::kDemolish: return "Demolish";
    default: return "unknown";
    }
}

void AppendQALog(const char* message);

void AppendQALog(const char* message)
{
    SYSTEMTIME now;
    GetSystemTime(&now);
    char line[640];
    sprintf_s(line, sizeof(line),
        "%04u-%02u-%02uT%02u:%02u:%02u.%03uZ pid=%lu SlayerQA %s\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds, GetCurrentProcessId(), message);
    OutputDebugStringA(line);
    FILE* file = 0;
    if (fopen_s(&file, "SlayerRuntimeQA.log", "a") == 0 && file)
    {
        fputs(line, file);
        fclose(file);
    }
}

OBJECT* SelectedTarget()
{
	if (SelectedCharacter >= 0 && SelectedCharacter < MAX_CHARACTERS_CLIENT)
	{
		OBJECT* target = &CharactersClient[SelectedCharacter].Object;
		if (target->Live)
			return target;
	}
	// Prefer the nearest live non-player actor so projectile travel/contact is
	// exercised by unattended QA instead of collapsing every cast onto Hero.
	if (Hero && Hero->Object.Live && CharactersClient)
	{
		OBJECT* nearest = 0;
		float nearestDistance = 100000000.f;
		for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
		{
			OBJECT* candidate = &CharactersClient[i].Object;
			if (!candidate->Live || candidate == &Hero->Object)
				continue;
			const float dx = candidate->Position[0] - Hero->Object.Position[0];
			const float dy = candidate->Position[1] - Hero->Object.Position[1];
			const float distance = dx * dx + dy * dy;
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearest = candidate;
			}
		}
		if (nearest)
			return nearest;
	}
	return Hero && Hero->Object.Live ? &Hero->Object : 0;
}

CHARACTER* SelectedTargetCharacter()
{
	OBJECT* object = SelectedTarget();
	if (!object || !CharactersClient)
		return Hero;
	for (int i = 0; i < MAX_CHARACTERS_CLIENT; ++i)
		if (&CharactersClient[i].Object == object)
			return &CharactersClient[i];
	return Hero;
}

int SelectedTargetId()
{
	if (!SelectedTarget())
		return -1;
	if (SelectedCharacter >= 0 && SelectedCharacter < MAX_CHARACTERS_CLIENT)
		return SelectedCharacter;
	return Hero ? Hero->Key : -1;
}

void SetResult(const char* text)
{
    strcpy_s(gLastResult, sizeof(gLastResult), text);
}

void CastCurrentSkill()
{
    const int skillId = kRuntimeQASkills[gRuntimeQASkill];
    CHARACTER* target = (skillId == slayer::kDetection ||
        skillId == slayer::kDemolish) ? Hero :
        SelectedTargetCharacter();
    const bool accepted = Hero && target &&
        slayer::DispatchNativeReceive(Hero, target, skillId);
    char line[192];
    sprintf_s(line, sizeof(line),
        "f7-production-dispatch skill=%d name=%s accepted=%d target=%d",
        skillId, SkillName(skillId), accepted ? 1 : 0,
        target ? target->Key : -1);
    AppendQALog(line);
    SetResult(accepted ? "production-dispatch-accepted" :
        "production-dispatch-rejected");
}

void AdvanceDotTick(unsigned deltaMs)
{
    const int actorId = Hero ? Hero->Key : 1;
    gVirtualNowMs = static_cast<std::uint64_t>(GetTickCount64()) + deltaMs;
    slayer::TickNativeRuntime(actorId, gVirtualNowMs);
    char line[192];
    sprintf_s(line, sizeof(line),
        "production-runtime-tick actor=%d nowMs=%llu deltaMs=%u",
        actorId, static_cast<unsigned long long>(gVirtualNowMs), deltaMs);
    AppendQALog(line);
}

void TogglePanel()
{
    gRuntimeQAIconPanelVisible = !gRuntimeQAIconPanelVisible;
    AppendQALog(gRuntimeQAIconPanelVisible ? "panel=on" : "panel=off");
}

bool IsSlayerAutoQaEnabled()
{
    const DWORD attributes = GetFileAttributesA("SlayerAutoQA.enable");
    return attributes != INVALID_FILE_ATTRIBUTES &&
        (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

void ProbeNativeSkillCatalog()
{
    if (gCatalogProbeLogged || !SkillAttribute)
        return;

    const int skillIds[] = { slayer::kSwordInertia, slayer::kBatFlock,
        slayer::kPierceAttack, slayer::kDetection, slayer::kDemolish };
    const char* names[] = { "Sword Inertia", "Bat Flock", "Pierce Attack",
        "Detection", "Demolish" };
    bool ready = true;
    for (unsigned i = 0; i < sizeof(skillIds) / sizeof(skillIds[0]); ++i)
    {
        ready = ready && _stricmp(SkillAttribute[skillIds[i]].Name, names[i]) == 0;
        ready = ready && SkillAttribute[skillIds[i]].Magic_Icon == 86 + i;
    }
    char line[160];
    sprintf_s(line, sizeof(line), "native-skill-catalog ready=%d rows=292,293,294,295,297 "
        "class-column=fail-closed", ready ? 1 : 0);
    AppendQALog(line);

    int learnedSlots[5] = { -1, -1, -1, -1, -1 };
    if (CharacterAttribute)
    {
        for (int slot = 0; slot < MAX_SKILLS; ++slot)
        {
            for (unsigned i = 0; i < sizeof(skillIds) / sizeof(skillIds[0]); ++i)
            {
                if (CharacterAttribute->Skill[slot] == skillIds[i])
                    learnedSlots[i] = slot;
            }
        }
    }
    sprintf_s(line, sizeof(line),
        "native-learned-skills class=%d slots=292:%d,293:%d,294:%d,295:%d,297:%d",
        Hero ? Hero->Class : -1, learnedSlots[0], learnedSlots[1],
        learnedSlots[2], learnedSlots[3], learnedSlots[4]);
    AppendQALog(line);
    gCatalogProbeLogged = true;
}

}

void AppendRuntimeQALog(const char* message)
{
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
	return SelectedTargetId();
}

bool RuntimeQASelectedTargetLive()
{
    return SelectedTarget() != 0;
}

unsigned RuntimeQALastEventCount()
{
    return gLastEventCount;
}

const char* RuntimeQALastResult()
{
    return gLastResult;
}

bool DispatchSlayerNativeReceive(CHARACTER* source, CHARACTER* target,
    int skillId)
{
    // F7 and packet receive share exactly one production graph implementation.
    return slayer::DispatchNativeReceive(source, target, skillId);
}

bool HandleRuntimeQAHotKey()
{
    ProbeNativeSkillCatalog();
    if (SEASON3B::IsPress(VK_F10))
    {
        TogglePanel();
        return true;
    }
    if (SEASON3B::IsPress(VK_F11))
    {
        AdvanceDotTick(5000);
        return true;
    }
    if (SEASON3B::IsPress(VK_F6))
    {
        gRuntimeQASkill = (gRuntimeQASkill + 1) % kRuntimeQASkillCount;
        char line[128];
        sprintf_s(line, sizeof(line), "select skill=%d name=%s",
            RuntimeQASelectedSkillId(), RuntimeQASelectedSkillName());
        AppendQALog(line);
        return true;
    }
    if (SEASON3B::IsPress(VK_F7))
    {
        CastCurrentSkill();
        return true;
    }
    if (SEASON3B::IsPress(VK_F9))
    {
        // F9 mirrors Grow Lancer's explicit target/contact checkpoint.  For
        // Slayer it advances the DOT clock and records the selected target.
        char line[160];
        sprintf_s(line, sizeof(line), "contact-check skill=%d target=%d live=%d",
            RuntimeQASelectedSkillId(), RuntimeQASelectedTargetIndex(),
            RuntimeQASelectedTargetLive() ? 1 : 0);
        AppendQALog(line);
        AdvanceDotTick(1000);
        return true;
    }
    return false;
}

void RunRuntimeQAAutoSequence()
{
    if (!IsSlayerAutoQaEnabled() || gAutoSequenceComplete ||
        Hero == 0 || !Hero->Object.Live)
        return;

    const DWORD now = GetTickCount();
    if (gAutoSequenceStep < 0)
    {
        gAutoSequenceStep = 0;
        gAutoSequenceNextTick = now + 2000;
        char line[192];
        sprintf_s(line, sizeof(line),
            "auto-sequence armed scene=MAIN actor=%d target=%d targetLive=%d",
            Hero->Key, RuntimeQASelectedTargetIndex(),
            RuntimeQASelectedTargetLive() ? 1 : 0);
        AppendQALog(line);
        return;
    }

    if (static_cast<LONG>(now - gAutoSequenceNextTick) < 0)
        return;

    // Emit the state before each scheduled transition.  A completion record
    // is valid only after the explicit five-step state reaches the count.
    char stateLine[96];
    sprintf_s(stateLine, sizeof(stateLine),
        "auto-sequence state step=%d count=%d", gAutoSequenceStep,
        kRuntimeQASkillCount);
    AppendQALog(stateLine);

    if (gAutoSequenceStep >= 0 && gAutoSequenceStep < kRuntimeQASkillCount)
    {
        gRuntimeQASkill = gAutoSequenceStep;
        gVirtualNowMs += 7000;
        char line[192];
        sprintf_s(line, sizeof(line), "auto-sequence cast step=%d skill=%d name=%s",
            gAutoSequenceStep, RuntimeQASelectedSkillId(),
            RuntimeQASelectedSkillName());
        AppendQALog(line);
        // Exercise the same packet-to-runtime bridge used by ReceiveMagic.
        // The probe is local and is labeled as such; only a later server
        // packet can establish authoritative gameplay acceptance.
        CHARACTER* target = (RuntimeQASelectedSkillId() == slayer::kDetection ||
            RuntimeQASelectedSkillId() == slayer::kDemolish) ?
            Hero : SelectedTargetCharacter();
        const bool nativeAccepted = DispatchSlayerNativeReceive(Hero, target,
            RuntimeQASelectedSkillId());
        SetResult(nativeAccepted ? "native-receive-accepted" :
            "native-receive-rejected");
        sprintf_s(line, sizeof(line),
            "auto-sequence native-receive-probe skill=%d accepted=%d",
            RuntimeQASelectedSkillId(), nativeAccepted ? 1 : 0);
        AppendQALog(line);

        ++gAutoSequenceStep;
        // Let the complete production graph finish before the next skill is
        // dispatched so the five visual signatures do not overlap in QA.
        gAutoSequenceNextTick = now + 7000;
        return;
    }

    if (gAutoSequenceStep == kRuntimeQASkillCount)
    {
        gAutoSequenceComplete = true;
        AppendQALog("auto-sequence complete skills=292,293,294,295,297");
    }
    else
    {
        sprintf_s(stateLine, sizeof(stateLine),
            "auto-sequence invalid-state step=%d count=%d",
            gAutoSequenceStep, kRuntimeQASkillCount);
        AppendQALog(stateLine);
    }
}

bool IsRuntimeQAIconPanelVisible()
{
    return gRuntimeQAIconPanelVisible;
}

} }

#endif
