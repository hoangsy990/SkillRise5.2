#pragma once
#include "../../../Shared/GrowLancerBuffHandshake.h"
#include "../SocketManager.h"
namespace rise { namespace growlancer {
inline bool ReceiveGrowLancerBuffHello(int index,const BYTE* bytes,int size)
{
    if(!OBJECT_USER_RANGE(index) || !bytes || size<0) return false;
    LPOBJ owner=&gObj[index];
    if(owner->Connected!=OBJECT_ONLINE || owner->Type!=OBJECT_USER ||
        owner->Socket==INVALID_SOCKET || !owner->PerSocketContext) return false;
    // Do not advertise until authoritative Wrath generation and client metadata
    // support are connected. Codec compilation alone is not feature readiness.
    constexpr bool featureReady=false;
    return ReceiveBuffHello(owner->GrowLancerBuffSession,bytes,
        static_cast<std::size_t>(size),featureReady,
        [index](unsigned char* packet,std::size_t length) {
            return gSocketManager.DataSend(index,packet,static_cast<int>(length));
        });
}
} }
