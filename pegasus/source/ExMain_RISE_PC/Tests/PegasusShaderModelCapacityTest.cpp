#include "../Main5.2_RISE/RISE/PegasusRuntimeCapacity.h"

#include <cassert>
#include <iostream>

int main()
{
    constexpr int legacyLimit = 12000;
    static_assert(rise::pegasus::RuntimeModelLimit(legacyLimit) == 12128,
        "Pegasus model tail must be included in the shader model limit");
    static_assert(rise::pegasus::IsRuntimeModelId(legacyLimit, legacyLimit),
        "first Pegasus tail model must be accepted");
    static_assert(rise::pegasus::IsRuntimeModelId(legacyLimit + 127, legacyLimit),
        "last Pegasus tail model must be accepted");
    static_assert(!rise::pegasus::IsRuntimeModelId(legacyLimit + 128, legacyLimit),
        "model index beyond the reserved tail must be rejected");

    assert(rise::pegasus::IsRuntimeModelId(legacyLimit + 52, legacyLimit));
    assert(rise::pegasus::IsRuntimeModelId(legacyLimit + 53, legacyLimit));
    std::cout << "PASS: shader accepts the complete isolated Pegasus model tail\n";
    return 0;
}
