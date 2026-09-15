#include "stdafx.h"
#include "GrowLancerResources.h"
#include "GrowLancerSkillIdCapacity.h"
#include "ZzzBMD.h"
#include "ZzzInfomation.h"
#include "ZzzTexture.h"
#include "DSPlaySound.h"
#include "../../../GrowLancer/compat/ClassBodyDescriptor.h"

extern float BoneScale; // existing native ZzzBMD.cpp transform scale

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
    {kClashRearModel, "Data\\RISE\\GrowLancer\\Clash\\", "crasha02.bmd"},
    // S21 0xAD9 is a one-mesh BMD using ground_wind.jpg.  The converted
    // file is staged under the Breche owner package, not the shared model
    // registry, so native MODEL_WINDFOCE is never replaced.
    {kBrecheOwnerWindModel, "Data\\RISE\\GrowLancer\\Breche\\Owner\\", "wind_foce.bmd"}
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

static bool EnsureClassBodyGpu(unsigned part)
{
#ifdef jdk_shader_local330
    if (!Models || part >= kClassBodyModelCount || !wglGetCurrentContext()) return false;
    if (!OGL330::IsShader()) return true; // native legacy path needs no VAO
    BMD& model = Models[ClassBodyModelId(part, MAX_MODELS)];
    if (model.NumMeshs != kBaseClassBodies[part].meshCount) return false;
    if (!model.NewMeshes)
    {
        // Reuse native conversion/upload, scoped to our five private body slots.
        // The legacy registry wrapper deliberately rejects extended model IDs.
        GLint vao = 0, arrayBuffer = 0, elementBuffer = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vao);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &arrayBuffer);
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBuffer);
        model.LoadMeshToVAO();
        model.UploadAllToGPU();
        glBindVertexArray(static_cast<GLuint>(vao));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(elementBuffer));
        glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(arrayBuffer));
    }
    bool valid = model.NewMeshes && model.NewMeshes->size() == static_cast<size_t>(model.NumMeshs);
    if (valid)
    {
        for (size_t mesh = 0; mesh < model.NewMeshes->size(); ++mesh)
        {
            const VAOMesh& gpu = (*model.NewMeshes)[mesh];
            valid = valid && gpu.VertexCount > 0 && gpu.IndexCount > 0
                && glIsVertexArray(gpu.VAO) && glIsBuffer(gpu.VBO) && glIsBuffer(gpu.IBO);
        }
    }
    if (!valid) model.ReleaseGpuMeshes(); // retry without reacquiring CPU/material refs
    return valid;
#else
    return part < kClassBodyModelCount;
#endif
}

static bool ClassBodyMaterialMatches(GLuint id, const ClassBodyMaterial* material,
    unsigned filter, unsigned wrap)
{
    BITMAP_t* bitmap = Bitmaps.FindTexture(id);
    if (!material || !bitmap || bitmap->Components != material->components
        || !glIsTexture(bitmap->TextureNumber)) return false;
    const std::string path = std::string("Data\\RISE\\GrowLancer\\ClassBody\\") + material->textureFile;
    if (Bitmaps.FindTexture(path) != bitmap) return false;
    GLint previous = 0, minFilter = 0, magFilter = 0, wrapS = 0, wrapT = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous);
    glBindTexture(GL_TEXTURE_2D, bitmap->TextureNumber);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(previous));
    return minFilter == filter && magFilter == filter && wrapS == wrap && wrapT == wrap;
}

