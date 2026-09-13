"""Reopen candidate and compare raw records; never stages or activates assets."""
import hashlib
import json
from pathlib import Path
import merge_player_actions as bmd

ROOT = Path(__file__).resolve().parents[2]
manifest = json.loads((ROOT / 'GrowLancer/mounted_reaction_candidate.json').read_text())
parsed = {}
for label in ('source', 'base', 'candidate'):
    path = Path(manifest[label])
    if not path.is_absolute():
        path = ROOT / path
    assert hashlib.sha256(path.read_bytes()).hexdigest().upper() == manifest[label+'Sha256']
    parsed[label] = bmd.parse(bmd.decode(path))
s, base, candidate = (parsed[k] for k in ('source', 'base', 'candidate'))
assert (base.action_count, candidate.action_count) == (294,298)
assert candidate.bone_headers == base.bone_headers
assert candidate.action_records[:294] == base.action_records
for old, new in zip(base.bone_action_keys, candidate.bone_action_keys):
    assert (old is None and new is None) or new[:294] == old
assert [(a['source'],a['candidate']) for a in manifest['actions']] == [(335,294),(336,295),(337,296),(338,297)]
check = bmd.COMPARER.parse_zero_mesh_skeleton(candidate.payload)
for action in manifest['actions']:
    old, new = action['source'], action['candidate']
    assert candidate.action_records[new] == s.action_records[old]
    for source_keys, candidate_keys in zip(s.bone_action_keys, candidate.bone_action_keys):
        assert (source_keys is None and candidate_keys is None) or source_keys[old] == candidate_keys[new]
    assert check.key_counts[new] == action['keys'] == 6
    assert check.action_hashes[new] == action['clipSha256']
assert manifest['activation'] == 'candidate_only_not_loaded_by_runtime'
print('PASS pinned assets, raw preservation of294 actions and four complete source-to-candidate clips')
print('OPEN runtime registration/staging/class/GS/visual gates; candidate only')
