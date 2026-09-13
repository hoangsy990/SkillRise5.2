"""Existing native class wire algebra, not Grow Lancer activation or DB parity."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

def compact(path):
    return ''.join((ROOT / path).read_text(errors='strict').split())

def main():
    client = compact('ExMain_RISE_PC/Main5.2_RISE/CharacterManager.cpp')
    server = compact('ExGameServer/GameServer/DSProtocol.cpp')
    objects = compact('ExGameServer/GameServer/ObjectManager.cpp')
    assert '(((byServerClassType>>4)&0x01)<<3)|(byServerClassType>>5)|(((byServerClassType>>3)&0x01)<<4)' in client
    assert 'pMsg.Class=(lpMsg->Class%16)*16;' in server
    assert 'pMsg.Class-=(pMsg.Class/32);' in server
    assert 'pMsg.Class+=(lpMsg->Class/16)*32;' in server
    assert 'lpObj->DBClass=lpMsg->Class;' in objects
    assert 'lpObj->Class=lpMsg->Class/16;' in objects
    assert 'lpObj->ChangeUp=lpMsg->Class%16;' in objects
    for base in range(8):
        for evolution in range(3):
            db = base * 16 + evolution
            wire = (db % 16) * 16
            wire -= wire // 32
            wire += (db // 16) * 32
            client_class = (((wire >> 4) & 1) << 3) | (wire >> 5) | (((wire >> 3) & 1) << 4)
            assert client_class & 7 == base
            assert (db // 16, db % 16) == (base, evolution)
            assert (client_class & 24) == (0,8,24)[evolution]
            assert wire <= 255
    print('PASS24 native-format algebra cases (8 base values x3 evolution values)')
    print('Base7 implies native DB112 and wire224 for unevolved state; not proof of S21 DB encoding')
    print('OPEN: class7 activation, evolution eligibility, defaults, stored procedure and persistence QA')

if __name__ == '__main__':
    main()
