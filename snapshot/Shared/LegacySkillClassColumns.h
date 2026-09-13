#pragma once

namespace rise { namespace growlancer {

// Verified legacy SS6 skill/item/quest/master formats have seven class columns,
// regardless of the
// destination class capacity. New classes must be supplied by a separate
// versioned/package contract, never by consuming another legacy row token.
template<unsigned int Capacity, class ReadNumber>
constexpr void ReadLegacyClassColumns(int (&columns)[Capacity], ReadNumber read)
{
    static_assert(Capacity >= 7, "Legacy SS6 requires seven class columns");
    for (unsigned int i = 0; i < Capacity; ++i)
        columns[i] = 0;
    for (unsigned int i = 0; i < 7; ++i)
        columns[i] = read();
}

template<unsigned int Capacity, class ReadNumber>
constexpr void ReadLegacySkillClassColumns(int (&columns)[Capacity], ReadNumber read)
{
    ReadLegacyClassColumns(columns, read);
}

} }
