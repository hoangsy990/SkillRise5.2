#include "../../Shared/GrowLancerBuffEnvelope.h"
using namespace rise::growlancer;
constexpr bool CheckEnvelope()
{
    unsigned char bytes[21]={};
    for(unsigned kind=1;kind<=3;++kind) {
        const BuffEnvelopeView input={static_cast<BuffEnvelopeKind>(kind),0xFEDCBA98u,{false,424,30,7}};
        const unsigned size=kind==3 ? 21 : 11;
        BuffEnvelopeView output={};
        if(!EncodeBuffEnvelope(input,bytes,size) || !DecodeBuffEnvelope(bytes,size,output)) return false;
        if(output.kind!=input.kind || output.sessionTag!=input.sessionTag) return false;
        if(kind==3 && (output.status.buff!=424 || output.status.durationSeconds!=30)) return false;
    }
    BuffEnvelopeView sentinel={BuffEnvelopeKind::Accept,77,{true,425,123,9}};
    for(unsigned size=0;size<21;++size) if(DecodeBuffEnvelope(bytes,size,sentinel)) return false;
    if(DecodeBuffEnvelope(nullptr,21,sentinel) || DecodeBuffEnvelope(bytes,22,sentinel)) return false;
    const unsigned char indexes[]={0,1,2,3,4,5,6};
    for(unsigned i=0;i<7;++i) {
        const unsigned at=indexes[i];const unsigned char saved=bytes[at];bytes[at]=0;
        if(DecodeBuffEnvelope(bytes,21,sentinel)) return false;
        bytes[at]=saved;
    }
    for(unsigned i=7;i<11;++i) bytes[i]=0;
    if(DecodeBuffEnvelope(bytes,21,sentinel)) return false;
    return sentinel.sessionTag==77 && sentinel.kind==BuffEnvelopeKind::Accept && sentinel.status.durationSeconds==123;
}
static_assert(CheckEnvelope(),"extension envelope bounds, magic, version, kind and tag validation");
