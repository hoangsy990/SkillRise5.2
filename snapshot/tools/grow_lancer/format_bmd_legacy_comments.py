"""Lossless-code comment formatting to make this pinned legacy file UTF-8 readable.
Only invalid UTF-8 bytes inside lexical comments become ASCII hex annotations.
All code and string-literal bytes remain identical. Default is read-only.
"""
from pathlib import Path
import argparse
import hashlib

root = Path(__file__).resolve().parents[2]
path = root/'ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp'
pin = '831a4c0bd79cad2720c2b3ca5c54efeb9325d115dbf243903af3288028be7d4a'
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument('--apply', action='store_true')
args = parser.parse_args()
data = path.read_bytes()
assert hashlib.sha256(data).hexdigest() == pin, 'Source changed; re-audit first'
comments = set()
i = 0
while i < len(data):
    if data[i:i+2] == b'//':
        end = data.find(b'\n', i)
        end = len(data) if end < 0 else end
        # Reject escaped multiline comments rather than misclassify following code.
        assert not data[i:end].rstrip(b'\r').endswith(b'\\')
        comments.update(range(i, end)); i = end
    elif data[i:i+2] == b'/*':
        end = data.find(b'*/', i+2)
        assert end >= 0
        comments.update(range(i, end+2)); i = end+2
    elif data[i:i+2] == b'R"':
        opening = data.find(b'(', i+2)
        assert opening >= 0 and opening-i <= 18
        delimiter = b')'+data[i+2:opening]+b'"'
        end = data.find(delimiter, opening+1)
        assert end >= 0
        i = end+len(delimiter)
    elif data[i] in (34,39):
        quote = data[i]; i += 1
        while i < len(data):
            if data[i] == 92: i += 2
            elif data[i] == quote: i += 1; break
            else: i += 1
        else: raise ValueError('Unterminated literal')
    else:
        i += 1

invalid = set()
offset = 0
for char in data.decode('utf-8', errors='surrogateescape'):
    raw = char.encode('utf-8', errors='surrogateescape')
    if 0xdc80 <= ord(char) <= 0xdcff: invalid.add(offset)
    offset += len(raw)
assert invalid and invalid <= comments, 'Invalid bytes outside comments; cannot format safely'
candidate = b''.join((('\\x%02X' % byte).encode('ascii') if n in invalid else bytes([byte]))
                     for n, byte in enumerate(data))
candidate.decode('utf-8', errors='strict')
print(f'Invalid UTF-8 bytes={len(invalid)}; all inside comments. Code/string bytes untouched.')
if args.apply:
    backup = root/'ExMain_RISE_PC/Tests/GrowLancerBuild/ZzzBMD.before-comment-format.bin'
    if backup.exists(): assert backup.read_bytes() == data
    else: backup.write_bytes(data)
    assert path.read_bytes() == data, 'Source changed during formatting'
    path.write_bytes(candidate)
    assert path.read_bytes() == candidate
    print('Formatted comments only; original bytes backed up at '+str(backup))
print('CandidateSHA256='+hashlib.sha256(candidate).hexdigest())
