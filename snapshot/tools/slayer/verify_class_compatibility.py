#!/usr/bin/env python3
"""Verify the isolated Slayer class marker/DB compatibility surface.

This is a source-wiring check only.  It does not claim that character creation,
gear/body assets, or owner in-game acceptance are complete.
"""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


def read(relative: str) -> str:
    path = ROOT / relative
    if not path.is_file():
        raise FileNotFoundError(path)
    return path.read_text(encoding="utf-8", errors="replace")


def require(text: str, needle: str, label: str) -> None:
    if needle not in text:
        raise AssertionError(f"missing {label}: {needle}")


def main() -> int:
    shared = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerSkillContractData.h")
    catalog = read("ExGameServer/GameServer/RISE/SlayerServerCatalog.h")
    runtime = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/client/SlayerNativeRuntime.cpp")
    skill_manager = read("ExGameServer/GameServer/SkillManager.cpp")

    require(shared, "inline bool IsSlayerClientClass", "client marker helper")
    require(shared, "return (classId & 0x7) == kRise52ReservedClassSlot;", "reserved client marker")
    for value in ("kS21SlayerDbClass = 144", "kS21RoyalSlayerDbClass = 145", "kS21MasterSlayerDbClass = 146"):
        require(shared, value, f"shared {value}")
    for value in ("kS21SlayerDbClass = 144", "kS21RoyalSlayerDbClass = 145", "kS21MasterSlayerDbClass = 146", "kSlayerClientClassByte = 0xE0"):
        require(catalog, value, f"server {value}")
    require(catalog, "LegacyArrayClassForDbClass", "safe legacy array mapping")
    require(skill_manager, "IsSlayerDbClass(lpObj->DBClass)", "DB class skill gate")
    require(runtime, "IsSlayerClientClass(source->Class)", "client marker native gate")

    print("PASS: Slayer class compatibility marker, DB identities, safe array mapping, and gates are wired")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
