#pragma once
class OBJECT;

namespace rise { namespace growlancer {

#ifdef RISE_GROW_LANCER_RUNTIME_QA
// -1 means disabled; otherwise offline probe exit status (no game connection).
int RunLanceLoadProbeQA();
int RunBodyLoadProbeQA();
int RunBodyTextureProbeQA();
void RecordShiningQASample(const OBJECT& controller, float factor);
void RecordControllerQASample(const OBJECT& controller, float factor);
void RecordWrathGroundQA(const OBJECT& effect);
// Opt-in, bounded readback around the actual native Wrath terrain draw.
// It observes fragments and five projected pixels; never changes blend/mesh.
unsigned BeginWrathGroundPixelsQA(const OBJECT& effect);
void EndWrathGroundPixelsQA(unsigned token);
// Bounded owner-only telemetry for the post-buff Wrath visual sequence. This
// is diagnostic: it never enables the buff or changes the production path.
void RecordWrathBuffVisualQA(const OBJECT& caster, const char* stage,
    int variant, int attachment, int bone);
void RecordMagicFrameQA(bool owned, bool ran, unsigned long long before,
    unsigned long long after, unsigned long long completed);
// outcome0=target rejected,1=target resolved,2=native initialization reached.
void RecordSpinOutcomeQA(const OBJECT& effect, int outcome, int targetIndex);
// Render-gate telemetry for the selected-target Spin Step fixture. This is
// diagnostic only: it records culling/resource/transform/submission outcomes
// and never changes the native render path.
void RecordSpinRenderQA(const OBJECT& effect, const char* stage, int result,
    int meshCount, int boneCount, int actionCount, int textureId);
// Opt-in, 12-draw native gameplay query for only the target-owned Spin cross.
// Shader queue is flushed inside the query; samples are diagnostic, not pixels.
unsigned BeginSpinCrossSamplesQA(const OBJECT& effect);
void EndSpinCrossSamplesQA(const OBJECT& effect, unsigned query);
// Actual native object-blur draw-path telemetry (not an allocator count).
// Bounded per explicitly armed Spin Step F7 cast; production path is absent.
void RecordSpinBlurDrawQA(const OBJECT* owner, int segments, int life,
    int textureId, const float* head1, const float* head2,
    const float* tail1, const float* tail2);
// Render-gate telemetry for private Magic Pin/Clash/Breche fixtures. This
// records native submission state only; it never reads back pixels or changes
// the renderer/pool path.
void RecordTargetSkillRenderQA(const OBJECT& effect, int skill,
    const char* stage, int result, int meshCount, int boneCount,
    int actionCount, int textureId);
// Once-per-private-model material probe: exact BMD filename versus bitmap
// cache entry and current GL allocation. No texture rebinding persists.
void RecordMagicMaterialQA(int modelType, unsigned textureId,
    const char* bmdTextureName);
// Explicit opt-in GL_SAMPLES_PASSED probe for two source-proven foot models.
// Query state is restored; zero/positive samples do not prove visual parity.
unsigned BeginMagicPinFootSamplesQA(const OBJECT& effect);
void EndMagicPinFootSamplesQA(const OBJECT& effect, unsigned query);
// Owner-only Breche cast telemetry at the native character render boundary.
// This observes action/body submission; it never creates a visual or changes
// RenderCharacter's production path.
void ResetBrecheCasterRenderQA();
void RecordBrecheCasterRenderQA(const OBJECT& actor, const char* stage,
    int bodyPart, int modelType, int meshCount, int boneCount,
    int actionCount, int result);
void RecordBrecheOwnerResourceGateQA(bool resourcesReady, int targetIndex,
    bool controllerCreated);
// QA-only diagnostic for the native animation completion boundary. It records
// why an imported owner action reached PlayerStopAnimationSetting; it never
// changes the action or renderer.
void RecordBrecheActionStopQA(const OBJECT& actor, int skill, int attackTime,
    int numAnimationKeys, float playSpeed, bool playResult);
// QA-only trace around the native animation-stop boundary. This identifies
// the exact before/after action values without altering production behavior.
void RecordBrecheActionBoundaryQA(const OBJECT& actor, const char* stage,
    unsigned short before, unsigned short after, bool playResult);
// Read-only pool snapshot around native map teardown. No allocation, target
// mutation or server packet is performed; this exists only in RuntimeQA.
void RecordMapEffectTeardownQA(const char* phase);
bool HandleRuntimeQAHotKey();
// F7 Circle Shield: a three-second owner-only visual probe. It does not
// register any buff, send a packet, or stand in for server authority.
bool IsCirclePersistentPreviewQA(const OBJECT& actor);
// Bounded owner-only observation of the native post-buff traversal. A failed
// allocator result is evidence, not permission to bypass the SS6 pool/budget.
void RecordCirclePersistentEmissionQA(const OBJECT& actor, bool ready,
    int boneCount, int monoCreated);
bool IsRuntimeQAIconPanelVisible();
int RuntimeQASelectedSkillId();
const char* RuntimeQASelectedSkillName();
int RuntimeQASelectedTargetIndex();
bool RuntimeQASelectedTargetLive();
#endif

}}
