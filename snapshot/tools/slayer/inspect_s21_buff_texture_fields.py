"""Read-only RGB field audit for the S21 Slayer buff model materials.

OZJ carries a 24-byte duplicate JPEG prefix before the complete JPEG. This
reports authored dark-field coverage; it neither changes texture bytes nor
claims a particular 5.2 framebuffer mesh caused an observed black block.
"""

from __future__ import annotations

import hashlib
from io import BytesIO
from pathlib import Path

from PIL import Image


ROOT = Path(r"D:\MU FICA Season 21\Data\Effect")
TEXTURES = {
    "Elite_monster_ground02.OZJ": (
        "ED8265FB90F5B6FBC5371EC5CCCCE8E703972A3DA1AB0BEB9062BFC54E6B4A1E", (256, 256), 16),
    "ark.OZJ": (
        "96B04ACCF070BABC8911D586A78E243A6672E926D15B755F4B8AD6DA1DC40D2A", (128, 128), 48),
    "empact01.OZJ": (
        "BBBF1FB4EDD1CB2492674CD321A8B0C240CA4A68131772924FD60749046628D7", (256, 256), 16),
}


def main() -> None:
    for name, (expected_sha, expected_size, current_cutoff) in TEXTURES.items():
        raw = (ROOT / name).read_bytes()
        digest = hashlib.sha256(raw).hexdigest().upper()
        if digest != expected_sha or raw[:4] != raw[24:28]:
            raise AssertionError(f"S21 OZJ hash/header drifted for {name}")
        image = Image.open(BytesIO(raw[24:])).convert("RGB")
        if image.size != expected_size:
            raise AssertionError(f"S21 OZJ dimensions drifted for {name}")
        pixels = image.tobytes()
        peaks = [max(pixels[index:index + 3])
                 for index in range(0, len(pixels), 3)]
        keyed = sum(value <= current_cutoff for value in peaks)
        residual = sum(current_cutoff < value <= 48 for value in peaks)
        print(f"S21 {name}: size={image.size} sha256={digest} "
              f"keyed<= {current_cutoff}: {keyed}/{len(peaks)} "
              f"dark-residual<=48: {residual}/{len(peaks)}")
    print("NOTE: texture field statistics do not attribute screenshot pixels to a model")


if __name__ == "__main__":
    main()
