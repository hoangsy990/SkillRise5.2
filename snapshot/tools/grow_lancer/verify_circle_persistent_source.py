#!/usr/bin/env python3
"""Pin the Circle Shield buff216/221/222 shoulder emitter and native adapter."""

from collections import Counter
from hashlib import sha256
from pathlib import Path
import struct

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_s21_payload, parse_s21_container,
)
from inspect_equipment_meshes import inspect
from inspect_magic_pin_model_geometry import animation_summary, mesh_bounds


ROOT = Path(__file__).resolve().parents[2]
IMAGE = Path(r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
             r"\memory_dump\process_23824\400000.main.exe")
IMAGE_SHA = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
BODY = Path(r"D:\MU FICA Season 21\Data\Player\HelmClass08.bmd")
BODY_SHA = "09128F0B627D718967FCCD14A3AD30ECAB86659C4A7317EF4A3B8F824951D668"
MONO = Path(r"D:\MU FICA Season 21\Data\Effect\firehik_mono01.OZJ")
STAGED = (ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" /
          "Client" / "Data" / "RISE" / "GrowLancer" / "CircleShield" /
          "firehik_mono01.OZJ")
MONO_SHA = "C3E18C474BA5AE9F06B9E7159AFC0DAB34869B78EAB425CB5538B94FE0E7138F"


def require(condition: bool, message: str) -> None:
    if not condition:
        raise AssertionError(message)


def main() -> None:
    image = IMAGE.read_bytes()
    require(sha256(image).hexdigest().upper() == IMAGE_SHA, "S21 image hash drift")
    start, end = 0x13ECB52, 0x13ECFBC
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    code = list(decoder.disasm(image[start - 0x400000:end - 0x400000], start))
    require(code and code[-1].address + code[-1].size == end,
            "Circle persistent block incomplete")
    pushes = [i.op_str for i in code if i.mnemonic == "push"]
    calls = Counter(i.op_str for i in code if i.mnemonic == "call")
    require(all(pushes.count(hex(b)) == 1 for b in (216, 221, 222)),
            "buff216/221/222 status gates drift")
    require(calls["0x130c8a5"] == 3 and calls["0x132ec63"] == 5 and
            calls["0x172760a"] == 5 and calls["0x1724176"] == 4,
            "status/bone/sprite/particle fanout drift")
    require(pushes.count("0x7ef7") == 5 and pushes.count("0x8086") == 4,
            "flare/mono resource fanout drift")
    # Each helper call's closest preceding PUSH is its native bone index.
    bone_calls = []
    for i, ins in enumerate(code):
        if ins.mnemonic == "call" and ins.op_str == "0x132ec63":
            preceding = next(x for x in reversed(code[max(0, i - 9):i])
                             if x.mnemonic == "push" and x.op_str not in ("eax",))
            bone_calls.append(int(preceding.op_str, 0))
    require(bone_calls == [20, 26, 35, 25, 34], f"bone sequence {bone_calls}")

    for addr, expected in {
        0x1B50134: 37.5, 0x1B4DF0C: .4, 0x1B4DF14: .5,
        0x1B4DD48: 1., 0x1B4E474: 2., 0x1B4E4D0: 10.,
        0x1B4E6E0: .3, 0x1B502B0: .38, 0x1B4E998: .6,
        0x1B4E860: 3.,
    }.items():
        actual = struct.unpack_from("<f", image, addr - 0x400000)[0]
        require(abs(actual - expected) < 1e-5, f"float {addr:#x} drift")
    for table, subtype, target in (
        (0x16A9C9D, 12, 0x166844B),
        (0x1723C01, 12, 0x16DB10E),
        (0x16410E8, 12, 0x1635D58),
    ):
        actual = struct.unpack_from("<I", image, table + subtype * 4 - 0x400000)[0]
        require(actual == target, f"mono subtype12 table {table:#x} drift")
    require(sha256(BODY.read_bytes()).hexdigest().upper() == BODY_SHA,
            "S21 Grow Lancer body hash drift")
    payload = decrypt_s21_payload(parse_s21_container(BODY.read_bytes()))
    layout = inspect(payload)
    _, offset = mesh_bounds(payload, layout["meshCount"])
    bones = animation_summary(payload, offset, layout["actionCount"],
                              layout["boneCount"])["bones"]
    names = [bones[i]["name"] for i in bone_calls]
    require(names == ["Bip01 Head", "Bip01 R UpperArm",
                      "Bip01 L UpperArm", "Bip01 R Clavicle",
                      "Bip01 L Clavicle"], f"body bone identity drift: {names}")
    require(MONO.is_file() and STAGED.is_file() and
            sha256(MONO.read_bytes()).hexdigest().upper() == MONO_SHA and
            sha256(STAGED.read_bytes()).hexdigest().upper() == MONO_SHA,
            "source/staged mono asset hash drift")
    source = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE")
    character = (source / "ZzzCharacter.cpp").read_text(encoding="utf-8", errors="replace")
    particle = (source / "ZzzEffectParticle.cpp").read_text(encoding="utf-8", errors="replace")
    runtime = (source / "RISE" / "GrowLancerEffectRuntime.cpp").read_text(
        encoding="utf-8", errors="replace")
    resources = (source / "RISE" / "GrowLancerResources.cpp").read_text(
        encoding="utf-8", errors="replace")
    require("SubmitCirclePersistentVisuals(*o)" in character and
            "static_cast<eBuffState>(216)" in character and
            "static_cast<eBuffState>(221)" in character and
            "static_cast<eBuffState>(222)" in character,
            "native status call site missing")
    require("CreateCircleUpperArmParticle" in particle and
            "UpdateCircleUpperArmMonoTick" in particle and
            "SubmitCirclePersistentVisuals" in runtime and
            "kCircleUpperArmMonoBitmap" in resources,
            "native private mono route missing")
    print("PASS S21 Circle buff216/221/222 -> five exact bones/five flares/"
          "four subtype12 mono particles; source/stage/native adapter pinned")
    print("Scope: static/source/build only; buff receipt, GPU pixels and pool QA OPEN")


if __name__ == "__main__":
    main()
