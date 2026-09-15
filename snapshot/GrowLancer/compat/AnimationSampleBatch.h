#pragma once

namespace rise { namespace growlancer {
// Experimental handoff storage, not enabled in native gameplay yet.
// Producer supplies actual post-animation samples and a fresh nonzero serial
// per batch. Generation is actor identity, not an address reused by a pool.
// Samples are non-destructive reads so multiple controllers see the same tick.
// No interpolation, ticking, action reset or lifetime policy lives here.
template<unsigned Capacity> class AnimationSampleBatch
{
    static_assert(Capacity > 0, "A batch must hold at least one sample");
    unsigned generation_ = 0, serial_ = 0, count_ = 0;
    unsigned short action_ = 0;
    bool valid_ = false, published_ = false;
    float frames_[Capacity] = {};
    unsigned short actions_[Capacity] = {};
public:
    constexpr bool Begin(unsigned generation, unsigned serial, unsigned short action)
    {
        if (!generation || !serial || (generation == generation_ && serial <= serial_))
            return false;
        generation_ = generation; serial_ = serial; action_ = action;
        count_ = 0; valid_ = true; published_ = false;
        return true;
    }
    constexpr bool Append(float frame)
    {
        return AppendStep(action_, frame);
    }
    constexpr bool AppendStep(unsigned short action, float frame)
    {
        if (published_) return false; // Published samples are immutable.
        // NaN, infinity and negative frames cannot be native post-step samples.
        if (!valid_ || !(frame >= 0.f && frame <= 3.402823466e+38f) || count_ == Capacity)
        {
            valid_ = false; count_ = 0;
            return false;
        }
        actions_[count_] = action;
        frames_[count_++] = frame;
        return true;
    }
    constexpr void Invalidate() { valid_ = false; count_ = 0; }
    constexpr bool Publish()
    {
        if (!valid_) return false;
        published_ = true;
        return true;
    }
    constexpr bool Read(unsigned generation, unsigned serial, unsigned short action,
                        unsigned index, float& output) const
    {
        if (!valid_ || !published_ || generation != generation_ || serial != serial_ ||
            action != action_ || index >= count_ || actions_[index] != action) return false;
        output = frames_[index];
        return true;
    }
    // Consumers must observe action exit at its own step, not borrow the
    // owner's final action for all earlier samples in a catch-up batch.
    constexpr bool ReadStep(unsigned generation, unsigned serial, unsigned index,
                            unsigned short& action, float& frame) const
    {
        if (!valid_ || !published_ || generation != generation_ || serial != serial_ ||
            index >= count_) return false;
        action = actions_[index]; frame = frames_[index];
        return true;
    }
};

// One cursor per controller allocation. Reset on pool allocation/destruction;
// actor generation must change before serial rollover. This is single-threaded
// handoff storage, NOT a lock-free publication or lifetime-safe owner pointer.
class AnimationSampleCursor
{
    unsigned generation_ = 0, serial_ = 0, next_ = 0;
public:
    constexpr void Reset() { generation_ = serial_ = next_ = 0; }
    constexpr bool Bind(unsigned generation, unsigned serial)
    {
        if (!generation || !serial || (generation == generation_ && serial <= serial_))
            return false;
        generation_ = generation; serial_ = serial; next_ = 0;
        return true;
    }
    template<unsigned Capacity>
    constexpr bool Next(const AnimationSampleBatch<Capacity>& batch,
                        unsigned short& action, float& frame)
    {
        if (!batch.ReadStep(generation_, serial_, next_, action, frame)) return false;
        ++next_;
        return true;
    }
};
} }
