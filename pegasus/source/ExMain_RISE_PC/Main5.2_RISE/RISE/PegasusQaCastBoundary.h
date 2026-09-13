#pragma once
#include "../../../Shared/PegasusRuntimeQa15.h"
#include <cstdio>
#include <cstring>

namespace rise { namespace pegasus {
class QaCastBoundary
{
    unsigned token = 0;
    int skill = -1;
    bool consumed = false;
public:
    void Reset() { token = 0; skill = -1; consumed = false; }
    bool Notice(const char* text)
    {
        if (!text || std::strncmp(text, "[QA15 ", 6) != 0) return false;
        for (std::size_t step = 0; step < kRuntimeQaSkillCount; ++step)
        {
            const unsigned count = RuntimeQaCastCount(step);
            for (unsigned cast = 1; cast <= count; ++cast)
            {
                char expected[160];
                std::snprintf(expected, sizeof(expected), "[QA15 %02u/15] CAST %s (%d) %u/%u.",
                    static_cast<unsigned>(step + 1), RuntimeQaSkillName(step),
                    RuntimeQaSkillId(step), cast, count);
                if (std::strcmp(text, expected) != 0) continue;
                const unsigned next = static_cast<unsigned>(step + 1) * 100 + cast;
                if (next <= token) return false;
                token = next;
                skill = RuntimeQaSkillId(step);
                consumed = false;
                return true;
            }
        }
        return false;
    }
    // 0=no matching boundary (legacy fallback),1=new root,2=same-cast echo.
    int Claim(int incomingSkill)
    {
        if (incomingSkill != skill) return 0;
        if (consumed) return 2;
        consumed = true;
        return 1;
    }
};
}}
