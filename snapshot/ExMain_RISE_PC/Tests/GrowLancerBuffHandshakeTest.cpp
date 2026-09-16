#include "../../Shared/GrowLancerBuffHandshake.h"
using namespace rise::growlancer;
struct Queue {
    int* calls;bool succeed;
    constexpr bool operator()(unsigned char* packet,std::size_t length) const {
        ++*calls;
        BuffEnvelopeView reply={};
        return DecodeBuffEnvelope(packet,length,reply) &&
            reply.kind==BuffEnvelopeKind::Accept && reply.sessionTag==17 && succeed;
    }
};
constexpr bool TestHello()
{
    BuffServerSession session;
    unsigned char hello[11]={};
    int calls=0;
    if(!EncodeBuffEnvelope({BuffEnvelopeKind::Hello,17,{}},hello,11)) return false;
    if(ReceiveBuffHello(session,hello,11,false,Queue{&calls,true}) || calls) return false;
    if(ReceiveBuffHello(session,hello,10,true,Queue{&calls,true}) || calls) return false;
    if(ReceiveBuffHello(session,hello,11,true,Queue{&calls,false}) || calls!=1) return false;
    BuffEnvelopeView status={};
    if(session.MakeStatus({false,424,10,0},status)) return false;
    if(!ReceiveBuffHello(session,hello,11,true,Queue{&calls,true}) || calls!=2) return false;
    if(!session.MakeStatus({false,425,10,0},status) || status.sessionTag!=17) return false;
    // A later send failure must revoke previously accepted readiness too.
    if(ReceiveBuffHello(session,hello,11,true,Queue{&calls,false})) return false;
    return !session.MakeStatus({false,424,10,0},status) && calls==3;
}
static_assert(TestHello(),"Hello frame validation and queue-success readiness/rollback");
struct Apply {
    int* calls;
    constexpr bool operator()(const WrathStatusMessage& message) const {
        ++*calls;return message.buff==424;
    }
};
constexpr bool TestClientFrames()
{
    BuffClientSession session;BuffEnvelopeView hello={};
    unsigned char ack[11]={},status[21]={};int calls=0;
    EncodeBuffEnvelope({BuffEnvelopeKind::Accept,19,{}},ack,11);
    EncodeBuffEnvelope({BuffEnvelopeKind::Status,19,{false,424,30,0}},status,21);
    if(ReceiveBuffServerFrame(session,ack,11,true,Apply{&calls})) return false;
    session.Begin(19,true,hello);
    if(ReceiveBuffServerFrame(session,status,21,true,Apply{&calls}) || calls) return false;
    if(!ReceiveBuffServerFrame(session,ack,11,true,Apply{&calls}) || calls) return false;
    if(ReceiveBuffServerFrame(session,status,20,true,Apply{&calls}) || calls) return false;
    if(!ReceiveBuffServerFrame(session,status,21,true,Apply{&calls}) || calls!=1) return false;
    if(ReceiveBuffServerFrame(session,status,21,false,Apply{&calls}) || calls!=1) return false;
    return !ReceiveBuffServerFrame(session,status,21,true,Apply{&calls}) && calls==1;
}
static_assert(TestClientFrames(),"client rejects unsolicited/malformed/disabled frames before status callback");
