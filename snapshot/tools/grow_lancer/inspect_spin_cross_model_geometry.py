"""Hash-pinned read-only S21 Spin Step cross geometry and private v0C parity."""

import json
from pathlib import Path

from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_rise_v0c, decrypt_s21_payload, parse_s21_container, sha256,
)
from inspect_equipment_meshes import inspect
from inspect_magic_pin_model_geometry import mesh_bounds, animation_summary


SOURCE = Path(r"D:\MU FICA Season 21\Data\Effect\spin_cross.bmd")
STAGED = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\SpinStep\spin_cross.bmd")
SOURCE_SHA = "B009704237F2B98BD8A1778CAB1FD3A35C6C926B4857E9D8CFDEA7549B7A566B"
STAGED_SHA = "6E0B4051D4D972EB1FEFCA8BA14A037FBB90A882D3B4B3026E9150503130C15D"


def main() -> None:
    original = SOURCE.read_bytes()
    staged = STAGED.read_bytes()
    assert sha256(original) == SOURCE_SHA, "S21 source BMD drifted"
    assert sha256(staged) == STAGED_SHA, "private converted BMD drifted"
    payload = decrypt_s21_payload(parse_s21_container(original))
    assert decrypt_rise_v0c(staged) == payload, "converted mesh is not S21 plaintext"
    layout = inspect(payload)
    assert layout["meshCount"] == layout["boneCount"] == layout["actionCount"] == 1
    meshes, offset = mesh_bounds(payload, layout["meshCount"])
    assert len(meshes) == 1 and meshes[0]["triangles"] > 0
    assert meshes[0]["texture"].lower() == "line_fire.jpg"
    animation = animation_summary(payload, offset, layout["actionCount"],
                                  layout["boneCount"])
    print(json.dumps({"sourceSha256": SOURCE_SHA, "stagedSha256": STAGED_SHA,
                      "layout": layout, "meshes": meshes,
                      "animation": animation}, indent=2))


if __name__ == "__main__":
    main()
