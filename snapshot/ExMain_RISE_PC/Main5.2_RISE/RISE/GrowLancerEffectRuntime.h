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
// Private animation-sample state only; do not mutate OBJECT/pool ownership.
bool ResetEffectAnimationSamples(OBJECT* effect);
void ResetAllEffectAnimationSamples();
void RetireCharacterAnimationSamples(const OBJECT* actor);
// Observational native-frame publication, NOT a source-quantized effect tick.
void PublishCharacterAnimationObservation(const OBJECT& actor);
bool ReadCharacterAnimationObservation(const OBJECT& controller,
    unsigned short& action, float& frame);
void UpdateEffect(OBJECT& effect, float animationFactor);
// One quantum over primary native slots; coordinated scheduler only.
void StepMagicPinModels();
// Local QA actor only. Returns whether a quantum was advanced, including exit.
bool StepMagicPinActor(OBJECT& actor);
// QA-only batch; returns completed quanta. Not a wall-clock or gameplay hook.
unsigned RunMagicPinQuanta(OBJECT& actor, unsigned ticks);
void BeginMagicPinFrame();
void RunMagicPinFrame();
void EndMagicPinFrame();
bool MagicPinFrameOwnsActor(const OBJECT& actor);
bool MagicPinFrameOwnsModel(const OBJECT& effect);
bool MagicPinFrameOwnsParticles();
bool RenderEffect(OBJECT& effect);
#ifdef RISE_GROW_LANCER_RUNTIME_QA
void RecordSpinRenderQA(const OBJECT& effect, const char* stage, int result,
    int meshCount, int boneCount, int actionCount, int textureId);
#endif

// SS21 render flag 0x200000 rejects a whole triangle only when the supplied
// V threshold is strictly above all three transformed vertex Z values.  This
// adapter temporarily filters the native mesh and still delegates the draw to
// the stock RISE BMD::RenderMesh path.
bool RenderBrecheClippedMesh(BMD& model, int meshIndex, int renderFlags,
    float alpha, int blendMeshIndex, float blendMeshLight,
    float blendMeshTextureCoordU, float zThreshold, int textureIndex);

void CreateClashRoot(OBJECT& caster, short targetIndex);
void CreateObsidianRoots(OBJECT& caster);
// True means action/model prerequisites passed and root creation was called;
// native effect-pool allocation and pixels remain separate QA gates.
bool CreateSpinStepRoot(OBJECT& caster, short targetIndex);
void CreateSpinStepHit(OBJECT& target);
bool CreateMagicPinRoots(OBJECT& caster);
void CreateMagicPinHit(OBJECT& target);
void CreateHarshStrikeRoot(OBJECT& caster);
void CreateShiningPeakRoots(OBJECT& caster);
void CreateWrathRoot(OBJECT& caster);
// Render-stage only. Caller establishes verified buff membership and clock sample.
bool RenderWrathPersistentGround(const OBJECT& caster, float sampledSourceClock);
// Separate source groups preserve intervening terrain/scatter order.
// Return means prerequisites valid, not all native sprite allocations succeeded.
bool SubmitWrathPersistentSprites(OBJECT& caster, bool boneFlareGroup);
// Checks native membership424/425 itself; caller must establish character render
// stage/visibility eligibility. This function never grants a buff or starts a timer.
// True means prerequisites accepted, not that every native pool allocation succeeded.
bool SubmitWrathPersistentVisuals(OBJECT& caster, float sampledSourceClock);
void CreateCircleShieldRoot(OBJECT& caster);
// Native character-slot reuse safety; caller must invoke before reactivation.
void RetireCircleCharacterEffects(OBJECT& retiredCharacter);
// Buff-contact receiver order: first packet actor, second packet actor.
// Visual only; caller must resolve and authorize the incoming event.
void CreateCircleShieldContact(OBJECT& firstActor, OBJECT& secondActor);
void CreateBrecheAction(OBJECT& caster);
// Receive/contact owner is the resolved target, not the casting actor.
void CreateBrecheHit(OBJECT& target);

}}
