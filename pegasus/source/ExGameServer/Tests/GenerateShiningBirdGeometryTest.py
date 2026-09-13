"""Generate an isolated test TU from the current native server function bodies.

Only a disposable build artifact is written; no source/math implementation is
copied into production or independently reimplemented as the test subject.
"""
from pathlib import Path
import re

root = Path(__file__).resolve().parents[2]
server = (root / 'ExGameServer/GameServer/SkillManager.cpp').read_text(encoding='latin1')
math = (root / 'Include/Math.cpp').read_text(encoding='latin1')

def function(text, signature):
    start = text.index(signature)
    opening = text.index('{', start)
    depth = 0
    for i in range(opening, len(text)):
        if text[i] == '{':
            depth += 1
        elif text[i] == '}':
            depth -= 1
            if depth == 0:
                return text[start:i+1]
    raise ValueError('Unclosed function: ' + signature)

native = '\n'.join(function(server, signature) for signature in (
    'int CSkillManager::GetSkillAngle(',
    'int CSkillManager::GetSkillFrustrum(',
    'bool CSkillManager::CheckSkillFrustrum(',
    'bool CSkillManager::CheckSkillRadio('))
helpers = '\n'.join(function(math, signature) for signature in (
    'void AngleMatrix (', 'void VectorRotate ('))
route = function(server, 'bool CSkillManager::SkillShiningBird(')
if 'lpObj->X, lpObj->Y, 1.5f, 6.0f, 1.5f, 0.0f)' not in route:
    raise SystemExit('Native Shining Bird frustum arguments changed; update test scope explicitly')
match = re.search(r'angle = static_cast<BYTE>\(\(this->GetSkillAngle\(.*?;', route, re.S)
if not match:
    raise SystemExit('Missing target-derived byte-angle expression')
