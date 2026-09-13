"""Read-only supplied S21 class namespaces and default-data provenance."""
import hashlib
from pathlib import Path
import re
import xml.etree.ElementTree as ET
from fractions import Fraction

ROOT = Path(r'D:\GameServer S21\Data')

def pinned(name, digest):
    data = (ROOT / name).read_bytes()
    assert hashlib.sha256(data).hexdigest().upper() == digest, name
    return data

def main():
    lua = pinned('Scripts/Character/CalcCharacter.lua',
        '9DA5000449CCEC0B3DA0CD2ADE11E55863B5CC0C56EC4063C5A64F78414C0A70').decode('utf-8-sig')
    assert re.search(r'^CLASS_GROWLANCER\s*=\s*7\s*$',lua,re.M)
    block = lua.split('function GrowLancerDamageCalc(',1)[1].split('\nend',1)[0]
    for hand in ('Left','Right'):
        assert f'AttackDamageMin{hand} = Strength / 8 + Dexterity / 10' in block
        assert f'AttackDamageMax{hand} = Strength / 4 + Dexterity / 6' in block
    branches = {
        'CalcAttackSpeed': ('AttackSpeed = Dexterity / 20', 'MagicSpeed = Dexterity / 20'),
        'CalcAttackSuccessRate_PvM': ('AttackSuccessRate = TotalLevel * 5 + Dexterity * 1.25 + Strength / 4',),
        'CalcDefenseSuccessRate_PvM': ('DefenseSuccessRate = Dexterity / 4',),
        'CalcDefense': ('Defense = Dexterity / 7',),
        'CalcAttackSuccessRate_PvP': ('AttackRate = Dexterity * 2.5 + 3 * TotalLevel',),
        'CalcDefenseSuccessRate_PvP': ('DefenseRate = Dexterity / 5 + 2 * TotalLevel',),
        'ElementalDamageCalc': ('MinDamage = ItemMinDamage + (Dexterity / 5)',
                               'MaxDamage = ItemMaxDamage + (Dexterity / 3)'),
        'ElementalAttackRateCalc_MvP': ('AttackSuccessRate = (3 * Dexterity / 2) + (5 * TotalLevel) + (Strength / 4)',),
        'ElementalAttackRateCalc_PvP': ('AttackSuccessRate = (3 * Dexterity / 2) + (5 * TotalLevel) + (Strength / 4)',),
        'ElementalDefenseCalc': ('Defense = (Strength / 9) + (Dexterity / 9)',),
        'ElementalDefenseRateCalc': ('DefenseRate = Dexterity / 3',),
        'CalcAbilityGauge': ('AG = (Strength * 0.15) + (Dexterity * 0.2) + (Vitality * 0.3) + (Energy * 1.0)',),
    }
    for name, expressions in branches.items():
        function = lua.split('function '+name+'(',1)[1].split('\nend',1)[0]
        selected = function.split('elseif(Class == CLASS_GROWLANCER) then',1)[1].split('elseif',1)[0]
        if any('TotalLevel' in expression for expression in expressions):
            assert 'local TotalLevel = NormalLevel + MasterLevel' in function, name
        for expression in expressions:
            assert expression in [line.strip() for line in selected.splitlines()], (name, expression)
    # Mathematical counterexample only; this does not execute Lua or its GS bridge.
    raw_min = Fraction(7,8) + Fraction(9,10)
    assert raw_min == Fraction(71,40)
    assert int(raw_min) == 1 and 7//8 + 9//10 == 0
    # Comments describe eligibility, but the Lua bodies do not enforce it.
    # Keep the tuple order explicit: it differs from the argument order.
    for name, percent in (('CalcTwoSameWeaponBonus', 65),
                          ('CalcTwoDifferentWeaponBonus', 55)):
        function = lua.split('function '+name+'(', 1)[1].split('\nend', 1)[0]
        body = function.split('\n', 1)[1]
        assert not re.search(r'\bClass\b', body), name
        lines = [line.strip() for line in body.splitlines()]
        for bound in ('Min', 'Max'):
            for hand in ('Left', 'Right'):
                assert f'OutDamage{bound}{hand} = Atk{bound}{hand} * {percent} / 100' in lines
        assert 'return OutDamageMinLeft, OutDamageMinRight, OutDamageMaxLeft, OutDamageMaxRight' in lines
    ini = pinned('CalcCharacter.ini',
        'BE300D43E4849FE2445C0DF3BDDEAA672F6E07FFBCEEF963F6FA1C9D2F9A631D').decode('utf-8-sig')
    assert 'MuOnline.dbo.DefaultClassType' in ini
    for field in ('LevelLife','LevelMana','VitalityToLife','EnergyToMana'):
        assert '['+field+']' in ini
    xml = pinned('StatSpecialize.xml',
        'D79F1CBBE8A6FD7207499CC5198E2F0E99A5BE06E2607C0A8B4879A3378E9AC9')
    root = ET.fromstring(xml)
    classes = [node for node in root.iter('Class') if node.get('ID') == '8']
    assert len(classes)==1
    options = classes[0].findall('Option')
    assert [(node.get('StatType'),node.get('Index')) for node in options]==[('1','1'),('2','4')]
    print('PASS source profiles: Lua base7, specialization row8, four attack expressions')
    print(f'PASS {len(branches)} class branches / {sum(map(len, branches.values()))} expressions and integer-division counterexample; not Lua execution')
    print('PASS two weapon-bonus bodies / eight expressions / return ordering; caller eligibility remains OPEN')
    print('OPEN defaults: referenced DB DefaultClassType values not supplied or inferred')
    print('Scope excludes executing Lua, rounding, live server dispatch and stat application')

if __name__ == '__main__':
    main()
