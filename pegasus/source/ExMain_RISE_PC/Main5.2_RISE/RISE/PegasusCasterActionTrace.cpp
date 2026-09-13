#include "stdafx.h"
#include "PegasusCasterActionTrace.h"
#include "PegasusRuntimeTrace.h"
#include "PegasusQaEvidence.h"
#include "../ZzzCharacter.h"
#include "../ZzzObject.h"
#include "../ZzzBMD.h"

namespace rise { namespace pegasus {
#if defined(PEGASUS_ISOLATED_TEST)
namespace {
struct CasterActionSample
{
    OBJECT* owner;
    DWORD started;
    int skill;
    int expectedAction;
    int selectedAction;
    float startFrame;
    float maxFrame;
    QaFrameProgress progress;
    bool reported;
    DWORD nextDetail;
    int lastAction;
};

CasterActionSample g_samples[16] = {};

CasterActionSample& AcquireSample(OBJECT* owner)
{
    CasterActionSample* oldest = &g_samples[0];
    for (int i = 0; i < 16; ++i)
    {
        if (g_samples[i].owner == owner || g_samples[i].owner == NULL)
            return g_samples[i];
        if (g_samples[i].started < oldest->started)
            oldest = &g_samples[i];
    }
    return *oldest;
}
}
#endif

void BeginCasterActionTrace(CHARACTER& caster, int skill, int expectedAction)
{
#if defined(PEGASUS_ISOLATED_TEST)
    OBJECT& owner = caster.Object;
    CasterActionSample& sample = AcquireSample(&owner);
    sample.owner = &owner;
    sample.started = GetTickCount();
    sample.skill = skill;
    sample.expectedAction = expectedAction;
    sample.selectedAction = owner.CurrentAction;
    sample.startFrame = owner.AnimationFrame;
    sample.maxFrame = owner.AnimationFrame;
    sample.progress.Reset(owner.AnimationFrame);
    sample.reported = false;
    sample.nextDetail = 0;
    sample.lastAction = owner.CurrentAction;
    const bool exact = expectedAction < 0 || owner.CurrentAction == expectedAction;
    TraceRuntime("CAST_ACTION_SELECT", skill, owner.CurrentAction,
        exact ? (expectedAction < 0 ? "SELECTED_BRANCH" : "EXACT") : "MISMATCH");
#else
    (void)caster;
    (void)skill;
    (void)expectedAction;
#endif
}

void UpdateCasterActionTrace(CHARACTER& caster)
{
#if defined(PEGASUS_ISOLATED_TEST)
    OBJECT& owner = caster.Object;
    for (int i = 0; i < 16; ++i)
    {
        CasterActionSample& sample = g_samples[i];
        if (sample.owner != &owner || sample.reported)
            continue;
        const DWORD elapsed = GetTickCount() - sample.started;
        if (elapsed >= sample.nextDetail || owner.CurrentAction != sample.lastAction)
        {
            const int action = owner.CurrentAction;
            const BMD& model = Models[owner.Type];
            const bool valid = action >= 0 && action < model.NumActions && model.Actions;
            char detail[256] = {};
            _snprintf_s(detail, sizeof(detail), _TRUNCATE,
                "elapsed=%lu selected=%d actual=%d frame=%.3f prior=%.3f speed=%.3f model=%d class=%d",
                elapsed, sample.selectedAction, action, owner.AnimationFrame,
                owner.PriorAnimationFrame, valid ? model.Actions[action].PlaySpeed : -1.0f,
                owner.Type, static_cast<int>(caster.Class));
            TraceRuntime("CAST_ACTION_TIMELINE", sample.skill, action, detail);
            sample.nextDetail = elapsed + 100;
            sample.lastAction = action;
        }
        if (owner.CurrentAction == sample.selectedAction)
            sample.progress.Observe(owner.AnimationFrame);
        if (owner.CurrentAction == sample.selectedAction &&
            owner.AnimationFrame > sample.maxFrame)
            sample.maxFrame = owner.AnimationFrame;
        if (GetTickCount() - sample.started < 900)
            continue;

        const bool exact = sample.expectedAction < 0 ||
            sample.selectedAction == sample.expectedAction;
        const bool progressed = sample.progress.Progressed();
        const char* result = progressed ?
            (exact ? "PROGRESSED" : "PROGRESSED_WRONG_ACTION") :
            (owner.CurrentAction == sample.selectedAction ?
                "STATIONARY" : "OVERRIDDEN_WITHOUT_PROGRESS");
        TraceRuntime("CAST_ACTION_FRAME", sample.skill,
            static_cast<int>(sample.maxFrame * 1000.0f), result);
        sample.reported = true;
    }
#else
    (void)caster;
#endif
}
}}
