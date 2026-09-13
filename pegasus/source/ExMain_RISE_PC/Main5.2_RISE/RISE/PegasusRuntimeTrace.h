#pragma once

#include "PegasusRuntimeCapacity.h"

#if defined(PEGASUS_ISOLATED_TEST)
#include "PegasusQaEvidence.h"
#include <cstdio>
#include <windows.h>
#endif

namespace rise { namespace pegasus {

inline void TraceRuntime(const char* stage, int value1, int value2, const char* result)
{
#if defined(PEGASUS_ISOLATED_TEST)
    SYSTEMTIME now;
    GetLocalTime(&now);

    char line[512] = {};
    const int length = _snprintf_s(line, sizeof(line), _TRUNCATE,
        "[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s value1=%d value2=%d result=%s\r\n",
        now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
        now.wSecond, now.wMilliseconds, stage, value1, value2,
        result != nullptr ? result : "");
    if (length <= 0)
        return;
    const HANDLE file = CreateFileA("PegasusRuntimeQA.log", FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE)
        return;
    DWORD written = 0;
    WriteFile(file, line, static_cast<DWORD>(length), &written, nullptr);
    CloseHandle(file);
#else
    (void)stage;
    (void)value1;
    (void)value2;
    (void)result;
#endif
}

inline void TraceRuntimeModelStateOnce(const char* stage, int modelId,
    int legacyModelLimit, bool passed)
{
#if defined(PEGASUS_ISOLATED_TEST)
    if (modelId < legacyModelLimit ||
        !IsRuntimeModelId(static_cast<unsigned int>(modelId), legacyModelLimit))
        return;
    const int offset = modelId - legacyModelLimit;
    static bool passSeen[kDynamicModelCapacity] = {};
    static bool failSeen[kDynamicModelCapacity] = {};
    bool* seen = passed ? passSeen : failSeen;
    if (!seen[offset])
    {
        seen[offset] = true;
        TraceRuntime(stage, modelId, offset, passed ? "PASS" : "FAIL");
    }
#else
    (void)stage;
    (void)modelId;
    (void)legacyModelLimit;
    (void)passed;
#endif
}

template <typename TObject>
inline void TraceRuntimeModelRangeRenderStateOnce(const char* stage,
    const TObject& object, int modelBegin, int modelEnd, bool rendered)
{
#if defined(PEGASUS_ISOLATED_TEST)
    const int modelId = object.Type;
    if (modelId < modelBegin || modelId > modelEnd)
        return;
    const int offset = modelId - modelBegin;
    if (offset < 0 || offset >= kDynamicModelCapacity)
        return;
    static QaEvidenceOnce evidence;
    const bool transparent = !rendered && object.Alpha < 0.01f;
    const unsigned event = rendered ? 1u : (transparent ? 2u : 4u);
    if (evidence.Claim(modelBegin, modelId, event))
    {
        TraceRuntime(stage, modelId, offset, rendered ? "PASS" :
            (transparent ? "TRANSPARENT_SKIP" : "FAIL"));
    }
#else
    (void)stage;
    (void)object;
    (void)modelBegin;
    (void)modelEnd;
    (void)rendered;
#endif
}

template <typename TObject>
inline void TraceRuntimeModelRenderStateOnce(const char* stage,
    const TObject& object, int legacyModelLimit, bool rendered)
{
    TraceRuntimeModelRangeRenderStateOnce(stage, object, legacyModelLimit,
        legacyModelLimit + kDynamicModelCapacity - 1, rendered);
}

inline void TraceRuntimeModelRangeAnimationOnce(const char* stage, int modelId,
    int modelBegin, int modelEnd, float animationFrame)
{
#if defined(PEGASUS_ISOLATED_TEST)
    if (modelId < modelBegin || modelId > modelEnd ||
        animationFrame < 0.25f)
        return;
    const int offset = modelId - modelBegin;
    if (offset < 0 || offset >= kDynamicModelCapacity)
        return;
    static QaEvidenceOnce evidence;
    if (evidence.Claim(modelBegin, modelId, 1u))
    {
        TraceRuntime(stage, modelId,
            static_cast<int>(animationFrame * 1000.0f), "PROGRESSED");
    }
#else
    (void)stage;
    (void)modelId;
    (void)modelBegin;
    (void)modelEnd;
    (void)animationFrame;
#endif
}

inline void TraceRuntimeModelAnimationOnce(int modelId, int legacyModelLimit,
    float animationFrame)
{
    TraceRuntimeModelRangeAnimationOnce("EFFECT_BMD_FRAME", modelId,
        legacyModelLimit, legacyModelLimit + kDynamicModelCapacity - 1,
        animationFrame);
}

}}
