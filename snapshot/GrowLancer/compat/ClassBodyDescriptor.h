#pragma once

namespace rise { namespace growlancer {

// Source identities only. Never use these resource numbers as native model IDs.
// Final native class/evolution activation is separate from this asset contract.
enum class ClassBodyPart { Head, Armor, Pants, Gloves, Boots };
struct ClassBodyDescriptor
{
    ClassBodyPart part;
    unsigned sourceModel;
    const char* modelFile;
    unsigned meshCount;
    unsigned boneCount;
    unsigned actionCount;
};

constexpr ClassBodyDescriptor kBaseClassBodies[] = {
    {ClassBodyPart::Head,   0x370a, "HelmClass08.bmd",  2, 51, 1},
    {ClassBodyPart::Armor,  0x375a, "ArmorClass08.bmd", 1, 51, 1},
    {ClassBodyPart::Pants,  0x37aa, "PantClass08.bmd",  1, 51, 1},
    {ClassBodyPart::Gloves, 0x37fa, "GloveClass08.bmd", 1, 51, 1},
    {ClassBodyPart::Boots,  0x384a, "BootClass08.bmd",  1, 51, 1}
};

struct ClassBodyMaterial
{
    unsigned sourceTexture;
    const char* textureFile;
    unsigned components;
};
constexpr ClassBodyMaterial kBaseClassSkin = {0x772c, "skin_Class108.jpg", 3};
constexpr ClassBodyMaterial kBaseClassHair = {0x777d, "hair_Class108.tga", 4};

// Mesh order established by bounded decoded records, not by asset filenames.
constexpr const ClassBodyMaterial* BaseClassMaterial(ClassBodyPart part, unsigned mesh)
{
    switch (part)
    {
    case ClassBodyPart::Head:
        return mesh == 0 ? &kBaseClassHair : mesh == 1 ? &kBaseClassSkin : nullptr;
    case ClassBodyPart::Armor:
    case ClassBodyPart::Pants:
    case ClassBodyPart::Gloves:
    case ClassBodyPart::Boots:
        return mesh == 0 ? &kBaseClassSkin : nullptr;
    }
    return nullptr;
}

// Do not truncate/mask native or server class values into this source contract.
constexpr bool HasVerifiedBaseBodyMaterial(unsigned sourceClassByte)
{
    return sourceClassByte == 0x07;
}

} }