bool EnsureClassBodyModel(unsigned sourceClassByte, unsigned part, unsigned filter, unsigned wrap)
{
    if (!HasVerifiedBaseBodyMaterial(sourceClassByte) || part >= kClassBodyModelCount
        || !Models || !wglGetCurrentContext()) return false;
    if ((filter != GL_NEAREST && filter != GL_LINEAR)
        || (wrap != GL_REPEAT && wrap != GL_CLAMP_TO_EDGE)) return false;
    const int id = ClassBodyModelId(part, MAX_MODELS);
    const ClassBodyDescriptor& descriptor = kBaseClassBodies[part];
    BMD& model = Models[id];
    if (model.NumMeshs > 0)
    {
        if (model.NumMeshs != descriptor.meshCount || model.NumBones != descriptor.boneCount
            || model.NumActions != descriptor.actionCount || !model.Meshs || !model.IndexTexture)
            return false;
        for (unsigned mesh = 0; mesh < descriptor.meshCount; ++mesh)
            if (model.Meshs[mesh].Texture != mesh || !ClassBodyMaterialMatches(model.IndexTexture[mesh],
                BaseClassMaterial(descriptor.part, mesh), filter, wrap)) return false;
        return EnsureClassBodyGpu(part);
    }
    char directory[] = "Data\\RISE\\GrowLancer\\ClassBody\\";
    char filename[32] = {};
    strcpy_s(filename, descriptor.modelFile);
    model.m_iBMDSeqID = id;
    if (!model.Open2(directory, filename)) return false;
    bool valid = model.NumMeshs == descriptor.meshCount && model.NumBones == descriptor.boneCount
        && model.NumActions == descriptor.actionCount;
    // Native Release reads IndexTexture[mesh.Texture]. Initialize every slot
    // before any material load can fail. These fixed bodies use unique slots.
    for (int mesh = 0; mesh < model.NumMeshs; ++mesh)
    {
        model.IndexTexture[mesh] = BITMAP_SKIN; // native no-delete sentinel
        if (model.Meshs[mesh].Texture != mesh) valid = false;
        model.Meshs[mesh].Texture = static_cast<short>(mesh);
    }
    if (!valid) { model.Release(); model.m_bCompletedAlloc = false; return false; }
    for (unsigned mesh = 0; mesh < descriptor.meshCount; ++mesh)
    {
        const ClassBodyMaterial* material = BaseClassMaterial(descriptor.part, mesh);
        const GLuint texture = material ? Bitmaps.LoadImageFile(
            std::string(directory) + material->textureFile, filter, wrap) : BITMAP_UNKNOWN;
        if (texture == BITMAP_UNKNOWN)
        {
            // Unassigned slots remain sentinel; previously acquired refs are
            // released exactly once by the existing native BMD lifecycle.
            model.Release();
            model.m_bCompletedAlloc = false;
            return false;
        }
        model.IndexTexture[mesh] = texture;
        // Native filename cache ignores requested sampler on reuse. Never mutate
        // a shared texture to satisfy this private body request.
        if (!ClassBodyMaterialMatches(texture, material, filter, wrap))
        {
            model.Release(); model.m_bCompletedAlloc = false;
            return false;
        }
    }
    return EnsureClassBodyGpu(part);
}

BaseBodySubmitResult SubmitClassBaseBody(unsigned sourceClassByte, unsigned part,
    OBJECT* owner, unsigned boneCount, const float* light, bool hideSkin,
    bool translate, unsigned filter, unsigned wrap)
{
    if (!HasVerifiedBaseBodyMaterial(sourceClassByte) || part >= kClassBodyModelCount
        || !owner || !light) return BaseBodySubmitResult::Unavailable;
    // Every verified base-body material is skin/hair. Concrete private texture
    // IDs bypass legacy sentinels, so suppress this pass before resource loading.
    if (hideSkin || owner->Alpha <= 0.01f) return BaseBodySubmitResult::Skipped;
    if (!owner->BoneTransform || boneCount < kBaseClassBodies[part].boneCount
        || !EnsureClassBodyModel(sourceClassByte, part, filter, wrap))
        return BaseBodySubmitResult::Unavailable;
    BMD& body = Models[ClassBodyModelId(part, MAX_MODELS)];
    body.HideSkin = false;
    body.BodyScale = owner->Scale;
    body.ContrastEnable = owner->ContrastEnable;
    body.LightEnable = owner->LightEnable;
    VectorCopy(owner->Position, body.BodyOrigin);
    VectorCopy(light, body.BodyLight);
    // Ordinary RenderPartObject resets BoneScale to1. Preserve the caller's
    // global value, and do not enter its item/Monk/cloth/selection dispatch.
    struct BoneScaleScope
    {
        float previous;
        BoneScaleScope() : previous(BoneScale) { BoneScale = 1.0f; }
        ~BoneScaleScope() { BoneScale = previous; }
    } scaleScope;
    body.Transform(owner->BoneTransform, owner->BoundingBoxMin, owner->BoundingBoxMax,
        &owner->OBB, translate);
    // Matches generic native base-body pass: no owner HiddenMesh override.
    // BMD::RenderBody keeps its native mesh scripts and synchronous shader flush.
    body.RenderBody(RENDER_TEXTURE, owner->Alpha, owner->BlendMesh, owner->BlendMeshLight,
        owner->BlendMeshTexCoordU, owner->BlendMeshTexCoordV);
    return BaseBodySubmitResult::Submitted;
}

