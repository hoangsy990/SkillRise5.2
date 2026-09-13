"""Create a source-only, hash-verified recovery overlay in an isolated clone."""
from pathlib import Path
import hashlib
import json
import shutil
import subprocess

root = Path(__file__).resolve().parents[2]
destination = root / 'ExMain_RISE_PC/Tests/PegasusBuild/GitHubBackup_20260913/pegasus'
def git(*args):
    return subprocess.check_output(['git', '-c', 'core.autocrlf=false', *args], cwd=root).decode().strip()
paths = set(git('diff', '--name-only', 'HEAD').splitlines())
paths.update(git('ls-files', '--others', '--exclude-standard').splitlines())
allowed = {'.cpp', '.h', '.ps1', '.py', '.cmd', '.md', '.json', '.vcxproj'}
entries = []
deleted = []
for name in sorted(paths):
    relative = Path(name)
    if any(part in {'PegasusBuild', '__pycache__', '.git'} for part in relative.parts):
        continue
    if relative.suffix.lower() not in allowed:
        continue
    source = root / relative
    if not source.exists():
        deleted.append(name)
        continue
    if not source.resolve().is_relative_to(root.resolve()):
        raise ValueError('Source escapes worktree')
    data = source.read_bytes()
    if len(data) > 10_000_000:
        raise ValueError('Unexpected large source: ' + name)
    target = destination / 'source' / relative
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, target)
    digest = hashlib.sha256(data).hexdigest()
    if hashlib.sha256(target.read_bytes()).hexdigest() != digest:
        raise ValueError('Copy mismatch: ' + name)
    entries.append({'path': name, 'bytes': len(data), 'sha256': digest})
manifest = {'kind': 'source-overlay', 'base_repository': 'https://github.com/hoangsy990/Rise-CrossPlatform',
            'base_commit': git('rev-parse', 'HEAD'), 'branch': git('branch', '--show-current'),
            'status': 'IN_PROCESS', 'files': entries, 'deleted_paths': deleted,
            'excluded': ['generated builds', 'runtime game data', 'saved accounts/config', 'logs', 'original Pegasus binaries/assets']}
(destination / 'BACKUP_MANIFEST.json').write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
print(f'Verified {len(entries)} source files, {sum(x["bytes"] for x in entries)} bytes; deleted={len(deleted)}')
