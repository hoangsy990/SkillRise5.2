"""Pinned AD6/subtype6 render-call contract; not visual acceptance."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    assert hashlib.sha256(data).hexdigest() == "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    instructions = {}
    for start, end in ((0xA1CF92, 0xA1CFA3), (0xA72AD0, 0xA73123)):
        instructions.update({i.address: i for i in decoder.disasm(
            data[start-IMAGE_BASE:end-IMAGE_BASE], start)})
    pins = {
        0xA1CF92: ("push", "0xa72ad0"),
        0xA1CF97: ("push", "0xad6"),
        0xA1CF9C: ("call", "0xa1dce6"),
        0xA72F40: ("jne", "0xa7300c"),
        0xA73075: ("push", "-1"),
        0xA730BD: ("push", "0x42"),
        0xA730BF: ("push", "0"),
        0xA730C4: ("call", "0x968cb3"),
        0xA730C9: ("push", "-1"),
        0xA73111: ("push", "0x42"),
        0xA73113: ("push", "0"),
        0xA73118: ("call", "0x968cb3"),
    }
    for address, pair in pins.items():
        i = instructions[address]
        assert (i.mnemonic, i.op_str) == pair, hex(address)
    body = [i for i in instructions.values() if 0xA7300C <= i.address < 0xA7311E]
    assert sum(i.mnemonic == "call" and i.op_str == "0x968cb3" for i in body) == 2
    assert not any(i.mnemonic.startswith("mul") for i in body)
    runtime = (Path(__file__).resolve().parents[2] /
        "ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp").read_text()
    begin = runtime.index("// S21 AD6 registration")
    branch = runtime[begin:runtime.index("return true;", begin)]
    assert "VectorCopy(effect.Light, model.BodyLight)" in branch
    assert "pass < 2" in branch and "effect.BlendMeshLight" in branch
    assert "model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT" in branch
    print("PASS: AD6 registration, subtype6 two mesh0 calls, raw Light and private native adapter")
    print("Scope excludes GPU parity, transform and whole-skill acceptance.")


if __name__ == "__main__":
    main()
