"""Source wiring check only; actual owner submission QA is separate."""
from pathlib import Path
root = Path(__file__).resolve().parents[2]
src = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp').read_text()
body = src.split('BaseBodySubmitResult SubmitClassBaseBody(',1)[1].split('bool EnsureModel(',1)[0]
for token in ('HasVerifiedBaseBodyMaterial(sourceClassByte)', 'part >= kClassBodyModelCount',
              'hideSkin || owner->Alpha <= 0.01f', '!owner->BoneTransform',
              'boneCount < kBaseClassBodies[part].boneCount', 'body.BodyScale = owner->Scale;',
              'VectorCopy(owner->Position, body.BodyOrigin);', 'VectorCopy(light, body.BodyLight);',
              'body.Transform(owner->BoneTransform', 'body.RenderBody(RENDER_TEXTURE',
              '~BoneScaleScope() { BoneScale = previous; }'):
    assert token in body, token
assert body.index('BaseBodySubmitResult::Skipped') < body.index('EnsureClassBodyModel(')
assert 'RenderPartObject(' not in body and 'RageFighterEffect(' not in body
print('PASS scoped source/part/bone/hide guards and native Transform/RenderBody reuse')
print('OPEN runtime owner submission, hide/alpha and lighting/selection/shadow parity')
