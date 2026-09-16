#include "../Main5.2_RISE/RISE/GrowLancerWrathReceipt.h"
using namespace rise::growlancer;
constexpr bool ReceiptContract()
{
    WrathReceiptCache cache{};
    if (cache.Find(278) || cache.Find(426)) return false;
    if (cache.Receive({false,424,10,12},1000u,true) || cache.Find(424)->received) return false;
    if (!cache.Receive({false,424,0xffffffffu,65535},1999u,false)) return false;
    const WrathReceipt* row = cache.Find(424);
    if (!row->received || row->countdown.Remaining(2000u)!=0xfffffffeu ||
        row->membershipAuxiliary!=65535) return false;
    if (!cache.Receive({false,424,20,7},3000u,true)) return false;
    if (row->lastAuxiliary!=7 || row->membershipAuxiliary!=65535 ||
        row->countdown.Remaining(4000u)!=19) return false;
    if (!cache.Receive({false,425,0,9},4000u,false)) return false;
    // Display expiry never removes membership or even the received record.
    if (!cache.Find(425)->received || cache.Find(425)->countdown.Remaining(999999u)) return false;
    if (cache.Receive({false,278,1,1},1u,false)) return false;
    if (row->lastAuxiliary!=7) return false;
    if (!cache.Receive({true,424,999,99},5000u,true)) return false;
    if (row->received || row->membershipAuxiliary || row->countdown.durationSeconds) return false;
    if (!cache.Find(425)->received) return false;
    if (!cache.Receive({false,424,1,3},6000u,false) || row->membershipAuxiliary!=3) return false;
    cache.Reset();
    return !cache.Find(424)->received && !cache.Find(425)->received;
}
static_assert(ReceiptContract(), "full-width receipt, refresh, removal, isolation and reset");
