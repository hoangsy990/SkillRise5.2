#!/usr/bin/env python3
"""Verify the Slayer dispatcher facts used by the isolated 5.2 port.

This is deliberately a provenance check, not a decompiler.  It pins the
captured S21 main image and asserts that the checked-in disassembly contains
the five skill compares/handlers, their direct effect roots, action values,
sound families, and the separate action-switch roots.  The 5.2 source remains
a native reimplementation; no S21 address is executed by the client.
"""

from __future__ import annotations

import hashlib
import re
import struct
from pathlib import Path


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
DISPATCH = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_full_skill_reverse\disasm_12CEB00_12D1A50.txt"
)
ACTION_SWITCH = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_full_skill_reverse\disasm_10DFF00_10ED000.txt"
)

EXPECTED_DUMP_SHA256 = (
    "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"
)

# These byte prefixes are read directly from the hash-pinned PE below.  The
# checked-in disassembly is useful for human review, but it must not be the
# only source of truth: a hand-edited text disassembly could otherwise make a
# false provenance claim.  The prefixes cover the five skill compares, the
# handler prologues, and the action/sound pushes in the second switch.
COMPARE_BYTES = {
    292: (0x012CEF03, "81 bd b0 e9 ff ff 24 01 00 00 0f 84 fc 1f 00 00"),
    293: (0x012CEF13, "81 bd b0 e9 ff ff 25 01 00 00 0f 84 7d 21 00 00"),
    294: (0x012CEF23, "81 bd b0 e9 ff ff 26 01 00 00 0f 84 d6 22 00 00"),
    # 295 has a range guard at 0x12CEE1E and the equality jump immediately
    # after it.  Both are pinned below; the latter is the handler edge.
    295: (0x012CEE1E, "81 bd b0 e9 ff ff 27 01 00 00 0f 87 0a 01 00 00"),
    297: (0x012CEF58, "81 bd b0 e9 ff ff 29 01 00 00 0f 84 db 24 00 00"),
}

EQUALITY_BYTES = {
    295: (0x012CEE2E, "81 bd b0 e9 ff ff 27 01 00 00 0f 84 5d 25 00 00"),
}

HANDLER_BYTES = {
    292: (0x012D0F0F, "81 bd b4 e9 ff ff 18 08 00 00 0f 85 7c 01 00 00"),
    293: (0x012D10A0, "8b 85 b8 e9 ff ff 3b 05 34 46 48 0a 0f 84 52 01 00"),
    294: (0x012D1209, "81 bd b4 e9 ff ff 16 08 00 00 0f 85 7d 01 00 00"),
    295: (0x012D139B, "8b 85 b8 e9 ff ff 3b 05 34 46 48 0a 0f 84 91 00 00"),
    297: (0x012D1443, "8b 85 b8 e9 ff ff 3b 05 34 46 48 0a 0f 84 91 00 00"),
}

ACTION_BYTES = {
    292: (0x010E68A3, "6a 01 6a 00 8b 45 d4 0f b7 40 14 50"),
    293: (0x010E6B96, "6a 01 6a 00 8b 45 d4 0f b7 40 14 50"),
    294: (0x010E6D1B, "6a 01 6a 00 8b 45 d4 0f b7 40 14 50"),
    295: (0x010E6F2A, "6a 01 6a 00 8b 45 d4 0f b7 40 14 50"),
    297: (0x010E6FF8, "6a 01 6a 00 8b 45 d4 0f b7 40 14 50"),
}

# Immediate pushes are stronger than checking the textual mnemonic: they
# prove the action/sound values came from these exact bytes in the dump.
ACTION_PUSH_BYTES = {
    292: (0x010E68E1, "68 e0 00 00 00"),
    293: (0x010E6BC7, "68 e3 00 00 00"),
    294: (0x010E6D59, "68 e4 00 00 00"),
    295: (0x010E6F68, "68 e8 00 00 00"),
    297: (0x010E7036, "68 e9 00 00 00"),
}

