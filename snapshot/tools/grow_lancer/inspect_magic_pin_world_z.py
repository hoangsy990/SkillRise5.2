"""Pinned MagicPin01 native BMD yaw-only bone/world-Z diagnostic, read only."""

from __future__ import annotations

import json
import math

from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_rise_v0c, decrypt_s21_payload, parse_s21_container, sha256,
)
from inspect_magic_pin_model_geometry import (
    SOURCE_DIR, SOURCES, STAGED_DIR, STAGED_SHA,
    animation_summary, mesh_bounds,
)


def quaternion(angles):
    # Native Math/ZzzMathLib.cpp::AngleQuaternion input angles are radians.
    sx, cx = math.sin(angles[0] / 2), math.cos(angles[0] / 2)
    sy, cy = math.sin(angles[2] / 2), math.cos(angles[2] / 2)
    sp, cp = math.sin(angles[1] / 2), math.cos(angles[1] / 2)
    return (
        sx * cp * cy - cx * sp * sy,
        cx * sp * cy + sx * cp * sy,
        cx * cp * sy - sx * sp * cy,
        cx * cp * cy + sx * sp * sy,
    )


def main() -> None:
    name = "magicpin01.bmd"
    source = (SOURCE_DIR / name).read_bytes()
    staged = (STAGED_DIR / name).read_bytes()
    assert sha256(source) == SOURCES[name]
    assert sha256(staged) == STAGED_SHA[name]
    payload = decrypt_s21_payload(parse_s21_container(source))
    assert decrypt_rise_v0c(staged) == payload
    mesh, offset = mesh_bounds(payload, 1)
    pose = animation_summary(payload, offset, 1, 1)
    bone = pose["bones"][0]
    assert bone["parent"] == -1 and bone["name"] == "Plane02"
    keys = bone["actions"][0]
    assert keys["keyCount"] == 1
    translation = keys["positionFirst"]
    qx, qy, qz, qw = quaternion(keys["rotationFirst"])
    # Native QuaternionMatrix row2 (the local-vertex contribution to world Z).
    row2 = (
        2 * qx * qz - 2 * qw * qy,
        2 * qy * qz + 2 * qw * qx,
        1 - 2 * qx * qx - 2 * qy * qy,
    )
    assert all(abs(a - b) < .001 for a, b in zip(row2, (-1, 0, 0)))
    # Calc_RenderObject(false,0,0) passes !Translate=true to BMD::Animation:
    # the yaw-only ParentMatrix carries BodyScale=.7 and world BodyOrigin.
    relative = [round(.7 * (translation[2] + sum(row2[i] * vertex[i]
                 for i in range(3))), 3)
                for vertex in mesh[0]["localVerticesXYZ"]]
    assert min(relative) < 0 and max(relative) > 60
    print(json.dumps({
        "source": name, "sourceSha256": SOURCES[name],
        "stagedPlaintextEqual": True,
        "bone": bone["name"], "boneRotationRadians": keys["rotationFirst"],
        "boneTranslation": translation, "nativeQuaternionRowZ": row2,
        "bodyScale": .7, "yawOnlyRelativeVertexZ": relative,
        "yawOnlyRelativeZRange": [min(relative), max(relative)],
        "exampleQaWorldOriginZ": 184.5,
        "exampleQaWorldZRange": [round(184.5 + min(relative), 3),
                                 round(184.5 + max(relative), 3)],
        "scope": "native BMD yaw-only transform calculation, not real frame GPU/depth/pixels",
    }, indent=2))


if __name__ == "__main__":
    main()
