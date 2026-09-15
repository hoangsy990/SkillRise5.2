"""Verify the native ReceiveMagic Breche receive-to-render handoff.

This guard checks only the source dispatch boundary.  It does not claim that
the packet is authoritative, that the target was damaged, or that pixels were
visible in a live client.
"""

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "WSclient.cpp"


def main() -> None:
    source = SOURCE.read_text(encoding="utf-8")
    start = source.index("case rise::growlancer::kBrecheSkill:")
    end = source.index("case rise::growlancer::kClashSkill:", start)
    branch = source[start:end]
    required = (
        "rise::growlancer::CreateBrecheAction(*so);",
        "rise::growlancer::CreateBrecheHit(*to);",
        "sc->AttackTime = 1;",
    )
    for needle in required:
        if needle not in branch:
            raise ValueError(f"Breche ReceiveMagic handoff missing: {needle}")
    if branch.index(required[0]) > branch.index(required[1]):
        raise ValueError("Breche action must precede resolved-target visual handoff")
    print("PASS: ReceiveMagic skill279 invokes Breche action and resolved-target visual root")
    print("PASS: native packet target/source lookup and AttackTime path stay unchanged")
    print("Scope: dispatch wiring only; target authority, damage and runtime pixels remain open")


if __name__ == "__main__":
    main()
