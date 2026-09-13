#include "../../Shared/LegacySkillClassColumns.h"

template<unsigned int Capacity>
constexpr bool CheckLegacyColumns()
{
    int output[Capacity] = {};
    int next = 0;
    const int input[] = {1, 0, 2, 0, 3, 1, 0, 279};
    rise::growlancer::ReadLegacySkillClassColumns(output, [&]() { return input[next++]; });
    if (next != 7 || input[next] != 279) return false;
    for (unsigned int i = 0; i < Capacity; ++i)
        if (output[i] != (i < 7 ? input[i] : 0)) return false;
    return true;
}
static_assert(CheckLegacyColumns<7>(), "Preserve all original class values");
static_assert(CheckLegacyColumns<8>(), "Do not consume next row for GL");
static_assert(CheckLegacyColumns<16>(), "Extra destination columns stay disabled");

constexpr bool CheckItemSections()
{
    for (int section = 0; section <= 15; ++section)
    {
        int columns[8] = {};
        int reads = 0;
        // The section14 no-class-column branch must remain outside the helper.
        if (section != 14)
            rise::growlancer::ReadLegacyClassColumns(columns, [&]() { return ++reads; });
        if (reads != (section == 14 ? 0 : 7) || columns[7] != 0) return false;
    }
    return true;
}
static_assert(CheckItemSections(), "Preserve item section14 grammar");

constexpr bool CheckTwoAdjacentRows()
{
    int tokens[] = {1,2,3,0,0,1,0, 901, 0,1,0,2,3,0,1, 902};
    int cursor = 0;
    int quest[8] = {}, master[8] = {};
    rise::growlancer::ReadLegacyClassColumns(quest, [&]() { return tokens[cursor++]; });
    if (tokens[cursor++] != 901) return false;
    rise::growlancer::ReadLegacyClassColumns(master, [&]() { return tokens[cursor++]; });
    return tokens[cursor] == 902 && cursor == 15 && quest[7] == 0 && master[7] == 0
        && quest[0] == 1 && quest[2] == 3 && master[3] == 2 && master[6] == 1;
}
static_assert(CheckTwoAdjacentRows(), "Quest/master class reads preserve next-row token");
