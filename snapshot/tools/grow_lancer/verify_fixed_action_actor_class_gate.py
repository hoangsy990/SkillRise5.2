"""Static guard for ordinary fixed Grow Lancer actions; not class-login QA."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] /
          "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text()
gate = source.split("bool IsExactGrowLancerBaseActor(", 1)[1].split(
    "bool PrepareFixedPlayerAction(", 1)[0]
fixed = source.split("bool PrepareFixedPlayerAction(", 1)[1].split(
    "bool PrepareLocalQADynamicAction(", 1)[0]

assert source.split("bool CanPlayAction(", 1)[1].split(
    "bool IsExactGrowLancerBaseActor(", 1)[0].rstrip().endswith(
        "#ifndef RISE_GROW_LANCER_RUNTIME_QA")
for token in ("SceneFlag != MAIN_SCENE", "actor.Type != MODEL_PLAYER",
              "!actor.Live", "&Hero->Object == &actor",
              "Hero->Class == kGrowLancerSourceBaseClass", "!CharactersClient",
              "i < MAX_CHARACTERS_CLIENT", "&candidate.Object == &actor",
              "candidate.Object.Live &&", "candidate.Class == kGrowLancerSourceBaseClass"):
    assert token in gate, token
assert gate.index("&Hero->Object == &actor") < gate.index("!CharactersClient")
assert "CharacterAttribute" not in gate
assert '#include "GrowLancerClassIdentity.h"' in source
assert "IsExactGrowLancerBaseActor(actor)" in fixed
assert fixed.index("!CanPlayAction(actor, action)") < fixed.index(
    "IsExactGrowLancerBaseActor(actor)") < fixed.index("S21FixedCastAnimationSpeed(")
assert fixed.count("#ifndef RISE_GROW_LANCER_RUNTIME_QA") == 1
assert "CharacterAttribute" not in fixed
print("PASS ordinary fixed actions require exact owning class7 actor, not local stats")
print("PASS isolated QA retains its DK preview boundary; unknown/evolved classes fail closed")
print("OPEN class7 create/login, dynamic speed, native cast/GS and owner visual QA")
