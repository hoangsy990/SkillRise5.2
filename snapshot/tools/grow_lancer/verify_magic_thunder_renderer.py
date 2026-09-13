"""Verify selected S21 Thunder renderer dispatch/arguments, not GPU parity."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    def decode(start, end):
        return {i.address: i for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start)}
    dispatch = decode(0x1620BE0, 0x1620D5A)
    # Walk the actual comparison tree for resource 7F2B, rather than infer
    # default routing from absence of a numeric xref.
    pc = 0x1620BE0
    comparison = 0
    visited = []
    while pc != 0x1640DD2:
        instruction = dispatch[pc]
        visited.append(pc)
        assert len(visited) < 80
        op = instruction.mnemonic
        next_pc = pc + instruction.size
        if op == 'cmp':
            assert instruction.op_str.startswith('dword ptr [ebp - 0x348], ')
            comparison = 0x7F2B - int(instruction.op_str.split(', ')[1], 0)
        elif op in ('jg', 'je', 'jmp'):
            if op == 'jmp' or (op == 'jg' and comparison > 0) or (op == 'je' and comparison == 0):
                next_pc = int(instruction.op_str, 0)
        else:
            raise AssertionError(instruction.op_str)
        pc = next_pc
    assert visited[-1] == 0x1620D55
    body = decode(0x1640DD2, 0x1640E76)
    pins = {
        0x1640DD2: ('push', '4'),
        0x1640E0E: ('movss', 'xmm0, dword ptr [eax + 0x60]'),
        0x1640E23: ('add', 'eax, 0x38'),
        0x1640E55: ('add', 'eax, 0x20'),
        0x1640E6B: ('push', 'dword ptr [eax + 0x10]'),
        0x1640E6E: ('call', '0x18e8b0c'),
    }
    for address, expected in pins.items():
        i = body[address]
        assert (i.mnemonic, i.op_str) == expected, hex(address)
    assert not any(i.mnemonic.startswith('mul') or '+ 0x44]' in i.op_str for i in body.values())
    source = (Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE/ZzzEffectParticle.cpp').read_text(encoding='utf-8')
    render = source[source.index('void RenderParticles'):]
    assert 'case BITMAP_ENERGY:' not in render
    assert 'default:\n                RenderSprite(o->TexType, o->Position, Width, Height, o->Light, o->Rotation);' in render
    print('PASS Thunder7F2B selected default route and raw RGB/rotation/texture arguments; native default matches')
    print('Scope excludes sprite raster internals, GPU parity, actual pool and whole-skill acceptance')


if __name__ == '__main__':
    main()
