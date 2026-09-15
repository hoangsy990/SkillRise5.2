"""Offline AVM1 disassembly, not execution. Adobe SWF v10 action encodings.
Spec: https://www.flashrealtime.com/content/dam/Adobe/en/devnet/swf/pdf/swf_file_format_spec_v10.pdf
"""
import hashlib
import struct
import zlib
import sys
from inspect_wrath_buff_gfx import BASE, PIN

NAMES = {0:'End', 0x17:'Pop', 0x1C:'GetVariable', 0x1D:'SetVariable',
    0x26:'Trace', 0x3C:'DefineLocal', 0x3D:'CallFunction', 0x3E:'Return',
    0x40:'NewObject', 0x41:'DefineLocal2', 0x42:'InitArray', 0x43:'InitObject',
    0x47:'Add2', 0x48:'Less2', 0x49:'Equals2', 0x4C:'PushDuplicate',
    0x4D:'StackSwap', 0x4E:'GetMember', 0x4F:'SetMember', 0x52:'CallMethod',
    0x54:'InstanceOf', 0x60:'BitAnd', 0x66:'StrictEquals', 0x67:'Greater',
    0x4A:'ToNumber', 0x2B:'CastOp', 0x69:'Extends',
    0x87:'StoreRegister', 0x88:'ConstantPool', 0x8E:'DefineFunction2',
    0x96:'Push', 0x99:'Jump', 0x9B:'DefineFunction', 0x9D:'If', 0x12:'Not'}


def decode(data, start, end, pool=(), depth=0):
    assert depth < 20
    pos = start
    while pos < end:
        at = pos
        op = data[pos]; pos += 1
        length = 0
        if op >= 0x80:
            assert pos + 2 <= end
            length = struct.unpack_from('<H', data, pos)[0]; pos += 2
        stop = pos + length
        assert stop <= end
        p = pos
        def take(fmt):
            nonlocal p
            size = struct.calcsize(fmt)
            assert p + size <= stop
            value = struct.unpack_from(fmt, data, p)[0]; p += size
            return value
        def string():
            nonlocal p
            last = data.index(b'\0', p, stop)
            value = data[p:last].decode('utf-8'); p = last + 1
            return value
        detail = ''
        child = None
        if op == 0x88:
            pool = tuple(string() for _ in range(take('<H')))
            detail = repr(pool)
        elif op == 0x96:
            values = []
            while p < stop:
                kind = take('<B')
                if kind == 0: value = string()
                elif kind == 1: value = take('<f')
                elif kind in (2,3): value = ('null' if kind == 2 else 'undefined')
                elif kind == 4: value = 'register:' + str(take('<B'))
                elif kind == 5: value = bool(take('<B'))
                elif kind == 6:
                    high, low = take('<I'), take('<I')
                    value = struct.unpack('<d', struct.pack('<II', low, high))[0]
                elif kind == 7: value = take('<i')
                elif kind in (8,9): value = pool[take('<B' if kind == 8 else '<H')]
                else: raise ValueError(f'unsupported Push type {kind} at {at:x}')
                values.append(value)
            detail = repr(values)
        elif op in (0x8E, 0x9B):
            name = string(); count = take('<H')
            registers = take('<B') if op == 0x8E else 0
            flags = take('<H') if op == 0x8E else 0
            params = [(take('<B'), string()) if op == 0x8E else string() for _ in range(count)]
            size = take('<H')
            assert p == stop and stop + size <= end
            child = (stop, stop + size)
            detail = repr(dict(name=name, registers=registers, flags=hex(flags), params=params, size=size))
        elif op in (0x99, 0x9D):
            detail = hex(stop + take('<h'))
            assert start <= int(detail,16) <= end
        elif op == 0x87:
            detail = str(take('<B'))
        else:
            detail = data[p:stop].hex(); p = stop
        assert p == stop
        print('  '*depth + f'{at:06x} {NAMES.get(op, hex(op))} {detail}')
        if child:
            decode(data, child[0], child[1], pool, depth + 1)
            pos = child[1]
        else:
            pos = stop
        if op == 0:
            assert pos == end
    assert pos == end


if __name__ == '__main__':
    sys.stdout.reconfigure(errors='backslashreplace')
    use_base = '--base' in sys.argv[1:]
    use_image = '--image' in sys.argv[1:]
    filename = 'base_input/base.gfx' if use_base else 'BuffList.gfx'
    expected = '632a88ae5034de482eff9968645b14c2f8a25f9dd23c9c1783ca321db01faf28' if use_base else PIN
    if use_image:
        filename = 'image_input/ImageData.gfx'
        expected = 'c524ff93eea8cc47bfb8b0765ad28182f746998a4b09d1ef6e7ba1d7ed064433'
    packed = (BASE/filename).read_bytes()
    assert hashlib.sha256(packed).hexdigest() == expected
    data = packed[:8] + zlib.decompress(packed[8:])
    assert len(data) == struct.unpack_from('<I', packed, 4)[0]
    if use_image:
        decode(data, 0x333C, 0x333C+182)
    elif use_base:
        decode(data, 0xADD, 0xADD+254)
    else:
        sprite = int(sys.argv[1]) if len(sys.argv) > 1 else 117
        offset, size = {117:(0x2C797,3090),76:(0xE66D,2056)}[sprite]
        assert struct.unpack_from('<H', data, offset)[0] == sprite
        decode(data, offset+2, offset+size)
