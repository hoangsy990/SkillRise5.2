"""Section-aware pinned config evidence, not proof of S21 handler arithmetic."""
from pathlib import Path
from decimal import Decimal
import hashlib

source = Path(r'D:\GameServer S21\Data\CalcCharacter.ini')
data = source.read_bytes()
assert hashlib.sha256(data).hexdigest().upper() == 'BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D'
section = ''
values = {}
for line in data.decode('utf-8-sig').splitlines():
    line = line.split(';', 1)[0].strip()
    if line.startswith('[') and line.endswith(']'):
        section = line[1:-1]
    elif '=' in line:
        key, value = (part.strip() for part in line.split('=', 1))
        assert (section,key) not in values
        values[section,key] = value

outgoing = ['31.24703548','34.11724265','29.22544179','33.36155585',
            '19','25.55220782','33.87089928','22.49264061']
incoming = ['14.63748459','29.05930889','41.85446802','47.13405707',
            '16.31879461','44.63967386','26.83767228']
classes = ['DW','DK','ELF','MG','DL','SU','RF','GL']
for target, expected in zip(classes, outgoing):
    key = f'GLTo{target}_PvPDamageRate'
    assert Decimal(values['PvPDamage',key]) == Decimal(expected)
    print(f'CONFIG PvPDamage/{key}={expected}')
for attacker, expected in zip(classes[:7], incoming):
    key = f'{attacker}ToGL_PvPDamageRate'
    assert Decimal(values['PvPDamage',key]) == Decimal(expected)
    print(f'CONFIG PvPDamage/{key}={expected}')
assert values['DarkSpirit','DLToGL_PvPDamageRate'] == '5.0'
assert values['PvMDamage','GLPvMDamageRate'] == '555'
assert values['PvPDamage','GLDamageStuckRate'] == '10'
native = Path(__file__).resolve().parents[2]/'ExGameServer/GameServer'
assert 'int m_DamageRateTo[MAX_CLASS][MAX_CLASS];' in (native/'ServerInfo.h').read_text()
assert 'damage = (damage * gServerInfo.m_DamageRateTo[lpObj->Class][lpTarget->Class]) / 100;' in (native/'Attack.cpp').read_text()
print('PASS 15 directed GL rates, section-separated DarkSpirit5.0, PvM555, stuck10; decimals preserved')
print('OPEN runtime S21 conversion/order and private native decimal adapter; no global matrix widening or integer truncation authorized by config evidence')
