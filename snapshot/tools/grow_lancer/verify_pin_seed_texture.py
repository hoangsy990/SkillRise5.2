"""Read-only source texel/UV evidence, not GPU raster acceptance."""
from pathlib import Path
import hashlib
import io
import math
from PIL import Image

source = Path(r'D:\MU FICA Season 21\Data\Effect\pin_lights.OZJ')
raw = source.read_bytes()
assert hashlib.sha256(raw).hexdigest() == 'a8c4db1921178c7a4905f0398a744b49ab8d283de36051584401566ce3fb2288'
with Image.open(io.BytesIO(raw[24:])) as texture:
    texture.load()
    assert texture.size == (16,128) and texture.mode == 'RGB'
    maxima = [max(pair[1] for pair in texture.crop((x,0,x+1,128)).getextrema()) for x in range(16)]
assert maxima == [1,1,1,1,1,1,13,152,212,152,13,1,1,1,1,1]
for tick in (5,8,9,10,17):
    # Seed is index tick, so its adjoining last segment is j=tick-1.
    lo, hi = 1-tick/17, 1-(tick-1)/17
    left = max(0,math.floor(lo*16-.5))
    right = min(15,math.ceil(hi*16-.5))
    peak = max(maxima[left:right+1])
    print(f'tick {tick}: U=[{lo:.6f},{hi:.6f}], linear-neighbor columns {left}..{right}, peak={peak}')
    if tick in (8,9,10): assert peak >= 152
print('PASS seed-adjoining segment crosses bright texture columns while seed still retained')
print('OPEN raster area, filtering/mips and gameplay parity; do not infer invisible seed from edge darkness')
