#include "../Main5.2_RISE/RISE/GrowLancerCirclePreviewGate.h"

struct Actor {};
constexpr Actor hero{};
constexpr Actor other{};
using namespace rise::growlancer;

static_assert(CirclePreviewStillArmed(&hero, &hero, true, 0, 0, 1000, 4000));
static_assert(CirclePreviewAppliesTo(&hero, &hero));
static_assert(!CirclePreviewAppliesTo(&hero, &other));
// Traversal of another actor does not cancel the owner interval.
static_assert(CirclePreviewStillArmed(&hero, &hero, true, 0, 0, 1200, 4000));
static_assert(!CirclePreviewStillArmed(&hero, &other, true, 0, 0, 1000, 4000));
static_assert(!CirclePreviewStillArmed(&hero, &hero, false, 0, 0, 1000, 4000));
static_assert(!CirclePreviewStillArmed(&hero, &hero, true, 1, 0, 1000, 4000));
static_assert(!CirclePreviewStillArmed(&hero, &hero, true, 0, 0, 4000, 4000));
// A short interval across GetTickCount wrap remains active, then expires.
static_assert(CirclePreviewStillArmed(&hero, &hero, true, 0, 0,
    0xFFFFFF00u, 0x00000AB8u));
static_assert(!CirclePreviewStillArmed(&hero, &hero, true, 0, 0,
    0x00000AB8u, 0x00000AB8u));
