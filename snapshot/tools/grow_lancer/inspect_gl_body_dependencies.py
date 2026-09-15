"""Read-only mesh texture inspection using the native serialized mesh strides.

Not a native BMD::Open execution, skeleton validation, or texture load proof.
"""
from pathlib import Path
import hashlib
import struct
from convert_s21_bmd_v0f_to_rise_v0c import (
    parse_s21_container, decrypt_s21_payload, decrypt_rise_v0c,
    inspect_plaintext, verify_reference_vector)

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(r'D:\MU FICA Season 21\Data\Player')


def inspect(data):
    identity = inspect_plaintext(data)
    offset = 38
    textures = []
    nodes = set()
    for index in range(identity['meshCount']):
        if offset + 10 > len(data):
            raise ValueError('truncated mesh header')
        nv, nn, nt, nf, texture = struct.unpack_from('<5h', data, offset)
        if min(nv, nn, nt, nf, texture) < 0:
            raise ValueError('negative mesh count/index')
        # ZzzBMD::Open uses sizeof(Vertex_t/Normal_t/TexCoord_t/Triangle_t2).
        records = offset + 10
        offset = records + nv*16 + nn*20 + nt*8 + nf*64
        if offset + 32 > len(data):
            raise ValueError('truncated mesh body/texture')
        for start, count, stride in ((records, nv, 16), (records+nv*16, nn, 20)):
            for record in range(count):
                node, = struct.unpack_from('<h', data, start+record*stride)
                if node < 0 or node >= identity['boneCount']:
                    raise ValueError('vertex/normal bone index out of range')
                nodes.add(node)
        raw = data[offset:offset+32]
        if b'\0' not in raw:
            raise ValueError('unterminated texture name')
        name = raw.split(b'\0', 1)[0].decode('ascii')
        if not name or Path(name).name != name or '\\' in name:
            raise ValueError('unsupported texture path')
        textures.append((index, texture, name))
        offset += 32
    identity['usedNodes'] = sorted(nodes)
    return identity, textures, offset


def inspect_skeleton(data, identity, offset):
    def take(size):
        nonlocal offset
        if size < 0 or offset + size > len(data):
            raise ValueError('truncated skeleton')
        result = data[offset:offset+size]
        offset += size
        return result

    keys = []
    for _ in range(identity['actionCount']):
        count, locked = struct.unpack('<hB', take(3))
        if count < 0 or locked not in (0, 1):
            raise ValueError('invalid action record')
        keys.append(count)
        if locked:
            take(count*12)
    bones = []
    for _ in range(identity['boneCount']):
        dummy = take(1)[0]
        if dummy not in (0, 1):
            raise ValueError('invalid dummy flag')
        if dummy:
            bones.append(None)
            continue
        name = take(32).split(b'\0', 1)[0].decode('ascii')
        parent, = struct.unpack('<h', take(2))
        if parent < -1 or parent >= identity['boneCount']:
            raise ValueError('invalid parent index')
        bones.append((name, parent))
        take(sum(keys)*24)
    return keys, bones, data[offset:]


def main():
    verify_reference_vector()
    player = ROOT/'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/Player/player.bmd'
    player_data = player.read_bytes()
    assert hashlib.sha256(player_data).hexdigest() == '0cc3d22d5bbd426128e6bffe9c3766585f9f28bbda5b3dd50122ec7ad6b9ca63'
    player_plain = decrypt_rise_v0c(player_data)
    player_identity = inspect_plaintext(player_plain)
    assert player_identity['meshCount'] == 0
    _, player_bones, player_tail = inspect_skeleton(player_plain, player_identity, 38)
    assert not any(player_tail)
    # Pins previously observed source bytes; refuse silently changing input.
    contract = (ROOT/'GrowLancer/CLASS_INTEGRATION_CONTRACT.md').read_text(encoding='utf-8')
    for tier in ('', '3'):
        for stem in ('Helm', 'Armor', 'Pant', 'Glove', 'Boot'):
            name = stem+'Class'+tier+'08.bmd'
            prefix = '| '+name+' | '
            expected = contract.split(prefix, 1)[1].split(' |', 1)[0]
            source = (SOURCE/name).read_bytes()
            assert hashlib.sha256(source).hexdigest() == expected
            plaintext = decrypt_s21_payload(parse_s21_container(source))
            identity, textures, end = inspect(plaintext)
            keys, bones, tail = inspect_skeleton(plaintext, identity, end)
            mismatches = [(i, bone, player_bones[i] if i < len(player_bones) else None)
                          for i, bone in enumerate(bones)
                          if i >= len(player_bones) or bone != player_bones[i]]
            print('  boneHeaderMismatchesAgainstPinnedQAPlayer=', mismatches)
            assert not mismatches, 'body bone headers differ from pinned Player prefix'
            assert all(player_bones[node] is not None and bones[node] is not None
                       for node in identity['usedNodes']), 'mesh references dummy bone'
            print('  keys=', keys, 'nonDummy=', sum(b is not None for b in bones),
                  'trailingBytes=', len(tail), 'tailHex=', tail.hex())
            print(name, identity, 'meshEnd='+str(end))
            for mesh, slot, texture in textures:
                suffix = Path(texture).suffix.lower()
                extension = {'.jpg': '.OZJ', '.tga': '.OZT'}.get(suffix)
                candidate = SOURCE/(Path(texture).stem+extension) if extension else None
                status = 'UNRESOLVED'
                if candidate and candidate.is_file():
                    status = str(candidate)+' sha256='+hashlib.sha256(candidate.read_bytes()).hexdigest()
                print(f'  mesh={mesh} textureSlot={slot} name={texture} candidate={status}')
    print('OPEN nonzero trailing bytes, bone binding, native struct ABI/load, texture resolver/render and staging')


if __name__ == '__main__':
    main()
