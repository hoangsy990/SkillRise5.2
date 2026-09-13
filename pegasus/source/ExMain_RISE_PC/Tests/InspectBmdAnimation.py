"""Inspect action key counts and named bone channels in encrypted MU BMD files."""

from __future__ import annotations

import argparse
import math
import struct
from pathlib import Path

from InspectPegasusBmd import decode


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("model", type=Path)
    parser.add_argument("--action", type=int, required=True)
    args = parser.parse_args()
    data = decode(args.model)
    cursor = 36
    meshes, bones, actions = struct.unpack_from("<3h", data, cursor)
    cursor += 6
    for _ in range(meshes):
        vertices, normals, uvs, triangles, _texture = struct.unpack_from(
            "<5h", data, cursor)
        cursor += 10 + vertices * 16 + normals * 20 + uvs * 8 + triangles * 64 + 32
    action_rows = []
    for _ in range(actions):
        keys, locked = struct.unpack_from("<hB", data, cursor)
        cursor += 3
        positions = []
        if locked:
            positions = [struct.unpack_from("<3f", data, cursor + key * 12)
                         for key in range(keys)]
            cursor += keys * 12
        action_rows.append((keys, locked, positions))
    if not 0 <= args.action < actions:
        raise ValueError(f"action must be in 0..{actions - 1}")
    print(f"meshes={meshes} bones={bones} actions={actions} action={args.action} "
          f"keys={action_rows[args.action][0]} locked={action_rows[args.action][1]}")
    if action_rows[args.action][2]:
        print("action_positions=", action_rows[args.action][2])
    for bone in range(bones):
        dummy = data[cursor]
        cursor += 1
        if dummy:
            print(f"bone={bone} dummy=1")
            continue
        name = data[cursor:cursor + 32].split(b"\0", 1)[0].decode("latin1")
        parent = struct.unpack_from("<h", data, cursor + 32)[0]
        cursor += 34
        selected = None
        for action, (keys, _locked, _positions) in enumerate(action_rows):
            positions = [struct.unpack_from("<3f", data, cursor + key * 12)
                         for key in range(keys)]
            cursor += keys * 12
            rotations = [struct.unpack_from("<3f", data, cursor + key * 12)
                         for key in range(keys)]
            cursor += keys * 12
            if action == args.action:
                selected = (positions, rotations)
        positions, rotations = selected
        degrees = [tuple(round(math.degrees(v), 1) for v in row) for row in rotations]
        rounded_positions = [tuple(round(v, 2) for v in row) for row in positions]
        print(f"bone={bone} name={name!r} parent={parent} pos={rounded_positions} rot_deg={degrees}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
