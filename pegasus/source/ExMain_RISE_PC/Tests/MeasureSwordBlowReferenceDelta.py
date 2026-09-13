"""Measure unclipped blue trail deltas in registered reference frames.

Read-only image analysis. Ratios are display-encoded image evidence, not
linear shader/material constants. Caller must verify camera registration.
"""
import argparse
import json
import statistics
from PIL import Image

parser = argparse.ArgumentParser()
parser.add_argument('background')
parser.add_argument('effect')
parser.add_argument('--roi', nargs=4, type=int, required=True)
args = parser.parse_args()
background = Image.open(args.background).convert('RGB')
effect = Image.open(args.effect).convert('RGB')
if background.size != effect.size:
    raise SystemExit('Frame dimensions differ')
x0, y0, x1, y1 = args.roi
if not (0 <= x0 < x1 <= effect.width and 0 <= y0 < y1 <= effect.height):
    raise SystemExit('ROI outside frame')
ratios = []
quiet = []
for before, after in zip(background.crop(args.roi).getdata(), effect.crop(args.roi).getdata()):
    delta = [after[i] - before[i] for i in range(3)]
    if max(abs(v) for v in delta) <= 12:
        quiet.append(sum(abs(v) for v in delta) / 3)
    if max(after) >= 245 or delta[2] < 35 or min(delta) < 0:
        continue
    if delta[2] <= delta[0] or delta[2] <= delta[1]:
        continue
    ratios.append((delta[0] / delta[2], delta[1] / delta[2], 1.0))
print(json.dumps({'roi': args.roi, 'selected_pixels': len(ratios),
    'median_delta_ratio': [statistics.median(row[i] for row in ratios) for i in range(3)] if ratios else None,
    'quiet_pixels': len(quiet), 'quiet_mean_abs_delta': statistics.mean(quiet) if quiet else None,
    'warning': 'Display-space reference delta only; not a material RGB prescription'}, indent=2))
