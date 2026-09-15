"""Hash-pinned S21 MagicPin01 mesh UV/OZT alpha coverage, read only.

This is material-input evidence, never proof of native GPU raster/pixels.
"""

from __future__ import annotations

import json
import struct

from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_rise_v0c, decrypt_s21_payload, parse_s21_container, sha256,
)
from inspect_magic_pin_model_geometry import (
    SOURCE_DIR, SOURCES, STAGED_DIR, STAGED_SHA, mesh_bounds,
)
from inspect_magic_pin_material_alpha import FILES


def inside_triangle(u: float, v: float, a, b, c) -> bool:
    def edge(p, q, r):
        return (r[0] - p[0]) * (q[1] - p[1]) - (r[1] - p[1]) * (q[0] - p[0])

    p = (u, v)
    e0, e1, e2 = edge(a, b, p), edge(b, c, p), edge(c, a, p)
    return (e0 >= 0 and e1 >= 0 and e2 >= 0) or (
        e0 <= 0 and e1 <= 0 and e2 <= 0
    )


def main() -> None:
    name = "magicpin01.bmd"
    source = (SOURCE_DIR / name).read_bytes()
    staged = (STAGED_DIR / name).read_bytes()
    assert sha256(source) == SOURCES[name], "unpinned original BMD"
    assert sha256(staged) == STAGED_SHA[name], "unpinned staged BMD"
    payload = decrypt_s21_payload(parse_s21_container(source))
    assert decrypt_rise_v0c(staged) == payload, "BMD plaintext mismatch"
    mesh, _ = mesh_bounds(payload, 1)
    mesh = mesh[0]
    assert mesh["texture"].lower() == "wind1.tga"
    assert mesh["triangles"] == 8

    texture = (SOURCE_DIR / "wind1.OZT").read_bytes()
    assert sha256(texture) == FILES["wind1.OZT"], "unpinned original OZT"
    width, height = struct.unpack_from("<HH", texture, 16)
    assert (width, height, texture[20]) == (64, 64, 32)
    alpha = texture[25:22 + width * height * 4:4]
    assert len(alpha) == width * height

    reports = []
    for flip_v in (False, True):
        seen = set()
        per_triangle = []
        for indices in mesh["triangleUVIndices"]:
            a, b, c = [mesh["localUV"][i] for i in indices]
            covered = []
            for y in range(height):
                for x in range(width):
                    u, v = (x + .5) / width, (y + .5) / height
                    if inside_triangle(u, v, a, b, c):
                        texture_y = height - 1 - y if flip_v else y
                        pixel = texture_y * width + x
                        covered.append(pixel)
                        seen.add(pixel)
            assert covered, "mesh UV triangle sampled no texel centers"
            per_triangle.append({
                "coveredTexels": len(covered),
                "nonzeroAlphaTexels": sum(alpha[p] > 0 for p in covered),
                "meanAlpha": round(sum(alpha[p] for p in covered) / len(covered), 3),
            })
        reports.append({
            "flipV": flip_v,
            "uniqueCoveredTexels": len(seen),
            "uniqueNonzeroAlphaTexels": sum(alpha[p] > 0 for p in seen),
            "minimumTriangleNonzeroFraction": round(min(
                row["nonzeroAlphaTexels"] / row["coveredTexels"]
                for row in per_triangle
            ), 4),
            "triangles": per_triangle,
        })
    print(json.dumps({
        "bmd": name, "bmdSha256": SOURCES[name],
        "ozt": "wind1.OZT", "oztSha256": FILES["wind1.OZT"],
        "meshUvEvidence": reports,
        "scope": "S21 input texture/mesh only; native transform, GPU samples and visible triangle OPEN",
    }, indent=2))


if __name__ == "__main__":
    main()
