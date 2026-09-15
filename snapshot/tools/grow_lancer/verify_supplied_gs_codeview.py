"""Read-only CodeView provenance of the supplied protected GameServer binary."""
from hashlib import sha256
from pathlib import Path
from struct import unpack_from
from uuid import UUID

EXE = Path(r"D:\GameServer S21\GameServer\IGC.GameServer2_R.exe")
SHA = "134baebf654d103b3fd65f8262cb20d2aec5969b3c09c68e26a87f6547f77d74"
DEBUG_FILE_OFFSET = 0x50C870
DEBUG_RECORD_SIZE = 0x83
GUID = "30c7611f-171b-4f28-9b0d-8dfc4377f4aa"
AGE = 3
PDB = (r"G:\DATA\Server Suite\Development\Sources\Head (Git)\Output"
       r"\Season 20\GameServer\Release\IGC.GameServer.pdb")


def main() -> None:
    data = EXE.read_bytes()
    assert sha256(data).hexdigest() == SHA, "supplied GS EXE changed"
    record = data[DEBUG_FILE_OFFSET:DEBUG_FILE_OFFSET + DEBUG_RECORD_SIZE]
    assert len(record) == DEBUG_RECORD_SIZE and record[:4] == b"RSDS"
    assert str(UUID(bytes_le=record[4:20])) == GUID
    assert unpack_from("<I", record, 20)[0] == AGE
    path = record[24:].split(b"\x00", 1)[0].decode("ascii")
    assert path == PDB, "embedded PDB path changed"
    assert not list(EXE.parents[1].rglob("*.pdb")), "PDB now present in supplied tree; reassess"
    print(f"PASS: supplied GS SHA256 {SHA.upper()}, RSDS {GUID} age{AGE}")
    print(f"Embedded PDB path: {path}")
    print("Scope: embedded build/debug provenance only. Season20 path is not proof of actual skill-handler version; no PDB exists in the supplied GameServer tree.")


if __name__ == "__main__":
    main()
