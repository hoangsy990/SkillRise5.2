#pragma once

// Slayer-only isolated runtime QA controls.  This header is compiled only in
// the private SlayerBuild client; it is never part of a normal 5.2 build.

class CHARACTER;

namespace rise { namespace slayerqa {

#ifdef RISE_SLAYER_RUNTIME_QA
// QA-only breadcrumb sink shared by the isolated login probe.  It writes to
// SlayerRuntimeQA.log beside the private executable and is never compiled in
// normal 5.2 builds.
void AppendRuntimeQALog(const char* message);
bool HandleRuntimeQAHotKey();
void RunRuntimeQAAutoSequence();
bool IsRuntimeQAIconPanelVisible();
int RuntimeQASelectedSkillId();
const char* RuntimeQASelectedSkillName();
int RuntimeQASelectedTargetIndex();
bool RuntimeQASelectedTargetLive();
unsigned RuntimeQALastEventCount();
const char* RuntimeQALastResult();

// ReceiveMagic bridge used by the isolated Slayer client.  This is the
// actual 5.2 packet-to-runtime entrypoint; it only binds the five Slayer
// skill ids and leaves the legacy skill receive switch untouched.
bool DispatchSlayerNativeReceive(CHARACTER* source, CHARACTER* target,
    int skillId);
#endif

} }
