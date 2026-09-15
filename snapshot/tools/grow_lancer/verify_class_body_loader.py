"""Native body loader source contract; failure/runtime QA remains separate."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
source = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp').read_text()
body = source.split('bool EnsureClassBodyModel(',1)[1].split('bool EnsureModel(',1)[0]
for expected in ('HasVerifiedBaseBodyMaterial(sourceClassByte)',
                 'part >= kClassBodyModelCount', '!wglGetCurrentContext()',
                 'ClassBodyModelId(part, MAX_MODELS)', 'model.Open2(directory, filename)',
                 'BaseClassMaterial(descriptor.part, mesh)',
                 'model.IndexTexture[mesh] = BITMAP_SKIN;',
                 'model.m_bCompletedAlloc = false;'):
    assert expected in body, expected
assert body.index('model.IndexTexture[mesh] = BITMAP_SKIN;') < body.index('Bitmaps.LoadImageFile(')
assert body.index('if (texture == BITMAP_UNKNOWN)') < body.index('model.IndexTexture[mesh] = texture;')
assert 'force' not in body and 'UnLoadImageFile' not in body
header = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.h').read_text()
assert 'kLastModel = kBrecheOwnerTwilight01Model' in header
assert 'kBrecheEmitterModel = MAX_MODELS + 30' in header
assert 'kBrecheOwnerTwilight01Model = MAX_MODELS + 36' in header
assert 'return (modelId >= kFirstModel && modelId <= kLastVisibleModel) ||' in source
assert 'modelId == kBrecheOwnerWindModel;' in source
print('PASS base gate, native loader, sentinel-before-acquisition, rollback reset, separate effect range')
print('OPEN actual adapter invocation, failure injection, cache/sampler contract and class rendering')