bool EnsureModel(int modelId)
{
    if (!Models || !IsVisibleModel(modelId))
        return false;
    BMD& model = Models[modelId];
    if (model.NumMeshs > 0 && model.NumBones > 0 && model.NumActions > 0)
    {
#ifdef jdk_shader_local330
        // Stock ConvertOldMeshToVaoMesh returns for modelId >= MAX_MODELS.
        // These are only the package's registered private BMDs, and can have
        // been opened before shader initialization. Reuse native BMD upload
        // once the real GL/shader context exists; leave SS6 converter intact.
        if (OGL330::IsShader() && wglGetCurrentContext() && !model.NewMeshes)
        {
            model.LoadMeshToVAO();
            model.UploadAllToGPU();
        }
#endif
        return true;
    }
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
    // The stock converter's MAX_MODELS guard excludes every private Grow
    // Lancer BMD. Follow the already-native class-body Open2/VAO adapter,
    // scoped here instead of widening the global SS6 shader model limit.
    if (OGL330::IsShader() && wglGetCurrentContext())
    {
        model.LoadMeshToVAO();
        model.UploadAllToGPU();
    }
#endif
    return model.NumMeshs > 0 && model.NumBones > 0 && model.NumActions > 0;
}

bool IsVisibleModel(int modelId)
{
    return (modelId >= kFirstModel && modelId <= kLastVisibleModel) ||
        modelId == kBrecheOwnerWindModel;
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
        {kBrecheTwilight01Bitmap, "Data\\RISE\\GrowLancer\\Breche\\twlighthik01.jpg"},
        {kBrecheOwnerFireRingBitmap, "Data\\RISE\\GrowLancer\\Breche\\Owner\\firering01.jpg"},
        {kBrecheOwnerRingBitmap, "Data\\RISE\\GrowLancer\\Breche\\Owner\\ring_of_gradation.jpg"}
    };
    for (int i = 0; i < static_cast<int>(sizeof(rows) / sizeof(rows[0])); ++i)
    {
        if (!Bitmaps.FindTexture(rows[i].id) &&
            !Bitmaps.LoadImageFile(rows[i].id, rows[i].path, GL_LINEAR, GL_CLAMP))
            return false;
    }
    return true;
}

bool EnsureWrathBuffAtlas()
{
    const char* path = "Data\\RISE\\GrowLancer\\Wrath\\WrathBuffAtlas.tga";
    BITMAP_t* bitmap = Bitmaps.FindTexture(kWrathBuffAtlasBitmap);
    if (bitmap && _stricmp(bitmap->FileName, path) != 0) return false;
    if (!bitmap && !Bitmaps.LoadImageFile(kWrathBuffAtlasBitmap, path, GL_LINEAR, GL_CLAMP)) return false;
    bitmap = Bitmaps.FindTexture(kWrathBuffAtlasBitmap);
    return bitmap && bitmap->Components == 4 && bitmap->Width == 1024.f && bitmap->Height == 256.f;
}

void ReleaseWrathBuffAtlas()
{
    BITMAP_t* bitmap = Bitmaps.FindTexture(kWrathBuffAtlasBitmap);
    if (bitmap && _stricmp(bitmap->FileName, "Data\\RISE\\GrowLancer\\Wrath\\WrathBuffAtlas.tga") == 0)
        DeleteBitmap(kWrathBuffAtlasBitmap);
}

