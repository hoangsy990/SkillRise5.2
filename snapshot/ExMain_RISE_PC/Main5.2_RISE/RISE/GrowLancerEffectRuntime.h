#pragma once

class OBJECT;
class BMD;

namespace rise { namespace growlancer {

// Source185..194 are ordinary completed casts at1423159 ->140AF0E.
constexpr bool IsImportedCastAction(unsigned short action)
{
    return action >= 284 && action <= 293;
}

bool IsEffectType(int type);
bool IsBrecheEffectType(int type);
bool IsSpinFlareEffect(const OBJECT& effect);
bool IsSpinGroundEffect(const OBJECT& effect);
bool IsCircleShinyEffect(const OBJECT& effect);
void InitializeEffect(OBJECT& effect);
void UpdateEffect(OBJECT& effect, float animationFactor);
bool RenderEffect(OBJECT& effect);

// SS21 render flag 0x200000 rejects a whole triangle only when the supplied
// V threshold is strictly above all three transformed vertex Z values.  This
// adapter temporarily filters the native mesh and still delegates the draw to
// the stock RISE BMD::RenderMesh path.
bool RenderBrecheClippedMesh(BMD& model, int meshIndex, int renderFlags,
    float alpha, int blendMeshIndex, float blendMeshLight,
    float blendMeshTextureCoordU, float zThreshold, int textureIndex);

void CreateClashRoot(OBJECT& caster, short targetIndex);
void CreateObsidianRoots(OBJECT& caster);
void CreateSpinStepRoot(OBJECT& caster, short targetIndex);
void CreateSpinStepHit(OBJECT& target);
void CreateMagicPinRoots(OBJECT& caster);
void CreateMagicPinHit(OBJECT& target);
void CreateHarshStrikeRoot(OBJECT& caster);
void CreateShiningPeakRoots(OBJECT& caster);
void CreateWrathRoot(OBJECT& caster);
void CreateCircleShieldRoot(OBJECT& caster);
// Buff-contact receiver order: first packet actor, second packet actor.
// Visual only; caller must resolve and authorize the incoming event.
void CreateCircleShieldContact(OBJECT& firstActor, OBJECT& secondActor);
void CreateBrecheAction(OBJECT& caster);
// Receive/contact owner is the resolved target, not the casting actor.
void CreateBrecheHit(OBJECT& target);

}}
