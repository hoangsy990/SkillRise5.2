"""Classify 17 nearby direct-pointer candidates in selected S21 basic blocks."""

import hashlib
import struct

from capstone import Cs, CS_ARCH_X86, CS_MODE_32

from disasm_s21_window import DUMP, IMAGE_BASE


SHA = "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"


def main() -> None:
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == SHA
    needle = struct.pack("<I", 0x1E04FBC)
    fields = (struct.pack("<I", 0x1B6), struct.pack("<I", 0x1C2))
    offsets = []
    start = 0
    while (position := data.find(needle, start)) >= 0:
        if any(field in data[position + 4:position + 200] for field in fields):
            offsets.append(position + IMAGE_BASE)
        start = position + 1
    expected = {
        0xB552BD, 0xB552D5, 0xB55317, 0xB55329,
        0xB56DBC, 0xB56DD1, 0xF62508, 0x127D95E,
        0x127D96B, 0x129644F, 0x1296462, 0x1408587,
        0x140859C, 0x1414022, 0x14142FC, 0x14155EF,
        0x1415860,
    }
    assert set(offsets) == expected, (len(offsets), set(offsets) ^ expected)
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    windows = (
        (0xB552B9, 0xB55341), (0xB56DBB, 0xB56DE3),
        (0xF62506, 0xF62524), (0x127D95D, 0x127D989),
        (0x1296442, 0x1296474), (0x1408580, 0x14085B6),
        (0x1414021, 0x1414077), (0x14142FB, 0x1414351),
        (0x14155EE, 0x141563F), (0x141585F, 0x14158B8),
    )
    decoded = {item.address: (item.mnemonic, item.op_str)
               for begin, end in windows for item in decoder.disasm(
                   data[begin - IMAGE_BASE:end - IMAGE_BASE], begin)}
    for address, pair in {
        0xB55316: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0xB5531B: ("mov", "ax, word ptr [eax + 0x1c2]"),
        0xB55328: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0xB5532D: ("mov", "ax, word ptr [eax + 0x1b6]"),
        0xB56DBB: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0xB56DC0: ("mov", "ax, word ptr [eax + 0x1c2]"),
        0xB56DD0: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0xB56DD5: ("mov", "ax, word ptr [eax + 0x1b6]"),
        0xF62506: ("mov", "ecx, dword ptr [0x1e04fbc]"),
        0xF6250C: ("movzx", "eax, word ptr [ecx + eax*2 + 0x1e6]"),
        0x127D95D: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x127D962: ("movzx", "eax, word ptr [eax + 0x1c2]"),
        0x127D96A: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x127D96F: ("movzx", "eax, word ptr [eax + 0x1b6]"),
        0x129645A: ("mov", "word ptr [eax + 0x1b6], cx"),
        0x129646D: ("mov", "word ptr [eax + 0x1c2], cx"),
        0x1408586: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x140858B: ("movzx", "eax, word ptr [eax + 0x1b6]"),
        0x140859B: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x14085A0: ("movzx", "eax, word ptr [eax + 0x1c2]"),
        0x1414021: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x1414026: ("movzx", "eax, word ptr [eax + 0x1b6]"),
        0x14142FB: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x1414300: ("movzx", "eax, word ptr [eax + 0x1b6]"),
        0x14155EE: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x14155F3: ("movzx", "eax, word ptr [eax + 0x1b6]"),
        0x141585F: ("mov", "eax, dword ptr [0x1e04fbc]"),
        0x1415864: ("movzx", "eax, word ptr [eax + 0x1b6]"),
    }.items():
        assert decoded[address] == pair, (hex(address), decoded.get(address))
    print("PASS 17 near-speed direct-pointer candidates: receiver1296442 writes two WORDs")
    print("PASS selected B553/B56D display, 127D95D push, 1408580 cast and 14140/14155 model-scalar paths read speed")
    print("PASS F62506 reads unrelated indexed member+1E6; B552BD/D5 read flag+1A6")
    print("Scope: immediate-pointer candidates with 200-byte proximity only; indirect aliases/other basic blocks and GS sender OPEN")


if __name__ == "__main__":
    main()