bool EnsureWrathPersistentBitmaps()
{
    // Private IDs and sampler: never replace an existing unrelated bitmap.
    // Missing persistent assets do not disable other Grow Lancer skills.
    const BitmapRow rows[] = {
        {kWrathMono01Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\firehik_mono01.jpg"},
        {kWrathMono02Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\firehik_mono02.jpg"},
        {kWrathMono03Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\firehik_mono03.jpg"}
    };
    for (int i = 0; i < static_cast<int>(sizeof(rows) / sizeof(rows[0])); ++i)
    {
        BITMAP_t* bitmap = Bitmaps.FindTexture(rows[i].id);
        if (bitmap && _stricmp(bitmap->FileName, rows[i].path) != 0)
            return false;
        if (!bitmap && !Bitmaps.LoadImageFile(rows[i].id, rows[i].path, GL_LINEAR, GL_CLAMP))
            return false;
        bitmap = Bitmaps.FindTexture(rows[i].id);
        if (!bitmap || bitmap->Components != 3 || bitmap->Width != 64.f || bitmap->Height != 64.f)
            return false;
    }
    return true;
}

bool EnsureCirclePersistentBitmap()
{
    // S21 0x18BD7EC registers 0x8086 with GL_LINEAR/GL_CLAMP_TO_EDGE.
    // Keep Circle's staged asset and native slot separate from Wrath.
    const BitmapRow row = {kCircleUpperArmMonoBitmap,
        "Data\\RISE\\GrowLancer\\CircleShield\\firehik_mono01.jpg"};
    BITMAP_t* bitmap = Bitmaps.FindTexture(row.id);
    if (bitmap && _stricmp(bitmap->FileName, row.path) != 0) return false;
    if (!bitmap && !Bitmaps.LoadImageFile(row.id, row.path,
        GL_LINEAR, GL_CLAMP_TO_EDGE)) return false;
    bitmap = Bitmaps.FindTexture(row.id);
    return bitmap && bitmap->Components == 3 && bitmap->Width == 64.f &&
        bitmap->Height == 64.f;
}

bool EnsureSpinMotionBlurBitmap()
{
    // S21 registration18BDDB8..18BDDD5: motion_blur ID0x7F08,
    // min/mag GL_NEAREST (0x2600), wrap GL_CLAMP (0x2900). This is a
    // private 5.2 registration, not a replacement of SS6 BITMAP_BLUR+1.
    const BitmapRow row = {kSpinMotionBlurBitmap,
        "Data\\RISE\\GrowLancer\\SpinStep\\motion_blur.jpg"};
    BITMAP_t* bitmap = Bitmaps.FindTexture(row.id);
    if (bitmap && _stricmp(bitmap->FileName, row.path) != 0) return false;
    if (!bitmap && !Bitmaps.LoadImageFile(row.id, row.path,
        GL_NEAREST, GL_CLAMP)) return false;
    bitmap = Bitmaps.FindTexture(row.id);
    return bitmap && bitmap->Components == 3 && bitmap->Width > 0.f &&
        bitmap->Height > 0.f;
}

bool EnsureWrathScatterBitmaps()
{
    const BitmapRow rows[] = {
        {kWrathScatter01Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\lighting_mega01.jpg"},
        {kWrathScatter02Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\lighting_mega02.jpg"},
        {kWrathScatter03Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\lighting_mega03.jpg"}
    };
    for (int i = 0; i < 3; ++i)
    {
        BITMAP_t* bitmap = Bitmaps.FindTexture(rows[i].id);
        if (bitmap && _stricmp(bitmap->FileName, rows[i].path) != 0) return false;
        // S21 scatter uses REPEAT, unlike the mono fire's CLAMP.
        if (!bitmap && !Bitmaps.LoadImageFile(rows[i].id, rows[i].path, GL_LINEAR, GL_REPEAT)) return false;
        bitmap = Bitmaps.FindTexture(rows[i].id);
        if (!bitmap || bitmap->Components != 3 || bitmap->Width != 128.f || bitmap->Height != 128.f) return false;
    }
    return true;
}

bool EnsureWrathGroundSpriteBitmaps()
{
    const BitmapRow rows[] = {
        {kWrathLightmarksBitmap, "Data\\RISE\\GrowLancer\\Wrath\\lightmarks.jpg"},
        {kWrathFlare01Bitmap, "Data\\RISE\\GrowLancer\\Wrath\\flare01.jpg"},
        {kWrathFlareBlueBitmap, "Data\\RISE\\GrowLancer\\Wrath\\flareBlue.jpg"},
        {kWrathShockwaveBitmap, "Data\\RISE\\GrowLancer\\Wrath\\Shockwave2.jpg"}
    };
    const float sizes[] = {128.f,64.f,64.f,256.f};
    for (int i = 0; i < 4; ++i)
    {
        BITMAP_t* bitmap = Bitmaps.FindTexture(rows[i].id);
        if (bitmap && _stricmp(bitmap->FileName, rows[i].path) != 0) return false;
        if (!bitmap && !Bitmaps.LoadImageFile(rows[i].id, rows[i].path, GL_LINEAR, GL_CLAMP)) return false;
        bitmap = Bitmaps.FindTexture(rows[i].id);
        if (!bitmap || bitmap->Components != 3 || bitmap->Width != sizes[i] || bitmap->Height != sizes[i]) return false;
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
    static_assert(kSkillAttributeIdCapacity > kWrathMasterSkillId,
        "The source-pinned S21 master ID must fit the isolated attribute lookup");
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
    // Skill.bmd remains the original 650-record SS6 format. This one high-ID
    // row comes from pinned S21 SkillList.xml + active third tree, not from
    // an out-of-bounds read or an inferred adjacent client skill record.
    // The native seven-class requirement bytes stay zero and GS cast stays
    // fail-closed until real class7/server behavior is ported.
    SKILL_ATTRIBUTE& master = SkillAttribute[kWrathMasterSkillId];
    ZeroMemory(&master, sizeof(master));
    strcpy_s(master.Name, "Wrath Strengthener");
    master.Level = 66;
    master.Damage = 312;
    master.Mana = 50;
    master.AbilityGuage = 40;
    master.Distance = 0;
    master.Delay = 0;
    master.Strength = 200;
    master.Dexterity = 200;
    master.MasteryType = 255;
    master.SkillUseType = SKILL_USE_TYPE_MASTERACTIVE;
    master.SkillBrand = kWrathSkill;
    master.SkillRank = 4;
    master.SkillGroup = kWrathMasterSkillGroup;
    master.Magic_Icon = kWrathMasterIconNumber;
}

}}
