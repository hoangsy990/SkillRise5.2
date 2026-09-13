#include "stdafx.h"
#include "GrowLancerResources.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzTexture.h"
#include "DSPlaySound.h"

namespace rise { namespace growlancer {
namespace {

struct ModelRow
{
    int id;
    const char* directory;
    const char* filename;
};

const ModelRow kModels[] = {
    {kWrathAuraModel, "Data\\RISE\\GrowLancer\\Wrath\\", "wratha01.bmd"},
    {kWrathTravelModel, "Data\\RISE\\GrowLancer\\Wrath\\", "wratha02.bmd"},
    {kObsidianRotatingModel, "Data\\RISE\\GrowLancer\\Obsidian\\", "obsidiana.bmd"},
    {kObsidianAuraModel, "Data\\RISE\\GrowLancer\\Obsidian\\", "obsidian.bmd"},
    {kCircleShieldModel, "Data\\RISE\\GrowLancer\\CircleShield\\", "circle_shields.bmd"},
    {kSpinCrossModel, "Data\\RISE\\GrowLancer\\SpinStep\\", "spin_cross.bmd"},
    {kMagicPin01Model, "Data\\RISE\\GrowLancer\\MagicPin\\", "magicpin01.bmd"},
    {kMagicPin03Model, "Data\\RISE\\GrowLancer\\MagicPin\\", "magicpin03_new.bmd"},
    {kMagicPinRootModel, "Data\\RISE\\GrowLancer\\MagicPin\\", "magicpina01_new.bmd"},
    {kMagicPinAuxModel, "Data\\RISE\\GrowLancer\\MagicPin\\", "magicpina02.bmd"},
    {kShiningPeakBodyModel, "Data\\RISE\\GrowLancer\\ShiningPeak\\", "shiningpeakbody.bmd"},
    {kShiningPeakPinModel, "Data\\RISE\\GrowLancer\\ShiningPeak\\", "shiningpeakpin.bmd"},
    {kShiningPeakSpinModel, "Data\\RISE\\GrowLancer\\ShiningPeak\\", "shiningpeakspinmagic.bmd"},
    {kShiningPeakShockwaveModel, "Data\\RISE\\GrowLancer\\ShiningPeak\\", "shockwave01.bmd"},
    {kHarshWind01Model, "Data\\RISE\\GrowLancer\\HarshStrike\\", "h_strike_wind01.bmd"},
    {kHarshWind02Model, "Data\\RISE\\GrowLancer\\HarshStrike\\", "h_strike_wind02.bmd"},
    {kClashFrontModel, "Data\\RISE\\GrowLancer\\Clash\\", "crasha01.bmd"},
    {kClashRearModel, "Data\\RISE\\GrowLancer\\Clash\\", "crasha02.bmd"}
};

struct BitmapRow
{
    int id;
    const char* path;
};

const BitmapRow kBitmaps[] = {
    {kWrathComboBitmap, "Data\\RISE\\GrowLancer\\Wrath\\!Combo3.jpg"},
    {kWrathAlphaLightBitmap, "Data\\RISE\\GrowLancer\\Wrath\\alpha_light.tga"},
    {kWrathBrokenBitmap, "Data\\RISE\\GrowLancer\\Wrath\\brocken.jpg"},
    {kCircleShinyBitmap, "Data\\RISE\\GrowLancer\\CircleShield\\shiny04.jpg"},
    {kFlareBlueBitmap, "Data\\RISE\\GrowLancer\\SpinStep\\flareBlue.jpg"},
    {kHoleBitmap, "Data\\RISE\\GrowLancer\\SpinStep\\hole.jpg"},
    {kSpinGroundBitmap, "Data\\RISE\\GrowLancer\\SpinStep\\magic_ground1_3.jpg"},
    {kShockwave2Bitmap, "Data\\RISE\\GrowLancer\\MagicPin\\Shockwave2.jpg"},
    {kXsuper0001Bitmap, "Data\\RISE\\GrowLancer\\ShiningPeak\\xsuper0001.jpg"},
    {kMagicGround2Bitmap, "Data\\RISE\\GrowLancer\\HarshStrike\\Magic_Ground2.jpg"},
    {kFlare01RedBitmap, "Data\\RISE\\GrowLancer\\HarshStrike\\flare01_red.jpg"},
    {kImpact03Bitmap, "Data\\RISE\\GrowLancer\\HarshStrike\\Impack03.jpg"},
    {kMagicGround3Bitmap, "Data\\RISE\\GrowLancer\\HarshStrike\\magic_ground3.jpg"},
    {kDamageMonoBitmap, "Data\\RISE\\GrowLancer\\HarshStrike\\Damage1mono.jpg"},
    {kBrecheRedBitmap, "Data\\RISE\\GrowLancer\\Breche\\Item1130_L_red.jpg"},
    {kXsuper0002Bitmap, "Data\\RISE\\GrowLancer\\ShiningPeak\\xsuper0002.jpg"},
    {kXsuper0003Bitmap, "Data\\RISE\\GrowLancer\\ShiningPeak\\xsuper0003.jpg"},
    {kXsuper0004Bitmap, "Data\\RISE\\GrowLancer\\ShiningPeak\\xsuper0004.jpg"},
    {kXsuper0005Bitmap, "Data\\RISE\\GrowLancer\\ShiningPeak\\xsuper0005.jpg"},
    {kXsuper0006Bitmap, "Data\\RISE\\GrowLancer\\ShiningPeak\\xsuper0006.jpg"}
};

struct SkillRow
{
    int id;
    const char* name;
    int level;
    int damage;
    int mana;
    int abilityGauge;
    int distance;
    int delay;
    int energy;
    int strength;
    int dexterity;
    int isDamage;
};

const SkillRow kSkills[] = {
    {271, "Spin Step", 83, 100, 12, 0, 2, 500, 0, 0, 150, 1},
    {272, "Circle Shield", 220, 0, 100, 50, 0, 0, 0, 0, 0, 0},
    {273, "Obsidian", 74, 0, 50, 50, 0, 0, 200, 0, 0, 0},
    {274, "Magic Pin", 30, 80, 5, 3, 2, 500, 0, 200, 200, 1},
    {275, "Clash", 0, 50, 50, 50, 6, 10000, 0, 0, 0, 0},
    {276, "Harsh Strike", 74, 100, 12, 0, 3, 500, 0, 150, 0, 1},
    {277, "Shining Peak", 92, 50, 8, 5, 4, 0, 0, 600, 0, 1},
    {278, "Wrath", 66, 0, 40, 30, 0, 0, 0, 200, 200, 0},
    {279, "Breche", 300, 230, 15, 6, 5, 0, 0, 300, 300, 1}
};

}

bool EnsureModel(int modelId)
{
    if (!Models || !IsVisibleModel(modelId))
        return false;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.NumBones > 0 && model.NumActions > 0)
        return true;
    const ModelRow* row = 0;
    for (int i = 0; i < static_cast<int>(sizeof(kModels) / sizeof(kModels[0])); ++i)
        if (kModels[i].id == modelId) { row = &kModels[i]; break; }
    if (!row)
        return false;
    char directory[MAX_PATH] = {};
    char filename[MAX_PATH] = {};
    strcpy_s(directory, row->directory);
    strcpy_s(filename, row->filename);
    model.m_iBMDSeqID = modelId;
    if (!model.Open2(directory, filename))
        return false;
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        const std::string path = std::string(directory) + model.Textures[mesh].FileName;
        model.IndexTexture[mesh] = Bitmaps.LoadImageFile(path, GL_LINEAR, GL_REPEAT);
        if (model.IndexTexture[mesh] == BITMAP_UNKNOWN)
        {
            model.Release();
            return false;
        }
    }
#ifdef jdk_shader_local330
    OGL330MODEL::ConvertOldMeshToVaoMesh(modelId);
#endif
    return model.NumMeshs > 0 && model.NumBones > 0 && model.NumActions > 0;
}

