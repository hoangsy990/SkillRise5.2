#pragma once
#include "GrowLancerBuffSession.h"
namespace rise { namespace growlancer {
template<class ApplyStatus>
constexpr bool ReceiveBuffServerFrame(BuffClientSession& session,const unsigned char* bytes,
    std::size_t size,bool featureReady,ApplyStatus apply)
{
    if(!featureReady) {session.Reset();return false;}
    BuffEnvelopeView message={};
    if(!DecodeBuffEnvelope(bytes,size,message)) return false;
    if(message.kind==BuffEnvelopeKind::Accept) return session.Accept(message);
    WrathStatusMessage status={};
    return session.ReadStatus(message,status) && apply(status);
}
// Queue must copy bytes before returning true; stack buffer is not retained.
template<class Queue>
constexpr bool ReceiveBuffHello(BuffServerSession& session,const unsigned char* bytes,
    std::size_t size,bool featureReady,Queue queue)
{
    if(!featureReady) {session.Reset();return false;}
    BuffEnvelopeView request={},reply={};
    if(!DecodeBuffEnvelope(bytes,size,request) || request.kind!=BuffEnvelopeKind::Hello)
        return false;
    if(!session.Hello(request,true,reply)) return false;
    unsigned char packet[kBuffEnvelopeHeaderSize]={};
    if(!EncodeBuffEnvelope(reply,packet,sizeof(packet)) || !queue(packet,sizeof(packet)))
    {
        session.Reset();return false;
    }
    return session.AcceptQueued(reply.sessionTag);
}
} }
