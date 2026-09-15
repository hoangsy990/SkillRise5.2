"""Verify isolated native BMD::Open2 lance probe, not gameplay acceptance."""
from __future__ import annotations

import argparse
import hashlib
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LOG = ROOT / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log'
EXE = ROOT / ('ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Bin/'
              'Engine-Port S21.before-lance-metadata-F0112DA5.exe')
EXE_SHA = 'F0112DA56BD1D33A91B283707146D5A68527D0D10E8932A770F05E0AAAC7DC04'
EXPECTED = {
    'RapideLance.bmd': (1, 8, 1),
    'ConmocionLance.bmd': (1, 3, 1),
    'PlumaLance.bmd': (3, 22, 1),
    'VisLance.bmd': (2, 15, 1),
    'PrickleLance.bmd': (2, 5, 1),
    'AlacranLance.bmd': (1, 5, 1),
    'bloodangellance01.bmd': (2, 9, 1),
}
LOAD = re.compile(r'event=lance-native-load file=(\S+) loaded=(\d) valid=(\d) '
                  r'meshes=(\d+) bones=(\d+) actions=(\d+) texturesLoaded=(\d+)')
RELEASE = re.compile(r'event=lance-native-release-returned file=(\S+)')


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('--pid', type=int, required=True)
    args = parser.parse_args()
    assert EXE.is_file() and LOG.is_file()
    digest = hashlib.sha256(EXE.read_bytes()).hexdigest().upper()
    assert digest == EXE_SHA, f'QA EXE mismatch: {digest}'
    loads: dict[str, tuple[int, int, int]] = {}
    released: set[str] = set()
    prefix = f'pid={args.pid} '
    with LOG.open('r', encoding='utf-8', errors='replace') as rows:
        for row in rows:
            if prefix not in row:
                continue
            if match := LOAD.search(row):
                name = match.group(1)
                assert name in EXPECTED and name not in loads, row.strip()
                assert (int(match.group(2)), int(match.group(3)), int(match.group(7))) == (1, 1, 0)
                loads[name] = tuple(map(int, match.group(4, 5, 6)))
                assert loads[name] == EXPECTED[name], row.strip()
            elif match := RELEASE.search(row):
                name = match.group(1)
                assert name in loads and name not in released, row.strip()
                released.add(name)
    assert loads == EXPECTED, f'Native load set incomplete: {loads}'
    assert released == EXPECTED.keys(), f'Native release set incomplete: {released}'
    print(f'PASS pid={args.pid} exe_sha256={digest} '
          'native_open2_models=7 native_release_returned=7 textures_loaded=0 '
          'scope=offline-geometry-only')


if __name__ == '__main__':
    main()
