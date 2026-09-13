"""Read only the pinned offline player action record, not a live cast."""
import hashlib
import struct

from minidump.minidumpfile import MinidumpFile
from inspect_breche_full_dump import SNAPSHOT, PIN
from disasm_s21_window import DUMP, IMAGE_BASE
from capstone import Cs, CS_ARCH_X86, CS_MODE_32


def main():
    binary = DUMP.read_bytes()
    assert hashlib.sha256(binary).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    instructions = [(i.address, i.mnemonic, i.op_str) for i in decoder.disasm(
        binary[0x15773D0-IMAGE_BASE:0x15773DB-IMAGE_BASE], 0x15773D0)]
    assert instructions == [(0x15773D0, 'shl', 'ecx, 4'),
                            (0x15773D3, 'add', 'ecx, dword ptr [eax + 0x30]'),
                            (0x15773D6, 'movss', 'xmm0, dword ptr [ecx + 4]')]
    with SNAPSHOT.open('rb') as stream:
        assert hashlib.file_digest(stream, 'sha256').hexdigest() == PIN
    dump = MinidumpFile.parse(str(SNAPSHOT))
    reader = dump.get_reader()

    def u32(address):
        return struct.unpack('<I', reader.read(address, 4))[0]

    manager = u32(0x1E04C8C)
    head = u32(manager + 4)
    node = u32(head + 4)
    seen = set()
    for _ in range(64):
        assert node and node not in seen
        seen.add(node)
        row = reader.read(node, 24)
        assert not row[13], 'player model absent'
        key = struct.unpack_from('<i', row, 16)[0]
        if key == 0xA94:
            model = struct.unpack_from('<I', row, 20)[0]
            bones, meshes, actions = struct.unpack('<hhh', reader.read(model + 0x22, 6))
            assert (bones, meshes, actions) == (60, 0, 410)
            # Pinned consumer15773D0/D3/D6: action stride16, model+30, speed+4.
            table = u32(model + 0x30)
            record = reader.read(table + 328 * 16, 16)
            speed = struct.unpack_from('<f', record, 4)[0]
            assert record[4:8] == struct.pack('<f', 0.4)
            print(f'SnapshotSHA256={PIN}; model={model:#x}; actionTable={table:#x}')
            print(f'Action=328; record={record.hex()}; speed={speed:.9g}')
            for action in range(185, 195):
                cast = reader.read(table + action * 16, 16)
                print(f'GrowLancerAction={action}; record={cast.hex()}; '
                      f'speed={struct.unpack_from("<f", cast, 4)[0]:.9g}')
            for action in range(335, 339):
                mounted = reader.read(table + action * 16, 16)
                print(f'MountedAction={action}; record={mounted.hex()}; '
                      f'speed={struct.unpack_from("<f", mounted, 4)[0]:.9g}')
            print('Scope: cached speed at this snapshot only; initialization and later writes remain unproven')
            return
        node = struct.unpack_from('<I', row, 0 if 0xA94 < key else 8)[0]
    raise ValueError('tree depth exceeded')


if __name__ == '__main__':
    main()
