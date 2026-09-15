"""Read-only original asset provenance and JPEG payload decoding; not engine load QA."""
from pathlib import Path
import hashlib
import io
from PIL import Image

source = Path(r'D:\MU FICA Season 21\Data\Effect')
assets = [
    ('firehik_mono01.OZJ',0x8086,16340,'c3e18c474ba5ae9f06b9e7159afc0dab34869b78eab425cb5538b94fe0e7138f'),
    ('firehik_mono02.OZJ',0x8087,17160,'ea67813627c32a9f9eae948ffff2ee47a0fd44a49426cce2879fcef4a6b5f7d6'),
    ('firehik_mono03.OZJ',0x8089,16049,'cf6ba21108507e377c40145912ec3db7fbbd19031ac8aeaf129f5a6773fff315'),
]
for filename,resource,length,digest in assets:
    data = (source / filename).read_bytes()
    assert len(data) == length and hashlib.sha256(data).hexdigest() == digest
    # Native OZJ loaders skip the 24-byte dump prefix, even if it starts FF D8.
    with Image.open(io.BytesIO(data[24:])) as im:
        im.load()
        assert im.size == (64,64) and im.mode == 'RGB'
    print(f'PASS {resource:X} {filename}: SHA256 pinned; offset24 JPEG decoded 64x64 RGB')
print('OPEN: S21 live metadata/blend, native registration/staging and engine rendering acceptance.')
