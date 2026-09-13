"""Selected pinned Clash renderer arguments; not visual or cache parity."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE

def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    instructions = {i.address:(i.mnemonic,i.op_str) for i in decoder.disasm(
        data[0xA4A102-IMAGE_BASE:0xA4A356-IMAGE_BASE],0xA4A102)}
    for address, pair in {
        0xA4A1C1:('subss','xmm0, dword ptr [0x1b4e6d4]'),
        0xA4A1DF:('subss','xmm0, dword ptr [0x1b4df00]'),
        0xA4A21A:('movss','xmm0, dword ptr [eax + 0xdc]'),
        0xA4A227:('push','0'),0xA4A23A:('push','0x42'),
        0xA4A23C:('push','0'),0xA4A241:('call','0x968cb3'),
        0xA4A326:('movss','xmm0, dword ptr [eax + 0xdc]'),
        0xA4A333:('push','0'),0xA4A346:('push','0x42'),
        0xA4A348:('push','0'),0xA4A34D:('call','0x968cb3'),
    }.items():
        assert instructions[address] == pair, hex(address)
    root = Path(__file__).resolve().parents[2]
    source = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    start = source.index('if (effect.Type != kClashFrontModel && effect.Type != kClashRearModel)')
    body = source[start:source.index('bool RenderBrecheClippedMesh',start)]
    assert 'effect.BlendMeshTexCoordU -= 0.05f;' in body
    assert 'effect.BlendMeshTexCoordV -= 0.01f;' in body
    for axis in range(3):
        assert f'model.BodyLight[{axis}] = effect.Light[{axis}] * effect.Alpha;' in body
    assert 'model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha, 0,' in body
    assert 'effect.Alpha, effect.BlendMeshTexCoordU, effect.BlendMeshTexCoordV,' in body
    print('PASS selected Clash mesh0/blend0/alpha/UV arguments; GPU and cache lifecycle OPEN')

if __name__ == '__main__':
    main()
