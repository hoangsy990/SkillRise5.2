"""Pinned base Obsidian mesh/blend indices and private source mapping."""
import hashlib
from pathlib import Path
from capstone import Cs,CS_ARCH_X86,CS_MODE_32
from disasm_s21_window import DUMP,IMAGE_BASE
def main():
    data=DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder=Cs(CS_ARCH_X86,CS_MODE_32)
    ins={i.address:i for i in decoder.disasm(data[0xA48B0E-IMAGE_BASE:0xA48EF3-IMAGE_BASE],0xA48B0E)}
    for blend,flags,mesh,call,index in ((0xA48C5F,0xA48C72,0xA48C74,0xA48C79,0),(0xA48DA1,0xA48DB4,0xA48DB6,0xA48DBB,0),(0xA48EC6,0xA48ED9,0xA48EDB,0xA48EE0,1)):
        for address,pair in ((blend,('push',str(index))),(flags,('push','0x42')),(mesh,('push',str(index))),(call,('call','0x968cb3'))):
            assert (ins[address].mnemonic,ins[address].op_str)==pair,hex(address)
    root=Path(__file__).resolve().parents[2]
    source=(root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    start=source.index('if (effect.Type == kObsidianRotatingModel)')
    body=source[start:source.index('if (effect.Type != kClashFrontModel',start)]
    assert '0, effect.Alpha, effect.BlendMeshTexCoordU' in body
    assert 'effect.Alpha, mesh, effect.Alpha,' in body
    assert '-2' not in body
    print('PASS Obsidian rotating blend0 and aura mesh/blend0/1; no GPU parity claim')
if __name__=='__main__':main()
