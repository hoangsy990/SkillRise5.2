"""Lossless decoded DDS -> native OZT serialization; isolated destination only."""
from pathlib import Path
import hashlib
import struct
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
assert ROOT == Path(r'D:\RISE-CrossPlatform\Source\_PC_GrowLancer')
source = ROOT/'artifacts/grow_lancer/buff_gfx_decode_20260913/Decrypted/atlas_input/ImageData_I7.dds'
assert hashlib.sha256(source.read_bytes()).hexdigest() == 'd00e868be46e73623c80afcb0661181816d661ad0d1586f5eb6e00198348baf8'
with Image.open(source) as image:
    image.load()
    assert image.size == (1024,256) and image.mode == 'RGBA'
    rgba = image.tobytes()
    # Native OpenTga reads dimensions at16,18 and bottom-up BGRA pixels at22.
    header = bytearray(22)
    header[6] = 2  # TGA uncompressed truecolor after four-byte OZT prefix.
    struct.pack_into('<HHBB', header, 16, 1024,256,32,8)
    bgra = image.tobytes('raw','BGRA',0,-1)
    output = bytes(header) + bgra
    restored = Image.frombytes('RGBA',(1024,256),output[22:],'raw','BGRA',0,-1)
    assert restored.tobytes() == rgba, 'native channel/row transform mismatch'
destination = ROOT/'Client/Data/RISE/GrowLancer/Wrath/WrathBuffAtlas.OZT'
destination.parent.mkdir(parents=True,exist_ok=True)
if destination.exists():
    assert destination.read_bytes() == output, 'refuse differing existing atlas'
else:
    with destination.open('xb') as stream:
        stream.write(output)
print('PASS exact RGBA native-load roundtrip; OZT SHA256=' + hashlib.sha256(output).hexdigest())
print(destination)