bool IsVisibleModel(int modelId)
{
    return modelId >= kFirstModel && modelId <= kLastVisibleModel;
}

bool EnsureBitmaps()
{
    for (int i = 0; i < static_cast<int>(sizeof(kBitmaps) / sizeof(kBitmaps[0])); ++i)
    {
        const BitmapRow& row = kBitmaps[i];
        if (Bitmaps.FindTexture(row.id))
            continue;
        if (!Bitmaps.LoadImageFile(row.id, row.path, GL_LINEAR, GL_CLAMP_TO_EDGE))
            return false;
    }
    return true;
}

bool EnsureBrecheBitmaps()
{
    // Separate gate: missing Breche resources must not prevent other skills
    // from allocating effects. Native loader handles the staged OZJ assets.
    const BitmapRow rows[] = {
        {kBrecheLightMarksBitmap, "Data\\RISE\\GrowLancer\\Breche\\lightmarks_red.jpg"},
        {kBrecheTwilight02Bitmap, "Data\\RISE\\GrowLancer\\Breche\\twlighthik02.jpg"},
        {kBrecheTwilight01Bitmap, "Data\\RISE\\GrowLancer\\Breche\\twlighthik01.jpg"}
    };
    for (int i = 0; i < static_cast<int>(sizeof(rows) / sizeof(rows[0])); ++i)
    {
        if (!Bitmaps.FindTexture(rows[i].id) &&
            !Bitmaps.LoadImageFile(rows[i].id, rows[i].path, GL_LINEAR, GL_CLAMP))
            return false;
    }
    return true;
}

