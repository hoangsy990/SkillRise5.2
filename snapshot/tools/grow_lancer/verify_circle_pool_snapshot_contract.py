"""QA-only targeted Circle pool snapshot; source guard, not a live leak test."""

from pathlib import Path

root = Path(__file__).resolve().parents[2]
qa = (root / "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp").read_text(encoding="utf-8")
snapshot = qa.split("void WritePoolSnapshot(const char* reason)", 1)[1].split(
    "OBJECT* SelectedTarget()", 1
)[0]
assert "Particles[i].Live" in snapshot
assert "Particles[i].Type == kCircleUpperArmMonoBitmap" in snapshot
assert "Particles[i].SubType == kCircleMonoSubType" in snapshot
assert "Joints[i].Live" in snapshot
assert "Joints[i].Type == BITMAP_FORCEPILLAR && Joints[i].SubType == 3" in snapshot
assert "circleMono=%d" in snapshot and "circleContact=%d" in snapshot
assert "AppendQALog(line);" in snapshot
assert "CreateParticle" not in snapshot and "CreateJoint" not in snapshot
print("PASS: F11/map QA snapshots count live Circle subtype12 mono and subtype3 contact joints without allocating effects")
print("Scope: QA source contract; repeat/map cleanup and owner visual acceptance remain OPEN")
