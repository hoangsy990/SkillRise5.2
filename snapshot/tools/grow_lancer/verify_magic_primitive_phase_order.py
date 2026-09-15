"""Pinned effect/joint/particle phase order; not full primitive lifecycle QA."""
import hashlib
from pathlib import Path
from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from disasm_s21_window import DUMP, IMAGE_BASE
data=DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest()=='6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
d=Cs(CS_ARCH_X86,CS_MODE_32)
rows={}
for start,end in ((0x18F4140,0x18F417A),(0x1618EF4,0x1618F44),(0x1620972,0x1620984)):
    instructions=list(d.disasm(data[start-IMAGE_BASE:end-IMAGE_BASE],start))
    assert sum(i.size for i in instructions)==end-start
    rows.update({i.address:(i.mnemonic,i.op_str) for i in instructions})
for address,pair in {
    0x18F4140:('call','0x13f3fb0'),
    0x18F4158:('call','0x1595834'),
    0x18F415D:('call','0x1618ef4'),
    0x18F4167:('call','0x1620972'),
    0x1618F06:('cmp','dword ptr [ebp - 4], 0x1f4'),
    0x1618F0F:('imul','eax, dword ptr [ebp - 4], 0xa64'),
    0x1618F21:('movzx','eax, byte ptr [eax]'),
    0x1618F26:('je','0x1618f35'),
    0x1618F2E:('call','0x15e5838'),
    0x1618F3C:('call','0x10f0381'),
    0x162097C:('call','0x16aa2ed'),
}.items(): assert rows[address]==pair,hex(address)
root=Path(__file__).resolve().parents[2]
native=(root/'ExMain_RISE_PC/Main5.2_RISE/ZzzScene.cpp').read_text(encoding='latin-1')
game=native.split('void MoveMainScene()',1)[1].split('bool RenderMainScene()',1)[0]
calls=['MoveCharactersClient();','MoveEffects();','MoveJoints();','MoveParticles();']
positions=[game.index(call) for call in calls]
assert positions==sorted(positions)
print('PASS selected S21 phase: characters -> model effects -> joints -> particle manager')
print('PASS joint traversal500 primary slots then secondary; live records invoke15E5838')
print('PASS native gameplay has the same phase order; do not reorder global pools')
print('OPEN particle container iteration/newborn semantics, sprite lifecycle, private per-tick filtering and runtime')
