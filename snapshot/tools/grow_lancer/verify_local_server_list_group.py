#!/usr/bin/env python3
"""Read-only check of the isolated QA server-list script's native group map."""

from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[2]
SCRIPT = (
    ROOT / "ExMain_RISE_PC" / "Tests" / "GrowLancerBuild" / "RuntimeQA"
    / "Client" / "Data" / "RISE" / "Config" / "ServerList.bmd"
)
SERVER_CODE = 10  # Isolated Grow Lancer QA CS/GS configuration.
BUX = (0xFC, 0xCF, 0xAB)
RECORD_LENGTH = 2 + 32 + 1 + 1 + 15 + 2


def inspect(data: bytes) -> dict[int, str]:
    groups: dict[int, str] = {}
    offset = 0
    while offset < len(data):
        if len(data) - offset < RECORD_LENGTH:
            raise ValueError(f"truncated native server-group record at {offset}")
        raw = data[offset:offset + RECORD_LENGTH]
        header = bytes(value ^ BUX[i % len(BUX)] for i, value in enumerate(raw))
        index = int.from_bytes(header[:2], "little")
        name = header[2:34].split(b"\0", 1)[0].decode("cp1258", errors="replace")
        description_length = int.from_bytes(header[-2:], "little", signed=True)
        if description_length < 0 or offset + RECORD_LENGTH + description_length > len(data):
            raise ValueError(f"invalid native description length at {offset}")
        if index in groups:
            raise ValueError(f"duplicate native server-group index {index}")
        groups[index] = name
        offset += RECORD_LENGTH + description_length
    return groups


def main() -> int:
    if not SCRIPT.is_file():
        print(f"FAIL missing staged server-list script: {SCRIPT}")
        return 1
    try:
        groups = inspect(SCRIPT.read_bytes())
    except ValueError as exc:
        print(f"FAIL native server-list parse: {exc}")
        return 1
    expected = SERVER_CODE // 20  # Native InsertServerGroup/MakeServerGroup algebra.
    if expected not in groups:
        print(f"FAIL QA code {SERVER_CODE} requires group {expected}; staged groups={groups}")
        return 1
    print(f"PASS QA code {SERVER_CODE} -> native group {expected}: {groups[expected]!r}; staged groups={groups}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
