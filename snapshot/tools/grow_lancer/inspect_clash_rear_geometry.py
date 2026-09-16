"""Read-only hash-pinned rear Clash mesh inventory; no rendered-pose claim."""

import hashlib
import json
import struct
from pathlib import Path

from convert_s21_bmd_v0f_to_rise_v0c import decrypt_rise_v0c


ROOT = Path(__file__).resolve().parents[2]
PROVENANCE = ROOT / "GrowLancer/asset_provenance.json"


def main() -> None:
    records = json.loads(PROVENANCE.read_text(encoding="utf-8"))
    # Provenance schema is a list of skill assets beneath named sections.
    flat = []
    for value in records.values() if isinstance(records, dict) else (records,):
        if isinstance(value, list):
            flat.extend(row for row in value if isinstance(row, dict))
    row = next(item for item in flat if item.get("skillId") == 275 and
               item.get("role") == "rear_flash_model")
    path = ROOT / row["stagedPath"]
    container = path.read_bytes()
    assert hashlib.sha256(container).hexdigest().upper() == row["stagedSha256"], path
    data = decrypt_rise_v0c(container)
    assert hashlib.sha256(data).hexdigest().upper() == row["roundTripDecodedSha256"]
    name = data[:32].split(b"\0", 1)[0].decode("ascii")
    mesh_count, bones, actions = struct.unpack_from("<3h", data, 32)
    assert name == "crasha02.SMD" and (mesh_count, bones, actions) == (1, 1, 1)
    vertices, normals, texcoords, triangles, texture = struct.unpack_from("<5h", data, 38)
    assert (vertices, normals, triangles) == (6, 2, 4)
    offset = 48
    points = []
    for _ in range(vertices):
        node, x, y, z = struct.unpack_from("<hxx3f", data, offset)
        assert node == 0
        points.append((x, y, z))
        offset += 16
    offset += normals * 20 + texcoords * 8
    face_indices = []
    for _ in range(triangles):
        polygon = data[offset]
        indices = struct.unpack_from("<4h", data, offset + 2)
        assert polygon in (3, 4) and all(0 <= i < vertices for i in indices[:polygon]), (offset, polygon, indices, data[offset:offset + 16].hex())
        face_indices.append(indices[:polygon])
        offset += 64  # Observed native source-compatible v0C record stride.
    print(f"PASS hash-pinned Clash rear name={name} mesh={mesh_count} bones={bones} actions={actions}")
    for index, point in enumerate(points):
        print(f"vertex {index}: {point}")
    for index, face in enumerate(face_indices):
        a, b, c = (points[vertex] for vertex in face[:3])
        u = tuple(b[axis] - a[axis] for axis in range(3))
        v = tuple(c[axis] - a[axis] for axis in range(3))
        cross = (u[1] * v[2] - u[2] * v[1],
                 u[2] * v[0] - u[0] * v[2],
                 u[0] * v[1] - u[1] * v[0])
        assert any(abs(component) > 0.01 for component in cross), (index, face)
        print(f"face {index}: {face}, rawCross={cross}")
    print("Scope: original mesh coordinates/indices only; native bone/camera projection remains open")


if __name__ == "__main__":
    main()
