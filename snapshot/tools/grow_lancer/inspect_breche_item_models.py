"""Read-only S21 item model provenance; no staging or runtime acceptance."""
import hashlib
import json
import struct
from pathlib import Path

ROOT = Path(r"D:\MU FICA Season 21")
TABLE = ROOT / "Data/Local/item.bmd"
INDICES = {185, *range(189, 200), 205, 217, 246, 264, 311, 346, 400}


def checksum(payload):
    # Pinned S21 0x97904E; loader 0xD0520B passes seed 0xE2F1.
    seed = 0xE2F1
    value = seed << 9
    for offset in range(0, len(payload) - 3, 4):
        word = struct.unpack_from('<I', payload, offset)[0]
        value = ((value + word) if ((offset // 4 + seed) % 2)
                 else (value ^ word)) & 0xFFFFFFFF
        if offset % 16 == 0:
            value ^= ((seed + value) & 0xFFFFFFFF) >> (offset // 4 % 8 + 1)
    return value


def main():
    data = TABLE.read_bytes()
    count = struct.unpack_from('<I', data)[0]
    if len(data) != 8 + count * 0x2C4:
        raise ValueError('Unexpected S21 record size/count')
    if checksum(data[4:-4]) != struct.unpack_from('<I', data, len(data)-4)[0]:
        raise ValueError('S21 checksum mismatch')
    results = []
    for ordinal in range(count):
        encoded = data[4 + ordinal*708:4 + (ordinal+1)*708]
        # 0x97900E modulo-3 XOR; key at mapped VA 0x1DD7448.
        row = bytes(v ^ b'\xfc\xcf\xab'[i % 3] for i, v in enumerate(encoded))
        item = struct.unpack_from('<I', row)[0]
        group, index = divmod(item, 512)
        if group not in range(8, 12) or index not in INDICES:
            continue
        directory = row[8:268].split(b'\0', 1)[0].decode('ascii')
        filename = row[268:528].split(b'\0', 1)[0].decode('ascii')
        source = (ROOT / directory / filename).resolve()
        if not source.is_relative_to(ROOT.resolve()):
            raise ValueError('Source path escapes read-only root')
        asset = source.read_bytes() if source.is_file() else None
        results.append(dict(group=group, index=index, runtimeId=0xAE9+item,
                            recordOrdinal=ordinal, source=str(source),
                            sha256=hashlib.sha256(asset).hexdigest() if asset else None,
                            headerHex=asset[:4].hex() if asset else None))
    print(json.dumps(dict(table=str(TABLE), tableSha256=hashlib.sha256(data).hexdigest(),
                          checksum='PASS', count=count, models=results), indent=2))


if __name__ == '__main__':
    main()
