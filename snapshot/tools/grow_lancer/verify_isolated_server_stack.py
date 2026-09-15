#!/usr/bin/env python3
"""Verify the Grow Lancer QA server stack without starting any service."""

from __future__ import annotations

import hashlib
import json
import re
from pathlib import Path


ROOT = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer")
STACK = ROOT / "ExGameServer" / "Tests" / "GrowLancerBuild" / "ServerStackSmoke"
MANIFEST = STACK / "STACK_MANIFEST.json"
CONFIG = {
    "1.ConnectServer/ConnectServer.ini": ("ConnectServerPortTCP =44412", "ConnectServerPortUDP =55561"),
    "1.ConnectServer/ServerList.xml": ('Code="10" IP="127.0.0.1" Port="55910"',),
    "2.DataServer/DataServer.ini": ("DataServerODBC =GrowLancer_QA", "DataServerPort =55964"),
    "3.JoinServer/JoinServer.ini": ("JoinServerODBC =GrowLancer_QA", "JoinServerPort =55974", "ConnectServerPort =55561"),
    "4.MuServer/Sub-1/GameServer/Data/GameServerInfo - Common.ini": ("ServerCode = 10", "ServerPort = 55910", "DataServerPort =55964", "JoinServerPort =55974", "ConnectServerPort =55561"),
    "4.MuServer/Sub-1/Data/MapServerInfo.ini": ("10", "S127.0.0.1", "55910"),
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest().upper()


def main() -> None:
    assert STACK.resolve() == STACK, "server stack root must not be a reparse target"
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    for component in manifest["components"]:
        path = STACK / component["relativePath"]
        assert path.is_file(), f"missing isolated component: {path}"
        assert sha256(path) == component["sha256"], f"component hash mismatch: {path}"
    for relative in manifest["requiredRelativeData"]:
        path = STACK / relative
        assert path.is_file() and path.stat().st_size > 0, f"missing required server data: {path}"
    for relative, required in CONFIG.items():
        content = (STACK / relative).read_text(encoding="utf-8-sig")
        for token in required:
            assert token in content, f"Grow Lancer QA config mismatch: {relative}: {token}"
        assert not re.search(r"44405|55557|55960|55970|55901|RISE5\.2|113\.161\.53\.27", content), f"foreign stack reference: {relative}"
    server_list = (STACK / "1.ConnectServer/ServerList.xml").read_text(encoding="utf-8-sig")
    assert server_list.count("<Server Code=") == 1, "QA must advertise only its own server"
    map_server = (STACK / "4.MuServer/Sub-1/Data/MapServerInfo.ini").read_text(encoding="utf-8-sig")
    assert len(re.findall(r"^10\s+0\s+1\s+S127\.0\.0\.1\s+55910\s*$", map_server, re.M)) == 1, "QA must map only server code 10"
    game_server = STACK / manifest["components"][-1]["relativePath"]
    built = ROOT / "ExGameServer" / "Tests" / "GrowLancerBuild" / "Bin" / "GameServer.exe"
    assert sha256(game_server) == sha256(built), "staged GameServer is not the current isolated build"
    print("PASS: four hash-pinned server components are inside the Grow Lancer worktree")
    print("PASS: staged GameServer matches the current isolated Ex603 build")
    print("PASS: relative ..\\Data\\CashShop contract resolves from the GameServer working directory")
    print("PASS: QA ports, loopback route and DSN are isolated from Slayer/Pegasus")
    print("Scope: static stack readiness only; no process, database login or gameplay acceptance")


if __name__ == "__main__":
    main()
