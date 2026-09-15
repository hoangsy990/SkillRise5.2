#!/usr/bin/env python3
"""Read-only structural parity check for S21 Slayer black-field models.

The v0F/v0C envelopes differ, but the imported BMD plaintext must not.
This reports authored mesh bounds and action keys without modifying assets.
"""

from __future__ import annotations

import importlib.util
import struct
from pathlib import Path


LOCAL_CONVERTER = Path(__file__).resolve().parents[1] / "grow_lancer" / "convert_s21_bmd_v0f_to_rise_v0c.py"
PINNED_CONVERTER = Path(r"C:\Users\DELL\Documents\Codex\2026-09-14\do\work\SkillRise5.2\snapshot\tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py")
CONVERTER = LOCAL_CONVERTER if LOCAL_CONVERTER.is_file() else PINNED_CONVERTER
NATIVE_EFFECT = Path(r"D:\MU FICA Season 21\Data\Effect")
IMPORTED_EFFECT = Path(r"D:\RISE-CrossPlatform\Source_PC_Slayer\ExMain_RISE_PC\Tests\SlayerBuild\Client\Data\RISE\Slayer\Effect")
MODELS = {
    "van_object03_skill.bmd": (
        "EC83F31E6AA0D3FA8468A54DEA562BF3AA6A96161FD8603CA479CA292BEC75DB",
        1, 248, 331.94268798828125,
    ),
    "Van_object04_skill.bmd": (
        "E223A0D578DEB234AE30CB4CC67FBA1809B7D640CC86B2A7D7031754D9A0EC6D",
        2, 152, 0.0,
    ),
    "marks_cylinder.bmd": (
        "F91EA00CFC10DC3E36935FE80AD6CA391EAED64142E7D572E5AB2939A7689BE3",
        1, 72, 214.58230590820312,
    ),
    "van_object02_skill.bmd": (
        "EC45FCBADBC129D1056D64EBFB70EA32D3AD3C0A29F47AF7D341A5A762DF5A5B",
        3, 64, 8.272600173950195,
    ),
    "Bat_van01.bmd": (
        "A01A9F4C767811CF5B5E2E855354A6DE2C63976C26BBE8A40DE886808E713C7E",
        1, 124, 6.300172805786133,
    ),
}


def converter():
    spec = importlib.util.spec_from_file_location("s21_bmd", CONVERTER)
    if spec is None or spec.loader is None:
        raise AssertionError("missing pinned BMD converter")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def parse(plain: bytes):
    # The converter returns the decrypted payload after the BMD envelope;
    # its first 32 bytes are the model name, then the Open2 mesh counts.
    offset = 32
    mesh_count, bone_count, action_count = struct.unpack_from("<hhh", plain, offset)
    offset += 6
    mesh_rows = []
    for mesh_index in range(mesh_count):
        vertices, normals, texcoords, triangles, material = struct.unpack_from(
            "<hhhhh", plain, offset
        )
        offset += 10
        bounds = []
        for vertex_index in range(vertices):
            bounds.append(struct.unpack_from("<3f", plain, offset + vertex_index * 16 + 4))
        uv_offset = offset + vertices * 16 + normals * 20
        uvs = [struct.unpack_from("<2f", plain, uv_offset + index * 8)
               for index in range(texcoords)]
        # Win32 C++ layout: Vertex_t=16, Normal_t=20, TexCoord_t=8,
        # Triangle_t2=64 (including the alignment gap after Polygon and
        # trailing struct padding in the Win32 compiler layout).
        offset += vertices * 16 + normals * 20 + texcoords * 8 + triangles * 64
        texture = plain[offset:offset + 32].split(b"\0", 1)[0].decode("ascii")
        offset += 32
        mesh_rows.append((mesh_index, vertices, triangles, material, texture,
                          tuple((min(v[axis] for v in bounds),
                                 max(v[axis] for v in bounds))
                                for axis in range(3)),
                          tuple((min(uv[axis] for uv in uvs),
                                 max(uv[axis] for uv in uvs))
                                for axis in range(2))))
    actions = []
    for _ in range(action_count):
        keys, locked = struct.unpack_from("<hB", plain, offset)
        offset += 3
        if locked:
            offset += keys * 12
        actions.append((keys, locked))
    bone_rows = []
    for bone_index in range(bone_count):
        dummy = plain[offset]
        offset += 1
        if dummy:
            bone_rows.append((bone_index, "dummy", None))
            continue
        name = plain[offset:offset + 32].split(b"\0", 1)[0].decode("ascii")
        offset += 32
        parent = struct.unpack_from("<h", plain, offset)[0]
        offset += 2
        starts = []
        position_ranges = []
        rotation_ranges = []
        for keys, _ in actions:
            positions = [struct.unpack_from("<3f", plain, offset + key * 12)
                         for key in range(keys)]
            rotations = [struct.unpack_from("<3f", plain, offset + keys * 12 + key * 12)
                         for key in range(keys)]
            starts.append(positions[0])
            position_ranges.append(tuple((min(v[axis] for v in positions),
                                          max(v[axis] for v in positions))
                                         for axis in range(3)))
            rotation_ranges.append(tuple((min(v[axis] for v in rotations),
                                          max(v[axis] for v in rotations))
                                         for axis in range(3)))
            offset += keys * 12 * 2
        bone_rows.append((bone_index, name, parent, starts,
                          position_ranges, rotation_ranges))
    if offset > len(plain) or len(plain) - offset > 15:
        raise AssertionError(f"BMD structure/padding mismatch: {offset}/{len(plain)}")
    return mesh_rows, actions, bone_rows


