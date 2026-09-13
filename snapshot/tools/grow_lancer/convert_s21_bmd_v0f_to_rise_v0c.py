#!/usr/bin/env python3
"""Convert a hash-pinned Season 21 BMD v0F into a RISE BMD v0C.

The source is never modified. Output is restricted to this Grow Lancer
worktree and is verified by decrypting the generated v0C container back to
the exact Season 21 plaintext payload.
"""

from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path


MASK32 = 0xFFFFFFFF
BLOCK_SIZE = 16
WORKTREE_ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer").resolve()
S21_KEY = bytes.fromhex(
    "CC504513C2A6574ED69A4589BF2FBCD9"
    "39B3B3BD50BDCCB68546D1D61654E087"
)
DELTA = (
    0xC3EFE9DB,
    0x44626B02,
    0x79E27C8A,
    0x78DF30EC,
    0x715EA49E,
    0xC785DA0A,
    0xE04EF22A,
    0xE5C40957,
)
KEY_ROTATIONS = (1, 3, 6, 11, 13, 17)
MAP_XOR_KEY = bytes.fromhex("D17352F6D29ACB273EAF593137B3E7A2")


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()


def rol32(value: int, bits: int) -> int:
    bits &= 31
    value &= MASK32
    return ((value << bits) | (value >> ((32 - bits) & 31))) & MASK32


def ror32(value: int, bits: int) -> int:
    return rol32(value, -bits)


def expand_lea256_key(key: bytes) -> list[tuple[int, ...]]:
    if len(key) != 32:
        raise ValueError("LEA-256 requires exactly 32 key bytes")
    words = list(struct.unpack("<8I", key))
    round_keys: list[tuple[int, ...]] = []
    for round_index in range(32):
        delta = DELTA[round_index % 8]
        selected: list[int] = []
        for lane, output_rotation in enumerate(KEY_ROTATIONS):
            word_index = (6 * round_index + lane) % 8
            words[word_index] = rol32(
                words[word_index] + rol32(delta, round_index + lane),
                output_rotation,
            )
            selected.append(words[word_index])
        round_keys.append(tuple(selected))
    return round_keys


def decrypt_lea256_block(block: bytes, round_keys: list[tuple[int, ...]]) -> bytes:
    if len(block) != BLOCK_SIZE:
        raise ValueError("LEA block must be 16 bytes")
    y0, y1, y2, y3 = struct.unpack("<4I", block)
    for key in reversed(round_keys):
        x0 = y3
        x1 = (ror32(y0, 9) - (x0 ^ key[0])) & MASK32
        x1 ^= key[1]
        x2 = (rol32(y1, 5) - (x1 ^ key[2])) & MASK32
        x2 ^= key[3]
        x3 = (rol32(y2, 3) - (x2 ^ key[4])) & MASK32
        x3 ^= key[5]
        y0, y1, y2, y3 = x0, x1, x2, x3
    return struct.pack("<4I", y0, y1, y2, y3)


def decrypt_s21_payload(payload: bytes) -> bytes:
    if len(payload) % BLOCK_SIZE:
        raise ValueError("S21 v0F payload length is not a multiple of 16")
    keys = expand_lea256_key(S21_KEY)
    return b"".join(
        decrypt_lea256_block(payload[offset : offset + BLOCK_SIZE], keys)
        for offset in range(0, len(payload), BLOCK_SIZE)
    )


def encrypt_rise_v0c(plaintext: bytes) -> bytes:
    encoded = bytearray(len(plaintext))
    rolling_key = 0x5E
    for offset, value in enumerate(plaintext):
        cipher = ((value + rolling_key) & 0xFF) ^ MAP_XOR_KEY[offset % 16]
        encoded[offset] = cipher
        rolling_key = (cipher + 0x3D) & 0xFF
    return b"BMD\x0c" + struct.pack("<I", len(encoded)) + bytes(encoded)


