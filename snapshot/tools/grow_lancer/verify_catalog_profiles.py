#!/usr/bin/env python3
"""Verify the separately pinned Grow Lancer client and server catalog profiles."""

from __future__ import annotations

import hashlib
import struct
import xml.etree.ElementTree as ET
from pathlib import Path


CLIENT = Path(r"D:\MU FICA Season 21\Data\Local\skill.bmd")
SERVER = Path(r"C:\Users\DELL\Desktop\Skills\SkillList.xml")
CLIENT_SHA = "AC74B3A175D08B50D8C5C3D5BD8C05420E9F6690BC7A5C6C95EF3D7F8278BF79"
SERVER_SHA = "3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0"

EXPECTED_CLIENT = {
    271: (83, 100, 12, 0, 2, 500),
    272: (220, 0, 100, 50, 0, 0),
    273: (74, 0, 50, 50, 0, 0),
    274: (30, 80, 30, 20, 2, 500),
    275: (0, 50, 50, 50, 6, 10000),
    276: (74, 100, 12, 0, 3, 500),
    277: (92, 70, 50, 20, 4, 500),
    278: (66, 0, 100, 50, 0, 60000),
    279: (44, 400, 40, 20, 3, 500),
}

EXPECTED_SERVER = {
    271: (83, 100, 12, 0, 2, 500),
    272: (220, 0, 100, 50, 0, 0),
    273: (74, 0, 50, 50, 0, 0),
    274: (30, 80, 5, 3, 2, 500),
    275: (0, 50, 50, 50, 6, 10000),
    276: (74, 100, 12, 0, 3, 500),
    277: (92, 50, 8, 5, 4, 0),
    278: (66, 0, 40, 30, 0, 0),
    279: (300, 230, 15, 6, 5, 0),
}


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main() -> None:
    assert sha256(CLIENT) == CLIENT_SHA
    assert sha256(SERVER) == SERVER_SHA
    raw = CLIENT.read_bytes()
    assert len(raw) == 750 * 120 + 4
    key = (0xFC, 0xCF, 0xAB)
    client_rows = {}
    for skill_id in range(271, 280):
        enc = raw[skill_id * 120:(skill_id + 1) * 120]
        row = bytes(value ^ key[offset % 3] for offset, value in enumerate(enc))
        client_rows[skill_id] = struct.unpack_from("<6H", row, 32)[:5] + (struct.unpack_from("<I", row, 44)[0],)
        assert struct.unpack_from("<H", row, 74)[0] == skill_id
    assert client_rows == EXPECTED_CLIENT

    root = ET.parse(SERVER).getroot()
    server_rows = {}
    for row in root.iter("Skill"):
        skill_id = int(row.attrib["Index"])
        if 271 <= skill_id <= 279:
            server_rows[skill_id] = tuple(int(row.attrib[name]) for name in (
                "ReqLevel", "Damage", "ManaUsage", "BPUsage", "Distance", "Delay"))
            assert row.attrib["GrowLancer"] == "1"
            assert int(row.attrib["IconNumber"]) == skill_id
    assert server_rows == EXPECTED_SERVER

    exact = [skill_id for skill_id in client_rows if client_rows[skill_id] == server_rows[skill_id]]
    different = [skill_id for skill_id in client_rows if client_rows[skill_id] != server_rows[skill_id]]
    assert exact == [271, 272, 273, 275, 276]
    assert different == [274, 277, 278, 279]
    print("PASS: pinned client profile and supplied server profile decoded independently")
    print("PASS: exact IDs 271,272,273,275,276; divergent IDs 274,277,278,279")


if __name__ == "__main__":
    main()
