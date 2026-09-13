"""Pinned offline registration check, not visual parity or full dispatch proof."""
import hashlib
import struct
from disasm_s21_window import DUMP, IMAGE_BASE


def main():
    data = DUMP.read_bytes()
    if hashlib.sha256(data).hexdigest() != '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637':
        raise ValueError('Reference dump changed')
    for va, callback, model in ((0xA1B334, 0xA49FEF, 0x5F9),
                                (0xA1B246, 0xA49422, 0x5EB)):
        offset = va - IMAGE_BASE
        prefix = b'\x68' + struct.pack('<I', callback) + b'\x68' + struct.pack('<I', model)
        if data[offset:offset+10] != prefix or data[offset+10] != 0xE8:
            raise ValueError('Registration instruction sequence differs')
        target = va + 15 + struct.unpack_from('<i', data, offset+11)[0]
        if target != 0xA1DCE6:
            raise ValueError('Registration callee differs')
        print(f'PASS: {model:#x} registers {callback:#x} via {target:#x}')
    print('Spin cross5F9 uses stored light * alpha and object UV; fixed RGB callback belongs to5EB.')


if __name__ == '__main__':
    main()
