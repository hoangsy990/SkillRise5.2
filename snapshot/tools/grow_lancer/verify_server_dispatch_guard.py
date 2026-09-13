"""Source ordering checks; does not execute GS packets or gameplay."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
source = (ROOT / 'ExGameServer/GameServer/SkillManager.cpp').read_text(encoding='utf-8-sig')

for name, returned in (('UseAttackSkill', 'return;'),
                       ('UseDurationSkillAttack', 'return;'),
                       ('RunningSkill', 'return false;')):
    body = source.split('CSkillManager::' + name + '(', 1)[1].split('\n{', 1)[1]
    prefix = body.split('LPOBJ lpObj = &gObj[aIndex];', 1)[0]
    for field in ('m_index', 'm_skill'):
        assert '!rise::growlancer::HasProvenServerRuntimeHandler(lpSkill->' + field + ')' in prefix, name
    assert returned in prefix, name
    assert ' ||' in prefix, name
    for mutation in ('GCDurationSkillAttackSend', 'DelEffect', 'ComboTime =', 'Mana -=', 'BP -='):
        assert mutation not in prefix, (name, mutation)
    print('PASS early rejection before actor mutation/broadcast:', name)
delay = source.split('CSkillManager::CheckSkillDelay(', 1)[1].split('\n{', 1)[1].split('\n}', 1)[0]
prefix = delay.split('SKILL_INFO SkillInfo;', 1)[0]
assert '!rise::growlancer::HasProvenServerRuntimeHandler(index)' in prefix
assert 'return false;' in prefix
assert 'lpObj->' not in prefix
assert delay.index('SKILL_INFO SkillInfo;') < delay.index('lpObj->SkillDelay[index] = GetTickCount();')
print('PASS delay rejection precedes actor access, RF bypass and cooldown write')
print('Scope: source guard ordering only; live no-side-effect packet QA remains OPEN')
