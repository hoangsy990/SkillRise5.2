"""Pinned Shining Peak joint evidence, not whole-engine equivalence."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=="6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    instructions={}
    for start,end in ((0x15DE358,0x15DE623),(0x1612773,0x1612CBD),
                      (0x1618F44,0x161AEB9), (0x10F0332,0x10F0381),
                      (0x15C65D6,0x15C6715), (0x15E21B9,0x15E21F1),
                      (0x15C6715,0x15C7274), (0x1618ADA,0x1618B27),
                      (0x15E37EF,0x15E3901), (0x8DDA8B,0x8DDAB9),
                      (0x10F040E,0x10F04D2), (0x15E2604,0x15E26A0)):
        instructions.update({i.address:i for i in decoder.disasm(
            data[start-IMAGE_BASE:end-IMAGE_BASE],start)})
    expected={0x15DE384:("je","0x15de53a"),0x15DE391:("je","0x15de53a"),
        0x15DE39E:("je","0x15de53a"),0x15DE550:("mov","byte ptr [eax + 0xa25], 3"),
        0x15DE573:("mov","dword ptr [eax + 0xa00], 0x14"),
        0x15DE593:("mov","dword ptr [eax + 0x6c], 0x12"),
        0x161279C:("je","0x1612963"),0x16127A9:("je","0x1612963"),
        0x16127B6:("je","0x1612963"),0x161952E:("cmp","eax, 3"),
        0x161954F:("subss","xmm1, xmm0"),0x1619577:("subss","xmm1, xmm0")}
    for address,pair in expected.items():
        i=instructions[address]; assert (i.mnemonic,i.op_str)==pair,hex(address)
    for address,pair in {
        0x15C65D6:('cmp','dword ptr [ebp - 0x234], 0x1f4'),
        0x15C6609:('call','0x10f0332'),
        0x15C6617:('call','0x10f02ce'),
        0x10F0344:('cmp','dword ptr [ebp + 0x10], eax'),
        0x10F0353:('cmp','dword ptr [ebp - 4], 0x7fce'),
        0x10F035C:('jmp','0x10f037b'),
        0x10F037B:('xor','eax, eax'),
        0x15C670A:('mov','cx, word ptr [ebp + 0x38]'),
        0x15C670E:('mov','word ptr [eax + 0xa44], cx'),
        0x15E21BF:('cmp','dword ptr [eax + 0x6c], 0x32'),
        0x15E21CB:('mov','dword ptr [eax + 0x6c], 0x32'),
    }.items():
        i=instructions[address]; assert (i.mnemonic,i.op_str)==pair,hex(address)
    for address,value in ((0x1B4E934,60.),(0x1B99910,135.),(0x1B502EC,.88),
        (0x1B6C6B4,32.),(0x1B4FA9C,-5.),(0x1B4E4D8,30.),
        (0x1B4EDF4,-15.),(0x1B4E860,3.)):
        assert abs(struct.unpack_from('<f',data,address-IMAGE_BASE)[0]-value)<.000001
    for i in instructions.values():
        if 0x1612963<=i.address<0x1612CBD or 0x1618F44<=i.address<0x161AEB9:
            assert '0xa44]' not in i.op_str,hex(i.address)
    print('PASS12 dispatch/constructor/UV pins,8 constants; no direct+A44 operand in selected body/renderer')
    print('PASS special pool predicate excludes8073; ordinary500-slot allocation and final50-tail cap')
    for address,pair in {
        0x15C6B2B:('and','dword ptr [eax + 0x68], 0'),
        0x15C6B2F:('mov','byte ptr [ebp - 0x229], 1'),
        0x15C6D42:('call','0xd30b1a'),
        0x15C6D4F:('movss','xmm0, dword ptr [eax + 0x14]'),
        0x15C6E9E:('movss','xmm0, dword ptr [eax + 0x14]'),
        0x15C7004:('movss','xmm0, dword ptr [eax + 0x14]'),
        0x15C7150:('movss','xmm0, dword ptr [eax + 0x14]'),
        0x15C6D98:('call','0xd3189d'), 0x15C6EE0:('call','0xd3189d'),
        0x15C702F:('call','0xd3189d'), 0x15C7174:('call','0xd3189d'),
    }.items():
        i=instructions[address]; assert (i.mnemonic,i.op_str)==pair,hex(address)
    assert struct.unpack_from('<f',data,0x1B4DF14-IMAGE_BASE)[0] == .5
    # Pin type does not match any of the initial-tail suppression type tests.
    types={i.op_str for i in instructions.values() if 0x15C6B36<=i.address<0x15C6D20
           and i.mnemonic=='cmp' and i.op_str.startswith('dword ptr [ebp + 8],')}
    assert types == {f'dword ptr [ebp + 8], {hex(t)}' for t in
                     (0x7F79,0x7FF9,0x7FD6,0x8106,0x814B,0x809D,0x7FDE,0x472,0x7FE5)}
    native=(Path(__file__).resolve().parents[2]/'ExMain_RISE_PC/Main5.2_RISE/ZzzEffectJoint.cpp').read_text(encoding='utf-8-sig')
    geometry=native.split('if (bCreateStartTail)',1)[1].split('vec3_t BitePosition;',1)[0]
    for vector in ('-o->Scale * 0.5f, 0.f, 0.f','o->Scale * 0.5f, 0.f, 0.f',
                   '0.f, 0.f, -o->Scale * 0.5f','0.f, 0.f, o->Scale * 0.5f'):
        assert f'Vector({vector}, Position);' in geometry
    assert native.index('if (bCreateStartTail)') < native.index('rise::growlancer::InitPinJoint(*o, Scale);')
    print('PASS initial-tail enabled, four stored-scale half-width vectors before subtype init; native order matches')
    for address,pair in {
        0x1618B13:('push','0'), 0x1618B1F:('call','0x15e2756'),
        0x15E37F5:('inc','eax'),
        0x15E37F9:('mov','dword ptr [ecx + 0x68], eax'),
        0x15E382F:('jl','0x15e3901'),
        0x15E386B:('inc','eax'),
        0x15E3883:('mov','dword ptr [eax], ecx'),
        0x1619486:('cmp','ecx, dword ptr [eax + 0x68]'),
        0x1619489:('jge','0x161ad97'),
        0x161AB7C:('inc','eax'),
        0x161AB83:('lea','ecx, [ecx + eax + 0x70]'),
    }.items():
        i=instructions[address]; assert (i.mnemonic,i.op_str)==pair,hex(address)
    tails=['seed']+[None]*17
    count=0
    for tick in range(1,19):
        count=min(count+1,17)
        for j in range(count-1,-1,-1): tails[j+1]=tails[j]
        tails[0]=tick
        if tick==5: assert count==5 and tails[5]=='seed'
        if tick==17: assert tails[17]=='seed'
        if tick==18: assert 'seed' not in tails
    print('PASS tail-history model: seed survives light-on tick5 through tick17; overwritten tick18')
    for address,pair in {
        0x8DDA93:('push','0x10f040e'), 0x8DDA98:('push','0x1f4'),
        0x8DDA9D:('push','0xa64'), 0x8DDAA2:('push','0xa5e15b0'),
        0x10F0498:('push','0xcc'), 0x10F049D:('push','0xc'),
        0x15E265D:('mov','byte ptr [eax], 0'),
        0x15E2696:('mov','byte ptr [eax], 0'),
    }.items():
        i=instructions[address]; assert (i.mnemonic,i.op_str)==pair,hex(address)
    ctor=[i for i in instructions.values() if 0x10F040E<=i.address<0x10F04D2]
    assert ctor[-1].mnemonic=='ret'
    assert [i.op_str for i in ctor if i.mnemonic=='call'] == ['0x960443']*6+['0x19b8e3f','0x960443']
    assert all('0x14]' not in i.op_str for i in ctor)
    print('PASS array constructor/vector initialization and explicit delete Live-only writes; no Scale reset established')
    print('Scope excludes indirect external access, geometry, pool and visual parity.')

if __name__=='__main__': main()
