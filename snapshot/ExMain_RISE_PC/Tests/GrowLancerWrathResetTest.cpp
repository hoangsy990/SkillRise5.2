#include "../Main5.2_RISE/RISE/GrowLancerWrathReset.h"
using namespace rise::growlancer;
constexpr bool ResetContract()
{
    WrathPendingReset state;
    int index=77,key=88;
    state.Capture(true,true);
    if (state.Take(index,key) || index!=77 || key!=88) return false;
    if (state.Bind(-1,4) || state.Bind(4,-1) || !state.Bind(4,99)) return false;
    if (state.Bind(5,99) || state.Bind(4,100) || !state.Bind(4,99)) return false;
    state.Capture(true,false);
    state.Capture(false,false); // receipt already cleared by earlier reset
    if (state.Bind(5,100) || state.Bind(4,99)) return false;
    if (state.Take(index,key)!=1 || index!=4 || key!=99) return false;
    if (state.Take(index,key)) return false;
    if (!state.Bind(5,100)) return false;
    state.Capture(false,true);
    state.Capture(true,false);
    if (state.Take(index,key)!=3 || index!=5 || key!=100) return false;
    if (!state.Bind(5,100)) return false; // same slot/key, after consumption
    state.Capture(false,false);
    state.Capture(true,true); // unbound after empty reset
    return state.Take(index,key)==0 && state.Bind(6,101);
}
static_assert(ResetContract(), "pending cleanup cannot be overwritten; repeat reset and drain preserve identity");
