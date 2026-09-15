#!/usr/bin/env python3
"""Inspect the native S21 Slayer effect graph from the pinned mapped image.

This is a read-only reverse-engineering aid.  It decodes the initializer,
per-frame update and renderer dispatch tables used by the secondary effect
pool, then prints instruction windows or a compact call/immediate summary for
the Slayer roots and their directly referenced children.
"""

from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from capstone.x86_const import X86_OP_IMM


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
IMAGE_BASE = 0x00400000
EXPECTED_SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"

EFFECT_CODES = (
    0x678, 0x679, 0x67A, 0x67B, 0x67C, 0x67D, 0x67E, 0x680, 0x681,
    0x682, 0x683, 0x684, 0x685, 0x686, 0x687, 0x688, 0x689, 0x68A,
    0x68B, 0x68C, 0x68D, 0x68E, 0x68F, 0x690,
    0x691, 0x692, 0x693, 0x694, 0x695, 0x696, 0x697, 0x76C, 0x76D,
)


def u32(data: bytes, va: int) -> int:
    return struct.unpack_from("<I", data, va - IMAGE_BASE)[0]


def initializer_target(data: bytes, code: int) -> int:
    if not 0x628 <= code <= 0x7F4:
        return 0
    return u32(data, 0x14B36E4 + 4 * (code - 0x628))


def update_target(data: bytes, code: int) -> int:
    if not 0x602 <= code <= 0x7E7:
        return 0
    return u32(data, 0x1594D10 + 4 * (code - 0x602))


def render_target(data: bytes, code: int) -> int:
    if 0x4EE <= code <= 0x67E:
        selector = data[0x15BD910 - IMAGE_BASE + code - 0x4EE]
        return u32(data, 0x15BD840 + 4 * selector)
    if 0x680 <= code <= 0x6BC:
        selector = data[0x15BDB08 - IMAGE_BASE + code - 0x680]
        return u32(data, 0x15BDAA4 + 4 * selector)
    if 0x6CD <= code <= 0x7A3:
        selector = data[0x15BDBD4 - IMAGE_BASE + code - 0x6CD]
        return u32(data, 0x15BDB48 + 4 * selector)
    return 0


