#include "../Main5.2_RISE/RISE/PegasusQaEvidence.h"
#include <cassert>
#include <cstdio>
int main()
{
    rise::pegasus::QaEvidenceOnce once;
    assert(once.Claim(100, 100, 1));
    assert(!once.Claim(100, 100, 1));
    assert(once.Claim(200, 200, 1)); // Both offset0, distinct models.
    assert(once.Claim(100, 100, 2)); // Transparent does not hide a failure.
    assert(once.Claim(100, 100, 4));
    assert(!once.Claim(100, 100, 4));
    rise::pegasus::QaFrameProgress frames;
    frames.Reset(5.977f);
    frames.Observe(0.0f);
    assert(!frames.Progressed()); // Reset alone must not pass.
    frames.Observe(0.1f);
    assert(!frames.Progressed());
    frames.Observe(0.4f);
    assert(frames.Progressed()); // Real forward motion after wrap.
    frames.Reset(5.9f);
    for (int n = 0; n < 100; ++n) frames.Observe(5.9f);
    assert(!frames.Progressed());
    std::puts("PASS: QA model identity isolation and wrap-aware frame progression");
}
