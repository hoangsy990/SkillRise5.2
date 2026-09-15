"""Bounded offline GFx tag inventory; symbol names alone are not icon mapping."""
from pathlib import Path
import hashlib
import struct
import zlib
import json
import re
import sys

ROOT = Path(__file__).resolve().parents[2]
BASE = ROOT / 'artifacts/grow_lancer/buff_gfx_decode_20260913/Decrypted'
PIN = '934f8fa696d2b9beada80cf1d8f7bdad337c42d8fa5da4a1364befe0a8595fe1'


def inspect(use_base=False, use_image=False):
    expected = '632a88ae5034de482eff9968645b14c2f8a25f9dd23c9c1783ca321db01faf28' if use_base else PIN
    filename = 'base_input/base.gfx' if use_base else 'BuffList.gfx'
    if use_image:
        expected = 'c524ff93eea8cc47bfb8b0765ad28182f746998a4b09d1ef6e7ba1d7ed064433'
        filename = 'image_input/ImageData.gfx'
    packed = (BASE / filename).read_bytes()
    assert hashlib.sha256(packed).hexdigest() == expected
    assert packed[:3] == b'CFX'
    length = struct.unpack_from('<I', packed, 4)[0]
    assert 8 <= length <= 1024 * 1024
    decoder = zlib.decompressobj()
    body = decoder.decompress(packed[8:], length - 8 + 1)
    assert decoder.eof and not decoder.unused_data and len(body) == length - 8
    data = packed[:8] + body
    nbits = data[8] >> 3
    start = 8 + (5 + 4 * nbits + 7) // 8 + 4
    results = []
    def tags(start, end, owner, depth=0):
        assert depth < 16
        offset = start
        while offset < end:
            assert offset + 2 <= end
            header = struct.unpack_from('<H', data, offset)[0]
            code, size = header >> 6, header & 63
            offset += 2
            if size == 63:
                assert offset + 4 <= end
                size = struct.unpack_from('<I', data, offset)[0]
                offset += 4
            assert offset + size <= end
            payload = data[offset:offset + size]
            if code in (56, 57, 71):
                pos = 0
                def text():
                    nonlocal pos
                    stop = payload.index(b'\0', pos)
                    value = payload[pos:stop].decode('utf-8')
                    pos = stop + 1
                    return value
                url = text() if code != 56 else None
                if code == 71:
                    assert payload[pos:pos+2] == b'\x01\x00'
                    pos += 2
                count = struct.unpack_from('<H', payload, pos)[0]
                pos += 2
                symbols = []
                for _ in range(count):
                    symbol = struct.unpack_from('<H', payload, pos)[0]
                    pos += 2
                    symbols.append([symbol, text()])
                assert pos == len(payload)
                results.append(dict(owner=owner, tag=code, url=url, symbols=symbols))
            elif code == 39:
                assert size >= 4
                sprite = struct.unpack_from('<H', payload)[0]
                tags(offset + 4, offset + size, sprite, depth + 1)
            elif code >= 1000:
                item = dict(owner=owner, tag=code, offset=hex(offset), size=size, raw=payload.hex())
                # GFx schema cross-checked against JPEXS DefineSubImage /
                # DefineExternalImage2 readData (reference only, no code copied).
                if code == 1008:
                    assert size == 12
                    item.update(zip(('characterId','imageId','x1','y1','x2','y2'),
                                    struct.unpack('<6H', payload)))
                    assert item['x1'] < item['x2'] and item['y1'] < item['y2']
                elif code == 1009:
                    assert size >= 12
                    item.update(zip(('imageId','idType','bitmapFormat','targetWidth','targetHeight'),
                                    struct.unpack_from('<5H', payload)))
                    p = 10
                    for field in ('exportName','fileName'):
                        n = payload[p]; p += 1
                        assert n < 128 and p + n <= size, 'only observed short NetStrings supported'
                        item[field] = payload[p:p+n].decode('utf-8'); p += n
                    item['extra'] = payload[p:].hex()
                results.append(item)
            elif code in (12, 59):
                matches = [s.decode('ascii') for s in re.findall(rb'[\x20-\x7e]{4,}', payload)
                           if any(x in s for x in (b'DrawIcon_Buff', b'BuffIcon', b'icon_', b'BuffType', b'getBitmap'))]
                if matches:
                    results.append(dict(owner=owner, tag=code, offset=hex(offset), size=size,
                        sprite=struct.unpack_from('<H', payload)[0] if code == 59 else None,
                        stringLeads=matches))
            offset += size
            if code == 0:
                assert size == 0 and offset == end
                return
        raise ValueError('missing End tag')
    tags(start, len(data), 'root')
    if use_image:
        exports = {name: symbol for row in results if row['tag'] == 56
                   for symbol, name in row['symbols']}
        assert exports['icon_424'] == 73 and exports['icon_425'] == 72
        for symbol, coords in ((73,(220,120,240,148)), (72,(242,120,262,148))):
            matches = [row for row in results if row['tag'] == 1008 and row['characterId'] == symbol]
            assert len(matches) == 1 and matches[0]['imageId'] == 0
            assert tuple(matches[0][k] for k in ('x1','y1','x2','y2')) == coords
        atlas = [row for row in results if row['tag'] == 1009 and row['imageId'] == 0]
        assert len(atlas) == 1
        assert (atlas[0]['fileName'],atlas[0]['targetWidth'],atlas[0]['targetHeight']) == ('ImageData_I7.dds',1024,256)
    print(json.dumps(dict(sha256=expected, uncompressedSize=length, tags=results), indent=2))


if __name__ == '__main__':
    inspect('--base' in sys.argv[1:], '--image' in sys.argv[1:])
