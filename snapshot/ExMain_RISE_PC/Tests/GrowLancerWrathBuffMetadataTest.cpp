#include "../Main5.2_RISE/RISE/GrowLancerWrathBuffMetadata.h"
using rise::growlancer::HasWrathBuffClassification;
constexpr bool ExactClassification()
{
    for (int id = -1; id < 65536; ++id)
        if (HasWrathBuffClassification(id) != (id == 424 || id == 425))
            return false;
    return !HasWrathBuffClassification(65536 + 424)
        && !HasWrathBuffClassification(65536 + 425)
        && !HasWrathBuffClassification(-65536 + 424);
}
static_assert(ExactClassification(), "Only exact Wrath buff IDs may bypass legacy classification");
constexpr bool IconMapping()
{
    using namespace rise::growlancer;
    WrathBuffIconRect rect = {-1,-1,-1,-1};
    if (GetWrathBuffIconRect(278,rect) || rect.x != -1) return false;
    if (!GetWrathBuffIconRect(424,rect) || rect.x != 220 || rect.y != 120
        || rect.width != 20 || rect.height != 28) return false;
    if (!GetWrathBuffIconRect(425,rect) || rect.x != 242 || rect.y != 120
        || rect.x+rect.width > kWrathBuffAtlasWidth
        || rect.y+rect.height > kWrathBuffAtlasHeight) return false;
    if (GetWrathBuffIconRect(65536+424,rect) || rect.x != 242) return false;
    return true;
}
static_assert(IconMapping(), "Wrath atlas IDs must map to exact source subimages");
constexpr int TextBytes(const char* value)
{
    int count = 0;
    while (value[count]) ++count;
    return count;
}
static_assert(TextBytes(rise::growlancer::WrathBuffName(424)) < 50, "Native name bound");
static_assert(TextBytes(rise::growlancer::WrathBuffName(425)) < 50, "Native name bound");
static_assert(TextBytes(rise::growlancer::kWrathBuffDescription) < 100, "Native description bound");
static_assert(rise::growlancer::WrathBuffName(278) == nullptr, "Skill ID is not buff metadata");
