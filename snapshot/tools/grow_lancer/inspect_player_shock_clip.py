"""Read-only cross-ID clip comparison; not proof of playback speed or gameplay."""
import hashlib
import re
import struct
from pathlib import Path

import merge_player_actions as parser

ROOT = Path(__file__).resolve().parents[2]


def shock_id(riding):
    text = (ROOT / 'ExMain_RISE_PC/Main5.2_RISE/_enum.h').read_text(encoding='latin1')
    text = text[text.index('PLAYER_SET,'):]
    text = text[:text.index('PLAYER_DIE1')]
    text = re.sub(r'//[^\n]*', '', text)
    text = re.sub(r'#ifdef YDG_ADD_SKILL_RIDING_ANIMATIONS(.*?)#endif',
                  lambda m: m[1] if riding else '', text, flags=re.S)
    values = {}
    value = -1
    for token in text.split(','):
        token = token.strip()
        if not token:
            continue
        match = re.fullmatch(r'(PLAYER_\w+)(?:\s*=\s*(PLAYER_\w+))?', token)
        assert match, token
        value = values[match[2]] if match[2] else value + 1
        values[match[1]] = value
    return values['PLAYER_SHOCK']


def canonical(header):
    return (True, b'', -1) if len(header) == 1 else (
        False, header[1:33].split(b'\0')[0], struct.unpack_from('<h', header, 33)[0])


def main():
    source = Path(r'D:\MU FICA Season 21\Data\Player\player.bmd')
    native = ROOT / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/Player/player.bmd'
    skeletons = []
    for path in (source, native):
        payload = parser.decode(path)
        raw = parser.parse(payload)
        clips = parser.COMPARER.parse_zero_mesh_skeleton(payload)
        skeletons.append((raw, clips))
        print(f'Asset={path}; SHA256={hashlib.sha256(path.read_bytes()).hexdigest().upper()}; actions={raw.action_count}')
    (raw_s, s), (raw_n, n) = skeletons
    assert [canonical(h) for h in raw_s.bone_headers] == [canonical(h) for h in raw_n.bone_headers]
    print(f'PASS canonical bone names/parents/dummy layout: {raw_s.bone_count}')
    for enabled in (False, True):
        action = shock_id(enabled)
        print(f'RidingMacro={enabled}; native={action}; S21=328; keys={n.key_counts[action]}/{s.key_counts[328]}; '
              f'lock={n.lock_positions[action]}/{s.lock_positions[328]}; equal={n.action_hashes[action] == s.action_hashes[328]}')
        print(f'NativeClipSHA256={n.action_hashes[action]}; S21ClipSHA256={s.action_hashes[328]}')
    for action in range(335, 342):
        matches = [i for i, digest in enumerate(n.action_hashes[:284])
                   if digest == s.action_hashes[action]]
        print(f'AuxiliaryReaction={action}; keys={s.key_counts[action]}; '
              f'legacyMatches={matches}; SHA256={s.action_hashes[action]}')
    print('OPEN: compiled macro selection, action speed, transition gates, auxiliary mounts and gameplay parity')


if __name__ == '__main__':
    main()
