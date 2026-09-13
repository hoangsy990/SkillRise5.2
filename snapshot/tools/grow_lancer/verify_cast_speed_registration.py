"""Selected S21 player speed writes; common speed term remains to trace."""
import hashlib
import struct
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE
data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
decoded = {i.address: (i.mnemonic,i.op_str) for start,end in (
    (0x1409FD8,0x140A1D8),(0xD6DB7E,0xD6DBBA))
    for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start)}
for address, pair in {
    0x1409FE0: ('addss','xmm0, dword ptr [ebp - 0x24]'),
    0x1409FE5: ('push','0xb9'),
    0x140A006: ('movss','dword ptr [eax + 4], xmm0'),
    0x140A018: ('push','0xba'),
    0x140A03E: ('push','0xbb'),
    0x140A087: ('push','0xbc'),
    0x140A0BA: ('push','0xbd'),
    0x140A0ED: ('push','0xbe'),
    0x140A113: ('push','0xbf'),
    0x140A14F: ('push','0xc0'),
    0x140A18B: ('push','0xc1'),
    0x140A1C7: ('push','0xc2'),
    0xD6DB87: ('mov','dword ptr [ebp - 4], 0xa94'),
    0xD6DBAE: ('shl','ecx, 4'),
    0xD6DBB1: ('add','ecx, dword ptr [eax + 0x30]'),
}.items(): assert decoded[address] == pair, hex(address)
for address,value in ((0x1B50278,.13),(0x1B76150,.16),(0x1B4DF14,.5),
                      (0x1B4DF08,.2),(0x1B4E6E0,.3),(0x1B500E4,.35)):
    assert data[address-IMAGE_BASE:address-IMAGE_BASE+4] == struct.pack('<f',value)
prefix = list(decoder.disasm(data[0x1408811-IMAGE_BASE:0x140A1E5-IMAGE_BASE],0x1408811))
assert [(i.address,i.mnemonic,i.op_str) for i in prefix
        if i.op_str.startswith('dword ptr [ebp - 0x24],')] == [
            (0x140886F,'movss','dword ptr [ebp - 0x24], xmm0')]
pins = {i.address:(i.mnemonic,i.op_str) for i in prefix}
assert pins[0x1408862] == ('movss','xmm0, dword ptr [ebp + 8]')
assert pins[0x1408867] == ('mulss','xmm0, dword ptr [0x1b4ed70]')
assert data[0x1B4ED70-IMAGE_BASE:0x1B4ED74-IMAGE_BASE] == struct.pack('<f',.002)
assert pins[0x140A1D8] == ('movss','xmm0, dword ptr [0x1b500e4]')
assert pins[0x140A1E0] == ('movss','dword ptr [eax + 4], xmm0')
print('PASS actions185..194 speed writes; sole common-term producer is adjusted input8 multiplied by float0.002')
print('OPEN caller adjustment/status/cap semantics; do not substitute raw native AttackSpeed yet')
assert struct.unpack_from('<I',data,0x99772D+7*4-IMAGE_BASE)[0] == 0x9976E4
branch = list(decoder.disasm(data[0x9976E4-IMAGE_BASE:0x9976EB-IMAGE_BASE],0x9976E4))
assert [(i.mnemonic,i.op_str) for i in branch] == [('mov','eax, 0x111'),('jmp','0x997728')]
assert data[0x1B4E4D4-IMAGE_BASE:0x1B4E4D8-IMAGE_BASE] == struct.pack('<f',20.0)
print('PASS class7 fallback273 and caller flag-bonus constant20; fallback is not universal local-player policy')
calls = {i.address:(i.mnemonic,i.op_str) for start,end in (
    (0x12FB28A,0x12FB2B1),(0x18CE254,0x18CE25C),(0x140DD63,0x140DD87))
    for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start)}
for address,pair in {
    0x12FB290:('call','0x14087c8'),
    0x12FB298:('push','0xe0'),
    0x12FB2A9:('call','0x1327de8'),
    0x18CE254:('push','0'),
    0x18CE256:('call','0x1408580'),
    0x140DD66:('add','eax, 0x3254'),
    0x140DD81:('call','0x1408580'),
}.items(): assert calls[address] == pair,hex(address)
print('PASS observed class-fallback caller precedes action224, not a general remote GrowLancer policy; player setup calls real-stat path with null object')

