#pragma once
namespace rise { namespace growlancer {
// Single connection, single local owner. No object pointers or cross-thread lock.
class WrathPendingReset
{
    int index_ = -1, key_ = -1;
    unsigned mask_ = 0;
public:
    constexpr bool Bind(int index, int key)
    {
        if (index < 0 || key < 0 || mask_) return false;
        if (index_ >= 0 && (index_ != index || key_ != key)) return false;
        index_ = index; key_ = key; return true;
    }
    constexpr void Capture(bool wrath, bool strengthened)
    {
        if (index_ < 0) return;
        mask_ |= (wrath ? 1u : 0u) | (strengthened ? 2u : 0u);
        if (!mask_) { index_ = -1; key_ = -1; }
    }
    constexpr unsigned Take(int& index, int& key)
    {
        const unsigned result = mask_;
        if (result)
        {
            index = index_; key = key_;
            mask_ = 0; index_ = -1; key_ = -1;
        }
        return result;
    }
};
} }
