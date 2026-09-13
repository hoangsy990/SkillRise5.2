"""Compile current native effect lifecycle bodies; only emit a build fixture."""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[2]
server = root / 'ExGameServer/GameServer'

def function(text, signature):
    start = text.index(signature)
    opening = text.index('{', start)
    depth = 0
    for i in range(opening, len(text)):
        depth += (text[i] == '{') - (text[i] == '}')
        if depth == 0:
            return text[start:i + 1]
    raise ValueError(signature)

effect = (server / 'Effect.cpp').read_text(encoding='latin1')
manager = (server / 'EffectManager.cpp').read_text(encoding='latin1')
seed_start = manager.index('for (const PegasusEffectSeed& seed : pegasusEffects)')
seed_end = manager.index('delete lpMemScript;', seed_start)
seed = manager[seed_start:seed_end]
save = re.search(r'info.Save\s*=\s*(\d+)\s*;', seed)
kind = re.search(r'info.Type\s*=\s*(\d+)\s*;', seed)
if not save or not kind or 'info.Group = seed.index;' not in seed:
    raise ValueError('Seed structure changed; re-audit actual lifecycle configuration')
native = '\n'.join(function(effect, sig) for sig in (
    'CEffect::CEffect()', 'void CEffect::Clear()', 'bool CEffect::IsEffect()',
    'bool CEffect::Set('))
native += '\n' + '\n'.join(function(manager, sig) for sig in (
    'void CEffectManager::ClearAllEffect(',
    'void CEffectManager::EffectByteConvert(',
    'bool CEffectManager::ConvertEffectByte('))
source = r'''
#include <cstdint>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <vector>
using BYTE=unsigned char; using WORD=unsigned short; using DWORD=unsigned long;
#define SET_NUMBERHB(x) static_cast<BYTE>((x)>>8)
#define SET_NUMBERLB(x) static_cast<BYTE>((x)&255)
#define SET_NUMBERHW(x) static_cast<WORD>((x)>>16)
#define SET_NUMBERLW(x) static_cast<WORD>((x)&65535)
#define MAKE_NUMBERW(a,b) static_cast<WORD>((static_cast<WORD>(a)<<8)|(b))
#define MAKE_NUMBERDW(a,b) ((static_cast<DWORD>(a)<<16)|(b))
#include "@EFFECT_HEADER@"
#include "@CATALOG_HEADER@"
constexpr int MAX_EFFECT_LIST=8;
struct TestObject { CEffect Effect[MAX_EFFECT_LIST]; };
using LPOBJ=TestObject*;
struct Notice { int group,index; };
class CEffectManager {
public:
    std::vector<int> removed;
    std::vector<Notice> notices;
    void ClearAllEffect(LPOBJ);
    void EffectByteConvert(BYTE*,CEffect*);
    bool ConvertEffectByte(CEffect*,BYTE*);
    void RemoveEffect(LPOBJ, CEffect* e) { removed.push_back(e->m_index); }
    void GCPeriodicEffectSend(LPOBJ,int before,int group,int operation,int count,int index) {
        if(before!=0 || operation!=1 || count!=0) std::abort();
        notices.push_back({group,index});
    }
};
@NATIVE@
void check(bool pass) { if(!pass) std::abort(); }
int main() {
    using namespace rise::pegasus;
    const BYTE ids[]={kSpiralChargeNativeEffect,kCrusherChargeNativeEffect,kElementalChargeNativeEffect};
    TestObject obj;
    CEffectManager mgr;
    for(int i=0;i<3;++i) {
        check(obj.Effect[i].Set(ids[i],ids[i],@SAVE@,@TYPE@,0,0,0,0,0,0,0));
        BYTE packet[13]={};
        mgr.EffectByteConvert(packet,&obj.Effect[i]);
        for(BYTE value:packet) check(value==255);
        CEffect restored;
        check(!mgr.ConvertEffectByte(&restored,packet));
        check(!restored.IsEffect());
    }
    // Saved non-Type0 effect is the control: preserve it on clear and serialize it.
    check(obj.Effect[3].Set(50,50,1,1,0,0,120,1,2,3,4));
    BYTE saved[13]={};
    mgr.EffectByteConvert(saved,&obj.Effect[3]);
    CEffect restored;
    check(mgr.ConvertEffectByte(&restored,saved));
    check(restored.m_index==50 && restored.m_count==120 && restored.m_value[3]==4);
    mgr.ClearAllEffect(&obj);
    check(mgr.removed.size()==3 && mgr.notices.size()==3);
    for(int i=0;i<3;++i) {
        check(!obj.Effect[i].IsEffect());
        check(mgr.removed[i]==ids[i] && mgr.notices[i].group==ids[i] && mgr.notices[i].index==ids[i]);
    }
    check(obj.Effect[3].IsEffect());
    mgr.ClearAllEffect(&obj);
    check(mgr.removed.size()==3 && mgr.notices.size()==3);
    std::puts("PASS: native charge clear/save/restore bodies; saved effect control preserved. Not live death/logout acceptance.");
}
'''
source = source.replace('@EFFECT_HEADER@', (server / 'Effect.h').as_posix())
source = source.replace('@CATALOG_HEADER@', (root / 'Shared/PegasusHighSkillCatalogData.h').as_posix())
source = source.replace('@NATIVE@', native)
source = source.replace('@SAVE@', save.group(1)).replace('@TYPE@', kind.group(1))
output = root / 'ExGameServer/Tests/PegasusBuild/PegasusBuffLifecycle.generated.cpp'
output.parent.mkdir(exist_ok=True)
output.write_text(source, encoding='utf-8')
print(output)
