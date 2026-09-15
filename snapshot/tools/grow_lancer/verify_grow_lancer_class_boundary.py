"""Guard reserved class7 capacity and the read-only newer-source lead.

The GS array index is reserved, but create/load/defaults/skills remain
fail-closed until the complete source-backed class contract is implemented.
"""
from pathlib import Path
import re


ROOT = Path(__file__).resolve().parents[2]
GS = ROOT / "ExGameServer" / "GameServer"
CLIENT = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE"
NEWER = Path(r"D:\MU-CrossPlatform\SRCMainGS\Source\GameServer\GameServer")


def text(path: Path) -> str:
    return path.read_text(encoding="latin-1")


def main() -> None:
    default_h = text(GS / "DefaultClassInfo.h")
    assert re.search(r"#define\s+MAX_CLASS\s+8\b", default_h)
    assert "#define CLASS_RF 6" in default_h
    assert "#define CLASS_GL 7" in default_h
    assert "DB_CLASS_GL" not in default_h
    assert "m_DefaultClassInfo[MAX_CLASS]" in default_h
    print("PASS GS default-class capacity8 reserves source class7; no DB GL create symbol")

    default_cpp = text(GS / "DefaultClassInfo.cpp")
    assert "CHECK_RANGE(info.Class,MAX_CLASS) == 0" in default_cpp
    assert "this->m_DefaultClassInfo[info.Class] = info;" in default_cpp
    assert "CHECK_RANGE(Class,MAX_CLASS) == 0" in default_cpp
    print("PASS default-class loader uses reserved capacity; unverified class7 row remains absent")

    protocol = text(GS / "Protocol.cpp")
    create = protocol.split("void CGCharacterCreateRecv(", 1)[1].split("void CGCharacterDeleteRecv(", 1)[0]
    classic_db = ("DB_CLASS_DW", "DB_CLASS_DK", "DB_CLASS_FE", "DB_CLASS_MG",
                  "DB_CLASS_DL", "DB_CLASS_SU", "DB_CLASS_RF")
    for token in classic_db:
        assert token in create
    assert "DB_CLASS_GL" not in create
    print("PASS GS character-create validation enumerates only classic seven DB classes")

    object_manager = text(GS / "ObjectManager.cpp")
    info = object_manager.split("bool CObjectManager::CharacterInfoSet(", 1)[1].split("void CObjectManager::CharacterInfoSetSkill(", 1)[0]
    # The function may be ordered before/after SetSkill in this source; use a
    # direct whole-file fallback for the stable persistence assignments.
    if "DB_CLASS_FM" not in info:
        info = object_manager.split("bool CObjectManager::CharacterInfoSet(", 1)[1]
    for token in ("DB_CLASS_DW", "DB_CLASS_RF", "DB_CLASS_FM", "if (lpMsg->Class !="):
        assert token in info
    assert re.search(r"lpObj->DBClass\s*=\s*lpMsg->Class;", object_manager)
    assert re.search(r"lpObj->Class\s*=\s*lpMsg->Class\s*/\s*16;", object_manager)
    assert re.search(r"lpObj->ChangeUp\s*=\s*lpMsg->Class\s*%\s*16;", object_manager)
    print("PASS DS/GS load path persists DBClass and derives Class/ChangeUp; GL DB range is not accepted")

    ds = text(GS / "DSProtocol.cpp")
    assert "gObjectManager.CharacterInfoSet((BYTE*)lpMsg, lpMsg->index)" in ds
    assert "pMsg.Class = lpObj->DBClass;" in ds
    print("PASS DS round-trip calls CharacterInfoSet and serializes DBClass")

    server_info = text(GS / "ServerInfo.h")
    arrays = re.findall(r"\b(?:int|float|BYTE|WORD)[^;\n]*\[[^;\n]*MAX_CLASS[^;\n]*\]", server_info)
    assert len(arrays) >= 10
    print(f"PASS ServerInfo retains {len(arrays)} MAX_CLASS-indexed arrays; class expansion is cross-cutting")

    client_define = text(CLIENT / "_define.h")
    assert re.search(r"#define\s+MAX_CLASS\s+7\b", client_define)
    manager = text(CLIENT / "CharacterManager.cpp")
    assert "ChangeServerClassTypeToClientClassType" in manager
    assert "byServerClassType >> 5" in manager
    print("PASS SS6 client class capacity/conversion also remains seven-class")

    if NEWER.exists():
        newer = text(NEWER / "DefaultClassInfo.h")
        assert "CLASS_GL  = 7" in newer
        assert "DB_CLASS_GL    = 112" in newer
        assert "DB_CLASS_GL_4  = 115" in newer
        print("REFERENCE PASS newer source exposes GL=7 and DB 112..115 as a lead only")
        print("REFERENCE SCOPE: no S21 defaults/skill handlers copied; newer source is not runtime evidence")
    else:
        print("REFERENCE OPEN newer class shell is unavailable; no fallback inferred")

    print("OPEN activation: defaults, equipment/skeleton, create/login, packets, persistence and class consumers")
    print("Scope: source boundary audit only; class capacity is not DB/persistence activation")


if __name__ == "__main__":
    main()
