#!/usr/bin/env python3
"""Compare selected animation clips in S21 and RISE player BMD files.

The parser intentionally supports only zero-mesh skeleton BMD payloads such as
Data/Player/player.bmd. It hashes each action header, optional root positions,
and the position/rotation keys for every non-dummy bone. No source is modified.
"""

from __future__ import annotations

import argparse
import hashlib
import importlib.util
import struct
from dataclasses import dataclass
from pathlib import Path


HERE = Path(__file__).resolve().parent
CONVERTER_PATH = HERE / "convert_s21_bmd_v0f_to_rise_v0c.py"


def load_converter():
    spec = importlib.util.spec_from_file_location("grow_lancer_bmd_converter", CONVERTER_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load converter: {CONVERTER_PATH}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


@dataclass
class ParsedSkeleton:
    name: str
    bone_count: int
    action_count: int
    key_counts: list[int]
    lock_positions: list[bool]
    action_hashes: list[str]


def take(payload: bytes, offset: int, size: int) -> tuple[bytes, int]:
    end = offset + size
    if end > len(payload):
        raise ValueError(f"BMD payload truncated at {offset}, need {size} bytes")
    return payload[offset:end], end


def parse_zero_mesh_skeleton(payload: bytes) -> ParsedSkeleton:
    if len(payload) < 38:
        raise ValueError("BMD payload is too short")
    raw_name = payload[:32]
    name = raw_name.split(b"\0", 1)[0].decode("cp949", errors="strict")
    mesh_count, bone_count, action_count = struct.unpack_from("<3h", payload, 32)
    if mesh_count != 0:
        raise ValueError(f"expected a zero-mesh skeleton, got {mesh_count} meshes")

    offset = 38
    key_counts: list[int] = []
    lock_positions: list[bool] = []
    action_chunks: list[list[bytes]] = [[] for _ in range(action_count)]
    for action in range(action_count):
        header, offset = take(payload, offset, 3)
        key_count = struct.unpack_from("<h", header, 0)[0]
        locked = bool(header[2])
        if key_count < 0:
            raise ValueError(f"negative key count for action {action}: {key_count}")
        key_counts.append(key_count)
        lock_positions.append(locked)
        action_chunks[action].append(header)
        if locked:
            positions, offset = take(payload, offset, key_count * 12)
            action_chunks[action].append(positions)

    for bone in range(bone_count):
        dummy_raw, offset = take(payload, offset, 1)
        if dummy_raw[0]:
            continue
        _bone_header, offset = take(payload, offset, 34)
        for action, key_count in enumerate(key_counts):
            keys, offset = take(payload, offset, key_count * 24)
            action_chunks[action].append(keys)

    trailing = payload[offset:]
    if len(trailing) > 15 or any(trailing):
        raise ValueError(
            f"unexpected bytes after skeleton: parsed={offset}, payload={len(payload)}, "
            f"trailing={trailing.hex()}"
        )

    return ParsedSkeleton(
        name=name,
        bone_count=bone_count,
        action_count=action_count,
        key_counts=key_counts,
        lock_positions=lock_positions,
        action_hashes=[hashlib.sha256(b"".join(chunks)).hexdigest().upper() for chunks in action_chunks],
    )


def decode_container(path: Path, converter) -> tuple[bytes, str]:
    data = path.resolve().read_bytes()
    if data[:4] == b"BMD\x0f":
        payload = converter.decrypt_s21_payload(converter.parse_s21_container(data))
        return payload, "v0F"
    if data[:4] == b"BMD\x0c":
        return converter.decrypt_rise_v0c(data), "v0C"
    raise ValueError(f"unsupported BMD container {data[:4]!r}: {path}")


def parse_action_spec(value: str) -> list[int]:
    result: list[int] = []
    for part in value.split(","):
        part = part.strip()
        if "-" in part:
            start, end = (int(piece, 0) for piece in part.split("-", 1))
            result.extend(range(start, end + 1))
        elif part:
            result.append(int(part, 0))
    return sorted(set(result))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("s21", type=Path)
    parser.add_argument("rise", type=Path)
    parser.add_argument("--actions", default="185-194")
    args = parser.parse_args()

    converter = load_converter()
    s21_payload, s21_version = decode_container(args.s21, converter)
    rise_payload, rise_version = decode_container(args.rise, converter)
    s21 = parse_zero_mesh_skeleton(s21_payload)
    rise = parse_zero_mesh_skeleton(rise_payload)

    print(f"S21={args.s21.resolve()} version={s21_version} name={s21.name!r} bones={s21.bone_count} actions={s21.action_count}")
    print(f"RISE={args.rise.resolve()} version={rise_version} name={rise.name!r} bones={rise.bone_count} actions={rise.action_count}")
    if s21.bone_count != rise.bone_count:
        raise ValueError(f"bone-count mismatch: S21={s21.bone_count}, RISE={rise.bone_count}")

    print("action,s21_keys,rise_keys,s21_lock,rise_lock,byte_exact,s21_sha256,rise_sha256")
    for action in parse_action_spec(args.actions):
        if action >= s21.action_count or action >= rise.action_count:
            print(f"{action},OUT_OF_RANGE")
            continue
        equal = s21.action_hashes[action] == rise.action_hashes[action]
        print(
            f"{action},{s21.key_counts[action]},{rise.key_counts[action]},"
            f"{int(s21.lock_positions[action])},{int(rise.lock_positions[action])},"
            f"{str(equal).lower()},{s21.action_hashes[action]},{rise.action_hashes[action]}"
        )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
