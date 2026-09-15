"""Pin Wrath ground additive selection; not visual acceptance."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
def decode(start, end):
    return {i.address: (i.mnemonic, i.op_str) for i in decoder.disasm(
        data[start - IMAGE_BASE:end - IMAGE_BASE], start)}
branch = decode(0x15A0B63, 0x15A0C12)
assert branch[0x15A0B63] == ('call', '0x18e7137')
assert branch[0x15A0C05] == ('push', '0x81e3')
assert branch[0x15A0C0A] == ('call', '0x1765df1')
blend = decode(0x18E7137, 0x18E71C3)
assert blend[0x18E7158] == ('push', '1')
assert blend[0x18E715A] == ('push', '1')
root = Path(__file__).resolve().parents[2]
source = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
body = source.split('if (effect.Type == kWrathBrokenBitmap)', 1)[1].split('return true;', 1)[0]
assert body.index('EnableAlphaBlend();') < body.index('RenderTerrainAlphaBitmap(')
assert body.index('RenderTerrainAlphaBitmap(') < body.index('RecordWrathGroundQA(effect);')
qa = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp').read_text()
sampler = qa.split('void RecordWrathGroundQA(', 1)[1].split('void RecordMagicFrameQA(', 1)[0]
for token in ('gControllerSkill != 278', 'effect.Type != kWrathBrokenBitmap',
              'gWrathGroundCount >= 32', 'glIsEnabled(GL_BLEND)',
              'glGetIntegerv(GL_BLEND_SRC', 'glGetIntegerv(GL_BLEND_DST',
              'glGetIntegerv(GL_TEXTURE_BINDING_2D'):
    assert token in sampler, token
assert sampler.index('gWrathGroundCount >= 32') < sampler.index('glIsEnabled(')
for token in ('AppendQALog(', 'glBlendFunc(', 'glEnable(', 'glDisable('):
    assert token not in sampler, token
native = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzOpenglUtil.cpp').read_text()
assert 'glBlendFunc(GL_ONE, GL_ONE);' in native.split('void EnableAlphaBlend()', 1)[1].split('void EnableAlphaBlendMinus()', 1)[0]
print('PASS: pinned S21 Wrath ground blend call/factors and native adapter ordering; visual QA OPEN')
