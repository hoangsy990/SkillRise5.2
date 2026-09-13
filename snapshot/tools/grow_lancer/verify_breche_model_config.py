"""Read-only pinned config-key audit, NOT proof of live model-cache contents."""
import hashlib
import json
import struct

from inspect_breche_item_models import TABLE, checksum

PIN = '9b9d62b7f3ba085d31b4aefbcc9de5db505c48d45c336a18cad424c93d3b02f6'


def audit(data):
    if hashlib.sha256(data).hexdigest() != PIN:
        raise ValueError('S21 item table hash changed')
    count = struct.unpack_from('<I', data)[0]
    if len(data) != 8 + count * 0x2C4:
        raise ValueError('Unexpected record boundaries')
    if checksum(data[4:-4]) != struct.unpack_from('<I', data, len(data)-4)[0]:
        raise ValueError('S21 checksum mismatch')
    keys = []
    for ordinal in range(count):
        encoded = data[4 + ordinal*708:4 + (ordinal+1)*708]
        row = bytes(v ^ b'\xfc\xcf\xab'[i % 3] for i, v in enumerate(encoded))
        keys.append(struct.unpack_from('<i', row)[0])
    root_key = 0x5FD - 0xAE9
    if (count, min(keys), max(keys), len(set(keys))) != (4950, 0, 10759, 4950):
        raise ValueError('Decoded key domain changed')
    if any(key < 0 for key in keys) or root_key in keys:
        raise ValueError('Unexpected negative or Breche root config key')
    return dict(table=str(TABLE), sha256=PIN, count=count,
                minimum=min(keys), maximum=max(keys), negativeKeys=0,
                duplicateKeys=0, rootRuntimeId=0x5FD, rootConfigKey=root_key,
                rootConfigPresent=False, checksum='PASS',
                scope='Pinned on-disk config only; live cache and caller filename unproven')


if __name__ == '__main__':
    print(json.dumps(audit(TABLE.read_bytes()), indent=2))
