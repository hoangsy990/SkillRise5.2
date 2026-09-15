"""Source-order audit for the temporary DK-only F7 visual fixture."""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
QA = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text()
cast = QA.split("void CastCurrentSkill()", 1)[1].split("void EmitCurrentContact()", 1)[0]
for token in (
    "CharacterAttribute != &CharacterMachine->Character",
    "gCharacterManager.GetBaseClass(sourceClass)",
    "baseClass != CLASS_KNIGHT",
    "event=cast-rejected-class",
    "use a Dark Knight test character",
):
    assert token in cast, token
assert cast.index("baseClass != CLASS_KNIGHT") < cast.index("FlushControllerSamples();")
assert cast.index("event=cast-rejected-class") < cast.index(
    '"action-dispatched-visual-chain-unverified" : "root-dispatched"')
contact = QA.split("void EmitCurrentContact()", 1)[1].split("void ToggleSkillIconQA()", 1)[0]
assert "CreateMagicPinHit(*target);" in contact
assert "event=cast-rejected-class" not in contact
print("PASS QA F7 DK-only fail-closed gate precedes sample arming/root success")
print("PASS separate selected-target F9 preview remains available; no GS claim")