def main() -> None:
    module = converter()
    for name, (expected_hash, expected_meshes, expected_vertices,
               expected_max_z) in MODELS.items():
        native = (NATIVE_EFFECT / name).read_bytes()
        if module.sha256(native) != expected_hash:
            raise AssertionError(f"native {name} hash drifted")
        plain = module.decrypt_s21_payload(module.parse_s21_container(native))
        imported_path = IMPORTED_EFFECT / name
        if imported_path.exists() and module.decrypt_rise_v0c(imported_path.read_bytes()) != plain:
            raise AssertionError(f"Slayer v0C import changed authored {name} plaintext")
        meshes, actions, bones = parse(plain)
        max_z = max(mesh[5][2][1] for mesh in meshes)
        if len(meshes) != expected_meshes or max(mesh[1] for mesh in meshes) != expected_vertices or abs(max_z - expected_max_z) > 0.001:
            raise AssertionError(f"authored {name} geometry drifted")
        expected_materials = (
            ("Elite_monster_ground02.JPG",)
            if name == "van_object03_skill.bmd" else
            ("lines2.jpg",)
            if name == "marks_cylinder.bmd" else
            ("marks_m03.jpg", "empact01.jpg", "macardkmono.jpg")
            if name == "van_object02_skill.bmd" else
            ("bet_grilsshot2.jpg",)
            if name == "Bat_van01.bmd" else
            ("ark.jpg", "empact01.jpg")
        )
        if tuple(mesh[4].lower() for mesh in meshes) != \
           tuple(material.lower() for material in expected_materials):
            raise AssertionError(f"S21 buff model material mapping drifted: {name}")
        for mesh in meshes:
            u_min, u_max = mesh[6][0]
            v_min, v_max = mesh[6][1]
            if name == "marks_cylinder.bmd":
                if abs(u_min - 0.0816) > 0.0001 or \
                   abs(u_max - 0.9699) > 0.0001 or \
                   abs(v_min) > 0.0001 or \
                   abs(v_max - 6.6296) > 0.0001:
                    raise AssertionError(f"S21 Pierce cylinder UV repeat drifted: {name}/mesh{mesh[0]}")
            elif name == "Bat_van01.bmd":
                if abs(u_min - 0.0216) > 0.0001 or abs(u_max - 0.9661) > 0.0001 or \
                   abs(v_min - 0.0230) > 0.0001 or abs(v_max - 0.9802) > 0.0001:
                    raise AssertionError(f"S21 Bat mesh UV field drifted: {name}/mesh{mesh[0]}")
            elif abs(u_min) > 0.0001 or abs(u_max - 1) > 0.0001 or \
                 (name == "Van_object04_skill.bmd" and
                  (abs(v_min) > 0.0001 or abs(v_max - 1) > 0.0001)):
                raise AssertionError(f"S21 buff model UV field drifted: {name}/mesh{mesh[0]}")
            if name == "van_object02_skill.bmd" and \
               (abs(v_min) > 0.0001 or abs(v_max - 1) > 0.0001):
                raise AssertionError(f"S21 Bat trail UV field drifted: {name}/mesh{mesh[0]}")
        mesh_summary = [(mesh[0], mesh[3], mesh[4], mesh[6]) for mesh in meshes]
        print(f"PASS: {name} S21 mesh={len(meshes)} largest={expected_vertices} vertices maxZ={max_z:.3f} actions={actions} bones={len(bones)} materials/UV={mesh_summary} plaintext={module.sha256(plain)} imported={'identical' if imported_path.exists() else 'not staged'}")


if __name__ == "__main__":
    main()
