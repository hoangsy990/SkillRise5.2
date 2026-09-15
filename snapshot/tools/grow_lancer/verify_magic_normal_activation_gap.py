"""Fail if Magic's QA cadence is mistaken for a normal-client activation."""

from pathlib import Path


root = Path(__file__).resolve().parents[2]
effect = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
character = (root / "ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp").read_text(encoding="utf-8")
receive = (root / "ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp").read_text(encoding="utf-8")
scene = (root / "ExMain_RISE_PC/Main5.2_RISE/ZzzScene.cpp").read_text(encoding="utf-8")


def body(source: str, signature: str, next_signature: str) -> str:
    return source.split(signature, 1)[1].split(next_signature, 1)[0]


prepare = body(effect, "bool PrepareLocalQADynamicAction(", "void TransformFromObject(")
assert "#ifdef RISE_GROW_LANCER_RUNTIME_QA" in prepare
assert "GetBaseClass(CharacterAttribute->Class) != CLASS_KNIGHT" in prepare
assert "return false;" in prepare.split("#else", 1)[1]

begin = body(effect, "void BeginMagicPinFrame()", "bool MagicPinFrameOwnsActor(")
assert "RISE_GL_MAGIC_FIXED_TICK_QA" in begin
assert "#ifdef RISE_GROW_LANCER_RUNTIME_QA" in begin
assert "if (!enabled) return;" in begin

quanta = body(effect, "unsigned RunMagicPinQuanta(", "void StepMagicPinModels(")
assert "#ifdef RISE_GROW_LANCER_RUNTIME_QA" in quanta
assert "return 0;" in quanta.split("#else", 1)[1]

actor = body(character, "bool rise::growlancer::StepMagicPinActor(", "void CreateWeaponBlur(")
assert "#ifdef RISE_GROW_LANCER_RUNTIME_QA" in actor
assert "return false;" in actor.split("#else", 1)[1]

root_cast = body(effect, "bool CreateMagicPinRoots(", "void CreateMagicPinHit(")
assert root_cast.index("PrepareLocalQADynamicAction(caster, 287)") < root_cast.index("SetAction(&caster, 287, true)")
assert "case rise::growlancer::kMagicPinSkill:" in receive
assert "CreateMagicPinRoots(*so)" in receive
magic_scene = scene.split("BeginMagicPinFrame();", 1)[1].split("EndMagicPinFrame();", 1)[0]
assert magic_scene.index("MoveCharactersClient();") < magic_scene.index("RunMagicPinFrame();")
assert magic_scene.index("RunMagicPinFrame();") < magic_scene.index("MoveEffects();")

print("PASS normal client has a receive edge but dynamic action and 40 ms producer are QA-only/fail-closed")
print("OPEN class7/stat/owner contract and ordinary-client Magic animation/child timing; QA fixture is not gameplay parity")
