"""Selected internal281 hit body and return tail; not server damage/reaction proof."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
body = list(decoder.disasm(data[0x12CB4A8-IMAGE_BASE:0x12CB59E-IMAGE_BASE], 0x12CB4A8))
tail = list(decoder.disasm(data[0x12CE702-IMAGE_BASE:0x12CE71D-IMAGE_BASE], 0x12CE702))
assert (body[-1].mnemonic, body[-1].op_str) == ('jmp', '0x12ce702')
assert (tail[-1].address, tail[-1].mnemonic) == (0x12CE71C, 'ret')
pins = {i.address: (i.mnemonic, i.op_str) for i in body+tail}
for address, pair in {
    0x12CB4B0: ('add', 'ecx, 0x158'),
    0x12CB4D2: ('add', 'ecx, 0x158'),
    0x12CB4F4: ('add', 'ecx, 0x158'),
    0x12CB51B: ('addss', 'xmm0, dword ptr [0x1b6d3e8]'),
    0x12CB548: ('push', 'dword ptr [ebp - 0x1948]'),
    0x12CB54E: ('push', '1'),
    0x12CB57B: ('push', '0x5b9'),
    0x12CB580: ('call', '0x143e57c'),
    0x12CB588: ('push', '0'),
    0x12CB58A: ('push', '0'),
    0x12CB58C: ('push', '0x45a'),
    0x12CB591: ('call', '0xa74f94'),
}.items(): assert pins[address] == pair
assert data[0x1B6D3E8-IMAGE_BASE:0x1B6D3E8-IMAGE_BASE+4] == struct.pack('<f', 140.0)
assert not any(i.mnemonic == 'call' and i.op_str == '0x1327de8' for i in body+tail)
root = Path(__file__).resolve().parents[2]
native = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
start = native.index('void CreateMagicPinHit(')
end = native.index('void CreateHarshStrikeRoot(', start)
hit = native[start:end]
assert 'VectorCopy(target.Position, position);' in hit and 'position[2] += 140.0f;' in hit
assert 'target.Light, 1, &target, -1, 0, kMagicPinExplosionSkill);' in hit
assert 'CurrentAction' not in hit and 'SetAction' not in hit
print('PASS selected281 target position+140Z, owner/subtype; direct body/tail has no1327DE8 action dispatch')
prefix = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
    data[0x12C5372-IMAGE_BASE:0x12C5540-IMAGE_BASE], 0x12C5372)}
for address, pair in {
    0x12C5372: ('mov', 'eax, dword ptr [ebp - 0x1958]'),
    0x12C5378: ('add', 'eax, 0x3254'),
    0x12C537D: ('mov', 'dword ptr [ebp - 0x1948], eax'),
    0x12C5415: ('cmp', 'dword ptr [eax + 0x90], 5'),
    0x12C541C: ('je', '0x12c54bf'),
    0x12C5496: ('call', '0x1326b06'),
    0x12C54A0: ('mov', 'ecx, dword ptr [ebp - 0x1944]'),
    0x12C54A6: ('add', 'ecx, 0x164'),
    0x12C54BF: ('mov', 'eax, dword ptr [ebp - 0x194c]'),
    0x12C54CC: ('mov', 'word ptr [eax + 0x40], cx'),
    0x12C5516: ('mov', 'byte ptr [eax + 7], cl'),
    0x12C5526: ('mov', 'word ptr [eax + 0x26], cx'),
    0x12C5532: ('push', 'dword ptr [ebp - 0x194c]'),
    0x12C5538: ('call', '0x140dc35'),
}.items(): assert prefix[address] == pair
print('PASS prefix writes source facing/target index/success/skill; metadata helper receives source character')
helper = {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
    data[0x140DC15-IMAGE_BASE:0x140DCC4-IMAGE_BASE], 0x140DC15)}
for address, pair in {
    0x140DC1F: ('cmp', 'dword ptr [ebp - 4], 6'),
    0x140DC25: ('cmp', 'dword ptr [ebp - 4], 0xf'),
    0x140DC31: ('xor', 'al, al'),
    0x140DC6B: ('cmp', 'dword ptr [eax + 0x90], 0'),
    0x140DC72: ('je', '0x140dc8c'),
    0x140DC92: ('call', '0x140dc15'),
    0x140DC9F: ('mov', 'byte ptr [ebp - 1], 1'),
    0x140DCAE: ('mov', 'byte ptr [eax + 0x31e8], 1'),
    0x140DCBE: ('mov', 'dword ptr [ecx + 0x31ec], eax'),
}.items(): assert helper[address] == pair
print('PASS metadata category0/key281 selects source suppression flag/timestamp writes; snapshot verified separately')
print('OPEN earlier normalization, auxiliary helper descendants, separate damage packets and sound mapping; not target hit-action proof')
