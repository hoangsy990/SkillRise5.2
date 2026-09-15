"""Compile actual mesh typedefs with native x86 compiler; not BMD::Open QA."""
from pathlib import Path
import re
import subprocess

root = Path(__file__).resolve().parents[2]
client = root/'ExMain_RISE_PC/Main5.2_RISE'
source = (client/'ZzzBMD.h').read_text(encoding='utf-8')
types = (client/'_types.h').read_bytes()
assert re.search(rb'typedef\s+float\s+vec_t\s*;', types)
assert re.search(rb'typedef\s+vec_t\s+vec3_t\[3\]\s*;', types)
records = dict((name, definition) for definition, name in re.findall(
    r'(typedef\s+struct\s*\{[^{}]*\}\s*(\w+)\s*;)', source))
names = ('Vertex_t', 'Normal_t', 'TexCoord_t', 'Triangle_t', 'Triangle_t2')
fixture = 'typedef float vec_t; typedef vec_t vec3_t[3];\n'
fixture += '\n'.join(records[name] for name in names)
for name, size in zip(names, (16, 20, 8, 36, 64)):
    fixture += f'\nstatic_assert(sizeof({name}) == {size}, "serialized {name} stride");'
fixture += '\nstatic_assert(sizeof(short)==2 && sizeof(bool)==1, "serialized scalar widths");\n'
build = root/'ExMain_RISE_PC/Tests/GrowLancerBuild'
generated = build/'BodySerializedABI.cpp'
generated.write_text(fixture, encoding='utf-8')
compiler = Path('C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/cl.exe')
subprocess.run([str(compiler), '/nologo', '/std:c++14', '/W4', '/WX', '/c',
                str(generated), '/Fo'+str(build/'BodySerializedABI.obj')], check=True)
print('PASS actual native typedef x86 sizes: Vertex16 Normal20 UV8 Triangle36 serializedTriangle64')
print('OPEN full project include/packing state and execution of BMD::Open')
