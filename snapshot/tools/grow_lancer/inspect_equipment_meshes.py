"""Bounds-checked offline BMD layout check; does not prove native GPU loading."""
import struct
import json
import subprocess
import sys
from pathlib import Path
from convert_s21_bmd_v0f_to_rise_v0c import (
    parse_s21_container, decrypt_s21_payload, inspect_plaintext,
)


def inspect(payload):
    result = inspect_plaintext(payload)
    offset = 38

    def take(size):
        nonlocal offset
        if size < 0 or offset + size > len(payload):
            raise ValueError('Truncated or negative BMD field at %d' % offset)
        value = payload[offset:offset+size]
        offset += size
        return value

    meshes = []
    used_bones = set()
    for index in range(result['meshCount']):
        nv, nn, nt, nf, texture = struct.unpack('<5h', take(10))
        if min(nv, nn, nt, nf) < 0:
            raise ValueError('Negative mesh count')
        # Native ZzzBMD.h default Win32 layout, consumed by BMD::Open2.
        vertices, normals = take(nv*16), take(nn*20)
        take(nt*8)
        for records, size in ((vertices, 16), (normals, 20)):
            for start in range(0, len(records), size):
                bone = struct.unpack_from('<h', records, start)[0]
                if bone < -1 or bone >= result['boneCount']:
                    raise ValueError('Bone reference out of range')
                if bone >= 0:
                    used_bones.add(bone)
        for _ in range(nf):
            triangle = take(64)
            if triangle[0] != 3:
                raise ValueError('Non-triangle polygon')
            for start, limit in ((2, nv), (10, nn), (18, nt)):
                if any(v < 0 or v >= limit for v in struct.unpack_from('<3h', triangle, start)):
                    raise ValueError('Triangle reference out of range')
        name = take(32).split(b'\0', 1)[0].decode('ascii')
        meshes.append(dict(index=index, vertices=nv, triangles=nf,
                           textureIndex=texture, texture=name))
    keys = []
    for _ in range(result['actionCount']):
        count, lock = struct.unpack('<hB', take(3))
        if count < 0 or lock not in (0, 1):
            raise ValueError('Invalid action header')
        keys.append(count)
        if lock:
            take(count*12)
    bones = []
    for bone_index in range(result['boneCount']):
        dummy = take(1)[0]
        if dummy not in (0, 1):
            raise ValueError('Invalid dummy flag')
        if not dummy:
            header = take(34)
            parent = struct.unpack_from('<h', header, 32)[0]
            if parent < -1 or parent >= result['boneCount'] or parent == bone_index:
                raise ValueError('Invalid bone parent')
            bones.append(dict(name=header[:32].split(b'\0', 1)[0].decode('ascii'),
                              parent=parent, dummy=False))
            take(sum(keys)*24)
        else:
            bones.append(dict(dummy=True))
    trailing = payload[offset:]
    # S21 loader 0x132C447 exits the bone loop to 0x132C70C and frees
    # the decoded buffer, without testing the remaining bytes. Keep an
    # offline conservative one-block limit; it is NOT a S21 size restriction.
    # Some observed aligned payloads carry a complete extra 16-byte block.
    if len(trailing) > 16:
        raise ValueError('Unexpected trailing payload: parsed=%d length=%d tail=%s' %
                         (offset, len(payload), trailing[:32].hex()))
    return dict(**result, meshes=meshes, bones=bones, usedBones=sorted(used_bones), parsedBytes=offset,
                paddingBytes=len(trailing), paddingHex=trailing.hex(),
                tailPolicy='ignored by S21 loader; offline limit 16 bytes',
                paddingContentsVerified=False,
                scope='offline layout only; tail contents and GPU loading not verified')


def main():
    report = json.loads(subprocess.check_output(
        [sys.executable, str(Path(__file__).with_name('inspect_breche_item_models.py'))]))
    failures = 0
    for source in sorted({row['source'] for row in report['models']}):
        try:
            result = inspect(decrypt_s21_payload(parse_s21_container(Path(source).read_bytes())))
        except ValueError as error:
            failures += 1
            result = dict(status='UNVERIFIED', error=str(error))
        print(json.dumps(dict(source=source, **result)), flush=True)
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
