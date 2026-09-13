"""Read-only S21 identity/code preflight. Does not arm a cast capture or hook.

Run with the current S21 PID. No privilege adjustment, injection, suspension,
process-memory writes, or target-file writes are performed. JSON goes to stdout.
"""
import argparse
import ctypes as c
from ctypes import wintypes as w
import hashlib
import json
from pathlib import Path

from disasm_s21_window import DUMP, IMAGE_BASE

CLIENT = Path(r'D:\MU FICA Season 21\main.exe')
CLIENT_SHA = 'f42ed0613ea0cd19d24c43c4168d1936adeb0c054b306973bd0afe6e364ccbd9'
DUMP_SHA = '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
# Anchored function entries, not object fields. Absolute-address client only;
# a relocated or mismatched image fails closed instead of guessing a new base.
ENTRIES = (0xEF2A81, 0x12C4F86, 0x143EECA, 0x14B76C0)


def check_live(pid):
    if hashlib.sha256(CLIENT.read_bytes()).hexdigest() != CLIENT_SHA:
        raise ValueError('S21 on-disk binary changed')
    reference = DUMP.read_bytes()
    if hashlib.sha256(reference).hexdigest() != DUMP_SHA:
        raise ValueError('Reference dump hash changed')
    kernel = c.WinDLL('kernel32', use_last_error=True)
    kernel.OpenProcess.argtypes = (w.DWORD, w.BOOL, w.DWORD)
    kernel.OpenProcess.restype = w.HANDLE
    kernel.CloseHandle.argtypes = (w.HANDLE,)
    kernel.CloseHandle.restype = w.BOOL
    kernel.QueryFullProcessImageNameW.argtypes = (
        w.HANDLE, w.DWORD, w.LPWSTR, c.POINTER(w.DWORD))
    kernel.QueryFullProcessImageNameW.restype = w.BOOL
    kernel.ReadProcessMemory.argtypes = (
        w.HANDLE, c.c_void_p, c.c_void_p, c.c_size_t, c.POINTER(c.c_size_t))
    kernel.ReadProcessMemory.restype = w.BOOL
    # PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ; no ALL_ACCESS.
    handle = kernel.OpenProcess(0x1010, False, pid)
    if not handle:
        raise c.WinError(c.get_last_error())
    try:
        path = c.create_unicode_buffer(32768)
        length = w.DWORD(len(path))
        if not kernel.QueryFullProcessImageNameW(handle, 0, path, c.byref(length)):
            raise c.WinError(c.get_last_error())
        if Path(path.value).resolve() != CLIENT.resolve():
            raise ValueError('PID belongs to a different executable')
        for va in ENTRIES:
            buffer = c.create_string_buffer(32)
            actual = c.c_size_t()
            if not kernel.ReadProcessMemory(handle, va, buffer, 32, c.byref(actual)):
                raise c.WinError(c.get_last_error())
            if actual.value != 32:
                raise ValueError('Short process-memory read')
            if buffer.raw != reference[va-IMAGE_BASE:va-IMAGE_BASE+32]:
                raise ValueError(f'Live code differs from pinned dump at {va:#x}')
        return dict(pid=pid, executable=path.value, status='PREFLIGHT_PASS',
                    checkedEntries=[hex(va) for va in ENTRIES],
                    scope='Identity and four code prefixes only; capture NOT armed')
    finally:
        kernel.CloseHandle(handle)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--pid', required=True, type=int)
    args = parser.parse_args()
    if args.pid <= 0:
        parser.error('PID must be positive')
    try:
        result = check_live(args.pid)
    except (OSError, ValueError) as error:
        print(json.dumps(dict(pid=args.pid, status='NOT_READY', error=str(error))))
        return 1
    print(json.dumps(result, indent=2))
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
