"""Read S21 mapped-image PE import names at the joint renderer's IAT slots."""

import hashlib
import struct
from disasm_s21_window import DUMP, IMAGE_BASE


def main() -> None:
    image = DUMP.read_bytes()
    assert hashlib.sha256(image).hexdigest() == (
        "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    )
    assert image[:2] == b"MZ"
    nt = struct.unpack_from("<I", image, 0x3C)[0]
    assert image[nt : nt + 4] == b"PE\0\0"
    optional = nt + 24
    assert struct.unpack_from("<H", image, optional)[0] == 0x10B
    import_rva = struct.unpack_from("<I", image, optional + 96 + 8)[0]
    targets = {0x1B4C754, 0x1B4C764, 0x1B4C770,
               0x1B4C774, 0x1B4C778, 0x1B4C77C}
    found = {}
    def asciiz(rva: int) -> str:
        end = image.index(0, rva)
        return image[rva:end].decode("ascii")
    for descriptor in range(import_rva, import_rva + 4096, 20):
        original, _, _, name_rva, first = struct.unpack_from("<IIIII", image, descriptor)
        if not (original or first):
            break
        dll = asciiz(name_rva)
        thunk = original or first
        for index in range(4096):
            value = struct.unpack_from("<I", image, thunk + index * 4)[0]
            if value == 0:
                break
            slot = IMAGE_BASE + first + index * 4
            if slot not in targets:
                continue
            name = f"ordinal{value & 0xFFFF}" if value & 0x80000000 else asciiz(value + 2)
            found[slot] = f"{dll}.{name}"
    assert targets <= found.keys(), (targets - found.keys())
    assert {slot: found[slot] for slot in targets} == {
        0x1B4C754: "opengl32.dll.glBegin",
        0x1B4C764: "opengl32.dll.glEnd",
        0x1B4C770: "opengl32.dll.glColor3f",
        0x1B4C774: "opengl32.dll.glColor3fv",
        0x1B4C778: "opengl32.dll.glTexCoord2f",
        0x1B4C77C: "opengl32.dll.glVertex3fv",
    }
    for slot in sorted(targets):
        print(f"{slot:#x} {found[slot]}")


if __name__ == "__main__":
    main()
