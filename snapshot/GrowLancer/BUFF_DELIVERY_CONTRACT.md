# Grow Lancer high-ID buff delivery — IN_PROCESS

## Receipt metadata adapter — 2026-09-13

UI follow-up: NewUIBuffWindow's outer loop formerly skipped every buff lacking
a legacy UserBuff timer row, preventing high-ID icon dispatch despite valid
membership. Exact424/425 now bypass that legacy-row requirement; other IDs stay
unchanged. Wrath never indexes UserBuff[-1], uses its private atlas, and its
tooltip renders verified name/description plus receipt Remaining(timeGetTime)
through native BuffTimeControl::GetStringTime(DWORD). No legacy timer registration,
gmtime conversion, expiry mutation or high-ID array insertion is used. Missing
receipt omits the time line. Native formatting below one minute uses GlobalText2308;
this is SS6 presentation, not S21 time-string parity. Wrath's legacy green timer
bar is not shown. Isolated client build PASS; layout/UTF8/time text in-game QA open.

Reset regression follow-up: WrathPendingReset now owns bookkeeping used by
CWsctlc. Bind rejects negative identities, an outstanding mask, or a different
owner before reset. Empty reset clears binding; repeated reset retains pending
identity/mask. Take consumes at most once and leaves outputs untouched when empty.
Native apply checks Bind before receipt or membership mutation. Focused constexpr
x86 W4/WX tests cover invalid/unbound capture, repeat reset, bit-mask merge,
rejected owner changes, one-shot consumption and rebind after drain. These tests
cover bookkeeping only, not actual character-slot generation or socket threading.

Deferred reset reconciliation: connection remembers index/key after successful
local application. Reset captures a two-bit mask from received424/425 metadata
before clearing it; repeated reset preserves pending work. ProtocolCompiler
consumes this work before reading packets, and the apply callback also consumes
it before a new owner binding (join-map can occur in the same queue drain).
Only matching current Hero pointer/index/key and Live owner is mutated; mismatches
discard stale work. Only masked, still-present IDs are unregistered. Socket
reset itself does not touch a character pointer or invoke rendering cleanup.
This is a RISE reconciliation policy, not inferred S21 wire semantics. Winmain's
message/scene loop invokes ProtocolCompiler each iteration, including no-packet
iterations. Same-key slot-generation and thread-safety stress remain runtime QA
requirements. Readiness is still disabled; no live packets or visual PASS.

Reconnect audit follow-up: ReconnectOnCloseSocket calls ReconnectViewportDestroy
when reconnect is enabled. Its old loop indexes Buff::GetBuff(k) while removing
entries; GetBuff indexes the CURRENT map and UnRegisterBuff erases that map,
so later entries can be skipped. The isolated import now unregisters exact424/425
by key before that legacy loop. Only present Wrath IDs invoke physical removal;
other buff logic is unchanged. Native CreateCharacterPointer subsequently clears
the complete buff map and resets scale to.9 before use, which covers join-map's
new owner slot. Isolated Win32 build PASS. Disconnect with reconnect disabled,
render between reset/teardown and receipt/owner reconciliation remain open; this
is not a claim that every disconnect path has been completed or runtime-tested.

Native application follow-up: validated FC Status callback now delegates to
ApplyGrowLancerWrathStatus. It rejects non-game socket, non-main scene, invalid
HeroIndex, mismatched Hero pointer/key, non-live/non-player owner and non424/425
before mutation. It stages receipt validation, stores metadata, then calls native
RegisterBuff(buff,owner,0) / UnRegisterBuff. The inspected high-ID classification,
exception and logical switches permit membership but have no timer/stat cases;
full uint32 duration remains in the receipt, never narrowed through the int API.
Postcondition checks requested membership; on mismatch restores receipt/scale.
Only the two exact IDs are admitted; native legacy packet parsing is untouched.
Full isolated Win32 build PASS. Readiness remains false: callback is wired but
not live. UI countdown access, reset-to-membership reconciliation, GS authoritative
generation and actual owner reuse/packet/visual QA still block activation.

