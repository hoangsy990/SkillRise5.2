"""Execute actual private particle driver with native option/record fixtures."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[2]
native=root/'ExMain_RISE_PC/Main5.2_RISE'
text=(native/'ZzzEffectParticle.cpp').read_text(encoding='utf-8')
body='void rise::growlancer::StepMagicPinParticles()'+text.split('void rise::growlancer::StepMagicPinParticles()',1)[1].split('int rise::growlancer::CreateBrecheFireParticle',1)[0]
prefix='#include <cassert>\n#include <cstdio>\n'
for path in (root/'GrowLancer/compat/ParticleBirthOrder.h',native/'RISE/GrowLancerTick.h'):
    prefix+=f'#include "{path.as_posix()}"\n'
prefix+='''
struct PARTICLE { bool Live=true; float LifeTime=2.f; int Type=1,SubType=19,updates=0; };
constexpr int BITMAP_CLUD64=1,kShockwave2Bitmap=2,BITMAP_ENERGY=3;
PARTICLE Particles[1];
rise::growlancer::ParticleBirthOrder<1> g_magicParticleBirthOrder;
struct Options { bool all=true,skill=true; bool GetRenderAllEffects(){return all;} bool GetRenderSkillEffects(){return skill;} } options;
Options* g_pOption=&options;
void UpdateMagicSmokeTick(PARTICLE& p){++p.updates;}
void UpdateMagicShockwaveTick(PARTICLE& p){++p.updates;}
void UpdateMagicThunderTick(PARTICLE& p){++p.updates;}
namespace rise { namespace growlancer { void StepMagicPinParticles(); } }
'''
tests='''
int main(){
    for(int flags=0;flags<4;++flags){
        options.all=(flags&1)!=0;options.skill=(flags&2)!=0;
        Particles[0]=PARTICLE{};g_magicParticleBirthOrder.Append(0);
        rise::growlancer::StepMagicPinParticles();
        const bool enabled=flags==3;
        assert(Particles[0].updates==(enabled?1:0));
        assert(Particles[0].LifeTime==(enabled?1.f:2.f));
        assert(Particles[0].Live && g_magicParticleBirthOrder.Contains(0));
    }
    std::puts("PASS actual particle driver respects all four native option combinations; no off-state lifetime mutation");
}
'''
build=root/'ExMain_RISE_PC/Tests/GrowLancerBuild'
cpp=build/'MagicParticleOptions.cpp';cpp.write_text(prefix+body+tests,encoding='utf-8')
vc=Path('C:/Program Files/Microsoft Visual Studio/18/Insiders/VC/Tools/MSVC/14.51.36231')
sdk=Path('C:/Program Files (x86)/Windows Kits/10')
exe=build/'MagicParticleOptions.exe'
subprocess.run([str(vc/'bin/Hostx64/x86/cl.exe'),'/nologo','/std:c++14','/EHsc','/MT','/W4','/WX',
    '/I'+str(vc/'include'),'/I'+str(sdk/'Include/10.0.26100.0/ucrt'),str(cpp),
    '/Fo'+str(build/'MagicParticleOptions.obj'),'/Fe'+str(exe),'/link',
    '/LIBPATH:'+str(vc/'lib/x86'),'/LIBPATH:'+str(sdk/'Lib/10.0.26100.0/ucrt/x86'),
    '/LIBPATH:'+str(sdk/'Lib/10.0.26100.0/um/x86')],check=True)
subprocess.run([str(exe)],check=True)
print('Scope: actual option gating, fixture particle kernels; not GPU or source parity')
