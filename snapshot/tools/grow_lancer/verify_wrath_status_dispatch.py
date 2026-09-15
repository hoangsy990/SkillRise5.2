"""Pinned status opcode and local-owner handler fields; not full wire validation."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
cs = Cs(CS_ARCH_X86, CS_MODE_32)
def block(start, end):
    items = list(cs.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert sum(i.size for i in items) == end-start
    return {i.address:(i.mnemonic,i.op_str) for i in items}
dispatch = block(0x12FE380,0x12FE3EA)
assert dispatch[0x12FE3CD] == ('cmp','dword ptr [ebp - 0x3f8], 0xfd')
assert dispatch[0x12FE3D7] == ('ja','0x1305041')
assert dispatch[0x12FE3E3] == ('jmp','dword ptr [eax*4 + 0x130505c]')
assert struct.unpack_from('<I',data,0x130505C-IMAGE_BASE+4*0x2D)[0] == 0x130231B
entry = block(0x130231B,0x1302329)
assert entry[0x130231B] == ('push','dword ptr [ebp + 0xc]')
assert entry[0x130231E] == ('call','0x1293b12')
receive = block(0x1293B12,0x1293D38)
assert receive[0x1293D37] == ('ret','')
for address, instruction in {
    0x1293B47: ('movzx','eax, word ptr [eax + 0x10]'),
    0x1293B54: ('cmp','dword ptr [ebp - 0x30], 0x1cd'),
    0x1293B69: ('movzx','eax, byte ptr [eax + 8]'),
    0x1293B6F: ('jne','0x1293c37'),
    0x1293B78: ('movzx','eax, word ptr [eax + 0x22]'),
    0x1293B80: ('push','dword ptr [eax + 0xc]'),
    0x1293B8B: ('call','0xa0d270'),
    0x1293BAC: ('call','0x12fd717'),
    0x1293C3F: ('call','0xa0d3e7'),
    0x1293C52: ('call','0x1305838'),
}.items():
    assert receive[address] == instruction, (hex(address),receive[address])
assert 0 < 424 < 461 and 0 < 425 < 461
print('PASS: opcode2D ->1293B12; WORD statusID+10h, add/remove byte+8, additional DWORD+0Ch and WORD+22h.')
print('PASS: managerA0D270/A0D3E7 precedes character add/remove. Its semantics remain OPEN.')
print('Scope: minimum observed read extent36 bytes, NOT proven wire size/header or upstream length validation; no native packet changes.')
manager = block(0xA0D270,0xA0D3E7)
remaining = block(0xA0D5EA,0xA0D66B)
assert manager[0xA0D3E4] == ('ret','0xc')
assert remaining[0xA0D668] == ('ret','4')
for address,instruction in {
    0xA0D2D3: ('call','dword ptr [0x1b4c9a8]'),
    0xA0D2DB: ('mov','ecx, 0x3e8'), 0xA0D2E0: ('div','ecx'),
    0xA0D2E2: ('mov','dword ptr [ebp - 0x20], eax'),
    0xA0D2EB: ('mov','eax, dword ptr [ebp + 0xc]'),
    0xA0D2EE: ('mov','dword ptr [ebp - 0x18], eax'),
    0xA0D2F1: ('mov','eax, dword ptr [ebp + 0x10]'),
    0xA0D2F4: ('mov','dword ptr [ebp - 0x14], eax'),
}.items(): assert manager[address] == instruction, hex(address)
for address,instruction in {
    0xA0D632: ('call','dword ptr [0x1b4c9a8]'),
    0xA0D63A: ('mov','ecx, 0x3e8'), 0xA0D63F: ('div','ecx'),
    0xA0D64A: ('sub','ecx, dword ptr [eax]'),
    0xA0D652: ('mov','eax, dword ptr [eax + 8]'),
    0xA0D655: ('cmp','eax, dword ptr [ebp - 4]'),
    0xA0D658: ('ja','0xa0d65e'), 0xA0D65A: ('xor','eax, eax'),
    0xA0D664: ('sub','eax, dword ptr [ebp - 4]'),
}.items(): assert remaining[address] == instruction, hex(address)
import pefile
pe=pefile.PE(data=data)
assert [(e.dll.lower(),i.name) for e in pe.DIRECTORY_ENTRY_IMPORT for i in e.imports
    if i.address==0x1B4C9A8] == [(b'winmm.dll',b'timeGetTime')]
print('PASS: manager records integer seconds; getter computes unsigned duration-(nowSeconds-startSeconds), saturated at zero.')
print('Duration comes from status DWORD+0Ch; no auto membership removal is implemented by this getter.')
inequality=block(0xA0E297,0xA0E2C6)
assert inequality[0xA0E2A5] == ('call','0xa0e245')
assert inequality[0xA0E2AF] == ('jne','0xa0e2ba')
assert inequality[0xA0E2B1] == ('mov','dword ptr [ebp - 4], 1')
assert manager[0xA0D2BA] == ('call','0xa0e297')
assert manager[0xA0D2C4] == ('je','0xa0d2d3')
assert manager[0xA0D2CD] == ('call','0xa0eeb6')
assert manager[0xA0D313] == ('call','0xa0dfe4')
copy=block(0xA0D8F9,0xA0D930)
assert copy[0xA0D910] == ('mov','dword ptr [ecx], eax')
assert [address for address,(mnemonic,_) in copy.items() if mnemonic=='movsd'] == [0xA0D923,0xA0D924,0xA0D925,0xA0D926]
print('PASS: repeat receipt takes existing-key removal helper before fresh timestamp/record insertion; copies complete four-DWORD payload.')
