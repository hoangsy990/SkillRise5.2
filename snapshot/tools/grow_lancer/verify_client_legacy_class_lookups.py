"""SS6 seven-column client lookup bounds; GL item/quest entitlement stays OPEN."""
from pathlib import Path
import hashlib

root = Path(__file__).resolve().parents[2]
client = root / 'ExMain_RISE_PC/Main5.2_RISE'
text = lambda name: (client / name).read_bytes().decode('utf-8', errors='surrogateescape')
assert '#define MAX_CLASS\t\t\t7' in text('_define.h')
assert 'BYTE RequireClass[MAX_CLASS]' in text('_struct.h')
assert 'BYTE    byRequestClass[MAX_CLASS]' in text('_struct.h')

inventory = text('NewUIMyInventory.cpp').split('bool CNewUIMyInventory::IsEquipable(', 1)[1]
assert inventory.index('if (byFirstClass >= MAX_CLASS)') < inventory.index(
    'pItemAttr->RequireClass[byFirstClass]')
tooltip = text('ZzzInfomation.cpp').split('bool IsRequireEquipItem(', 1)[1]
assert tooltip.index('if (byFirstClass >= MAX_CLASS)') < tooltip.index(
    'pItemAttr->RequireClass[byFirstClass]')
guild = text('UIGuildInfo.cpp').split('bool CheckUseMasterSkill(', 1)[1]
assert guild.index('if (Class >= MAX_CLASS)') < guild.index(
    'SkillAttribute[Index].RequireClass[Class]')
set_options = text('CSItemOption.cpp')
assert set_options.count('hasLegacyClassColumn && itemOption.byRequireClass[Class]') == 3
assert 'Class >= 0 && Class < MAX_CLASS' in set_options
quest = text('CSQuest.cpp')
assert quest.count('m_byClass < MAX_CLASS && pQuest->QuestAct[i].byRequestClass[m_byClass]') == 4
npc = text('NewUINPCQuest.cpp')
assert npc.count('nClass < 0 || nClass >= MAX_CLASS || !pQuest->QuestAct[i].byRequestClass[nClass]') == 2
helper = text('NewUIMuHelper.cpp')
assert helper.count('gCharacterManager.GetBaseClass(Hero->Class) < MAX_CLASS &&') == 7
assert helper.count('class_character[gCharacterManager.GetBaseClass(Hero->Class)]') == 7
assert 'BYTE class_character[MAX_CLASS]' in text('NewUIMuHelper.h')
character = text('ZzzCharacter.cpp')
assert 'Class < MAX_CLASS && gProtect->m_MainInfo.MaxAttackSpeed[Class]' in character
assert 'UINT                        MaxAttackSpeed[7]' in text('RISE/CProtect.h')
assert 'if (Class < 0 || Class >= MAX_CLASS)' in text('ZzzInfomation.cpp').split(
    'void CreateClassAttribute(', 1)[1].split('void CreateClassAttributes(', 1)[0]
assert 'if (Class >= MAX_CLASS)' in text('ZzzInfomation.cpp').split(
    'void CHARACTER_MACHINE::SetCharacter(', 1)[1].split('void CHARACTER_MACHINE::', 1)[0]

backup = (root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/EncodingBackup/NewUINPCQuest.cpp.cp949')
assert hashlib.sha256(backup.read_bytes()).hexdigest().upper() == (
    '68424FE0E58558DF3B5559BB2009292492685431633971A8F52847938930A476')
assert backup.read_bytes().decode('cp949').replace('\r\n', '\n') == text('NewUINPCQuest.cpp').replace('\r\n', '\n').replace(
    'nClass < 0 || nClass >= MAX_CLASS || !pQuest->QuestAct[i].byRequestClass[nClass]',
    '!pQuest->QuestAct[i].byRequestClass[nClass]')
mu_backup = root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/EncodingBackup/NewUIMuHelper.cpp.original'
assert hashlib.sha256(mu_backup.read_bytes()).hexdigest().upper() == (
    '24C54E600BAEFC0222F826A445ADA6E11BACFA77D44CF4BB58736002C57DD9B3')
print('PASS fixed SS6 seven-class client arrays bounded at item, quest, helper, attack-speed and class-default consumers')
print('OPEN source-backed GL item/quest requirement overlay and native class7 ingame acceptance')
