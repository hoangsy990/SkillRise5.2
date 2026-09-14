#!/usr/bin/env python3
"""Check that every imported Slayer id has a complete 5.2 source surface.

This is a static wiring check only.  It does not claim that a legacy character
class has been migrated or that an owner has accepted the visuals in-game.
"""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SKILLS = {
    292: ("kSwordInertia", "SkillSlayerSwordInertia"),
    293: ("kBatFlock", "SkillSlayerBatFlock"),
    294: ("kPierceAttack", "SkillSlayerPierceAttack"),
    295: ("kDetection", "SkillSlayerDetection"),
    297: ("kDemolish", "SkillSlayerDemolish"),
}


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
    server_catalog = read("ExGameServer/GameServer/RISE/SlayerServerCatalog.h")
    server = read("ExGameServer/GameServer/SkillManager.cpp")
    client_receive = read("ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp")
    client_use = read("ExMain_RISE_PC/Main5.2_RISE/ZzzInterface.cpp")
    runtime = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/client/SlayerNativeRuntime.cpp")
    resources = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/client/SlayerSkillResources.cpp")
    packet = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/server/SlayerPacketContract.h")

    for skill_id, (name, handler) in SKILLS.items():
        enum_name = name
        require(shared, f"{name} = {skill_id}", f"shared id {skill_id}")
        require(server_catalog, f"{{ {enum_name},", f"server catalog row {skill_id}")
        require(server, f"case rise::slayerserver::{name}:", f"server dispatch {skill_id}")
        require(server, handler, f"server handler {skill_id}")
        require(client_receive, f"case {skill_id}:", f"client receive case {skill_id}")
        require(client_use, f"rise::slayer::{name}", f"client use route {skill_id}")
        require(runtime, f"case {enum_name}:", f"native runtime case {skill_id}")
        require(resources, f"case {enum_name}:", f"resource case {skill_id}")
        require(packet, f"{{ {name},", f"packet row {skill_id}")
        print(f"PASS: skill={skill_id} name={name[1:]} source-surface=shared/server/receive/use/runtime/resources/packet")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
