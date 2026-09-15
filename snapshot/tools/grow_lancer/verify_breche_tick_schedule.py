"""Guard the Breche whole-tick scheduling boundary in native MoveEffect."""
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzEffect.cpp"


def main():
    source = SOURCE.read_text(encoding="utf-8")
    start = source.index("void MoveEffect(OBJECT* o, int iIndex)")
    end = source.index("vec3_t Light;", start)
    block = source[start:end]
    if "rise::growlancer::IsBrecheEffectType(o->Type)" not in block:
        raise ValueError("Breche types no longer use the private whole-tick path")
    if "rise::growlancer::AdvanceWholeTicks(*o, FPS_ANIMATION_FACTOR," not in block:
        raise ValueError("Breche whole-tick scheduler call changed")
    if block.index("AdvanceWholeTicks") > block.index("if (rise::growlancer::IsEffectType(o->Type))"):
        raise ValueError("Breche scheduler moved after generic fractional path")
    if "UpdateEffect(*o, FPS_ANIMATION_FACTOR)" not in block:
        raise ValueError("Generic effect fallback disappeared; scope may have widened")
    print("PASS: Breche root/children use whole-tick update-before-expiry scheduling")
    print("Scope: MoveEffect source boundary only; pool exhaustion and visual parity remain open")


if __name__ == "__main__":
    main()
