#include "../../Shared/GrowLancerClashMovementPayload.h"
#include <cassert>
#include <cstdio>

int main()
{
    using namespace rise::growlancer;
    assert(ClashDestinationTerrainAllowed(0));
    assert(ClashDestinationTerrainAllowed(0xE3));
    for (unsigned mask = 0x04; mask <= 0x10; mask <<= 1)
        assert(!ClashDestinationTerrainAllowed(static_cast<uint16_t>(mask)));
    assert(!ClashDestinationTerrainAllowed(0xFFFF));

    uint8_t payload[ClashMovementPayloadBytes] = {};
    EncodeS21ClashMovementPayload(0x1234, 0x0113, 0x55, 0xAA, payload);
    const uint8_t expected[ClashMovementPayloadBytes] =
        {1, 0x12, 0x01, 0x34, 0x13, 0x55, 0xAA};
    for (unsigned i = 0; i < ClashMovementPayloadBytes; ++i)
        assert(payload[i] == expected[i]);
    EncodeS21ClashMovementPayload(0, 0xFFFF, 0, 0xFF, payload);
    const uint8_t boundary[ClashMovementPayloadBytes] =
        {1, 0, 0xFF, 0, 0xFF, 0, 0xFF};
    for (unsigned i = 0; i < ClashMovementPayloadBytes; ++i)
        assert(payload[i] == boundary[i]);
    std::puts("PASS S21 Clash terrain mask and seven-byte payload; no native opcode/handler activated");
    return 0;
}