heading = match.group(0)
skill_header = (root / 'ExGameServer/GameServer/SkillManager.h').read_text(encoding='latin1')
count_macro = re.search(r'^#define CHECK_SKILL_ATTACK_COUNT\(x\).+$', skill_header, re.M).group(0)
header = (root / 'Include/Math.h').as_posix()
source = r'''
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include "@HEADER@"
typedef unsigned char BYTE;
constexpr int MAX_OBJECT=96, MAX_VIEWPORT=75, VIEWPORT_NONE=0, SKILL_COMBO=59;
#define OBJECT_RANGE(x) (((x)<0)?0:((x)>=MAX_OBJECT)?0:1)
@COUNT_MACRO@
struct View { int state=0,index=0,type=0; };
struct TestObject { int X=0,Y=0; View VpPlayer2[MAX_VIEWPORT]; };
using LPOBJ=TestObject*;
TestObject gObj[MAX_OBJECT];
struct CSkill { int m_index=241; };
struct SKILL_INFO { int Radio=0; };
struct Delayed { int message,target,delay,skill; bool combo; };
std::vector<Delayed> delayed;
std::vector<int> echoes;
void gObjAddAttackProcMsgSendDelay(LPOBJ,int message,int target,int delay,int skill,bool combo) {
    delayed.push_back({message,target,delay,skill,combo});
}
class CSkillManager {
public:
    int GetSkillAngle(int,int,int,int);
    int GetSkillFrustrum(int*,int*,BYTE,int,int,float,float,float,float);
    bool CheckSkillFrustrum(int*,int*,int,int);
    bool CheckSkillRadio(int,int,int,int,int);
    bool SkillShiningBird(int,int,CSkill*,BYTE,bool);
    int rejected=-1, radio=6;
    bool infoAvailable=true;
    bool GetInfo(int,SKILL_INFO* info) { info->Radio=radio; return infoAvailable; }
    bool CheckSkillTarget(LPOBJ,int index,int,int) {
        return OBJECT_RANGE(index) && index!=rejected;
    }
    void GCSkillAttackSend(LPOBJ,int skill,int,int) { echoes.push_back(skill); }
    BYTE TestHeading(int x,int y,int tx,int ty) {
        struct Point { int X,Y; } owner={x,y};
        gObj[1].X=tx; gObj[1].Y=ty;
        Point* lpObj=&owner;
        const int bIndex=1;
        BYTE angle=0;
        @HEADING@
        return angle;
    }
};
@HELPERS@
@NATIVE@
@ROUTE@
bool TestActualRoute(CSkillManager& manager) {
    CSkill skill;
    auto reset=[&]() {
        for(auto& object:gObj) object=TestObject{};
        delayed.clear(); echoes.clear(); manager.rejected=-1;
        manager.radio=6; manager.infoAvailable=true;
        gObj[0].X=100; gObj[0].Y=100;
        gObj[1].X=103; gObj[1].Y=100;
        gObj[0].VpPlayer2[0]={1,1,0};
    };
    reset();
    if(!manager.SkillShiningBird(0,1,&skill,0,false) || delayed.size()!=1 || echoes.size()!=1) return false;
    const auto hit=delayed[0];
    if(hit.target!=1 || hit.message!=50 || hit.delay!=200 || hit.skill!=241 || hit.combo || echoes[0]!=241) return false;
    reset();
    if(manager.SkillShiningBird(0,-1,&skill,0,false) || !delayed.empty() || !echoes.empty()) return false;
    if(manager.SkillShiningBird(0,MAX_OBJECT,&skill,0,false) || !delayed.empty() || !echoes.empty()) return false;
    for(int rejection=0;rejection<5;++rejection) {
        reset();
        if(rejection==0) manager.rejected=1;
        if(rejection==1) manager.radio=0;
        if(rejection==2) manager.infoAvailable=false;
        if(rejection==3) gObj[0].VpPlayer2[0].state=VIEWPORT_NONE;
        if(rejection==4) gObj[1].X=120;
        if(!manager.SkillShiningBird(0,1,&skill,0,false) || !delayed.empty() || echoes.size()!=1) return false;
    }
    reset();
    // Real geometry must filter a viewport target behind the selected heading.
    gObj[2].X=97; gObj[2].Y=100; gObj[0].VpPlayer2[1]={1,2,0};
    if(!manager.SkillShiningBird(0,1,&skill,0,true) || delayed.size()!=1 ||
        !delayed[0].combo || echoes.size()!=2 || echoes[1]!=SKILL_COMBO) return false;
    reset();
    for(int i=1;i<=12;++i) {
        gObj[i].X=103; gObj[i].Y=100; gObj[0].VpPlayer2[i-1]={1,i,0};
    }
    if(!manager.SkillShiningBird(0,1,&skill,0,false) || delayed.size()!=10) return false;
    for(int i=0;i<10;++i) if(delayed[i].target!=i+1) return false;
    return true;
}
int main() {
    CSkillManager manager;
    if(!TestActualRoute(manager)) {
        std::puts("FAIL: actual Shining Bird route fixture"); return 3;
    }
    std::puts("PASS: actual Shining Bird route schedules one 200ms hit, filters invalid/radius/viewport/behind targets, preserves combo and native ten-target cap (external services stubbed)");
    const int delta[8][2]={{3,0},{2,2},{0,3},{-2,2},
        {-3,0},{-2,-2},{0,-3},{2,-2}};
    int checked=0, oldMisses=0;
    for(int origin: {20,200}) {
        for(const auto& d: delta) {
            const int tx=origin+d[0], ty=origin+d[1];
            int x[4],y[4];
            const BYTE heading=manager.TestHeading(origin,origin,tx,ty);
            manager.GetSkillFrustrum(x,y,heading,origin,origin,1.5f,6.f,1.5f,0.f);
            if(!manager.CheckSkillFrustrum(x,y,tx,ty) ||
                manager.CheckSkillFrustrum(x,y,origin-d[0],origin-d[1]) ||
                manager.CheckSkillFrustrum(x,y,origin+d[0]*4,origin+d[1]*4)) {
                std::printf("FAIL dx=%d dy=%d heading=%u\n",d[0],d[1],heading);
                return 1;
            }
            ++checked;
            manager.GetSkillFrustrum(x,y,0,origin,origin,1.5f,6.f,1.5f,0.f);
            if(!manager.CheckSkillFrustrum(x,y,tx,ty)) ++oldMisses;
        }
    }
    if(oldMisses==0) return 2;
    std::printf("PASS: %d native directional targets; behind/far excluded; old angle0 misses=%d\n",checked,oldMisses);
}
'''.replace('@HEADER@', header).replace('@HEADING@', heading).replace('@HELPERS@',helpers).replace('@NATIVE@',native).replace('@ROUTE@',route).replace('@COUNT_MACRO@',count_macro)
# initializer_list is needed for the two origin cases, independently of PCH.
source = '#include <initializer_list>\n' + source
output = root / 'ExGameServer/Tests/PegasusBuild/ShiningBirdGeometry.generated.cpp'
output.parent.mkdir(parents=True, exist_ok=True)
output.write_text(source, encoding='ascii')
print('Generated native geometry test:', output)
