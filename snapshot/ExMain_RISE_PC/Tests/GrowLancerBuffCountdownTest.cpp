#include "../Main5.2_RISE/RISE/GrowLancerBuffCountdown.h"
using namespace rise::growlancer;
static_assert(BuffClockSeconds(1999u)==1u,"source integer seconds");
static_assert(BuffRemainingSeconds(1u,10u,1999u)==10u,"initial second");
static_assert(BuffRemainingSeconds(1u,10u,2000u)==9u,"quantized second boundary");
static_assert(BuffRemainingSeconds(1u,10u,11000u)==0u,"exact expiry");
static_assert(BuffRemainingSeconds(1u,10u,12000u)==0u,"clamp expired");
static_assert(BuffRemainingSeconds(1u,0u,1000u)==0u,"zero duration");
static_assert(BuffRemainingSeconds(2u,10u,1000u)==0u,"source unsigned underflow");
static_assert(BuffRemainingSeconds(4294967u,30u,0u)==0u,"source clock wrap behavior");
static_assert(BuffRemainingSeconds(2u,10u,2000u)==10u,"fresh received timestamp");
constexpr bool RepeatedReceive()
{
    BuffCountdown countdown{};
    countdown.Receive(1500u,10u);
    if(countdown.Remaining(5000u)!=6u) return false;
    countdown.Receive(5999u,20u);
    if(countdown.Remaining(5999u)!=20u || countdown.Remaining(6000u)!=19u) return false;
    countdown.Receive(7000u,0u);
    return countdown.Remaining(7000u)==0u && countdown.startSeconds==7u;
}
static_assert(RepeatedReceive(),"new receipt replaces countdown rather than accumulating duration");
