"""Pin S21 master icon page/UV route and private 5.2 mapping for ID 895.

Static asset/renderer evidence only. No character/master-tree activation claim.
"""
import hashlib
import re
from io import BytesIO
from pathlib import Path

from capstone import Cs, CS_ARCH_X86, CS_MODE_32
from PIL import Image, ImageStat
from disasm_s21_window import DUMP, IMAGE_BASE


root = Path(__file__).resolve().parents[2]
source = Path(r'D:\MU FICA Season 21\Data\Interface\new_Master_Icon.OZJ')
disabled_source = Path(r'D:\MU FICA Season 21\Data\Interface\new_Master_non_Icon.OZJ')
base_source = Path(r'D:\MU FICA Season 21\Data\Interface\newui_skill00.OZJ')
master_raw = source.read_bytes()
assert hashlib.sha256(master_raw).hexdigest() == (
    'df3d1f863741e720efc7b9ecc90117bb1be9ca49852a86420fa8ca3e396d9f74'
)
master = Image.open(BytesIO(master_raw[24:])).convert('RGB')
disabled_raw = disabled_source.read_bytes()
assert hashlib.sha256(disabled_raw).hexdigest() == (
    '9fbd4b584c260a97b39e7c5fb93473879ecf96b8646396288bc4542de36b9d5a'
)
disabled = Image.open(BytesIO(disabled_raw[24:])).convert('RGB')
base = Image.open(BytesIO(base_source.read_bytes()[24:])).convert('RGB')
assert master.size == disabled.size == base.size == (512, 512)

table = Path(r'D:\GameServer S21\Data\Skills\SkillList.xml').read_bytes()
assert hashlib.sha256(table).hexdigest() == (
    '3e238c786ecab3445a0db4756fe3d2a3923fbc0594506bb9c3ff206020a7e0a0'
)
row = re.search(rb'<Skill\s+Index="895"[^>]*\s/>', table)
assert row
for token in (b'Name="Wrath Strengthener"', b'GrowLancer="3"',
              b'Group="354"', b'Rank="4"', b'IconNumber="408"',
              b'Brand="278"'):
    assert token in row.group(0), token

data = DUMP.read_bytes()
assert hashlib.sha256(data).hexdigest() == (
    '6422cb4eba9432130eb247b47723ea6fc0014f5100ea0c6e63db8350f9275637'
)
decoder = Cs(CS_ARCH_X86, CS_MODE_32)
def at(start, end):
    rows = list(decoder.disasm(data[start - IMAGE_BASE:end - IMAGE_BASE], start))
    assert rows and rows[-1].address + rows[-1].size == end
    return {i.address: (i.mnemonic, i.op_str) for i in rows}

rows = at(0xC46360, 0xC463D6)
rows.update(at(0xC46401, 0xC4642D))
secondary_rows = at(0xBCF21E, 0xBCF2F8)
for address, expected in {
    0xC46360: ('mov', 'eax, dword ptr [eax + 0x8c]'),
    0xC46367: ('mov', 'ecx, 0x1c2'),
    0xC4636C: ('idiv', 'ecx'),
    0xC46374: ('mov', 'eax, dword ptr [eax + 0x8c]'),
    0xC463A6: ('push', '0x19'),
    0xC463A9: ('idiv', 'ecx'),
    0xC463CC: ('idiv', 'ecx'),
    0xC46418: ('add', 'eax, 0x7e18'),
    0xC46425: ('add', 'eax, 0x7e1a'),
}.items():
    assert rows[address] == expected, hex(address)
for address, expected in {
    0xBCF221: ('mov', 'eax, dword ptr [eax + 0x8c]'),
    0xBCF228: ('mov', 'ecx, 0x1c2'),
    0xBCF235: ('mov', 'eax, dword ptr [eax + 0x8c]'),
    0xBCF24D: ('idiv', 'ecx'),
    0xBCF267: ('idiv', 'ecx'),
    0xBCF2EA: ('add', 'eax, 0x7e18'),
    0xBCF2F0: ('call', '0x18ea880'),
}.items():
    assert secondary_rows[address] == expected, hex(address)