# Anchor the complete caller, not a numeric-xref-sized window.
caller = {i.address:(i.mnemonic,i.op_str) for i in decoder.disasm(
    data[0x1408580-IMAGE_BASE:0x14087C8-IMAGE_BASE],0x1408580)}
for address,pair in {
    0x14086AF:('movzx','eax, byte ptr [eax + 0x1a6]'),
    0x14086B6:('and','eax, 1'),
    0x14086C0:('addss','xmm0, dword ptr [0x1b4e4d4]'),
    0x14086DF:('jmp','0x1408716'),
    0x14086ED:('and','eax, 8'),
    0x14086F7:('addss','xmm0, dword ptr [0x1b4e4d4]'),
    0x1408716:('cmp','dword ptr [ebp + 8], 0'),
    0x140871A:('je','0x140875a'),
    0x140871C:('push','0xb0'),
    0x140872A:('call','0x130c8a5'),
    0x1408734:('je','0x140875a'),
    0x140873B:('mulss','xmm0, dword ptr [0x1b4df14]'),
    0x140874D:('mulss','xmm0, dword ptr [0x1b4df14]'),
    0x140876C:('je','0x14087a9'),
    0x1408771:('call','0xb608f4'),
    0x1408784:('comiss','xmm0, dword ptr [ebp - 0xc]'),
    0x1408788:('jbe','0x1408794'),
    0x140878F:('movss','dword ptr [ebp - 8], xmm0'),
    0x140879D:('jbe','0x14087a9'),
    0x14087A4:('movss','dword ptr [ebp - 4], xmm0'),
    0x14087BF:('call','0x1408811'),
    0x14087C7:('ret',''),
}.items(): assert caller[address] == pair,hex(address)
print('PASS source tail: flag1 OR flag8 adds20 once, optional object Tornado176 halves both speeds, optional cap follows; native input mapping remains OPEN')

from pathlib import Path
root = Path(__file__).resolve().parents[2]
native = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text(encoding='utf-8')
helper = native.split('bool PrepareFixedPlayerAction(',1)[1].split('void TransformFromObject',1)[0]
for token in ('actor.Type != MODEL_PLAYER','action < 284 || action > 293',
              '!CanPlayAction(actor, action)', '!Models[MODEL_PLAYER].Actions',
              'S21FixedCastAnimationSpeed(action - 284 + 185)',
              'Models[MODEL_PLAYER].Actions[action].PlaySpeed = speed;'):
    assert token in helper,token
for name,action in [('CreateClashRoot',293),('CreateObsidianRoots',290),
                    ('CreateWrathRoot',291),('CreateCircleShieldRoot',286)]:
    body=native.split('void '+name+'(',1)[1].split('\nvoid ',1)[0]
    assert body.index(f'PrepareFixedPlayerAction(caster, {action})') < body.index('caster.CurrentAction =')
print('PASS four fixed-speed native cast paths prepare player-only private actions before assignment; dynamic-speed paths remain OPEN')
qa_helper=native.split('bool PrepareLocalQADynamicAction(',1)[1].split('void TransformFromObject',1)[0]
for token in ('#ifdef RISE_GROW_LANCER_RUNTIME_QA','&actor != &Hero->Object',
              'CharacterAttribute != &CharacterMachine->Character',
              'GetBaseClass(CharacterAttribute->Class) != CLASS_KNIGHT',
              'action < 284 || action > 289 || action == 286',
              'static_cast<float>(CharacterAttribute->AttackSpeed)',
              'S21GrowLancerInitialSpeedLimit', '#else'):
    assert token in qa_helper,token
assert 'S21AdjustCastSpeedTail(' not in qa_helper
assert 'return false;' in qa_helper.split('#else',1)[1]
for name,action in [('CreateSpinStepRoot',285),('CreateMagicPinRoots',287),
                    ('CreateHarshStrikeRoot',284),('CreateShiningPeakRoots',288),
                    ('CreateBrecheAction',289)]:
    body=native.split('void '+name+'(',1)[1].split('\nvoid ',1)[0]
    assert body.index(f'PrepareLocalQADynamicAction(caster, {action})') < body.index('caster.CurrentAction =')
print('PASS five local-DK QA dynamic cast entrypoints register speed without duplicate ability bonus; remote/non-QA stays disabled pending final contracts')
