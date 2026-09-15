#pragma once
#include <cstdint>
namespace rise { namespace growlancer {
// S21 A0D270 / A0D5EA: divide the millisecond clock BEFORE subtraction.
// This is display-time arithmetic, not authoritative buff membership expiry.
constexpr std::uint32_t BuffClockSeconds(std::uint32_t milliseconds)
{
    return milliseconds / 1000u;
}
constexpr std::uint32_t BuffRemainingSeconds(std::uint32_t startSeconds,
    std::uint32_t durationSeconds, std::uint32_t currentMilliseconds)
{
    const std::uint32_t elapsed = BuffClockSeconds(currentMilliseconds)-startSeconds;
    return durationSeconds > elapsed ? durationSeconds-elapsed : 0u;
}
// Caller-owned countdown only: no duplicate buff map, packet layout or timer.
struct BuffCountdown
{
    std::uint32_t startSeconds = 0;
    std::uint32_t durationSeconds = 0;
    constexpr void Receive(std::uint32_t milliseconds, std::uint32_t duration)
    {
        startSeconds = BuffClockSeconds(milliseconds);
        durationSeconds = duration;
    }
    constexpr std::uint32_t Remaining(std::uint32_t milliseconds) const
    {
        return BuffRemainingSeconds(startSeconds,durationSeconds,milliseconds);
    }
};
} }
