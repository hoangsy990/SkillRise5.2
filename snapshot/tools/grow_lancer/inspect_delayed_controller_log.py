"""Before-update samples: quantify controller delay, not visual onset."""
from pathlib import Path
import hashlib
import re

root = Path(__file__).resolve().parents[2]
raw = (root/'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log').read_bytes()
print('source_sha256', hashlib.sha256(raw).hexdigest())
casts = {274: [], 276: []}
for line in raw.decode('utf-8', errors='replace').splitlines():
    if 'event=controller-sample ' not in line:
        continue
    row = dict(re.findall(r'(\w+)=([^ ]+)', line))
    skill = int(row['skill'])
    if skill not in casts:
        continue
    if row['index'] == '0' or not casts[skill]:
        casts[skill].append([])
    casts[skill][-1].append(row)
for skill, groups in casts.items():
    for number, rows in enumerate(groups, 1):
        gates = [r for r in rows if 5.2 <= float(r['frame']) <= 5.8 and r['stage']=='0']
        changes = [r for i,r in enumerate(rows) if i and r['stage']!=rows[i-1]['stage']]
        print('skill',skill,'cast',number,'pid',rows[0]['pid'],'samples',len(rows),
              'first_gate_ms',gates[0]['ms'] if gates else 'NONE',
              'stage_observations',[(r['ms'],r['frame'],r['stage']) for r in changes],
              'actions',sorted({r['action'] for r in rows}))
print('Before-update observations bound stage transitions; no GPU/visible onset or S21 wall-time parity is established.')
