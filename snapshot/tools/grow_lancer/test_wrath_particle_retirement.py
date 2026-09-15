"""Compile actual retirement body against scoped owner/slot fixtures."""
from pathlib import Path
import subprocess
root = Path(__file__).resolve().parents[2]
source = (root/'ExMain_RISE_PC/Main5.2_RISE/ZzzEffectParticle.cpp').read_text(encoding='utf-8')
body = source.split('void rise::growlancer::RetireWrathPersistentParticles(',1)[1].split('\n}\n',1)[0] + '\n}\n'
fixture = r'''
#define NULL nullptr
struct OBJECT {};
struct PARTICLE { bool Live=true; int Type=0; OBJECT* Target=nullptr; };
struct Fixture {
    static constexpr int MAX_PARTICLES=6;
    static constexpr int kWrathMono01Bitmap=32970, kWrathMono03Bitmap=32972;
    PARTICLE Particles[MAX_PARTICLES]{};
    float g_wrathParticleRemainder[MAX_PARTICLES]{};
    constexpr void RetireWrathPersistentParticles(''' + body + r'''
};
constexpr bool Check() {
    Fixture f{}; OBJECT a{},b{};
    for(int i=0;i<6;++i) {
        f.Particles[i].Target=&a;
        f.Particles[i].Type=32970+i%3;
        f.g_wrathParticleRemainder[i]=.5f;
    }
    f.Particles[1].Target=&b;
    f.Particles[2].Type=32969;
    f.Particles[3].Type=32973;
    f.Particles[4].Live=false; // stale borrowed owner in a dead slot
    f.RetireWrathPersistentParticles(nullptr);
    if(!f.Particles[0].Live) return false;
    f.RetireWrathPersistentParticles(&a);
    return !f.Particles[0].Live && !f.Particles[0].Target &&
        !f.Particles[4].Target && !f.Particles[5].Live && !f.Particles[5].Target &&
        f.g_wrathParticleRemainder[0]==0 && f.g_wrathParticleRemainder[4]==0 &&
        f.g_wrathParticleRemainder[5]==0 && f.Particles[1].Live &&
        f.Particles[1].Target==&b && f.g_wrathParticleRemainder[1]==.5f &&
        f.Particles[2].Live && f.Particles[2].Target==&a &&
        f.Particles[3].Live && f.Particles[3].Target==&a;
}
static_assert(Check(), "owner-scoped retirement, first/last slot, stale dead slot, unrelated types");
'''
out = root/'ExMain_RISE_PC/Tests/GrowLancerBuild/WrathRetirementTest'
out.mkdir(exist_ok=True)
cpp = out/'Retirement.cpp'
cpp.write_text(fixture)
compiler = r'C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x86\cl.exe'
subprocess.run([compiler,'/nologo','/std:c++17','/W4','/WX','/c',str(cpp),
                '/Fo'+str(out/'Retirement.obj')],check=True)
print('PASS actual retirement body: null/other owner, unrelated types, live/dead matching slots, first/last slot and remainder reset.')
print('No runtime object-generation or visual acceptance implied.')
