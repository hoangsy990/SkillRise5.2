#!/usr/bin/env python3
"""Read-only search for S21 781/782 client code dispatches in pinned main dump.

This scans immediate bytes and reports only likely x86 CMP sites. It is a
locator, not proof that every reported occurrence owns a Slayer cast path.
"""

from __future__ import annotations

import hashlib
import mmap
import struct
from pathlib import Path


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
SHA = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
BASE = 0x400000
MODEL_TYPE_CMP_781 = {0x13DC8AE, 0x13DDBBF, 0x14241E7}


def main() -> None:
    with DUMP.open("rb") as source:
        image = mmap.mmap(source.fileno(), 0, access=mmap.ACCESS_READ)
        try:
            if hashlib.sha256(image).hexdigest().upper() != SHA:
                raise AssertionError("pinned S21 main dump hash drifted")
            for skill in (781, 782):
                needle = struct.pack("<I", skill)
                sites: list[int] = []
                matches = 0
                index = image.find(needle)
                while index >= 0:
                    matches += 1
                    # 3D = cmp eax, imm32; 81 /7 = cmp r/m32, imm32.
                    if (index > 1 and image[index - 1] == 0x3D) or (
                        index > 2 and image[index - 2] == 0x81 and
                        image[index - 1] & 0x38 == 0x38
                    ):
                        sites.append(BASE + index)
                    index = image.find(needle, index + 1)
                print(f"skill={skill} imm32_matches={matches} cmp_sites={len(sites)}")
                print("cmp immediate addresses:", [hex(site) for site in sites[:80]])
                if skill == 781 and set(sites) != MODEL_TYPE_CMP_781:
                    raise AssertionError("S21 781 cmp-site inventory drifted")
                if skill == 782 and sites:
                    raise AssertionError("S21 782 immediate CMP inventory drifted")
            print("NOTE: the three 781 CMP sites compare OBJECT model Type +0x146, "
                  "not MagicNumber/skill; no 781/782 cast dispatcher is proved "
                  "by this imm32 search")
        finally:
            image.close()


if __name__ == "__main__":
    main()
