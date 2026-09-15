"""Read-only QA log audit. Input snapshots are not controller/render traces."""
import re
from pathlib import Path

root = Path(__file__).resolve().parents[2]
log = root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/GrowLancerRuntimeQA.log'
pattern = re.compile(r'^(\S+) pid=(\d+).*phase=before-dispatch skill=(\d+).* action=(\d+) frame=([\d.]+)')
# First emission gates in the current native adapter. Not wall-clock delays.
gates = {271: (285, 4.7), 274: (287, 5.2), 276: (284, 5.2)}
rows = []
for line in log.read_text(errors='replace').splitlines():
    match = pattern.search(line)
    if match:
        stamp, pid, skill, action, frame = match.groups()
        skill, action, frame = int(skill), int(action), float(frame)
        if skill in gates and action == gates[skill][0] and frame < gates[skill][1]:
            rows.append((stamp, pid, skill, action, frame))
for stamp, pid, skill, action, frame in rows:
    print(f'{stamp} pid={pid} skill={skill} same-action restart frame={frame:.6f} before first gate={gates[skill][1]}')
print(f'{len(rows)} early same-action restart snapshots; no claim of visual cause or stage execution')
