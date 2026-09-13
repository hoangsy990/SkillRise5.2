"""Pinned wind01 material contract, not rendered parity."""
import hashlib
import struct
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    code=list(Cs(CS_ARCH_X86,CS_MODE_32).disasm(
        data[0xA48821-IMAGE_BASE:0xA48A5B-IMAGE_BASE],0xA48821))
    calls=[i.address for i in code if i.mnemonic=='call' and i.op_str=='0x968cb3']
    assert calls==[0xA488B4,0xA4893E,0xA489C8,0xA48A52]
    assert not any(i.mnemonic=='mulss' for i in code)
    instructions={i.address:i for i in code}
    for address in (0xA4887E,0xA48887,0xA48908,0xA48911,
                    0xA48992,0xA4899B,0xA48A1C,0xA48A25):
        i=instructions[address]
        assert (i.mnemonic,i.op_str)==('xorps','xmm0, xmm0')
    for address,value in ((0x1B4DD48,1.),(0x1B4E6E0,.3),
                          (0x1B4FB08,.95),(0x1B4E6DC,.25)):
        assert abs(struct.unpack_from('<f',data,address-IMAGE_BASE)[0]-value)<.000001
    runtime=(Path(__file__).resolve().parents[2]/
        'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    start=runtime.index('const float colors[4][3]')
    branch=runtime[start:runtime.index('\n        else',start)]
    assert 'VectorCopy(colors[mesh], model.BodyLight)' in branch
    assert '0.0f, 0.0f, -1)' in branch
    assert '* effect.Alpha' not in branch
    print('PASS four wind01 draws,raw material RGB,zero UV and private source mapping')

if __name__=='__main__': main()