void LoadSounds()
{
    LoadWaveFile(kSpinStepCastSound, "Data\\RISE\\GrowLancer\\SpinStep\\SpinStep_Cast.wav");
    LoadWaveFile(kSpinStepHitSound, "Data\\RISE\\GrowLancer\\SpinStep\\SpinStep_Hit.wav");
    LoadWaveFile(kCircleShieldSound, "Data\\RISE\\GrowLancer\\CircleShield\\CircleShield.wav");
    LoadWaveFile(kObsidianSound, "Data\\RISE\\GrowLancer\\Obsidian\\Obsidian.wav");
    LoadWaveFile(kMagicPinCastSound, "Data\\RISE\\GrowLancer\\MagicPin\\MagicPin.wav");
    LoadWaveFile(kMagicPinHitSound, "Data\\RISE\\GrowLancer\\MagicPin\\MagicPinExplode.wav");
    LoadWaveFile(kClashSound, "Data\\RISE\\GrowLancer\\Clash\\Crash.wav");
    LoadWaveFile(kHarshStrikeSound, "Data\\RISE\\GrowLancer\\HarshStrike\\HashStrike.wav");
    LoadWaveFile(kShiningPeakSound, "Data\\RISE\\GrowLancer\\ShiningPeak\\ShiningPeak.wav");
    LoadWaveFile(kWrathSound, "Data\\RISE\\GrowLancer\\Wrath\\Wrath.wav");
    LoadWaveFile(kBrecheSound, "Data\\RISE\\GrowLancer\\Breche\\Breche.wav");
}

void ApplySkillCatalog()
{
    static_assert(MAX_SKILLS > kMagicPinExplosionSkill,
        "Grow Lancer skill ID capacity is too small");
    static_assert(MAX_CLASS == 7,
        "Fail-closed class contract must be reviewed if class count changes");
    for (int i = 0; i < static_cast<int>(sizeof(kSkills) / sizeof(kSkills[0])); ++i)
    {
        const SkillRow& row = kSkills[i];
        SKILL_ATTRIBUTE& value = SkillAttribute[row.id];
        ZeroMemory(&value, sizeof(value));
        strcpy_s(value.Name, row.name);
        value.Level = static_cast<WORD>(row.level);
        value.Damage = static_cast<WORD>(row.damage);
        value.Mana = static_cast<WORD>(row.mana);
        value.AbilityGuage = static_cast<WORD>(row.abilityGauge);
        value.Distance = row.distance;
        value.Delay = row.delay;
        value.Energy = row.energy;
        value.Strength = row.strength;
        value.Dexterity = row.dexterity;
        value.MasteryType = 255;
        value.SkillUseType = 0;
        value.TypeSkill = 0;
        value.ItemSkill = 0;
        value.IsDamage = static_cast<BYTE>(row.isDamage);
        value.Magic_Icon = static_cast<WORD>(row.id);
        // Intentionally leave all seven RequireClass bytes zero. RISE 5.2 has
        // no Grow Lancer class slot; assigning an SS6 class would be guessing.
    }
}

}}
