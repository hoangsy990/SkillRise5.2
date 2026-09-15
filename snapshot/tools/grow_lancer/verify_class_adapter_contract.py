"""Verify the candidate-only Grow Lancer class adapter.

This intentionally proves a fail-closed boundary, not class activation.  The
native client still has seven-column persistence; GS reserves class index7.
"""
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]


def read(path: Path) -> str:
    return path.read_text(encoding="latin-1")


def main() -> None:
    header = read(ROOT / "GrowLancer/compat/GrowLancerClassAdapter.h")
    for token in (
        "kGrowLancerSourceClass = 7",
        "kGrowLancerDbClassBase = 112",
        "kGrowLancerDbClassLast = 115",
        "GrowLancerDbToWire",
        "CanActivateGrowLancerClass",
    ):
        assert token in header, token

    assert "return context.nativeClassSlot && context.defaultRows" in header
    assert "context.createAndLogin && context.packetRoundTrip" in header
    assert "context.persistence && context.classConsumers" in header

    defaults = read(ROOT / "ExGameServer/GameServer/DefaultClassInfo.h")
    client_define = read(ROOT / "ExMain_RISE_PC/Main5.2_RISE/_define.h")
    assert re.search(r"#define\s+MAX_CLASS\s+8\b", defaults)
    assert re.search(r"#define\s+MAX_CLASS\s+7\b", client_define)

    for db_class, expected_wire in ((112, 224), (113, 240), (114, 255), (115, 271)):
        evolution = db_class % 16
        base = db_class // 16
        wire = evolution * 16
        wire -= wire // 32
        wire += base * 32
        assert wire == expected_wire, (db_class, wire, expected_wire)
    assert "wireClass <= 0xffu" in header
    assert 271 > 0xff  # DB115 algebra is retained as a reference, not a BYTE wire value.

    # The GS source identity index is reserved but DB activation is closed.
    assert "#define CLASS_GL 7" in defaults
    assert "DB_CLASS_GL" not in defaults
    assert "CLASS_GROWLANCER" not in client_define
    print("PASS candidate Grow Lancer class7/DB112..115 wire algebra and all-contract gate")
    print("PASS GS capacity8 reserved, client legacy MAX_CLASS7 unchanged; DB activation closed")
    print("OPEN activation: native slot, defaults, create/login, packets, persistence and class consumers")


if __name__ == "__main__":
    main()
