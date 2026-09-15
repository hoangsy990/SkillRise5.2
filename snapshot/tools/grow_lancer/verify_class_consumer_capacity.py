#!/usr/bin/env python3
"""Read-only class-consumer audit after reserving GS class index7.

Reserved array capacity and source identity are not create/login/skill
activation. Legacy SS6 client/UI class tables still have seven columns.
"""

from __future__ import annotations

import hashlib
from pathlib import Path
import sys


ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE"
SERVER = ROOT / "ExGameServer" / "GameServer"


ANCHORS = {
    CLIENT / "_define.h": ("#define MAX_CLASS",),
    CLIENT / "CharMakeWin.cpp": (
        "int _btn_classname[MAX_CLASS]",
        "char* apszStat[MAX_CLASS][4]",
    ),
    CLIENT / "ZzzOpenData.cpp": (
        "MODEL_BODY_HELM + i",
        "(MAX_CLASS * 2) + i",
    ),
    CLIENT / "CharacterManager.cpp": (
        "GetSkinModelIndex",
        "* MAX_CLASS",
    ),
    CLIENT / "NewUIMuHelper.h": ("BYTE class_character[MAX_CLASS]",),
    CLIENT / "_struct.h": ("BYTE RequireClass[MAX_CLASS]",),
    SERVER / "DefaultClassInfo.h": (
        "#define MAX_CLASS 8",
        "#define CLASS_GL 7",
        "m_DefaultClassInfo[MAX_CLASS]",
    ),
    SERVER / "SkillManager.h": ("int RequireClass[MAX_CLASS]",),
    SERVER / "MasterSkillTree.h": ("int RequireClass[MAX_CLASS]",),
    SERVER / "ItemManager.h": ("RequireClass[MAX_CLASS]",),
    SERVER / "ItemBagEx.h": ("RequireClass[MAX_CLASS]",),
    SERVER / "ServerInfo.h": ("m_DamageRateTo[MAX_CLASS][MAX_CLASS]",),
    SERVER / "Protocol.cpp": ("lpMsg->Class != DB_CLASS_RF",),
    SERVER / "ObjectManager.cpp": (
        "lpObj->Class\t\t\t= lpMsg->Class / 16",
        "lpObj->ChangeUp",
        "lpMsg->Class % 16",
    ),
    SERVER / "User.h": ("WORD Class;", "BYTE DBClass;", "BYTE ChangeUp;"),
}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest().upper()


def main() -> int:
    failures: list[str] = []
    for path, tokens in ANCHORS.items():
        if not path.is_file():
            failures.append(f"missing {path.relative_to(ROOT)}")
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        for token in tokens:
            if token not in text:
                failures.append(f"missing anchor {token!r} in {path.relative_to(ROOT)}")

    native_roots = (CLIENT, SERVER)
    forbidden = ("DB_CLASS_GL", "DB_CLASS_ML", "DB_CLASS_GL_4",
                 "GetLegacyConfigClass")
    for tree in native_roots:
        for path in tree.rglob("*"):
            if path.suffix.lower() not in {".h", ".hpp", ".c", ".cpp", ".inl"}:
                continue
            text = path.read_text(encoding="utf-8", errors="replace")
            for token in forbidden:
                if token in text:
                    failures.append(f"forbidden native activation token {token} in {path.relative_to(ROOT)}")

    for path in (SERVER / "Protocol.cpp", SERVER / "ObjectManager.cpp"):
        text = path.read_text(encoding="utf-8", errors="replace")
        if "CLASS_GL" in text or "DB_CLASS_GL" in text:
            failures.append(f"class7 create/load gate activated in {path.relative_to(ROOT)}")
    legacy = (ROOT / "Shared" / "LegacySkillClassColumns.h").read_text()
    if "i < 7" not in legacy or "i < Capacity" not in legacy:
        failures.append("legacy seven-column reader changed")

    if failures:
        for failure in failures:
            print(f"FAIL {failure}")
        return 1

    print("PASS SS6 class-consumer anchors and reserved GS class7 capacity")
    for path in ANCHORS:
        print(f"  {path.relative_to(ROOT)} sha256={sha256(path)}")
    print("PASS legacy seven-column reader and no GL config alias or DB create/load activation")
    print("Scope: source capacity audit; class7 ABI/defaults/rates/skills still open")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
