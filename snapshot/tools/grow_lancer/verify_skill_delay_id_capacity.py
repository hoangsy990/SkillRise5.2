"""Pin S21 active ID 895 against isolated 5.2 cooldown capacity/slot split.

The check does not claim a Grow Lancer server cast handler or master activation.
"""
import hashlib
import re
from pathlib import Path


root = Path(__file__).resolve().parents[2]
table = Path(r'D:\GameServer S21\Data\Skills\SkillList.xml')
raw = table.read_bytes()
assert hashlib.sha256(raw).hexdigest() == (
    '3e238c786ecab3445a0db4756fe3d2a3923fbc0594506bb9c3ff206020a7e0a0'
)
row = re.search(rb'<Skill\s+Index="895"[^>]*\s/>', raw)
assert row and b'Name="Wrath Strengthener"' in row.group(0)
assert b'GrowLancer="3"' in row.group(0)
assert b'IconNumber="408"' in row.group(0)

header = (root / 'ExGameServer/GameServer/SkillManager.h').read_text()
editor_count = int(re.search(r'^#define MAX_SKILL (\d+)$', header, re.M).group(1))
delay_count = int(re.search(r'^#define MAX_SKILL_DELAY_ID (\d+)$', header, re.M).group(1))
owned_count = int(re.search(r'^#define MAX_SKILL_LIST (\d+)$', header, re.M).group(1))
master_slots = int(re.search(r'^#define MAX_MASTER_SKILL_LIST (\d+)$', header, re.M).group(1))
assert (editor_count, delay_count, owned_count, master_slots) == (629, 896, 60, 120)

allocator = (root / 'ExGameServer/GameServer/MemoryAllocatorInfo.cpp').read_text()
user = (root / 'ExGameServer/GameServer/User.cpp').read_text()
manager = (root / 'ExGameServer/GameServer/SkillManager.cpp').read_text()
assert 'm_SkillDelay = new DWORD[MAX_SKILL_DELAY_ID];' in allocator
assert 'for (int n = 0; n < MAX_SKILL_DELAY_ID; n++)' in user
check = manager.split('bool CSkillManager::CheckSkillDelay(LPOBJ lpObj, int index)', 1)[1].split('\nbool CSkillManager::CheckSkillTarget', 1)[0]
assert check.index('index < 0 || index >= MAX_SKILL_DELAY_ID') < check.index('lpObj->SkillDelay[index]')
assert 'SkillDelay[index] = GetTickCount();' in check

client_header = (root / 'ExMain_RISE_PC/Main5.2_RISE/_define.h').read_text(encoding='latin-1')
assert re.search(r'^#define MAX_SKILLS\s+650$', client_header, re.M)
client_loader = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzInfomation.cpp').read_text(encoding='latin-1')
assert 'fread(Buffer, Size * MAX_SKILLS, 1, fp);' in client_loader
assert 'GenerateCheckSum2(Buffer, Size * MAX_SKILLS, 0x5A18)' in client_loader
runtime_cap = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerSkillIdCapacity.h').read_text()
assert 'kSkillAttributeIdCapacity = 896;' in runtime_cap
winmain = (root / 'ExMain_RISE_PC/Main5.2_RISE/Winmain.cpp').read_text(encoding='latin-1')
assert 'new SKILL_ATTRIBUTE[rise::growlancer::kSkillAttributeIdCapacity]' in winmain
assert 'sizeof(SKILL_ATTRIBUTE) *\n\t\trise::growlancer::kSkillAttributeIdCapacity' in winmain
assert 'iSkillIdx >= rise::growlancer::kSkillAttributeIdCapacity' in client_loader
manager_client = (root / 'ExMain_RISE_PC/Main5.2_RISE/SkillManager.cpp').read_text(encoding='latin-1')
assert 'SkillType >= rise::growlancer::kSkillAttributeIdCapacity' in manager_client
master_ui = (root / 'ExMain_RISE_PC/Main5.2_RISE/NewUIMasterSkillTree.cpp').read_text(encoding='latin-1')
assert 'index < 0 || index >= rise::growlancer::kSkillAttributeIdCapacity' in master_ui

print('PASS S21 active master895 fits server cooldown ID array [0,896)')
print('PASS client runtime attribute lookup [0,896), fixed 650-record Skill.bmd and owned slots unchanged')
print('PASS 60 owned/120 master slots and 629 editor count unchanged')
print('OPEN master895 attribute overlay, MuHelper selection guard, class7 handler, real cast/cooldown QA')
