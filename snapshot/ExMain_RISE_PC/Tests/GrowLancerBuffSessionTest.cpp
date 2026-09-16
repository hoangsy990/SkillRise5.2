#include "../../Shared/GrowLancerBuffSession.h"
using namespace rise::growlancer;
constexpr bool SessionChecks()
{
    BuffClientSession client;BuffServerSession server;
    BuffEnvelopeView hello={},ack={},frame={};
    WrathStatusMessage status={false,424,30,9},output={true,425,7,3};
    if(server.MakeStatus(status,frame)) return false;
    if(client.Begin(1,false,hello) || client.Begin(0,true,hello)) return false;
    if(!client.Begin(1,true,hello)) return false;
    if(client.Accept({BuffEnvelopeKind::Accept,2,{}})) return false;
    if(server.Hello(hello,false,ack) || !server.Hello(hello,true,ack)) return false;
    if(server.MakeStatus(status,frame) || server.AcceptQueued(2)) return false;
    if(!server.AcceptQueued(1) || !server.MakeStatus(status,frame)) return false;
    if(client.ReadStatus(frame,output)) return false;
    if(output.buff!=425 || output.durationSeconds!=7) return false;
    if(!client.Accept(ack) || !client.ReadStatus(frame,output) || output.buff!=424) return false;
    if(server.Hello({BuffEnvelopeKind::Status,1,status},true,ack)) return false;
    if(server.Hello({BuffEnvelopeKind::Hello,2,{}},true,ack)) return false;
    const BuffEnvelopeView oldFrame=frame,oldAck=ack;
    client.Reset();server.Reset();
    if(client.ReadStatus(oldFrame,output) || client.Accept(oldAck) || server.MakeStatus(status,frame)) return false;
    if(!client.Begin(2,true,hello) || client.Accept(oldAck)) return false;
    if(!server.Hello(hello,true,ack) || !server.AcceptQueued(2) || !client.Accept(ack)) return false;
    if(client.ReadStatus(oldFrame,output)) return false;
    status={true,425,0,0};
    if(!server.MakeStatus(status,frame) || !client.ReadStatus(frame,output) || !output.remove) return false;
    status.buff=278;
    if(server.MakeStatus(status,frame)) return false;
    if(server.Hello(hello,false,ack) || server.MakeStatus({false,424,1,0},frame)) return false;
    return true;
}
static_assert(SessionChecks(),"session negotiation, direction, readiness, reset and stale frame rejection");