SOUND_PUSH_BYTES = {
    292: (0x010E68FD, "68 09 05 00 00"),
    293: (0x010E6BE3, "68 0b 05 00 00"),
    294: (0x010E6D75, "68 0d 05 00 00"),
    295: (0x010E6F84, "68 11 05 00 00"),
    297: (0x010E7052, "68 11 05 00 00"),
}

# skill: compare VA, handler VA, direct root values, handler action (when the
# receive handler emits it), and sound.  292/294 set their player action in
# the separate action switch below, so their handler action is intentionally
# None.
DISPATCH_FACTS = {
    292: ("012CEF03", "012D0F0F", ("68a", "689"), None, "50d"),
    293: ("012CEF13", "012D10A0", ("682", "683", "684"), "e3", "50b"),
    294: ("012CEF23", "012D1209", ("679",), None, "509"),
    295: ("012CEE1E", "012D139B", ("692",), "e8", "511"),
    297: ("012CEF58", "012D1443", ("695",), "e9", "511"),
}

# The second switch is a call-site distinction.  These are direct roots in
# the relevant branch, not additional ReceiveMagic children.
ACTION_BRANCHES = {
    292: ("010E68A3", ("679", "68a", "689"), "e0"),
    293: ("010E6B96", ("682", "683", "684"), "e3"),
    294: ("010E6D1B", ("68a", "679"), "e4"),
    295: ("010E6F2A", ("692",), "e8"),
    297: ("010E6FF8", ("695",), "e9"),
}


def require(text: str, needle: str, label: str) -> None:
    if needle.lower() not in text.lower():
        raise AssertionError(f"missing {label}: {needle}")


def handler_window(text: str, handler: str, next_handler: str | None) -> str:
    start = text.lower().find(handler.lower())
    if start < 0:
        raise AssertionError(f"missing handler {handler}")
    end = text.lower().find(next_handler.lower(), start + len(handler)) if next_handler else len(text)
    return text[start:end if end >= 0 else len(text)]


def rva_to_file_offset(data: bytes, rva: int) -> int:
    """Map an RVA through the PE section table without external packages."""
    if data[:2] != b"MZ":
        raise AssertionError("dump is not an MZ image")
    pe_offset = struct.unpack_from("<I", data, 0x3C)[0]
    if data[pe_offset:pe_offset + 4] != b"PE\0\0":
        raise AssertionError("dump has no PE signature")
    number_of_sections = struct.unpack_from("<H", data, pe_offset + 6)[0]
    optional_size = struct.unpack_from("<H", data, pe_offset + 20)[0]
    section_table = pe_offset + 24 + optional_size
    for index in range(number_of_sections):
        section = section_table + index * 40
        virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from(
            "<IIII", data, section + 8
        )
        span = max(virtual_size, raw_size)
        if virtual_address <= rva < virtual_address + span:
            delta = rva - virtual_address
            if delta >= raw_size:
                raise AssertionError(f"RVA 0x{rva:x} is not backed by file data")
            return raw_pointer + delta
    raise AssertionError(f"RVA 0x{rva:x} is outside PE sections")


def read_va(data: bytes, va: int, size: int) -> bytes:
    rva = va - 0x00400000
    if rva < 0:
        raise AssertionError(f"VA 0x{va:x} precedes the PE image")
    offset = rva_to_file_offset(data, rva)
    value = data[offset:offset + size]
    if len(value) != size:
        raise AssertionError(f"short read at VA 0x{va:x}")
    return value


