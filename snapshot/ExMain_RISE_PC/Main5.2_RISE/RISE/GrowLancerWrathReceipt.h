#pragma once
#include "GrowLancerBuffCountdown.h"
#include "../../../Shared/GrowLancerBuffStatusCodec.h"

namespace rise { namespace growlancer {
// Connection/character-owned receipt metadata, NOT authoritative membership.
// The caller supplies current native membership and must reset on owner change.
struct WrathReceipt
{
    BuffCountdown countdown{};
    std::uint16_t lastAuxiliary = 0;
    std::uint16_t membershipAuxiliary = 0;
    bool received = false;
};
class WrathReceiptCache
{
    WrathReceipt rows_[2]{};
public:
    constexpr void Reset() { rows_[0] = {}; rows_[1] = {}; }
    constexpr const WrathReceipt* Find(std::uint16_t id) const
    {
        return IsWrathStatusId(id) ? &rows_[id-424] : nullptr;
    }
    constexpr bool Receive(const WrathStatusMessage& message,
        std::uint32_t milliseconds, bool alreadyMember)
    {
        if (!IsWrathStatusId(message.buff)) return false;
        WrathReceipt& row = rows_[message.buff-424];
        if (message.remove) { row = {}; return true; }
        // Existing membership with no matching receipt needs reconciliation;
        // do not invent its original auxiliary value after an owner/reset gap.
        if (alreadyMember && !row.received) return false;
        // Source countdown replaces on every receipt. Membership insertion
        // does not overwrite an existing auxiliary payload (130C63E).
        row.countdown.Receive(milliseconds, message.durationSeconds);
        row.lastAuxiliary = message.auxiliary;
        if (!alreadyMember) row.membershipAuxiliary = message.auxiliary;
        row.received = true;
        return true;
    }
};
} }
