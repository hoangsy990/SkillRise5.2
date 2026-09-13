"""Compare animation payloads between two unencrypted MU BMD v0A models."""

import argparse
import hashlib
import struct
from pathlib import Path


def read_actions(path: Path):
    data = path.read_bytes()
    if data[:4] == b"BMD\x0c":
        encrypted_size = struct.unpack_from("<I", data, 4)[0]
        encrypted = data[8:8 + encrypted_size]
        if len(encrypted) != encrypted_size:
            raise ValueError(f"{path}: truncated encrypted payload")
        xor_key = (0xD1, 0x73, 0x52, 0xF6, 0xD2, 0x9A, 0xCB, 0x27,
                   0x3E, 0xAF, 0x59, 0x31, 0x37, 0xB3, 0xE7, 0xA2)
        previous = 0x5E
        decrypted = bytearray(encrypted_size)
        for n, value in enumerate(encrypted):
            decrypted[n] = ((value ^ xor_key[n % 16]) - previous) & 0xFF
            previous = (value + 0x3D) & 0xFF
        data = b"BMD\x0a" + decrypted
    if data[:4] != b"BMD\x0a":
        raise ValueError(f"{path}: expected unencrypted BMD v0A")
    cursor = 36

    def take(size):
        nonlocal cursor
        result = data[cursor:cursor + size]
        if len(result) != size:
            raise ValueError(f"{path}: truncated at {cursor}")
        cursor += size
        return result

    meshes, bones, action_count = struct.unpack("<3h", take(6))
    for _ in range(meshes):
        vertices, normals, uvs, triangles, _texture = struct.unpack("<5h", take(10))
        take(vertices * 16 + normals * 20 + uvs * 8 + triangles * 64 + 32)

    key_counts = []
    payloads = [bytearray() for _ in range(action_count)]
    for action in range(action_count):
        count, locked = struct.unpack("<hB", take(3))
        key_counts.append(count)
        payloads[action].extend(struct.pack("<hB", count, locked))
        if locked:
            payloads[action].extend(take(count * 12))

    for _ in range(bones):
        dummy = take(1)[0]
        if dummy:
            continue
        take(34)
        for action, count in enumerate(key_counts):
            payloads[action].extend(take(count * 24))

    if cursor != len(data):
        raise ValueError(f"{path}: {len(data) - cursor} trailing bytes")
    return bones, [(key_counts[n], hashlib.sha256(payload).hexdigest())
                   for n, payload in enumerate(payloads)]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("--action", type=int, required=True)
    args = parser.parse_args()
    reference_bones, reference = read_actions(args.reference)
    candidate_bones, candidate = read_actions(args.candidate)
    if args.action < 0 or args.action >= len(reference):
        raise ValueError("reference action is out of range")
    wanted = reference[args.action]
    exact = [n for n, value in enumerate(candidate) if value == wanted]
    same_keys = [n for n, value in enumerate(candidate) if value[0] == wanted[0]]
    print(f"reference bones={reference_bones} actions={len(reference)} "
          f"action={args.action} keys={wanted[0]} sha256={wanted[1]}")
    print(f"candidate bones={candidate_bones} actions={len(candidate)}")
    print("exact matches:", exact)
    print("same key-count:", same_keys)


if __name__ == "__main__":
    main()
