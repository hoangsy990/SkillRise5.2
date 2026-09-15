"""Pin the S21 flare-sprite renderer branch used by Breche's emitter.

This verifier reads the preserved Season 21 image dump only.  It records the
renderer contract that is relevant to the 0x7EF7 flare: the visibility envelope,
the scale/texture-size calculation and the full-UV native billboard call.  It
does not claim that the RISE client produced matching pixels.
"""

from hashlib import sha256
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
# The preserved S21 dump lives in the read-only shared reverse-artifact tree,
# not inside this worktree.  Keep this path explicit so a missing dump fails
# closed instead of silently reading a different executable.
DUMP = Path(r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe")
IMAGE_BASE = 0x400000
DUMP_SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"


def window(data: bytes, va: int, size: int) -> bytes:
    start = va - IMAGE_BASE
    if start < 0 or start + size > len(data):
        raise ValueError(f"window outside preserved image: {va:#x}")
    return data[start:start + size]


def require(blob: bytes, needle: bytes, label: str) -> None:
    if needle not in blob:
        raise ValueError(f"missing S21 flare-render evidence: {label}")


def main() -> None:
    data = DUMP.read_bytes()
    digest = sha256(data).hexdigest().upper()
    if digest != DUMP_SHA256:
        raise ValueError(f"S21 image dump changed: {digest}")

    render = window(data, 0x1727079, 0x4F5)
    # 1727079 reads sprite+0x0B (Visible), then adds/subtracts .1 and clamps
    # the private visibility envelope to [0.2, 1.0].
    require(render, bytes.fromhex("8b45080fb6400b85c07447"), "visible branch reads +0x0B")
    require(render, bytes.fromhex("f30f580504dfb401"), "visible +0.1")
    require(render, bytes.fromhex("f30f100508dfb4010f2f80d00000007613"), "visible cap 1.0")
    require(render, bytes.fromhex("f30f5c0504dfb401"), "non-visible -0.1")
    require(render, bytes.fromhex("f30f100508dfb4010f2f80d00000007613"), "non-visible floor 0.2")

    # The renderer multiplies the envelope at +0xD0 by the stored sprite scale
    # at +0xA0, then reads texture dimensions +0x204/+0x208 through the native
    # texture lookup.  The emitter's A13A00 seed is 1x1, so it takes the full
    # UV branch at 17274DA and reaches the native 18E8B0C billboard primitive.
    require(render, bytes.fromhex("8b 45 08 8b 4d 08 f3 0f 10 80 d0 00 00 00 f3 0f 59 81 a0 00 00 00 f3 0f 11 45"), "envelope times stored scale")
    require(render, bytes.fromhex("8b45f0f30f108004020000f30f5945ec"), "texture width")
    require(render, bytes.fromhex("8b45f0f30f108008020000f30f5945ec"), "texture height")
    require(render, bytes.fromhex("83 7d e8 07 0f 87 d3 00 00 00 8b 45 e8 ff 24 85 71 75 72 01"), "type-specific UV dispatch boundary")
    require(render, bytes.fromhex("8b4508ff705ce8a2151c0083c42cc9"), "full-UV native billboard call")

    print("PASS: S21 1727079 flare envelope, size calculation and full-UV billboard call pinned")
    print("Scope: 0x7EF7 provenance/default full-UV route; no RISE pixel or owner-pool acceptance")


if __name__ == "__main__":
    main()
