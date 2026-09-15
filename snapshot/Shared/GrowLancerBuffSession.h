#pragma once
#include "GrowLancerBuffEnvelope.h"
namespace rise { namespace growlancer {
// Connection-owned, not global. Reset on disconnect/character change/socket reuse.
// Callers supply trusted transport and a fresh nonzero tag on each new session.
class BuffClientSession
{
    std::uint32_t tag_=0;
    bool ready_=false;
public:
    constexpr void Reset() {tag_=0;ready_=false;}
    constexpr bool Begin(std::uint32_t freshTag,bool featureReady,BuffEnvelopeView& hello)
    {
        Reset();
        if(!featureReady || !freshTag) return false;
        tag_=freshTag; hello={BuffEnvelopeKind::Hello,tag_,{}};return true;
    }
    constexpr bool Accept(const BuffEnvelopeView& message)
    {
        if(!tag_ || message.kind!=BuffEnvelopeKind::Accept || message.sessionTag!=tag_) return false;
        ready_=true;return true;
    }
    constexpr bool ReadStatus(const BuffEnvelopeView& message,WrathStatusMessage& output) const
    {
        if(!ready_ || message.kind!=BuffEnvelopeKind::Status || message.sessionTag!=tag_ ||
            !IsWrathStatusId(message.status.buff)) return false;
        output=message.status;return true;
    }
};
class BuffServerSession
{
    std::uint32_t tag_=0;
    bool ready_=false;
public:
    constexpr void Reset() {tag_=0;ready_=false;}
    constexpr bool Hello(const BuffEnvelopeView& message,bool featureReady,BuffEnvelopeView& reply)
    {
        if(!featureReady) {Reset();return false;}
        if(message.kind!=BuffEnvelopeKind::Hello || !message.sessionTag ||
            (tag_ && message.sessionTag!=tag_)) return false;
        tag_=message.sessionTag;reply={BuffEnvelopeKind::Accept,tag_,{}};return true;
    }
    // Commit only after Accept was queued on the same ordered connection.
    constexpr bool AcceptQueued(std::uint32_t tag)
    {
        if(!tag_ || tag!=tag_) return false;
        ready_=true;return true;
    }
    constexpr bool MakeStatus(const WrathStatusMessage& status,BuffEnvelopeView& output) const
    {
        if(!ready_ || !IsWrathStatusId(status.buff)) return false;
        output={BuffEnvelopeKind::Status,tag_,status};return true;
    }
};
} }
