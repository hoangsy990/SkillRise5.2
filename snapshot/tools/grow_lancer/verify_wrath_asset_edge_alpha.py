"""Read-only Wrath source/stage corner and alpha audit; not ingame pixels."""
from hashlib import sha256
from io import BytesIO
from pathlib import Path
from struct import unpack_from
from PIL import Image

ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
ORIGINAL = Path(r"D:\MU FICA Season 21\Data\Effect")
STAGED = (ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "Client"
          / "Data" / "RISE" / "GrowLancer" / "Wrath")
JPEGS = {
    "brocken.OZJ": "ce40175cc9420b38242a7e3b1f1f425eb6fa1c1f15f9bf6dc15cf6c39f8ae2d7",
    "!Combo3.OZJ": "fbb0ee033d39ba7576714ef96c4a8244ca14480545809fe98147117dfeead984",
    "Combo4.OZJ": "389780dfb08902e299df9289f829349e58200b0b5bf7847aadfadcba687b0fda",
    "alpha_line.OZJ": "c9fa50d8d6604bb81087f1837f7f0553377b7eabf57569cb54465f085dacce9b",
}
TGAS = {
    "alpha_light.OZT": "ecb031ec0d7037015da6758fe644200338e279d377d095b6ab6e14c9b51cbb8a",
    "dust01.OZT": "69f4a9f6dc806a77cc3cd7463c554e710487b46e5986b210255c738373c44bda",
}


def exact(name: str, expected: str) -> bytes:
    original = (ORIGINAL / name).read_bytes()
    staged = (STAGED / name).read_bytes()
    assert sha256(original).hexdigest() == expected, f"S21 {name} hash drift"
    assert staged == original, f"staged {name} differs from source"
    return original


def outer4(x: int, y: int, width: int, height: int) -> bool:
    return x < 4 or y < 4 or x >= width - 4 or y >= height - 4


def main() -> None:
    for name, expected in JPEGS.items():
        blob = exact(name, expected)
        assert blob[24:27] == b"\xff\xd8\xff", f"native JPEG payload {name}"
        image = Image.open(BytesIO(blob[24:])).convert("RGB")
        corners = [image.getpixel(point) for point in
                   ((0, 0), (image.width-1, 0),
                    (0, image.height-1), (image.width-1, image.height-1))]
        assert corners == [(0, 0, 0)] * 4, f"{name} corners changed"
        edge = [max(image.getpixel((x, y))) for y in range(image.height)
                for x in range(image.width) if outer4(x, y, image.width, image.height)]
        print(f"JPEG {name}: {image.width}x{image.height}, corner RGB0, "
              f"outer4 max={max(edge)} <=8 fraction={sum(v<=8 for v in edge)}/{len(edge)}")
    for name, expected in TGAS.items():
        blob = exact(name, expected)
        width, height, depth = unpack_from("<hhB", blob, 16)
        assert depth == 32 and width > 0 and height > 0
        # Native GlobalBitmap::OpenTga: 12-byte dump prefix + 10-byte TGA
        # header; pixel stream begins at offset22, not offset24.
        pixels = blob[22:22 + width * height * 4]
        assert len(pixels) == width * height * 4
        edge_alpha = [pixels[(y * width + x) * 4 + 3]
                      for y in range(height) for x in range(width)
                      if outer4(x, y, width, height)]
        assert max(edge_alpha) < 64, f"{name} opaque outer4 alpha"
        print(f"TGA {name}: {width}x{height}, outer4 alpha max={max(edge_alpha)}, "
              "below native 0.25 alpha-test threshold")
    loader = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "GlobalBitmap.cpp").read_text(
        encoding="utf-8", errors="replace")
    native = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "ZzzOpenglUtil.cpp").read_text(
        encoding="utf-8", errors="replace")
    assert "fseek(infile,24,SEEK_SET)" in loader
    assert "int index = 12;\n\tindex += 4;" in loader
    assert "glAlphaFunc(GL_GREATER, 0.25f);" in native
    print("PASS: hash-pinned original/staged Wrath JPG/OZT edges; black-square owner cause remains OPEN")
    print("Scope: asset/native-loader statics only, not actual GL blend/depth/shader pixels")


if __name__ == "__main__":
    main()
