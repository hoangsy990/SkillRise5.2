"""Read-only S21 joint-record +A44 operand inventory; leads, not reachability proof."""

import hashlib
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main() -> None:
    image = DUMP.read_bytes()
    expected = "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    if hashlib.sha256(image).hexdigest() != expected:
        raise RuntimeError("S21 client dump changed")
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    # The selected joint constructor/update/renderer is within this image
    # code band. A raw displacement match is only a candidate: decode a full
    # instruction across the displacement before reporting it.
    start, end = 0x400000, 0x1900000
    region = image[start - IMAGE_BASE : end - IMAGE_BASE]
    needle = b"\x44\x0a\x00\x00"
    hits = {}
    cursor = 0
    while True:
        offset = region.find(needle, cursor)
        if offset < 0:
            break
        cursor = offset + 1
        needle_address = start + offset
        # x86 instructions are at most 15 bytes; a decoded candidate must
        # contain the entire four-byte disp and access a register+0xA44.
        for begin in range(max(start, needle_address - 14), needle_address + 1):
            raw = image[begin - IMAGE_BASE : needle_address - IMAGE_BASE + 15]
            instruction = next(decoder.disasm(raw, begin, count=1), None)
            if (instruction and instruction.address <= needle_address and
                instruction.address + instruction.size >= needle_address + 4 and
                "0xa44]" in instruction.op_str):
                hits[instruction.address] = (instruction.mnemonic, instruction.op_str)
    # Remove overlapping decodes that begin inside the real 0x66 word-store
    # prefix. They are disassembly leads, not distinct source operations.
    canonical = {}
    covered_until = 0
    for address, instruction in sorted(hits.items()):
        if address < covered_until:
            continue
        decoded = next(decoder.disasm(image[address - IMAGE_BASE : address - IMAGE_BASE + 15], address, count=1))
        canonical[address] = instruction
        covered_until = address + decoded.size
    for address, (mnemonic, operands) in sorted(canonical.items()):
        print(f"{address:#x} {mnemonic} {operands}")
    print(f"candidate_direct_operands={len(canonical)}; indirect/virtual accesses not excluded")
    assert (canonical[0x15C670E] ==
            ("mov", "word ptr [eax + 0xa44], cx"))
    assert (canonical[0x15DE586] ==
            ("mov", "word ptr [ecx + 0xa44], ax"))
    assert (canonical[0x1613AD7] ==
            ("movsx", "ecx, word ptr [ecx + 0xa44]"))
    for begin, finish in ((0x1612773, 0x1612CBD),
                          (0x1618F44, 0x161AEB9),
                          (0x15E2756, 0x15E37EF)):
        assert not any(begin <= address < finish for address in canonical), (
            hex(begin), hex(finish))
    selected = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        image[0x1612773 - IMAGE_BASE : 0x1612CBD - IMAGE_BASE], 0x1612773)}
    assert selected[0x161279C] == ("je", "0x1612963")
    assert selected[0x16127A9] == ("je", "0x1612963")
    assert selected[0x16127B6] == ("je", "0x1612963")
    assert selected[0x1612CB8] == ("jmp", "0x1618ada")
    unrelated = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        image[0x16139CC - IMAGE_BASE : 0x1613B2F - IMAGE_BASE], 0x16139CC)}
    assert unrelated[0x16139F2] == ("cmp", "dword ptr [eax + 0x5c], 0xae4")
    assert unrelated[0x1613AD7] == canonical[0x1613AD7]
    print("selected pin2/3/4 branch, renderer and called tail-shift: no direct +A44; unrelated reader is guarded by owner type0xAE4")
    for begin, finish in ((0x1612963, 0x1612CBD),
                          (0x1618F44, 0x161AEB9),
                          (0x15E2756, 0x15E37EF)):
        instructions = tuple(decoder.disasm(
            image[begin - IMAGE_BASE : finish - IMAGE_BASE], begin))
        calls = [(i.address, i.op_str) for i in instructions if i.mnemonic == "call"]
        indirect = [(address, target) for address, target in calls
                    if not target.startswith("0x")]
        if begin == 0x1618F44:
            # These are the mapped OpenGL IAT slots, not joint-record
            # vtables. map_s21_joint_renderer_imports.py resolves names.
            allowed = {0x1B4C754, 0x1B4C764, 0x1B4C770,
                       0x1B4C774, 0x1B4C778, 0x1B4C77C}
            assert all(target.startswith("dword ptr [0x") and
                       int(target.split("[")[1].split("]")[0], 16) in allowed
                       for _, target in indirect)
        else:
            assert not indirect, (hex(begin), indirect)
        print(f"calls {begin:#x}..{finish:#x}: total={len(calls)} indirect={len(indirect)} "
              f"mappedGLIAT={len(indirect) if begin == 0x1618F44 else 0}")
    for target in (0x1612773, 0x16139CC):
        pointer = target.to_bytes(4, "little")
        pointer_locations = []
        offset = 0
        while True:
            offset = image.find(pointer, offset)
            if offset < 0:
                break
            pointer_locations.append(IMAGE_BASE + offset)
            offset += 1
        print(f"handler_pointer {target:#x}: {[hex(a) for a in pointer_locations[:16]]}")
    type_needle = (0x8073).to_bytes(4, "little")
    type_region = image[0x1600000 - IMAGE_BASE : 0x1613000 - IMAGE_BASE]
    type_cursor = 0
    type_offsets = []
    while True:
        type_cursor = type_region.find(type_needle, type_cursor)
        if type_cursor < 0:
            break
        type_offsets.append(0x1600000 + type_cursor)
        type_cursor += 1
    print(f"8073_immediate_leads_in_joint_update: {[hex(a) for a in type_offsets]}")


if __name__ == "__main__":
    main()
