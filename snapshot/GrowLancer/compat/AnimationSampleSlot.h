#pragma once
namespace rise { namespace growlancer {
// Numeric address validation avoids subtraction/comparison of unrelated C++
// object pointers. No dereference, end-address addition or multiplication.
template<class Address>
constexpr bool AnimationSampleSlot(Address address, Address base, Address stride,
                                   unsigned count, unsigned& index)
{
    if (!base || !stride || address < base) return false;
    const Address delta = address - base;
    if (delta % stride || delta / stride >= count) return false;
    index = static_cast<unsigned>(delta / stride);
    return true;
}
} }