group = 354
assert group // 450 == 0
page_cell = group % 450
x, y = (page_cell % 25) * 20, (page_cell // 25) * 28
assert (x, y) == (80, 392) and x + 20 <= 512 and y + 28 <= 512
master_mean = ImageStat.Stat(master.crop((x, y, x + 20, y + 28))).mean
base_mean = ImageStat.Stat(base.crop((x, y, x + 20, y + 28))).mean
assert max(master_mean) - min(master_mean) > 20
assert max(base_mean) - min(base_mean) < 1

resources = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp').read_text()
ui = (root / 'ExMain_RISE_PC/Main5.2_RISE/NewUIMasterSkillTree.cpp').read_text()
list_ui = (root / 'ExMain_RISE_PC/Main5.2_RISE/NewUIMainFrameWindow.cpp').read_text()
ids = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.h').read_text()
assert 'kWrathMasterIconAtlasBitmap = 32984' in ids
assert 'kWrathMasterDisabledIconAtlasBitmap = 32985' in ids
assert 'kWrathMasterSkillGroup = 354' in ids
assert 'master.SkillGroup = kWrathMasterSkillGroup;' in resources
assert 'master.Magic_Icon = kWrathMasterIconNumber;' in resources
assert 'master.SkillUseType = SKILL_USE_TYPE_MASTERACTIVE;' in resources
assert 'kWrathMasterIconAtlasBitmap, GL_LINEAR' in ui
assert 'kWrathMasterDisabledIconAtlasBitmap, GL_LINEAR' in ui
assert 'Skill == rise::growlancer::kWrathMasterSkillId ?' in ui
assert 'DeleteBitmap(rise::growlancer::kWrathMasterIconAtlasBitmap, 0);' in ui
assert 'DeleteBitmap(rise::growlancer::kWrathMasterDisabledIconAtlasBitmap, 0);' in ui
assert 'bySkillType == rise::growlancer::kWrathMasterSkillId ?' in list_ui
assert 'rise::growlancer::kWrathMasterDisabledIconAtlasBitmap : BITMAP_INTERFACE_MASTER_BEGIN + 3' in list_ui
assert 'rise::growlancer::kWrathMasterIconAtlasBitmap : BITMAP_INTERFACE_MASTER_BEGIN + 2' in list_ui
stage = (root / 'tools/grow_lancer/stage_circle_preview_qa_candidate.ps1').read_text()
assert "masterAssetHash = 'DF3D1F863741E720EFC7B9ECC90117BB1BE9CA49852A86420FA8CA3E396D9F74'" in stage
assert 'Master\\new_Master_Icon.OZJ' in stage
assert "masterDisabledHash = '9FBD4B584C260A97B39E7C5FB93473879ECF96B8646396288BC4542DE36B9D5A'" in stage
inactive = (root / 'ExMain_RISE_PC/Tests/GrowLancerBuild/Client/Data/RISE/'
            'GrowLancer/Master/new_Master_Icon.OZJ')
assert hashlib.sha256(inactive.read_bytes()).hexdigest() == hashlib.sha256(master_raw).hexdigest()
inactive_disabled = inactive.with_name('new_Master_non_Icon.OZJ')
assert hashlib.sha256(inactive_disabled.read_bytes()).hexdigest() == hashlib.sha256(disabled_raw).hexdigest()
assert 'inactiveStageVerified": true' in (root / 'GrowLancer/asset_provenance.json').read_text()

print('PASS S21 master895 Group354 -> page0 0x7E18, UV cell(80,392,20,28)')
print('PASS source page0 atlas color differs from grayscale ordinary-skill cell354')
print('PASS isolated 5.2 master-tree atlas/attribute route and hash-pinned inactive asset')
print('PASS second S21 Group/page renderer and isolated 5.2 master skill-list normal/disabled route')
print('OPEN live 5.2 master-tree/list pixels, exact S21 list icon path attribution, class7/server cast')
