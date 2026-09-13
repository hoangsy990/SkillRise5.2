"""Selected S21 mesh callbacks and legacy bounds guard, not GPU parity."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    def decode(start, end):
        return list(decoder.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE], start))
    registration = {i.address: i for i in decode(0xA1B2DF, 0xA1B323)}
    for address, value in ((0xA1B2DF, '0xa49d77'), (0xA1B2E4, '0x5f4'),
        (0xA1B2F0, '0xa49dd2'), (0xA1B2F5, '0x5f5'),
        (0xA1B312, '0xa49ee5'), (0xA1B317, '0x5f7')):
        assert (registration[address].mnemonic, registration[address].op_str) == ('push', value)
    for start, end, count, multiplies in ((0xA49D77,0xA49DD2,1,0),
        (0xA49DD2,0xA49EDE,1,3), (0xA49EE5,0xA49FE8,3,0)):
        code = decode(start, end)
        assert sum(i.mnemonic == 'call' and i.op_str == '0x968cb3' for i in code) == count
        assert sum(i.mnemonic == 'mulss' for i in code) == multiplies
    guard = {i.address: i for i in decode(0x13312EF,0x1331335)}
    for address, pair in {
        0x1331321: ('movsx','eax, word ptr [eax + 0x24]'),
        0x1331325: ('cmp','dword ptr [ebp + 8], eax'),
        0x1331328: ('jge','0x1331330'),
        0x133132A: ('cmp','dword ptr [ebp + 8], 0'),
        0x133132E: ('jge','0x1331335'),
        0x1331330: ('jmp','0x1333a15'),
    }.items():
        assert (guard[address].mnemonic,guard[address].op_str) == pair
    native = (Path(__file__).resolve().parents[2] /
        'ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp').read_text()
    assert 'if (meshIndex >= NumMeshs || meshIndex < 0) return;' in native
    shader = (Path(__file__).resolve().parents[2] /
        'ExMain_RISE_PC/Main5.2_RISE/New_ModelBMD.cpp').read_text()
    shader_start = shader.index('bool OGL330MODEL::CGMMeshShader::AddMeshCommand(')
    shader_guard = shader[shader_start:shader.index('VAOMesh& newMesh',shader_start)]
    assert 'meshIndex < 0 || meshIndex >= source->NumMeshs' in shader_guard
    assert 'meshIndex >= static_cast<int>(source->NewMeshes->size())' in shader_guard
    defaults = {i.address:i for i in decode(0x1315E97,0x1315EE6)}
    for address, operand in ((0x1315EDB,'byte ptr [eax + 6], 1'),
                             (0x1315EE2,'byte ptr [eax + 7], 1')):
        assert (defaults[address].mnemonic,defaults[address].op_str) == ('mov',operand)
    create = {i.address:i for i in decode(0x143E719,0x143E753)}
    assert create[0x143E71F].op_str == '0x1315e97'
    assert create[0x143E74F].op_str == 'byte ptr [eax + 5], 1'
    runtime = (Path(__file__).resolve().parents[2] /
        'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
    start = runtime.index('// Body/spin callbacks')
    branch = runtime[start:runtime.index('int meshCount',start)]
    assert 'if (effect.Type == kShiningPeakPinModel)' in branch
    assert branch.count('model.BodyLight[') == 3
    print('PASS: three registrations, callback counts/multiply distinction, S21 and native legacy mesh bounds')
    print('PASS: default lighting, pin-only override and shader bounds; buff/terrain/GPU remain QA gates.')


if __name__ == '__main__':
    main()
