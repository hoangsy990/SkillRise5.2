"""Pin the S21 Breche model/particle retirement boundary.

The dump shows 0x806E and 0x5FD taking the default model-effect reset path;
their particle children are not sent through that destructor.  This verifier
is static evidence only and does not claim allocator or owner-pixel parity.
"""
import hashlib
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32

ROOT = Path(__file__).resolve().parents[2]
DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa"
    r"\memory_dump\process_23824\400000.main.exe"
)
IMAGE_BASE = 0x400000
EXPECTED_SHA256 = "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"


def decode(data: bytes, start: int, end: int):
    code = data[start - IMAGE_BASE:end - IMAGE_BASE]
    items = list(Cs(CS_ARCH_X86, CS_MODE_32).disasm(code, start))
    assert sum(i.size for i in items) == end - start
    return items


data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == EXPECTED_SHA256

destructor = decode(data, 0x14B6820, 0x14B6926)
dpins = {i.address: (i.mnemonic, i.op_str) for i in destructor}
assert dpins[0x14B687F] == ("jmp", "0x14b691b")
assert dpins[0x14B689F] == ("jmp", "0x14b691b")
assert dpins[0x14B691E] == ("call", "0x1315e97")
assert dpins[0x14B6834] == ("call", "0x13163fb")
compared_types = [i.op_str for i in destructor if i.mnemonic == "cmp" and "[ebp - 4]" in i.op_str]
assert compared_types == ["dword ptr [ebp - 4], 0x66d", "dword ptr [ebp - 4], 0x66d",
                          "dword ptr [ebp - 4], 0x4e5", "dword ptr [ebp - 4], 0x522",
                          "dword ptr [ebp - 4], 0x582", "dword ptr [ebp - 4], 0x66c",
                          "dword ptr [ebp - 4], 0x66e", "dword ptr [ebp - 4], 0x80ba",
                          "dword ptr [ebp - 4], 0x81cd"]
print("PASS: S21 destructor sends unlisted 0x806E/0x5FD model records to native reset 0x1315E97")

tail = decode(data, 0x15945E5, 0x1594709)
tpins = {i.address: (i.mnemonic, i.op_str) for i in tail}
assert tpins[0x15945EA] == ("call", "0x1595a11")
assert tpins[0x1594627] == ("cmp", "dword ptr [eax + 0x6c], 0")
assert tpins[0x1594630] == ("call", "0x14b6820")
assert [i.op_str for i in tail if i.mnemonic == "cmp" and ("0x413" in i.op_str or "0x7ef7" in i.op_str)] == [
    "dword ptr [ebp - 0x12464], 0x413", "dword ptr [ebp - 0x12464], 0x7ef7",
    "dword ptr [ebp - 0x12468], 0x413", "dword ptr [ebp - 0x12468], 0x7ef7",
    "dword ptr [ebp - 0x1246c], 0x413", "dword ptr [ebp - 0x1246c], 0x7ef7",
]
assert not any("0x806e" in i.op_str or "0x5fd" in i.op_str for i in tail)
print("PASS: common tail moves, checks life, and only special-decrements 0x413/0x7EF7")

effect = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.cpp").read_text(encoding="utf-8")
particle = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/ZzzEffectParticle.cpp").read_text(encoding="utf-8")
assert "EffectDestructor(o);" in effect
assert "EffectDestructor" not in particle[particle.index("if (g_brecheParticleMode[i] != 0)"):particle.index("if (g_brecheParticleMode[i] != 0)") + 1600]
print("PASS: RISE keeps model-effect reset and Breche PARTICLE retirement on separate native pools")
print("OPEN: allocator reuse/exhaustion, GPU state restoration and owner-target pixels still require QA")
