"""Pinned S21 storage behavior and native wire-width mismatch; no activation."""
from pathlib import Path
import hashlib
import re
from capstone import Cs, CS_ARCH_X86, CS_MODE_32

root = Path(__file__).resolve().parents[2]
dump = Path(r'D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe').read_bytes()
assert hashlib.sha256(dump).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
instructions = {}
for start, end in ((0x130C63E, 0x130C6CD), (0x130C769, 0x130C7ED),
                   (0x130C8A5, 0x130C8FE)):
    decoded = list(decoder.disasm(dump[start-0x400000:end-0x400000], start))
    assert decoded[-1].mnemonic == 'ret'
    instructions.update((i.address, (i.mnemonic, i.op_str)) for i in decoded)
for address, expected in {
    0x130C655: ('call', '0x130f0cb'),
    0x130C678: ('je', '0x130c6c9'),
    0x130C67A: ('push', '0xc'),
    0x130C68E: ('mov', 'dword ptr [eax], 1'),
    0x130C697: ('mov', 'ecx, dword ptr [ebp + 0xc]'),
    0x130C69A: ('mov', 'dword ptr [eax + 4], ecx'),
    0x130C6A0: ('or', 'dword ptr [eax + 8], 0xffffffff'),
    0x130C6A8: ('lea', 'eax, [ebp + 8]'),
    0x130C6C3: ('call', '0x130dcd5'),
    0x130C7BE: ('mov', 'eax, dword ptr [eax + 4]'),
    0x130C7CA: ('push', '0xc'),
    0x130C7CF: ('call', '0x19b902c'),
    0x130C7E3: ('call', '0x130dc61'),
    0x130C8C1: ('lea', 'eax, [ebp + 8]'),
    0x130C8CF: ('call', '0x130f0cb'),
}.items():
    assert instructions[address] == expected, hex(address)

pc = root/'ExMain_RISE_PC/Main5.2_RISE'
header = (pc/'WSclient.h').read_bytes()
status = header.split(b'} PMSG_VIEWSKILLSTATE,', 1)[0].rsplit(b'typedef struct', 1)[1]
assert re.search(rb'\bBYTE\s+BuffIndex\s*;', status)
assert len(re.findall(rb'\bBYTE\s+s_BuffEffectState\[MAX_BUFF_SLOT_INDEX\]', header)) == 5
assert b'typedef std::map<eBuffState, DWORD>    BuffStateMap;' in (pc/'_types.h').read_bytes()
native = (pc/'w_Buff.cpp').read_bytes().split(b'void Buff::RegisterBuff( eBuffState buffstate )', 1)[1]
native = native.split(b'void Buff::RegisterBuff(std::list', 1)[0]
assert b'if( iter == m_Buff.end() )' in native
assert b'm_Buff.insert(std::make_pair( buffstate, 1 ) );' in native
for index in (274, 278, 424, 425):
    assert index > 255
    print(f'WIDTH MISMATCH buff {index}: byte would truncate to {index & 255}; never use that alias')
print('PASS S21 insert-only 12-byte payload / removal path; native map stores DWORD value')
print('PASS current native status packet and five viewport lists use BYTE buff IDs')
gs = root/'ExGameServer/GameServer'
manager_header = (gs/'EffectManager.h').read_bytes()
assert re.search(rb'#define\s+MAX_EFFECT\s+256\b', manager_header)
assert re.search(rb'#define\s+MAX_EFFECT_LIST\s+32\b', manager_header)
assert re.search(rb'\bBYTE\s+m_index\s*;', (gs/'Effect.h').read_bytes())
effect = (gs/'Effect.cpp').read_bytes()
assert b'this->m_index = 0xFF;' in effect
assert b'if(this->m_index == 0xFF)' in effect
manager = (gs/'EffectManager.cpp').read_bytes()
assert b'void CEffectManager::GCEffectStateSend(LPOBJ lpObj, BYTE state, BYTE effect)' in manager
assert b'pMsg.header.set(0x07, sizeof(pMsg));' in manager
assert b'pMsg.header.set(0x2D, sizeof(pMsg));' in manager
assert b'lpMsg[0] = lpEffect->m_index;' in manager
for filename in (gs/'DSProtocol.h', root/'ExDataServer/DataServer/DataServerProtocol.h'):
    assert len(re.findall(rb'BYTE\s+Effect\[MAX_EFFECT_LIST\]\[13\]', filename.read_bytes())) == 2
assert re.search(rb'struct PMSG_VIEWPORT_STATE\s*\{\s*BYTE\s+effect;', (gs/'Viewport.h').read_bytes())
print('PASS GS IDs=256 vs actor slots=32; BYTE index with 255 empty sentinel')
print('PASS legacy 0x07/0x2D sender widths and GS/DS 13-byte effect record declarations')
print('OPEN S21 ingress, payload field meanings, negotiated server/client buff extension and live QA')
