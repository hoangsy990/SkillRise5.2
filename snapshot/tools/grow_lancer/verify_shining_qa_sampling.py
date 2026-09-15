"""Instrumentation scope checks, not execution/visual evidence."""
from pathlib import Path
root = Path(__file__).resolve().parents[2] / 'ExMain_RISE_PC/Main5.2_RISE/RISE'
qa = (root/'GrowLancerRuntimeQA.cpp').read_text()
# Stop at the next function.  The body-texture probe intentionally allocates
# a private BMD table and lives after this callback; including it here made
# the old guard reject its own unrelated ``new``/OpenGL QA code.
body = qa.split('void RecordShiningQASample(', 1)[1].split('int RunBodyTextureProbeQA()', 1)[0]
assert 'controller.Owner != &Hero->Object' in body
assert 'controller.SubType != 0' in body
assert 'gShiningSampleCount >= 512' in body
assert 'gShiningDropped != 0xffffffffu' in body
for forbidden in ('AppendQALog', 'fopen', 'CreateEffect', 'new ', 'malloc', 'SetAction'):
    assert forbidden not in body
assert 'gShiningSamples[512]' in qa
assert 'kRuntimeQASkills[gRuntimeQASkill] == 277' in qa
assert qa.index('#ifdef RISE_GROW_LANCER_RUNTIME_QA') < qa.index('gShiningSamples[512]')
runtime = (root/'GrowLancerEffectRuntime.cpp').read_text()
assert '#ifdef RISE_GROW_LANCER_RUNTIME_QA\n        RecordShiningQASample(effect, animationFactor);\n#endif' in runtime
print('PASS QA-only bounded local subtype0 pre-update sampler; no allocation/disk write/game mutation inside callback')
print('OPEN actual sample capture; use one isolated cast, wait for effects to expire, then F11; overlapping roots are not distinguished')
