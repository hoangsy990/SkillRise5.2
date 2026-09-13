"""Read-only index/name/parent comparison; not bind-pose or skinning parity."""
import hashlib
import json
import subprocess
import sys
from pathlib import Path
from inspect_equipment_meshes import inspect
from merge_player_actions import decode

ROOT = Path(__file__).resolve().parents[2]


def mismatches(equipment, player):
    # Include ancestors, not only the nodes directly referenced by vertices.
    needed = set(equipment['usedBones'])
    for index in list(needed):
        seen = set()
        while index >= 0:
            if index in seen:
                raise ValueError('Cyclic equipment parent chain')
            seen.add(index)
            needed.add(index)
            bone = equipment['bones'][index]
            index = bone.get('parent', -1)
    return [index for index in sorted(needed)
            if index >= len(player['bones']) or
            equipment['bones'][index] != player['bones'][index]]


def main():
    paths = dict(s21=Path(r'D:\MU FICA Season 21\Data\Player\player.bmd'),
                 dkQA=ROOT / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/Player/player.bmd')
    players = {}
    for label, path in paths.items():
        players[label] = inspect(decode(path))
        print(json.dumps(dict(player=label, sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
                              boneCount=players[label]['boneCount'])), flush=True)
    report = json.loads(subprocess.check_output(
        [sys.executable, str(Path(__file__).with_name('inspect_breche_item_models.py'))]))
    unverified = 0
    for source in sorted({row['source'] for row in report['models']}):
        try:
            equipment = inspect(decode(Path(source)))
            result = {label: mismatches(equipment, player) for label, player in players.items()}
            result['scope'] = 'bone index/name/parent only; pose and renderer unverified'
        except ValueError as error:
            unverified += 1
            result = dict(status='UNVERIFIED', error=str(error))
        print(json.dumps(dict(source=source, **result)), flush=True)
    return 1 if unverified else 0


if __name__ == '__main__':
    sys.exit(main())
