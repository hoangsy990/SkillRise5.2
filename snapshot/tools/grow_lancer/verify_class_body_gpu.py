"""Scoped GPU adapter source contract, NOT a GPU runtime/visual test."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
src = (root/'ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerResources.cpp').read_text()
gpu = src.split('static bool EnsureClassBodyGpu(', 1)[1].split('bool EnsureClassBodyModel(', 1)[0]
loader = src.split('bool EnsureClassBodyModel(', 1)[1].split('bool EnsureModel(', 1)[0]
for token in ('part >= kClassBodyModelCount', '!wglGetCurrentContext()',
              '!OGL330::IsShader()', 'ClassBodyModelId(part, MAX_MODELS)',
              'if (!model.NewMeshes)', 'model.LoadMeshToVAO();',
              'model.UploadAllToGPU();', 'glIsVertexArray(gpu.VAO)',
              'glIsBuffer(gpu.VBO)', 'glIsBuffer(gpu.IBO)',
              'if (!valid) model.ReleaseGpuMeshes();'):
    assert token in gpu, token
assert gpu.index('glGetIntegerv(GL_VERTEX_ARRAY_BINDING') < gpu.index('model.UploadAllToGPU();')
assert gpu.index('model.UploadAllToGPU();') < gpu.index('glBindVertexArray(static_cast<GLuint>(vao))')
assert loader.count('EnsureClassBodyGpu(part)') == 2, 'cached and fresh CPU paths both need GPU readiness'
assert 'ConvertOldMeshToVaoMesh' not in loader
legacy = (root/'ExMain_RISE_PC/Main5.2_RISE/New_ModelBMD.cpp').read_text()
wrapper = legacy.split('void OGL330MODEL::ConvertOldMeshToVaoMesh', 1)[1].split('void OGL330MODEL::UseShader', 1)[0]
assert 'modelId >= MAX_MODELS' in wrapper, 'legacy registry policy must not broaden'
print('PASS private-five-slot/native-upload/cache-retry/binding-restoration source contract')
print('OPEN actual shader initialization, upload sizes, deletion, draw and visual acceptance')
