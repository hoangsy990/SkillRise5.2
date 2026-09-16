#include "../../GrowLancer/compat/ParticleBirthOrder.h"
using rise::growlancer::ParticleBirthOrder;

constexpr bool BirthAndReuse()
{
    ParticleBirthOrder<4> order;
    if (order.First() != order.End() || order.Append(4)) return false;
    order.Append(3); order.Append(0); order.Append(2);
    if (order.First() != 3 || order.Next(3) != 0 || order.Next(0) != 2) return false;
    order.Remove(0); // ordinary native allocation reuses this slot
    if (order.Contains(0) || order.Next(3) != 2) return false;
    order.Append(3); // a new Magic birth at reused slot3 belongs at the tail
    if (order.First() != 2 || order.Next(2) != 3) return false;
    order.Remove(2); order.Remove(3); order.Remove(9);
    return order.First() == order.End() && order.Next(3) == order.End();
}
constexpr bool BirthDuringTraversal()
{
    ParticleBirthOrder<4> order;
    order.Append(3);
    unsigned visited = 0;
    for (unsigned slot = order.First(); slot != order.End(); slot = order.Next(slot))
    {
        ++visited;
        if (slot == 3) order.Append(0);
    }
    if (visited != 2) return false;
    // Retirement must save next before unlinking the current entry.
    unsigned slot = order.First();
    while (slot != order.End())
    {
        const unsigned next = order.Next(slot);
        order.Remove(slot);
        slot = next;
    }
    return order.First() == order.End();
}
static_assert(BirthAndReuse(), "ring slot indices must not reorder particle births");
static_assert(BirthDuringTraversal(), "append during update remains visible this traversal");
