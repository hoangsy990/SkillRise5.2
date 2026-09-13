#pragma once
#include <map>
#include <utility>

namespace rise { namespace pegasus {
// Relative offsets are not identities: Sword Blow offset0 and dynamic offset0
// refer to different models. Keep each range/model pair independent.
class QaEvidenceOnce
{
    std::map<std::pair<int, int>, unsigned> seen;
public:
    bool Claim(int rangeBegin, int modelId, unsigned event)
    {
        unsigned& flags = seen[std::make_pair(rangeBegin, modelId)];
        if ((flags & event) != 0) return false;
        flags |= event;
        return true;
    }
};

struct QaFrameProgress
{
    float previous = 0.0f;
    float advanced = 0.0f;
    void Reset(float frame) { previous = frame; advanced = 0.0f; }
    void Observe(float frame)
    {
        // A wrap/reset is not itself progress. Subsequent forward movement is.
        if (frame > previous) advanced += frame - previous;
        previous = frame;
    }
    bool Progressed() const { return advanced >= 0.25f; }
};
}}
