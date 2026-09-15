#pragma once
namespace rise { namespace growlancer {
// Adapt source40ms quantum to native high-FPS frames without sleeping or using
// capped FPS_ANIMATION_FACTOR. Caller supplies monotonic32-bit millisecond samples,
// less than one complete clock wrap apart, and resets at scene/identity handoff.
class MagicQuantumClock
{
    unsigned last_ = 0;
    unsigned long long pending_ = 0;
    bool armed_ = false;
public:
    static_assert(sizeof(unsigned) == 4, "clock samples must be32-bit");
    constexpr void Reset() { last_ = 0; pending_ = 0; armed_ = false; }
    constexpr void Start(unsigned now) { last_ = now; pending_ = 0; armed_ = true; }
    constexpr bool Observe(unsigned now)
    {
        if (!armed_) return false;
        const unsigned elapsed = now - last_; // unsigned wrap is deliberate
        if (pending_ > ~0ull - elapsed) return false;
        pending_ += elapsed;
        last_ = now;
        return true;
    }
    constexpr unsigned long long Due() const { return pending_ / 40ull; }
    constexpr unsigned long long PendingMilliseconds() const { return pending_; }
    constexpr bool Consume(unsigned completed)
    {
        if (completed > Due()) return false;
        pending_ -= static_cast<unsigned long long>(completed) * 40ull;
        return true;
    }
};
} }
