#include "../../GrowLancer/compat/ClassBodyDescriptor.h"
using namespace rise::growlancer;
constexpr bool TestBodyDescriptor()
{
    for (unsigned value = 0; value < 512; ++value)
        if (HasVerifiedBaseBodyMaterial(value) != (value == 7)) return false;
    for (unsigned i = 0; i < 5; ++i)
    {
        const auto& body = kBaseClassBodies[i];
        if (body.boneCount != 51 || body.actionCount != 1) return false;
        if (body.sourceModel != 0x370a + 0x50*i) return false;
        if (BaseClassMaterial(body.part, body.meshCount) != nullptr) return false;
        for (unsigned mesh = 0; mesh < body.meshCount; ++mesh)
        {
            const auto* material = BaseClassMaterial(body.part, mesh);
            if (!material) return false;
            if (i == 0 && mesh == 0)
            { if (material->sourceTexture != 0x777d || material->components != 4) return false; }
            else if (material->sourceTexture != 0x772c || material->components != 3) return false;
        }
    }
    return BaseClassMaterial(static_cast<ClassBodyPart>(99), 0) == nullptr;
}
static_assert(TestBodyDescriptor(), "verified base GL body contract; unsupported values rejected");
