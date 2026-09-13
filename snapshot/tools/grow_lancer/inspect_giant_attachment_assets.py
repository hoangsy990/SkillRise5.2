"""Read-only Giant configuration skeleton inspection, not native load/visual QA."""
import hashlib
import json
import struct
from pathlib import Path
import compare_player_actions as clips
import merge_player_actions as skeletons
from disasm_s21_window import DUMP, IMAGE_BASE
from io import BytesIO
from PIL import Image

ROOT = Path(r'D:\MU FICA Season 21\Data\Skill')
PINS = {
    'Giant_DarkWizard_01': '710EF1764CE5EB6E19C618CA6FA6612A216B484E63F72BE0F7BE26E7440C4949',
    'Giant_Elf_01': '7ED8A4797796AA86568E87EDE260EFC6303A9100863C3484AFE5E8B0BB35EF9D',
    'Giant_GrowLancer_01': '3B66BD850CF0BF2D22A44472ED1C3F57F744AA8919378E32653B45BE685E202B',
}

def main():
    binary = DUMP.read_bytes()
    assert hashlib.sha256(binary).hexdigest() == '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
    extra = (
        (0x84E8, 0x1BAD1BC, 'Giant_darkW_Neon_render', '429BAC4FBAE02A0A62A8F67239FB1B4E1FAFB0BC239CFDAD5C51DB90E228D38C', 128),
        (0x84E9, 0x1BAD204, 'Giant_Elf_Neon_r', '7E6AA3D1A6890D5C32D7BBC1D52458C20C2B96842ADBA6E9468F684EFF4A416D', 128),
        (0x84EA, 0x1BAD248, 'Giant_GrowLancer_Neon_render', '3086D76FE112E60E58F9E61D02E8FA3568E4D96B64E91AF6A573CA76CA74A1FB', 128),
        (0x84EB, 0x1BAD270, 'Giant_GrowLancer_Jewel_render', '9C2B8B224EF638BBE74AB8F4702540AD1A185B5DC39767126F37775497C56776', 32),
    )
    for resource, address, stem, digest, size in extra:
        assert binary[address-IMAGE_BASE:].split(b'\0', 1)[0] == ('Skill\\' + stem + '.jpg').encode('ascii')
        path = ROOT / (stem + '.OZJ')
        raw = path.read_bytes()
        assert hashlib.sha256(raw).hexdigest().upper() == digest
        with Image.open(BytesIO(raw[24:])) as decoded:
            decoded.load()
            assert decoded.format == 'JPEG' and decoded.size == (size, size)
        print(json.dumps(dict(extraResource=hex(resource), candidateFile=str(path), sha256=digest,
                             dimensions=[size, size], scope='registered logical name and physical decode; runtime binding unverified')))
    converter = clips.load_converter()
    for name in ('Giant_DarkWizard_01', 'Giant_Elf_01', 'Giant_GrowLancer_01'):
        path = ROOT / (name + '.bmd')
        raw = path.read_bytes()
        assert hashlib.sha256(raw).hexdigest().upper() == PINS[name], name
        payload, version = clips.decode_container(path, converter)
        meshes, bones, actions = struct.unpack_from('<3h', payload, 32)
        assert 0 < meshes <= 50 and 0 < bones <= 200 and 0 < actions <= 4096
        offset = 38
        textures = []
        # Native Open2 disk records: Vertex16, Normal20, UV8, Triangle_t2 64.
        for _ in range(meshes):
            header, offset = skeletons.take(payload, offset, 10)
            vertices, normals, uvs, triangles, texture = struct.unpack('<5h', header)
            assert min(vertices, normals, uvs, triangles, texture) >= 0
            _, offset = skeletons.take(payload, offset,
                vertices * 16 + normals * 20 + uvs * 8 + triangles * 64)
            filename, offset = skeletons.take(payload, offset, 32)
            textures.append(filename.split(b'\0', 1)[0].decode('ascii'))
        # Reuse the strict animation/bone parser on an in-memory zero-mesh view.
        view = payload[:32] + struct.pack('<3h', 0, bones, actions) + payload[offset:]
        parsed = skeletons.parse(view)
        seats = []
        for index, header in enumerate(parsed.bone_headers):
            if header[0] == 0 and header[1:33].split(b'\0', 1)[0] == b'sit_pos_0':
                seats.append(dict(index=index, parent=struct.unpack_from('<h', header, 33)[0]))
        assert len(seats) == 1, (name, seats)
        print(json.dumps(dict(file=str(path), sha256=hashlib.sha256(raw).hexdigest().upper(),
            version=version, meshes=meshes, bones=bones, actions=actions,
            seat=seats[0], textures=textures)))
    print('PASS structural parse and unique seat bone; no conversion, staging or native load')

if __name__ == '__main__':
    main()
