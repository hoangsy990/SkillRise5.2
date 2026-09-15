"""Pin native caller hazards before wiring private body IDs. Not runtime QA."""
from pathlib import Path
root = Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE'
obj = (root/'ZzzObject.cpp').read_text(errors='replace')
part = obj.split('void RenderPartObject(OBJECT*', 1)[1].split('float AmbientShadowAngle', 1)[0]
for token in ('b->HideSkin = HideSkin;', 'b->BodyScale = o->Scale;',
              'b->Transform(o->BoneTransform', 'g_CMonkSystem.RageFighterEffect(o, Type)',
              'RenderPartObjectEffect(o, Type', 'DeleteCloth(NULL, o, p)'):
    assert token in part, token
monk = (root/'MonkSystem.cpp').read_text(errors='replace')
effect = monk.split('bool CMonkSystem::RageFighterEffect(', 1)[1].split('void CMonkSystem::SetDarksideTargetIndex', 1)[0]
assert 'PLAYER_SKILL_DARKSIDE_READY' in effect
assert 'PLAYER_SKILL_ATT_UP_OURFORCES' in effect
assert 'GetBaseClass' not in effect and 'CLASS_RAGEFIGHTER' not in effect
assert '_pObj->Angle[2] = 45.0f;' in effect
body = obj.split('void RenderPartObjectBody(BMD*', 1)[1].split('void RenderPartObjectBodyColor(', 1)[0]
assert 'b->RenderBody(RenderType, Alpha, o->BlendMesh, o->BlendMeshLight, o->BlendMeshTexCoordU, o->BlendMeshTexCoordV);' in body
print('PASS native dispatch boundary audit: action-only Monk interception, cloth side effects, generic body default')
print('OPEN complete ordinary path mapping; do not claim RenderPartObject is a side-effect-free draw API')
