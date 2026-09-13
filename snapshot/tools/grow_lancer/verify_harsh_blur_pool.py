"""Selected object-blur pool semantics, excluding frame cadence and GPU QA."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    code={}
    for start,end in ((0x15C01CC,0x15C030C),(0x15C030C,0x15C04B5),
                      (0x15C22D8,0x15C2558)):
        code.update({i.address:i for i in decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start)})
    pins={
        0x15C01DF:('cmp','dword ptr [ebp - 8], 0x3e8'),
        0x15C020A:('jne','0x15c023e'),
        0x15C0210:('jle','0x15c0222'),
        0x15C0231:('call','0x15c22d8'),
        0x15C02B8:('mov','dword ptr [ebp - 0x14], 0x1e'),
        0x15C0350:('dec','eax'),0x15C035D:('dec','eax'),
        0x15C0367:('cmp','dword ptr [eax + 8], 0'),
        0x15C0377:('mov','byte ptr [eax], 0'),
        0x15C2542:('cmp','dword ptr [eax + 0x10], 0x257'),
        0x15C254E:('mov','dword ptr [eax + 0x10], 0x257'),
    }
    for address,pair in pins.items():
        assert (code[address].mnemonic,code[address].op_str)==pair,hex(address)
    native=(Path(__file__).resolve().parents[2]/
        'ExMain_RISE_PC/Main5.2_RISE/ZzzEffectBlurSpark.cpp').read_text()
    for text in ('MAX_OBJECTBLURS\t\t1000','MAX_OBJECTBLUR_TAILS 600',
        'MAX_OBJECTBLUR_LIFETIME 30','b->Live && b->Owner==Owner',
        'SubType > 0 && b->SubType != SubType'):
        assert text in native,text
    add=native[native.index('void AddObjectBlur('):native.index('void CreateObjectBlur(')]
    assert 'LifeTime' not in add
    print('PASS blur capacity,group reuse,lifetime30,no append refresh,expiry and599tail clamp')
    print('Excludes tick scheduling,pool stress,owner reuse and rendered parity.')

if __name__=='__main__': main()
