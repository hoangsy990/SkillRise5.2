"""Pin S21 head82/sub04 fixed byte offsets without inventing C2 support."""

import hashlib

from capstone import Cs, CS_ARCH_X86, CS_MODE_32

from disasm_s21_window import DUMP, IMAGE_BASE


def main() -> None:
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == (
        "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    )
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoded = {item.address: (item.mnemonic, item.op_str)
               for begin, end in ((0x1303939, 0x1303A1E),
                                  (0x1303D1B, 0x1303D29),
                                  (0x1296442, 0x1296474))
               for item in decoder.disasm(data[begin - IMAGE_BASE:end - IMAGE_BASE], begin)}
    for address, pair in {
        0x130394D: ("cmp", "eax, 0xc1"),
        0x1303963: ("movzx", "eax, byte ptr [eax + 3]"),
        0x130397C: ("cmp", "eax, 0xc2"),
        0x1303992: ("movzx", "eax, byte ptr [eax + 4]"),
        0x1303A0C: ("cmp", "dword ptr [ebp - 0x290], 4"),
        0x1303A13: ("je", "0x1303d1b"),
        0x1303D1B: ("push", "dword ptr [ebp + 0xc]"),
        0x1303D1E: ("call", "0x1296442"),
        0x1296456: ("mov", "cx, word ptr [ecx + 4]"),
        0x129645A: ("mov", "word ptr [eax + 0x1b6], cx"),
        0x1296469: ("mov", "cx, word ptr [ecx + 8]"),
        0x129646D: ("mov", "word ptr [eax + 0x1c2], cx"),
    }.items():
        assert decoded[address] == pair, (hex(address), decoded.get(address))
    local = [decoded[address] for address in sorted(decoded) if 0x1303939 <= address <= 0x1303A13]
    assert all(not (mnemonic == "cmp" and "byte ptr [" in operand)
               for mnemonic, operand in local)
    print("PASS S21 head82 C1 sub-byte3/C2 sub-byte4 branches select sub04 raw-buffer receiver")
    print("PASS selected receiver always reads WORDs at raw+4/+8 into global stat; selected branch does not compare packet-length bytes")
    print("OPEN upstream packet validation, actual C2 sub04 use, sender/rate and native wire equivalence")


if __name__ == "__main__":
    main()
