"""One owner-authorized S21 full-dump attempt; no hooks or target writes.

Uses MiniDumpWriteDump from Windows System32. Dump remains local and may
contain sensitive process data. Do not upload it. No retry or privilege bypass.
API: https://learn.microsoft.com/en-us/windows/win32/api/minidumpapiset/nf-minidumpapiset-minidumpwritedump
"""
import argparse
import ctypes as c
from ctypes import wintypes as w
from datetime import datetime, timezone
import hashlib
import json
import msvcrt
import os
from pathlib import Path
import shutil
import uuid

from preflight_s21_live import CLIENT, CLIENT_SHA

ROOT = Path(r'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\artifacts\grow_lancer\full_dump')
CONSENT = 'owner-one-attempt-20260913'


def capture(pid):
    if hashlib.sha256(CLIENT.read_bytes()).hexdigest() != CLIENT_SHA:
        raise ValueError('S21 file identity changed')
    ROOT.mkdir(parents=True, exist_ok=True)
    if ROOT.resolve() != ROOT:
        raise ValueError('Output root redirected; refusing capture')
    if shutil.disk_usage(ROOT).free < 5 * 1024**3:
        raise ValueError('Need at least 5 GiB free for x86 full dump')
    kernel = c.WinDLL('kernel32', use_last_error=True)
    kernel.OpenProcess.argtypes = (w.DWORD, w.BOOL, w.DWORD)
    kernel.OpenProcess.restype = w.HANDLE
    kernel.CloseHandle.argtypes = (w.HANDLE,)
    kernel.CloseHandle.restype = w.BOOL
    kernel.QueryFullProcessImageNameW.argtypes = (
        w.HANDLE, w.DWORD, w.LPWSTR, c.POINTER(w.DWORD))
    kernel.QueryFullProcessImageNameW.restype = w.BOOL
    dbg = c.WinDLL(os.path.join(os.environ['SystemRoot'], 'System32', 'dbghelp.dll'),
                   use_last_error=True)
    dbg.MiniDumpWriteDump.argtypes = (
        w.HANDLE, w.DWORD, w.HANDLE, w.DWORD, c.c_void_p, c.c_void_p, c.c_void_p)
    dbg.MiniDumpWriteDump.restype = w.BOOL
    # Exclusive marker consumes this authorization once; never remove/retry
    # automatically even if OpenProcess or dump generation fails.
    with (ROOT / (CONSENT + '.attempt')).open('x', encoding='utf-8') as marker:
        marker.write(f'pid={pid}\nutc={datetime.now(timezone.utc).isoformat()}\n')
    output = ROOT / ('s21-' + str(pid) + '-' + uuid.uuid4().hex + '.dmp.partial')
    report = dict(pid=pid, startedUtc=datetime.now(timezone.utc).isoformat(),
                  flags='0x1802', status='FAILED', dump=str(output),
                  scope='Snapshot only; not a cast trace or parity proof')
    handle = None
    try:
        # QUERY_INFORMATION | VM_READ. No DUP_HANDLE/ALL_ACCESS, no token edits.
        handle = kernel.OpenProcess(0x0410, False, pid)
        if not handle:
            raise c.WinError(c.get_last_error())
        name = c.create_unicode_buffer(32768)
        size = w.DWORD(len(name))
        if not kernel.QueryFullProcessImageNameW(handle, 0, name, c.byref(size)):
            raise c.WinError(c.get_last_error())
        if Path(name.value).resolve() != CLIENT.resolve():
            raise ValueError('PID is not the authorized S21 executable')
        print('CAPTURING ONCE. Game may pause or exit. Do not launch another reader.', flush=True)
        with output.open('xb') as file:
            # FullMemory | FullMemoryInfo | ThreadInfo. Do NOT silently
            # ignore inaccessible ranges; API failure remains a failed dump.
            if not dbg.MiniDumpWriteDump(handle, pid, msvcrt.get_osfhandle(file.fileno()),
                                        0x1802, None, None, None):
                raise c.WinError(c.get_last_error())
        completed = output.with_suffix('')
        output.rename(completed)
        report.update(status='DUMP_WRITTEN_UNVERIFIED', dump=str(completed),
                      bytes=completed.stat().st_size)
    except (OSError, ValueError) as error:
        report['error'] = str(error)
    finally:
        if handle:
            kernel.CloseHandle(handle)
        report['endedUtc'] = datetime.now(timezone.utc).isoformat()
        with output.with_suffix('.json').open('x', encoding='utf-8') as file:
            json.dump(report, file, indent=2)
        print(json.dumps(report, indent=2), flush=True)
    return 0 if report['status'] == 'DUMP_WRITTEN_UNVERIFIED' else 1


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pid', required=True, type=int)
    args = parser.parse_args()
    if args.pid <= 0:
        parser.error('PID must be positive')
    raise SystemExit(capture(args.pid))
