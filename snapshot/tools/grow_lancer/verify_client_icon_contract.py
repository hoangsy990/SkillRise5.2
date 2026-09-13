#!/usr/bin/env python3
"""Verify pinned S21 Grow Lancer client skill/icon evidence and staging."""

from __future__ import annotations

import hashlib
import struct
from pathlib import Path


ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
S21 = Path(r"D:\MU FICA Season 21\Data")
SKILL_BMD = S21 / "Local" / "skill.bmd"
SKILL_HASH = "AC74B3A175D08B50D8C5C3D5BD8C05420E9F6690BC7A5C6C95EF3D7F8278BF79"
ATLASES = {
    "newui_skill00.OZJ": (492160, "8C3140438BFE4829507A257BBA9591BDDC858C751944086E431F79207B6FAC29"),
    "newui_skill00_Non_Icon.OZJ": (227685, "658EE7BC6CB19F0654AB783FAA22E41031065F6D36E530BBE573BE899BBEEC19"),
}
NAMES = {
    271: "스핀스텝", 272: "서클실드", 273: "옵시디언",
    274: "매직핀", 275: "격돌", 276: "하쉬스트라이크",
    277: "샤이닝픽", 278: "격노", 279: "브레쉬",
}


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def jpeg_size(payload: bytes) -> tuple[int, int]:
    if payload[:2] != b"\xff\xd8":
        raise AssertionError("wrapped payload is not JPEG")
    pos = 2
    while pos + 9 <= len(payload):
        if payload[pos] != 0xFF:
            pos += 1
            continue
        marker = payload[pos + 1]
        pos += 2
        if marker in (0xD8, 0xD9) or 0xD0 <= marker <= 0xD7:
            continue
        length = int.from_bytes(payload[pos:pos + 2], "big")
        if marker in range(0xC0, 0xC4):
            return (int.from_bytes(payload[pos + 5:pos + 7], "big"),
                    int.from_bytes(payload[pos + 3:pos + 5], "big"))
        pos += length
    raise AssertionError("JPEG dimensions not found")


def main() -> None:
    data = SKILL_BMD.read_bytes()
    assert len(data) == 750 * 120 + 4
    assert sha256(SKILL_BMD) == SKILL_HASH
    key = (0xFC, 0xCF, 0xAB)
    for skill_id, expected_name in NAMES.items():
        encrypted = data[skill_id * 120:(skill_id + 1) * 120]
        record = bytes(value ^ key[offset % 3]
                       for offset, value in enumerate(encrypted))
        name = record[:32].split(b"\0", 1)[0].decode("cp949")
        icon_id = struct.unpack_from("<H", record, 74)[0]
        assert name == expected_name, (skill_id, name)
        assert icon_id == skill_id, (skill_id, icon_id)

    stage_roots = (
        ROOT / "Client" / "Data" / "RISE" / "GrowLancer" / "Interface",
        ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" /
        "Client" / "Data" / "RISE" / "GrowLancer" / "Interface",
    )
    for name, (size, expected_hash) in ATLASES.items():
        source = S21 / "Interface" / name
        raw = source.read_bytes()
        assert len(raw) == size
        assert sha256(source) == expected_hash
        assert jpeg_size(raw[24:]) == (512, 512)
        for root in stage_roots:
            staged = root / name
            assert staged.stat().st_size == size
            assert sha256(staged) == expected_hash

    print("PASS: S21 skill.bmd 120-byte records 271..279 and icon IDs")
    print("PASS: exact 512x512 normal/disabled atlases staged and hash-pinned")


if __name__ == "__main__":
    main()
