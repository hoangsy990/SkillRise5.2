"""Read-only, narrowly scoped renderer-table inspection; never dump config secrets.

Source: 6C0FED -> 64266E loads Pegasus.bmd at 106B9D8+4, size185F4C.
6426F8..642713 decodes each byte in place. 5291DC ->493D99 consumes
100 records at1182914, stride1C, stopping on unsigned index >99.
"""
import hashlib
import pathlib
import struct
import sys

if len(sys.argv) != 3:
    raise SystemExit("usage: SCRIPT PINNED_EXE Pegasus.bmd")
exe = pathlib.Path(sys.argv[1]).read_bytes()
if hashlib.sha256(exe).hexdigest().upper() != (
        "02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA"):
    raise SystemExit("executable hash mismatch")
raw = pathlib.Path(sys.argv[2]).read_bytes()
if len(raw) != 0x185F4C:
    raise SystemExit("config size mismatch")
decoded = bytes((((byte - (((i >> 8) & 255) ^ 0x95)) & 255)
                 ^ (i & 255) ^ 0xCA) for i, byte in enumerate(raw))
print("config_sha256=" + hashlib.sha256(raw).hexdigest().upper())
base = 0x106B9D8 + 4
offset = 0x1182914 - base
models = []
for i in range(100):
    index, model = struct.unpack_from("<II", decoded, offset + i * 0x1C)
    if index > 99:
        print(f"renderer_table_stop={i} sentinel={index:#x}")
        break
    models.append(model)
    print(f"renderer_record={i} index={index} model={model:#x}")
print(f"renderer_count={len(models)}")
print(f"wrath_direct_matches={models.count(0x27B3)}")
print(f"wrath_plus285_matches={sum(((model + 0x285) & 0xFFFFFFFF) == 0x27B3 for model in models)}")

# 494322..49438E copies at most300 records (stride60), uses field+4 as
# map key, and stops when unsigned field0 >299. Print only model match
# metadata; no strings or unrelated configuration bytes are disclosed.
count = 0
matches = []
for i in range(300):
    record_offset = 0x1128D58 - base + i * 0x60
    index, model = struct.unpack_from("<II", decoded, record_offset)
    if index > 299:
        print(f"classifier_stop={i} sentinel={index:#x}")
        break
    count += 1
    print(f"classifier_record={i} index={index} model_key={model:#x}")
    if model in (0x27B3, 0x27B3 - 0x493):
        matches.append((i, model))
print(f"classifier_count={count} wrath_key_matches={matches}")
