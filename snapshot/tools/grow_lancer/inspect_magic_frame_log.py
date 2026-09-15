"""Validate captured frame accounting only. Never a skill/visual PASS gate."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import re


def inspect(text, selected_pid=None):
    groups = []
    active = {}
    errors = []
    for line_number, line in enumerate(text.splitlines(), 1):
        if not re.search(r'event=magic-frames?\s', line):
            continue
        fields = dict(re.findall(r'(\w+)=([^\s]+)', line))
        try:
            pid = int(fields['pid'])
            if selected_pid is not None and pid != selected_pid:
                continue
            if fields['event'] == 'magic-frames':
                count, dropped = int(fields['count']), int(fields['dropped'])
                if not 0 <= count <= 512 or dropped < 0:
                    raise ValueError('invalid summary counts')
                group = dict(pid=pid, expected=count, dropped=dropped, rows=[])
                active[pid] = group
                groups.append(group)
                continue
            if pid not in active:
                raise ValueError('row without summary')
            group = active[pid]
            values = {key: int(fields[key]) for key in (
                'index', 'ms', 'owned', 'ran', 'pendingBefore', 'pendingAfter', 'completed', 'action')}
            frame = float(fields['frame'])
            if any(value < 0 for value in values.values()) or not math.isfinite(frame) or frame < 0:
                raise ValueError('invalid numeric field')
            if values['owned'] not in (0, 1) or values['ran'] not in (0, 1):
                raise ValueError('invalid boolean')
            if values['index'] != len(group['rows']):
                raise ValueError('nonconsecutive index')
            if values['owned']:
                if values['pendingBefore'] != values['pendingAfter'] + 40 * values['completed']:
                    raise ValueError('40ms accounting mismatch')
                if not values['ran'] and values['completed']:
                    raise ValueError('completed ticks without Run')
            group['rows'].append(values)
        except (ValueError, KeyError) as error:
            errors.append(f'line {line_number}: {error}')
    rows = [row for group in groups for row in group['rows']]
    dropped_groups = sum(group['dropped'] > 0 for group in groups)
    truncated_groups = sum(len(group['rows']) != group['expected'] for group in groups)
    incomplete = dropped_groups > 0 or truncated_groups > 0
    owned = sum(row['owned'] for row in rows)
    # A mid-frame lifecycle reset can legitimately clear ownership and pending time;
    # it needs separate runtime review, not a false arithmetic PASS or FAIL.
    resets = sum(not row['owned'] and (row['completed'] or row['pendingBefore'] or row['pendingAfter']) != 0 for row in rows)
    completed = sum(row['completed'] for row in rows)
    status = ('INVALID' if errors else 'INCOMPLETE' if incomplete else
              'NO_CAPTURE' if not rows else 'RESET_REVIEW_REQUIRED' if resets else
              'MODE_OFF' if not owned else 'NO_QUANTA' if not completed else 'ACCOUNTING_CONSISTENT')
    return dict(status=status, groups=len(groups), rows=len(rows),
                dropped_groups=dropped_groups, truncated_groups=truncated_groups,
                owned_rows=owned,
                completed=completed, reset_rows=resets,
                errors=errors, visual_acceptance=False)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('log', nargs='?', type=Path, default=Path(__file__).resolve().parents[2] /
                        'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log')
    parser.add_argument('--pid', type=int, default=None,
                        help='Inspect only one exact isolated QA process ID')
    args = parser.parse_args()
    raw = args.log.read_bytes()
    result = inspect(raw.decode('utf-8', errors='replace'), args.pid)
    result['pid'] = args.pid
    result['sha256'] = hashlib.sha256(raw).hexdigest()
    print(json.dumps(result, indent=2))
    raise SystemExit(0 if result['status'] == 'ACCOUNTING_CONSISTENT' else 2)
