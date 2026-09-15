"""Source guard/ownership audit, not native probe runtime evidence."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
client = root/'ExMain_RISE_PC/Main5.2_RISE'
source = (client/'RISE/GrowLancerRuntimeQA.cpp').read_text()
assert source.index('#ifdef RISE_GROW_LANCER_RUNTIME_QA') < source.index('int RunBodyLoadProbeQA()')
probe = source.split('int RunBodyLoadProbeQA()',1)[1].split('bool HandleRuntimeQAHotKey()',1)[0]
assert 'static bool bodyProbeChecked = false;' in probe
assert 'GetEnvironmentVariableA("RISE_GL_BODY_LOAD_QA", enabled, sizeof(enabled)) == 1' in probe
assert "enabled[0] == '1'" in probe
assert 'candidate.Open2(directory, filename)' in probe
assert probe.index('candidate.IndexTexture[mesh] = BITMAP_SKIN') < probe.index('event=body-native-load')
for forbidden in ('LoadImageFile', 'Models[', 'CreateEffect(', 'SetAction(', 'Release();'):
    assert forbidden not in probe
bmd = (client/'ZzzBMD.cpp').read_text()
release = bmd.split('void BMD::Release()',1)[1].split('void BMD::FindNearTriangle',1)[0]
assert 'case BITMAP_SKIN:' in release and 'DeleteBitmap(IndexTexture[m->Texture]);' in release
assert 'BMD::~BMD()\n{\n    Release();\n}' in bmd
startup = (client/'Winmain.cpp').read_text(errors='replace').split('int __stdcall APIENTRY WinMain(',1)[1]
assert startup.index('RunBodyLoadProbeQA()') < startup.index('pMain->WinHook(hInstance)')
assert 'if (bodyProbeResult >= 0) return bodyProbeResult;' in startup
print('PASS opt-in QA native Open2/RAII and no-texture ownership wiring')
print('OPEN actual load/release logs, malformed input, GL state, rendering and leak QA')
