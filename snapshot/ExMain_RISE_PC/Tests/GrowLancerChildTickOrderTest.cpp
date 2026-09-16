// Ordering counterexample only, not execution of the S21/native allocator.
struct Slot { bool live = false; int ticks = 0; };
constexpr int ChildUpdates(bool tickMajor, bool childBeforeParent)
{
    Slot slots[2]{};
    const int parent = childBeforeParent ? 1 : 0;
    const int child = 1-parent;
    slots[parent].live = true;
    if (tickMajor)
    {
        for (int tick=0;tick<2;++tick)
            for (int slot=0;slot<2;++slot)
                if (slots[slot].live)
                {
                    ++slots[slot].ticks;
                    if (slot==parent && slots[slot].ticks==2) slots[child].live=true;
                }
    }
    else
    {
        for (int slot=0;slot<2;++slot)
            if (slots[slot].live)
                for (int tick=0;tick<2;++tick)
                {
                    ++slots[slot].ticks;
                    if (slot==parent && slots[slot].ticks==2) slots[child].live=true;
                }
    }
    return slots[child].ticks;
}
static_assert(ChildUpdates(true,false)==1, "later child born on tick2 gets only tick2");
static_assert(ChildUpdates(false,false)==2, "record-major catch-up wrongly gives newborn both ticks");
static_assert(ChildUpdates(true,true)==0, "earlier child waits until next traversal");
static_assert(ChildUpdates(false,true)==0, "no immediate child pass may be invented");
