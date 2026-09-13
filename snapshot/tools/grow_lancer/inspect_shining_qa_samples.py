"""Read buffered native QA diagnostics; never declares source/visual parity."""
import json
import math
from pathlib import Path
import re
import sys

def inspect(text):
    batches = []
    for line in text.splitlines():
        if 'event=shining-samples ' in line:
            fields = dict(re.findall(r'(\w+)=([^\s]+)', line))
            batches.append({'expected': int(fields['count']), 'dropped': int(fields['dropped']), 'samples': []})
        elif 'event=shining-sample ' in line and batches:
            fields = dict(re.findall(r'(\w+)=([^\s]+)', line))
            batches[-1]['samples'].append({
                key: float(fields[key]) if key in ('frame', 'factor', 'life') else int(fields[key])
                for key in ('index', 'action', 'frame', 'factor', 'stage', 'life')})
    for batch in batches:
        rows = batch['samples']
        valid = (0 < batch['expected'] <= 512 and len(rows) == batch['expected']
                 and batch['dropped'] == 0
                 and all(r['index'] == i and 0 <= r['stage'] <= 4
                         and all(math.isfinite(r[k]) for k in ('frame', 'factor', 'life'))
                         for i, r in enumerate(rows)))
        batch['status'] = 'COMPLETE_LOG_BATCH_ONLY' if valid else 'INCOMPLETE_OR_INVALID'
        batch['observed_stages'] = sorted({r['stage'] for r in rows})
        batch['observed_action_end'] = any(r['action'] != 288 for r in rows)
        # A complete flush is not a complete cast, successful child allocation,
        # isolated ownership, measured source correspondence or visual PASS.
    return {'scope': 'native QA logs only; no parity verdict', 'batches': batches}

if __name__ == '__main__':
    if len(sys.argv) != 2:
        raise SystemExit('usage: inspect_shining_qa_samples.py LOG')
    result = inspect(Path(sys.argv[1]).read_text(encoding='utf-8', errors='replace'))
    print(json.dumps(result, indent=2))
    raise SystemExit(0 if result['batches'] else 2)
