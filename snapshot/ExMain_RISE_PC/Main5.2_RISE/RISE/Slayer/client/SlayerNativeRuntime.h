#pragma once

#include <cstdint>

class CHARACTER;

namespace rise { namespace slayer {

// Production packet-to-render bridge for the isolated Slayer port.  The
// packet has already been decoded by 5.2 ReceiveMagic before entering here.
bool DispatchNativeReceive(CHARACTER* source, CHARACTER* target, int skillId);
bool DispatchNativeLocalCast(CHARACTER* source, CHARACTER* target, int skillId);
bool DispatchBatFanout(CHARACTER* source, const short* targetIndexes,
    int targetCount);
bool DispatchPierceFanout(CHARACTER* source, const short* targetIndexes,
    int targetCount, std::uint8_t castSerial);
bool DispatchDetectionReveal(CHARACTER* source, int map,
    int durationSeconds);
void TickNativeRuntime(int actorKey, std::uint64_t nowMs);
void ResetNativeRuntime(int actorKey);

}}
