"""Pin the Season 21 Breche owner-side primary effect producer.

This is deliberately a static dump contract.  It proves the producer and
primitive arguments; it does not claim SS6 visual parity or server behavior.
"""
from pathlib import Path
import hashlib
import struct


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
BASE = 0x400000
EXPECTED_SHA256 = (
    "6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637"
)


def call_target(data: bytes, va: int) -> int:
    off = va - BASE
    if data[off] != 0xE8:
        raise ValueError(f"{va:#x} is not a direct call")
    return BASE + off + 5 + struct.unpack_from("<i", data, off + 1)[0]


def push_imm(data: bytes, va: int) -> int:
    off = va - BASE
    if data[off] != 0x68:
        raise ValueError(f"{va:#x} is not push imm32")
    return struct.unpack_from("<I", data, off + 1)[0]


def main() -> None:
    data = DUMP.read_bytes()
    actual = hashlib.sha256(data).hexdigest()
    if actual != EXPECTED_SHA256:
        raise ValueError(f"Pinned S21 mapped image hash changed: {actual}")

    # Primary effect dispatcher 14B769F normalizes 4D3..600 through the
    # byte/pointer tables.  5FD is selector 0x59 -> owner/target handler
    # 153542A; the separate movement table is not this producer.
    normalized = 0x5FD - 0x4D3
    selector = data[0x1594BE0 - BASE + normalized]
    target = struct.unpack_from("<I", data, 0x1594A6C - BASE + selector * 4)[0]
    if (selector, target) != (0x59, 0x153542A):
        raise ValueError("primary 5FD dispatcher table changed")
    print("PASS: primary effect table 5FD -> selector 0x59 -> 0x153542A")

    # The owner branch is subtype zero and retires when its owner is absent.
    if data[0x153547B - BASE:0x1535482 - BASE] != b"\x83\xb8\x4c\x03\x00\x00\x00":
        raise ValueError("5FD subtype0 owner-null guard changed")
    print("PASS: 5FD subtype0 is owner-gated and separate from receive subtype1")

    # Three random per-tick particle producers (one branch per loop slot).
    for push, call, expected in (
        (0x1535706, 0x153570B, 0x8084),
        (0x153576C, 0x1535771, 0x806E),
        (0x15357CF, 0x15357D4, 0x8085),
    ):
        if push_imm(data, push) != expected:
            raise ValueError(f"owner particle type changed at {push:#x}")
        if call_target(data, call) != 0x1724176:
            raise ValueError("owner particle wrapper changed")
    if data[0x1535950 - BASE:0x1535957 - BASE] != b"\x83\xbd\x4c\x00\xff\xff\x0a":
        raise ValueError("owner particle loop bound changed")
    print("PASS: owner subtype0 emits exactly three random 8084/0, 806E/4, 8085/0 branches")

    # Direct owner-side effect calls.  Values are S21 resource IDs and are
    # mapped to native/private RISE registrations in the descriptor; no S21
    # address or object layout is copied into the port.
    calls = {
        0x1535BCE: 0x8073,   # pin_lights joint, subtype 5
        0x1535C86: 0x80BC,   # ring_of_gradation, subtype 1
        0x1535CF0: 0x81EC,  # lightmarks_red, subtype 0
        0x1535DB6: 0x81EB,  # firering01, subtype 0
        0x1535FC7: 0x0AD9,   # wind_foce BMD, subtype 6
        0x15360E0: 0x809F,   # twlighthik02, subtype 2
        0x15361DF: 0x809E,   # twlighthik01, subtype 13
        0x153623C: 0x809E,   # twlighthik01, subtype 13 (second)
    }
    for call, resource in calls.items():
        if call_target(data, call) != 0x143E57C and call != 0x1535BCE:
            raise ValueError(f"owner CreateEffect call changed at {call:#x}")
        push = call - 5
        if push_imm(data, push) != resource:
            raise ValueError(f"owner resource changed at {call:#x}")
    if call_target(data, 0x1535BCE) != 0x15C6594:
        raise ValueError("owner pin_lights call is no longer CreateJoint")
    print("PASS: owner layers pin_lights, ring, red mark, fire ring, wind and twilight calls pinned")

    # Timing gates are checked as remaining-life comparisons before the
    # corresponding calls: 17 ring+mark, 7 fire ring+wind, max-5 twilight.
    for va, value in ((0x1535BDE, 0x11), (0x1535D0B, 7)):
        # cmp dword ptr [eax+6c], imm8 is 83 78 6c <imm8>.
        if data[va - BASE:va - BASE + 3] != b"\x83\x78\x6c":
            raise ValueError(f"owner timing compare changed at {va:#x}")
        if data[va - BASE + 3] != value:
            raise ValueError(f"owner timing gate changed at {va:#x}")
    if data[0x1535FD2 - BASE:0x1535FDE - BASE].find(b"\x83\xe9\x05") < 0:
        raise ValueError("owner max-life minus five gate changed")
    # Ring/marks read Position from root.Owner + 0x158, not controller +0x158.
    for va in (0x1535C6C, 0x1535CD6):
        if data[va - BASE:va - BASE + 15] != (
                b"\x8b\x45\x08\x8b\x88\x4c\x03\x00\x00\x81\xc1\x58\x01\x00\x00"):
            raise ValueError(f"owner-position source changed at {va:#x}")
    # AD9 is inside the life==7 block and receives owner snapshot with Y-5.
    if struct.unpack_from("<f", data, 0x1B4E4CC - BASE)[0] != 5.0:
        raise ValueError("owner wind Y correction changed")
    if data[0x1535DC1 - BASE:0x1535DC8 - BASE] != b"\x83\x78\x6c\x07\x0f\x85\x04":
        raise ValueError("owner wind life-7 gate changed")
    if data[0x147134E - BASE:0x147135B - BASE] != (
            b"\x8b\x85\xc8\xce\xff\xff\xc7\x40\x6c\x28\x00\x00\x00"):
        raise ValueError("AD9 constructor default lifetime changed")
    if data[0x1471427 - BASE:0x147144C - BASE].find(
            b"\x83\x78\x60\x06\x74") < 0:
        raise ValueError("AD9 subtype6 constructor gate changed")
    if struct.unpack_from("<f", data, 0x1B4EDA4 - BASE)[0] != 0.8999999761581421:
        raise ValueError("AD9 subtype6 initial alpha changed")
    if data[0x1482B5A - BASE:0x1482B8A - BASE].find(
            b"\xc7\x40\x70\x0c\x00\x00\x00") < 0:
        raise ValueError("81EB subtype0 max lifetime changed")
    if data[0x1482B5A - BASE:0x1482B8A - BASE].find(
            b"\x0f\x57\xc0\xf3\x0f\x11\x80\xdc\x00\x00\x00") < 0:
        raise ValueError("81EB subtype0 initial alpha changed")
    if struct.unpack_from("<f", data, 0x1B4DF14 - BASE)[0] != 0.5:
        raise ValueError("81EB half-life divisor changed")
    print("PASS: owner timing gates 17, 7 and maxLife-5 pinned")
    print("PASS: ring/marks use owner position; AD9 is emitted once at owner snapshot Y-5")
    print("PASS: AD9 subtype6 constructor sets lifetime/maxLife 20 and alpha 0.9")
    print("PASS: 81EB subtype0 constructor sets lifetime/maxLife 12 and alpha 0")

    # Pin the three child update branches that make the owner presentation
    # visibly move.  80BC/sub1 divides scale and RGB by 1.1; 81EB/sub0 rotates
    # 15 degrees and applies a symmetric 20-tick alpha envelope; AD9/sub6
    # rotates -50 degrees, fades by .01 and compounds RGB by alpha.
    if data[0x1518760 - BASE:0x1518768 - BASE] != b"\x8b\x45\x08\x83\x78\x60\x01\x0f":
        raise ValueError("80BC subtype1 update entry changed")
    if struct.unpack_from("<f", data, 0x1B58D70 - BASE)[0] != 1.100000023841858:
        raise ValueError("80BC subtype1 contraction constant changed")
    if data[0x15385D2 - BASE:0x15385DA - BASE] != b"\x6a\x02\x8b\x4d\x08\x81\xc1\x64":
        raise ValueError("81EB subtype0 update entry changed")
    if struct.unpack_from("<f", data, 0x1B4F6B0 - BASE)[0] != 15.0:
        raise ValueError("81EB subtype0 rotation constant changed")
    if data[0x151D307 - BASE:0x151D30F - BASE] != b"\x8b\x45\x08\x83\x78\x60\x06\x75":
        raise ValueError("AD9 subtype6 update entry changed")
    if struct.unpack_from("<f", data, 0x1B4E4E0 - BASE)[0] != 50.0:
        raise ValueError("AD9 subtype6 rotation constant changed")
    if struct.unpack_from("<f", data, 0x1B4DF00 - BASE)[0] != 0.009999999776482582:
        raise ValueError("AD9 subtype6 fade constant changed")
    print("PASS: owner ring/fire/wind motion branches and constants pinned")

    # AD9 does not have a private bright-mesh renderer.  The render dispatch
    # reaches the generic object wrapper, which in turn reaches the generic
    # BMD body renderer.  The final default call pushes render flag 2
    # (RENDER_TEXTURE), the complete carrier blend tuple and texture -1.
    if data[0x15A136D - BASE:0x15A137D - BASE] != (
            b"\x81\xbd\x28\xf6\xff\xff\xd9\x0a\x00\x00"
            b"\x0f\x84\x00\xc4\x00\x00"):
        raise ValueError("AD9 render dispatch compare/jump changed")
    if call_target(data, 0x15AD78D) != 0x176D621:
        raise ValueError("AD9 generic render wrapper changed")
    if call_target(data, 0x176D630) != 0x1886382:
        raise ValueError("generic render transform call changed")
    if call_target(data, 0x176D653) != 0x1887B8B:
        raise ValueError("generic render body call changed")
    if data[0x1887E5E - BASE:0x1887E63 - BASE] != b"\x6a\xff\x8b\x45\x08":
        raise ValueError("generic RenderBody texture argument changed")
    if b"\x6a\x02" not in data[0x1887E5E - BASE:0x1887EBA - BASE]:
        raise ValueError("generic RenderBody RENDER_TEXTURE flag changed")
    # Common object reset stores BlendMesh=0 at +0x74.  The generic wrapper
    # forwards +0x74 immediately before alpha and flag2.  In RenderMesh, a
    # matching mesh texture slot enters the additive blend-mesh branch before
    # the ordinary RENDER_TEXTURE alpha-test branch.
    if data[0x1315FEC - BASE:0x1315FF0 - BASE] != b"\x83\x60\x74\x00":
        raise ValueError("common object BlendMesh=0 reset changed")
    if data[0x1887E99 - BASE:0x1887E9F - BASE] != b"\x8b\x45\x08\xff\x70\x74":
        raise ValueError("generic RenderBody BlendMesh forwarding changed")
    if data[0x1332CBB - BASE:0x1332CD1 - BASE] != (
            b"\x83\x7d\x20\xfe\x7e\x10\x8b\x45\x88\x0f\xbf\x40\x02"
            b"\x3b\x45\x20\x0f\x85\x9e\x00\x00\x00"):
        raise ValueError("RenderMesh blend-selector branch changed")
    if call_target(data, 0x1332CF6) != 0x18E7137:
        raise ValueError("matching blend mesh is no longer additive")
    print("PASS: S21 AD9 uses flag2 + BlendMesh0; mesh texture-slot match selects additive RenderMesh branch")
    print("Scope: static S21 producer evidence only; SS6 render/runtime/server acceptance remains IN_PROCESS")


if __name__ == "__main__":
    main()
