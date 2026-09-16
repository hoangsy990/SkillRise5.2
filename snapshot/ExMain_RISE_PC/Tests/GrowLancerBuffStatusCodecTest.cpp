#include "../../Shared/GrowLancerBuffStatusCodec.h"
using namespace rise::growlancer;
constexpr bool TestCodec()
{
    unsigned char bytes[10]={};
    for(unsigned id=424;id<=425;++id)
    for(unsigned operation=0;operation<2;++operation)
    {
        const WrathStatusMessage input={operation!=0,static_cast<std::uint16_t>(id),0xFEDCBA98u,0x4321};
        WrathStatusMessage output={};
        if(!EncodeWrathStatus(input,bytes,10) || !DecodeWrathStatus(bytes,10,output)) return false;
        if(output.buff!=id || output.remove!=input.remove || output.durationSeconds!=input.durationSeconds || output.auxiliary!=input.auxiliary) return false;
        if(bytes[3]!=1 || bytes[4]!=0x98 || bytes[7]!=0xFE || bytes[8]!=0x21) return false;
    }
    WrathStatusMessage sentinel={true,425,123,17};
    for(unsigned size=0;size<10;++size)
        if(DecodeWrathStatus(bytes,size,sentinel)) return false;
    if(DecodeWrathStatus(bytes,11,sentinel) || DecodeWrathStatus(nullptr,10,sentinel)) return false;
    bytes[0]=2; if(DecodeWrathStatus(bytes,10,sentinel)) return false;
    bytes[0]=1; bytes[1]=2; if(DecodeWrathStatus(bytes,10,sentinel)) return false;
    bytes[1]=0; bytes[2]=22; bytes[3]=1; // skill278 is NOT buff424
    if(DecodeWrathStatus(bytes,10,sentinel)) return false;
    if(sentinel.buff!=425 || sentinel.durationSeconds!=123 || sentinel.auxiliary!=17 || !sentinel.remove) return false;
    const WrathStatusMessage invalid={false,278,5,0};
    const unsigned char old=bytes[0];
    return !EncodeWrathStatus(invalid,bytes,10) && bytes[0]==old;
}
static_assert(TestCodec(),"Wrath codec must preserve16-bit IDs and reject malformed payload without mutation");