def decode(data: bytes, start: int, end: int):
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.skipdata = False
    return list(decoder.disasm(data[start - IMAGE_BASE:end - IMAGE_BASE], start))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--code", type=lambda value: int(value, 0))
    parser.add_argument("--stage", choices=("init", "update", "render"))
    parser.add_argument("--size", type=lambda value: int(value, 0), default=0x300)
    parser.add_argument("--full", action="store_true")
    parser.add_argument("--calls", action="store_true",
                        help="show each direct call with its preceding argument window")
    parser.add_argument("--function", action="store_true",
                        help="stop at the next decoded case target")
    parser.add_argument("--quiet", action="store_true")
    parser.add_argument("--xref-string",
                        help="locate an ASCII string and absolute/RVA pointer references")
    parser.add_argument("--xref-u32", type=lambda value: int(value, 0),
                        help="locate raw little-endian 32-bit values")
    parser.add_argument("--callers", type=lambda value: int(value, 0),
                        help="locate direct rel32 calls to an absolute VA")
    parser.add_argument("--xref-action-compare", type=lambda value: int(value, 0),
                        help="find decoded native x86 compares against an action id")
    parser.add_argument("--address", type=lambda value: int(value, 0),
                        help="decode an arbitrary virtual address")
    args = parser.parse_args()

    data = DUMP.read_bytes()
    digest = hashlib.sha256(data).hexdigest().upper()
    if digest != EXPECTED_SHA256:
        raise AssertionError(f"S21 dump hash mismatch: {digest}")

    if args.xref_string:
        needle = args.xref_string.encode("ascii")
        offset = data.find(needle)
        if offset < 0:
            raise ValueError(f"string not found: {args.xref_string}")
        print(f"string offset={offset:#x} va={offset + IMAGE_BASE:#x}")
        for label, value in (("va", offset + IMAGE_BASE), ("rva", offset)):
            pattern = struct.pack("<I", value)
            cursor = 0
            found = []
            while True:
                cursor = data.find(pattern, cursor)
                if cursor < 0:
                    break
                found.append(cursor + IMAGE_BASE)
                cursor += 1
            print(f"{label}-refs=" + ",".join(hex(item) for item in found[:64]))
        return 0
    if args.xref_u32 is not None:
        pattern = struct.pack("<I", args.xref_u32)
        cursor = 0
        found = []
        while True:
            cursor = data.find(pattern, cursor)
            if cursor < 0:
                break
            found.append(cursor + IMAGE_BASE)
            cursor += 1
        print(",".join(hex(item) for item in
                       (found if args.full else found[:256])))
        for item in found:
            offset = item - IMAGE_BASE
            if offset > 0 and data[offset - 1] == 0x68 and offset + 8 < len(data):
                suffix = ""
                if data[offset + 4] == 0x68:
                    pointer = u32(data, item + 4)
                    suffix = f" next-push={pointer:#x}"
                print(f"push-ref={item - 1:#x}{suffix}")
        return 0
    if args.callers is not None:
        cursor = 0
        sites = []
        while True:
            cursor = data.find(b"\xE8", cursor)
            if cursor < 0 or cursor + 5 > len(data):
                break
            displacement = struct.unpack_from("<i", data, cursor + 1)[0]
            if IMAGE_BASE + cursor + 5 + displacement == args.callers:
                sites.append(IMAGE_BASE + cursor)
            cursor += 1
        print(f"target={args.callers:#x} direct-call-sites={len(sites)}")
        for site in sites[:512 if args.full else 128]:
            prior = data[site - IMAGE_BASE - 32:site - IMAGE_BASE]
            action_pushes = [
                value for value in (0xE0, 0xE1, 0xE2, 0xE3, 0xE4)
                if b"\x68" + struct.pack("<I", value) in prior
            ]
            print(f"{site:#x} prior-actions=" +
                  ",".join(hex(value) for value in action_pushes))
        return 0
    if args.xref_action_compare is not None:
        # Search the mapped client code region, then validate each raw
        # immediate as the terminal operand of one complete x86 CMP. A raw
        # four-byte hit alone could be a PUSH, data or instruction tail.
        start_offset = 0x00D00000 - IMAGE_BASE
        end_offset = 0x01800000 - IMAGE_BASE
        needle = struct.pack("<I", args.xref_action_compare)
        decoder = Cs(CS_ARCH_X86, CS_MODE_32)
        decoder.detail = True
        sites = {}
        cursor = data.find(needle, start_offset, end_offset)
        while cursor >= 0:
            for prior in range(max(start_offset, cursor - 10), cursor):
                instructions = list(decoder.disasm(
                    data[prior:cursor + 4], prior + IMAGE_BASE, count=1))
                if not instructions:
                    continue
                instruction = instructions[0]
                if (instruction.mnemonic == "cmp" and
                    instruction.address + instruction.size == cursor + 4 + IMAGE_BASE and
                    instruction.operands and
                    instruction.operands[-1].type == X86_OP_IMM and
                    instruction.operands[-1].imm == args.xref_action_compare):
                    sites[instruction.address] = f"{instruction.mnemonic} {instruction.op_str}"
            cursor = data.find(needle, cursor + 1, end_offset)
        if 0 <= args.xref_action_compare <= 0x7f:
            # Most subtype selectors compile as 83 /7 imm8, so they do not
            # contain the four-byte immediate scanned above.
            prefixes = (
                [bytes((0x83, modrm, 0x14)) for modrm in range(0x78, 0x80)] +
                [bytes((0x83, modrm, 0x14, 0, 0, 0))
                 for modrm in range(0xb8, 0xc0)]
            )
            for prefix in prefixes:
                compact = prefix + bytes((args.xref_action_compare,))
                cursor = data.find(compact, start_offset, end_offset)
                while cursor >= 0:
                    instructions = list(decoder.disasm(
                        data[cursor:cursor + len(compact)],
                        cursor + IMAGE_BASE, count=1))
                    if (instructions and instructions[0].mnemonic == "cmp" and
                        instructions[0].operands[-1].type == X86_OP_IMM and
                        instructions[0].operands[-1].imm == args.xref_action_compare):
                        sites[instructions[0].address] = (
                            f"cmp {instructions[0].op_str}")
                    cursor = data.find(compact, cursor + 1, end_offset)
        print(f"action={args.xref_action_compare:#x} decoded-cmp-sites={len(sites)}")
        for site, instruction in sorted(sites.items()):
            print(f"{site:#x} {instruction}")
        return 0
    if args.address is not None:
        for instruction in decode(data, args.address, args.address + args.size):
            print(
                f"0x{instruction.address:08X}: "
                f"{instruction.mnemonic:<8} {instruction.op_str}"
            )
        return 0

    stages = {
        "init": initializer_target,
        "update": update_target,
        "render": render_target,
    }
    if not args.quiet:
        print("code initializer update renderer")
        for code in EFFECT_CODES:
            print(
                f"0x{code:03X} 0x{initializer_target(data, code):08X} "
                f"0x{update_target(data, code):08X} 0x{render_target(data, code):08X}"
            )

    if args.code is None:
        return 0
    if args.stage is None:
        parser.error("--stage is required with --code")
    start = stages[args.stage](data, args.code)
    if not start:
        raise ValueError(f"no {args.stage} target for {args.code:#x}")
    end = start + args.size
    if args.function:
        targets = sorted({fn(data, code) for code in EFFECT_CODES for fn in
                          (initializer_target, update_target, render_target) if fn(data, code)})
        later = [target for target in targets if target > start]
        if later:
            end = min(end, later[0])
    instructions = decode(data, start, end)
    print(f"\n[{args.stage} code={args.code:#x} target={start:#x}]")
    for index, instruction in enumerate(instructions):
        if args.calls and instruction.mnemonic == "call":
            print("\n-- call window --")
            for prior in instructions[max(0, index - 18):index + 1]:
                print(
                    f"0x{prior.address:08X}: "
                    f"{prior.mnemonic:<8} {prior.op_str}"
                )
            continue
        if args.full or instruction.mnemonic in {
            "call", "cmp", "je", "jne", "jg", "jge", "jl", "jle", "ja",
            "jae", "jb", "jbe", "jmp", "push", "movss", "addss", "subss",
            "mulss", "divss",
        }:
            print(
                f"0x{instruction.address:08X}: "
                f"{instruction.mnemonic:<8} {instruction.op_str}"
            )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