Connection attachment follow-up: CWsctlc now owns one WrathReceiptCache alongside
its BuffClientSession. ResetGrowLancerBuffSession resets both. Existing Cleanup,
Create, Connect, Close, matching-socket shutdown/close and ReceiveCharacterList
therefore clear receipt metadata too. ReceiveJoinMapServer additionally resets
before allocating a new HeroIndex/owner slot, even if the socket survives.
The disabled FC receiver explicitly uses this combined reset before the shared
handshake helper (which knows only the session). Both full isolated client builds
passed. Metadata reset is NOT character membership removal: reconciliation and
native apply remain required before featureReady may become true. No live receipt.

GrowLancerWrathReceipt.h preserves uint32 duration and uint16 auxiliary without
passing duration through legacy RegisterBuff's signed int. Countdown refreshes
every receipt; first membership auxiliary stays unchanged while native membership
exists, while lastAuxiliary records the received value separately. Removal clears
only that ID; Reset clears both metadata records. No countdown expiry removes
membership. Existing native membership with missing receipt rejects an add until
reconciled instead of inventing the original auxiliary. This is an explicit RISE
safety gate, not a claimed S21 recovery behavior.
Focused x86 constexpr tests W4/WX PASS, including UINT32_MAX, zero duration,
refresh/aux preservation, invalid ID, independent IDs, remove/reapply and reset.
This cache is not yet attached to a socket/owner or the native callback. Next:
attach with all owner/session resets before any callback mutation; preserve the
disabled transport gate until authoritative activation and reconciliation work.

The shared `GrowLancerBuffStatusCodec.h` is a RISE-designed v1 payload, not an
assertion about the S21 packet layout. The isolated envelope below reserves an
opcode, but is not connected to client dispatch, GameServer send, or live buff activation.

Native WSclient.cpp case0x2D sends the same buffer to ReceiveBuffState and
CustomInfoIconBuff::Recv. Their existing schemas use byte buff IDs. Do not
widen one reader alone, append high bytes to legacy2D, or alias424 to168.

This first payload covers local-owner Wrath424/425 only. Ten explicitly encoded
bytes: version1, operation0(add/refresh)/1(remove), uint16LE buff ID,
uint32LE durationSeconds, uint16LE auxiliary. The auxiliary field is preserved
without inventing its gameplay meaning. No struct memcpy/packing dependency.
It rejects unknown version/operation/ID and non-exact length before modifying
the output event. Character skill278 is deliberately rejected as a buff ID.

Remaining transport gates: unique envelope/subcode inventory, server/client
capability agreement, authoritative server cast/buff generation, client metadata
classification, snapshot/reconnect/removal and remote viewport behavior. This
payload does not implement those gates or expand legacy skill/effect slots.

Focused constexpr x86 tests cover both IDs/operations, full-width duration,
endianness, null/truncated/oversized buffers, invalid version/operation/skill ID,
and unchanged decode output on failure. These are codec tests, not network QA.

## Isolated envelope reservation

`GrowLancerBuffEnvelope.h` reserves C1/FC with literal GL signature, version1,
kind1 Hello /2 Accept /3 Status, nonzero uint32LE session tag. Header11 bytes;
Status appends the10-byte payload (total21). Other lengths/versions/kinds,
zero tag or bad signature fail before output mutation. Session tag is only a
stale-session discriminator, NOT authentication; transport must supply trust.

Inventory: native GS Protocol.cpp rootFC currently breaks without handler;
client WSclient.cpp has no rootFC case. Relevant literal FC/252 search found
crypto keys, colors and unrelated values, not another packet sender. Protocol
code macros useD3/D7/DF/10 orD4, notFC. This is a checkout-scoped reservation,
not universal MU compatibility or proof about dynamically generated opcodes.
Existing production and legacy2D remain unchanged.

Envelope round-trip and malformed-frame constexpr tests pass x86. Required
next step: per-connection handshake gate and resets, then actual client/server
wiring with feature readiness checks. No live packets are emitted yet.

## Connection-state contract

