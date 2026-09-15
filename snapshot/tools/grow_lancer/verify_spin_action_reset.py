"""Pin the Spin local action edge and the isolated native/QA contract.

This is static action evidence, never a claim about effect allocation or pixels.
"""
import hashlib
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == (
    '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
)
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
start, end = 0x10E3A86, 0x10E3AA3
rows = list(decoder.disasm(data[start - IMAGE_BASE:end - IMAGE_BASE], start))
assert rows and rows[-1].address + rows[-1].size == end
assert [(row.address, row.mnemonic, row.op_str) for row in rows] == [
    (0x10E3A86, 'push', '-1'),
    (0x10E3A88, 'push', '0xba'),
    (0x10E3A8D, 'mov', 'eax, dword ptr [ebp - 0xf560]'),
    (0x10E3A93, 'push', 'dword ptr [eax + 0xc]'),
    (0x10E3A96, 'mov', 'eax, dword ptr [ebp - 0xf560]'),
    (0x10E3A9C, 'push', 'dword ptr [eax]'),
    (0x10E3A9E, 'call', '0x1327de8'),
]

root = Path(__file__).resolve().parents[2]
native = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
spin = native.split('bool CreateSpinStepRoot(OBJECT& caster, short targetIndex)', 1)[1].split('\n}', 1)[0]
assert spin.index('PrepareLocalQADynamicAction(caster, 285)') < spin.index('SetAction(&caster, 285, true);')
assert 'caster.AnimationFrame =' not in spin
assert 'caster.PriorAnimationFrame =' not in spin
assert 'return false;' in spin and 'return true;' in spin

qa = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp').read_text()
cast = qa.split('void CastCurrentSkill()', 1)[1].split('\nvoid EmitCurrentContact()', 1)[0]
for token in (
    'baseClass != CLASS_KNIGHT',
    '(skill == 271 || skill == 275 || skill == 279) && !SelectedTarget()',
    'case 271: actionPrepared = CreateSpinStepRoot(',
    'if (!actionPrepared)',
    'cast-rejected-action-model-unavailable',
):
    assert token in cast, token
assert cast.index('if (!actionPrepared)') < cast.index('"root-dispatched"')
receive = (root / 'ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp').read_text(encoding='latin-1')
dispatch = receive.split('switch (MagicNumber)', 1)[1].split('case AT_SKILL_', 1)[0]
for skill_name, root_call in (
    ('kSpinStepSkill', 'CreateSpinStepRoot'),
    ('kMagicPinSkill', 'CreateMagicPinRoots'),
):
    branch = dispatch.split('case rise::growlancer::' + skill_name + ':', 1)[1].split('\n\tcase ', 1)[0]
    assert 'if (rise::growlancer::' + root_call + '(' in branch
    assert branch.count('sc->AttackTime = 1;') == 1
print('PASS pinned S21 local Spin186/-1 setter edge and native private285 transition')
print('PASS QA F7 class/target/action prerequisites reject false root-dispatched status')
print('PASS received Spin/Magic attack-time arm depends on root action/model prerequisite')
print('OPEN selected-target visual fanout, internal280 contact and cleanup acceptance')
