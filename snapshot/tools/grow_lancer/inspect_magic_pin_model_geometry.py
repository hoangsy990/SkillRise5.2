"""Read-only, hash-pinned Magic Pin mesh/action data; no runtime/GPU claim."""

import argparse
import json
import struct
from pathlib import Path

from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_rise_v0c, decrypt_s21_payload, parse_s21_container, sha256,
)
from inspect_equipment_meshes import inspect


SOURCE_DIR = Path(r"D:\MU FICA Season 21\Data\Effect")
STAGED_DIR = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\MagicPin")
SOURCES = {
    "magicpin01.bmd": "2C1F5FD48EA22FD4497A749F46397F31769BFAED74AA3F0672E52C78EF1CF292",
    "magicpin03_new.bmd": "6F3BFA98A5B8BC1A792E92130B72142BC3FB84C9B882AA4BA3DA7CE883DA9B67",
    "magicpina01_new.bmd": "2CF882BE345DC9088EF6345CAF65FBCD64D8F5C0AFDE78877C89EE9E7282B977",
    "magicpina02.bmd": "DC913B31FE27293687DB38352F74D52785179636D8EB713227A7AE39A206B95C",
}
STAGED_SHA = {
    "magicpin01.bmd": "63C557190E4675476B6BCD3D91826078A4B01430A0C056B88A33D34E5BBA25A9",
    "magicpin03_new.bmd": "20400EFCBD810E82B63F9BE511D812B7C3EB8BEF3F885C8B82B261EE27C4ED9F",
    "magicpina01_new.bmd": "4963E0278FE8F3D4B42F683CFA24BCF8E178DBB210FC23FE8DA10FB3D5A90D72",
    "magicpina02.bmd": "04EBD89D5A28A2C583B8FFE99BA8DC9F155772D4B78327EB072847F4A4AD4986",
}


def mesh_bounds(payload: bytes, mesh_count: int):
    offset = 38
    result = []
    for index in range(mesh_count):
        nv, nn, nt, nf, texture_index = struct.unpack_from("<5h", payload, offset)
        offset += 10
        vertices = [struct.unpack_from("<hxxfff", payload, offset + i * 16)
                    for i in range(nv)]
        texcoord_offset = offset + nv * 16 + nn * 20
        texcoords = [struct.unpack_from("<ff", payload, texcoord_offset + i * 8)
                     for i in range(nt)]
        triangle_offset = offset + nv * 16 + nn * 20 + nt * 8
        triangle_vertices = [struct.unpack_from("<3h", payload,
                             triangle_offset + i * 64 + 2)
                             for i in range(nf)]
        triangle_uvs = [struct.unpack_from("<3h", payload,
                        triangle_offset + i * 64 + 18)
                        for i in range(nf)]
        offset += nv * 16 + nn * 20 + nt * 8 + nf * 64
        texture = payload[offset:offset + 32].split(b"\0", 1)[0].decode("ascii")
        offset += 32
        xyz = [row[1:] for row in vertices]
        bounds = [[round(min(row[axis] for row in xyz), 4),
                   round(max(row[axis] for row in xyz), 4)]
                  for axis in range(3)] if xyz else []
        result.append(dict(mesh=index, vertices=nv, triangles=nf,
                           textureIndex=texture_index, texture=texture,
                           usedVertexBones=sorted({row[0] for row in vertices}),
                           localXYZBounds=bounds,
                           localVerticesXYZ=[[round(value, 3) for value in row[1:]]
                                             for row in vertices],
                           triangleVertexIndices=triangle_vertices,
                           localUV=[[round(value, 5) for value in row]
                                    for row in texcoords],
                           triangleUVIndices=triangle_uvs))
    return result, offset


def animation_summary(payload: bytes, offset: int, action_count: int,
                      bone_count: int):
    """Follow the native BMD::Open action, then per-bone Position/Rotation order."""
    actions = []
    for _ in range(action_count):
        count, locked = struct.unpack_from("<hB", payload, offset)
        offset += 3
        root_positions = [struct.unpack_from("<3f", payload, offset + i * 12)
                          for i in range(count)] if locked else []
        offset += count * 12 if locked else 0
        actions.append(dict(keys=count, locked=bool(locked),
                            rootPositionFirst=root_positions[0] if root_positions else None,
                            rootPositionLast=root_positions[-1] if root_positions else None))
    bones = []
    for _ in range(bone_count):
        dummy = payload[offset]
        offset += 1
        if dummy:
            bones.append(dict(dummy=True))
            continue
        name = payload[offset:offset + 32].split(b"\0", 1)[0].decode("ascii")
        parent = struct.unpack_from("<h", payload, offset + 32)[0]
        offset += 34
        keys = []
        for action in actions:
            count = action["keys"]
            positions = [struct.unpack_from("<3f", payload, offset + i * 12)
                         for i in range(count)]
            offset += count * 12
            rotations = [struct.unpack_from("<3f", payload, offset + i * 12)
                         for i in range(count)]
            offset += count * 12
            keys.append(dict(positionFirst=positions[0] if positions else None,
                             positionLast=positions[-1] if positions else None,
                             rotationFirst=rotations[0] if rotations else None,
                             rotationLast=rotations[-1] if rotations else None,
                             keyCount=count))
        bones.append(dict(name=name, parent=parent, dummy=False, actions=keys))
    if len(payload) - offset > 16:
        raise ValueError("Animation parser did not reach native BMD tail")
    return dict(actions=actions, bones=bones, parsedBytes=offset,
                paddingBytes=len(payload) - offset)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--animation-summary", action="store_true")
    args = parser.parse_args()
    reports = []
    for file, expected_sha in SOURCES.items():
        source = SOURCE_DIR / file
        raw = source.read_bytes()
        if sha256(raw) != expected_sha:
            raise ValueError(f"Source SHA-256 mismatch: {source}")
        payload = decrypt_s21_payload(parse_s21_container(raw))
        staged = (STAGED_DIR / file).read_bytes()
        if sha256(staged) != STAGED_SHA[file]:
            raise ValueError(f"Staged RISE BMD SHA-256 mismatch: {file}")
        if decrypt_rise_v0c(staged) != payload:
            raise ValueError(f"Staged RISE BMD plaintext differs from S21: {file}")
        layout = inspect(payload)  # full bounds/triangle/bone/action validation
        meshes, offset = mesh_bounds(payload, layout["meshCount"])
        animation = animation_summary(payload, offset,
                                      layout["actionCount"], layout["boneCount"])
        reports.append(dict(source=str(source), sha256=expected_sha,
                            stagedSource=str(STAGED_DIR / file),
                            stagedSha256=STAGED_SHA[file],
                            plaintextEqual=True,
                            modelName=layout["name"],
                            boneCount=layout["boneCount"],
                            actionCount=layout["actionCount"],
                            animation=animation,
                            meshes=None if args.animation_summary else meshes,
                            scope="offline mesh/animation coordinates only; no caster-foot pixel attribution"))
    print(json.dumps(reports, indent=2))


if __name__ == "__main__":
    main()