def decrypt_rise_v0c(container: bytes) -> bytes:
    if len(container) < 8 or container[:4] != b"BMD\x0c":
        raise ValueError("generated output is not a BMD v0C container")
    encoded_length = struct.unpack_from("<I", container, 4)[0]
    encoded = container[8:]
    if encoded_length != len(encoded):
        raise ValueError(
            f"RISE v0C length mismatch: header={encoded_length}, actual={len(encoded)}"
        )
    plaintext = bytearray(encoded_length)
    rolling_key = 0x5E
    for offset, value in enumerate(encoded):
        plaintext[offset] = ((value ^ MAP_XOR_KEY[offset % 16]) - rolling_key) & 0xFF
        rolling_key = (value + 0x3D) & 0xFF
    return bytes(plaintext)


def parse_s21_container(data: bytes) -> bytes:
    if len(data) < 8 or data[:4] != b"BMD\x0f":
        raise ValueError("input is not a Season 21 BMD v0F container")
    encoded_length = struct.unpack_from("<I", data, 4)[0]
    payload = data[8:]
    if encoded_length != len(payload):
        raise ValueError(
            f"S21 v0F length mismatch: header={encoded_length}, actual={len(payload)}"
        )
    return payload


def inspect_plaintext(payload: bytes) -> dict[str, object]:
    if len(payload) < 38:
        raise ValueError("decrypted BMD payload is shorter than its fixed header")
    raw_name = payload[:32]
    name = raw_name.split(b"\0", 1)[0].decode("cp949", errors="strict")
    mesh_count, bone_count, action_count = struct.unpack_from("<3h", payload, 32)
    if not name or any(ord(ch) < 0x20 for ch in name):
        raise ValueError(f"invalid model identity: {raw_name.hex()}")
    if not all(0 <= value <= 4096 for value in (mesh_count, bone_count, action_count)):
        raise ValueError(
            f"implausible counts: meshes={mesh_count}, bones={bone_count}, actions={action_count}"
        )
    return {
        "name": name,
        "meshCount": mesh_count,
        "boneCount": bone_count,
        "actionCount": action_count,
    }


def verify_reference_vector() -> None:
    key = bytes.fromhex(
        "0F1E2D3C4B5A69788796A5B4C3D2E1F0"
        "F0E1D2C3B4A5968778695A4B3C2D1E0F"
    )
    ciphertext = bytes.fromhex("D651AFF647B189C13A8900CA27F9E197")
    expected = bytes.fromhex("303132333435363738393A3B3C3D3E3F")
    actual = decrypt_lea256_block(ciphertext, expand_lea256_key(key))
    if actual != expected:
        raise RuntimeError("LEA-256 reference-vector self-test failed")


def require_isolated_output(path: Path) -> Path:
    resolved = path.resolve()
    try:
        resolved.relative_to(WORKTREE_ROOT)
    except ValueError as error:
        raise ValueError(f"output must stay under {WORKTREE_ROOT}: {resolved}") from error
    return resolved


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--expected-sha256", required=True)
    parser.add_argument("--expected-name", required=True)
    args = parser.parse_args()

    verify_reference_vector()
    source = args.input.resolve().read_bytes()
    source_hash = sha256(source)
    if source_hash != args.expected_sha256.upper():
        raise ValueError(
            f"source SHA-256 mismatch: expected={args.expected_sha256.upper()}, actual={source_hash}"
        )

    plaintext = decrypt_s21_payload(parse_s21_container(source))
    identity = inspect_plaintext(plaintext)
    if identity["name"].casefold() != args.expected_name.casefold():
        raise ValueError(
            f"model identity mismatch: expected={args.expected_name!r}, actual={identity['name']!r}"
        )

    output = require_isolated_output(args.output)
    container = encrypt_rise_v0c(plaintext)
    if decrypt_rise_v0c(container) != plaintext:
        raise RuntimeError("generated v0C round-trip verification failed")
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(container)

    print("Status=PASS")
    print(f"Source={args.input.resolve()}")
    print(f"SourceSha256={source_hash}")
    print(f"PlaintextSha256={sha256(plaintext)}")
    print(f"Output={output}")
    print(f"OutputSha256={sha256(container)}")
    print(f"RoundTripSha256={sha256(decrypt_rise_v0c(container))}")
    for key, value in identity.items():
        print(f"{key}={value}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
