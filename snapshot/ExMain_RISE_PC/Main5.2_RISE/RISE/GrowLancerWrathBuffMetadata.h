#pragma once

namespace rise { namespace growlancer {
// Pinned S21 BuffEffect rows 424/425: classifier 0 (normal buff).
// Exact signed comparison: do not narrow an arbitrary enum to a packet WORD.
// This is classification only, not icon/text registration or activation readiness.
constexpr bool HasWrathBuffClassification(int id)
{
    return id == 424 || id == 425;
}
// Exact UTF8 text from the pinned S21 BuffEffect heap records, not formulas.
constexpr const char* WrathBuffName(int id)
{
    return id == 424 ? u8"Th\u1ecbnh N\u1ed9" :
        id == 425 ? u8"T\u0103ng C\u01b0\u1eddng Th\u1ecbnh N\u1ed9" : nullptr;
}
constexpr const char* kWrathBuffDescription =
    u8"T\u0103ng s\u1ee9c t\u1ea5n c\u00f4ng v\u00e0 s\u1ee9c chi\u1ebfn \u0111\u1ea5u. Ph\u00f2ng th\u1ee7 gi\u1ea3m.";
// ImageData.gfx export73=icon_424, export72=icon_425; imageId0 atlas I7.
// Source rectangles are 20x28 in a 1024x256 atlas, not full 26x36 UI size.
struct WrathBuffIconRect { int x, y, width, height; };
constexpr int kWrathBuffAtlasWidth = 1024;
constexpr int kWrathBuffAtlasHeight = 256;
constexpr bool GetWrathBuffIconRect(int id, WrathBuffIconRect& result)
{
    if (!HasWrathBuffClassification(id)) return false;
    result = {id == 424 ? 220 : 242, 120, 20, 28};
    return true;
}
} }
