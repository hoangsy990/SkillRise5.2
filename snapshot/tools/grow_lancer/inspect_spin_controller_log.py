"""Report recorded Spin frame/target and native render evidence.

This is a read-only QA log tool.  A resolved target and successful native
submission stages do not prove that the owner saw matching GPU pixels.
"""
from pathlib import Path
import hashlib
import re
from collections import Counter

root = Path(__file__).resolve().parents[2]
path = root/'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log'
raw = path.read_bytes()
print('source_sha256', hashlib.sha256(raw).hexdigest())
casts = []
render_re = re.compile(
    r'event=spin-render stage=([^ ]+) result=(-?\d+) type=(\d+) '
    r'subtype=(\d+) .*?target=(-?\d+)')
for line in raw.decode('utf-8', errors='replace').splitlines():
    if 'event=controller-sample skill=271 ' not in line:
        continue
    row = dict(re.findall(r'(\w+)=([^ ]+)', line))
    if row['index'] == '0' or not casts:
        casts.append([])
    casts[-1].append(row)
for number, rows in enumerate(casts, 1):
    gate = [r for r in rows if 4.7 < float(r['frame']) <= 6.5]
    print('cast', number, 'pid', rows[0]['pid'], 'samples', len(rows),
          'targets', sorted({int(r['target']) for r in rows}),
          'gate_samples', len(gate), 'gate_stages', sorted({int(r['stage']) for r in gate}))

# Re-associate render records with the nearest preceding controller cast.  The
# runtime recorder is bounded per cast, but does not write a cast serial.
lines = raw.decode('utf-8', errors='replace').splitlines()
cast_serial = -1
renders = {}
for line in lines:
    if 'event=cast-input phase=before-dispatch skill=271 ' in line:
        cast_serial += 1
        renders[cast_serial] = []
    match = render_re.search(line)
    if match and cast_serial >= 0:
        renders[cast_serial].append(match.groups())

for serial, rows in sorted(renders.items()):
    if not rows:
        continue
    stages = Counter(stage for stage, _result, _type, _subtype, _target in rows)
    successes = Counter(stage for stage, result, _type, _subtype, _target in rows
                        if int(result) == 1)
    print('cast', serial + 1, 'render_records', len(rows),
          'stages', dict(stages), 'result1', dict(successes))
print('Stage/gate evidence proves only native dispatch/submission. It does not prove target Live, GPU pixels or visual parity.')
