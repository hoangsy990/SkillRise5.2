"""Audit speed divisor packet/table capacity without inventing class7 rate."""

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / "ExMain_RISE_PC/Main5.2_RISE"
SERVER = ROOT / "ExGameServer/GameServer"


def read(path: Path) -> str:
    return path.read_text(encoding="latin-1")


def main() -> None:
    client_header = read(CLIENT / "WSclient.h")
    packet = client_header[client_header.index("struct GetValueSpeedAttack"):]
    assert "BYTE  ThisValue[14];" in packet[:packet.index("};")]
    client_table = read(CLIENT / "RISE/MainProtocol.h")
    assert "BYTE  ThisValue[14];" in client_table
    receiver = read(CLIENT / "RISE/MainProtocol.cpp")
    assert "for (int RISE = 0; RISE < 14; RISE++)" in receiver
    calc = read(CLIENT / "ZzzInfomation.cpp")
    speed = calc[calc.index("void CHARACTER_MACHINE::CalculateAttackSpeed()"):]
    speed = speed[:speed.index("ITEM* r =")]
    assert "case CLASS_RAGEFIGHTER: index = 12; break;" in speed
    assert not re.search(r"case\s+CLASS_GROWLANCER\b", speed)
    assert "speed[index + 1]" in speed

    gs_header = read(SERVER / "Protocol.h")
    gs_packet = gs_header[gs_header.index("struct PMSG_ATTACK_SEND"):]
    assert "BYTE  ThisValue[14];" in gs_packet[:gs_packet.index("};")]
    sender = read(SERVER / "Protocol.cpp")
    sender = sender[sender.index("void GCSendAttackSend("):]
    sender = sender[:sender.index("void GCAutoMove(")]
    assert "pMsg.header.set(0xF3, 0xED, sizeof(pMsg));" in sender
    for slot in range(14):
        assert f"pMsg.ThisValue[{slot}] = " in sender
    assert "pMsg.ThisValue[14]" not in sender
    assert "DataSend(lpObj->Index, (BYTE*)&pMsg, pMsg.header.size);" in sender
    gs = read(SERVER / "ObjectManager.cpp")
    assert "GCSendAttackSend(lpObj);" in gs
    assert not re.search(r"\bm_GL(Physi|Magic)SpeedConstA\b", read(SERVER / "ServerInfo.h"))
    print("PASS native F3:ED speed-DIVISOR table/packet exactly 14 bytes, class0..6 pairs0..13")
    print("GAP class7 pair14/15 and GS config absent; unrelated join final DWORD speed packet is separate")
    print("Scope: capacity/protocol audit only; no slot-count increase, packet expansion or guessed rate")


if __name__ == "__main__":
    main()