`GrowLancerBuffSession.h` now supplies caller-owned client/server handshake
state. Client starts only with feature-ready and fresh nonzero session tag;
accept must match a pending tag. Server accepts only Hello and rejects a tag
change until reset. Status cannot be built until Accept is queued on that
same ordered connection. Client rejects status before Accept, wrong kind,
wrong tag and non-Wrath IDs. Server never accepts client-originated Status.

Both objects require Reset on disconnect, character change and socket/index
reuse. A new nonzero tag must be supplied for a new session; the helper neither
generates tags nor authenticates a peer. A caller must decode/validate envelope
before invoking these methods. Feature-ready cannot be set merely because this
codec builds: actual metadata/lifecycle/server support must be installed.

Focused constexpr tests cover disabled readiness, unsolicited/wrong-tag Accept,
pre-ack Status, reversed direction, reset/reconnect stale frames, remove, invalid
skill ID and server disable. These prove state-machine behavior, NOT actual
reset hook coverage, transport send success or live runtime activation.

## Client socket ownership installed

CWsctlc now owns a BuffClientSession per instance. Create, Connect, Close,
Cleanup reset it at entry; ShutdownConnection and Close(SOCKET&) reset only
when targeting that instance's socket. ReceiveCharacterList resets the game
socket session. This covers the inspected direct Close callers in reconnect
and Winmain through the central method. Further character transition and
socket reuse audits remain; server object ownership is not installed yet.

WSctlc.cpp was CP949 (one Korean comment) and is now UTF8 BOM. A decode/encode
roundtrip was byte-equal before normalization; no comment translation or
behavioral rewrite was used. Full isolated Global Release Win32 build PASS.
No Begin/Accept/status dispatch is enabled, so feature readiness stays false.

## Server object ownership installed

OBJECTSTRUCT now appends BuffServerSession after existing fields; the bot reward
record is unchanged. gObjCharZeroSet, gObjDel and CharacterGameClose reset it.
Reset occurs before offline/not-online early exits after index validation.
Allocation uses new OBJECTSTRUCT followed by existing zeroing; session storage
contains only unsigned tag/bool readiness, no heap ownership. Character DB
serialization has not been extended to include session state.

Full Ex603 Win32 build passed with OutDir/IntDir explicitly within the isolated
GrowLancerBuild/Server folder and pre/post build events disabled. Initial compile
caught a misplaced declaration in adjacent reward struct; corrected to the
OBJECTSTRUCT tail before the successful build. No production deployment occurred.
Actual connection concurrency, handoff/offline-agent behavior and handshake
dispatch still require tests; build does not prove those lifecycle paths.

## Server Hello dispatcher installed, readiness disabled

ProtocolCore intercepts FC before legacy buffer reads and calls the bounded
GrowLancerBuffDelivery wrapper. It checks user index, online user type, socket,
context and input size. ReceiveBuffHello decodes the envelope, allows Hello
only, queues an encoded Accept and commits readiness only on successful queue.
Queue failure resets state, including a previously ready session.

Native CSocketManager::DataSend copies into its own main/side buffer. True
means queued or WSASend accepted/pending, not remote delivery. False paths
include invalid/offline socket, overflow and send failure. The handshake uses
that return value, not an unconditional success assumption.

featureReady remains explicitly false in the native wrapper: authoritative GS
Wrath generation and client metadata/lifecycle are not wired. Therefore current
runtime emits no Accept or Status. Fake-queue tests cover malformed/truncated
Hello, disabled readiness, success and failed-send reset. Ex603 Win32 build PASS.

## Client bounded receiver installed, readiness disabled

CWsctlc::GetReadMsg(int*) exposes the queued CPacket size before PopPacket;
the original no-argument overload delegates to it. ProtocolCompiler intercepts
plaintext C1/FC before legacy dispatch, using that actual length. Only the game
socket outside translation mode passes frames to ReceiveBuffServerFrame.
The shared helper rejects malformed envelopes, unsolicited Accept and Status
before readiness; disabling readiness resets the session. Status callback runs
only after session and ID validation. Native featureReady is false and the
callback performs no mutation: no Hello sender or live buff activation is enabled.

Focused client frame constexpr tests PASS, and isolated Global Release Win32
MSBuild exited 0. No QA deployment or live packet/visual acceptance was performed.
