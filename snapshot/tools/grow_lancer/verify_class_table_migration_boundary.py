"""Current migration hazards, NOT validation that class7 is implemented."""
from pathlib import Path
import re
root = Path(__file__).resolve().parents[2]/'ExGameServer/GameServer'
defaults = (root/'DefaultClassInfo.h').read_text()
assert re.search(r'#define\s+MAX_CLASS\s+7\b', defaults)
skills = (root/'SkillManager.cpp').read_text()
prefix = skills.split('info.Damage = lpMemScript->GetAsNumber();', 1)[1].split('ReadLegacySkillClassColumns', 1)[0]
fields = ['Damage'] + re.findall(r'info\.(\w+) = lpMemScript->GetAsNumber\(\);', prefix)
assert fields == ['Damage','Mana','BP','Range','Radio','Delay','Type','Effect',
                  'RequireLevel','RequireEnergy','RequireLeadership','RequireKillCount','RequireGuildStatus']
assert 'ReadLegacySkillClassColumns(info.RequireClass,' in skills
assert '[lpMemScript]() { return lpMemScript->GetAsNumber(); }' in skills
helper = (root.parents[1]/'Shared/LegacySkillClassColumns.h').read_text()
assert 'i < 7' in helper and 'i < Capacity' in helper and 'columns[i] = 0;' in helper
items = (root/'ItemManager.cpp').read_text()
assert re.search(r'if\s*\(section != 14\)\s*\{\s*rise::growlancer::ReadLegacyClassColumns\(info.RequireClass,\s*\[lpMemScript\]\(\)\s*\{ return lpMemScript->GetAsNumber\(\); \}\);\s*\}', items)
assert 'if (section <= 11 || section == 13)' in items
for filename, preceding, following in (
    ('Quest.cpp', 'info.RequireMaxLevel = lpMemScript->GetAsNumber();', 'this->SetInfo(info);'),
    ('MasterSkillTree.cpp', 'info.RequireSkill[1] = lpMemScript->GetAsNumber();', 'this->m_MasterSkillTreeInfo.insert('),
    ('QuestObjective.cpp', 'info.RequireState = lpMemScript->GetAsNumber();', 'this->SetInfo(info);'),
    ('QuestReward.cpp', 'info.RequireState = lpMemScript->GetAsNumber();', 'this->SetInfo(info);'),
    ('QuestWorld.cpp', 'info.RequireCooldown = lpMemScript->GetAsNumber();', 'this->SetInfo(info);'),
    ('QuestWorldObjective.cpp', 'info.RequireState = lpMemScript->GetAsNumber();', 'this->SetInfo(info);'),
    ('QuestWorldReward.cpp', 'info.RequireState = lpMemScript->GetAsNumber();', 'this->SetInfo(info);'),
    ('ItemBagEx.cpp', 'info.OptionValue = lpMemScript->GetAsNumber();', 'std::map<int,ITEM_BAG_EX_INFO>::iterator'),
):
    source = (root/filename).read_text()
    begin = source.index(preceding)
    end = source.index(following, begin)
    block = source[begin:end]
    assert 'ReadLegacyClassColumns(info.RequireClass,' in block
    assert 'MAX_CLASS' not in block
    assert block.count('GetAsNumber()') == 2 # preceding field plus callback
bag = (root/'ItemBagEx.cpp').read_text()
section4 = bag.split('else if(section == 4)',1)[1].split('else if(section >= 5)',1)[0]
assert 'ReadLegacyClassColumns(info.RequireClass,' in section4
assert bag.count('ReadLegacyClassColumns(') == 1
server = (root/'ServerInfo.cpp').read_text()
rows = re.findall(r'm_DamageRateTo\[(\d+)\]\[(\d+)\]\s*=\s*GetPrivateProfileInt', server)
pairs = {(int(a), int(b)) for a,b in rows}
assert pairs == {(a,b) for a in range(7) for b in range(7)}, pairs
loader = (root/'DefaultClassInfo.cpp').read_text()
assert 'memset(this->m_DefaultClassInfo,0,sizeof(this->m_DefaultClassInfo));' in loader
assert 'if(CHECK_RANGE(info.Class,MAX_CLASS) == 0)' in loader
print('PASS legacy skill reader decoupled from capacity: seven tokens only, extra columns zero;49 explicit damage cells;default rows zero initialized')
print('PASS thirteen metadata fields in full skill fixture match actual loader order')
print('PASS item reader preserves section14 exclusion and preceding section-specific token step')
print('PASS eight quest/master/bag reader placements preserve preceding fields and next-row boundary; bag section4 only')
print('OPEN class activation: new class package,15 matrix cells/default stats and other class-dependent loaders')
