"""Pin Spin Step's separate cast 271 and target-contact 280 visual triggers."""

import hashlib
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RUNTIME = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text(
    encoding="utf-8")
QA = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(
    encoding="utf-8")
RECEIVER = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp").read_text(
    encoding="utf-8")
REFERENCE = ROOT / "artifacts/runtime_qa/s21_reference/batch_20260912_2136/frame_00041.jpg"
REFERENCE_SHA = "3FFD5023C9D7D7CA068EAB44E05C582C82BFECE913531BB097BDE18A99C9AE4D"


def main() -> None:
    assert hashlib.sha256(REFERENCE.read_bytes()).hexdigest().upper() == REFERENCE_SHA
    receiver = RECEIVER.split("case rise::growlancer::kSpinStepSkill:", 1)[1].split(
        "case rise::growlancer::kCircleShieldSkill:", 1)[0]
    assert receiver.index("CreateSpinStepRoot(") < receiver.index(
        "case rise::growlancer::kSpinStepExplosionSkill:")
    assert receiver.index("case rise::growlancer::kSpinStepExplosionSkill:") < receiver.index(
        "CreateSpinStepHit(")
    qa_cast = QA.split("void CastCurrentSkill()", 1)[1].split(
        "void EmitCurrentContact()", 1)[0]
    qa_contact = QA.split("void EmitCurrentContact()", 1)[1].split(
        "int Run", 1)[0]
    assert "case 271: actionPrepared = CreateSpinStepRoot(" in qa_cast
    assert "CreateSpinStepHit(" not in qa_cast
    assert "if (skill == 271)" in qa_contact and "CreateSpinStepHit(*target)" in qa_contact
    root = RUNTIME.split("bool CreateSpinStepRoot(", 1)[1].split(
        "void CreateSpinStepHit(", 1)[0]
    hit = RUNTIME.split("void CreateSpinStepHit(", 1)[1].split(
        "bool CreateMagicPinRoots(", 1)[0]
    assert "kSpinStepSkill" in root and "kSpinStepExplosionSkill" not in root
    assert "kSpinStepExplosionSkill" in hit and "kSpinStepSkill" not in hit
    print("PASS: pinned S21 reference and distinct 271 cast / 280 contact receive branches")
    print("QA: F7 plus separate F9 covers both visuals; no GameServer damage or visual PASS")


if __name__ == "__main__":
    main()
