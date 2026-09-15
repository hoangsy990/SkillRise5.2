"""Static boundary for the opt-in Magic Pin raster diagnostic, not visual QA."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
qa = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerRuntimeQA.cpp').read_text()
runtime = (root / 'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerEffectRuntime.cpp').read_text()
native = (root / 'ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp').read_text()

begin = qa.split('unsigned BeginMagicPinFootSamplesQA(', 1)[1].split(
    'void EndMagicPinFootSamplesQA(', 1)[0]
end = qa.split('void EndMagicPinFootSamplesQA(', 1)[1].split(
    'void ResetBrecheCasterRenderQA(', 1)[0]
assert begin.index('RISE_GL_MAGIC_RASTER_QA') < begin.index('glGenQueries')
assert begin.index('GMMeshShader->FlushAllMesh();') < begin.index('glBeginQuery(')
assert end.index('GMMeshShader->FlushAllMesh();') < end.index('glEndQuery(')
assert begin.count('GMMeshShader->FlushAllMesh();') == 1
assert end.count('GMMeshShader->FlushAllMesh();') == 1
assert 'if (OGL330::IsShader() && GMMeshShader)' in begin
assert 'shaderBackend = OGL330::IsShader() ? 1 : 0;' in end
assert 'if (shaderBackend && GMMeshShader)' in end
assert 'shaderQueueFlushed=1' in end
assert 'candidate=%s' in end
assert 'angular-strip' in end and 'annular-root' in end
assert 'effect.Type == kMagicPin01Model ? 0' in begin
assert 'effect.Type == kMagicPinRootModel ? 1' in begin
assert 'gMagicRasterProbeCount[modelSlot] >= 8' in begin
for field in ('vao=%u', 'indices=%u', 'program=%d', 'depth=%d', 'cull=%d'):
    assert field in end

render = runtime.split('const unsigned magicRasterQuery', 1)[1].split(
    'EndMagicPinFootSamplesQA(effect, magicRasterQuery);', 1)[0]
assert render.index('BeginMagicPinFootSamplesQA(effect)') < render.index(
    'model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT')
assert 'effect.Type == kMagicPinRootModel' in render
assert '#ifdef RISE_GROW_LANCER_RUNTIME_QA' in runtime.split(
    'const unsigned magicRasterQuery', 1)[0][-50:]
assert native.index('GMMeshShader->AddMeshCommand(this, meshIndex') < native.index(
    'GMMeshShader->FlushAllMesh();')
print('PASS opt-in two-model Magic Pin query excludes prior shader queue and flushes its draw before GL end')
print('OPEN: shader/backend state, ingame samples, footprint pixels and S21 visual parity')
