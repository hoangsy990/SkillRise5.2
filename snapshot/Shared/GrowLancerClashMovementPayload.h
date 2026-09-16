#pragma once

#include <stdint.h>

namespace rise { namespace growlancer {

// S21 C1:59 payload only. Do NOT send this under a native 5.2 opcode until
// the authoritative siege handler, rollback and packet routing are recovered.
enum { ClashMovementPayloadBytes = 7, ClashBlockedTerrainMask = 0x1C };

inline bool ClashDestinationTerrainAllowed(uint16_t terrainFlags)
{
    return (terrainFlags & ClashBlockedTerrainMask) == 0;
}

inline void EncodeS21ClashMovementPayload(uint16_t targetIndex,
    uint16_t skillId, uint8_t destinationX, uint8_t destinationY,
    uint8_t (&payload)[ClashMovementPayloadBytes])
{
    payload[0] = 1;
    payload[1] = static_cast<uint8_t>(targetIndex >> 8);
    payload[2] = static_cast<uint8_t>(skillId >> 8);
    payload[3] = static_cast<uint8_t>(targetIndex);
    payload[4] = static_cast<uint8_t>(skillId);
    payload[5] = destinationX;
    payload[6] = destinationY;
}

}}
