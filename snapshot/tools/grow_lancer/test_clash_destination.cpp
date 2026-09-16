#include "../../Shared/GrowLancerClashDestination.h"
#include <cassert>
#include <cstdio>

int main()
{
    using namespace rise::growlancer;
    ClashTile destination = {0, 0};
    assert(ComputeS21ClashDestination({100, 100}, {100, 100}, destination));
    assert(destination.x == 100 && destination.y == 100);
    assert(ComputeS21ClashDestination({100, 100}, {105, 100}, destination));
    assert(destination.x == 105 && destination.y == 100);
    assert(ComputeS21ClashDestination({100, 100}, {104, 103}, destination));
    assert(destination.x == 104 && destination.y == 103);
    assert(ComputeS21ClashDestination({100, 100}, {103, 102}, destination));
    assert(destination.x == 102 && destination.y == 102);
    assert(ComputeS21ClashDestination({100, 100}, {97, 98}, destination));
    assert(destination.x == 98 && destination.y == 98);
    assert(ComputeS21ClashDestination({0, 0}, {255, 255}, destination));
    assert(destination.x == 255 && destination.y == 255);
    std::puts("PASS S21 Clash length/truncate/normalize/half-away destination; source packets still disabled");
    return 0;
}
