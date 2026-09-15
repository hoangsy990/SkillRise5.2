"""Native body resource boundary audit; no new-class activation or visual proof."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
CLIENT = ROOT / 'ExMain_RISE_PC/Main5.2_RISE'


def main():
    # Preserve legacy non-UTF8 comments: only inspect the ASCII function bytes.
    data = (CLIENT / 'CharacterManager.cpp').read_bytes()
    body = data.split(b'BYTE CCharacterManager::GetSkinModelIndex(', 1)[1].split(
        b'BYTE CCharacterManager::GetStepClass(', 1)[0]
    compact = b''.join(body.split())
    assert b'byFirstClass=byClass&0x7;' in compact
    assert b'byFirstClass+(bySecondClass+byThirdClass)*MAX_CLASS' in compact
    assert b'byFirstClass+(byThirdClass*2)*MAX_CLASS' in compact
    assert b'byFirstClass==CLASS_WIZARD||byFirstClass==CLASS_KNIGHT||byFirstClass==CLASS_ELF||byFirstClass==CLASS_SUMMONER' in compact
    loader = b''.join((CLIENT / 'ZzzOpenData.cpp').read_bytes().split())
    for part, filename in [('HELM', 'Helm'), ('ARMOR', 'Armor'), ('PANTS', 'Pant'),
                           ('GLOVES', 'Glove'), ('BOOTS', 'Boot')]:
        for offset, tier in [('i', ''), ('MAX_CLASS+i', '2'), ('(MAX_CLASS*2)+i', '3')]:
            expected = ('gLoadData.AccessModel(MODEL_BODY_' + part + '+' + offset
                        + ',"Data\\\\Player\\\\","' + filename + 'Class' + tier + '",i+1);')
            assert expected.encode() in loader, expected
    assert b'nIndex=MAX_CLASS*j+i;' in loader

    def skin(base, second, third, stride):
        return base + ((second + third) if base in (0, 1, 2, 5) else 2 * third) * stride

    changed = []
    for base in range(7):
        for second, third in ((0, 0), (1, 0), (1, 1)):
            old, widened = skin(base, second, third, 7), skin(base, second, third, 8)
            if old != widened:
                changed.append((base, second, third, old, widened))
    assert len(changed) == 11
    assert skin(7, 1, 0, 8) == 7  # loader would also load second-tier slot15
    assert 8 + 7 == 15
    print('PASS: all15 body registration expressions and texture indexing checked')
    print('Boundary: MAX_CLASS7->8 renumbers11/21 tested legacy class/evolution combinations')
    print('Base7 second-bit case selects7, not newly registered tier2 slot15')
    print('These are algebra cases, not proof of valid Grow Lancer evolutions')
    print('OPEN: coordinated registry/selector/texture migration and verified GL assets')


if __name__ == '__main__':
    main()
