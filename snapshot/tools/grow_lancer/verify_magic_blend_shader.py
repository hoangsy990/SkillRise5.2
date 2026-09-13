"""Read-only shader/source contracts; never writes through QA Data junctions."""
import hashlib
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BASES = (ROOT / 'Client/Data/Effect/VBO',
         ROOT / 'ExMain_RISE_PC/Tests/GrowLancerBuild/RuntimeQA/Client/Data/Effect/VBO')
PINS = {
    'BlendMesh.vs': '6a3a51a7e6dcbe9a9ebececa7abf5004141270bd9f4a1137e0cddf6284f2df67',
    'BlendMesh.fs': '0d50150d15184f8030899815699769ac2fca715fa707bd4366369a8543ff9a26',
}
for base in BASES:
    for name, digest in PINS.items():
        assert hashlib.sha256((base / name).read_bytes()).hexdigest() == digest
vertex = (BASES[0] / 'BlendMesh.vs').read_text()
fragment = (BASES[0] / 'BlendMesh.fs').read_text()
assert 'vTex = aTex + u_meshUV.xy;' in vertex
assert 'color.rgb *= mix(1.0, intensity, lightFlag);' in vertex
assert 'color.rgb = clamp(color.rgb, vec3(0.45 * lightFlag), vec3(1.0));' in vertex
assert 'FragColor = texColor * vColor;' in fragment
source = (ROOT / 'ExMain_RISE_PC/Main5.2_RISE/New_ModelBMD.cpp').read_text()
for statement in (
    'meshUv.x = blendU;', 'meshUv.y = blendV;',
    'command.BodyLight = bodyLight;', 'command.MeshUv = meshUv;',
    'glUniform4f(locations.BodyLight, bodyLight.x, bodyLight.y, bodyLight.z, bodyLight.w);',
    'glUniform4f(locations.MeshUv, meshUv.x, meshUv.y, meshUv.z, meshUv.w);',
    'glUniform1i(locations.EnableLight, enableLight ? 1 : 0);',
):
    assert statement in source
# MagicPin03 callback RGB=(.48,.73,1)*alpha, blend light=alpha,
# selected envelope alpha in [0,.8]. All resulting colors in [0,.64].
# With lightFlag=0, the shader's clamp [0,1] cannot change that range.
for alpha in (0.0, 0.16, 0.32, 0.48, 0.64, 0.8):
    for channel in (0.48, 0.73, 1.0):
        color = channel * alpha * alpha
        assert 0 <= color <= 0.6400000000000001
        assert min(1.0, max(0.0, color)) == color
print('PASS source and QA BlendMesh VS/FS hashes identical; UV/color uniform preparation and selected clamp range')
draw = (ROOT / 'ExMain_RISE_PC/Main5.2_RISE/New_RenderBMD.cpp').read_text()
begin = draw.index('OGL330MODEL::SendUniform(')
end = draw.index('if (!mesh.SendIndexBone(', begin)
submission = draw[begin:end]
for field in ('command.Shader', 'command.BodyLight', 'command.LightPosition',
              'command.MeshUv', 'command.Setting1', 'command.Setting2',
              'command.EnableLight', 'useTexture', 'bodyOrigin'):
    assert field in submission
assert end < draw.index('glBindVertexArray(mesh.VAO);') < draw.index('glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, NULL);')
queue = source[source.index('void OGL330MODEL::CGMMeshShader::FlushAllMesh()'):]
assert 'for (MeshVAO::iterator it = m_data.begin(); it != m_data.end(); ++it)' in queue
assert queue.index('g_NewRenderBMD->Render(*it);') < queue.index('m_data.clear();')
assert 'sort(' not in queue
assert 'm_data.push_back(command);' in source
print('PASS command uniforms submitted before bone upload/draw; queue traverses insertion order then clears')
print('Scope: static source/data and arithmetic; no live shader compilation, draw, backend or visual PASS')
