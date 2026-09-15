"""Verify the read-only newer-source Grow Lancer class reference.

This is a provenance check only. Reserved CLASS_GL index7 is not permission
to copy newer server handlers/defaults into the SS6 worktree.
"""
from hashlib import sha256
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
REFERENCE_ROOT = Path(r"D:\MU-CrossPlatform\SRCMainGS\Source\GameServer\GameServer")

PINNED = {
    "DefaultClassInfo.h": "0726F56BD5004EA5310BF5BAB3818F0AA0D142635900D9232CB154FA1EB5E1D1",
    "Protocol.cpp": "5CCC2E60ED3D8BBCE68186F8292C850A8694BCD37C1B5885DC2BA0E2BF0BCC63",
    "ObjectManager.cpp": "F8D515A1589E8BF8E93362657ACBBF04C9F66BA5EAE14A37CF99D94089B49C52",
    "ServerInfo.cpp": "52AD94DD946A9D13DDE4ED39ACF2F592B4A80AE47ABD7861985EEC3D52C24918",
    "User.cpp": "AD242CE75840193035FAA80496D3EDB862020CDB7ABF3792D5C9743041D996A3",
    "Attack.cpp": "EE22F6877D42E34C27BF90328AD9077830C78A9B7C0563D0B08758F87E39D83C",
    "Item.cpp": "D283C33FA15C79D5101AE1D9C6513ECFE29B43560CD7D4B57F0BA4514738C17F",
    "CommandManager.cpp": "C30F20A3BDB0672C7D9B21A40899C5240B6CE75EDE61FB08E773F0A74B1D0536",
}


def digest(path: Path) -> str:
    h = sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest().upper()


def main() -> None:
    doc = (ROOT / "GrowLancer/NEWER_SOURCE_CLASS_REFERENCE.md").read_text(
        encoding="utf-8"
    )
    assert "REFERENCE_ONLY" in doc
    assert "Do not copy newer" in doc
    for name, expected in PINNED.items():
        path = REFERENCE_ROOT / name
        assert path.is_file(), path
        actual = digest(path)
        assert actual == expected, (name, actual, expected)
        assert expected in doc, (name, "hash missing from reference doc")

    newer_header = (REFERENCE_ROOT / "DefaultClassInfo.h").read_text(
        encoding="latin-1"
    )
    assert "CLASS_GL  = 7" in newer_header
    assert "DB_CLASS_GL    = 112" in newer_header
    assert "DB_CLASS_GL_4  = 115" in newer_header

    ss6_server = (ROOT / "ExGameServer/GameServer/DefaultClassInfo.h").read_text(
        encoding="latin-1"
    )
    ss6_client = (ROOT / "ExMain_RISE_PC/Main5.2_RISE/_define.h").read_text(
        encoding="latin-1"
    )
    assert re.search(r"#define\s+MAX_CLASS\s+8\b", ss6_server)
    assert re.search(r"#define\s+MAX_CLASS\s+7\b", ss6_client)
    assert "#define CLASS_GL 7" in ss6_server and "DB_CLASS_GL" not in ss6_server
    assert "CLASS_GROWLANCER" not in ss6_client
    print("PASS newer-source GL class reference hashes and symbols are pinned")
    print("PASS GS class7 capacity reserved/client seven-class format retained; no DB GL activation")
    print("Scope: read-only port map; no source copied and no class ABI enabled")


if __name__ == "__main__":
    main()
