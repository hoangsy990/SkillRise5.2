"""Pin Magic local action selection and setter reset policy, not live timing."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
def decode(start, end):
    rows = list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    assert rows and rows[-1].address + rows[-1].size == end
    return {i.address: (i.mnemonic, i.op_str) for i in rows}

rows = decode(0x1327D72, 0x1327DE8)
rows.update(decode(0x1327DE8, 0x1327E57))
rows.update(decode(0x10E4170, 0x10E4190))
rows.update(decode(0x12CB397, 0x12CB3C4))
for address, expected in {
    0x10E4170: ('push', '-1'),
    0x10E4172: ('push', '0xbc'),
    0x10E4188: ('call', '0x1327de8'),
    0x12CB39D: ('cmp', 'eax, dword ptr [0xa484634]'),
    0x12CB3A3: ('je', '0x12cb488'),
    0x12CB3A9: ('push', '-1'),
    0x12CB3AB: ('push', '0xbc'),
    0x12CB3BC: ('call', '0x1327de8'),
    0x1327E27: ('cmp', 'dword ptr [ebp + 0x14], -1'),
    0x1327E2B: ('je', '0x1327e42'),
    0x1327E42: ('push', 'dword ptr [ebp + 0x10]'),
    0x1327E4E: ('call', '0x1327d72'),
    0x1327DAB: ('je', '0x1327de6'),
    0x1327DB7: ('mov', 'word ptr [eax + 0x2c], cx'),
    0x1327DC1: ('mov', 'ecx, dword ptr [ecx + 0xd0]'),
    0x1327DC7: ('mov', 'dword ptr [eax + 0xd4], ecx'),
    0x1327DD4: ('mov', 'word ptr [eax + 0x2a], cx'),
    0x1327DDB: ('xorps', 'xmm0, xmm0'),
    0x1327DDE: ('movss', 'dword ptr [eax + 0xd0], xmm0'),
}.items():
    assert rows[address] == expected, hex(address)
root = Path(__file__).resolve().parents[2]
native = (root/'ExMain_RISE_PC/Main5.2_RISE/ZzzAI.cpp').read_text(encoding='latin-1')
setter = native.split('void SetAction(OBJECT* o, int Action, bool bBlending)', 1)[1].split('bool TestDistance', 1)[0]
for token in ('if (o->CurrentAction != Action)', 'o->PriorAction = o->CurrentAction;',
              'o->PriorAnimationFrame = o->AnimationFrame;', 'o->AnimationFrame = 0;',
              'if (bBlending == false)'):
    assert token in setter
runtime = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
magic = runtime.split('bool CreateMagicPinRoots(OBJECT& caster)', 1)[1].split('\n}', 1)[0]
assert 'SetAction(&caster, 287, true);' in magic
assert 'caster.AnimationFrame =' not in magic
assert 'caster.PriorAnimationFrame =' not in magic
print('PASS pinned local Magic188 -> setter; native287 preserves prior frame and identical-action phase')
print('PASS received Magic uses same188/-1 arguments; local actor bypasses this remote action/emission block')
print('OPEN effective speed, controller scheduling, complete receive route and visual QA')
