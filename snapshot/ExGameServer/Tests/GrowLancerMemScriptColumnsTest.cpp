#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include "../GameServer/MemScript.h"
#include "../../Shared/LegacySkillClassColumns.h"

template<unsigned int Capacity>
bool Run(const char* path, bool fullSkillRow)
{
    CMemScript script;
    if (!script.SetBuffer(path)) return false;
    const int expected[2][7] = {{1,0,2,3,0,1,0}, {0,3,2,1,0,0,1}};
    for (int row = 0; row < 2; ++row)
    {
        if (script.GetToken() != TOKEN_NUMBER || script.GetNumber() != 901 + row) return false;
        if (std::strcmp(script.GetAsString(), row == 0 ? "First row" : "Second row") != 0) return false;
        // SkillManager::Load: Damage through RequireGuildStatus, in source order.
        // Unique sentinels detect token shifts; these are NOT real skill values.
        if (fullSkillRow)
            for (int field = 0; field < 13; ++field)
                if (script.GetAsNumber() != row * 100 + 10 + field) return false;
        int columns[Capacity] = {};
        rise::growlancer::ReadLegacyClassColumns(columns, [&]() { return script.GetAsNumber(); });
        for (unsigned int i = 0; i < Capacity; ++i)
            if (columns[i] != (i < 7 ? expected[row][i] : 0)) return false;
    }
    if (script.GetToken() != TOKEN_STRING || std::strcmp(script.GetString(), "end") != 0) return false;
    return script.GetToken() == TOKEN_END;
}

int main(int argc, char** argv)
{
    if (argc != 2 && argc != 3) return 2;
    const bool fullSkillRow = argc == 3 && std::strcmp(argv[2], "full-skill") == 0;
    if (argc == 3 && !fullSkillRow) return 2;
    try {
        if (!Run<7>(argv[1], fullSkillRow) || !Run<8>(argv[1], fullSkillRow) ||
            !Run<16>(argv[1], fullSkillRow)) return 1;
    } catch (...) { return 3; }
    std::puts("PASS real CMemScript: comments, quoted names, adjacent rows, end and EOF at capacities7/8/16");
    if (fullSkillRow) std::puts("PASS full skill-row fixture: thirteen metadata tokens before class columns; synthetic values only");
    return 0;
}
