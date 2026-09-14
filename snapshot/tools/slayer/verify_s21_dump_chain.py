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


def main() -> int:
    if not DUMP.is_file():
        raise FileNotFoundError(DUMP)
    if not DISPATCH.is_file():
        raise FileNotFoundError(DISPATCH)
    if not ACTION_SWITCH.is_file():
        raise FileNotFoundError(ACTION_SWITCH)

    digest = hashlib.sha256(DUMP.read_bytes()).hexdigest().upper()
    if digest != EXPECTED_DUMP_SHA256:
        raise AssertionError(f"S21 dump hash mismatch: {digest}")
    print(f"PASS: S21 main dump sha256={digest}")

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
