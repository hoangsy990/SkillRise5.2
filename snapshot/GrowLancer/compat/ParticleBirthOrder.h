#pragma once

namespace rise { namespace growlancer {
// Sidecar only: native allocation and Live remain authoritative. One entry per
// native slot; reuse unlinks its old position before appending the new birth.
// Single-threaded scene traversal; never retain an index across slot reuse.
template<unsigned Capacity> class ParticleBirthOrder
{
    struct Link { unsigned next; unsigned previous; bool linked; };
    Link links_[Capacity] = {};
    unsigned first_ = Capacity;
    unsigned last_ = Capacity;
public:
    static_assert(Capacity > 0, "particle pool must be nonempty");
    constexpr unsigned End() const { return Capacity; }
    constexpr unsigned First() const { return first_; }
    constexpr bool Contains(unsigned slot) const
    { return slot < Capacity && links_[slot].linked; }
    constexpr unsigned Next(unsigned slot) const
    { return Contains(slot) ? links_[slot].next : Capacity; }
    constexpr void Remove(unsigned slot)
    {
        if (!Contains(slot)) return;
        const unsigned previous = links_[slot].previous;
        const unsigned next = links_[slot].next;
        if (previous == Capacity) first_ = next;
        else links_[previous].next = next;
        if (next == Capacity) last_ = previous;
        else links_[next].previous = previous;
        links_[slot].linked = false;
    }
    constexpr bool Append(unsigned slot)
    {
        if (slot >= Capacity) return false;
        Remove(slot);
        links_[slot] = {Capacity, last_, true};
        if (last_ == Capacity) first_ = slot;
        else links_[last_].next = slot;
        last_ = slot;
        return true;
    }
};
} }
