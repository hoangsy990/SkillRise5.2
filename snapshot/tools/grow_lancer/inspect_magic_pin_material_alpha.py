"""Hash-pinned read-only OZT/TGA payload alpha check; no GPU/pixel claim."""

import hashlib
import json
import struct
from pathlib import Path


SOURCE = Path(r"D:\MU FICA Season 21\Data\Effect")
FILES = {
    "wind1.OZT": "098E001E87FE37CDCD2D29D96E806F7009395E338ADA2294EF0590B4F9C277AC",
    "alpha_line03.OZT": "0FC49D015E42754A5A0B809B6C08413B4941877BC4668C4065BE93BD34562306",
}


def inspect(path: Path, expected_hash: str):
    data = path.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != expected_hash:
        raise ValueError(f"Source hash mismatch: {path}")
    # Exact native GlobalBitmap.cpp::OpenTga OZT offsets: dimensions at +16,
    # bit-depth at +20, pixel stream at +22; no image export or mutation.
    width, height = struct.unpack_from("<HH", data, 16)
    bit_depth = data[20]
    if bit_depth != 32 or width != 64 or height != 64:
        raise ValueError(f"Unexpected OZT layout: {path}")
    end = 22 + width * height * 4
    if len(data) < end:
        raise ValueError(f"Truncated OZT pixel stream: {path}")
    alpha = data[25:end:4]
    return {
        "source": str(path), "sha256": digest, "width": width,
        "height": height, "bitDepth": bit_depth,
        "zeroAlphaPixels": alpha.count(0),
        "nonzeroAlphaPixels": len(alpha) - alpha.count(0),
        "maxAlpha": max(alpha), "minAlpha": min(alpha),
        "scope": "original OZT payload only; mesh UV sampling/native GPU draw still open",
    }


if __name__ == "__main__":
    print(json.dumps([inspect(SOURCE / name, digest)
                      for name, digest in FILES.items()], indent=2))
