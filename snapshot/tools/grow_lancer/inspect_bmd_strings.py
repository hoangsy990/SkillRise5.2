#!/usr/bin/env python3
"""Print printable strings from a v0F/v0C BMD plaintext payload."""

from __future__ import annotations

import argparse
import importlib.util
import re
from pathlib import Path


HERE = Path(__file__).resolve().parent
CONVERTER_PATH = HERE / "convert_s21_bmd_v0f_to_rise_v0c.py"


def load_converter():
    spec = importlib.util.spec_from_file_location("grow_lancer_bmd_converter", CONVERTER_PATH)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load converter: {CONVERTER_PATH}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("bmd", type=Path)
    args = parser.parse_args()

    converter = load_converter()
    container = args.bmd.read_bytes()
    if container[:4] == b"BMD\x0f":
        plaintext = converter.decrypt_s21_payload(converter.parse_s21_container(container))
        version = "v0F"
    elif container[:4] == b"BMD\x0c":
        plaintext = converter.decrypt_rise_v0c(container)
        version = "v0C"
    else:
        raise ValueError(f"unsupported container: {container[:4]!r}")

    print(f"Path={args.bmd.resolve()}")
    print(f"Version={version}")
    for match in re.finditer(rb"[ -~]{4,}", plaintext):
        print(f"0x{match.start():08X} {match.group().decode('ascii')}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
