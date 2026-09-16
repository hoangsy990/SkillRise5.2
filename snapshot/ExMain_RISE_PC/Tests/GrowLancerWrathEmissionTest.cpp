#include "../Main5.2_RISE/RISE/GrowLancerWrathEmission.h"
struct State {
    int rng=0, count=0, stage=0;
    int variants[6]={}, xs[2]={}, ys[2]={}, attachments[4]={};
    float scales[2]={};
};
struct Random { State* s; constexpr int operator()() const { return s->rng++; } };
struct Scatter {
    State* s;
    constexpr void operator()(int v,int x,int y,float scale) const {
        const int i=s->count++;
        s->variants[i]=v;s->xs[i]=x;s->ys[i]=y;s->scales[i]=scale;
        s->rng+=2; // Simulated constructor draws; next position must follow these.
    }
};
struct Flares {
    State* s;
    constexpr void operator()() const {s->stage=s->count==2 ? 1 : -1;}
};
struct Mono {
    State* s;
    constexpr void operator()(int v,int attachment) const {
        if(s->stage!=1) s->stage=-1;
        const int i=s->count++;
        s->variants[i]=v;s->attachments[i-2]=attachment;s->rng+=3;
    }
};
constexpr bool Check(int seed) {
    State s{};s.rng=seed;
    rise::growlancer::SubmitWrathParticleSequence(Random{&s},Scatter{&s},Flares{&s},Mono{&s});
    const int mono=(seed+11)%3==1 ? 1 : 0;
    return s.count==6 && s.stage==1 && s.rng==seed+24 &&
        s.xs[0]==seed%220-110 && s.ys[0]==(seed+1)%220-110 &&
        s.xs[1]==(seed+6)%220-110 && s.ys[1]==(seed+7)%220-110 &&
        s.scales[0]==static_cast<float>((seed+2)%3+3)*.1f &&
        s.scales[0]==s.scales[1] && s.variants[0]==(seed+3)%3 &&
        s.variants[1]==(seed+8)%3 && s.variants[2]==mono &&
        s.variants[3]==mono && s.variants[4]==2 && s.variants[5]==2 &&
        s.attachments[0]==0 && s.attachments[1]==1 &&
        s.attachments[2]==0 && s.attachments[3]==1;
}
static_assert(Check(0) && Check(1) && Check(2),"Wrath source emission/RNG order");
