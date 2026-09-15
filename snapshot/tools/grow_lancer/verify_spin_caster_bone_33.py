"""Read-only S21/isolated-player bone and Spin action identity check."""

from hashlib import sha256
from pathlib import Path
from struct import unpack_from

import compare_player_actions as clips
import merge_player_actions as bmd


ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(r"D:\MU FICA Season 21\Data\Player\player.bmd")
NATIVE = (ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" /
          "Client" / "Data" / "Player" / "player.bmd")
SOURCE_SHA = "E0F4CB5A0956192D04026135766C566BC6B9B7B559D2F196F74D24D28E2E755A"
NATIVE_SHA = "0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63"


def identity(path: Path, expected_sha: str):
    raw = path.read_bytes()
    assert sha256(raw).hexdigest().upper() == expected_sha, path
    payload = bmd.decode(path)
    skeleton = bmd.parse(payload)
    actions = clips.parse_zero_mesh_skeleton(payload)
    assert skeleton.bone_count == 60
    header = skeleton.bone_headers[33]
    assert len(header) == 35 and header[0] == 0
    name = header[1:33].split(b"\0", 1)[0].decode("ascii")
    assert name == "knife_gdf", (path, name)
    parent = unpack_from("<h", header, 33)[0]
    return skeleton, actions, (name, parent)


def main() -> None:
    source, source_actions, source_bone = identity(SOURCE, SOURCE_SHA)
    native, native_actions, native_bone = identity(NATIVE, NATIVE_SHA)
    # Bone-header padding is not canonical and differs across the clients.
    # Identity is dummy/name/parent, not a misleading raw-header equality.
    assert source_bone == native_bone == ("knife_gdf", 28)
    assert source_actions.key_counts[186] == native_actions.key_counts[285] == 6
    assert source_actions.action_hashes[186] == native_actions.action_hashes[285]
    runtime = (ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" /
               "GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
    body = runtime.split("void EmitSpinStepWeaponBlur(", 1)[1].split(
        "void CreateCarrier(", 1)[0]
    assert "BoneTransform[33]" in body
    assert "Vector(0.0f, -180.0f, 20.0f, localEnd)" in body
    assert "CreateObjectBlur(&effect, start, end, light, 1" in body
    print("PASS Spin caster bone33 knife_gdf and S21 action186/native285 exact clip")
    print("OPEN playback speed, world transform, pool allocation and gameplay pixels")


if __name__ == "__main__":
    main()
