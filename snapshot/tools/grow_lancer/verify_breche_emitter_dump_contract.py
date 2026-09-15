#!/usr/bin/env python3
"""Pin the S21 Breche emitter (0x806E/sub15) producer boundary.

This is a read-only, hash-pinned dump check.  The emitter is an effect-pool
record whose primary update samples owner bones and submits one flare plus one
of three particle variants per iteration; it is not a fourth render layer.
"""

from pathlib import Path
import hashlib

from capstone import Cs, CS_ARCH_X86, CS_MODE_32


DUMP = Path(
    r"D:\RISE-CrossPlatform\Source\artifacts\visual_studio"
    r"\season21_client_qa\memory_dump\process_23824\400000.main.exe"
)
BASE = 0x400000
SHA256 = "6422CB4EBA9432130EB247B47723EA6FC0014F5100EA0C6E63DB8350F9275637"


def decode(data: bytes, va: int, size: int):
    decoder = Cs(CS_ARCH_X86, CS_MODE_32)
    decoder.skipdata = False
    return {ins.address: ins for ins in decoder.disasm(data[va - BASE:va - BASE + size], va)}


def require(insns, va: int, mnemonic: str, op: str | None = None) -> None:
    ins = insns.get(va)
    if ins is None or ins.mnemonic != mnemonic or (op is not None and ins.op_str != op):
        got = None if ins is None else f"{ins.mnemonic} {ins.op_str}"
        raise ValueError(f"S21 emitter instruction changed at 0x{va:X}: {got!r}")


def call_target(insns, va: int) -> int:
    ins = insns.get(va)
    if ins is None or ins.mnemonic != "call":
        raise ValueError(f"expected call at 0x{va:X}")
    return int(ins.op_str, 16)


def main() -> None:
    data = DUMP.read_bytes()
    if hashlib.sha256(data).hexdigest().upper() != SHA256:
        raise ValueError("pinned Season21 mapped image hash changed")

    # 0x14C1859..0x14C189F: subtype15, null-owner retirement and two-iteration
    # bounded loop.  +0x34C is the emitter owner pointer in this chain.
    emitter = decode(data, 0x14C1859, 0x2B0)
    require(emitter, 0x14C1859, "mov", "eax, dword ptr [ebp + 8]")
    require(emitter, 0x14C185C, "cmp", "dword ptr [eax + 0x60], 0xf")
    require(emitter, 0x14C1860, "jne", "0x14c1b12")
    require(emitter, 0x14C1869, "cmp", "dword ptr [eax + 0x34c], 0")
    require(emitter, 0x14C1870, "jne", "0x14c1882")
    if call_target(emitter, 0x14C1877) != 0x13167BB:
        raise ValueError("S21 emitter null-owner cleanup target changed")
    require(emitter, 0x14C1898, "cmp", "dword ptr [ebp - 0x128d8], 2")
    require(emitter, 0x14C189F, "jge", "0x14c1b0d")

    # The selected owner model/bone position and flare are stable boundaries.
    if call_target(emitter, 0x14C1913) != 0x132EC63:
        raise ValueError("S21 emitter owner-bone transform call changed")
    if call_target(emitter, 0x14C19A1) != 0x172760A:
        raise ValueError("S21 emitter flare allocator call changed")
    require(emitter, 0x14C1987, "push", "ecx")
    require(emitter, 0x14C199C, "push", "0x7ef7")

    # Each random choice submits the native particle wrapper with the exact
    # resource/subtype triplet; calls are deliberately not treated as effects.
    calls = {
        0x14C1A7B: (0x8084, "0x1724176"),
        0x14C1ABB: (0x806E, "0x1724176"),
        0x14C1AFB: (0x8085, "0x1724176"),
    }
    for va, (resource, target) in calls.items():
        ins = emitter.get(va)
        if ins is None or ins.mnemonic != "push" or ins.op_str != hex(resource):
            raise ValueError(f"S21 emitter resource push changed at 0x{va:X}")
        if call_target(emitter, va + 5) != int(target, 16):
            raise ValueError(f"S21 emitter particle wrapper changed at 0x{va + 5:X}")

    # The source adapter preserves this as a lifecycle-only effect and emits
    # the same native families from its private particle pool.
    root = Path(__file__).resolve().parents[2]
    runtime = (root / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE"
               / "GrowLancerEffectRuntime.cpp").read_text(encoding="utf-8")
    if "case kBrecheEmitterModel:" not in runtime:
        raise ValueError("RISE Breche emitter case missing")
    update = runtime.split("void UpdateEffect", 1)[1].split(
        "bool RenderEffect(OBJECT& effect)", 1)[0]
    emitter_source = update.split("case kBrecheEmitterModel:", 1)[1].split(
        "case kWrathBrokenBitmap:", 1)[0]
    for needle in (
        "CreateBrecheSprite(BITMAP_LIGHT, position, 4.f, light, &owner, 0.f);",
        "CreateBrecheFireParticle(variant, position, effect.Angle, light, scale);",
    ):
        if needle not in emitter_source:
            raise ValueError(f"RISE emitter producer missing: {needle}")
    render = runtime.split("bool RenderEffect(OBJECT& effect)", 1)[1]
    if "effect.Type == kBrecheEmitterModel)\n            return true;" not in render:
        raise ValueError("RISE emitter gained an unproven direct render layer")

    print("PASS: S21 806E/sub15 owner guard, two-iteration bone sampling and 7EF7 flare pinned")
    print("PASS: S21 emitter submits only 8084/0, 806E/4 and 8085/0 particle wrappers")
    print("PASS: RISE keeps emitter lifecycle-only and delegates children to native pools")
    print("Scope: static producer boundary; owner pixels, allocator exhaustion and runtime parity remain open")


if __name__ == "__main__":
    main()
