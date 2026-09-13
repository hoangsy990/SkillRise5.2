"""Offline dispatch leads only; bounded candidate scans do not prove semantics."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
leads = {}
for opcode in range(0xFE):
    entry = struct.unpack_from('<I', data, 0x130505C-IMAGE_BASE+4*opcode)[0]
    # Only straight-line entry blocks; stop at the first transfer, including
    # conditional jumps. Indirect/subcommand routes are deliberately excluded.
    for instruction in decoder.disasm(data[entry-IMAGE_BASE:entry-IMAGE_BASE+128], entry):
        if instruction.mnemonic == 'call' and instruction.op_str.startswith('0x'):
            target = int(instruction.op_str, 16)
            leads.setdefault(target, []).append((opcode, entry, instruction.address))
        if instruction.mnemonic.startswith('j') or instruction.mnemonic.startswith('ret'):
            break
for target, routes in sorted(leads.items()):
    block = []
    for instruction in decoder.disasm(data[target-IMAGE_BASE:target-IMAGE_BASE+2048], target):
        block.append(instruction)
        if instruction.mnemonic.startswith('ret'):
            break
    masks = [hex(i.address) for i in block if '0x7fff' in i.op_str]
    if masks:
        calls = [(hex(i.address), i.op_str) for i in block if i.mnemonic == 'call']
        print(dict(target=hex(target), routes=[tuple(hex(x) for x in r) for r in routes],
                   keyMaskLeads=masks, calls=calls))
print('Scope: direct entry blocks and bounded mask leads only; excludes subcommands/indirect handlers.')
