"""Hash-guarded isolated S21 lance BMD/texture staging; dry-run by default.

No model registration, Item.bmd rewrite, production Data or gameplay launch.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import shutil
from pathlib import Path

from PIL import Image

from convert_s21_bmd_v0f_to_rise_v0c import (
    decrypt_rise_v0c, decrypt_s21_payload, encrypt_rise_v0c,
    inspect_plaintext, parse_s21_container, verify_reference_vector,
)
from inspect_gl_lance_item_chain import MODEL_SHA
from inspect_magic_pin_model_geometry import mesh_bounds

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(r'D:\MU FICA Season 21\Data\Item')
TEXTURE_SOURCE = SOURCE / 'texture'
TARGET = ROOT / 'ExMain_RISE_PC/Tests/GrowLancerBuild/ClassItems/Data/RISE/GrowLancer/ClassItems'
MODELS = {
    13: ('RapideLance.bmd', 'RapideLance.SMD'),
    14: ('ConmocionLance.bmd', 'ConmocionLance.SMD'),
    15: ('PlumaLance.bmd', 'PlumaLance.SMD'),
    16: ('VisLance.bmd', 'VisLance.SMD'),
    17: ('PrickleLance.bmd', 'PrickleLance.SMD'),
    18: ('AlacranLance.bmd', 'AlacranLance.SMD'),
    19: ('bloodangellance01.bmd', 'bloodangellance01.SMD'),
}
TEXTURES = {
    'RapideLance.OZJ': '109514344E94C9D6729588D924B4FFEBF3CF067B6C23A6A45A1CC43EF178DC29',
    'ConmocionLance.OZJ': 'DDF68FD34B05AE5C14E6DCB3407A6B52E47B90B616F989F86C15A2B0986B71CB',
    'PlumaLance.OZJ': 'E09728D583EF83EAFF6F2463934C75737559845BD8DD93A5565B0345DD0E9F4C',
    'gra.ozj': 'E7C6D3F1706D3296AB1FF848570A21232C207D4A03AD5E1AC480EC2381EC1E65',
    'flareBlue.OZJ': 'E261B59F57FDF0C514917A388F02AC66B832C2BE9F97A3F90BC36C98D7B2FB32',
    'VisLance.OZJ': '516BF858F98C94BE4DC5B8CF5A9C4B74148B47F1DB63A40234BFA53874BB3219',
    'flame_chrom.OZJ': 'AB66333A9C6409898067F34D035B2802D2048DDCF8ED7102FC6D9EC1C9A42A46',
    'PrickleLance.OZJ': 'F6E336B4D46478EB4AE88EA573F4BE713345AB33F05BC525FF17D097778E52E2',
    'rgb_mix.OZJ': '35EE2C2BABD3744EF06E1AD97DB7112E580B54341DAAB119C52F34C65B343433',
    'AlacranLance.OZJ': '5F123541A62B4CB898DADE015BF5820567E40ABCCBC2D4B364C32751F116122C',
    'bloodangellance01.OZJ': 'D8275A36F2A8FE88C9A91C177AAE01671561BAF6C420CF8B0C3B5B8BD07432C1',
    'bloodangellance02.OZJ': '4CBB1242C4536504EE453C1E3735507D41717A4CCE647F17892DC015CCD689C7',
}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest().upper()


def checked_destination(relative: str) -> Path:
    output = TARGET / relative
    output.resolve().relative_to(TARGET.resolve())
    for parent in (output, *output.parents):
        if parent.exists() and (parent.is_symlink() or parent.is_junction()):
            raise ValueError(f'reparse candidate path: {parent}')
        if parent == ROOT:
            break
    return output


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--apply', action='store_true')
    apply = parser.parse_args().apply
    verify_reference_vector()
    TARGET.resolve().relative_to(ROOT.resolve())
    planned: list[tuple[Path, bytes, str]] = []
    required_textures: set[str] = set()
    for index, (name, expected_name) in MODELS.items():
        source = SOURCE / name
        raw = source.read_bytes()
        assert sha(raw) == MODEL_SHA[index], name
        plain = decrypt_s21_payload(parse_s21_container(raw))
        info = inspect_plaintext(plain)
        assert info['name'] == expected_name
        meshes, end = mesh_bounds(plain, info['meshCount'])
        assert end < len(plain)
        for mesh in meshes:
            texture = mesh['texture']
            assert texture.lower().endswith('.jpg'), texture
            required_textures.add(texture[:-4].lower())
        converted = encrypt_rise_v0c(plain)
        assert decrypt_rise_v0c(converted) == plain
        output = checked_destination('Models/' + name)
        if output.exists() and output.read_bytes() != converted:
            raise ValueError(f'candidate model conflict: {output}')
        planned.append((output, converted, 'model'))
        print(('STAGED' if apply else 'PLAN'), '3:'+str(index), name,
              'v0F->v0C', sha(converted), 'meshes='+str(info['meshCount']))

    available = {path.stem.lower(): path for path in TEXTURE_SOURCE.iterdir()
                 if path.is_file() and path.suffix.lower() == '.ozj'
                 and path.name in TEXTURES}
    assert required_textures == set(available), (
        'BMD texture names/source OZJ mismatch', required_textures, set(available))
    for filename, digest in TEXTURES.items():
        source = TEXTURE_SOURCE / filename
        raw = source.read_bytes()
        assert sha(raw) == digest
        with Image.open(io.BytesIO(raw[24:])) as image:
            image.load()
            assert image.width > 0 and image.height > 0
            dimensions = image.size
        output = checked_destination('Textures/' + filename)
        if output.exists() and output.read_bytes() != raw:
            raise ValueError(f'candidate texture conflict: {output}')
        planned.append((output, raw, 'texture'))
        print(('STAGED' if apply else 'PLAN'), filename, digest,
              'OZJ JPEG', dimensions)

    if not apply:
        print('DRY RUN: original S21 read-only; no candidate writes')
        return
    for output, data, kind in planned:
        if not output.exists():
            output.parent.mkdir(parents=True, exist_ok=True)
            if kind == 'texture':
                source = TEXTURE_SOURCE / output.name
                assert sha(source.read_bytes()) == sha(data)
                shutil.copy2(source, output)  # preserve original OZJ bytes
            else:
                output.write_bytes(data)  # generated v0C candidate only
        assert output.read_bytes() == data
    print('PASS isolated lance candidates staged with byte-exact BMD plaintext/texture hashes')
    print('OPEN native Open2/bitmap registration, item attributes, GS entitlement, gameplay pixels')


if __name__ == '__main__':
    main()
