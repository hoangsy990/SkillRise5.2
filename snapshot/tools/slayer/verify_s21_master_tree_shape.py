#!/usr/bin/env python3
"""Pin the read-only S21 Master Slayer BMD shape against the 5.2 loader.

This intentionally does not convert or rewrite either protected/source BMD.
The two 24-byte records have a different last-field meaning in S21 and 5.2;
a blind copy is not a usable native class-tree import.
"""

from __future__ import annotations

import hashlib
import struct
from pathlib import Path


S21 = Path(r"D:\MU FICA Season 21\Data\Local\masterskilltreedata.bmd")
RISE52 = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer\Client\Data\RISE\Config\MasterSkillTreeData.bmd")
S21_SHA = "A0381045194779304C7922685D1DDB9EA1E23DD0354FE02A36A9C91C8305D586"
RISE52_SHA = "D67B20890CBB2DCFF9FF9CAB670E30D51A3DD13C97B17D4C641676B43A7DDECC"
RECORD = struct.Struct("<HHBBBBiiiI")
BUX = (0xFC, 0xCF, 0xAB)


def rows(path: Path, expected_sha: str, expected_count: int):
    raw = path.read_bytes()
    digest = hashlib.sha256(raw).hexdigest().upper()
    if digest != expected_sha:
        raise AssertionError(f"{path}: hash drift {digest}")
    if len(raw) != RECORD.size * expected_count + 4:
        raise AssertionError(f"{path}: record count/size drift {len(raw)}")
    result = []
    for index in range(expected_count):
        offset = index * RECORD.size
        decoded = bytes(
            value ^ BUX[byte_index % len(BUX)]
            for byte_index, value in enumerate(raw[offset:offset + RECORD.size])
        )
        result.append(RECORD.unpack(decoded))
    return result


def main() -> None:
    s21 = rows(S21, S21_SHA, 2048)
    rise52 = rows(RISE52, RISE52_SHA, 512)
    if any(row[0] == 0 for row in rise52):
        raise AssertionError("5.2 512-record table unexpectedly has a free slot")
    slayer = [row for row in s21 if row[0] and row[1] & 512]
    if len(slayer) != 58 or {row[2] for row in slayer} != {0, 1, 2}:
        raise AssertionError("S21 Master Slayer three-category tree shape drifted")
    strengthener = s21[653]
    mastery = s21[655]
    if strengthener[:10] != (58, 512, 1, 1, 20, 3, 0, 0, 781, 22):
        raise AssertionError(f"S21 781 UI row drifted: {strengthener}")
    if mastery[:10] != (62, 512, 1, 10, 10, 0, 781, 0, 782, 23):
        raise AssertionError(f"S21 782 UI row drifted: {mastery}")
    if rise52[0][9] != 1094755287:
        raise AssertionError("5.2 final field no longer has its known float bits")
    print("PASS: pinned S21/5.2 mastery BMD hashes and 24-byte Bux records")
    print("PASS: S21 Master Slayer class bit=512, 58 nodes, categories=0/1/2")
    print("PASS: S21 781 slot=58 and 782 slot=62 parent=781, records 653/655")
    print("PASS: 5.2 loader has 512 occupied records; S21 uses 2048")
    print("NOTE: S21 last field is integer 22/23 here; 5.2 parses it as float")


if __name__ == "__main__":
    main()
