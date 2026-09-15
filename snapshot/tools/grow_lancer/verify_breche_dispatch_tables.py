"""Pinned table checks only; not a full chain or runtime verifier."""
from pathlib import Path
import hashlib
import struct

DUMP = Path(r'D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe')
BASE = 0x400000


def main():
    data = DUMP.read_bytes()
    if hashlib.sha256(data).hexdigest() != '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637':
        raise ValueError('Pinned Season21 mapped image hash changed')
    # ProtocolCore12FE380 bounds its first argument to0xFD and dispatches
    # directly through130505C. DF selects the receive caller12FEB99.
    protocol_entry = struct.unpack_from('<I', data, 0x130505C - BASE + 0xDF*4)[0]
    if protocol_entry != 0x12FEB99:
        raise ValueError('S21 skill receive opcode dispatch changed')
    call = 0x12FEBA2 - BASE
    if data[call] != 0xE8 or BASE + call + 5 + struct.unpack_from('<i', data, call+1)[0] != 0x12C4F86:
        raise ValueError('S21 skill receive direct call changed')
    print('PASS: pinned S21 opcode DF ->12FEB99 ->receive12C4F86; not native5.2 opcode19')
    selector = data[0x12CE942 - BASE]
    receive = struct.unpack_from('<I', data, 0x12CE916 - BASE + selector*4)[0]
    constructor = struct.unpack_from('<I', data, 0x14B3224 - BASE + (0x5FD-0x4F7)*4)[0]
    if (selector, receive, constructor) != (0, 0x12CB7F5, 0x148251E):
        raise ValueError('Breche table evidence changed')
    # 0x15748ED routes types >0x5F2 to0x1574AA4, NOT0x1574A79.
    # The latter table ends at0x5F1; indexing it with0x5FD reads another table.
    root_type = 0x5FD
    if not 0x5F2 < root_type < 0x64B:
        raise ValueError('Root no longer selects the audited secondary branch')
    secondary_index = root_type - 0x5F3
    if not 0 <= secondary_index <= 0x48:
        raise ValueError('Secondary selector index outside audited bounds')
    secondary_selector = data[0x1595728 - BASE + secondary_index]
    secondary_target = struct.unpack_from(
        '<I', data, 0x15956A4 - BASE + secondary_selector*4)[0]
    if (secondary_selector, secondary_target) != (0x20, 0x15945E5):
        raise ValueError('Breche secondary default evidence changed')
    print('PASS: normalized skill279 receive ->12CB7F5; effect5FD constructor ->148251E')
    print('PASS: bounded secondary5FD ->selector32 ->15945E5 (generic movement)')
    render_index = root_type - 0x4EE
    if not 0 <= render_index <= 0x190:
        raise ValueError('Root outside audited renderer table bounds')
    render_selector = data[0x15BD910 - BASE + render_index]
    render_target = struct.unpack_from('<I', data, 0x15BD840 - BASE + 4*render_selector)[0]
    if (render_selector, render_target) != (51, 0x15BD5DC):
        raise ValueError('Breche default model render dispatch changed')
    print('PASS: bounded render5FD ->selector51 ->15BD5DC (default model route)')
    # Per-character owner branch: normalized skill 0x117 (279/Breche) at
    # 0x1424B34 jumps 0x142B21D and creates 0x5FD subtype 0 on the current
    # object with the 10.0f scale constant. This is not the receive subtype1
    # child tree above and must never be reused as a target visual.
    branch = 0x14259F9 - BASE
    if data[branch:branch + 10] != b'\x81\xbd\x00\xfd\xff\xff\x17\x01\x00\x00':
        raise ValueError('Breche owner normalized-skill compare changed')
    owner_jump = 0x1425A03 - BASE
    if data[owner_jump:owner_jump + 2] != b'\x0f\x84':
        raise ValueError('Breche owner branch conditional jump changed')
    rel = struct.unpack_from('<i', data, owner_jump + 2)[0]
    if BASE + owner_jump + 6 + rel != 0x142B21D:
        raise ValueError('Breche owner branch target changed')
    owner_call = 0x142B26B - BASE
    if data[owner_call] != 0x68 or struct.unpack_from('<I', data, owner_call + 1)[0] != 0x5FD:
        raise ValueError('Breche owner CreateEffect type changed')
    call = owner_call + 5
    if data[call] != 0xE8 or BASE + call + 5 + struct.unpack_from('<i', data, call + 1)[0] != 0x143E57C:
        raise ValueError('Breche owner CreateEffect call changed')
    scale = struct.unpack_from('<f', data, 0x1B4E4D0 - BASE)[0]
    if abs(scale - 10.0) > 1e-6:
        raise ValueError('Breche owner scale constant changed')
    if data[0x142B23C - BASE:0x142B23C - BASE + 2] != b'\x6a\x00':
        raise ValueError('Breche owner subtype push changed')
    print('PASS: normalized skill279 owner branch ->142B21D ->5FD subtype0, scale10')
    for particle, expected_selector, expected_target in (
        (0x8084, 30, 0x16673C6), (0x806E, 20, 0x164B3FF),
        (0x8085, 31, 0x166C453),
    ):
        index = particle - 0x8012
        if not (0x8008 < particle < 0x8147 and 0 <= index <= 0x134):
            raise ValueError('Particle outside audited constructor table bounds')
        slot = data[0x16A9789 - BASE + index]
        target = struct.unpack_from('<I', data, 0x16A96A1 - BASE + slot*4)[0]
        if (slot, target) != (expected_selector, expected_target):
            raise ValueError('Particle constructor dispatch changed')
    if struct.unpack_from('<I', data, 0x16A9C9D - BASE)[0] != 0x1667599:
        raise ValueError('8084 subtype0 entry changed')
    print('PASS: three bounded particle constructor entries;8084/sub0 ->1667599')
    print('Scope: table values only, not owner semantics, all children, or visual parity')


if __name__ == '__main__':
    main()
