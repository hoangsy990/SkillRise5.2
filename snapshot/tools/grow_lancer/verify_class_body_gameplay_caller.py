"""Scoped class7 base-body render caller; static/compile evidence only."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
src = root / 'ExMain_RISE_PC/Main5.2_RISE'
native = (src / 'ZzzCharacter.cpp').read_text(encoding='utf-8')
caller = (src / 'RISE/GrowLancerClassBodyCaller.h').read_text()
resource = (src / 'RISE/GrowLancerResources.cpp').read_text()

for row in (
    'MODEL_BODY_HELM + gCharacterManager.GetBaseClass(Class)',
    'MODEL_BODY_ARMOR + gCharacterManager.GetBaseClass(Class)',
    'MODEL_BODY_PANTS + gCharacterManager.GetBaseClass(Class)',
    'MODEL_BODY_GLOVES + gCharacterManager.GetBaseClass(Class)',
    'MODEL_BODY_BOOTS + gCharacterManager.GetBaseClass(Class)',
):
    assert row in native
assert 'nativeBodyPart >= 1 && nativeBodyPart <= 5' in caller
assert 'ChooseBaseBody(sourceClassByte, nativeBodyPart - 1' in caller
assert 'c->Class == rise::growlancer::kGrowLancerSourceBaseClass' in native
assert 'SceneFlag == MAIN_SCENE && o->Kind == KIND_PLAYER' in native
assert 'o->Type == MODEL_PLAYER && i >= BODYPART_HELM' in native
assert 'i <= BODYPART_BOOTS' in native
assert 'Type != legacyBaseType[i - BODYPART_HELM]' in native
assert 'g_pOption->GetRenderEquipMent() != 0, false' in native
assert 'choice == rise::growlancer::BaseBodyChoice::PrivateBase' in native
assert 'Models[MODEL_PLAYER].NumBones, c->Light, false' in native
assert 'Translate, GL_LINEAR, GL_REPEAT);' in native
assert 'continue;' in native.split('choice == rise::growlancer::BaseBodyChoice::PrivateBase', 1)[1].split('if (CLASS_SUMMONER', 1)[0]
assert 'model.Open2(directory, filename)' in resource
assert 'body.Transform(owner->BoneTransform' in resource
assert 'body.RenderBody(RENDER_TEXTURE' in resource

print('PASS normal class7 body slot -> native private BMD Open2/Transform/RenderBody scoped pass')
print('PASS equipped SS6 branch preserved; unavailable class7 base body does not use unregistered placeholder')
print('OPEN ingame class7 bones/material/selection/equipment/pixel parity and create/login authority')
