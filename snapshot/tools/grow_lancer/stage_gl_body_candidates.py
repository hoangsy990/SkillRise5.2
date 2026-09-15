"""Hash-guarded isolated BMD conversion. Dry run default; no runtime deployment."""
import argparse
import hashlib
from pathlib import Path
import subprocess
import sys
import shutil
import struct
import io
from PIL import Image
from convert_s21_bmd_v0f_to_rise_v0c import (
    parse_s21_container, decrypt_s21_payload, inspect_plaintext,
    encrypt_rise_v0c, decrypt_rise_v0c, verify_reference_vector)
from inspect_gl_body_dependencies import inspect, inspect_skeleton

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(r'D:\MU FICA Season 21\Data\Player')
TARGET = ROOT/'ExMain_RISE_PC/Tests/GrowLancerBuild/BodyCandidates/Data/RISE/GrowLancer/ClassBody'


def checked_output(name):
    output = TARGET/name
    if output.is_symlink() or output.is_junction():
        raise ValueError(f'reparse output: {output}')
    output.resolve().relative_to(TARGET.resolve())
    return output


def main():
    args = argparse.ArgumentParser()
    args.add_argument('--apply', action='store_true')
    apply = args.parse_args().apply
    verify_reference_vector()
    TARGET.resolve().relative_to(ROOT.resolve())
    for path in (TARGET, *TARGET.parents):
        if path.exists() and (path.is_symlink() or path.is_junction()):
            raise ValueError(f'reparse target ancestor: {path}')
        if path == ROOT:
            break
    contract = (ROOT/'GrowLancer/CLASS_INTEGRATION_CONTRACT.md').read_text(encoding='utf-8')
    plan = []
    texture_plan = []
    for name, expected in (
        ('hair_Class108.OZT', 'e13026b066e69982cf8cddc0f1a8d9558fda0673b2a52b82f56f920614ea4a7b'),
        ('skin_Class108.OZJ', '37d230ab5e35c76220b54df28abb52f619e1e86f9a4cb9a96c77b8c54c344a89'),
        ('Skin_Class308.OZJ', 'cb264c223a1e6691ccb7caff885cdb35c3aeac10381262ddd808fe0d92cb5ba6')):
        source = SOURCE/name
        data = source.read_bytes()
        assert hashlib.sha256(data).hexdigest() == expected
        if name.endswith('.OZT'):
            width, height, bits = struct.unpack_from('<hhB', data, 16)
            assert width > 0 and height > 0 and bits == 32
            assert len(data) >= 22 + width*height*4
            # Same raw pixel span native OpenTga consumes; no image transformation.
            alpha = data[25:22+width*height*4:4]
            print('TEXTURE', name, width, height, 'alphaRange', min(alpha), max(alpha))
        else:
            # Native OpenJpeg skips24 bytes before jpeg_stdio_src.
            with Image.open(io.BytesIO(data[24:])) as decoded:
                decoded.load()
                print('TEXTURE', name, decoded.size, decoded.mode)
        output = checked_output(name)
        if output.exists() and output.read_bytes() != data:
            raise ValueError(f'refuse replacing different texture: {output}')
        texture_plan.append((source, output, expected))
    for tier in ('', '3'):
        for stem in ('Helm', 'Armor', 'Pant', 'Glove', 'Boot'):
            name = stem+'Class'+tier+'08.bmd'
            expected = contract.split('| '+name+' | ',1)[1].split(' |',1)[0]
            source = SOURCE/name
            data = source.read_bytes()
            assert hashlib.sha256(data).hexdigest() == expected
            plain = decrypt_s21_payload(parse_s21_container(data))
            identity, _, end = inspect(plain)
            inspect_skeleton(plain, identity, end)
            converted = encrypt_rise_v0c(plain)
            assert decrypt_rise_v0c(converted) == plain
            output = checked_output(name)
            if output.exists() and output.read_bytes() != converted:
                raise ValueError(f'refuse replacing different candidate: {output}')
            plan.append((source, output, expected, inspect_plaintext(plain)['name'], converted))
    for source, output, expected, model_name, converted in plan:
        if apply and not output.exists():
            subprocess.run([sys.executable, str(Path(__file__).with_name('convert_s21_bmd_v0f_to_rise_v0c.py')),
                            str(source), str(output), '--expected-sha256', expected,
                            '--expected-name', model_name], check=True, stdout=subprocess.DEVNULL)
        if apply:
            assert output.read_bytes() == converted
        print(('STAGED' if apply else 'PLAN'), output.name, hashlib.sha256(converted).hexdigest())
    for source, output, expected in texture_plan:
        if apply and not output.exists():
            assert hashlib.sha256(source.read_bytes()).hexdigest() == expected
            shutil.copy2(source, output)
        if apply:
            assert hashlib.sha256(output.read_bytes()).hexdigest() == expected
        print(('STAGED' if apply else 'PLAN'), output.name, expected)
    print('Native Open2 supports0C/0E, not S21 encrypted0F: converted candidates only')
    print('No runtime registry/QA Client/production changes; actual native texture upload still OPEN')


if __name__ == '__main__':
    main()