def verify_dump_bytes(dump_data: bytes) -> None:
    def check(table: dict[int, tuple[int, str]], label: str) -> None:
        for skill, (va, hex_bytes) in table.items():
            expected = bytes.fromhex(hex_bytes)
            actual = read_va(dump_data, va, len(expected))
            if actual != expected:
                raise AssertionError(
                    f"{label} bytes mismatch skill {skill} at 0x{va:x}: "
                    f"{actual.hex(' ')}"
                )
            print(f"PASS: dump-bytes {label.lower()} skill={skill} va=0x{va:x}")

    check(COMPARE_BYTES, "compare")
    check(EQUALITY_BYTES, "equality-compare")
    check(HANDLER_BYTES, "handler")
    check(ACTION_BYTES, "action-branch")
    check(ACTION_PUSH_BYTES, "action-push")
    check(SOUND_PUSH_BYTES, "sound-push")

    # Decode the actual conditional-jump displacement for every direct edge.
    # The 295 edge is the second equality compare above; its first compare is
    # intentionally only a range guard.
    edge_sites = {skill: va for skill, (va, _) in COMPARE_BYTES.items() if skill != 295}
    edge_sites.update({295: EQUALITY_BYTES[295][0]})
    expected_handlers = {
        292: 0x012D0F0F,
        293: 0x012D10A0,
        294: 0x012D1209,
        295: 0x012D139B,
        297: 0x012D1443,
    }
    for skill, va in edge_sites.items():
        instruction = read_va(dump_data, va, 16)
        if instruction[10:12] not in (b"\x0f\x84", b"\x0f\x85"):
            raise AssertionError(f"skill {skill} compare has no equality edge at 0x{va:x}")
        displacement = struct.unpack_from("<i", instruction, 12)[0]
        target = va + 16 + displacement
        if target != expected_handlers[skill]:
            raise AssertionError(
                f"skill {skill} jump target mismatch: 0x{target:x} "
                f"!= 0x{expected_handlers[skill]:x}"
            )
        print(f"PASS: dump-edge skill={skill} compare=0x{va:x} -> handler=0x{target:x}")


def main() -> int:
    if not DUMP.is_file():
        raise FileNotFoundError(DUMP)
    if not DISPATCH.is_file():
        raise FileNotFoundError(DISPATCH)
    if not ACTION_SWITCH.is_file():
        raise FileNotFoundError(ACTION_SWITCH)

    dump_data = DUMP.read_bytes()
    digest = hashlib.sha256(dump_data).hexdigest().upper()
    if digest != EXPECTED_DUMP_SHA256:
        raise AssertionError(f"S21 dump hash mismatch: {digest}")
    print(f"PASS: S21 main dump sha256={digest}")
    verify_dump_bytes(dump_data)

    dispatch_text = DISPATCH.read_text(encoding="utf-8", errors="replace")
    action_text = ACTION_SWITCH.read_text(encoding="utf-8", errors="replace")
    ordered_handlers = [facts[1] for facts in DISPATCH_FACTS.values()]
    for skill, (compare, handler, roots, action, sound) in DISPATCH_FACTS.items():
        require(dispatch_text, f"{compare}  ", f"skill {skill} compare VA")
        require(dispatch_text, f"cmp      dword ptr [ebp - 0x1650], 0x{skill:x}", f"skill {skill} compare")
        require(dispatch_text, handler, f"skill {skill} handler VA")
        next_handler = ordered_handlers[ordered_handlers.index(handler) + 1] if handler != ordered_handlers[-1] else None
        window = handler_window(dispatch_text, handler, next_handler)
        for root in roots:
            require(window, f"push     0x{root}", f"skill {skill} root 0x{root}")
        if action is not None:
            require(window, f"push     0x{action}", f"skill {skill} action 0x{action}")
        require(window, f"push     0x{sound}", f"skill {skill} sound 0x{sound}")
        print(
            f"PASS: skill={skill} compare=0x{compare} handler=0x{handler} "
            f"roots={','.join('0x' + r for r in roots)} "
            f"handlerAction={'0x' + action if action else 'action-switch'} sound=0x{sound}"
        )

    for skill, (branch, roots, action) in ACTION_BRANCHES.items():
        require(action_text, branch, f"skill {skill} action branch")
        start = action_text.lower().find(branch.lower())
        next_starts = [
            action_text.lower().find(other[0].lower(), start + len(branch))
            for other in ACTION_BRANCHES.values()
            if action_text.lower().find(other[0].lower(), start + len(branch)) >= 0
        ]
        end = min(next_starts) if next_starts else len(action_text)
        window = action_text[start:end]
        for root in roots:
            require(window, f"push     0x{root}", f"skill {skill} action root 0x{root}")
        require(window, f"push     0x{action}", f"skill {skill} action id 0x{action}")
        print(f"PASS: action-switch skill={skill} branch=0x{branch} roots={','.join('0x'+r for r in roots)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
