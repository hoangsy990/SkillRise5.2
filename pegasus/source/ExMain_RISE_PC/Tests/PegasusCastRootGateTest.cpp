#include "../Main5.2_RISE/RISE/PegasusCastRootGate.h"
#include "../Main5.2_RISE/RISE/PegasusQaCastBoundary.h"
#include <cassert>

using namespace rise::pegasus;

static_assert(!IsDuplicateCastRoot(1000, 1000, false),
    "an empty stamp must accept the first root");
static_assert(IsDuplicateCastRoot(1000, 1000, true),
    "same-tick multi-target echoes must share one root");
static_assert(IsDuplicateCastRoot(1249, 1000, true),
    "the duplicate window is inclusive below its boundary");
static_assert(!IsDuplicateCastRoot(1250, 1000, true),
    "a later real cast at the boundary must be accepted");
static_assert(IsDuplicateCastRoot(20, 0xFFFFFFF0u, true),
    "GetTickCount wrap-around must retain unsigned elapsed semantics");

int main()
{
    CastRootStamp stamp;
    stamp.sourceKey = 42;
    stamp.tick = 1000;
    stamp.valid = true;
    assert(!stamp.Complete(43)); // Another caster cannot close this batch.
    assert(IsDuplicateCastRoot(1001, stamp.tick, stamp.valid));
    assert(stamp.Complete(42));
    assert(!IsDuplicateCastRoot(1001, stamp.tick, stamp.valid));
    assert(!stamp.Complete(42));
    QaCastBoundary boundary;
    assert(boundary.Claim(242) == 0);
    assert(boundary.Notice("[QA15 13/15] CAST Dragon Violent (242) 1/10."));
    assert(boundary.Claim(242) == 1);
    for (int n = 0; n < 3; ++n) assert(boundary.Claim(242) == 2);
    // Second cast may arrive in the same millisecond; no clock is consulted.
    assert(boundary.Notice("[QA15 13/15] CAST Dragon Violent (242) 2/10."));
    assert(boundary.Claim(242) == 1);
    assert(boundary.Claim(242) == 2);
    assert(!boundary.Notice("[QA15 13/15] CAST Dragon Violent (242) 2/10."));
    assert(!boundary.Notice("[QA15 13/15] CAST Dragon Violent (242) 1/10."));
    assert(!boundary.Notice("[QA15 13/15] CAST Dragon Violent (242) 11/10."));
    assert(!boundary.Notice("[QA15 13/15] CAST Fake (242) 3/10."));
    assert(boundary.Claim(1501) == 0);
    boundary.Reset();
    assert(boundary.Claim(242) == 0);
    assert(boundary.Notice("[QA15 13/15] CAST Dragon Violent (242) 1/10."));
    assert(boundary.Claim(242) == 1);
    return 0;
}
