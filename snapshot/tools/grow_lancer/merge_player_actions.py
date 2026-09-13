#!/usr/bin/env python3
"""Append selected S21 player animation clips to an isolated RISE player BMD.

Existing RISE actions remain byte-for-byte unchanged and selected S21 clips are
appended at new IDs. The tool accepts only matching zero-mesh, 60-bone player
skeletons and confines output to the Grow Lancer worktree.
"""

from __future__ import annotations

import argparse
import hashlib
import importlib.util
import struct
import sys
from dataclasses import dataclass
from pathlib import Path


HERE = Path(__file__).resolve().parent
WORKTREE_ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer").resolve()


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load module: {path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


CONVERTER = load_module("grow_lancer_bmd_converter", HERE / "convert_s21_bmd_v0f_to_rise_v0c.py")
COMPARER = load_module("grow_lancer_action_compare", HERE / "compare_player_actions.py")


@dataclass
class RawSkeleton:
    payload: bytes
    name: bytes
    bone_count: int
    action_count: int
    action_records: list[bytes]
    bone_headers: list[bytes]
    bone_action_keys: list[list[bytes] | None]


def take(payload: bytes, offset: int, size: int) -> tuple[bytes, int]:
    end = offset + size
    if end > len(payload):
        raise ValueError(f"BMD payload truncated at {offset}, need {size} bytes")
    return payload[offset:end], end


def parse(payload: bytes) -> RawSkeleton:
    if len(payload) < 38:
        raise ValueError("BMD payload is too short")
    mesh_count, bone_count, action_count = struct.unpack_from("<3h", payload, 32)
    if mesh_count != 0:
        raise ValueError(f"expected zero-mesh player skeleton, got {mesh_count}")
    offset = 38
    key_counts: list[int] = []
    actions: list[bytes] = []
    for action in range(action_count):
        header, offset = take(payload, offset, 3)
        key_count = struct.unpack_from("<h", header, 0)[0]
        if key_count < 0:
            raise ValueError(f"negative key count at action {action}")
        record = bytearray(header)
        if header[2]:
            positions, offset = take(payload, offset, key_count * 12)
            record.extend(positions)
        key_counts.append(key_count)
        actions.append(bytes(record))

    bone_headers: list[bytes] = []
    bone_keys: list[list[bytes] | None] = []
    for _bone in range(bone_count):
        dummy, offset = take(payload, offset, 1)
        if dummy[0]:
            bone_headers.append(dummy)
            bone_keys.append(None)
            continue
        metadata, offset = take(payload, offset, 34)
        per_action: list[bytes] = []
        for key_count in key_counts:
            keys, offset = take(payload, offset, key_count * 24)
            per_action.append(keys)
        bone_headers.append(dummy + metadata)
        bone_keys.append(per_action)

    trailing = payload[offset:]
    if len(trailing) > 15 or any(trailing):
        raise ValueError(f"unexpected trailing payload bytes: {trailing.hex()}")
    return RawSkeleton(payload, payload[:32], bone_count, action_count, actions, bone_headers, bone_keys)


def decode(path: Path) -> bytes:
    data = path.resolve().read_bytes()
    if data[:4] == b"BMD\x0f":
        return CONVERTER.decrypt_s21_payload(CONVERTER.parse_s21_container(data))
    if data[:4] == b"BMD\x0c":
        return CONVERTER.decrypt_rise_v0c(data)
    raise ValueError(f"unsupported container {data[:4]!r}: {path}")


def require_hash(path: Path, expected: str) -> bytes:
    data = path.resolve().read_bytes()
    actual = hashlib.sha256(data).hexdigest().upper()
    if actual != expected.upper():
        raise ValueError(f"SHA-256 mismatch for {path}: expected={expected.upper()}, actual={actual}")
    return data


def require_output(path: Path) -> Path:
    result = path.resolve()
    try:
        result.relative_to(WORKTREE_ROOT)
    except ValueError as error:
        raise ValueError(f"output must stay under {WORKTREE_ROOT}: {result}") from error
    return result


def merge(base: RawSkeleton, source: RawSkeleton, selected: list[int]) -> tuple[bytes, list[int]]:
    if base.bone_count != source.bone_count:
        raise ValueError(f"bone-count mismatch: base={base.bone_count}, source={source.bone_count}")
    def canonical_bone(header: bytes) -> tuple[bool, bytes, int]:
        if len(header) == 1:
            return True, b"", -1
        name = header[1:33].split(b"\0", 1)[0]
        parent = struct.unpack_from("<h", header, 33)[0]
        return False, name, parent

    base_bones = [canonical_bone(header) for header in base.bone_headers]
    source_bones = [canonical_bone(header) for header in source.bone_headers]
    if base_bones != source_bones:
        raise ValueError("canonical bone names/parents/dummy layout differ; refusing unsafe animation transplant")
    if any(action < 0 or action >= source.action_count for action in selected):
        raise ValueError("selected source action is out of range")

    new_ids = list(range(base.action_count, base.action_count + len(selected)))
    out = bytearray()
    out.extend(base.name)
    out.extend(struct.pack("<3h", 0, base.bone_count, base.action_count + len(selected)))
    out.extend(b"".join(base.action_records))
    out.extend(b"".join(source.action_records[action] for action in selected))
    for bone, header in enumerate(base.bone_headers):
        out.extend(header)
        base_keys = base.bone_action_keys[bone]
        source_keys = source.bone_action_keys[bone]
        if base_keys is None:
            if source_keys is not None:
                raise ValueError(f"dummy mismatch at bone {bone}")
            continue
        if source_keys is None:
            raise ValueError(f"dummy mismatch at bone {bone}")
        out.extend(b"".join(base_keys))
        out.extend(b"".join(source_keys[action] for action in selected))
    return bytes(out), new_ids


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("s21", type=Path)
    parser.add_argument("rise", type=Path)
    parser.add_argument("output", type=Path)
    parser.add_argument("--actions", default="185-194")
    parser.add_argument("--expected-s21-sha256", required=True)
    parser.add_argument("--expected-rise-sha256", required=True)
    args = parser.parse_args()

    require_hash(args.s21, args.expected_s21_sha256)
    require_hash(args.rise, args.expected_rise_sha256)
    selected = COMPARER.parse_action_spec(args.actions)
    source = parse(decode(args.s21))
    base = parse(decode(args.rise))
    merged_payload, new_ids = merge(base, source, selected)
    merged = CONVERTER.encrypt_rise_v0c(merged_payload)
    reparsed = parse(CONVERTER.decrypt_rise_v0c(merged))

    base_check = COMPARER.parse_zero_mesh_skeleton(base.payload)
    source_check = COMPARER.parse_zero_mesh_skeleton(source.payload)
    merged_check = COMPARER.parse_zero_mesh_skeleton(merged_payload)
    for old_id in range(base.action_count):
        if merged_check.action_hashes[old_id] != base_check.action_hashes[old_id]:
            raise RuntimeError(f"existing RISE action {old_id} changed during merge")
    for source_id, new_id in zip(selected, new_ids):
        if merged_check.action_hashes[new_id] != source_check.action_hashes[source_id]:
            raise RuntimeError(f"appended action mismatch: source={source_id}, new={new_id}")

    output = require_output(args.output)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(merged)
    print("Status=PASS")
    print(f"Output={output}")
    print(f"OutputSha256={hashlib.sha256(merged).hexdigest().upper()}")
    print(f"BaseActionsPreserved={base.action_count}")
    print(f"MergedActionCount={reparsed.action_count}")
    for source_id, new_id in zip(selected, new_ids):
        print(f"ActionMap={source_id}->{new_id}; keys={source_check.key_counts[source_id]}; sha256={source_check.action_hashes[source_id]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
