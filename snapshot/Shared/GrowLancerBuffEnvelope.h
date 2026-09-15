#pragma once
#include "GrowLancerBuffStatusCodec.h"
namespace rise { namespace growlancer {
// Isolated RISE extension reservation. Not a Season21 opcode or legacy2D change.
// C1,size,FC,'G','L',version1,kind,sessionTag:u32LE,[status payload].
// Session tag rejects stale acknowledgements; it is NOT authentication.
enum class BuffEnvelopeKind : unsigned char { Hello=1, Accept=2, Status=3 };
constexpr std::size_t kBuffEnvelopeHeaderSize=11;
constexpr std::size_t kBuffStatusEnvelopeSize=21;
struct BuffEnvelopeView {
    BuffEnvelopeKind kind;
    std::uint32_t sessionTag;
    WrathStatusMessage status;
};
constexpr bool DecodeBuffEnvelope(const unsigned char* data,std::size_t size,
    BuffEnvelopeView& output)
{
    if(!data || (size!=kBuffEnvelopeHeaderSize && size!=kBuffStatusEnvelopeSize)) return false;
    if(data[0]!=0xC1 || data[1]!=size || data[2]!=0xFC || data[3]!='G' ||
        data[4]!='L' || data[5]!=1 || data[6]<1 || data[6]>3) return false;
    const bool status=data[6]==3;
    if(size!=(status ? kBuffStatusEnvelopeSize : kBuffEnvelopeHeaderSize)) return false;
    const std::uint32_t tag=std::uint32_t(data[7]) | (std::uint32_t(data[8])<<8) |
        (std::uint32_t(data[9])<<16) | (std::uint32_t(data[10])<<24);
    if(tag==0) return false;
    WrathStatusMessage message={};
    if(status && !DecodeWrathStatus(data+11,10,message)) return false;
    output={static_cast<BuffEnvelopeKind>(data[6]),tag,message};
    return true;
}
constexpr bool EncodeBuffEnvelope(const BuffEnvelopeView& input,unsigned char* data,
    std::size_t size)
{
    const unsigned kind=static_cast<unsigned>(input.kind);
    const bool status=kind==3;
    if(!data || kind<1 || kind>3 || input.sessionTag==0 ||
        size!=(status ? kBuffStatusEnvelopeSize : kBuffEnvelopeHeaderSize) ||
        (status && !IsWrathStatusId(input.status.buff))) return false;
    data[0]=0xC1;data[1]=static_cast<unsigned char>(size);data[2]=0xFC;
    data[3]='G';data[4]='L';data[5]=1;data[6]=static_cast<unsigned char>(kind);
    for(unsigned i=0;i<4;++i) data[7+i]=static_cast<unsigned char>(input.sessionTag>>(i*8));
    return !status || EncodeWrathStatus(input.status,data+11,10);
}
} }
