# Grow Lancer Season 21 -> RISE PC 5.2/SS6

STATUS: IN_PROCESS

## CURRENT PHASE

Native-reference-driven RISE 5.2 visual correction plus fail-closed GameServer catalog integration after static client-chain recovery and isolated asset staging.

## CURRENT TASK

Reverse and port Spin Step, Circle Shield, Obsidian, Magic Pin, Clash, Harsh Strike, Shining Peak, Wrath and Breche as a package independent from Wind Soul and Pegasus.

Owner decision: final recipient is a NEW native Grow Lancer class in 5.2.
Immediate testing uses DK in the isolated QA client only. This does not authorize
changing production DK learning/damage rules or treating DK equipment as Breche
parity. Final class creation, equipment, packets and persistence are now in scope;
numeric class IDs/DB encoding remain evidence-gated.

## CURRENT SUBTASK

Compare each isolated 5.2 visual chain with its captured native S21 sequence,
closing primitive ownership and render arguments directly from the pinned dump
before owner gameplay QA, while retaining fail-closed class/packet boundaries.

## COMPLETED THIS RUN

- Circle F9 paired-target QA fixture added and staged:
  REVERSE EVIDENCE: native EffectManager07 has state/one index/BYTE effect;
  periodic2D has group/value/state/time/BYTE effect, no two-actor contact.
  ReiDoMU opcode75 is GS->DataServer ranking traffic, NOT client buff contact;
  no cross-channel opcode collision claim or guessed replacement introduced.
  COMPLETED THIS RUN: QA-only F9 skill272 invokes Contact(Hero,selected),
  rejects absent/dead/self caster,target check already exists. Explicit log
  circle-contact-qa-only-first-hero-second-selected, no packet/cost/damage.
  CHAIN COVERAGE: manual visual entrypoint, not authoritative receiver.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: RuntimeQA.cpp,RUNTIME_QA.md,state.
  LAST STATIC CHECK: build compiles guarded QA path; no gameplay QA performed.
  LAST BUILD RESULT: PC x86 exit0,known cryptlib LNK4099 warnings.
  Bin AND staged Client SHA256 E6FED7D0C85494143A10EF5F519EE536891459512CE4AB2A81F580C0B70B9DAD.
  No running Engine-Port process; non-reparse target/hash prechecks passed.
  Recoverable backup Bin/Engine-Port S21.pre-circle-contact-0EA22D64.exe
  verified matches prior0EA22D64. GS5D5BD2DF unchanged, no server deployment.
  LAST RUNTIME QA: NOT RUN; game not launched. KNOWN DIFFERENCES: manual
  local fixture is not buff proc or server-authoritative hit validation.
  MISSING CHAIN PARTS: native two-actor event contract,GS proc/AG cost,class,
  paired visual QA and all9 acceptance. BLOCKER: none for offline work.
  NEXT EXACT ACTION: audit Circle joint pool ownership and missing buff216
  server proc requirements; verify QA stage manifest after EXE-only staging.
  NEXT FILE TO OPEN: stage verifier, Circle server config/source requirements.
  NEXT FUNCTION TO IMPLEMENT: proven Circle server event/receive contract.

- Circle received-contact native adapter compiled (trigger still OPEN):
  COMPLETED THIS RUN: CreateCircleShieldContact(firstActor,secondActor)
  emits controller1 at actor2, facing actor1, followed by three joint3
  launches from actor1 per recovered offsets/angles. Actor2 owns all four.
  No animation reset/sound/send/cost added. Defensive Live checks only.
  REVERSE EVIDENCE: receiver129F04C..129F60C. Source contract verifier
  checks counts/ownership/offsets and absent action/sound/send mutations.
  Updated renderer audit's stale subtype2-only tail-scaling guard for2/3;
  UV2 inversion reused natively, subtype3 draws both faces.
  CHAIN COVERAGE: visual adapter, NOT registered incoming packet handler.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: EffectRuntime.cpp/.h,
  verify_packet_effect_lead.py,verify_circle_joint_renderer.py,state.
  LAST STATIC CHECK: contact and31instruction renderer verifier PASS.
  LAST BUILD RESULT: initial missing CreateAngle declaration fixed with
  native ZzzAI.h include; isolated PC x86 rebuild exit0, cryptlib warnings.
  Bin SHA256 24AA014AE1A793FD2AC1D328B6980CFF160B833D830EFED330B4928164FE8581.
  NOT STAGED; client test remains0EA22D64,GS5D5BD2DF unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: defensive Live check; live packet
  authority/receiver and visual comparison OPEN. MISSING CHAIN PARTS:
  native authoritative trigger,paired actor QA,final class/GS/all9visual.
  BLOCKER: none offline. NEXT EXACT ACTION: inspect native incoming buff
  event/GS send contracts; separately add explicit isolated paired-target
  QA invocation without treating client QA event as server authority.
  NEXT FILE TO OPEN: WSclient.cpp, GrowLancerRuntimeQA.cpp and GS buff packets.
  NEXT FUNCTION TO IMPLEMENT: verified receiver or isolated contact QA trigger.

- Circle contact multi-tick lifecycle helper integrated and tested:
  COMPLETED THIS RUN: extracted real runtime accumulator/decrement loop into
  AdvanceCircleContactJoint; native movement/homing/tail callback unchanged.
  Added constexpr tests for factors1,.5,.25,2.5; no-contact life100..0 yields
  101 callbacks/tails and15 early-move ticks. Contact death still decrements
  once, stops catchup with remainder1.5; dead/nonpositive/fractional gates pass.
  REVERSE EVIDENCE: common S21 tail-before-decrement ordering retained.
  CHAIN COVERAGE: actual helper lifecycle, synthetic callback; NOT actual
  homing geometry or in-game tail rendering acceptance.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: CircleJoint.h,ZzzEffectJoint.cpp,
  CircleContactGatesTest.cpp,state. LAST STATIC CHECK: x86 /W4 /WX constexpr
  compile PASS; packet evidence/project integration verifiers PASS.
  LAST BUILD RESULT: isolated PC x86 exit0; cryptlib PDB warnings unchanged.
  Bin SHA256 55D9F58F017D6E1A197343C1F79A02D48DF121F14490D8FDE79379E3DAB7611F.
  NOT STAGED: Client0EA22D64,GS5D5BD2DF unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: owner defensive guard; geometry/render/pool reuse open.
  MISSING CHAIN PARTS: receiver trigger,renderer audit,final class/GS/all9QA.
  BLOCKER: none offline. NEXT EXACT ACTION: inspect native renderer reverseUV2
  around7478 and S21 equivalent for subtype3 before paired-target activation.
  NEXT FILE TO OPEN: ZzzEffectJoint.cpp RenderJoints and S21 joint renderer.
  NEXT FUNCTION TO IMPLEMENT: verified native Circle buff-contact trigger.

- Native Circle contact subtype3 constructor/update implemented:
  REVERSE EVIDENCE: allocation15C6877 enables generic movement;15C66CA
  enables tails;15C6676 face3.1327059 returns D2E703 sqrt(x*x+y*y+z*z).
  Common1618ADA tails with pre-turn matrix even after contact death, then
  decrements life. Native branch preserves these movement/handler/tail steps.
  COMPLETED THIS RUN: subtype3 constructor life100/tails10/velocity10,
  random sign,UV2,RGB(.5,.4,1); private whole-tick movement,extra early
  displacement,target+120Z,homing,contact/brake,rand consumption and cleanup.
  Added null/non-live target rejection as explicit native safety guard.
  CHAIN COVERAGE: primitive compiled, no buff receiver activation yet.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: ZzzEffectJoint.cpp,Circle doc,
  acceptance matrix,state. LAST STATIC CHECK: dump verifier/project includes
  PASS; git diff --check no whitespace errors (existing CRLF warnings).
  LAST BUILD RESULT: isolated PC x86 build exit0; known cryptlib LNK4099.
  New Bin SHA256 50AE0EDD3F046F663312A5469B9B90B153F9ACE2D019186F4843B1B3652197B8.
  NOT STAGED: Client remains0EA22D64; GS5D5BD2DF unchanged.
  Initial hash attempt during linker lock failed, retried after build exit0.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: defensive target guard; full
  renderer/pool lifecycle unverified. MISSING CHAIN PARTS: receiver,subtype3
  runtime regression,final class/GS authority/all9 visual. BLOCKER: none offline.
  NEXT EXACT ACTION: add focused multi-tick contact lifecycle tests and audit
  source joint render/texture scroll fields before receiver integration/staging.
  NEXT FILE TO OPEN: ZzzEffectJoint.cpp native subtype3 and joint render path.
  NEXT FUNCTION TO IMPLEMENT: native Circle contact receiver with verified IDs.

- Circle subtype3 handler gates transcribed and boundary-tested:
  REVERSE EVIDENCE: subtype dispatch1612E42/49 ->161303C. Life>85 invokes
  local(0,-10,0) movement13272A0; otherwise velocity+=7 capped40. Target
  position refreshed from owner then Z+=120. Homing1327059 returns distance:
  <=35 kills; (35,70] plus absolute raw yaw delta>20 and velocity>=10
  subtracts50 (may go negative). Random local light calculation follows,
  even after Live=false, then common tail path1613428->1618ADA.
  COMPLETED THIS RUN: added constexpr handler gates to existing CircleJoint
  header and13 boundary static_assert tests. Not connected to runtime yet.
  CHAIN COVERAGE: scalar thresholds, not whole target/move/render lifecycle.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: CircleJoint.h,new contact gate
  test,verify_packet_effect_lead.py,state.
  LAST STATIC CHECK: pinned handler branch/constants PASS; x86 cl C++17
  /W4 /WX test compile PASS (CircleContactGates.obj, isolated).
  LAST BUILD RESULT: full client0EA22D64/GS5D5BD2DF unchanged; helper only.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: subtype3 still not activated.
  MISSING CHAIN PARTS: generic movement gate,tail/render/default initialization,
  owner validity contract,receiver,class/GS/all9visual. BLOCKER: none offline.
  NEXT EXACT ACTION: verify generic pre-move eligibility for80E3/subtype3,
  return metric of1327059 and default tail/render fields before wiring helper.
  NEXT FILE TO OPEN: S21 15E58DB,1327059,1618ADA; native ZzzEffectJoint.cpp.
  NEXT FUNCTION TO IMPLEMENT: AdvanceCircleContactJoint native tick adapter.

- Circle contact gap confirmed, acceptance corrected to PARTIAL:
  REVERSE EVIDENCE: complete216/221/222 block129F04C..129F60C contains
  one5DD/subtype1 and three80E3/subtype3 calls129F28A/129F447/129F604.
  All joint scale40,target/owner second object. Starts first object+Z100,
  and rotated(+100,50,0)/(-100,50,0); Z angles-135/-90/+135 respectively.
  Subtype3 constructor15DEA92->15DECA1 has life100/tails10/velocity10,
  random sign+/-1,reverseUV2. Native forcepillar only specializes subtype2;
  subtype3 currently falls into legacy life7/tails5, a real missing layer.
  COMPLETED THIS RUN: receiver call-count/constructor/constants verifier
  expanded; Circle doc and acceptance matrix corrected, no longer claiming
  whole visual chain implemented when contact subtype3 is absent.
  CHAIN COVERAGE: full direct receiver emissions; subtype3 update/render open.
  ASSET PROVENANCE: same80E3 forcepillar, unchanged staging.
  FILES MODIFIED: verifier,CIRCLE_SHIELD_REVERSE.md,ACCEPTANCE_GATE_MATRIX.md,state.
  LAST STATIC CHECK: hash-pinned continuous contact block1effect/3joints PASS.
  LAST BUILD RESULT: GS5D5BD2DF/client0EA22D64 unchanged, no game edits.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: contact subtype3 absent natively.
  MISSING CHAIN PARTS: subtype3 movement/render/cleanup,receiver integration,
  class/GS authority and all9 visual QA. BLOCKER: none offline.
  NEXT EXACT ACTION: trace subtype3 update161303C onward and its dispatcher,
  then implement isolated joint3 using proven native lifecycle fields.
  NEXT FILE TO OPEN: mapped dump161303C, ZzzEffectJoint.cpp.
  NEXT FUNCTION TO IMPLEMENT: Circle contact joint3 update/constructor.

- Buff-contact receiver identified (supersedes prior opcode uncertainty):
  REVERSE EVIDENCE: opcode75 table130505C ->1300320 ->129E990; pinned
  diagnostic says buff-effect delivery. Config hash201452C0 names buff275
  Bleeding, NOT Clash; buff216 Circle Shield. Actor fields4/5,6/7 and buff
  selector8/9 are big-endian pairs.216/221/222 branch emits5DD subtype1
  owned by second object at129F14B; full remaining receiver emissions OPEN.
  COMPLETED THIS RUN: expanded verifier with dispatcher table/call/string
  and hash-pinned buff names; added separate Circle incoming contact gate.
  CHAIN COVERAGE: new incoming trigger, not just cast root. No native packet
  or server mapping guessed. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: verify_packet_effect_lead.py,CIRCLE_SHIELD_REVERSE.md,state.
  LAST STATIC CHECK: receiver selector/dispatch/config PASS.
  LAST BUILD RESULT: GS5D5BD2DF/client0EA22D64 unchanged (docs/tests only).
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: incoming buff-contact integration
  unproven. MISSING CHAIN PARTS: full receiver,grant,class,GS,all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace129F153..129F670 emissions
  and native counterpart before implementing Circle received-contact path.
  NEXT FILE TO OPEN: S21 receiver129E990 and native WSclient.cpp.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed native buff-contact adapter.

- Receive-side numeric275 lead anchored, not mislabeled Clash learning:
  REVERSE EVIDENCE: continuous decoding from prologue129E990 to129F006
  proves compared value at129EE43 comes from packet bytes8/9, big endian;
  value275 branches129EFC1 and calls17259CC with argument -7 at129F000.
  Caller rel32 found1300369 inside dispatcher prologue12FE380; opcode and
  callee semantics not yet established. Later same function compares216,
  221,222 and333 then creates5DD subtype1 at129F14B for first three:
  potential Circle contact path, NOT confirmed skill namespace.
  COMPLETED THIS RUN: added hash-pinned anchored verifier for receive lead.
  CHAIN COVERAGE: packet-derived selector, not auto-grant/equipment mapping.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: verify_packet_effect_lead.py,state.
  LAST STATIC CHECK: pinned dump + continuous instruction boundary PASS.
  LAST BUILD RESULT: GS5D5BD2DF/client0EA22D64 unchanged (reverse-only).
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: opcode/selector meaning open.
  MISSING CHAIN PARTS: grant, final class,GS authority,all9 visual QA.
  BLOCKER: none for offline trace.
  NEXT EXACT ACTION: decode dispatcher12FE380 to caller1300369 to establish
  packet opcode; identify17259CC before classifying value275 as skill/buff.
  NEXT FILE TO OPEN: pinned S21 mapped binary at12FE380 and17259CC.
  NEXT FUNCTION TO IMPLEMENT: confirmed receive/contact adapter only.

- Local integration audit extended (not Git merge acceptance):
  COMPLETED THIS RUN: verifier now checks all10 GS legacy-column consumers
  are unique, non-excluded ClCompile entries; resolves64 task-specific
  literal includes within this worktree using source-relative paths and
  the native project include roots. Client3cpp/24headers remain included.
  Initial verifier falsely rejected RISE/ZzzToolKit.cpp include via the
  configured Main5.2_RISE include root; corrected auditor, no game code fix.
  REVERSE EVIDENCE: 110026B..1100397 occurrences of110h are double-field
  offsets, not skill272 constants; discarded as learn-source evidence.
  CHAIN COVERAGE: local project/dependency inclusion only, not preprocessor
  reachability or completed class/skill integration. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: verify_project_integration.py, GROW_LANCER_STATE.md.
  LAST STATIC CHECK: 3cpp/24headers/10GS consumers/64includes PASS;
  class table migration and missing learn-source verifiers PASS.
  LAST BUILD RESULT: GS5D5BD2DF/client0EA22D64 unchanged; no rebuild needed
  for audit-only edits. LAST RUNTIME QA: none this run.
  KNOWN DIFFERENCES: source remains modified/untracked; no Git merge done.
  MISSING CHAIN PARTS: Circle/Clash grant, final class, server authority,
  all9 gameplay visual acceptance. BLOCKER: none for further offline reverse.
  NEXT EXACT ACTION: trace equipment-to-skill assignment from class7 path;
  ignore raw field-offset numeric xrefs and preserve fail-closed activation.
  NEXT FILE TO OPEN: S21 equipment skill construction and native ItemManager.
  NEXT FUNCTION TO IMPLEMENT: proven Circle/Clash grant contract only.

- Circle/Clash missing learn-source narrowed:
  REVERSE EVIDENCE: hash-pinned ClassQuest.xml,Warps/ClassQuest.xml and
  SkillRequire.xml have no active XML attribute value272/275; ItemList has
  no Item SkillIndex272/275. Eligible GL items with nonzero SkillIndex are
  8 rows18 and10 rows20. These are leads for equipment/class conversion,
  NOT proof18->272 or20->275. No guessed scroll added.
  Numeric push xrefs also hit unrelated text lookup1734024->11D8FA3 and
  allocator-sized9B488F->19B902C; numeric occurrence alone discarded as
  learn evidence. CHAIN COVERAGE: scoped config paths excluded, binary
  grant/conversion still open. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: verify_missing_learn_sources.py,state.
  LAST STATIC CHECK: source hashes,active XML scope,eligible item counts PASS.
  LAST BUILD RESULT: GS5D5BD2DF/client0EA22D64 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: Circle/Clash grant origin unresolved; absence in these
  tables does not establish no learn source. MISSING CHAIN PARTS: skill grant,
  final class/defaults,GS authority,all9 visual QA. BLOCKER: none offline.
  NEXT EXACT ACTION: trace client equipment SkillIndex processing and class7
  condition for18/20; trace Clash Brand75 separately, without equating Brand
  with a learn item or assuming a generic siege-class mapping.
  NEXT FILE TO OPEN: native item-to-skill code and S21 equipment-skill xrefs.
  NEXT FUNCTION TO IMPLEMENT: proven Circle/Clash grant contract only.

- Full skill-row tokenizer fixture executed:
  COMPLETED THIS RUN: harness full-skill mode verifies13 metadata fields
  before7 class columns for two rows, comments/quoted names/end/EOF, capacity
  7/8/16. Source verifier confirms exact Damage..RequireGuildStatus order.
  Added Test-LegacyClassTokenizer.ps1 reproducible isolated build/link/run
  using real MemScript/stdafx objects; no game/server launch.
  REVERSE EVIDENCE: actual SkillManager loader order matched. Fixture uses
  synthetic sentinels, NOT production values. No native Skill.txt found in
  scoped ExGameServer/Client file inventory, so no actual deployment-table
  acceptance is claimed. CHAIN COVERAGE: full row token consumption, not
  CSkillManager whole-load/map/catch/reload behavior.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: harness,full-row fixture,
  tokenizer test script,boundary verifier,state.
  LAST STATIC CHECK: exact13field order PASS; real tokenizer minimal+full-row
  executions PASS exit0. LAST BUILD RESULT: GS5D5BD2DF/client0EA22D64 unchanged;
  rebuilt test EXE only. LAST RUNTIME QA: tokenizer harness only.
  KNOWN DIFFERENCES: class defaults/learning/server authority remain missing.
  MISSING CHAIN PARTS: actual full-table regression,final class,GS handlers,
  all9 visual QA. BLOCKER: no real native deployment tables in inspected scope;
  other skill identity/learning reverse can proceed.
  NEXT EXACT ACTION: return to unresolved Circle Shield272 and Clash275 learn
  sources; inspect supplied ClassQuest/SkillRequire and client registration
  evidence rather than filling missing scroll IDs by adjacency.
  NEXT FILE TO OPEN: S21 Data/ClassQuest.xml and Skills/SkillRequire.xml(read-only).
  NEXT FUNCTION TO IMPLEMENT: evidence-backed missing learn-source contract.

- Real CMemScript regression executed:
  COMPLETED THIS RUN: new isolated harness links actual GS MemScript.obj
  and stdafx.obj with /LTCG, opens fixture via real SetBuffer/GetToken. Two
  rows901/902,quoted names,//comments,blank lines,7values,end/EOF all checked
  at capacities7/8/16; extra columns zero and next row preserved. Exit0.
  REVERSE EVIDENCE: full native MemScript inspected; no row boundary API,
  GetAsNumber advances token; helper changes only fixed column consumption.
  CHAIN COVERAGE: actual tokenizer + helper execution, not whole config parser.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: MemScriptColumnsTest.cpp,
  Tests/fixtures/legacy_class_columns.txt,class integration doc,state.
  LAST STATIC CHECK: harness /W4/WX compile PASS; link initially needed ATL
  lib path and stdafx PCH object, then /LTCG link+execution PASS. No tokenizer
  source edits/workarounds. LAST BUILD RESULT: GS5D5BD2DF and client0EA22D64
  unchanged; new test EXE only. LAST RUNTIME QA: local tokenizer harness only,
  no game/server main/network/DB launch. KNOWN DIFFERENCES: malformed inputs,
  full real-table loading and live reload remain unverified.
  MISSING CHAIN PARTS: full configs,class defaults,GS authority,all9 visual QA.
  BLOCKER: none for offline regression. NEXT EXACT ACTION: run representative
  actual seven-column skill/item/quest configs through isolated parser harness
  or validate their full token order; do not equate small fixture with every
  loader. Also keep missing final class DB defaults visible as external input.
  NEXT FILE TO OPEN: native test/config locations and skill loader field order.
  NEXT FUNCTION TO IMPLEMENT: full-row legacy skill parser regression.

- Six remaining identified legacy RequireClass readers migrated:
  COMPLETED THIS RUN: QuestObjective,QuestReward,QuestWorld,
  QuestWorldObjective,QuestWorldReward and ItemBagEx now read exactly7
  columns via shared helper. All original preceding fields/SetInfo or map
  insertion preserved; ItemBagEx section4 only, other sections unchanged.
  REVERSE EVIDENCE: actual loader bodies inspected, source-order assertions
  now cover all8 quest/master/bag positions plus skill/item checks.
  CHAIN COVERAGE:10 identified readers decoupled from class capacity; no
  MAX_CLASS change or new class activation. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: six namedcpp,boundary verifier,class integration doc,state.
  LAST STATIC CHECK: boundary/source placement PASS; focused helper x86/W4/WX
  compile PASS; former RequireClass[n]/GetAsNumber pattern has no matches.
  This is not proof of all config consumers/full actual file parsing.
  LAST BUILD RESULT: isolated Ex603 Win32 PASS exit0,postbuild disabled.
  GS SHA2565D5BD2DFEE7457D65A95C9A98FACC7E6C6174386CF09B96507E70CF38784DE10.
  Client Bin/staged0EA22D64 unchanged. LAST RUNTIME QA: none/no deployment.
  KNOWN DIFFERENCES: final class/defaults/rate semantics still incomplete.
  MISSING CHAIN PARTS: actual legacy file parser regression,remaining class
  consumers,defaults,GS authority,all9 visual QA. BLOCKER: none for tests.
  NEXT EXACT ACTION: add integration regression using real CMemScript on
  isolated legacy-format fixtures, checking consecutive rows/comments/end
  markers at capacity7 and8; do not accept compile alone as parser proof.
  NEXT FILE TO OPEN: MemScript.cpp and existing GameServer test harness.
  NEXT FUNCTION TO IMPLEMENT: real-tokenizer legacy-column regression harness.

- Quest/master legacy class-column compatibility implemented:
  COMPLETED THIS RUN: Quest Load reads7 columns afterRequireMaxLevel;
  MasterSkillTree Load reads7 afterRequireSkill[1]. Reused bounded helper;
  existing prior fields/SetInfo/map insertion and master links unchanged.
  REVERSE EVIDENCE: both full loader prefixes inspected; no line autodetection.
  CHAIN COVERAGE: four loaders(skill/item/quest/master) now schema-bound,
  class capacity still7. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: Quest.cpp,MasterSkillTree.cpp,shared helper comment,
  LegacyColumnsTest.cpp,boundary verifier,class integration doc,state.
  LAST STATIC CHECK: source-order checks PASS; consecutive synthetic rows901/
  902 retain boundaries at capacity8; x86/W4/WX focused compile PASS.
  LAST BUILD RESULT: Ex603 Win32 isolated PASS exit0; private OutDir/IntDir,
  postbuild disabled. GS SHA256:
  6F32D1DD7D3503324E400C904A7F6B480C5162BC9F5794AA8F29B99F405CF08E.
  Client Bin/staged0EA22D64 unchanged. LAST RUNTIME QA: none/no deployment.
  KNOWN DIFFERENCES: six matching readers remain; final class not activated.
  MISSING CHAIN PARTS: remaining schemas,defaults,GS authority,all9 visual QA.
  BLOCKER: none for bounded reader changes. NEXT EXACT ACTION: inspect and
  migrate QuestObjective,QuestReward,QuestWorld,QuestWorldObjective,
  QuestWorldReward,ItemBagEx with guards/following fields preserved; then
  scan for other class-count-driven config reads before considering capacity.
  NEXT FILE TO OPEN: QuestObjective.cpp and QuestReward.cpp Load.
  NEXT FUNCTION TO IMPLEMENT: remaining verified legacy class-column reads.

- Legacy item class-column migration correction:
  COMPLETED THIS RUN: generic ReadLegacyClassColumns shared core with old
  skill wrapper preserved. ItemManager Load uses7-token core only inside
  existing section!=14 guard. Existing special GetToken for<=11/13 retained;
  no item permission/cast/class activation changes.
  REVERSE EVIDENCE: native item parser section branches explicitly inspected.
  CHAIN COVERAGE: item/skill readers capacity-independent; quest/master pending.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: Shared helper,ItemManager.cpp,
  LegacyColumnsTest.cpp,class boundary verifier,class integration doc,state.
  LAST STATIC CHECK: capacities7/8/16 and section0..15 constexpr x86/W4/WX PASS;
  actual item section guard + source boundary verifier PASS. Does not prove
  full item-file runtime parse. LAST BUILD RESULT: Ex603 Win32 isolated PASS
  exit0, private output/intermediate dirs and postbuild disabled. GS SHA256:
  2F60EBF75FE26B256EFF274B3584DED0A45C7ABCBF457D37F5CC905E69A15FBA.
  Client Bin/staged0EA22D64 unchanged. LAST RUNTIME QA: none/no deployment.
  KNOWN DIFFERENCES: final GL class and authority still absent.
  MISSING CHAIN PARTS: quest/master schemas,defaults,GS handlers,all9 visual QA.
  BLOCKER: none for bounded loader migration. NEXT EXACT ACTION: inspect
  Quest.cpp and MasterSkillTree.cpp class-column placement and following tokens;
  preserve native7 grammar with explicit tests before future class expansion.
  NEXT FILE TO OPEN: Quest.cpp and MasterSkillTree.cpp Load bodies.
  NEXT FUNCTION TO IMPLEMENT: legacy quest/master class-column reads.

- Legacy skill class-column reader isolated from capacity:
  COMPLETED THIS RUN: Shared/LegacySkillClassColumns.h reads exactly7 numeric
  tokens, zeroes extra destination columns, compile-time rejects capacity<7.
  SkillManager Load uses it; no new format autodetection, no MAX_CLASS change,
  no class7 activation. Legacy seven-column files keep exact consumption.
  REVERSE EVIDENCE: MemScript token API has no row-boundary accessor; native
  old loader directly tied read count to MAX_CLASS. Separate package remains
  intended source of future GL permissions, not an extra legacy token.
  CHAIN COVERAGE: one real parser migration hazard fixed; item/quest/master
  readers remain separate. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: Shared helper,SkillManager.cpp,LegacyColumnsTest.cpp,
  class boundary verifier,class integration doc,state.
  LAST STATIC CHECK: constexpr7/8/16 capacities + next-row sentinel x86/W4/WX
  PASS; boundary verifier PASS. LAST BUILD RESULT: Ex603 Win32 isolated PASS
  exit0, OutDir/IntDir private, postbuild disabled. New GS SHA256:
  D42C89B03BD93C6D1E2C94065B6DB30CFDE1557BB1D75B9CEA7EF721CE5084FC.
  Client Bin/staged0EA22D64 unchanged. LAST RUNTIME QA: no server launch/deploy.
  KNOWN DIFFERENCES: final new class and damage semantics still absent.
  MISSING CHAIN PARTS: other class-table parsers,defaults,GS handlers,visual QA.
  BLOCKER: none for remaining scoped parser work. NEXT EXACT ACTION: inspect
  item/quest/master MAX_CLASS token readers and apply the same schema/capacity
  separation where actual legacy7 columns are verified, with per-reader tests.
  NEXT FILE TO OPEN: ItemManager.cpp legacy RequireClass loader.
  NEXT FUNCTION TO IMPLEMENT: legacy item class-column compatibility read.

- S21 class-rate config evidence recovered:
  REVERSE EVIDENCE: hash-pinned CalcCharacter.ini BE300D43... [PvPDamage]
  supplies all15 directed GL/legacy rates with decimals; [PvMDamage] GL555,
  stuck10. Section-sensitive DLToGL is16.31879461 in PvPDamage but5.0 in
  DarkSpirit. Native stores int matrix and applies damage*rate/100 after
  general/attacker multipliers. Blind GetPrivateProfileInt import loses decimals;
  whole-matrix float replacement would alter legacy behavior.
  CHAIN COVERAGE: missing configuration values recovered, S21 actual runtime
  rounding/order still unproven. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: verify_s21_class_damage_rates.py,class integration doc,state.
  LAST STATIC CHECK: pinned15rates/section identity/PvM/stuck/native boundary PASS.
  LAST BUILD RESULT: Bin/staged0EA22D64,GS C85CEF94 unchanged; no runtime edit.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: decimal vs integer class rates.
  MISSING CHAIN PARTS: rate application semantics,class defaults,GS handlers,
  all9 visual QA. BLOCKER: supplied GS runtime handler evidence still missing;
  config alone cannot justify guessed rounding. NEXT EXACT ACTION: inspect
  native skill-table parser API for versioned class-column support that retains
  seven-column legacy files; keep class7 activation off until full inputs exist.
  NEXT FILE TO OPEN: SkillManager.cpp loader and MemScript token/line API.
  NEXT FUNCTION TO IMPLEMENT: scoped versioned GL class-column reader.

- Class table migration audit:
  REVERSE EVIDENCE: current MAX_CLASS7 controls SkillManager text row
  GetAsNumber count; increasing to8 also changes file grammar, risking next
  token consumption on legacy rows. ServerInfo loads exactly49 explicit
  directed damage matrix cells0..6, not a capacity loop; class7 needs15 more
  cells with verified values. DefaultClassInfo zero-initializes rows and
  currently rejects7; array expansion cannot establish missing default stats.
  CHAIN COVERAGE: concrete parser/config migration prerequisites identified.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: class integration doc,
  verify_class_table_migration_boundary.py,state.
  LAST STATIC CHECK: seven-column parser/49-cell matrix/default init PASS.
  LAST BUILD RESULT: Bin/staged0EA22D64,GS C85CEF94 unchanged; no runtime edit.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: new class not activated.
  MISSING CHAIN PARTS: class defaults/15rates/versioned grammar,GS handlers,
  all9 visual QA. BLOCKER: actual default-class DB row remains missing;
  available S21 rate/config inspection can proceed without inventing it.
  NEXT EXACT ACTION: inspect supplied S21 PvP/PvM class rate configuration
  for GL directed matrix entries; design versioned class-column migration
  preserving legacy seven-class skill data before capacity change.
  NEXT FILE TO OPEN: source S21 server class damage-rate configs(read-only).
  NEXT FUNCTION TO IMPLEMENT: evidence-backed class config compatibility reader.

- Telemetry staging/reference and log reader checkpoint:
  COMPLETED THIS RUN: no running Engine-Port S21; checked private paths
  non-reparse and exact old/new hashes; backed up FA39CD60 to RuntimeQA/Bin/
  Engine-Port S21.pre-sampling-FA39CD60.exe, then staged0EA22D64.
  REVERSE EVIDENCE: reopened S21_NATIVE_REFERENCE.md and actual2136frames.csv
  (SHA E520671C...),result4CD0352E...,Shiningframe7947BF2BE0C... match pins.
  CSV columns are frame,utc_iso,tick_ms,left,top,width,height,file: screenshot
  timing/geometry only, no actual animation frame or attack speed. This is
  visual evidence, not controller sampling telemetry.
  CHAIN COVERAGE: new inspect_shining_qa_samples.py reads bounded batches,
  flags incomplete/truncated/nonfinite records, reports observed stages/action
  end. Complete log batch explicitly does NOT mean complete cast/parity.
  ASSET PROVENANCE: unchanged; FILES MODIFIED: stage EXE/backup,stage verifier,
  sample log reader,state. LAST STATIC CHECK: stage2658base/79overlay/3root PASS;
  synthetic parser empty/incomplete/valid/dropped/nonfinite cases PASS.
  LAST BUILD RESULT: Bin AND Client0EA22D643B4D51BC577B4923F7302500BDE3DA1F64C5EBB50C07B08041994EBD;
  GS C85CEF94 unchanged. LAST RUNTIME QA: not launched; no new samples.
  KNOWN DIFFERENCES: source screenshot reference lacks per-cast numeric state.
  MISSING CHAIN PARTS: real cast telemetry,final class/GS,all9 visual QA.
  BLOCKER: per-cast source data missing; other offline implementation remains.
  NEXT EXACT ACTION: audit final class/GameServer acceptance requirements and
  available server contracts while owner is unavailable for paired runtime QA;
  do not keep treating screenshot cadence as controller frames.
  NEXT FILE TO OPEN: ACCEPTANCE_GATE_MATRIX.md and server catalog/class contract.
  NEXT FUNCTION TO IMPLEMENT: next evidence-backed missing server/class behavior.

- Bounded QA-only Shining controller telemetry implemented:
  COMPLETED THIS RUN: F7 skill277 arms fixed512-sample buffer. Local Hero
  subtype0 controller pre-update records frame,action,factor,stage,lifetime.
  No disk writes/allocations/game mutation in sampling callback. F11 or next
  cast flushes samples with count/dropped header and disarms. Saturating drop
  counter reports truncation. One isolated cast only; overlapping roots are
  not distinguished. Existing before/after cast input logs supply attack speed.
  REVERSE EVIDENCE: samples the proven owner/frame/controller-stage fields
  in native runtime; not fabricated source evidence. CHAIN COVERAGE: telemetry
  prepared, no actual sample/visual result yet. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: GrowLancerRuntimeQA.cpp/.h,GrowLancerEffectRuntime.cpp,
  verify_shining_qa_sampling.py,state.
  LAST STATIC CHECK: bounded QA scope PASS; native speed/project checks PASS.
  LAST BUILD RESULT: isolated Global Release Win32 PASS exit0; known cryptlib
  LNK4099 warnings. New BIN SHA256:
  0EA22D643B4D51BC577B4923F7302500BDE3DA1F64C5EBB50C07B08041994EBD.
  STAGED CLIENT remainsFA39CD6048DE398C1F1004CAAEB1F984849D09439845BBBBA85CE1AD7E1B7EAB;
  no staging this checkpoint. GS C85CEF94 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: sampling discrepancy unresolved; sampler cannot identify
  overlapping old/new controllers. MISSING CHAIN PARTS: source/native cast
  samples,final class/GS,all9 visual QA. BLOCKER: none for staging/offline work.
  NEXT EXACT ACTION: hash-guard backup/stage telemetry candidate; update stage
  verifier. Locate historical S21 visual-reference manifest separately; do not
  treat this new native telemetry as evidence of S21 actual frame values.
  NEXT FILE TO OPEN: verify_runtime_qa_stage.py and reference manifests.
  NEXT FUNCTION TO IMPLEMENT: actual-sample parser/comparison with source evidence.

- Bounded Shining sampling differential checkpoint:
  REVERSE EVIDENCE: compare_shining_sampling.py models float32 positive
  constant speed, start0, seven unlocked keys, stop-before-effect, sequential
  stage gates. At adjusted273 stages0/1/2 occur at25FPS and50FPS, all4 at60
  and144FPS; at10FPS only0/1. At adjusted100/10FPS no stage occurs because
  the first narrow gate is missed. Source25Hz/nativefactor1 replay agrees.
  Initial test assumption that273/50FPS gives4 was contradicted by replay;
  corrected differential uses60FPS, preserving actual arithmetic output.
  These are simulated cases, NOT observed casts or mandatory4-child policy.
  Existing artifacts/grow_lancer tree contains full dump/preflight/candidate,
  no named Shining frame capture. Active QA log has Spin271 events and no
  skill277/Frame-input match. Historical state records visual captures; images
  alone do not establish the speed/frame values used in the simulated cases.
  CHAIN COVERAGE: concrete FPS sampling discrepancy exposed, not fixed.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: compare_shining_sampling.py,state.
  LAST STATIC CHECK: bounded replay PASS after correcting50->60 test premise.
  LAST BUILD RESULT: Bin/stagedFA39CD60; GS C85CEF94 unchanged.
  LAST RUNTIME QA: none this checkpoint. KNOWN DIFFERENCES: variable sampling
  affects stage count. MISSING CHAIN PARTS: actual Shining samples,final class/
  GS,all9 visual QA. BLOCKER: no verified per-cast source frame telemetry yet;
  offline instrumentation preparation remains possible.
  NEXT EXACT ACTION: inspect reference-capture paths from historical state;
  add bounded QA-only Shining controller frame/stage diagnostics if absent,
  without changing runtime timings or global FPS behavior.
  NEXT FILE TO OPEN: GrowLancerRuntimeQA.cpp and historical reference manifests.
  NEXT FUNCTION TO IMPLEMENT: bounded local Shining sampling telemetry.

- Source clock identity closed:
  REVERSE EVIDENCE: parsed mapped PE OFT/import descriptors against pinned
  main SHA;1B4C4A0=kernel32.GetTickCount,1B4C4A4=kernel32.Sleep,
  1B4C9A8=winmm.timeGetTime.18EF05D saves oldBEACF44 into928;18EF068 reads
  GetTickCount then18EF06E storesBEACF44. Previous loop duration/sleep/carry
  arithmetic is milliseconds:40ms fixed simulation quantum,25Hz catch-up.
  Native REFERENCE_FPS=25(ZzzAI.h); variable factor multiplies frame step.
  At factor1 ordinary stepping agrees. Snapshot273/7keys misses final window
  under both rules; this is not proof of a native-only bug or actual cast speed.
  CHAIN COVERAGE: clock units/catch-up closed; actual cast samples still open.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: animation verifier,Shining doc,state.
  LAST STATIC CHECK: imports/clock writes and prior chain pins PASS.
  LAST BUILD RESULT: Bin/stagedFA39CD60,GS C85CEF94 unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: variable vs fixed sampling,
  not a baseline25Hz mismatch. MISSING CHAIN PARTS: source actual cast-frame
  evidence and native FPS equivalence,final class/GS,all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: inspect existing S21 cast captures/
  telemetry for Shining owner frames/speed; build bounded sampling comparison
  across native FPS factors without widening source emission windows.
  NEXT FILE TO OPEN: existing capture artifacts and verify_staged_action_clips.py.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed per-cast timing adapter if needed.

- S21 catch-up loop checkpoint:
  REVERSE EVIDENCE:18EEAAF subtracts40 from local8A4;18EEAE1 loops back
  to18EE826 threshold. Tail18EF1FD..258 computes BEACF44-local928 into89C,
  if below40 calls IAT1B4C4A4 with difference and sets89C=40, then adds the
  remaining8A4 into93C;18F1114 stores93C intoBEACF88. Units and IAT identity
  not yet proven, but repeated fixed-quantum update is now established.
  Exploratory full-dump check: its own18EE816 call targets108B83C0, NOT
  mapped-main dump108C83C0; full snapshot IGC.dll base10850000. Correct target
  calls native1326893 then DLL helpers. Do not reuse absolute DLL addresses
  across these captures. No runtime or protected-file changes.
  CHAIN COVERAGE: catch-up policy narrowed; clock producer still open.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: source animation verifier,state.
  LAST STATIC CHECK: catch-up and tail instruction pins PASS.
  LAST BUILD RESULT: Bin/stagedFA39CD60; GS C85CEF94 unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: native variable frame step versus
  source fixed-quantum update; actual units/sample sequence remain unverified.
  MISSING CHAIN PARTS: clock identity/frame samples,final class/GS,visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace BEACF44 and local928
  producers; resolve IAT1B4C4A4 and native1326893. Hash-pin full snapshot
  before turning exploratory DLL observations into a verifier contract.
  NEXT FILE TO OPEN: source1326893 and BEACF44 xrefs.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed controller timing correction.

- S21 scene identity and inner animation edge checkpoint:
  REVERSE EVIDENCE:18EE7E4 dispatch maps scene3 ->18F40BE, scene6 ->18F2EBF.
  Anchored scene6 throughRET18F3CE8 calls characters13F3FB0 at18F3BB1 before
  effects1595834 at18F3C30. Character1424B34 valid-model path writes owner
  scale/action to model74/A8 then calls1423BA4 at1424D62, which reaches
  14115BB at1423CA8. Earlier scene3 evidence alone was insufficient; scene6
  now separately inspected. Both observed orders match native characters-first.
  CHAIN COVERAGE: inner animation edge closed; no scene reorder justified.
  Outer update checks BEACF88>=40 after108C83C0(outside mapped main); units,
  counter producer and catch-up policy remain unproven, not assumed25Hz.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: animation verifier,Shining doc,state.
  LAST STATIC CHECK: inner edge/scene dispatch/scene6 order pins PASS.
  LAST BUILD RESULT: Bin/stagedFA39CD60; GS C85CEF94 unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: FPS-scaled native frame step;
  exact reference cadence and final-window samples still open.
  MISSING CHAIN PARTS: cadence/sample reconciliation,final class/GS,visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: inspect BEACF88 producers and
  remaining18EE7E4 tail/catch-up loop; use full dump only if external108C83C0
  is necessary and present. Do not label integer40 as milliseconds without proof.
  NEXT FILE TO OPEN: source18EE7E4 tail and BEACF88 xrefs.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed controller timing correction.

- S21 outer pool/character traversal checkpoint:
  REVERSE EVIDENCE: anchored1595834..15958E8 loops200 ordinary records
  (150 under DE7FE7 predicate), stride3D8, calls14B76C0 for live records,
  then10F027E secondary pool. Direct scene-call leads18F3C30/18F3D6A/18F4158.
  Anchored18F40BE conditional path calls13F3FB0 at18F4140 BEFORE1595834 at
  18F4158. Character traversal13F4101..411A loops400 entries into13F3EF8;
  its live-object body calls14356FB,1424B34,142F1E9 in order. Need connect
  1424B34 to1423CA8 with valid control flow before full scheduling claim.
  CHAIN COVERAGE: scene ordering narrowed; branch predicate/cadence not proven.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: source animation verifier,state.
  LAST STATIC CHECK: new pool/scene/traversal instruction pins PASS.
  LAST BUILD RESULT: Bin/staged FA39CD60,GS C85CEF94 unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: timing risk still open.
  MISSING CHAIN PARTS: inner character reachability,scene cadence,final class/
  GS,all9 visual QA. BLOCKER: none offline. NEXT EXACT ACTION: trace
  1424B34 character update into1423CA8 and identify18F40BE scene predicate;
  do not reorder native effects based solely on these partial call chains.
  NEXT FILE TO OPEN: pinned binary1424B34 and callers of1423CA8 container.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed controller timing correction.

- Action-end candidate staged plus native scheduler audit:
  COMPLETED THIS RUN: checked no Engine-Port S21 process and non-reparse
  private paths; backed up prior staged4F0D59C2 to RuntimeQA/Bin/
  Engine-Port S21.pre-action-end-4F0D59C2.exe, verified backup hash, then
  staged hash-pinned FA39CD6048DE398C1F1004CAAEB1F984849D09439845BBBBA85CE1AD7E1B7EAB.
  REVERSE EVIDENCE: native MoveMainScene calls MoveCharactersClient before
  MoveEffects; ordinary completed action stops before later controller read.
  Shining controller uses generic UpdateEffect path (not whole-tick list);
  children body/pin/spin/shockwave use AdvanceWholeTicks. Subtype0 requires
  owner action288 before sampling frame. More lifetime cannot repair a missed
  frame window after action transition. S21 outer cadence remains unproven.
  CHAIN COVERAGE: native schedule/path distinction recorded, not visual PASS.
  ASSET PROVENANCE: unchanged, all staged hashes checked.
  FILES MODIFIED: isolated staged EXE/backup,stage verifier,Shining doc,state.
  LAST STATIC CHECK: stage PASS2658base/79overlay/3root,Player CRC/hash PASS.
  LAST BUILD RESULT: Bin AND Client now FA39CD60; GS C85CEF94 unchanged.
  LAST RUNTIME QA: not launched. KNOWN DIFFERENCES: sampling risk remains.
  MISSING CHAIN PARTS: S21 scheduler parity, final class/GS, all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace source effect manager
  1594624 dispatch/15946E3 expiry outward to gameplay update order relative
  to character caller1423CA8; preserve exact source controller windows.
  NEXT FILE TO OPEN: pinned binary1594624 enclosing function/callers.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed Shining scheduling adapter.

- Private cast action-end integration correction:
  REVERSE EVIDENCE: source1423159..14232ED sends all185..194 through
  111<=action<327, excluding159/213/313, to140AF0E at14232BC. Native stock
  PlayerStopAnimationSetting did not cover appended284..293. Added private
  predicate plus MODEL_PLAYER/NumActions guard calling native SetPlayerStop;
  no legacy enum range expansion or clip/window change.
  CHAIN COVERAGE: ordinary imported action completion now mapped; effect
  scheduling/final-emission and runtime cleanup still need verification.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: ZzzCharacter.cpp,
  GrowLancerEffectRuntime.h,CastActionEndTest.cpp,source animation verifier,
  SHINING_PEAK_REVERSE.md,state.
  LAST STATIC CHECK: source end-branch pins PASS; exhaustive65536-action
  predicate test x86/W4/WX PASS; project3cpp/24headers PASS.
  LAST BUILD RESULT: isolated PC Global Release Win32 PASS(exit0), known
  cryptlib LNK4099 missing-PDB warnings. NEW Bin SHA256:
  FA39CD6048DE398C1F1004CAAEB1F984849D09439845BBBBA85CE1AD7E1B7EAB.
  Staged Client remains4F0D59C2153BFC4101269CEB4FB781964704E0C7D48BC4D276FD6F75F33A8EB2;
  no staging/deployment performed this checkpoint. GS C85CEF94 unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: Shining final-window sampling
  risk remains; completed casts now stop instead of implicit repeated cycles.
  MISSING CHAIN PARTS: scheduler/final emission, final class/GS, all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: hash-guard backup/stage candidate
  in private QA Client and update stage verifier; trace source outer effect
  scheduler vs native owner action transition before altering timing.
  NEXT FILE TO OPEN: verify_runtime_qa_stage.py,source157A60F caller chain.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed controller timing correction.

- Source CharacterAnimation caller checkpoint:
  REVERSE EVIDENCE: anchored14115BB prologue through1411A56. Positive
  Actions[CurrentAction].PlaySpeed reaches PlayAnimation unchanged for
  Shining189/playerA94; all action/model modifiers bypassed. Four source
  status gates61/72/178/280 can return before stepping; no enum mapping guessed.
  Native BMD multiplies speed by FPS_ANIMATION_FACTOR, computed in ZzzAI.cpp
  as min(REFERENCE_FPS/FPS,2.5); source examined path has no such multiplier.
  This per-call difference does not establish wall-clock cadence mismatch.
  CHAIN COVERAGE: caller speed preparation closed for this path. Outer
  caller1423CA8 consumes false return and dispatches1422EC8(event flag) or
  1423159(ordinary player); native analogous branch calls
  PlayerNpcStopAnimationSetting / PlayerStopAnimationSetting.
  ASSET PROVENANCE: actual staged10clips/all284legacy reverified unchanged.
  FILES MODIFIED: source animation verifier,SHINING_PEAK_REVERSE.md,state.
  LAST STATIC CHECK: expanded caller pins PASS; staged clip identities PASS;
  bounded final-window sampling RISK remains. LAST BUILD RESULT: unchanged
  PC4F0D59C2 / GS C85CEF94; no runtime source edit. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: native FPS factor; outer cadence not yet reconciled.
  MISSING CHAIN PARTS: transition/scheduler, final class/GS, visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace source1423159 ordinary
  player action end and native PlayerStopAnimationSetting for Shining189/288;
  establish whether controller survives wrap before any timing modification.
  NEXT FILE TO OPEN: source1423159 and native ZzzCharacter.cpp.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed Shining timing correction.

- Source animation-step and integration audit checkpoint:
  REVERSE EVIDENCE: pinned binary132D144..185 adds supplied speed directly;
  character caller1411A2F..56 passes OBJECT+D0 frame,D4 prior frame,2C prior
  action to132D0CD. Shining157A63C reads the same owner+D0. Wrap132D300..32C
  uses integer modulo plus fractional remainder, matching native arithmetic.
  CHAIN COVERAGE: frame-field identity and update arithmetic verified; caller
  speed scaling and effect/animation scheduling remain OPEN. No gate widening,
  key extension or runtime timing change made from this partial proof.
  ASSET PROVENANCE: unchanged; staged tree/hashes reverified.
  FILES MODIFIED: verify_source_animation_step.py and this state.
  LAST STATIC CHECK: source step PASS; project3cpp/24headers PASS;
  isolated stage2658base/79overlay/3root PASS.
  LAST BUILD RESULT: unchanged PC4F0D59C2 / GS C85CEF94; no rebuild needed
  for verifier/state-only change. LAST RUNTIME QA: none this checkpoint.
  KNOWN DIFFERENCES: Shining first-traversal final-window sampling risk remains.
  MISSING CHAIN PARTS: caller speed/scheduler, final class/GS, all9 visual QA.
  BLOCKER: none for offline reverse. Local integration is NOT Git merge;
  modified/untracked files remain, no commit/merge/deploy performed.
  NEXT EXACT ACTION: anchor enclosing caller before1411A56; trace local14
  speed assignments and scaling, then compare native PlayCharacterAnimation.
  NEXT FILE TO OPEN: pinned S21 binary character animation caller1411A56.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed Shining timing correction only.

- Staged clip/frame audit checkpoint: new verify_staged_action_clips.py
  reopens hash-pinned S21,legacy production(read-only),actual staged player.
  All284 legacy action hashes preserved; all10 source185..194 ->284..293
  clips equal. Keycounts7,6,12,12,7,7,10,11,11,11; all unlocked.
  Shining288 has7keys while final controller window is6.8..7.6. Under native
  unlocked PlayAnimation rule at speed.74599999 and factor1, first traversal
  last pre-wrap frame6.71399927; no sample enters final window. This is a
  bounded first-traversal sampling counterexample, NOT runtime proof of absent
  child forever (scheduler/transition/subsequent cycles unmodeled).
  REVERSE EVIDENCE: asset payload hashes and native frame-wrap source;
  source189/frame update and temporal call order must be traced next.
  CHAIN COVERAGE: actual clip merge verified; timing risk exposed, not fixed.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: staged clip verifier,state.
  LAST STATIC CHECK: clip identity PASS; timing audit reports RISK as above.
  LAST BUILD RESULT: Bin/staged4F0D59C2,GS C85CEF94 unchanged. LAST RUNTIME QA:
  none. KNOWN DIFFERENCES: potential Shining final emission skip/delay.
  MISSING CHAIN PARTS: timing reconciliation,final class/GS,all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace S21 BMD frame stepping and
  Shining controller owner-frame field/update order against native PlayAnimation;
  do not extend clip, force looping or widen emission thresholds without proof.
  NEXT FILE TO OPEN: source157A60F and native PlayAnimation callers.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed Shining timing correction.

- Local DK dynamic-speed QA integration checkpoint: five dynamic entrypoints
  (Harsh284,Spin285,MagicPin287,Shining288,Breche289) now prepare speed via
  native DWORD AttackSpeed ->float ->source GL initial upper limit273 ->
  proven S21 action formula. No repeated ability bonus. Helper requires QA
  compile flag, local Hero identity, MODEL_PLAYER, native base CLASS_KNIGHT,
  Attribute/Machine alias and valid Actions. Remote/non-QA paths reject rather
  than using local stats/uninitialized action speed. This is the authorized
  DK test profile, NOT final class/remote/Tornado/advanced-bonus parity.
  Four fixed-speed entrypoints remain registered independently.
  REVERSE EVIDENCE: prior source formula,cap/UI/native stage proofs; expanded
  registration verifier checks five pre-action guards and QA-only policy.
  CHAIN COVERAGE: all9 QA entrypoints have explicit speed registration now;
  does not prove effects, hits, GS or visual acceptance. ASSET PROVENANCE:
  unchanged. FILES MODIFIED: effect runtime,registration/stage verifiers,state.
  LAST STATIC CHECK: registration PASS, stage PASS2658base/79overlay/3root.
  LAST BUILD RESULT: PC Global Release Win32 isolated PASS, cryptlib LNK4099
  warnings; Bin and staged Client Engine SHA256:
  4F0D59C2153BFC4101269CEB4FB781964704E0C7D48BC4D276FD6F75F33A8EB2.
  Old staged C93DB845 preserved at RuntimeQA/Bin/Engine-Port S21.pre-speed-
  C93DB845.exe (filename continuous). Exact old/new hash guards and client
  root non-reparse check passed; no running Engine-Port process before copy.
  GS C85CEF94 unchanged. LAST RUNTIME QA: none; did not launch game.
  KNOWN DIFFERENCES: advanced bonuses/Tornado/final GL/remote remain OPEN.
  MISSING CHAIN PARTS: full class/GS and all9 visual/pool/regression QA.
  BLOCKER: none offline. NEXT EXACT ACTION: verify all nine action mappings
  and frame gates against staged clips, then continue remaining runtime chain
  gaps before owner visual acceptance; do not mark speed setup full parity.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime frame gates/package action map.
  NEXT FUNCTION TO IMPLEMENT: remaining evidence-backed runtime chain gap.

- Source speed-field UI association checkpoint: property manager70A1D6C
  lookup21 returns node+14 record; captured label atrecord+8 is UTF8
  "* Toc do danh" (Vietnamese accents present in exact-byte assertion).
  Display B56CFF chooses field1C2 only for profile classes0/5/8, otherwise
  field1B6, then publishes property21 viaB57047/B5704F. Profile class7 already
  pinned by constructor/snapshot. Thus GL displayed attack speed consumes
  1B6; this is no longer an inference from shared cached value273 alone.
  REVERSE EVIDENCE: expanded inspect_cast_speed_snapshot.py pins property
  lookup offsets,class branches,field reads,display property call and label.
  CHAIN COVERAGE: semantic association for GL's selected attack-speed input;
  not all class calculations or live cast timing. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: snapshot inspector,state. LAST STATIC CHECK: inspector PASS.
  LAST BUILD RESULT: Bin FDA5924A,staged C93DB845,GS C85CEF94 unchanged.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: native adds ability bonus during
  CalculateAttackSpeed; S21 adds in animation setup. Do not replay addition.
  MISSING CHAIN PARTS: dynamic adapter,class,GS,all9 visual/pool acceptance.
  BLOCKER: none offline. NEXT EXACT ACTION: implement private native dynamic
  speed adapter using calculated attack speed with explicit adjustment stage,
  source class7 cap contract and no duplicate ability bonus; keep local/remote
  ownership policy evidence-gated. NEXT FILE TO OPEN: GrowLancerEffectRuntime
  dynamic action callers and CHARACTER_ATTRIBUTE definitions.
  NEXT FUNCTION TO IMPLEMENT: private dynamic adjusted-speed input adapter.

- Native receive-order and cast telemetry checkpoint: ReceiveJoinMapServer
  assigns received speeds before SetCharacterClass(c), whose tail calls
  CalculateAll ->CalculateAttackSpeed. ReceiveHelperItem cases0/2 also set
  speed flags then recalculate. Login packet value therefore not a stable
  animation input cache. Added QA-only before/after-dispatch observations:
  skill,class,AttackSpeed,MagicSpeed,Ability,current action/frame and whether
  Attribute aliases Machine.Character. No pointer values/account data logged;
  no stat mutation/recalculation and no uninitialized PlaySpeed reads.
  REVERSE EVIDENCE: expanded native speed-boundary verifier checks call order
  and observational QA contract. CHAIN COVERAGE: native update order and
  instrumentation ready, not observed runtime values. ASSET PROVENANCE:
  unchanged. FILES MODIFIED: RuntimeQA.cpp,boundary verifier,state.
  LAST STATIC CHECK: native boundary verifier PASS. LAST BUILD RESULT:
  isolated PC Global Release Win32 PASS exit0; cryptlib missing-PDB warnings.
  New RuntimeQA/Bin Engine SHA256:
  FDA5924A483B096DEB0374675350F99205406CEDE9BFDF852EA2FE230DA309D1.
  QA staged Engine still C93DB845; GS C85CEF94 unchanged. LAST RUNTIME QA:
  none; new executable not staged or launched. KNOWN DIFFERENCES: source
  input-stage semantic mapping still needed, no inferred universal cap/subtract.
  MISSING CHAIN PARTS: dynamic animation,class,GS,nine-skill visual/pool QA.
  BLOCKER: none offline. NEXT EXACT ACTION: resolve source physical-speed
  field binding, then use validated native input stage for dynamic actions.
  NEXT FILE TO OPEN: source property21 record and native private adapter.
  NEXT FUNCTION TO IMPLEMENT: private dynamic adjusted-speed input adapter.

- Native dynamic-speed boundary audit: CalculateAttackSpeed already adds20
  once for Ability flags01/08 (if/else-if), plus item/buff/socket speed values.
  Native SetAttackSpeed consumes CharacterAttribute->AttackSpeed directly.
  Therefore replaying S21 flag addition on native calculated speed double
  counts; do not blindly call S21AdjustCastSpeedTail with native Ability.
  REVERSE EVIDENCE: verify_native_speed_boundary.py checks native named masks,
  branch arithmetic, socket and buff contributions plus native consumer.
  Existing BmdRISE CRC accepts legacy7D56FCEB or privateE51E1780; fixed-action
  helper checks player type/action bounds/Actions pointer. No package bypass
  added. CHAIN COVERAGE: native input stage distinction, not S21 parity.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: new boundary verifier,state.
  LAST STATIC CHECK: native boundary and source registration verifiers PASS.
  LAST BUILD RESULT: last new isolated Bin3DCE4B95 PASS; staged C93DB845,
  GS C85CEF94 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: native receive writes may precede/replace recalculation;
  cannot universally subtract20 either without call-order proof.
  MISSING CHAIN PARTS: dynamic adapter,class,GS,all9 runtime acceptance.
  BLOCKER: none offline. NEXT EXACT ACTION: trace native receive ->stat
  recalculation ->cast order and source attribute field producers to select
  correct speed stage, then integrate dynamic private actions.
  NEXT FILE TO OPEN: WSclient receive and CHARACTER_MACHINE calculation calls.
  NEXT FUNCTION TO IMPLEMENT: private dynamic adjusted-speed input adapter.

- Native fixed-speed activation checkpoint: added PrepareFixedPlayerAction
  in private GrowLancerEffectRuntime. Requires MODEL_PLAYER, private mapped
  action284..293, valid action count and nonnull Actions; rejects dynamic
  actions via S21FixedCastAnimationSpeed rather than substituting zero stats.
  Circle Shield286/source187 now .5; Obsidian290/source191, Wrath291/source192
  and Clash293/source194 now .35 before action assignment. Legacy SS6 action
  speeds untouched. REVERSE EVIDENCE: prior pinned constant writes; expanded
  verifier checks four entrypoint ordering and private model/action guards.
  CHAIN COVERAGE: four fixed-speed cast registrations implemented; variable
  speed actions still OPEN. ASSET PROVENANCE: unchanged. FILES MODIFIED:
  effect runtime,compat speed header,focused test,registration verifier,state.
  LAST STATIC CHECK:34 speed static_asserts MSVC x86 /W4 /WX PASS; source
  registration and project inclusion verifiers PASS. LAST BUILD RESULT:
  isolated PC RuntimeQA Global Release Win32 PASS, exit0, cryptlib LNK4099
  missing-PDB warnings only. New Bin Engine SHA256:
  3DCE4B95651F418A47F202DBAA8C7FA4022A5E9F1591D99F7D6C697DA3C9F527.
  Staged QA Client intentionally still C93DB845; new binary NOT yet staged
  or run. GS unchanged C85CEF94. LAST RUNTIME QA: none for this fix.
  KNOWN DIFFERENCES: only four fixed-speed entrypoints wired, not all9 parity.
  MISSING CHAIN PARTS: dynamic speed registration,class,GS,visual/pool QA.
  BLOCKER: none offline. NEXT EXACT ACTION: finish dynamic native input
  mapping then build/stage hash-guarded candidate; preserve old staged binary.
  NEXT FILE TO OPEN: native CharacterAttribute and source stat-field UI binding.
  NEXT FUNCTION TO IMPLEMENT: private dynamic adjusted-speed input adapter.

- Base-skill optional bonus snapshot checkpoint: getter961588 reads UI1F8;
  C4FF95 finds skillID in manager+A0 tree and returns null on missing key.
  Pinned full dump has no records271..279 in this map; UI298 optional slot
  manager has all three item pointers null. With prior caller branches this
  proves both optional additions skipped for those base skills in this capture,
  NOT for all possible characters. REVERSE EVIDENCE: expanded bonus verifier
  pins manager getters, lookup/null path, tree root/key/nil/child fields and
  reopens snapshot to check all9 keys and3 slot pointers.
  CHAIN COVERAGE: captured input path excludes unearned bonuses; no live cast.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: bonus verifier,state.
  LAST STATIC CHECK: expanded bonus verifier PASS. LAST BUILD RESULT:
  PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: advanced skill paths remain conditional; no global zero.
  MISSING CHAIN PARTS: native stat naming/registration,class,GS,all9 QA.
  BLOCKER: none offline. NEXT EXACT ACTION: finish native input contract using
  source field semantics and proven adjustments, then private action setup.
  NEXT FILE TO OPEN: native CharacterAttribute and GrowLancerRuntimeQA.cpp.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Optional speed bonus chain checkpoint: full C5B813..C5B8B0 loops three
  slots (stride1C), skips null item pointer18, reads item key10; resolves
  class/item table1118C35 using group*512+index, matches requested bonus type
  row18 then adds row1C. Caller requests type1. Pinned class7 snapshot table
  has two36-byte rows: item20/193 skill2065 ->1235,type2/value1;
  item20/234 skill2089 ->1236,type1/value15. Hash-pinned GS ItemList names
  them Oversting of Saturation and Wild Breath of Gale. Base271..279 are
  not directly remapped by these two rows. Do NOT apply+15 universally.
  REVERSE EVIDENCE: verify_cast_speed_bonus.py pins complete loop and lookup
  consumers, reopens full dump and cross-checks source ItemList. Separate
  B60918 property-map3F0 getter has zero path for missing key via10FC071.
  CHAIN COVERAGE: bonus selection/arithmetic; native slot acquisition OPEN.
  ASSET PROVENANCE: unchanged, read-only inputs hash-pinned. FILES MODIFIED:
  new bonus verifier,state. LAST STATIC CHECK: bonus verifier PASS.
  LAST BUILD RESULT: PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA:
  none. KNOWN DIFFERENCES: advanced skill bonus not default base skill value.
  MISSING CHAIN PARTS: native acquisition/registration,class,GS,all9 QA.
  BLOCKER: none offline. NEXT EXACT ACTION: inspect base271..279 skill
  property3F0 records and optional-slot state in snapshot, then define native
  input adapter with explicit validated bonus sources (no double counting).
  NEXT FILE TO OPEN: C4FF95 skill lookup,961588 manager getter.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Class7 speed-limit constructor closed: anchored complete9A23EC..9A245F
  calls base996B70, writes vtable1B4E9A0 and classfield1D4=7, calls997699(7)
  then writes result273 into1D8. Full-dump profile vtable/class match exactly.
  REVERSE EVIDENCE: expanded inspect_cast_speed_snapshot.py pins constructor,
  class-table7 branch and snapshot receiver identity. CHAIN COVERAGE: default
  limit producer ->profile getter ->float clamp established, later indirect
  mutations remain unaudited. ASSET PROVENANCE: unchanged. FILES MODIFIED:
  inspector,compat speed header,focused test,state. Added scoped constexpr
  S21GrowLancerInitialSpeedLimit=273, explicitly not default input or DK rule.
  LAST STATIC CHECK: snapshot inspector PASS; focused26 speed assertions
  MSVC x86 /W4 /WX compile PASS. LAST BUILD RESULT: full PC C93DB845,GS C85CEF94
  unchanged. LAST RUNTIME QA: none. KNOWN DIFFERENCES: native activation
  still not wired; do not treat constructor constant as global cap policy.
  MISSING CHAIN PARTS: stat naming/bonus acquisition/private activation,
  class,authoritativeGS,nine-skill runtime acceptance. BLOCKER: none offline.
  NEXT EXACT ACTION: inspect property21 registry11425C6 and source bonus
  managerC5B813/C5B7F5 to finish stat mapping, then wire private setup.
  NEXT FILE TO OPEN:11425C6,C5B813.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Offline text/speed snapshot checkpoint: followed9609DC ->11D8FA3 map+38,
  node key+10/string+1C and8E7C00 small-string selection. Pinned snapshot
  text64 decodes to Vietnamese attack-speed label;4286 is ONLY "%d / %d",
  not a descriptive label. Thus4286 alone cannot name fields1B6/1C2.
  Snapshot attributes1B6=273 and1C2=273, profile upper-limit operand1D8=273
  via global1E04FB8 ->machine2D98 ->profile. This offers a real-stat-path
  explanation for cached action speeds; does NOT prove last caller or justify
  hardcoding273. REVERSE EVIDENCE: inspect_cast_speed_snapshot.py pins dump,
  map/string/operand getter instructions and independently reopens full dump.
  CHAIN COVERAGE: input-state evidence, not cast-time tracking or native setup.
  ASSET PROVENANCE: read-only snapshots; no asset changes. FILES MODIFIED:
  new snapshot inspector,state. LAST STATIC CHECK: inspector PASS with both
  binary and full-dump SHA checks. LAST BUILD RESULT: PC C93DB845,GS C85CEF94
  unchanged. LAST RUNTIME QA: none. KNOWN DIFFERENCES: profile limit operand
  semantic source/writer remains unknown, do not present as universal cap.
  MISSING CHAIN PARTS: input adapter/action registration,class,GS,all9 QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace profile1D8 writer and
  B54C76 display property21 label binding before naming source stat fields;
  then map validated inputs into private native animation registration.
  NEXT FILE TO OPEN: B54C76 and profile1D8 write xrefs.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Speed receive provenance checkpoint: source opcode82 table selects1303939;
  subcode04 branch1303A13 ->1303D1B ->1296442. Entire small handler decoded:
  reads words packet+4/+8 into global attributes1B6/1C2, then B56CFF display
  refresh; no direct1408580/1408811 call. Display consumes same fields and
  formats GlobalText4286. Source C2 subcode read exists at+4, but handler fixed
  offsets are not evidence of valid C2 payload support. Native named receive
  SpeedAttack/MagicAttack fields are DWORD, so transport/truncation equivalence
  is not assumed. REVERSE EVIDENCE: new verify_cast_speed_receive.py pins
  dispatch table, branches, full handler and display, plus native field widths.
  CHAIN COVERAGE: source input producer identified beyond cached snapshot.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: new receive verifier,state.
  LAST STATIC CHECK: new verifier PASS. LAST BUILD RESULT: unchanged PC
  C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES: source
  display uses WORD arithmetic while animation uses float; do not reuse the
  UI calculation as animation input. MISSING CHAIN PARTS: semantic naming,
  manager modifiers/cap, native action registration,class,GS,all9 runtime QA.
  BLOCKER: none offline. NEXT EXACT ACTION: resolve text4286 from loaded text
  resources and trace source manager bonus3F0/cap record, then connect private
  native speed input without copying packet layout or lowering value width.
  NEXT FILE TO OPEN: GlobalText lookup9609DC and snapshot/text resources.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Overnight continuation and integration audit checkpoint (2026-09-13):
  Owner requests continued completion and checking everything is merged.
  Local project verifier PASS:3cpp/24headers included once, no build exclusion.
  QA stage verifier PASS:2658 base RISE files,79 GL overlay,3 root Data files,
  pinned Engine/player hashes and player CRC. This is NOT Git merge proof:
  branch feature/grow-lancer-skills-s21 remains at f2e8e558 with modified and
  untracked implementation files. No commit, merge or production deployment.
  Heartbeat ti-p-t-c-engine-port-s21-grow-lancer ACTIVE every30minutes in this
  task; continue exact next work, report meaningful events only, stop when done.
  REVERSE EVIDENCE: hash-pinned BuffEffectManager.xml names176 Tornado,
  EffectType100, description attack speed reduced to half; source1408580
  anchored tail independently pins flags1/8 +20 once, optional object B0
  membership halves BOTH source speeds, then optional upper cap. Added pure
  S21AdjustCastSpeedTail, preserving operation order and absent-vs-zero cap.
  CHAIN COVERAGE: input-adjustment arithmetic only, no native input mapping.
  ASSET PROVENANCE: unchanged, verified stage. FILES MODIFIED: compat speed
  header,cast-speed test,two reverse verifiers,state. LAST STATIC CHECK:
  buff names/cast pins/project/stage/server guard PASS;23 static_asserts
  compiled MSVC x86 /W4 /WX. LAST BUILD RESULT: full PC C93DB845 and GS
  C85CEF94 unchanged; no runtime implementation changed in this checkpoint.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: appended PlaySpeed registration
  still missing; source field/manager meanings and native translation OPEN.
  MISSING CHAIN PARTS: runtime registration, new class, authoritative GS,
  nine-skill visual/pool/regression acceptance. BLOCKER: none for offline work.
  NEXT EXACT ACTION: trace source attribute1B6/1C2 writers and manager skill
  bonus3F0/cap acquisition, then native stat adapter and private action setup.
  NEXT FILE TO OPEN: pinned source field writers/native CharacterAttribute.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Caller audit rules out assuming fallback273 is generic remote GL policy:
  direct rel32 scan finds14087C8 called at12FB290 immediately before action
  E0=224 via1327DE8. Actual player setup18CE254 passesnull into1408580;
  140DD81 passes character Object3254 into real-stat path. Thus shared player
  action table can contain fallback-derived values without proving GL uses
  a per-class remote-speed rule. Do not hard-code273 into native registration.
  REVERSE EVIDENCE: direct-call scan and anchored caller/action/setup pins.
  CHAIN COVERAGE: observed invocation context, not all indirect callers.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: cast verifier,state.
  LAST STATIC CHECK: expanded cast verifier PASS. LAST BUILD RESULT:
  PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: source real-stat path objectarg is for buff check;
  current fallback capture may be last-writer state, not captured GL cast.
  MISSING CHAIN PARTS: statfield identities/modifier/cap mapping,private setup,
  class,GS,all9 QA. BLOCKER: none offline. NEXT EXACT ACTION: identify global
  attributes1E04FBC fields1B6/1C2 and compare native Attribute speed fields;
  trace statusB0 identity and skill-speed bonus path before native hookup.
  NEXT FILE TO OPEN: attribute field writers and status/resource tables.
  NEXT FUNCTION TO IMPLEMENT: private native adjusted-speed input adapter.

- Implemented pure ten-action speed formula S21CastAnimationSpeed using
  explicit already-adjusted input, no runtime stat assumption.13 compile-time
  checks include all ten snapshot matches at273 and unsupported IDs.
  Class fallback997699 table7 ->9976E4 returns273. Real-stat caller1408580
  reads unsigned words1B6/1C2, optional manager bonuses, flags1/8 +20 once,
  statusB0 half then optional manager cap1D8 before1408811. Names and native
  equivalents of modifiers remain OPEN; do not use fallback273 universally.
  REVERSE EVIDENCE: source function entry, table branch, constant20 and prior
  selected formula pins. CHAIN COVERAGE: formula implementation, not native
  stat acquisition or active registration. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: compat speed header,focused test,verifier,state.
  LAST STATIC CHECK:13 assertions MSVC x86 /W4 /WX PASS; source verifier PASS.
  LAST BUILD RESULT: full PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA:
  none. KNOWN DIFFERENCES: appended actions still need runtime speed setup.
  MISSING CHAIN PARTS: caller stat/modifier/cap mapping,local/remote call policy,
  class,GS,all9 visual QA. BLOCKER: none offline. NEXT EXACT ACTION: inspect
  1408580 and14087C8 callers to establish local-vs-remote update policy and
  source field1B6/1C2 semantic identity before integrating native registration.
  NEXT FILE TO OPEN: rel32 xrefs1408580/14087C8 and native animation update.
  NEXT FUNCTION TO IMPLEMENT: private native speed registration with proven input.

- Anchored speed function1408811. Local24 has exactly one direct write in
  entry-through-194 region: input8 * float0.002 at1408862/67/6F.194 final
  write140A1D8/E0 uses0.35, completing selected ten-action formulas.
  Caller14087BF passes adjusted local8/local4, with preceding additions,
  statusB0 half-speed path and dynamic cap; alternate1408808 passes a
  class-derived value997699 to both inputs. Raw native AttackSpeed is NOT
  yet proven equivalent. REVERSE EVIDENCE: producer/constants/194-write pins.
  CHAIN COVERAGE: adjusted-input-to-ten-action-speed formula, not stat pipeline.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: cast-speed verifier,state.
  LAST STATIC CHECK: expanded verifier PASS. LAST BUILD RESULT: unchanged
  PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES:
  input modifiers/cap must map natively, not copy offsets or snapshot values.
  MISSING CHAIN PARTS: speed-input contract/init, mounted lifecycle,class,GS,QA.
  BLOCKER: none offline. NEXT EXACT ACTION: anchor caller start before14086A0,
  trace local8 stat source and statusB0/cap meanings; inspect997699 fallback.
  NEXT FILE TO OPEN: caller14087BF predecessor and997699.
  NEXT FUNCTION TO IMPLEMENT: private adjusted-speed computation/registration.

- Found actual S21 cast-speed registration1409FD8 onwards: action185=.13+T,
  186/188=.16+T,189=.2+T,190=.3+T,187=.5,191..193=.35;194 push found,
  trailing write needs pin. T is local24, producer not yet traced.
  D6DB7E selects player modelA94 then action stride16/table30; writes field4.
  Snapshot185..194 speeds .676/.706/.5/.706/.746/.846/.35/.35/.35/.35
  agree with observed common term .546, but do not infer its stat formula.
  REVERSE EVIDENCE: source registration/constants pins + snapshot records.
  CHAIN COVERAGE: selected speed writes; no guessed runtime constants applied.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: snapshot inspector,cast-speed
  verifier,state. LAST STATIC CHECK: selected speed verifier PASS.
  LAST BUILD RESULT: PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: native appended speed setup still absent in inspected
  paths. MISSING CHAIN PARTS: complete speed formula/init, mounted setup,
  GS,class,all9 QA. BLOCKER: none offline. NEXT EXACT ACTION: locate full
  source speed function entry and local24 writes; map stat/speed conversion
  before adding native284..293 registration to existing SetAttackSpeed path.
  NEXT FILE TO OPEN: S21 preceding1409FD8/local24 producer,194 trailing write.
  NEXT FUNCTION TO IMPLEMENT: proven private GrowLancer animation speed update.

- Found priority animation initialization gap while auditing candidate load:
  native AccessModel calls BMD::Open2; Open2 allocates plain Action_t[] and
  fills Loop/keys/LockPositions/Positions, not PlaySpeed. OpenPlayers assigns
  named legacy speeds; private GrowLancer runtime only READS PlaySpeed at
  owner blur sampling and Spin velocity. No writer found in private package,
  WSclient or Winmain. Existing appended284..293 therefore require a complete
  writer audit before visual testing; do not assume BMD stores playback speed.
  REVERSE EVIDENCE: LoadData.cpp AccessModel; ZzzBMD.cpp2949/3002; Action_t
  has no constructor; native OpenPlayers and SetAttackSpeed inspected partially.
  CHAIN COVERAGE: loader initialization gap, not exhaustive all-writer proof.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: state only this checkpoint.
  LAST STATIC CHECK: source audit; no new runtime test. LAST BUILD RESULT:
  full PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: potential uninitialized appended action speeds is now
  higher priority than enabling mounted candidate. CRC path currently accepts
  only legacy7D56FCEB and mergedE51E1780; candidate remains rightly rejected.
  MISSING CHAIN PARTS: proven cast speed contract/registration, mounted setup,
  GS,class,all9 QA. BLOCKER: none offline. NEXT EXACT ACTION: inspect complete
  SetAttackSpeed and all native PlaySpeed writers; extract source185..194
  speed initialization/formula before implementing private registration.
  NEXT FILE TO OPEN: ZzzCharacter.cpp SetAttackSpeed and S21 action speed setup.
  NEXT FUNCTION TO IMPLEMENT: native appended animation speed initialization.

- Added independent candidate reopen verifier: checks all three asset hashes,
  raw294 action records/bone keys unchanged and appended four source records/
  keys equal; manifest mappings/clip hashes checked. Added typed candidate
  mapping335..338 ->294..297 with whole-package count>=298 guard; unsupported
  or invalid source enum returns-1. Count does NOT authenticate package; caller
  must validate pinned asset identity before runtime use. No activation added.
  REVERSE EVIDENCE: original binary selector pins and candidate raw records.
  CHAIN COVERAGE: candidate mapping and negative capacity tests.
  ASSET PROVENANCE: candidate24EC40D7 unchanged. FILES MODIFIED: candidate
  verifier,compat selector header,focused test,state. LAST STATIC CHECK:
  raw verifier PASS;11 new mapping assertions plus prior selector cases
  MSVC x86 /W4 /WX PASS. LAST BUILD RESULT: full PC C93DB845,GS C85CEF94
  unchanged. LAST RUNTIME QA: none. KNOWN DIFFERENCES: runtime package
  authentication/speed registration/staging remain absent; not native PASS.
  MISSING CHAIN PARTS: activation,teardown,class,GS,all9 gameplay gates.
  BLOCKER: none offline. NEXT EXACT ACTION: inspect full OpenPlayers appended
  action-speed initialization and shared CRC checks for candidate-aware loading
  without removing existing Player compatibility or touching production.
  NEXT FILE TO OPEN: ZzzOpenData OpenPlayers tail and RISE/ZzzToolKit CRC path.
  NEXT FUNCTION TO IMPLEMENT: candidate-authenticated private action setup.

- Generated separate mounted Player candidate with append335..338 ->294..297.
  All294 existing action hashes preserved; new clip hashes equal S21 originals,
  canonical60-bone skeleton matches, v0C decrypt/reparse PASS, total298 actions.
  Candidate SHA24EC40D7A367ED393D4B0832C3F55BB2560805531817FEFCD893166B76E434D4.
  Original snapshot shows all four cached PlaySpeed0.4, six keys each.
  REVERSE EVIDENCE: pinned snapshot plus merge tool full per-action comparison.
  CHAIN COVERAGE: candidate clip conversion, NOT active native animation.
  ASSET PROVENANCE: mounted_reaction_candidate.json records sources/hashes,
  tool/actions and candidate IDs. RuntimeQA Player remains unchanged.
  FILES MODIFIED: snapshot inspector,candidate BMD,manifest,state.
  LAST STATIC CHECK: merge/reparse/action preservation PASS.
  LAST BUILD RESULT: full PC C93DB845,GS C85CEF94 unchanged. LAST RUNTIME QA:
  none. KNOWN DIFFERENCES: candidate not authorized/staged/activated;
  snapshot speed is not all-write proof. MISSING CHAIN PARTS: native mapping,
  capacity/CRC, speed registration,mount lifecycle,class,GS,all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: add independent manifest verifier
  and inspect native action count/speed registration before coherent staging.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp animation setup and capacity.
  NEXT FUNCTION TO IMPLEMENT: typed source-to-native mounted clip mapping.

- Implemented pure private SelectS21MountedReaction in compat header with
  typed SOURCE clip identity, stored state input and hand/category inputs.
  Preserves unarmed/state gate, book-before-orb-before-elixir precedence,
  then both/hand0/hand1 selection. No native ID cast, global writes or runtime
  activation. Empty-hand stale category is ignored under validated-catalog
  input contract. Final native mapping/staging and outer cast gates separate.
  REVERSE EVIDENCE: existing SHA-pinned auxiliary verifier rerun PASS.
  CHAIN COVERAGE: executable selection contract, not mounted gameplay.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: S21MountedReaction.h,
  GrowLancerMountedReactionTest.cpp,state. LAST STATIC CHECK:14 explicit
  static assertions plus100 eligible-category cases PASS; MSVC14.51 x86
  /std:c++17 /W4 /WX /c PASS, isolated test object only.
  LAST BUILD RESULT: full PC/GS unchanged C93DB845/C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: typed source clips335..341 not
  staged/mapped; compat header intentionally not runtime-connected.
  MISSING CHAIN PARTS: native clip mapping, mount teardown, GS,class,all9 QA.
  BLOCKER: none offline. NEXT EXACT ACTION: verify mounted clip skeleton/
  speed contract and stage only required source335..338 to isolated package
  once capacity/CRC/descriptor integration changes are planned together.
  NEXT FILE TO OPEN: merge_player_actions.py and GrowLancer action descriptor.
  NEXT FUNCTION TO IMPLEMENT: private source-to-native mounted action mapping.

- Extended state-writer audit:9F71B9 reads owner byte4 then9F71CB copies into
  embedded character;12F1352 explicitly sets true.12522E6..2323 and1253AC1
  use context field3AC4>130 plus nonempty item checks, not terrain lookup.
  This disproves replacing the stored gate with a terrain-only calculation.
  REVERSE EVIDENCE: SHA-pinned getter/setter calls and conditional blocks.
  CHAIN COVERAGE: multiple state producers, not complete context semantics.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: auxiliary verifier,class
  contract,state. LAST STATIC CHECK: expanded verifier PASS.
  LAST BUILD RESULT: unchanged PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: preserve stored native state; do not mutate global
  SafeZone from private skill code. MISSING CHAIN PARTS: remaining contexts,
  teardown, mounted clips,GS,class,all9 QA. BLOCKER: none offline.
  NEXT EXACT ACTION: add pure private mounted reaction selection contract
  using explicit state input and equipment categories; keep activation gated
  until source clip staging and native class integration are proven.
  NEXT FILE TO OPEN: GrowLancer/compat and existing focused test conventions.
  NEXT FUNCTION TO IMPLEMENT: private reaction selection with no global writes.

- Traced state-byte4 reaction gate to terrain update13F7126..7169:
  wrapped cell index176BA3C=(y&255)*256+(x&255), word terrain arrayB7AAF60
  bit1 selects setter131510E(true/false), which writes character4 read by
  961566. Native ZzzCharacter5450/11532 uses TerrainWall bitTW_SAFEZONE=1.
  Thus selected source path supports native SafeZone interpretation, not
  a generic equipped-mount flag. Other setter callers not all audited.
  REVERSE EVIDENCE: index, terrain-bit, setter/getter and consumer pins.
  CHAIN COVERAGE: terrain-state-to-reaction gate. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: auxiliary verifier,state. LAST STATIC CHECK: verifier PASS.
  LAST BUILD RESULT: unchanged PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: for eligible GL categories safe-state can select335 even
  with hands occupied; native Fenrir selector alone only tests equipment.
  MISSING CHAIN PARTS: all state writers, teardown, mounted QA,GS,class,all9.
  BLOCKER: none offline. NEXT EXACT ACTION: finish remaining byte4 writers
  before codifying SafeZone equivalence and private mounted selection tests.
  NEXT FILE TO OPEN: setters at125230C/1253AE4/12F1357 and9F36BA/9F71CB.
  NEXT FUNCTION TO IMPLEMENT: private mounted selector with proven state gate.

- Cross-checked53 supplied GS GrowLancer-positive weapon/shield rows against
  original client catalog. Exact KindB match: group/category0/0=1,0/1=3,
  1/4=2,3/7=23,6/15=24; none89/90/114. Do not hard-code only spears/shields:
  supplied eligibility also includes basic swords/axes and Pandora Pick.
  REVERSE EVIDENCE: both source SHA/checksum pins and exact category counts.
  CHAIN COVERAGE: eligible-item category exclusion for special reactions,
  not equipped hand assignment or cast legality. ASSET PROVENANCE: read-only.
  FILES MODIFIED: weapon category inspector,class contract,state.
  LAST STATIC CHECK: cross-table inspector PASS. LAST BUILD RESULT: unchanged
  PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES:
  full class equipment permissions still unimplemented; no guessed hand map.
  MISSING CHAIN PARTS: state predicate, actual hand selection, mounted clips,
  GS,class and all9 visual QA. BLOCKER: none offline. NEXT EXACT ACTION:
  verify state byte4 gate used by10C3B1A and source equipped hand decoding;
  then select private mounted clips without changing existing SS6 actions.
  NEXT FILE TO OPEN: 961566/131510E callers and character equipment decoder.
  NEXT FUNCTION TO IMPLEMENT: private mounted action selector after gates.

- Closed mounted reaction owner provenance: manager10C2FD4 calls factory
  10C464B, virtual4 reset108A124, then1089CD3(character arg10, descriptor).
  Initializer stores character in record14, character+3254 in record18,
  creates child object in record1C and points child's Owner34C at record18.
  On success manager inserts by character key3E; failure calls virtual0.
  Reaction wrappers previously pinned read same record14 character.
  REVERSE EVIDENCE: complete manager/init/reset blocks with instruction pins.
  CHAIN COVERAGE: init-to-reaction owner chain, not complete teardown audit.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: auxiliary verifier,state.
  LAST STATIC CHECK: expanded verifier PASS. LAST BUILD RESULT: unchanged
  PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES:
  source owns separate auxiliary child; no S21 offsets copied into native.
  MISSING CHAIN PARTS: concrete GrowLancer gear/action selection, teardown,
  GS,class and all9 QA. BLOCKER: none offline. NEXT EXACT ACTION: validate
  GrowLancer spear/shield item categories then record exact mounted reaction
  source clip mapping and native compatibility requirements.
  NEXT FILE TO OPEN: supplied ItemList.xml GrowLancer weapons and item.bmd.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed private mounted action selector.

- Resolved equipment category helper13150A8 ->131501E: accepts hand0/1,
  reads weapon model, subtractsAE9, looks up catalog and returns byte251.
  SHA/checksummed catalog categories89/90/114 contain19/20/19 records, all
  group5 with book/orb/elixir examples. Their reaction339/340/341 branches
  must not be treated as a default GrowLancer spear/shield mapping.
  Factory common tail10C500D only returns record, does not initialize owner.
  REVERSE EVIDENCE: complete category helper pins and exact catalog counts.
  CHAIN COVERAGE: equipment category semantics, not final owner assignment.
  ASSET PROVENANCE: no changes. FILES MODIFIED: auxiliary verifier,state.
  LAST STATIC CHECK: expanded auxiliary verifier PASS. LAST BUILD RESULT:
  unchanged PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: source-specific group5 branches isolated in evidence;
  hand-presence default still needs GrowLancer item validation.
  MISSING CHAIN PARTS: owner init, class activation, GS,all9 QA. BLOCKER: none
  offline. NEXT EXACT ACTION: inspect factory caller for owner14 assignment,
  then validate selected GrowLancer spear/shield catalog rows and clip mapping.
  NEXT FILE TO OPEN: factory function start/callers and auxiliary init virtual.
  NEXT FUNCTION TO IMPLEMENT: proven private mounted reaction adapter.

- Resolved Fenrir concrete virtual reaction path: factory item1A25 branch
  10C48C6/CD ->10C49F9 allocates50 and calls constructor10C59E2; constructor
  installs vtable1B876E4. Slots2C/30 are108A781/108A79C, which pass record14
  and action argument into108AC04/108AC77. Both return4. Previously recovered
  action335..341 selectors are therefore reachable via Fenrir virtual path,
  not merely null-record fallback. Owner-record initialization still OPEN.
  REVERSE EVIDENCE: SHA-pinned factory,ctor,vtable and complete wrapper pins.
  CHAIN COVERAGE: Fenrir virtual dispatch closure, not equipment activation.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: auxiliary verifier,state.
  LAST STATIC CHECK: expanded auxiliary verifier PASS. LAST BUILD RESULT:
  unchanged PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: S21 mounted reaction clips absent byte-identically from
  legacy284; no unproven action appended. MISSING CHAIN PARTS: record owner
  initialization, GrowLancer equipment selectors, GS,class,all9 visual QA.
  BLOCKER: none offline. NEXT EXACT ACTION: prove factory input/record14
  owner assignment and categorize GrowLancer spear/shield through13150A8.
  NEXT FILE TO OPEN: factory common tail10C500D and13150A8.
  NEXT FUNCTION TO IMPLEMENT: private mounted reaction mapping after selection.

- Traced action6 into complete10C3B1A: record absent selects108AC04/108AC77,
  record present uses virtual2C/30. Unarmed fallback6 selects335; equipped
  fallback has category-specific339/340/341 then hand-presence338/336/337.
  Clip comparison found NO byte-identical legacy action among284 SS6 clips
  for S21 335..341 (keys6/6/6/6/7/6/6). Do not claim native Fenrir parity.
  REVERSE EVIDENCE: complete helper/selected fallback pins and original BMD
  cross-ID hashes. CHAIN COVERAGE: fallback selection, not concrete Fenrir
  registered-vtable behavior. ASSET PROVENANCE: read-only, no clip appended.
  FILES MODIFIED: auxiliary verifier,clip inspector,state. LAST STATIC CHECK:
  auxiliary verifier PASS; comparison completed. LAST BUILD RESULT: unchanged
  PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES:
  existing native mounted clips not byte-identical; virtual route unresolved.
  MISSING CHAIN PARTS: concrete vtable/class mount mapping, GS,class,all9 QA.
  BLOCKER: none offline. NEXT EXACT ACTION: identify Fenrir record constructor
  and virtual2C/30 destinations before choosing new clip mappings.
  NEXT FILE TO OPEN: 10C30B1 manager lookup and Fenrir registration constructor.
  NEXT FUNCTION TO IMPLEMENT: only proven private auxiliary reaction adapter.

- Identified source scream exclusion1A25 in checksum/SHA-pinned item table:
  explicit group13/index37, Horn of Fenrir, model fr_6.bmd. It is not a
  GrowLancer buff. Auxiliary reaction branch also compares1A25 then selects
  sound700/701 under manager/predicate and random modulo3 gates.
  REVERSE EVIDENCE: catalog row,140AAA6..140AB45 pins; packet caller129A39A
  decodes bytes30/31 into setter1314F4C (field source lead, not full packet).
  CHAIN COVERAGE: Fenrir identity, not full mount activation equivalence.
  ASSET PROVENANCE: original catalog read-only. FILES MODIFIED: sound verifier,
  class contract,state. LAST STATIC CHECK: expanded sound verifier PASS.
  LAST BUILD RESULT: unchanged PC C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: native ordinary scream is not excluded on Fenrir;
  source full auxiliary predicate must be established before private fix.
  MISSING CHAIN PARTS: mount selection, GS, class and all9 QA. BLOCKER: none
  offline. NEXT EXACT ACTION: map auxiliary action6 through10C3B1A for Fenrir
  and compare native SetAction_Fenrir_Damage, keeping generic Muun separate.
  NEXT FILE TO OPEN: S21 10C3B1A, native SetAction_Fenrir_Damage.
  NEXT FUNCTION TO IMPLEMENT: proven private mount reaction adapter only.

- Pinned female reaction sound registration109/110 ->pFemaleScream1/2.wav;
  original S21 and isolated QA assets have identical SHA256 RIFF/WAVE bytes.
  Native already registers both filenames/IDs. No duplicate staging needed.
  REVERSE EVIDENCE: 18E015B..18E01D2 filename/ID/register-call pins and hashes
  in class contract/verifier. CHAIN COVERAGE: sound identity/assets, not live
  playback configuration. ASSET PROVENANCE: read-only existing files.
  FILES MODIFIED: reaction verifier,class contract,state. LAST STATIC CHECK:
  expanded sound verifier PASS. LAST BUILD RESULT: unchanged PC C93DB845,
  GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES: class7 not enabled;
  suppression field322E unresolved. Setter1314F4C writes word;129961A1 clears
  toFFFF and129961CF copies record10 under guarded path (leads, not identity).
  MISSING CHAIN PARTS: suppression semantics,mount,GS,class and all9 QA.
  BLOCKER: none offline. NEXT EXACT ACTION: trace setter call129A3CE and
  record10 provenance to determine whether suppression is equipment-specific.
  NEXT FILE TO OPEN: S21 129A3CE caller and full setter call context.
  NEXT FUNCTION TO IMPLEMENT: proven private class compatibility only.

- Completed source reaction sound branch inspection and pinned sex selector:
  BB6224 returns true for base2/5/7/8/9/12/14 via class&F helper960FB1.
  Class7 therefore takes female sound109/110; native IsFemale currently
  only Elf/Summoner. Final class contract now records this missing integration.
  REVERSE EVIDENCE: complete helper boundaries and140AD18/20/ADC2/ADC6 pins.
  CHAIN COVERAGE: player sound branch classification, not sound asset parity.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: reaction sound verifier,
  CLASS_INTEGRATION_CONTRACT.md,state. LAST STATIC CHECK: verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: native class7 sex absent;
  S21 character word322E==1A25 suppresses player scream, field meaning OPEN.
  MISSING CHAIN PARTS: mount/aux behavior, suppression identity, sound assets,
  class activation, GS and all9 gameplay gates. BLOCKER: none offline.
  NEXT EXACT ACTION: trace character322E getter writers/consumer semantics
  and sound109/110 registration before adding any private suppression rule.
  NEXT FILE TO OPEN: S21 getter1314FAE and pinned sound registry.
  NEXT FUNCTION TO IMPLEMENT: final GrowLancer sex integration with class
  activation, not a global change to DK testing behavior.

- Offline player snapshot confirms action328 cached speed0.4 (float bytes
  CDCCCC3E), matching native ZzzOpenData PLAYER_SHOCK.PlaySpeed0.4.
  REVERSE EVIDENCE: SHA-pinned snapshot model37CF7750/actionTable384FEFB0,
  modelA94 counts60/0/410; pinned consumer15773D0/D3/D6 establishes action
  stride16, table+30, speed+4. RuntimeQA compile command has no riding macro.
  CHAIN COVERAGE: matching clip and cached speed, not complete runtime timing.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: shock snapshot inspector,state.
  LAST STATIC CHECK: snapshot inspector PASS. LAST BUILD RESULT: unchanged
  PC/stage C93DB845,GS C85CEF94. LAST RUNTIME QA: no new live capture.
  KNOWN DIFFERENCES: initialization/later speed writes and auxiliary reactions
  remain unverified. MISSING CHAIN PARTS: GS producer, class and full9 QA.
  BLOCKER: none for offline reaction trace. NEXT EXACT ACTION: finish source
  reaction helper sound/mount tail; retain existing native230 without adding
  redundant clip or applying global S21 offset patches.
  NEXT FILE TO OPEN: S21 140AC15..140AF01 and native SetPlayerShock.
  NEXT FUNCTION TO IMPLEMENT: only proven private reaction differences.

- Compared S21 default shock328 against native player clips with a read-only
  cross-ID parser. Native230 is byte-identical: seven keys, unlocked root,
  clip SHA E1726BF433B5B0DE75040827E85E5331A6530EC6C3E025BA7366A5BC5220C3B8.
  All60 canonical bone names/parents/dummy flags match. Conditional riding
  macro would shift enum to236 (20 keys, NOT equivalent); compiled macro
  selection remains a separate gate. Native PlaySpeed is0.4, S21 not yet pinned.
  REVERSE EVIDENCE: original Player SHA E0F4CB5A0956192D04026135766C566BC6B9B7B559D2F196F74D24D28E2E755A;
  isolated Player SHA0CC3D22D unchanged. CHAIN COVERAGE: clip data only,
  not reaction timing or server producer. ASSET PROVENANCE: read-only comparison.
  FILES MODIFIED: inspect_player_shock_clip.py,state. LAST STATIC CHECK:
  comparison PASS; damage route verifier PASS; project3cpp/24headers PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: conditional enum and playback
  speed still require validation. MISSING CHAIN PARTS: aux reactions, GS,
  class integration and gameplay acceptance. Git remains modified/untracked;
  project inclusion is NOT a completed Git merge. BLOCKER: none offline.
  NEXT EXACT ACTION: inspect actual compile macro evidence and S21 action328
  speed initialization before declaring default shock behavior equivalent.
  NEXT FILE TO OPEN: RuntimeQA Obj CL.command.1.tlog, S21 animation setup.
  NEXT FUNCTION TO IMPLEMENT: only proven private reaction compatibility.

- Pinned S21 reaction branches: ordinary monster uses action5 excluding
  current3/4, matching native MONSTER01_SHOCK guard. Special monster763
  also suppresses actions>=8. Default player branch requests328 and clears
  movement; action setter skips identical action (no repeated-frame reset).
  REVERSE EVIDENCE: complete monster predicate/setter and action call pins.
  CHAIN COVERAGE: ordinary monster/default player branch, not clip parity.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: damage verifier,state.
  LAST STATIC CHECK: expanded damage verifier PASS. LAST BUILD RESULT:
  unchanged PC/stage C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: S21 player328 requires clip mapping; special763 and
  damage12/15 conditions not blindly ported globally.
  MISSING CHAIN PARTS: clip equivalence, auxiliary branches, GS type/timing.
  BLOCKER: none for offline action comparison. NEXT EXACT ACTION: resolve
  native PLAYER_SHOCK numeric and compare native/S21 player shock clips with
  existing action parser; retain native action if actual skeleton/timing match.
  NEXT FILE TO OPEN: _enum.h PLAYER_SHOCK, compare_player_actions.py.
  NEXT FUNCTION TO IMPLEMENT: source-proven private reaction mapping only.

- Identified and pinned S21 damage route23 ->12FEB6B ->128FE90 ->140A9C0:
  key3/4 high-bit call gate, masked actor key, damageDWORD8, typeD low nibble.
  Reaction helper allows type12/15 OR positive damage after action exclusions;
  native Hit>0 alone is not full S21 semantics. No global reaction edit.
  REVERSE EVIDENCE: SHA table/call/decoder/helper pins. CHAIN COVERAGE:
  damage receiver identity and initial reaction gate, not full handler.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: candidate inspector,damage
  verifier,Magic note,state. LAST STATIC CHECK: expanded damage verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: damage type12/15 exception;
  GrowLancer producer association unknown. MISSING CHAIN PARTS: full reaction
  action/mount/monster branches, GS production semantics/full gates.
  BLOCKER: none for offline handler trace. NEXT EXACT ACTION: finish140A9C0
  player/monster action mapping and compare native SetPlayerShock; isolate
  actual GrowLancer-specific requirement before any shared change.
  NEXT FILE TO OPEN: pinned140ABC4 onwards and native SetPlayerShock.
  NEXT FUNCTION TO IMPLEMENT: only proven private reaction compatibility.

- Verified native opcode11 routes ReceiveAttackDamage with conditional
  SetPlayerShock, whose player/monster/mount/dead/action guards remain native.
  Pinned S21 table130505C entry11 instead points return-only1305041 tail.
  Do not transplant native damage opcode or force shock from visual281.
  REVERSE EVIDENCE: SHA-pinned table/return instructions and native source.
  CHAIN COVERAGE: damage versus visual route boundary, S21 handler stillOPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: new damage boundary verifier,state.
  LAST STATIC CHECK: damage boundary verifier PASS after legacy source encoding
  read correction. LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: opcode mapping not shared across
  versions. MISSING CHAIN PARTS: actual S21 damage handler/producer/full gates.
  BLOCKER: none for offline dispatcher investigation. NEXT EXACT ACTION:
  enumerate pinned ProtocolCore direct receiver callsites and identify damage
  candidates by key/damage decode plus shock call, not numeric opcode alone.
  NEXT FILE TO OPEN: pinned ProtocolCore12FE380 table130505C and receiver calls.
  NEXT FUNCTION TO IMPLEMENT: source-proven private damage response only.

- Read pinned offline metadata tree for internal280/281: both category90=0;
  281 record164EF404 byte0=0/key4=281. Category5 facing suppression does not
  apply to base281. Helper140DC35 category0/key281 sets caster31E8 flag and
  timestamp31EC before conditional auxiliary descendants; not target action.
  REVERSE EVIDENCE: full snapshot374D1ECB, tree traversal, helper pins.
  CHAIN COVERAGE: base281 metadata/helper branch; no offset transplant.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: snapshot inspector,hit verifier,
  Magic note,state. LAST STATIC CHECK: internal snapshot and hit verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: auxiliary caster behavior distinct
  from target reaction; not implemented blindly. MISSING CHAIN PARTS:
  separate damage reaction/producer, auxiliary semantics, full acceptance.
  BLOCKER: none for offline damage receiver trace. NEXT EXACT ACTION: inspect
  native ReceiveDamage target-action path, locate matching S21 damage opcode
  dispatcher and compare conditions, keeping visual281 distinct.
  NEXT FILE TO OPEN: WSclient.cpp ReceiveDamage and S21 packet dispatcher.
  NEXT FUNCTION TO IMPLEMENT: only proven private damage-reaction mismatch.

- Traced shared receive prefix5372..5540: secondary object1948, source-facing
  write1944.Angle.Z, source target-index/success/skill writes194C; preswitch
  helper140DC35 receives source character, not target. Extra metadata90==5
  facing suppression exists; selected281 value not inferred from server XML.
  REVERSE EVIDENCE: pinned prefix instructions and existing native receiver.
  CHAIN COVERAGE: prefix field recipients distinguished, helper effects OPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: hit verifier,Magic note,state.
  LAST STATIC CHECK: expanded hit verifier PASS. LAST BUILD RESULT:
  unchanged PC/stage C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: metadata facing suppression not mapped by guess.
  MISSING CHAIN PARTS: earlier normalization/helper and separate damage reaction.
  BLOCKER: none for offline metadata audit. NEXT EXACT ACTION: verify cached
  client281 metadata90 and helper140DC35 selected behavior using pinned dump;
  then inspect damage receiver rather than invent reaction from visual branch.
  NEXT FILE TO OPEN: metadata lookup963291 / existing offline skill table tools.
  NEXT FUNCTION TO IMPLEMENT: only proven private receiver discrepancy.

- Audited complete internal281 receive body and return tail: target+140Z,
  subtype1/target owner match native; no direct1327DE8 action dispatch in
  these scopes. Reaction may be in other damage paths; no guessed action.
  REVERSE EVIDENCE: pinned body/tail/constants and native hit helper.
  CHAIN COVERAGE: selected hit contact/direct action scope only.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: new hit receive verifier,
  Magic note,state. LAST STATIC CHECK: hit receive verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: source sound wrapper signature
  differs; native registration default3D=false means passed target alone
  does not establish spatial mismatch. MISSING CHAIN PARTS: receiver prefix/
  separate damage reaction, sound scheduling and full acceptance.
  BLOCKER: none for offline receiver trace. NEXT EXACT ACTION: trace receiver
  prefix locals1948/194C target/caster and common reaction before skill switch;
  distinguish skill-visual response from damage response.
  NEXT FILE TO OPEN: pinned receiver entry leading12CB4A8 and native WSclient.
  NEXT FUNCTION TO IMPLEMENT: source-proven private target reaction only.

- Closed native uniform submission source trace: command BodyLight/MeshUv/
  EnableLight passed before bone upload and GL draw; FlushAllMesh traverses
  insertion order then clears. No selected color/UV mismatch established.
  REVERSE EVIDENCE: source caller/order assertions in shader verifier.
  CHAIN COVERAGE: shader submission source, not live GL execution.
  ASSET PROVENANCE: unchanged shader hashes. FILES MODIFIED: shader verifier,
  Magic note,state. LAST STATIC CHECK: expanded shader verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: live backend/visual unverified.
  MISSING CHAIN PARTS: target reaction, server authority and full acceptance.
  BLOCKER: none for offline contact trace. NEXT EXACT ACTION: move from
  lighting to internal281 receive12CB4A8, inspect common receiver tail for
  target action/reaction and compare native WSclient3852 CreateMagicPinHit.
  NEXT FILE TO OPEN: pinned12CB4A8 and WSclient receiver surrounding281.
  NEXT FUNCTION TO IMPLEMENT: source-proven private target reaction if absent.

- Checked BlendMesh VS/FS source and QA hashes equal; UV add/texture color
  multiplication preserved. For selected MagicPin03 alpha[0,.8], RGB after
  blend lies[0,.64]; shader clamp with lighting disabled does not change it.
  REVERSE EVIDENCE: source uniforms/GLSL, SHA pins and range check.
  CHAIN COVERAGE: shader input preparation and fragment arithmetic, not live.
  ASSET PROVENANCE: VS6A3A51A7/FS0D50150D; QA Effect junction targets shared
  D:\RISE-CrossPlatform\Client\Data\Effect, read-only, no writes performed.
  FILES MODIFIED: new shader verifier,Magic note,state. LAST STATIC CHECK:
  magic blend shader verifier PASS. LAST BUILD RESULT: unchanged PC/stage
  C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES: source
  uniforms do not prove actual GL draw or active backend.
  MISSING CHAIN PARTS: submission callsite/live parity and full gates.
  BLOCKER: none for offline submission trace. NEXT EXACT ACTION: inspect
  SendUniform caller and draw command ordering, then move to remaining
  non-lighting chain gaps if selected source path matches.
  NEXT FILE TO OPEN: native SendUniform callers.
  NEXT FUNCTION TO IMPLEMENT: only source-proven private submission mismatch.

- Audited actual RenderMesh CPU fallback (not only Alternative) and native
  shader AddMeshCommand blend branch: both multiply RGB by blend light and
  disable normal lighting. Shader sets Alpha1 before MakeShaderType; no
  extra non-unit alpha multiplication at that preparation step.
  REVERSE EVIDENCE: pinned S21 path plus native source branch assertions.
  CHAIN COVERAGE: selected color preparation across CPU/shader command paths.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: verifier,Magic note,state.
  LAST STATIC CHECK: expanded transform-light verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none; live backend NOT observed. KNOWN DIFFERENCES:
  shader capability is compiled by source define, activation needs GL3.3 and
  successful init; enqueue failure flushes then CPU fallback.
  MISSING CHAIN PARTS: shader execution/visual parity and full gates.
  BLOCKER: none for offline shader submission audit. NEXT EXACT ACTION:
  inspect MakeShaderType/Draw command uniforms and active shader source for
  BLENDMESH, ensure RGB/UV reach shader unchanged and no hidden alpha factor.
  NEXT FILE TO OPEN: New_ModelBMD.cpp MakeShaderType and shader binding paths.
  NEXT FUNCTION TO IMPLEMENT: only source-proven private render discrepancy.

- Closed selected fixed-function MagicPin03 normal-light bypass: flags42
  skips color/chrome masks into blend branch; texture slot0 matches blend0;
  local79 cleared then triangle texture-mode path skips per-normal colors.
  Native corresponding branch agrees. No lighting adapter/global BMD edit.
  REVERSE EVIDENCE: flag masks/branches, texture metadata and consumer pins.
  CHAIN COVERAGE: selected fixed-function lighting path, NOT allvisual parity.
  ASSET PROVENANCE: unchanged pinned MagicPin03 source/decoded hashes.
  FILES MODIFIED: verifier,Magic note,state. LAST STATIC CHECK: expanded
  transform-light verifier PASS. LAST BUILD RESULT: unchanged PC/stage
  C93DB845,GS C85CEF94. LAST RUNTIME QA: none. KNOWN DIFFERENCES: shader
  path still separate; no proven normal-light discrepancy on selected path.
  MISSING CHAIN PARTS: shader agreement, remaining full acceptance gates.
  BLOCKER: none for native source audit. NEXT EXACT ACTION: verify active
  native shader/CPU dispatch configuration and equivalent blend color policy
  in RenderMesh shader path; avoid auditing inactive paths as build evidence.
  NEXT FILE TO OPEN: ZzzBMD.cpp RenderMesh entry and jdk_shader_local330 config.
  NEXT FUNCTION TO IMPLEMENT: only proven private shader/color mismatch.

- Verified MagicPin03 original/decoded hashes and mesh texture slot0.
  S21 blend branch1332CBB..1332D6E compares texture slot (not meshindex),
  multiplies BodyLight by BlendMeshLight and clears normal-light flag79;
  native corresponding branch agrees. REVERSE EVIDENCE: SHA-pinned BMD
  mesh header, instruction pins, native branch assertion.
  CHAIN COVERAGE: branch-level blend/lighting behavior. ASSET PROVENANCE:
  source6F3BFA98/decoded415FE7BD confirmed read-only, no staging changes.
  FILES MODIFIED: transform-light verifier,Magic note,state.
  LAST STATIC CHECK: expanded transform-light verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845,GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: earlier flag routing and later
  color submission not yet closed. MISSING CHAIN PARTS: routing/consumer,
  shader parity and full gates. BLOCKER: none for offline renderer trace.
  NEXT EXACT ACTION: trace1331F3E flag selector for42 to blend branch and
  local79 consumers in triangle loop before asserting full lighting bypass.
  NEXT FILE TO OPEN: pinned1331F3E / renderer local79 uses.
  NEXT FUNCTION TO IMPLEMENT: only proven private color-path discrepancy.

- Completed selected5E7 constructor14808E9..1480D49 and return epilogue
  14B2E0C..14B2E35: no direct E4/E8 overrides. Discarded disassembly after
  RET (jump table, not code). Native blend-mesh branch disables per-normal
  lighting; S21 wrapper968CB3 forwards mesh rendering to13312EF.
  REVERSE EVIDENCE: complete selected scopes, pinned verifier and native source.
  CHAIN COVERAGE: direct constructor lighting overrides excluded, indirect
  callees/actual render consumption still OPEN. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: transform verifier,Magic note,state. LAST STATIC CHECK:
  expanded transform-light verifier PASS. LAST BUILD RESULT: unchanged
  PC/stage C93DB845,GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: cannot claim lighting parity from defaults alone.
  MISSING CHAIN PARTS: renderer blend-mesh lighting branch and full gates.
  BLOCKER: none for offline renderer trace. NEXT EXACT ACTION: follow
  13312EF renderer for mesh0/blendmesh0/flags42 and determine whether it
  bypasses normal-light intensity as native does.
  NEXT FILE TO OPEN: pinned13312EF and native ZzzBMD RenderMesh blend branch.
  NEXT FUNCTION TO IMPLEMENT: source-proven private renderer mismatch only.

- Confirmed common effect reset initializes luminosity E4=0.8/E8=0.2,
  exactly native CPU defaults. CreateEffect143E71F calls reset1315E97;
  reset stores1316368/637B use pinned constants. Selected5E7 constructor
  table resolves14808E9, ending selected block1480D45 before next case.
  REVERSE EVIDENCE: caller/reset/float/table pins. CHAIN COVERAGE: common
  defaults closed; selected overrides/callee side effects still OPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: transform verifier,note,state.
  LAST STATIC CHECK: expanded transform-light verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845, GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: configurability alone is not a
  selected skill mismatch. MISSING CHAIN PARTS: selected constructor lighting,
  shader parity and full acceptance. BLOCKER: none for offline trace.
  NEXT EXACT ACTION: inspect complete14808E9..1480D49 plus common postcase
  14B2E0C for lighting overrides, then callback render flags' lighting use.
  NEXT FILE TO OPEN: pinned5E7 constructor14808E9.
  NEXT FUNCTION TO IMPLEMENT: only a proven private lighting discrepancy.

- Traced preparation transform tail through complete wrapper968D0E into
  normal-light loop132E6A1: extra objectE4/E8 are dot-product multiplier and
  minimum luminosity, NOT geometry scale. Bias literal0.4; native CPU path
  uses0.8 multiplier/0.2 minimum. REVERSE EVIDENCE: pinned arguments, wrapper,
  loop and binary float constant. CHAIN COVERAGE: extra transform argument
  roles identified; selected initial values/shader parity remain OPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: new transform-light verifier,
  Magic note,state. LAST STATIC CHECK: transform-light verifier PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845, GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: configurable S21 luminosity vs
  native constants, actual selected values not yet known. MISSING CHAIN PARTS:
  initialization/lighting relevance, indirect geometry and full acceptance.
  BLOCKER: none for offline initializer trace. NEXT EXACT ACTION: find common
  effect construction/reset E4/E8 writes and selected5E7 overrides; determine
  whether LightEnable makes this path active before adding any adapter.
  NEXT FILE TO OPEN: pinned effect constructor/reset and5E7 constructor.
  NEXT FUNCTION TO IMPLEMENT: source-proven private lighting correction only.

- Closed selected5E7 preparation reachability to owner-status gate on all
  maps: bypasses special types7E4/7E3, map6E/6F type0, map84 type1E.
  Pinned table identities61/72/178/280 = Stun/Sleep/Darkness/Stun;
  native enum61/72 matches Stun/Sleep. Do not alias280 or truncate to24.
  REVERSE EVIDENCE: branch pins and SHA-pinned XML parsed in memory.
  CHAIN COVERAGE: initial origin through owner gate; extra status delivery OPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: two verifiers, Magic note,state.
  LAST STATIC CHECK: Magic path and auxiliary buff verifiers PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845, GS C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: extra statuses lack mapped delivery.
  MISSING CHAIN PARTS: indirect animation/transform consumers, full gates.
  BLOCKER: none for offline trace. NEXT EXACT ACTION: verify selected normal
  preparation tail into968D0E (transform), compare native BMD transform inputs;
  tail inspected1887852..1887964 but not yet pinned in verifier.
  NEXT FILE TO OPEN: dump968D0E and native Calc_RenderObject transform tail.
  NEXT FUNCTION TO IMPLEMENT: source-proven private transform discrepancy only.

- Completed lighting helper1885EB0..1886381 audit: no direct1D0/34C access.
  Identified separate common preparation owner-status gate18872E6..737D:
  four keys3D/48/B2/118 can zero effectD0. Native Calc_RenderObject already
  handles owner Stun/Sleep; extra status translation remains unproven.
  REVERSE EVIDENCE: pinned complete helper and exact gate instructions.
  CHAIN COVERAGE: lighting helper closed for direct snapshot access only.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: Magic note/verifier/state.
  LAST STATIC CHECK: expanded Magic verifier PASS. LAST BUILD RESULT:
  unchanged PC/stage C93DB845, GS C85CEF94. LAST RUNTIME QA: none.
  KNOWN DIFFERENCES: extra S21 statuses not mapped; no guessed numeric alias.
  MISSING CHAIN PARTS: remaining preparation branches/reachability, full gates.
  BLOCKER: none for offline analysis. NEXT EXACT ACTION: decode preparation
  from18865E9 through18872E6 at valid boundaries to establish5E7 route;
  then verify status identities from pinned table and native enum.
  NEXT FILE TO OPEN: dump18865E9, BuffEffectManager.xml read-only.
  NEXT FUNCTION TO IMPLEMENT: only proven private preparation discrepancy.

- Reconfirmed project integration: 3 cpp and 24 headers included exactly once,
  no exclusions; full isolated stage verifier PASS (2658 base, 79 overlay,
  3 root Data files and Engine/Player hashes). Git remains modified/untracked;
  this is NOT a completed merge, and no protected branch was changed.
  REVERSE EVIDENCE: outer Magic Pin route 15BD5DC -> 176D621 first calls
  preparation 1886382; only success calls renderer 1887B8B. Initial preparation
  copies effect Position158 XYZ into model78, not owner snapshot1D0, after
  helper1885EB0. CHAIN COVERAGE: selected initial origin path only.
  ASSET PROVENANCE: unchanged, stage verified. FILES MODIFIED: verifier/state.
  LAST STATIC CHECK: expanded magic controller and project/stage PASS.
  LAST BUILD RESULT: unchanged PC/stage C93DB845, GS C85CEF94.
  LAST RUNTIME QA: none this run. KNOWN DIFFERENCES: remaining preparation
  branches/helper and shadow consumers not closed. MISSING CHAIN PARTS:
  remaining render preparation, full GS/class and visual acceptance.
  BLOCKER: none for offline trace. NEXT EXACT ACTION: inspect helper1885EB0
  and remaining1886382 preparation before excluding snapshot consumers.
  NEXT FILE TO OPEN: pinned dump1885EB0. NEXT FUNCTION TO IMPLEMENT:
  only a source-proven private adapter discrepancy, if found.

- Audited completeMagicPin03 registeredrendererA49029..A49188 andcommon
  tail15945F1..1594708: no direct1D0/Owner34C access; meshwrapper receivesmodel
  plus scalararguments.5E7 skips special413/7EF7 recursion/decrement paths;
  preserves selected life10..0 visits. REVERSE EVIDENCE:fullfunction/pinchecks.
  CHAIN COVERAGE:direct snapshot consumers excluded in these two scopes only.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:Magic verifier,note,state.
  LAST STATIC CHECK:renderer/tail verifier PASS (re-run confirmed). LAST BUILD RESULT:unchanged
  stagedC93DB845/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:outer
  render preparation/destructor internals stillnotcovered; no speculativeadapter.
  MISSING CHAIN PARTS:outer snapshotconsumers and remaining fullacceptancegates.
  BLOCKER:none for offline renderpreparation trace. NEXT EXACT ACTION: trace
  modelrender preparation for5E7 before registeredcallback, especially shadow/
  terrain passes using effect1D0. NEXT FILE TO OPEN:default effectrender route
  15BD5DC and modelrender1887... entries. NEXT FUNCTION TO IMPLEMENT:proven
  private ground snapshot behavior if visible consumerfound.

- Confirmed S21 common prefix DOES dereference non-null owner for MagicPin
  into snapshot1D0 and terrainZ; no Live test. Selected5E7 does not directly
  consume1D0; native private early-return skips that snapshot path. No guessed
  target-follow motion or unused snapshot port. REVERSE EVIDENCE:prefixpins
  14B77AC..780C plus native private MoveEffect ordering.
  CHAIN COVERAGE:common owner read identified, consumer closure stillOPEN.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:Magic verifier,note,state.
  LAST STATIC CHECK:expanded Magic prefix verifier PASS. LAST BUILD RESULT:unchanged
  stagedC93DB845/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:bounded
  child body no-owner claim does not covercommonprefix; snapshot consumersOPEN.
  MISSING CHAIN PARTS:renderer/common-tail/indirect snapshot consumers,fullgates.
  BLOCKER:none for offline consumer trace. NEXT EXACT ACTION:trace5E7 registered
  renderer and common15945F1 tail for snapshot1D0 use before deciding adapter.
  NEXT FILE TO OPEN:MagicPin rendererA490xx andcommon effecttail15945F1.
  NEXT FUNCTION TO IMPLEMENT:source-proven snapshot behavior if consumerfound.

- Audited MagicPin03/5E7 selected update: self Angle164 and Position158 drive
  motion, no direct Owner34C in complete15786D7..15788B1. Native selected body
  matches self-relative motion/no owner dereference. No speculative follow or
  owner-expiry kill introduced. REVERSE EVIDENCE:self-field/function pins.
  CHAIN COVERAGE:selected child update only, common prefix stillnotexcluded.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:Magic verifier,note,state.
  LAST STATIC CHECK:expanded Magic ownership verifier PASS. LAST BUILD RESULT:unchanged
  stagedC93DB845/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:bounded
  no-owner access is not whole-engine pool reuse proof. MISSING CHAIN PARTS:
  common prefix and descendants ownership,liveQA,fullGS/class gates.
  BLOCKER:none for offline prefix trace. NEXT EXACT ACTION: trace common effect
  update entry through5E7 dispatch for Owner accesses or cleanup conditions.
  NEXT FILE TO OPEN:primary effect update entry/selector in pinned dump.
  NEXT FUNCTION TO IMPLEMENT:private ownership fix if common-prefix mismatchfound.

- Staged owner-availability corrected candidate C93DB845 with old/new hashguards
  and non-reparse checks. Backup:RuntimeQA/Bin/Engine-Port S21.pre-owner-guard-
  22FCA35D.exe (exact priorhash). No launch,production writes or cross-task merge.
  REVERSE EVIDENCE: same source-pinned Clash frame/cache guards.
  CHAIN COVERAGE: corrected native executable now in isolated testclient.
  ASSET PROVENANCE:2658base/79overlay/3rootData verified unchanged.
  FILES MODIFIED:isolated Engine,stage verifier hash,state;backupcreated.
  LAST STATIC CHECK:fullstage/Engine/PlayerCRC PASS,Clash frame/cache/ownerguard
  PASS,project3cpp/24headers PASS. LAST BUILD RESULT:PC and staged
  C93DB84572CD6148F914433F7D4B620CD052B83CCD871763CC0F9346FC4ABBD7;
  GS C85CEF94 unchanged. LAST RUNTIME QA:none. KNOWN DIFFERENCES:Live/action
  pointer checks do not establish generation-safe owner reuse; no such native
  guard found by focused runtime search. MISSING CHAIN PARTS:owner reuseQA,
  fullGS/newclass and allvisual acceptance. BLOCKER:none for offline chain audit.
  NEXT EXACT ACTION:inspect MagicPin controller-owned children and pool reset
  together; distinguish actor owner from pooled effect owner before addingguard.
  NEXT FILE TO OPEN:MagicPin03 tick/update and source controller ownerchain.
  NEXT FUNCTION TO IMPLEMENT:private ownership correction only with evidence.

- Added scoped Clash owner-action availability recheck before emission. Both
  QA/WSclient creation already guard action293; update now skips missing model/
  mapped action without consuming counter, lifetime still handled by whole ticks.
  REVERSE EVIDENCE:S21 model-null common-tail skip plus native dispatch guards.
  CHAIN COVERAGE:owner model change compatibility, not pointer-reuse guarantee.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:runtime.cpp,Clash verifier,note,state.
  LAST STATIC CHECK:gate/cache/owner guard verifier PASS. LAST BUILD RESULT:
  isolated Win32 QA Build exit0, known cryptlibLNK4099;
  candidate C93DB84572CD6148F914433F7D4B620CD052B83CCD871763CC0F9346FC4ABBD7.
  NOT STAGED yet: testclient22FCA35D,GS C85CEF94 unchanged. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:native appended-action availability is adapter safety, S21
  gate only checks cache existence. MISSING CHAIN PARTS:stage,live owner-change
  retry/pool reuse,fullGS/newclass gates. BLOCKER:none for staging/statictests.
  NEXT EXACT ACTION:stage C93DB845 with previouscandidate backup/hashguard and
  rerun complete QA verifier; then review owner reuse safeguards for all9skills.
  NEXT FILE TO OPEN:verify_runtime_qa_stage.py and runtime owner handling.
  NEXT FUNCTION TO IMPLEMENT:source-proven ownership correction if discrepancy.

- Completed selected Clash cached-result usage audit157D996..157DF45: both
  result locals only stored/null-tested, no later load/address-taking. Frame
  copy uses owner/controller OBJECT locals, not model. No extra bone/action
  requirement for empty controller model is justified by this block.
  REVERSE EVIDENCE: bounded disassembly and exact local use counts; verifier.
  CHAIN COVERAGE: controller cache geometry requirement excluded in this block.
  ASSET PROVENANCE:unchanged; nofakeBMD. FILES MODIFIED:Clash verifier,note,state.
  LAST STATIC CHECK: expanded cache-use verifier PASS. LAST BUILD RESULT:
  unchanged staged22FCA35D / GS C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES: lookup sideeffects/missing owner-model condition separate;
  source-equivalent logical controller not proof of GPU/gameplay parity.
  MISSING CHAIN PARTS:owner model availability,fullGS/newclass/liveQA.
  BLOCKER:none for native owner-state check. NEXT EXACT ACTION: inspect existing
  CanPlay action/owner safety at dispatch and received Clash path; determine
  whether unloaded-model owner can reach effect update without valid action.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp and RuntimeQA dispatch wrappers.
  NEXT FUNCTION TO IMPLEMENT:scoped owner availability guard if unsupportedpath.

- Staged corrected Clash timing candidate with exact old/new hash guards and
  non-reparse checks; preserved prior90416CB2 candidate at RuntimeQA/Bin/
  Engine-Port S21.pre-clash-gate-90416CB2.exe. No game launch/deployment.
  REVERSE EVIDENCE: same pinned(1.0,6.5] gate. CHAIN COVERAGE:built correction
  now present in isolated client. ASSET PROVENANCE:unchanged,all staging checked.
  FILES MODIFIED:isolated Engine artifact,stage verifier hash,state;backupcreated.
  LAST STATIC CHECK:stage2658base/79overlay/3rootData PASS,Engine/PlayerhashCRC
  PASS;project3cpp/24headers PASS;Clash gate source verifier PASS.
  LAST BUILD RESULT:PC/staged22FCA35D6075A0FCCC28B73211C487A134352C646A4E935A0B0ACF00A689407A;
  GS unchangedC85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:build/staging
  not visual acceptance. MISSING CHAIN PARTS:remaining cache consumers,liveQA,
  nativeclass/fullGS. BLOCKER:none for offline trace.
  NEXT EXACT ACTION: finish Clash cached-model-result use audit over selected
  handler before deciding whether native controller needs extra guard.
  NEXT FILE TO OPEN:pinned157D9AE..157DF45 local11A08/11A10 usages.
  NEXT FUNCTION TO IMPLEMENT:source-proven Clash controller compatibility.

- Fixed real Clash premature emission: pinned157DA26/2D threshold1B4DD48 is
  1.0, not0.0. Shared ClashEmissionDue now requires(1.0,6.5],counter0.
  REVERSE EVIDENCE:new verify_clash_emission_gate.py pins instructions/constants.
  CHAIN COVERAGE: launch timing correction. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:Clash helper,test,source verifier,Clash note,state.
  LAST STATIC CHECK:x86W4WX 12tick+6pose+expanded frame boundary PASS;
  source verifier PASS. LAST BUILD RESULT:isolated QA Win32 Build exit0,
  Engine SHA22FCA35D6075A0FCCC28B73211C487A134352C646A4E935A0B0ACF00A689407A.
  Known cryptlib LNK4099 only. NOT STAGED: client test still previous90416CB2;
  GS unchangedC85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES: prior tests
  encoded wrong zero lower bound; superseded by new source-pinned boundaries.
  MISSING CHAIN PARTS:stage corrected candidate,liveClashQA and remaininggates.
  BLOCKER:none for isolated staging. NEXT EXACT ACTION: read existing isolated
  staging/manifest tooling, preserve oldcandidate and stage hash22FCA35D safely.
  NEXT FILE TO OPEN:tools/grow_lancer/verify_runtime_qa_stage.py and QA setup.
  NEXT FUNCTION TO IMPLEMENT:continue cache consumer audit after staged rebuild.

- Added executable QA procedure for mixed-pool Shining seed-tail comparison
  (not run), corrected acceptance matrix to distinguish skill versus buff wire
  capacity. Advanced Clash cache audit using pinned full dump:5FA model exists
  at3B348F80, bones/meshes/actions all0, manager26D1B600 count4967.
  REVERSE EVIDENCE: hash-checked offline map traversal; no process access.
  CHAIN COVERAGE: invisible controller cache presence; not all record consumers.
  ASSET PROVENANCE:no fakeBMD created. FILES MODIFIED:offline inspector supports
  scoped5FA/5FD selection,Clash note,QA,matrix,state. LAST STATIC CHECK:5FA
  snapshot lookup PASS. LAST BUILD RESULT:unchanged PC90416CB2/GS C85CEF94.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:S21 cache entry versus native logical
  controller must map behavior,not copied layout. MISSING CHAIN PARTS:remaining
  cached field consumers and owner model gate,liveQA,GS/class integration.
  BLOCKER:none for Clash consumer trace. NEXT EXACT ACTION: trace157D9AE onward
  uses of both model lookup results before child emission, distinguishing null
  gates from animation/geometry reads. NEXT FILE TO OPEN:pinnedClash handler.
  NEXT FUNCTION TO IMPLEMENT:native Clash cache-equivalent guard if needed.

- Decoded hash-pinned pin_lights16x128 RGB and checked UV3 seed-segment range.
  Seed segment crosses bright columns at updates8..10, not only dark border.
  This excludes texture-edge invisibility as blanket justification for ignoring
  pool history, without claiming actual GPU error. Native loader linear/clamp.
  REVERSE EVIDENCE: existing provenance hash, decoded column maxima and UV math.
  CHAIN COVERAGE: potential seed texture contribution. ASSET PROVENANCE:source
  unchanged/read-only. FILES MODIFIED:seed texture verifier,Shining note,state.
  LAST STATIC CHECK: seed texture/UV verifier PASS. LAST BUILD RESULT:unchanged
  PC90416CB2/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES: mixed-slot
  history requires gameplay comparison, not invented reset. MISSING CHAIN PARTS:
  live seed comparison plus GS/class gates. BLOCKER: runtime seed acceptance
  requires later isolated gameplay; offline other-skill audit remains possible.
  NEXT EXACT ACTION: record mixed-pool Shining test in runtime QA checklist,
  then resume unresolved client chain entries outside this runtime-only question.
  NEXT FILE TO OPEN:GrowLancer/RUNTIME_QA.md and acceptance matrix.
  NEXT FUNCTION TO IMPLEMENT: next source-proven client discrepancy, if found.

- Located actual joint array initializer8DDA8B and complete element constructor
  10F040E: vector initialization only, no Scale14 assignment. Explicit delete
  branches and ordinary expiry clear Live only; native lifecycle matches that
  behavior. Do not invent a missing Scale reset from pool-history differences.
  REVERSE EVIDENCE: initializer/constructor/delete pins; native global POD.
  CHAIN COVERAGE: construction and reuse cleanup; first-use image state notproved.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:pin verifier,Shining note,state.
  LAST STATIC CHECK: constructor/delete verifier PASS. LAST BUILD RESULT: unchanged
  PC90416CB2/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES: mixed pool
  history can differ; same source lifecycle is not identical GPU output.
  MISSING CHAIN PARTS: seed UV/raster contribution, live mixed-poolQA,GS logic.
  BLOCKER:none for renderer/asset audit. NEXT EXACT ACTION: inspect seed-end
  UV at light-on and actual pinned pin_lights texture border contribution;
  do not infer visual immunity from black-before-life16.
  NEXT FILE TO OPEN: pin_lights asset registry/provenance and renderer UV3.
  NEXT FUNCTION TO IMPLEMENT: scoped seed handling only if mismatch proven.

- Traced Shining CreateTail(false) and renderer indices. Seed tail is shifted,
  not immediately overwritten: survives index5 at light-on update5 through
  update17, drops update18. Added binary pins and deterministic history test.
  REVERSE EVIDENCE:1618B13/1F,15E37EF..3901,renderer j<NumTails and j+1 loads.
  CHAIN COVERAGE: seed reachability, not raster contribution or prior poolScale.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:pin verifier,Shining note,state.
  LAST STATIC CHECK: expanded pin/history verifier PASS. LAST BUILD RESULT:unchanged
  PC90416CB2/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:early dark
  ticks do not eliminate seed dependency. MISSING CHAIN PARTS: source poolScale
  initialization/reuse and actual seed UV contribution; fullGS/liveQA remain.
  BLOCKER:none for source initialization audit. NEXT EXACT ACTION: inspect S21
  joint array constructor/reset to establish first-use Scale and reuse behavior.
  NEXT FILE TO OPEN: pinned joint pool A5E15B0 constructor/xrefs.
  NEXT FUNCTION TO IMPLEMENT: private seed contract only after scale evidence.

- Closed Shining initial-tail suppression and four-vector construction from
  pinned prefix:8073 creates initial tail0 with copied caller angle and stored
  Scale/2 BEFORE pin subtype sets callerScale/AngleX-=135. Native has matching
  order; pool prior-Scale dependency is not proven equal between engines.
  REVERSE EVIDENCE:15C6B25..15C7270 plus native initial geometry/source order.
  CHAIN COVERAGE: initial four vertices; visibility/overwrite still needs trace.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:pin verifier,Shining note,state.
  LAST STATIC CHECK: initial-geometry verifier PASS. LAST BUILD RESULT: unchanged
  PC90416CB2/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:prior slot
  Scale not proven identical; do not reorder init or change global SS6 reset.
  MISSING CHAIN PARTS: initial-tail renderer visibility/overwrite, liveQA,GS.
  BLOCKER:none for client tail trace. NEXT EXACT ACTION: trace first CreateTail
  update and renderer segment indices to establish whether tail0 survives until
  pin Light turns nonzero at life16. NEXT FILE TO OPEN:1618ADA and joint renderer.
  NEXT FUNCTION TO IMPLEMENT: scoped seed-tail compatibility only if observable.

- Closed Shining joint allocator special-pool exclusion: complete10F0332
  predicate accepts only local-owned7FCE sub0/10/11/12, not8073. Ordinary500
  slot path applies; selected constructor tail caps MaxTails50, leaves18 and
  returns without additional tail emission. General+A44 initializer reads
  low16 bits of arg+38 before selected subtype overwrites20; meaning stillOPEN.
  REVERSE EVIDENCE: predicate, allocation prefix and final-block pins.
  CHAIN COVERAGE: ownership/pool selection and constructor exit; geometryOPEN.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:pin verifier,Shining note,state.
  LAST STATIC CHECK: expanded pin verifier PASS. LAST BUILD RESULT: unchanged
  PC90416CB2/GS C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:no invented
  field/manager port. MISSING CHAIN PARTS: initial geometry, external+A44 reads,
  live acceptance and authoritative GS. BLOCKER:none for client prefix audit.
  NEXT EXACT ACTION: follow common constructor initial-tail geometry before
  subtype dispatch and compare native CreateJoint initial orientation/scale.
  NEXT FILE TO OPEN:pinned15C6715 onward and nativeZzzEffectJoint.cpp prefix.
  NEXT FUNCTION TO IMPLEMENT: initial pin tail adapter only if source mismatch.

- Verified supplied S21 GS evidence boundary: only IGC.GameServer2_R.exe in
  scoped binary/source inventory, SHA134BAEBF, AMD64 base140000000, boot entry
  RVA481B058 and protection sections. No raw ASCII formula/Wrath anchors.
  REVERSE EVIDENCE: read-only PE inspection in S21_SERVER_EVIDENCE_LIMIT.md.
  CHAIN COVERAGE: server consumer evidence unavailable from this inspection;
  XML formulas remain useful but not runtime behavior. ASSET PROVENANCE:unchanged.
  FILES MODIFIED: evidence-limit note,state. LAST STATIC CHECK: PE fields read;
  no runtime edit/build. LAST BUILD RESULT: unchanged PC90416CB2/GS C85CEF94.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES: client dump cannot prove GS logic.
  MISSING CHAIN PARTS: authoritative server handlers/behavior and live acceptance.
  BLOCKER: server semantics need matching runtime dump/PDB or explicit approved
  replacement behavior; independent client reverse remains available.
  NEXT EXACT ACTION: resume Shining Peak unresolved joint field+A44 consumers
  from pinned client dump, without treating absent direct xrefs as proof unused.
  NEXT FILE TO OPEN: SHINING_PEAK_REVERSE.md sections at field+A44.
  NEXT FUNCTION TO IMPLEMENT: source-proven private pin behavior if mismatch found.

- Located hash-pinned Wrath formulas specifically under FormulaData/Character:
  44 -> 10, 46 -> (STR+AGI)/3, 47 -> -0.05*DEF mathematically. Repeated IDs in
  other sections differ. SkillList matches accepted profile; 278/895 Duration0
  cannot establish infinite/zero lifetime without server consumer evidence.
  REVERSE EVIDENCE: FormulaData and SkillList exact hashes/section attributes.
  CHAIN COVERAGE: buff formula inputs, not application units/rounding/lifecycle.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: Wrath note, focused verifier,state.
  LAST STATIC CHECK: Wrath formula verifier PASS. LAST BUILD RESULT: PC90416CB2/GS C85CEF94
  unchanged. LAST RUNTIME QA:none. KNOWN DIFFERENCES: semantic meaning of
  Duration0 remains OPEN. MISSING CHAIN PARTS: formula consumers and buff timer.
  BLOCKER:none for offline caller search. NEXT EXACT ACTION: find source formula
  strings/Character formula44/46/47 callers in available S21 server evidence.
  NEXT FILE TO OPEN: available S21 server binaries/source inventory read-only.
  NEXT FUNCTION TO IMPLEMENT: Wrath authoritative calculation after call proof.

- Audited GS buff sender, in-memory capacity and GS/DS persistence. CEffect
  stores BYTE index with FF empty sentinel; MAX_EFFECT=256, actor slots=32.
  Legacy 07/2D and viewport carry BYTE IDs; save records are 13 bytes in both
  GS and DS, with index at byte zero. No blind slot/packet/DB widening.
  REVERSE EVIDENCE: EffectManager/Effect/Viewport/DSProtocol and DS reader/save;
  S21 pinned table confirms Wrath424/425 ClearType1 is death/regen, not logout.
  CHAIN COVERAGE: server delivery/persistence integration surfaces identified.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: storage verifier, auxiliary note,
  state. LAST STATIC CHECK: expanded storage verifier PASS, including GS/DS.
  LAST BUILD RESULT: unchanged PC90416CB2/GS C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES: Wrath duration/save policy unresolved; legacy packet width
  cannot represent source IDs. MISSING CHAIN PARTS: full authoritative GL buffs,
  class integration and visual acceptance. BLOCKER:none for offline metadata.
  NEXT EXACT ACTION: trace source Wrath duration and lifecycle data before
  implementing scoped buff extension; do not modify existing DS record width.
  NEXT FILE TO OPEN: GrowLancer/WRATH_REVERSE.md and supplied Skill/Formula data.
  NEXT FUNCTION TO IMPLEMENT: verified Wrath buff lifecycle contract.

- Closed auxiliary membership/insert/removal helper boundaries: S21 absent-key
  insert owns 12-byte payload (1,arg2,-1), duplicate insert does not refresh;
  native map stores DWORD 1 instead. Added verify_buff_storage_contract.py.
  REVERSE EVIDENCE: anchored 130C63E/130C769/130C8A5 and native Buff source.
  CHAIN COVERAGE: storage ownership, not S21 packet ingress yet.
  Found native status packet plus five viewport lists use BYTE IDs: cannot
  carry Wrath 424/425 or auxiliary 274/278. Skill-ID capacity is independent.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: storage verifier, auxiliary note,
  state. LAST STATIC CHECK: naming and storage verifiers PASS. GS header also
  declares GCEffectStateSend(LPOBJ,BYTE state,BYTE effect); sender body next.
  LAST BUILD RESULT: unchanged PC 90416CB2 / GS C85CEF94 (no runtime edits).
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: buff payload fields/ingress unknown.
  MISSING CHAIN PARTS: extended authoritative buff delivery and full nine-skill
  acceptance. BLOCKER: none for offline GS sender trace.
  NEXT EXACT ACTION: inspect GS effect packet sender and client receive routing.
  NEXT FILE TO OPEN: ExGameServer/GameServer/EffectManager.cpp.
  NEXT FUNCTION TO IMPLEMENT: scoped buff wire contract after sender verification.

- Pinned supplied BuffEffectManager.xml and named eight auxiliary callback keys;
  see GrowLancer/AUXILIARY_STATUS_MAPPING.md. Native enum verifies Freeze=56,
  BlowOfDestruction=86; six additional S21 states remain unmapped. Source XML
  malformed comment is stripped only in verifier memory, never edited upstream.
  REVERSE EVIDENCE: hash-pinned table plus native enum prefix; client packet
  translation remains OPEN. CHAIN COVERAGE: auxiliary post-render status naming.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: status note, naming verifier,
  renderer verifier final scope message, this state.
  LAST STATIC CHECK: project 3 cpp/24 headers PASS; staged 2658 base/79 overlay/
  3 root Data PASS including Engine/Player hashes; GS early guards PASS.
  LAST BUILD RESULT: unchanged PC 90416CB2 / GS C85CEF94; no rebuild this phase.
  LAST RUNTIME QA: none, owner sleeping. KNOWN DIFFERENCES: table naming is not
  proven client status translation. MISSING CHAIN PARTS: native class, full GS
  authority and runtime acceptance remain open. BLOCKER: none for offline trace.
  Integration audit: branch feature/grow-lancer-skills-s21 HEAD f2e8e558;
  modified/untracked implementation is NOT committed/merged. No cross-task merge.
  NEXT EXACT ACTION: trace S21 status ingestion and compare native buff updates.
  NEXT FILE TO OPEN: binary membership helper 0x130C8A5 and status insertion xrefs.
  NEXT FUNCTION TO IMPLEMENT: evidence-gated auxiliary status adapter.

- Found native lateoverlay inDraw_RenderObject2622..2630:Freeze preservesRGB,
  BlowOfDestruction writes(.3,.5,1), bothbody flags2/blend-2/light1. Source
  pairchecked; largerS21membershipset notautomaticallymapped. Equipment
  RenderPartObjectEffect hasadditionalStun skullsideeffects,so notsubstitute
  auxiliaryrenderpath. REVERSE EVIDENCE:nativefunctionboundaries/sourcepair
  versuscompleteS21postcallback. CHAIN COVERAGE:nativeintegrationentrychoice.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:renderverifier,Breche reverse,state.
  LAST STATIC CHECK:exactnativeoverlaypair added. LAST BUILD RESULT:unchanged
  90416CB2/C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:statusmapping
  mustremainexplicit,notgenericitemrendererreuse. MISSING CHAIN PARTS:
  auxiliarynativeactivation andallliveacceptance. BLOCKER:none formappingaudit.
  NEXT EXACT ACTION: resolveS21membershipIDs againstsuppliedbufftables, then
  map onlyproven nativeequivalents; remainingstates stayexplicitcompatgates.
  NEXT FILE TO OPEN:suppliedS21buff/effecttables andnativeeBuffState.
  NEXT FUNCTION TO IMPLEMENT:verified auxiliarystatusoverlaymapping.

- Completed1893296 postcallback decode: no741specialcase/automaticRGBrestore;
  statusmembership firstgroup drawsbody withincomingRGB, alternate56 writesRGB
  beforebody,otherwise no draw. RestoringRGB beforecallback wouldchangefirst
  branch. REVERSE EVIDENCE:completefunction andnewbranch/bodycallpins.
  CHAIN COVERAGE: postrender state dependency. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:Giant verifier,Breche reverse,state. LAST STATIC CHECK:new
  callbackchecks added; no native change. LAST BUILD RESULT:unchanged
  90416CB2/C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:statusIDs not
  assigned guessednames; restoration scope mustincludepostrenderbehavior.
  MISSING CHAIN PARTS:nativestatusmapping/configurationactivation/liveQA.
  BLOCKER:none fornative statusrendercomparison. NEXT EXACT ACTION: compare
  nativeRenderObject status-body overlays and current auxiliary model render
  entry; decide correct isolated scope withoutduplicatingglobalstatuslogic.
  NEXT FILE TO OPEN:ZzzObject.cpp nativeRenderObject statusoverlay.
  NEXT FUNCTION TO IMPLEMENT:auxiliaryrenderer/postoverlay integration contract.

- Closed renderer mapconsumer:1887DF2..E5C looksupobject.Type in1E050B0,
  indirectcallsregisteredrenderer and true skipsdefaultbody; afterwardcalls
  1893296. Native custommeshlist restoresRGB,butdefault-effect path doesnot
  unconditionallyrestore. REVERSE EVIDENCE:mapconsumerpins andnative
  CustomRenderEffect/CustomItemRenderMesh source. CHAIN COVERAGE: dispatch
  association andcleanup boundary. ASSET PROVENANCE:unchanged. FILES MODIFIED:
  Giantverifier,Breche reverse,state. LAST STATIC CHECK:5consumerpins added.
  LAST BUILD RESULT:unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:sourcepostcallback mayconsumechangedRGB;noassertionof
  automaticrestoration orglobalitemfix. MISSING CHAIN PARTS:auxiliarynative
  integration andfull9skillliveacceptance. BLOCKER:none forpostcallbackaudit.
  NEXT EXACT ACTION: inspect1893296 Giant741 branch/postrender dependencies
  before choosingprivateRGBscope; retainexistingSS6itembehavior.
  NEXT FILE TO OPEN:pinned1893296 entry/741 selection.
  NEXT FUNCTION TO IMPLEMENT:auxiliaryrenderer state-restoration contract.

- Implemented private fixed(-4,0) GiantPulse candidate with externally sampled
  floattime, x86CVTTSS2SI and signedremainder, cosine and source meshremap.
  No nativeRender22/globalclock changes. REVERSE EVIDENCE: pinnedsourcepulse
  contract. CHAIN COVERAGE: isolatedmathhelper, NOTactivatedauxiliaryrenderer.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:compat/S21GiantPulse.h,
  GrowLancerGiantPulseTest.cpp,Breche reverse,state. LAST STATIC CHECK:
  nineboundaries plus1570ticks PASS; overflow/NaN/infinity conversion tested.
  LAST BUILD RESULT:focusedx86 W4/WX fp:precise compile/run PASS; client/server
  candidates unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:stdcos exactS21CRTbitparity unproven; helper intentionally
  unconnected/notinclientproject untilconfigurationintegrationready.
  MISSING CHAIN PARTS: auxiliaryactivation/render/state ownership andliveQA.
  BLOCKER:none forsharedrender-stateaudit. NEXT EXACT ACTION: trace renderer
  caller BodyLight save/restore before wiring10pass auxiliary path; confirm
  noleak betweenactors. NEXT FILE TO OPEN: S21 renderdispatch A1DCE6 map
  consumer andnativeBMD runtime_make_render caller. NEXT FUNCTION TO IMPLEMENT:
  isolated auxiliaryrenderer withverifiedstateownership,notglobalitem changes.

- Found nativeRender22 but disproved equivalentreuse: a1=-4 produces divisor
  -251 versusS21 -1570; reducedphase0 returns.5 versus0. Native WorldTime
  isdouble CTimerelapsed,notS21 sampledfloat timeGetTime. REVERSE EVIDENCE:
  CustomItemRenderMesh.cpp7..22,ZzzAI.cpp786/814,pinnedS21 helper. CHAIN
  COVERAGE: native timing/helpercompatibility. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:Giant verifier,Breche reverse,state. LAST STATIC CHECK:
  helper mismatch assertions added; priorS21pins retained. LAST BUILD RESULT:
  unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:
  existing itemRender22 callers mustremainunchanged;no globalclockpatch.
  MISSING CHAIN PARTS: privateadapter numerical/runtimevalidation andclass/GS.
  BLOCKER:none forbounded numerical contract. NEXT EXACT ACTION: runexpanded
  verifier then define sourcepulse input-domain/invalidconversion behavior
  before a privateadapter, preserving signedremainder and sampledfloatclock.
  NEXT FILE TO OPEN:verify_giant_render_passes.py and sourcepulse18F930D.
  NEXT FUNCTION TO IMPLEMENT: isolatedsourcepulse compatibility helper.

- Identified pulse cosine via scalarfallback1A36C9F FCOS/FPREM1/FCOS.
  Filtered direct-store scan isolated13268FB writer of70B53D8; IAT1B4C9A8
  resolveswinmm.timeGetTime; unsignedDWORD converts via double correction
  table0/4294967296 thenfloat. REVERSE EVIDENCE:pinned import/instructions/
  constants. CHAIN COVERAGE:pulse math identity and sampledclock origin;
  notexactCRT numericalparity. ASSET PROVENANCE:unchanged. FILES MODIFIED:
  Giant renderverifier,Breche reverse,state. LAST STATIC CHECK:cosine/time
  pins and existing10pass/registration checks PASS. LAST BUILD RESULT:
  unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES:
  floatclock precision/CVTTSS2SI range/optimizedCRTpath remain explicit;
  directscan doesnotexclude indirectwrites. MISSING CHAIN PARTS:auxiliary
  nativeintegration/liveparity. BLOCKER:none fornative clockcomparison.
  NEXT EXACT ACTION: inspect5.2 WorldTime and periodic-light helpers for
  anexisting equivalent, preserving source signedremainder and sampling.
  NEXT FILE TO OPEN:native time/render utility source and WorldTime writers.
  NEXT FUNCTION TO IMPLEMENT:verified nativeauxiliary pulse adapter.

- Confirmed741->A5D5F6 registration pair throughA1DCE6. Decoded pulse helper
  full18F930D body: caller argument-4,zero phase; signed integer conversion
  andIDIV remainder before float scaling/sign and final remaps. It is not
  justified to replace with guessed sin(time)/float fmod. REVERSE EVIDENCE:
  anchoredregistration/helper and constant read. CHAIN COVERAGE: registration
  association and pulse arithmetic; exact math helper/time producer OPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: Giant renderer verifier,Breche
  reverse,state. LAST STATIC CHECK: registration/signedpulse/10passes PASS.
  LAST BUILD RESULT: unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES: map registration conditional, runtime dispatch not exercised;
  transcendental helper not named speculatively. MISSING CHAIN PARTS: auxiliary
  nativecompatibility and all live acceptance. BLOCKER:none forofflinehelpers.
  NEXT EXACT ACTION: identify1A29A90 scalar fallback1A36C9F and time70B53D8
  producer; reuse native behavior only with verified arithmetic mapping.
  NEXT FILE TO OPEN:pinned1A36C9F function and70B53D8 xrefs.
  NEXT FUNCTION TO IMPLEMENT:evidence-backed auxiliary pulse contract.

- Traced84EA/84EB to complete rendererA5D5F6..A5DB14:ten mesh-wrapper
  calls, including explicit texture redraws onmesh2/3 flags42; NOT particles.
  Finalmesh0/1 use7F1F flags44, exposing additional material dependency.
  REVERSE EVIDENCE: complete anchored function and new render-pass verifier.
  CHAIN COVERAGE: texture consumers/pass structure, not outerdispatch/parity.
  ASSET PROVENANCE: unchanged; no speculative staging. FILES MODIFIED:new
  renderer verifier,Breche reverse,state. LAST STATIC CHECK:10 callsite/texture
  argument pins PASS. LAST BUILD RESULT:unchanged90416CB2/C85CEF94.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:pulse helper/dispatch/modelRGB
  restoration stillopen. MISSING CHAIN PARTS:auxiliarycompatibility/liveQA.
  BLOCKER:none forofflinecallers. NEXT EXACT ACTION: confirm renderer dispatch
  binding to741 and pulse18F930D semantics, then decide which shared native
  renderer path can implement proven behavior without global changes.
  NEXT FILE TO OPEN:A5D5F6 xrefs/registration and18F930D entry.
  NEXT FUNCTION TO IMPLEMENT:proven auxiliary render adapter contract.

- Revalidated pinned Giant BMD parses; registration18D5018..18D5134 adds
  four extra textures84E8..84EB beyond mesh texture names. Pinned logical
  strings and candidateOZJ SHA values, fully decodedJPEG payloads after24-byte
  headers:128x128 firstthree,32x32 jewel. REVERSE EVIDENCE: anchoredregistration,
  binary strings and physical filedecodes. CHAIN COVERAGE: extra registration,
  not renderer usage/runtime binding. ASSET PROVENANCE: inspector records hashes;
  no source edits/staging. FILES MODIFIED: Giant inspector,Breche reverse,state.
  LAST STATIC CHECK: pinnedBMD and4texturedecode PASS. LAST BUILD RESULT:
  unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none. KNOWN DIFFERENCES: physical
  OZJ candidates are not livebindingproof. MISSING CHAIN PARTS: auxiliaryrender
  consumers and nativeconfiguration/visualQA. BLOCKER:none foroffline xrefs.
  NEXT EXACT ACTION: trace84EA/84EB consumers in Giant GrowLancer renderer,
  distinguish render-pass substitutions from particles. NEXT FILE TO OPEN:
  pinned module numericrefs84EA/84EB then anchored consumer functions.
  NEXT FUNCTION TO IMPLEMENT: proven auxiliary render-pass contract.

- Added read-only Giant attachment inspector, parsed three v0F BMDs fully
  through mesh/skeleton/tail and recorded source SHA pins. Unique sit_pos_0
  is97 in DarkWizard/Elf,102 in GrowLancer (parent20); bones112/112/116,
  actions8 each. REVERSE EVIDENCE: actual source BMD records, not names alone.
  CHAIN COVERAGE: configuration seat bone existence, not live transform.
  ASSET PROVENANCE: hashes in inspector/Breche reverse; textures read from mesh.
  FILES MODIFIED: new inspector,Breche children reverse,state. LAST STATIC CHECK:
  three structural parses/unique seat checks PASS; hash pins added afterward.
  LAST BUILD RESULT: unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES: seat indices differ; no hardcoded universal index allowed.
  MISSING CHAIN PARTS: texture resolution/native configuration/visual QA.
  BLOCKER:none for texture provenance. NEXT EXACT ACTION: rerun pinned inspector,
  resolve emitted texture names through actual S21 registration/loading rules,
  hash resolved files and distinguish common textures from model-specific ones.
  NEXT FILE TO OPEN: model registration18D5018..18D50C2 and mesh texture paths.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed auxiliary texture manifest.

- Resolved category2 descriptor model setup through virtual38: callbacks
  1098DEE/1099B2F/109A909 select73F/740/741, store resolved model at record20,
  reject null, assign auxiliary Type and allocate its bone matrices. Registry
  names Giant_DarkWizard_01/Giant_Elf_01/Giant_GrowLancer_01. Shared1098102
  initializes Scale=float32(.85). REVERSE EVIDENCE: complete setup bodies,
  vtable slots and registry. CHAIN COVERAGE: specific configuration model
  producer, not generic Muun or skill-root identity. ASSET PROVENANCE:no new
  staging; names alone insufficient. FILES MODIFIED: action verifier,Breche
  children reverse,state. LAST STATIC CHECK: expanded vtable/model/scale pins
  PASS. LAST BUILD RESULT: unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES: category2 must not be generalized to other descriptors.
  MISSING CHAIN PARTS: configuration assets/parse/native state and live parity.
  BLOCKER:none for read-only asset inspection. NEXT EXACT ACTION: locate the
  three registered Giant BMDs, hash and parse bone names to verify sit_pos_0
  actually exists; keep outside staged skill assets until dependency warranted.
  NEXT FILE TO OPEN: original Data/Skill/Giant_GrowLancer_01.bmd (read-only).
  NEXT FUNCTION TO IMPLEMENT: evidence-backed auxiliary asset/bone contract.

- Closed auxiliary bone lookup/transform arguments:13164A0 returns matrix
  pointer, preferring object cached matrices for caller boolean1, with global
  fallback. Missing lookup returns null;132F433 leaves output unchanged on null.
  Caller1097AFE passes Translate=false then scales record24 explicitly. Native
  BMD TransformPosition(false) matches transform path but lacks null checking.
  REVERSE EVIDENCE: anchored lookup/transform/caller and native ZzzBMD.cpp406.
  CHAIN COVERAGE: matrix provenance/coordinate and invalid-lookup behavior.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: action verifier, Breche children
  reverse,state. LAST STATIC CHECK:13 new instruction pins added to verifier.
  LAST BUILD RESULT: unchanged90416CB2/C85CEF94. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES: null does not imply zero offset; no unsafe native call or
  fabricated global offset implemented. MISSING CHAIN PARTS: auxiliary model
  identity/availability and state activation, full live parity. BLOCKER:none
  for offline configuration model tracing. NEXT EXACT ACTION: trace record20
  model initialization and descriptor-selected auxiliary model resource before
  choosing native bone lookup/staging. NEXT FILE TO OPEN:1089CD3 initializer
  virtual setup callbacks and descriptor factory. NEXT FUNCTION TO IMPLEMENT:
  evidence-backed auxiliary model and missing-matrix compatibility contract.

- Closed133E8A7 destination provenance: prologue133607C stores model lookup
  arg1C at local-b7b8; preceding gates admit only5C0/5C1 and character byte4==0.
  Registry names are Robotwing/Robotwing_gold; this is model action0/1, not
  player191/317 and not a new Obsidian child. REVERSE EVIDENCE: continuous
  decode from prologue, registry rows, first registration arguments and11 pins.
  CHAIN COVERAGE: shared suppression consumer distinguished from skill child.
  ASSET PROVENANCE: no extra asset staged based on shared xref alone.
  FILES MODIFIED: action verifier, Obsidian reverse,state. LAST STATIC CHECK:
  expanded action verifier PASS. LAST BUILD RESULT: unchanged90416CB2/C85CEF94.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: outer scheduling and byte4 meaning
  not inferred. MISSING CHAIN PARTS: native auxiliary pose/action317/live parity.
  BLOCKER: none for offline native adapter investigation. NEXT EXACT ACTION:
  trace primary auxiliary pose model and sit-position transform from existing
  Breche cached-owner evidence; do not spend further work on unrelated wing
  model behavior. NEXT FILE TO OPEN: BRECHE_CHILDREN_REVERSE.md cached-owner
  producer and108A1BE auxiliary update. NEXT FUNCTION TO IMPLEMENT: proven
  native auxiliary attachment contract shared by Breche/Obsidian.

- Traced both direct suppression-expiry callers.1424C4E requires successful
  model lookup and object.TypeA94 inside1424B34;13F3EF8 calls it only for a
  live character object.133E8A7 bounded block requires character byte38==1
  and registered-entry predicate, then writes destination wordA8 according
  to returned flag. REVERSE EVIDENCE: anchored caller blocks and18 new pins.
  CHAIN COVERAGE: conditional call sites, not unconditional per-frame cadence.
  ASSET PROVENANCE: unchanged pinned module. FILES MODIFIED: action verifier,
  OBSIDIAN_REVERSE.md,state. LAST STATIC CHECK: expanded action verifier PASS.
  LAST BUILD RESULT: unchanged client90416CB2/serverC85CEF94. LAST RUNTIME QA:
  none. KNOWN DIFFERENCES: second caller destination/enclosing gates unverified;
  not equated with191/317 setter. MISSING CHAIN PARTS: native auxiliary pose,
  exact scheduling and live parity. BLOCKER: none for offline caller tracing.
  NEXT EXACT ACTION: recover enclosing133E8A7 function boundary and destination
  local-b7b8 provenance, separating helper model action from player action.
  NEXT FILE TO OPEN: pinned module preceding133E87B and relevant model lookup.
  NEXT FUNCTION TO IMPLEMENT: evidence-backed auxiliary pose selection contract.

- Closed Obsidian suppression time-source import identity: pinned PE import
  table maps IAT1B4C4A0 uniquely to kernel32.dll!GetTickCount. Re-anchored
  10C34A5 confirms DWORD saved+3000 and unsigned strict comparison, not elapsed
  subtraction. Added six boundary/wrap cases including source early-expiry
  behavior when addition wraps. REVERSE EVIDENCE: pinned module import directory
  plus instruction block. CHAIN COVERAGE: alternate-action suppression clock,
  not complete auxiliary pose compatibility. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: action verifier, OBSIDIAN_REVERSE.md,state. LAST STATIC CHECK:
  action/clip/category pins and six deadline cases PASS. LAST BUILD RESULT:
  unchanged client90416CB2/serverC85CEF94; no runtime code changes this run.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: source comparison is not wrap-safe;
  no guessed native timer policy or generic Muun selector added. MISSING CHAIN
  PARTS: native auxiliary state/pose, action317 integration and live parity.
  BLOCKER: none for offline producer/caller xrefs. NEXT EXACT ACTION: find callers
  of10C34A5 to establish suppression-update cadence and whether it executes
  before selection each frame; do not infer scheduling from helper body alone.
  NEXT FILE TO OPEN: pinned module xrefs to10C34A5 and bounded caller blocks.
  NEXT FUNCTION TO IMPLEMENT: proven auxiliary update cadence contract.

- Fixed GL cooldown mutation before rejection: CheckSkillDelay wrote
  SkillDelay[index] before receiver class checks. It now rejects unsupported
  base/internal IDs before actor access, RF continuation bypass or timestamp.
  REVERSE EVIDENCE: SkillManager CheckSkillDelay and three receive paths;
  source ordering verifier now checks all four early guards (PASS).
  CHAIN COVERAGE: rejection cooldown boundary, not completed casting logic.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: SkillManager.cpp, guard verifier,
  class contract, state and matrix. LAST STATIC CHECK: four source-order guards
  PASS; diff check has only existing CRLF warning. LAST BUILD RESULT: Ex603
  Win32 isolated build/link exit0, SHA256
  C85CEF9419D6602D6E5921CF0306B287DD41CC8CC14E7318F6BFF709A32CF1C2.
  LAST RUNTIME QA: none, no deployment. KNOWN DIFFERENCES: master mapping is
  one-hop RelatedSkill in native CSkill::Set, not recursive S21 Brand resolution;
  guards do not claim coverage for every S21 master alias. MISSING CHAIN PARTS:
  GS hardcoded skill handlers, class defaults/activation and live parity.
  BLOCKER: S21 server caller behavior unavailable in current table-only evidence;
  remaining offline asset/animation gates can still advance. NEXT EXACT ACTION:
  revisit Obsidian alternate action317 native auxiliary-pose requirements and
  available player action staging, without equating it to generic Muun presence.
  NEXT FILE TO OPEN: GrowLancer/OBSIDIAN_REVERSE.md alternate action contract.
  NEXT FUNCTION TO IMPLEMENT: evidence-proven native auxiliary-pose adapter,
  not guessed server/master dispatch.

- Fixed late server rejection side effects: GL RunningSkill cases previously
  returned false only AFTER shield reset/invisibility removal; duration wrapper
  could broadcast before that rejection. Added early guards on both m_index and
  m_skill in UseAttackSkill, UseDurationSkillAttack and RunningSkill, before
  actor lookup/mutation, combo/Nova processing, duration broadcast or debit.
  Existing base/internal switch rejection remains defense in depth.
  REVERSE EVIDENCE: native SkillManager call order and existing shared fail-closed
  contract. CHAIN COVERAGE: rejected unimplemented dispatch side effects only;
  NOT completed authoritative skill implementation. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: SkillManager.cpp, server contract test, new source guard verifier,
  state and acceptance matrix. LAST STATIC CHECK: C++ contract W4/WX PASS including
  2048-ID gate range; three source-order guards PASS; diff check PASS.
  LAST BUILD RESULT: Ex603 Win32 build/link exit0, explicit isolated OutDir/IntDir,
  SHA25687F0B1C284124E871C954AE847AA87660232CA483ACC5CB8602E44FD5F113D4E.
  LAST RUNTIME QA: no server launched/deployed; live rejection QA OPEN.
  KNOWN DIFFERENCES: packet receiver cooldown checks precede wrapper; not claiming
  all packet side effects closed. Guards cover271..281, not unimplemented master
  namespace aliases. MISSING CHAIN PARTS: complete GS/class/packet/visual acceptance.
  BLOCKER: none for further offline call-order review. NEXT EXACT ACTION: inspect
  CheckSkillDelay and receiver ordering for rejected GL casts, and confirm master
  brand mapping before extending any guard. NEXT FILE TO OPEN: SkillManager.cpp
  CheckSkillDelay, CGMultiSkillAttackRecv and MasterSkillTree lookup.
  NEXT FUNCTION TO IMPLEMENT: rejection before unintended cooldown mutation if
  current source proves that path, without changing unrelated SS6 behavior.

- Verified weapon-bonus application boundary: pinned S21 same/different
  functions use65/55percent but do not inspect Class; exact return tuple
  is minLeft,minRight,maxLeft,maxRight, unlike argument order. Expanded
  profile verifier checks eight assignments and both return orders (PASS).
  Native ObjectManager restricts dual weapons to DK/MG/DL/RF and groups0..3;
  nonRF55percent versusRF min60/max65 after item/master/custom modifiers.
  REVERSE EVIDENCE: CalcCharacter.lua1022 onward and ObjectManager2970..3423.
  CHAIN COVERAGE: class/weapon arithmetic application and BP dispatch boundary.
  ASSET PROVENANCE: source hash unchanged. FILES MODIFIED: profile verifier,
  class contract,state. LAST STATIC CHECK: profiles PASS12/14 plus two bonus
  bodies/eight assignments; diff check PASS. LAST BUILD RESULT: unchanged90416CB2.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: DK QA can inherit DK weapon scaling;
  not GL damage parity. CharacterCalcBP has no eighth class/default; matching
  DK/RF coefficients do not prove S21 GL bridge conversion. MISSING CHAIN PARTS:
  source caller eligibility/rounding, class defaults, authoritative live QA.
  BLOCKER: none for remaining offline integration review. NEXT EXACT ACTION:
  rerun and inspect server contract verifier coverage, trace which learning,
  cast and packet paths remain fail-closed versus implemented.
  NEXT FILE TO OPEN: tools/grow_lancer/verify_server_contracts.py and GS catalog
  callers. NEXT FUNCTION TO IMPLEMENT: source-proven server activation path,
  only after unresolved identity/default/authority gates are satisfied.

- Expanded pinned class source checks to 12 branches / 14 exact assignments
  plus four physical damage assignments. Added PvP attack/defense, elemental
  damage/rates/defense and AG; verified TotalLevel definitions where used.
  REVERSE EVIDENCE: unchanged hash-pinned CalcCharacter.lua; exact expressions
  recorded in CLASS_INTEGRATION_CONTRACT.md. CHAIN COVERAGE: source stat
  formulas only, not GS bridge execution or skill-specific cost/recovery.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: profile verifier, class contract,
  state. LAST STATIC CHECK: profile PASS12/14; native encoding PASS24;
  project inclusion PASS3cpp/24headers; stage PASS2658base/79overlay/3rootData,
  Engine/player hashes and player CRC. LAST BUILD RESULT: unchanged90416CB2;
  no runtime source changes this run. LAST RUNTIME QA: none, owner asleep.
  KNOWN DIFFERENCES: integration checks are NOT Git merge or visual acceptance.
  MISSING CHAIN PARTS: class defaults/GS bridge/activation/server authority/live QA.
  BLOCKER: DB defaults require external evidence; offline source work remains.
  NEXT EXACT ACTION: inspect remaining CalcTwoSameWeaponBonus and downstream
  class formula application; distinguish class-specific exclusions from defaults.
  NEXT FILE TO OPEN: CalcCharacter.lua from line1022 and native stat consumers.
  NEXT FUNCTION TO IMPLEMENT: evidence-gated class numeric/application contract.

- Identified nativeformula precision hazard:int stats/intconst division
  before__int64 damageassignment truncates perterm;S21 sourceexpressions
  requireseparatebridge roundingproof. CounterexampleSTR7/DEX9 rawmin1.775,
  pertermint0,after-sumtruncate1. No guessedconversion implemented.
  Verified4 GrowLancerbranches/5expressions:attack/magicspeedDEX20,
  PvMhitTotalLevel*5+DEX*1.25+STR/4,PvMdefenserateDEX4,defenseDEX7.
  REVERSE EVIDENCE:pinnedCalcCharacter.lua andnativeObjectManager intlocals,
  ServerInfo intdivisors,User.h __int64damage. CHAIN COVERAGE:sourceformulas
  versusnative arithmeticboundary. ASSET PROVENANCE:unchangedpinnedscript.
  FILES MODIFIED:profileverifier,classcontract,state. LAST STATIC CHECK:
  sourcebranches/rationalcounterexample checks added;notLuaexecution.
  LAST BUILD RESULT:unchanged90416CB2. LAST RUNTIME QA:none. KNOWN DIFFERENCES:
  GSbridge rounding/defaults remainunknown;lua/luajit/lupa notavailable.
  MISSING CHAIN PARTS:class/defaults/GSauthority/visualQA. BLOCKER:defaults
  requireDBexport,butremainingofflineformulas canadvance. NEXT EXACT ACTION:
  runexpandedprofileverifier,thenextract remainingPvP/AG/elemental classbranches
  withexplicitprecisionandactivationgates. NEXT FILE TO OPEN:CalcCharacter.lua
  lines669..1020. NEXT FUNCTION TO IMPLEMENT:verified completeclassformula
  contract,notguessedrounding ordefaultstats.

- Verified supplied classprofiles:LuaGrowLancer7 versusStatSpecializeID8;
  distinctnamespaces. CalcCharacter.ini namesDefaultClassType DB as source
  ofHP/MP multipliers;values notavailableininspectedfiles,notguessed. Native
  DefaultClassInfo loader needsattributes/HPMP/multipliers. Threeheaders
  independentlydefineMAX_CLASS7; item/quest/master/startitems andServerInfo
  recovery/PvP/elementalmatrices need coordinatedclassdata.
  REVERSE EVIDENCE:hashpinnedLua/INI/XML andnativeheaders/loaders inclasscontract.
  CHAIN COVERAGE:classdata origins/namespaces/affectedconsumers. ASSET PROVENANCE:
  three sourcehashes documented. FILES MODIFIED:profileverifier,classcontract,state.
  LAST STATIC CHECK:profilehash/namespaces/4damageexpressionsPASS (notLuaexecution).
  LAST BUILD RESULT:unchanged90416CB2. LAST RUNTIME QA:none. KNOWN DIFFERENCES:
  actualDefaultClassType/WZ_CreateCharacter export neededforDBdefaultsproof;
  noDBaccess performed. MISSING CHAIN PARTS:classdefaults/activation/GS/live.
  BLOCKER:DBdefaults unavailableforclassactivation,butofflineformula/consumer
  work remains. NEXT EXACT ACTION:traceclassformula dispatch andnative stat
  application/rounding;separateverifiedexpressionsfrommissingdefaultrows.
  NEXT FILE TO OPEN:S21 CalcCharacter.lua classbranches andnativeObjectManager
  character statcalculation. NEXT FUNCTION TO IMPLEMENT:verifiedclassformula
  contract withoutinventingstartingstats or enablingincompleteclass.

- Closed native classload/create path:GS explicit classicwhitelists reject
  newclass before initialization;ObjectManager storesDBClass anddivides/mod16
  forbase/evolution. DataServer creation delegatesWZ_CreateCharacter, updates
  AccountCharacter slot onlyonsuccess;proceduredefinitionnotinworktreeSQL.
  REVERSE EVIDENCE:ProtocolCGCharacterCreateRecv,ObjectManager3549..3551,
  DataServerProtocol1149. Added source-expression checked24case nativeencoding
  verifierPASS;unevolvedbase7 mathematicallyDB112/wire224 undernativeformula,
  NOTproofS21DB/evolutioneligibility. CHAIN COVERAGE:nativeencoding boundaries.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:classverifier,contract,state.
  LAST STATIC CHECK:24algebra/sourceexpressioncasesPASS. LAST BUILD RESULT:
  unchanged90416CB2. LAST RUNTIME QA:none;noDBaccess/modification.
  KNOWN DIFFERENCES:actualprocedure/defaults/unlock/evolutionclass pathsopen.
  MISSING CHAIN PARTS:nativeclass/serverauthority/visualgates. BLOCKER:none
  forofflineclassdefaults/consumerwork;DBvalidationrequiresisolatedfixture.
  NEXT EXACT ACTION:locate suppliedGrowLancerdefaultstats andenumerate indexed
  MAX_CLASS consumers;designcomplete classdata without guessing defaults.
  NEXT FILE TO OPEN:DefaultClassInfo.cpp andS21 CalcCharacter.lua/defaultdata.
  NEXT FUNCTION TO IMPLEMENT:verifiedGrowLancerclassdata contract.

- Aligned Clash descriptor with proven rear initialpose/storedoffset,
  controller(0,6.5] bounds,31/28/11 tickvisits andfront alpha multiplier.7
  versusattainablepeak.65. JSON parsePASS. Began native class contract:
  supplied S21 CalcCharacter.lua explicitlyGrowLancer7; native basebitfield
  fits7 but GetCharacterClass lacks7 anddefaults0,GS arraysMAX_CLASS7.
  REVERSE EVIDENCE:sourcepaths/hash inCLASS_INTEGRATION_CONTRACT.md;
  DSProtocol593/878/1848 andViewport1034 showseparateDB/networkencoding.
  CHAIN COVERAGE:descriptor fidelity andbaseclass integration boundary.
  ASSET PROVENANCE:read-onlyS21scriptSHA recorded. FILES MODIFIED:descriptor,
  newclasscontract,state. LAST STATIC CHECK:descriptorJSONPASS;previousstage
  complete2658base79overlay3rootData/hash/CRC verifiedPASS.
  LAST BUILD RESULT:unchangedlinked/staged90416CB2. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:base7scriptidentity notproofDB/evolutionencoding;
  classactivation stillfailclosed. MISSING CHAIN PARTS:class/GS/live gates.
  BLOCKER:noneoffline. NEXT EXACT ACTION:traceDSload/createDBClass assignment,
  base/evolution conversion andMAX_CLASS consumers beforeclassimplementation.
  NEXT FILE TO OPEN:ExGameServer/GameServer/DSProtocol.cpp load/createpaths.
  NEXT FUNCTION TO IMPLEMENT:evidence-backed native class conversion contract.

- Selected Clash renderer verified mesh0/flags42/blend0/Alpha/storedUV,
  front-only U-.05/V-.01. New pinned renderer verifierPASS. Invisible
  controller cannot use visible-only EnsureModel;source cache presence is
  not geometry readiness,compatibility remains OPEN. REVERSE EVIDENCE:
  A4A102..A4A355 plus native resource table/EnsureModel. CHAIN COVERAGE:
  selected renderer and explicit cache boundary. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:renderer verifier,stage verifier,reverse,acceptance,state.
  LAST STATIC CHECK:rendererPASS. LAST BUILD RESULT:isolatedWin32linkPASS,
  cryptlib missingPDB warnings;staged90416CB2616CC265D650BBFDD1E62E596D0F785B21CAB90A059F31A617DFA18C.
  Backupbefore-clash-f84689a2bd24416f9e20dbd9c78c6829 retains6BB0C35B.
  LAST RUNTIME QA:none;no game launch. KNOWN DIFFERENCES:cache lifecycle/
  normalization API regression/real pools unverified. MISSING CHAIN PARTS:
  Obsidian317,class/GS/runtime gates unchanged. BLOCKER:none offline.
  NEXT EXACT ACTION:verify staged tree result then audit remaining descriptor
  mismatches forClash creation offsets/timing and client/server class boundary.
  NEXT FILE TO OPEN:grow_lancer_skills_descriptor.json Clash entry.
  NEXT FUNCTION TO IMPLEMENT:only evidence-backed descriptor/runtime gap.

- Shared Clash pose helpers now used by runtime and6 tests:vertical/sloped/
  zero direction,initialrear versusstoredtick,owner movement. Missing target
  controller now skips instead of expires;owner null/Live/action checks kept.
  ControllerTimer0/privatewhole31visits;shared(0,6.5]/counter0 predicate;
  fourcontroller cadence tests skip2 attempts thenemit once. REVERSE EVIDENCE:
  prior157DA7B..DA84->DF45,157DA18..DA58,157DF45 common tail.
  CHAIN COVERAGE:12tick+6pose cases/frame boundaries;not actual model/pool.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:Clashhelper/test,runtime,ZzzEffect,
  reverse,state. LAST STATIC CHECK:x86W4WX18cases+boundaries PASS.
  LAST BUILD RESULT:isolatedWin32ClCompile PASS;NOTlinked/staged,6BB0C35B
  remains test candidate. LAST RUNTIME QA:none. KNOWN DIFFERENCES:native
  model-presence gates andselected renderer stillopen;normalization API not
  covered by pose test. MISSING CHAIN PARTS:Obsidian317,class/GS/live gates.
  BLOCKER:none. NEXT EXACT ACTION:audit model registration/cache readiness
  checks forClashcontroller andrendererA4A102/A4A24A beforelink/stage.
  NEXT FILE TO OPEN:GrowLancerResources.cpp andClashRenderEffect branch.
  NEXT FUNCTION TO IMPLEMENT:onlyproven Clash cache/render discrepancy.

- Full Clash controller decode exposed positioning mismatches: normalized
  direction usesXYZ (old port flattenedZ); tiny length<1e-6 zeros vector
  without abort (old threshold<=.001 aborted). Rear provisional-50 position
  is overwritten by caster+unit beforeCreateEffect, while constructor only
  stores-50 direction; subsequent tick repositions. Corrected native XYZ,
  threshold,retained constructor pose and rear initial caster+unit point.
  REVERSE EVIDENCE:D308C3..D309B8,157DD8B..DE2F,DED0,14822E2..2519,
  front14820BE..20EF. CHAIN COVERAGE:selected position inputs/update boundary.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:runtime,Clashreverse,state.
  LAST STATIC CHECK:anchored branch decode/native math inspection.
  LAST BUILD RESULT:isolatedWin32ClCompile PASS;NOTlinked/staged,6BB0C35B
  remains test candidate. LAST RUNTIME QA:none. KNOWN DIFFERENCES:missing
  target source skips, native currentlyexpires controller;model gates and
  whole controller clock still pending. MISSING CHAIN PARTS:class/GS/live.
  BLOCKER:none. NEXT EXACT ACTION:add shared direction/initial-position
  regressions (vertical,targetcoincident,firsttick),then reconcile missing
  target skip and controller wholeclock. NEXT FILE TO OPEN:Clash tick helper
  and test. NEXT FUNCTION TO IMPLEMENT:tested Clash direction/pose contract.

- Extracted shared Clash front/rear scalar helper used by native runtime
  and new focused test.8 W4WX x86 cases PASS across fourFPS:28/11 visits,
  one cleanup,alpha0 endpoints,peaks.65/.9,scale.89/1.30,frontY+280.
  REVERSE EVIDENCE:existing157E060..E168/E26C..E327 scalar bodies;controller
  157D9AE..D9F9 additionally gates owner/controller model cache before emit.
  CHAIN COVERAGE:child scalars/cadence,not owners/pools/GPU. ASSET PROVENANCE:
  unchanged. FILES MODIFIED:Clashhelper/test,runtime,project/filters,reverse,state.
  LAST STATIC CHECK:8cases/project3cpp24headers PASS. LAST BUILD RESULT:
  isolatedWin32ClCompile PASS;NOTlinked/staged,6BB0C35B remains test candidate.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:controller model gates/tick mapping
  still audit pending. MISSING CHAIN PARTS:Obsidian317,class/GS/live gates.
  BLOCKER:none offline. NEXT EXACT ACTION:finish anchored Clash controller
  157DA5E..157DF45 target lookup/emission and missing-model handling, then
  map whole ticks and regression without erasing owner/target safeguards.
  NEXT FILE TO OPEN:pinned dump157DA5E. NEXT FUNCTION TO IMPLEMENT:
  verified Clash controller tick and model-presence contract.

- Native auxiliary boundary inspected: two MuunHelper slots feed CreateMount;
  petType8 follows via RiseMuunFollow. No proven equivalent to S21 keyed
  record/suppression/sit_pos_0 in these paths;317 remains explicitly OPEN,
  not aliased to SafeZone or equipped Muun. Continued separate Clash audit.
  Corrected Clash subtype0 front/rear to private whole ticks and Timer0 ctor.
  REVERSE EVIDENCE:front157E168,rear157E327->15945F1 before common decrement.
  CHAIN COVERAGE:selected child cadence,expected28/11 visits. ASSET PROVENANCE:
  unchanged. FILES MODIFIED:runtime,ZzzEffect,Obsidian/Clashreverse,state.
  LAST STATIC CHECK:project3cpp23headers/diffcheck PASS. LAST BUILD RESULT:
  isolatedWin32ClCompile PASS;NOTlinked/staged,6BB0C35B still test candidate.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:Clash controller clock/scalar/pool
  tests open;Obsidian auxiliary native adapter still needed. MISSING CHAIN
  PARTS:class/GS/live gates. BLOCKER:none for other offline skill work.
  NEXT EXACT ACTION:extract Clash child scalar helper and verify28/11 updates
  across fourFPS;then audit controller157D958 before whole-tick migration.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime Clash child scalar branches.
  NEXT FUNCTION TO IMPLEMENT:shared tested Clash child tick helper.

- Closed local/received Obsidian metadata key provenance. Local context+0C
  both selects273/693 and is passed to action helper; receive1950 selects
  273->selector23->12CB59E and is passed unchanged. Base273 metadata1 does
  not newly set suppression with false; prior suppression remains relevant.
  REVERSE EVIDENCE:10E34E8/3567/3571/360D/3617/46E5,12C5540/55EF/55F6,
  tables12CE8CA/12CE85A,12CB5B6. CHAIN COVERAGE:base local/receive key closure.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:descriptor alternate contract,
  corrected integer-half alpha descriptor,action verifier,reverse,state.
  LAST STATIC CHECK:local pins and descriptor JSON PASS; received pins added.
  LAST BUILD RESULT:unchanged6BB0C35B;no native edits. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:317 native auxiliary-pose state adapter still OPEN;
  master693 not equated to base273. MISSING CHAIN PARTS:class/GS/live gates.
  BLOCKER:none for offline work. NEXT EXACT ACTION:check native auxiliary
  state/action capabilities against established source contract, then decide
  minimum private adapter needed without changing global Muun behavior.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime CreateObsidianRoots and native
  character/Muun pose paths. NEXT FUNCTION TO IMPLEMENT:private action adapter.

- Recovered runtime metadata lookup (inline node+14, NOT raw skill.bmd row).
  Pinned full dump all9 keys exist:byte0=0,dword4=key;dword90=1 for272/273/278,
  0 forother6. 140DC15 accepts6/15 only;140DC35 with category1 and false does
  not set suppression, nor clear existing suppression. Still verify call-key
  provenance before applying this to Obsidian dispatch. REVERSE EVIDENCE:
  963291/95ED56/960B61,140DC15,full dump key273 metadata164EE7D4.
  CHAIN COVERAGE:offline runtime rows and conditional metadata behavior.
  ASSET PROVENANCE:pinned full dump read-only. FILES MODIFIED:new snapshot
  inspector,Obsidianreverse,state. LAST STATIC CHECK:offline9row read PASS;
  inspector now asserts exact values. LAST BUILD RESULT:unchanged6BB0C35B.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:local saved+0C key source unclosed.
  MISSING CHAIN PARTS:class/GS/live gates unchanged. BLOCKER:none offline.
  NEXT EXACT ACTION:trace caller context [ebp-F560]+0C initialization and
  received Obsidian action arguments; establish exact metadata key273.
  NEXT FILE TO OPEN:pinned local dispatcher context and12CB59E receive branch.
  NEXT FUNCTION TO IMPLEMENT:verified alternate animation state contract.

- Recovered registered flag producer and cross-linked existing Breche manager
  evidence:record34=characterObject.Live & !character31E8 at108A408.
  31E8 is temporarily set by140DC35 before action choice, savedcounter31EC;
  10C34A5 clears only unsigned(saved+3000)<now, not equality. Do not name it
  safe-zone/equipped-Muun or transplant offsets. REVERSE EVIDENCE:1089CD3,
  complete108A1BE..108A42F,96139E,10C34A5..34EA,140DC35..DCC4.
  CHAIN COVERAGE:registered flag producer and expiry; skill-specific trigger
  metadata remains OPEN. ASSET PROVENANCE:unchanged. FILES MODIFIED:action
  verifier,Obsidianreverse,state. LAST STATIC CHECK:expanded pinned verifier
  PASS. LAST BUILD RESULT:unchanged6BB0C35B;no native edits. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:skill273 metadata suppression condition/native contract
  unresolved. MISSING CHAIN PARTS:class/GS/live gates remain. BLOCKER:none.
  NEXT EXACT ACTION:decode140DC15 and skill metadata lookup963291 used by
  140DC35; correlate273 row to determine pre-selection suppression behavior.
  NEXT FILE TO OPEN:pinned dump140DC15 and existing skill-table decoder.
  NEXT FUNCTION TO IMPLEMENT:verified alternate-action contract only.

- Identified alternate-action category64 as16 exact item rows: groups13/16
  indices231,232,326..331, Tibetton/Repen/Pawn/Grippus and evolved variants.
  Supplied GS ItemList KindB64 and MuunInfo independently identify Muun rows;
  no inference that any equipped Muun enables317. REVERSE EVIDENCE:loader
  D052FA/D05301/D05367/D0537B and catalog D06184/D064E6, pinned item table.
  CHAIN COVERAGE:catalog record identity, not registered-state transition.
  ASSET PROVENANCE:item/GS XML hashes documented in Obsidian reverse;read-only.
  FILES MODIFIED:action verifier,Obsidian reverse,state. LAST STATIC CHECK:
  binary/clip/item checksum and16identity verifier PASS. LAST BUILD RESULT:
  unchanged6BB0C35B; no native code edit. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:byte34/31E8 semantics and native equivalent still OPEN.
  MISSING CHAIN PARTS:class/GS/live gates unchanged. BLOCKER:none offline.
  NEXT EXACT ACTION:follow manager factory10C464B through common1089C09 and
  initialization1089CD3 to recover registered byte34 writes and source state.
  NEXT FILE TO OPEN:pinned dump1089C09/1089CD3. NEXT FUNCTION TO IMPLEMENT:
  evidence-backed alternate state only; no global Muun behavior changes.

- Closed alternate-action accessor/clip ambiguity:9C54AA is no-argument
  singleton accessor, caster remains on stack for10C352E ret4. Predicate
  uses registered record byte34 or catalog category64 for caster word322E;
  prerequisite also checks three IDs3238/323C/3240. Semantic names still OPEN.
  REVERSE EVIDENCE:9C54AA..9C5536,10C352E..10C35D9,10C34ED/10C2F89,
  141BA4B,1314FAE,D064B4. Original Player v0F60bones410actions;191/317
  both10keys but distinct hashes. CHAIN COVERAGE:conditional branch and clips.
  ASSET PROVENANCE:original read-only; new clip hashes in Obsidian reverse.
  FILES MODIFIED:action verifier,reverse,state. LAST STATIC CHECK:action pins
  and original clip verifier PASS. LAST BUILD RESULT:unchanged6BB0C35B;
  no runtime edit. LAST RUNTIME QA:none. KNOWN DIFFERENCES:category64 meaning
  and native equivalent unknown;317 not appended/enabled speculatively.
  MISSING CHAIN PARTS:class/GS/live gates unchanged. BLOCKER:none offline.
  NEXT EXACT ACTION:identify catalog field251/category64 and manager record
  constructors from registration/data evidence; establish state semantics.
  NEXT FILE TO OPEN:S21 item catalog decoder and manager10C2FD4 constructor
  path. NEXT FUNCTION TO IMPLEMENT:only evidence-backed alternate-state adapter.

- Rechecked requested integration completeness: project3cpp/23headers each
  exactly once, no exclusion; stage2658base/79overlay/3rootData hash/CRC PASS.
  This is project/stage inclusion, NOT a Git merge: dirty/untracked files
  remain on feature/grow-lancer-skills-s21, protected source untouched.
  Updated acceptance matrix and Obsidian reverse to staged6BB0C35B.
  REVERSE EVIDENCE:10E46D5..10E46F0 and complete1327DE8..1327E56 show
  action317 is conditional on predicate10C352E, byte+31E8 zero and arg!=-1;
  otherwise191. CHAIN COVERAGE:alternate-action branch, semantics still OPEN.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:state,acceptance,Obsidianreverse.
  LAST STATIC CHECK:project/stage/Obsidianrenderer/diffcheck PASS.
  LAST BUILD RESULT:unchanged linked/staged6BB0C35B. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:predicate/skeleton317 unverified, no speculative mapping.
  MISSING CHAIN PARTS:class/GS/runtime gates unchanged. BLOCKER:none for offline
  reverse. NEXT EXACT ACTION:decode10C352E and9C54AA to identify alternate
  action condition, then inspect actual clip317. NEXT FILE TO OPEN:pinned dump.
  NEXT FUNCTION TO IMPLEMENT:only verified alternate-action adapter.

- Fixed Obsidian renderer blend indices from provisional-2 to rotating0,
  aura0/1 matching mesh indices. REVERSE EVIDENCE:A48C5F/DA1/EC6,
  A48C79/DBB/EE0 RenderMesh calls. CHAIN COVERAGE:selected blend arguments.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:runtime,renderer verifier,
  stageverifier,state. LAST STATIC CHECK:rendererPASS,project3cpp23headers
  PASS,diffcheckPASS. LAST BUILD RESULT:isolatedWin32linkPASS,PDBwarnings.
  Linked/stagedSHA6BB0C35B23D5F92118960FE2D55B31409338BFADCCAA9C3632633560A6BB5FE3.
  Previous323C7F8F recoverable before-obsidian GUID backup retained.
  LAST RUNTIME QA:none;stage tree/hash/CRC verifierPASS. KNOWN DIFFERENCES:
  secondaction317,realpool/GPU stillunverified. MISSING CHAIN PARTS:class/GS
  andremaining skill/runtime gates. BLOCKER:none. NEXT EXACT ACTION:inspect
  native action317 S21 identity/skeleton and existing action mapping evidence;
  do not invent SS6 equivalent. NEXT FILE TO OPEN:Obsidian reverse/action map.
  NEXT FUNCTION TO IMPLEMENT:only evidence-backed second-action contract.

- Implemented missing Obsidian Thunder8:life15,callerScale,Rotation0 each
  tick,Scale+.03,RGB*.85;private whole-particle clock reset every allocation.
  Shared Obsidian model/particle helper tested across fourFPS12cases;
  16/21/15 visits verified. REVERSE EVIDENCE:1650BA3..1650BED,
  16C00CB..16C01CE->16C03D0->172343C. CHAIN COVERAGE:Thunder8 selected
  ctor/update and shared model scalars. ASSET PROVENANCE:unchanged7F2B.
  FILES MODIFIED:Obsidianhelper/test,runtime,particle,project/filters,
  alpha verifier,reverse,state. LAST STATIC CHECK:x86W4WX12casesPASS,
  pinnedalpha verifierPASS,project3cpp23headersPASS. LAST BUILD RESULT:
  isolatedWin32ClCompilePASS,notlinked/staged. LAST RUNTIME QA:none;
  staged323C7F8F unchanged. KNOWN DIFFERENCES:action317 notmapped;
  actual pool/GPU untested. MISSING CHAIN PARTS:Obsidian render arguments,
  class/GS/fullruntime. BLOCKER:none. NEXT EXACT ACTION:compare Obsidian
  mesh blend indices (doc0/1 versus native code),correct andlink/stage.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp Obsidian RenderEffect.
  NEXT FUNCTION TO IMPLEMENT:verified Obsidian renderer mismatch.

- Corrected Obsidian rotating alpha:integer half7,denominator8,not7.5;
  native private whole ticks now include life0 for both base models. Removed
  aura's invented alpha clamp. REVERSE EVIDENCE:1578145..15781C3 integer
  half;1578208..1578283 incremental alpha;both common15945F1 tails.
  CHAIN COVERAGE:selected model alpha/lifetime. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:runtime,ZzzEffect,alpha verifier,Obsidianreverse,state.
  LAST STATIC CHECK:pinned alpha/source verifier PASS. LAST BUILD RESULT:
  isolatedWin32ClCompile PASS,notlinked/staged. LAST RUNTIME QA:none;
  staged323C7F8F unchanged. KNOWN DIFFERENCES:second valid S21 action317
  notmapped; Thunder8 still has no audited native subtype implementation.
  MISSING CHAIN PARTS:Obsidian particle8/render/scalar runtime,class/GS/QA.
  BLOCKER:none for remaining offline work. NEXT EXACT ACTION:add shared
 16/21-visit scalar regression,then decode Thunder8 constructor/update.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp BITMAP_ENERGY subtype8.
  NEXT FUNCTION TO IMPLEMENT:verified Obsidian tick helper/Thunder8 adapter.

- Verified Wrath aura0/1 renderer matches selected S21 mesh/blend/UV/RGB
  arguments; corrected ambiguous prose (replace BodyLight,not multiply old
  lighting). Linked candidate323C7F8F staged with hash guards and recoverable
  before-wrath backup of7E619364;production untouched.
  REVERSE EVIDENCE:A49554..A497AD,A49690/A497A5 mesh0 calls.
  CHAIN COVERAGE:selected aura renderer;not whole visual acceptance.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:aura verifier,stage verifier,
  reverse,acceptance matrix,state. LAST STATIC CHECK:aura verifierPASS;
  stage2658base79overlay3rootData hash/CRC PASS.
  LAST BUILD RESULT:323C7F8FC757D7D99F4BC8BCC7D6D8CDEA3E549DDF5E5CDEA3DDDD818F659B9F linked/staged.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:real buff424/425,owner/pool and
  GPU acceptance remain open. MISSING CHAIN PARTS:remaining skill audit,
  native class/authoritativeGS/fullruntime. BLOCKER:none.
  NEXT EXACT ACTION:review Obsidian controller and child timing for same
  pre-decrement/fractional errors,starting exact dispatcher in reverse doc.
  NEXT FILE TO OPEN:GrowLancer/OBSIDIAN_REVERSE.md.
  NEXT FUNCTION TO IMPLEMENT:only verified Obsidian controller mismatch.

- Extracted Wrath aura scalar helper used by runtime/tests;24 total cases
  PASS,31 aura visits and one life1 eligibility;end alpha-.097/-.74 and
  scale/UV endpoints verified. Buff query/CreateEffect not simulated as proof.
  REVERSE EVIDENCE:existing157939E..15798E2 scalar contracts and life1.
  CHAIN COVERAGE:actual shared scalar implementation,not live recurrence.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:Wrathhelper/test,runtime,
  Wrathreverse,state. LAST STATIC CHECK:x86W4WX24cases,project3cpp22headers
  PASS. LAST BUILD RESULT:isolatedWin32 link PASS,cryptlib PDB warnings.
  CandidateSHA323C7F8FC757D7D99F4BC8BCC7D6D8CDEA3E549DDF5E5CDEA3DDDD818F659B9F.
  LAST RUNTIME QA:none;NOTstaged,7E619364 client remains. KNOWN DIFFERENCES:
  aura renderer and real buff recurrence/pool acceptance open.
  MISSING CHAIN PARTS:full native class/GS and nine-skill runtime gates.
  BLOCKER:none. NEXT EXACT ACTION:verify Wrath A49554 renderer Light/alpha
  mapping,then hash-guard stage323C7F8F with recoverable7E619364 backup.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp WrathAura renderer.
  NEXT FUNCTION TO IMPLEMENT:only proven aura render mismatch.

- Shared Wrath stage predicate and16-case regression added:frame3.5 takes
  two sequential stages in one visit;71 controller visits across four FPS.
  Aura0/1 Timer0/private whole ticks and exact life==1 continuation gate
  implemented, preserving buff424/425 condition and31 visits30..0.
  REVERSE EVIDENCE:157959D/1579844 equality gates;1579645/15798E2
  ->1579AAF->15945F1. CHAIN COVERAGE:stage boundaries,aura cadence.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:Wrathhelper/test,runtime,
  ZzzEffect,Wrathreverse,state. LAST STATIC CHECK:x86W4WX16cases PASS.
  LAST BUILD RESULT:isolatedWin32ClCompile PASS,notlinked/staged.
  LAST RUNTIME QA:none;7E619364 unchanged. KNOWN DIFFERENCES:aura scalar
  endpoints and buff-driven recurrence need tests;16cases do not cover them.
  MISSING CHAIN PARTS:aura scalar/renderer QA,class/GS/runtime gates.
  BLOCKER:none. NEXT EXACT ACTION:extract verified aura scalar helper/test
  with31 ticks and one continuation atlife1,thenlink/stage Wrath corrections.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp WrathAura scalar branch.
  NEXT FUNCTION TO IMPLEMENT:shared aura scalar/timing regression.

- Wrath controller Timer0/private whole ticks added, preserving life70..0;
  retained correct caster-owned5EB/5EC (unlike MagicPin controller-owned
  children). Ground call now passes controllerLight and null owner as source.
  Removed unreachable fractional Wrath particle updates,other subtype no-ops
  retained. REVERSE EVIDENCE:1480E08..E35 ctor,1578D1C/1578ED3 owners,
  15791F3 Light,1579222->15945F1 common tail.
  CHAIN COVERAGE:controller lifetime and selected child call arguments.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:runtime,particle,ZzzEffect,
  Wrathreverse,state. LAST STATIC CHECK:project3cpp22headers PASS.
  LAST BUILD RESULT:isolatedWin32ClCompile PASS,notlinked/staged.
  LAST RUNTIME QA:none;7E619364 stagedunchanged. KNOWN DIFFERENCES:
  frame3.5 can satisfy two sequential stage windows; keep source behavior,
  need regression for this boundary. CallLight fix not claimed visible.
  MISSING CHAIN PARTS:Wrath stage/mesh aura audit,class/GS/runtime gates.
  BLOCKER:none. NEXT EXACT ACTION:share Wrath stage predicate and test3.5
  dual-stage transition plus71 lifetime visits,then audit5EC aura update.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp kWrathAuraModel.
  NEXT FUNCTION TO IMPLEMENT:verified Wrath stage helper/aura mismatch.

- Implemented private Wrath particle ticks for81E4 subtype0 and81E6 0/1,
  keeping entry-clamp behavior and10/20 exact updates. Shared helper used
  by runtime/tests; per-slot remainder resets on every allocation.
  REVERSE EVIDENCE:17095FD..1709744,1709749..17097C4->172343C.
  CHAIN COVERAGE:selected Wrath particle scale/RGB/alpha/lifetime.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:WrathParticle.h/test,
  particle,project/filters,Wrathreverse,state. LAST STATIC CHECK:x86W4WX
  12cases and scale-crossing test PASS;project3cpp/22headers PASS.
  LAST BUILD RESULT:isolatedWin32 ClCompile PASS,notlinked/staged.
  LAST RUNTIME QA:none;7E619364 stagedunchanged. KNOWN DIFFERENCES:
  actual pool,render sampling and Wrath controller cadence remain open.
  MISSING CHAIN PARTS:controller children/ownership and class/GS/visual gates.
  BLOCKER:none. NEXT EXACT ACTION:remove superseded unreachable generic
  Wrath update bodies,then audit controller70-tick stage/owner behavior.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp WrathController update.
  NEXT FUNCTION TO IMPLEMENT:only verified controller timing/owner mismatch.

- Closed zero-velocity movement question for finite inputs:13272A0 rotates
  velocity into local vector then adds to position; D3189D has only three
  dot products and no translation. No extra angular/position adapter needed.
  REVERSE EVIDENCE:13272A0..1327351,D3189D..D31A0D and nativeZzzAI
  MovePosition. CHAIN COVERAGE:common finite zero-motion structure.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:prefix verifier,MagicPin
  reverse,acceptance matrix,state. LAST STATIC CHECK:prefix verifier PASS;
  private stage full tree/hash/CRC verifier PASS. LAST BUILD RESULT:prior
  7E619364 linked/staged;no new C++ edits. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:nonfinite FP behavior and external owner snapshot
  consumers not proven. Acceptance matrix now accurately labels MagicPin
  IMPLEMENTED/QA OPEN and current hash/project21headers,not full visualPASS.
  MISSING CHAIN PARTS:remaining skills' timing audit,class/GS,pool/visual QA.
  BLOCKER:none. NEXT EXACT ACTION:audit Wrath81E4/81E6 pre-decrement versus
  S21 particle updates and native emitted-lifetime envelopes.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp Wrath particle branches.
  NEXT FUNCTION TO IMPLEMENT:only verified Wrath particle timing mismatch.

- Audited Shockwave1 common prefix versus selected consumers: owner snapshot
 54 exists in S21 prefix but is not read by selected update/default renderer;
  subtype1 does NOT follow owner110, unlike neighbouring render subtypes3..6.
  No speculative position-following or cascade cleanup added.
  REVERSE EVIDENCE:16AA743..16AA7D2,1641638..164168E zero velocity,
  1705379..17054EE,1621A15->1621CF1..1621D95; nativeEffectDestructor.
  CHAIN COVERAGE:bounded prefix/snapshot/render dependencies,not global proof.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:prefix verifier,reverse,state.
  LAST STATIC CHECK:prefix/controller/Thunder verifiers PASS.
  LAST BUILD RESULT:prior7E619364 linked/staged; no new runtime code changes.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:external snapshot consumers and
  actual pool reuse not established; S21 whole path does dereference owner.
  MISSING CHAIN PARTS:common MovePosition helper semantics and full pool/QA,
  class/GS gates. BLOCKER:none. NEXT EXACT ACTION:decode13272A0 movement
  helper at zero velocity to determine whether skipping prefix loses angular
  or other side effects,then progress to remaining skill audit.
  NEXT FILE TO OPEN:pinned dump13272A0 and native MovePosition.
  NEXT FUNCTION TO IMPLEMENT:only demonstrated common movement mismatch.

- Recovered missing smoke19 update, replacing incorrect no-update assumption:
  RGB*.86,Scale+.03,Rotation-.7,Z+3 for20 particle ticks. Private helper and
  per-allocation clock reset implemented; smoke renderer uses rawRGB additive.
  REVERSE EVIDENCE:16AAA32 table selector1723604=10,17235B1->16C2A23;
  selected19 body16C5301..16C5499->16C6317->172343C;renderer1624F33
  ->1625050. CHAIN COVERAGE:smoke selected update/render arguments.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:MagicPinTick/test,particle,
  reverse,stageverifier,thisstate. LAST STATIC CHECK:x86W4WX44cases PASS;
  project3cpp/21headers PASS,private stage2658base79overlay3root PASS.
  LAST BUILD RESULT:isolatedWin32 link PASS,cryptlib PDB warnings only.
  Linked/stagedSHA2567E619364536A55703CE04FA1EBDB28C55C9660AF9EADC578D1BB7D3475CFBA0E.
  BE3A1521 preserved in before-hit GUID backup. LAST RUNTIME QA:none.
  KNOWN DIFFERENCES:common S21 particle prefix16AA743 copies non-null
  owner's position into saved field54 BEFORE subtype dispatch; selected
  Shockwave body does not dereference owner but full prefix does. Audit
  whether saved field is consumed before making whole-path ownership claim.
  MISSING CHAIN PARTS:common prefix/owner lifecycle,actual pools,GPU and
  class/GS gates. BLOCKER:none. NEXT EXACT ACTION:inspect saved-field54
  consumers and native common prefix for private hit particles; test regression.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp common MoveParticles prefix.
  NEXT FUNCTION TO IMPLEMENT:only required native common-prefix mapping.

- Fixed MagicPin Shockwave2 subtype1 pre-decrement mismatch through private
  particle ticks: life8,7,6 retain alpha.6; life5..1 fade to.2, eight updates.
  Preserved null-only owner check with no owner dereference, and reset new
  per-slot remainder for every allocation. Generic other particles unchanged.
  REVERSE EVIDENCE:1705379..17054EE,1706179->172343C;1B9EBD4 float
  .133332998,1B4E910 .0799999982. CHAIN COVERAGE:Shockwave scalar/expiry.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:MagicPinTick/test,
  ZzzEffectParticle,MagicPin reverse,this state. LAST STATIC CHECK:x86W4WX
  40 cases PASS including null and inactive non-null owner at four FPS.
  LAST BUILD RESULT:isolated Win32 ClCompile PASS,not linked/staged.
  LAST RUNTIME QA:none;staged BE3A1521 unchanged. KNOWN DIFFERENCES:
  actual pool reuse and GPU sampling untested. MISSING CHAIN PARTS:smoke19
  full update/render plus remaining class/GS/visual gates. BLOCKER:none.
  NEXT EXACT ACTION:trace CLUD64 subtype19 S21 update and render routes;
  correct only proven mismatch,then link/stage accumulated MagicPin fixes.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp BITMAP_CLUD64 update/render.
  NEXT FUNCTION TO IMPLEMENT:only verified smoke19 adapter mismatch.

- Added shared MagicPin stage predicate and32-case regression including
  inclusive frame boundaries,76 base visits, and9 hit visits/five bursts.
  Hit carrier now uses private whole ticks; no FPS-dependent burst count or
  lost life0 burst. Verified randomXYZ helper matches current cube offsets.
  REVERSE EVIDENCE:1575643 life<5;1575818->1575A35->15945F1;
  DB1E40..DB1F20 independent rand()%41-20 on each axis.
  CHAIN COVERAGE:controller stage/lifetime and hit emission cadence.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:MagicPinTick/test,runtime,
  ZzzEffect,controller verifier,reverse,this state.
  LAST STATIC CHECK:x86 W4WX32cases PASS; controller verifier PASS.
  LAST BUILD RESULT:isolated Win32 ClCompile PASS; not linked/staged.
  LAST RUNTIME QA:none; staged BE3A1521 unchanged. KNOWN DIFFERENCES:
  30 smoke/10 shockwave are creation attempts, not proven allocations;
  actual child pool/cadence/GPU untested. MISSING CHAIN PARTS:hit particle
  update/reuse and class/GS/visual gates. BLOCKER:none.
  NEXT EXACT ACTION:audit smoke19 and Shockwave2 subtype1 native updates
  against pinned S21 before linking/staging controller corrections.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp kShockwave2Bitmap update.
  NEXT FUNCTION TO IMPLEMENT:only verified hit particle mismatch.

- Corrected MagicPin base controller child ownership and transforms: all four
  creations now use controller owner/angle/light; first offset is rotated and
  translated from controller snapshot, not live caster. Base subtype0 now
  sets Scale1/Timer0 and follows private whole ticks, preserving life0 visit.
  REVERSE EVIDENCE:1480871 ctor,1578320/8495/8581/8613 owner pushes,
  15783AB/83EF/8425/845A transform,1578652 common tail.
  CHAIN COVERAGE:base controller creation args/timing integration.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:runtime,ZzzEffect,controller
  verifier,MagicPin reverse,this state. LAST STATIC CHECK:controller pins
  and project3cpp/21headers PASS. LAST BUILD RESULT:Win32 ClCompile PASS,
  not linked/staged; QA executable remains BE3A1521.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:owner animation sampling and
  actual child-owner pool reuse untested; synthetic hit subtype1 untouched.
  MISSING CHAIN PARTS:hit controller/cadence, class/GS/visual gates.
  BLOCKER:none. NEXT EXACT ACTION:add shared base-controller stage/lifetime
  regression, then inspect actual5B9 hit constructor/update before any change.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp EmitMagicHitTick.
  NEXT FUNCTION TO IMPLEMENT:verified base stage helper and hit adapter fix.

- Closed selected Thunder renderer routing/arguments without a speculative
  render change: S21 raw RGB ignores particle alpha, same as native default.
  REVERSE EVIDENCE:1620BE0 comparison tree through1620D55->1640DD2,
  default1640E6E sprite call;1620AD0 size/blend setup,18E7137 additive1/1.
  CHAIN COVERAGE:7F2B dispatch and RGB/rotation/texture arguments verified.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:MagicPin reverse,new
  verify_magic_thunder_renderer.py,this state. LAST STATIC CHECK:verifier
  PASS actual selected tree walk and callback/native default checks.
  LAST BUILD RESULT:prior BE3A1521 linked/staged, no new native code changes.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:GPU/pool cadence remains open;
  no additional alpha fade should be introduced for Thunder9.
  MISSING CHAIN PARTS:MagicPin controller/contact cadence and overall
  class/GS/visual gates. BLOCKER:none. NEXT EXACT ACTION:audit MagicPin
  controller lifetime and timed child emission against S21 complete update.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp MagicPinController update.
  NEXT FUNCTION TO IMPLEMENT:only proven controller timing mismatch.

- Implemented MagicPin Thunder9/10 constructor and private whole-particle
  updates; no change to existing ENERGY subtypes1..7. Conditional color/fade
  uses remaininglife2 AND1, not a guessed one-time half-life envelope.
  REVERSE EVIDENCE:1650BF2..1650D9B,16C01D3..16C03D0; particle common
  expiry16AA648 and post-decrement172343C. CHAIN COVERAGE:constructor,
  scalar update,3 visits and native Live=false expiry; renderer still open.
  ASSET PROVENANCE:unchanged pinned7F2B Thunder01. FILES MODIFIED:
  GrowLancerMagicPinTick.h,ZzzEffectParticle.cpp,MagicPinTickTest,reverse,
  stage verifier,this state. LAST STATIC CHECK:24 shared-helper cases PASS;
  project inclusion3cpp/21headers PASS; alpha verifier PASS.
  LAST BUILD RESULT:isolated Win32 link PASS (cryptlib missing-PDB warnings).
  LAST RUNTIME QA:not run; isolated stage verifier PASS2658base/79overlay/
  3rootData files. Linked/stagedSHA256
  BE3A1521657A096C5A2B206443686A6F7DBC213FD6660B4AA964FE0EE4C0B7D5.
  Previous AE87 executable retained in before-thunder GUID backup.
  KNOWN DIFFERENCES:Thunder renderer and real pool cadence unverified.
  MISSING CHAIN PARTS:remaining renderer/contact/controller audit and
  class/authoritative GS/owner visual gates. BLOCKER:none for offline work.
  MERGE AUDIT:project inclusion confirmed, NOT Git-merged; changes remain
  dirty/untracked in feature/grow-lancer-skills-s21, protected trees untouched.
  NEXT EXACT ACTION:trace S21 7F2B particle render selection and alpha use,
  compare native RenderParticles before claiming Thunder visual parity.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp RenderParticles.
  NEXT FUNCTION TO IMPLEMENT:only proven private Thunder render mismatch.

- Found missing MagicPin Thunder subtype9/10 implementation: nativeENERGY
  constructor/update onlyhandle1..7. Same texture name was insufficient.
  REVERSE EVIDENCE:1480B6D/1480BBB push7F2B;1641908->16508CA constructor,
  16AA98D->16BF9D6 update. Registry8148 alsoThunder01 but differentwrap.
  CHAIN COVERAGE:exactresource and handler targets;subtypes notyetrecovered.
  ASSET PROVENANCE:existingbyteidentical7F2B texturevalid,not8148substitution.
  FILES MODIFIED:MagicPinreverse,thisstate. LAST STATIC CHECK:sourceabsence
  anddispatch evidence;no newtest. LAST BUILD RESULT:priorAE87B374 linked.
  LAST RUNTIME QA:none. KNOWN DIFFERENCES:9/10 currentlyfallthroughnative
  defaults,soMagicPin particle parity NOTachieved. MISSING CHAIN PARTS:
  constructor/update/render9/10 plusfullgates. BLOCKER:none.
  NEXT EXACT ACTION:decode16508CA and16BF9D6 subtype9/10 complete paths,
  then implement narrowly scoped particle constructor/update/lifetime.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp BITMAP_ENERGY branches.
  NEXT FUNCTION TO IMPLEMENT:verifiedThunder9/10 private adapter.

- MagicPin01 blendlight corrected to1;01/root literalzeroUV preserved,
 03/Aux retainV+.035 and animatedUV. SpinCrossshared branch unchanged.
  REVERSE EVIDENCE:A1B1F1 registrations,A48FEF literal1,A48FDD/E6,
  A4926C/75 zeroUV,A49044/A492D6 Vadvance. CHAIN COVERAGE:renderargs.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:runtime,stageverifier,MagicPin
  reverse,acceptance matrix,state. LAST BUILD RESULT:isolatedWin32linkPASS.
  LAST RUNTIME QA:none;stagedSHA256
  AE87B3740DA5EAA096034C57E32810050219FE5C0F1B8777500AE7CC294D525D.
  Prior7650105C retained in isolated before-magic-pin GUID backup.
  KNOWN DIFFERENCES:actualparticle/update/renderer/pool QAopen.
  MISSING CHAIN PARTS:MagicPin Thunder subtype9/10 particle chain/cadence,
  class/GS/runtime. BLOCKER:none. NEXT EXACT ACTION:verify stage,then
  audit Thunder subtype9/10 constructor/update/renderer against native path.
  NEXT FILE TO OPEN:ZzzEffectParticle.cpp GrowLancer Thunder branch.
  NEXT FUNCTION TO IMPLEMENT:only verified private particle mismatch.

- MagicPin shared scalar regression implemented: runtime uses same private
  header as test;16cases atfourFPS PASS11/11/15/11 visits,alpha/scale/angle
  endpoints and singlecleanup. Removed root's non-source clamp;no global edit.
  REVERSE EVIDENCE:priorMagicPin update/math pins,verifier stillPASS after
  extraction. CHAIN COVERAGE:actual sharedscalar code,not realpool/vector/GPU.
  ASSET PROVENANCE:unchanged. FILES MODIFIED:MagicPin helper/test,runtime,
  project/filters,alpha verifier,reverse doc,this state.
  LAST STATIC CHECK:x86W4WX16casesPASS,project3cpp/21headersPASS.
  LAST BUILD RESULT:isolatedQAClCompilePASS,notlinked/staged.
  LAST RUNTIME QA:none;7650105C unchanged. KNOWN DIFFERENCES:movement and
  particle children/render not fully re-audited. MISSING CHAIN PARTS:UV/pass
  parameters and fullruntime/class/GS gates. BLOCKER:none.
  NEXT EXACT ACTION:inspect MagicPin renderer registration callbacks,verify
  V advancement/meshblend arguments,thenlink/stage private child corrections.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp MagicPin RenderEffect.
  NEXT FUNCTION TO IMPLEMENT:only proven private render mismatch.

- MagicPin03/Root/Aux Timer0/whole ticks added; auxiliary scale fixed from
  invented-.0025 to -float(sin(30radians))*.005 (positive~.004940158).
  REVERSE EVIDENCE:157890B..15789D8,96A03C double-wrapper,CRT fallback
  fsin1A36DAD; threshold5/peaks.8/.75 verified for03/Aux.
  CHAIN COVERAGE:selectedscalar updates/lifetime. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:runtime,MoveEffect,alpha verifier,MagicPin doc,this state.
  LAST STATIC CHECK:extended math pins; not native execution regression.
  LAST BUILD RESULT:isolatedQA ClCompilePASS,notlinked/staged.
  LAST RUNTIME QA:none;7650105C unchanged. KNOWN DIFFERENCES:actual movement,
  samples/pool/visual QA still open. MISSING CHAIN PARTS:MagicPin particle
  children/renderer/cadence and class/GS acceptance. BLOCKER:none.
  NEXT EXACT ACTION:add focused11/15/11 scalar/lifetime regression and
  verifyUV renderer,then link/stage all MagicPin child corrections.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp MagicPin render branch.
  NEXT FUNCTION TO IMPLEMENT:only proven private MagicPin render mismatch.

- MagicPin01 alpha corrected from generic triangle to incremental +/-.2;
  Timer0/private whole ticks added, life10..0 includes11 updates, no clamp.
  REVERSE EVIDENCE:1578657..15786D2, max10/constant2, direct common tail.
  CHAIN COVERAGE:selected5E6 alpha/lifecycle. ASSET PROVENANCE:unchanged.
  FILES MODIFIED:runtime,MoveEffect,alpha verifier,MagicPin doc,this state.
  LAST STATIC CHECK:pinned alpha/source verifier added;not runtime test.
  LAST BUILD RESULT:isolatedQA ClCompilePASS,notlinked/staged.
  LAST RUNTIME QA:none;7650105C unchanged. KNOWN DIFFERENCES:other MagicPin
  children still generic envelopes;Aux uses-.0025 rather than source sine.
  MISSING CHAIN PARTS:5E7/5E8/5E9 preciseupdates and remaining full gates.
  BLOCKER:none. NEXT EXACT ACTION:verify5E7 threshold/divisor and5E9 sine
  constant/radians before correcting remaining private child updates.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp MagicPin03/Root/Aux.
  NEXT FUNCTION TO IMPLEMENT:next verified MagicPin child scalar correction.

- Linked/staged Harsh controller+private blur cadence. SHA256
  7650105CF09FC5EFA932FAA173A822D82BFE6F647ADF63FFEAFB3C3BF45A4F65.
  Prior90E643C5 backup Engine-Port S21.exe.before-harsh-blur-
  b2c0fdffe51a4ec7a4af952ebdb42594.bak inside isolated client.
  REVERSE EVIDENCE: style0 renderer anchored15C04B6;15C07FE..15C09D9
  matches native tail RGB,U/V,vertex order. CHAIN COVERAGE:selected renderer
  args;no new global blur edit. ASSET PROVENANCE:stage2658+79+3,hash/CRC PASS.
  FILES MODIFIED:stage verifier,acceptance matrix,Harsh doc,this state.
  LAST STATIC CHECK: stagePASS. LAST BUILD RESULT: isolatedWin32linkPASS,
  cryptlibPDBwarnings. LAST RUNTIME QA:none. KNOWN DIFFERENCES:lowFPS
  batching/actualGLstate/owner reuse/pool stress remain open.
  MISSING CHAIN PARTS: other skill scalar/render audits plus class/GS/runtime.
  BLOCKER:none. NEXT EXACT ACTION: audit Magic Pin child alpha envelopes,
  especially generic SymmetricAlpha versus exact S21 update thresholds.
  NEXT FILE TO OPEN:GrowLancerEffectRuntime.cpp MagicPin01/03/Root/Aux update.
  NEXT FUNCTION TO IMPLEMENT:only proven Magic Pin update mismatch.

- Implemented private Harsh blur cadence sidecar at allocation; reset on
  every allocation/ClearAll, no update-time owner dereference. Existing
  appends do not refresh lifetime/clock. Legacy executes original one tick.
  REVERSE EVIDENCE: priorpool/cadence audit in HARSH_STRIKE_REVERSE.md.
  CHAIN COVERAGE: private accumulator integrated around unchanged native
  lifetime/Number/tail tick. ASSET PROVENANCE: unchanged. FILES MODIFIED:
  Harsh helper/test,ZzzEffectBlurSpark.cpp,Harsh doc,this state.
  LAST STATIC CHECK: prior16cases plus4FPS cadence/reset/legacy helpersPASS.
  LAST BUILD RESULT: isolatedQA ClCompilePASS,not linked/staged.
  LAST RUNTIME QA:none;90E643C5 unchanged. KNOWN DIFFERENCES: tests model
  append counter only,not actual allocator/tail geometry;lowFPS batching
  and real exhaustion/owner reuse untested. MISSING CHAIN PARTS: blur
  renderer and actual pool/visualQA,class/GS gates. BLOCKER:none.
  NEXT EXACT ACTION: inspect integrated allocation reset and run pool verifier,
  then link/stage combinedcontroller+blur correction and audit style0 renderer.
  NEXT FILE TO OPEN:ZzzEffectBlurSpark.cpp RenderObjectBlurs.
  NEXT FUNCTION TO IMPLEMENT:next proven private render mismatch only.

- Verified native blur scheduling mismatch: MainScene CalcFPS->MoveMainScene
  ->MoveCharactersClient->MoveBlurs->MoveObjectBlurs runs consumer before
  MoveEffects. Consumer uses one decrement/call; private Harsh producer now
  uses accumulated REFERENCE_FPS/FPS. No global primitive changed yet.
  REVERSE EVIDENCE: ZzzScene2222/1937/1942,ZzzCharacter6346,ZzzAI825;
  S21 anchoredMoveBlurs15BFCBD->15C030C and caller13F4122.
  CHAIN COVERAGE: native call ordering and producer/consumer mismatch.
  ASSET PROVENANCE: unchanged. FILES MODIFIED:Harsh reverse,this state.
  LAST STATIC CHECK: priorpoolverifierPASS; new scheduling evidence is source
  audit,not measuredFPS. LAST BUILD RESULT: controllerClCompilePASS unchanged,
  notlinked. LAST RUNTIME QA:none;stage90E643C5 unchanged.
  KNOWN DIFFERENCES: factor.5 currently consumes30life in15 reference ticks.
  MISSING CHAIN PARTS: private blur cadence metadata/reset/order regression,
  S21 higher-level scheduling,render and full acceptance gates.
  BLOCKER:none. NEXT EXACT ACTION: implement narrowly scoped Harsh blur
  per-slot cadence and test consumer-before-producer/append/reuse; preserve
  legacy SS6. Confirm S21 parent prologue before any further caller trace.
  NEXT FILE TO OPEN:ZzzEffectBlurSpark.cpp CreateObjectBlur/MoveObjectBlurs.
  NEXT FUNCTION TO IMPLEMENT:private Harsh blur cadence with allocation reset.

- Harsh object-blur pool matched to native:1000 records,599tail clamp,
  owner/group3 reuse,defaultlife30,no append lifetime refresh,pre-decrement
  expiry. No global primitive changes. REVERSE EVIDENCE:15C01CC allocator,
  15C22D8 append,15C030C update; verify_harsh_blur_pool.py PASS.
  CHAIN COVERAGE: selected pool semantics only. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: pool verifier,Harsh doc,this state. LAST STATIC CHECK:
  pool pinsPASS; previousHarsh16casesPASS. LAST BUILD RESULT: priorcontroller
  ClCompilePASS,not linked/staged. LAST RUNTIME QA:none;90E643C5 unchanged.
  KNOWN DIFFERENCES: producer whole ticks vs consumer scheduling unresolved.
  MISSING CHAIN PARTS: blur higher-level cadence,style0 renderer/pool stress,
  class/GS/runtime gates. BLOCKER:none. NEXT EXACT ACTION: trace native
  MoveCharactersClient scheduling and S21 caller15BFE6E for blur cadence;
  only then consider scoped per-record tick adapter if mismatch proven.
  NEXT FILE TO OPEN: ZzzScene.cpp MoveCharactersClient call sites.
  NEXT FUNCTION TO IMPLEMENT: only proven private blur cadence mismatch.

- Harsh controller whole ticks/Timer0 and blur animation window corrected.
  REVERSE EVIDENCE:15777033/15777D6C->15945F1;157773D6 raw action speed,
  six-step157773EB;1577748F..157774B3 gates Animation and blur alike.
  CHAIN COVERAGE: default controller expiry and shared-bone write window.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: runtime,MoveEffect,Harsh
  focused test,reverse doc,this state. LAST BUILD RESULT: QA ClCompilePASS,
  not linked/staged. LAST RUNTIME QA: none; staged90E643C5 unchanged.
  KNOWN DIFFERENCES: lowFPS owner-frame interpolation and actual blur pool
  still need validation. MISSING CHAIN PARTS: blur primitive/render lifecycle,
  full class/GS/runtime gates. BLOCKER: none. NEXT EXACT ACTION: run extended
  lifecycle test, then audit native blur update/render vs S21 type11 group3.
  NEXT FILE TO OPEN: ZzzEffectBlurSpark.cpp CreateObjectBlur/MoveBlur.
  NEXT FUNCTION TO IMPLEMENT: next proven private blur mismatch only.

- Harsh wind01 material corrected: fixed RGB without extra Alpha multiply;
  all four native mesh calls use literal zero UV, matching S21 callback.
  REVERSE EVIDENCE: A48821..A48A5A, verifier four calls/eight UV zeros and
  material constants PASS. CHAIN COVERAGE: selected wind01 render contract.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: runtime, renderer verifier,
  stage verifier,Harsh reverse,acceptance matrix,this state.
  LAST STATIC CHECK: renderer verifier PASS. LAST BUILD RESULT: isolated
  Win32 linkPASS with cryptlib PDB warnings; includes prior9/8tick fixes.
  LAST RUNTIME QA: no gameplay. StagedSHA256
  90E643C5A85934AA6A7E664A425C220E9B828FD31C1E96EDDCE5D938661F810F.
  PriorBFF61DB6 backup Engine-Port S21.exe.before-harsh-wind-
  1ad87d78eb504c96957463d395fd3932.bak in isolated client.
  KNOWN DIFFERENCES: actual transforms/pool/visualQA open.
  MISSING CHAIN PARTS: remaining controller/blur validation,class/GS/runtime.
  BLOCKER: none. NEXT EXACT ACTION: verify stage then audit Harsh controller
  animation/blur sampling against native six-sample source and lifecycle.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp Harsh controller update.
  NEXT FUNCTION TO IMPLEMENT: next proven controller/blur mismatch only.

- Harsh wind01/02 moved to audited private whole ticks with Timer0. Removed
  non-source wind01 Alpha clamp and fractional RGB power fade.
  REVERSE EVIDENCE:1577F01/157810E->15945F1; selectedtypes skip early
  decrement;1577EEE Alpha-.125,15778065 Scale+.45,RGB*.9 at98/CA/FC.
  CHAIN COVERAGE:9/8 update visits for initial8/7life; scalar updates/expiry.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: Harsh helper/test,runtime,
  MoveEffect,project/filters,Harsh reverse doc,this state.
  LAST STATIC CHECK: focused8cases x86/W4/WX PASS; project3cpp/20headersPASS.
  LAST BUILD RESULT: isolated QA ClCompilePASS, not linked/staged.
  LAST RUNTIME QA: none; stagedBFF61DB6...A064835D unchanged.
  KNOWN DIFFERENCES: actual transformed movement/renderer/pool still needQA.
  MISSING CHAIN PARTS: Harsh renderer parameter audit and full runtime/class/GS.
  BLOCKER: none for offline work. NEXT EXACT ACTION: audit wind01 fixed-color
  renderer against A48821 and current alpha multiplication, then link/stage.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp RenderEffect Harsh branch.
  NEXT FUNCTION TO IMPLEMENT: next proven Harsh renderer mismatch only.

- Linked/staged body/spin shared-lighting correction. SHA256
  BFF61DB6F1C15EFFC28370E6A2BECAC121A044B1BBA696CB48A053A9A064835D.
  Previous2EEBB97F backed up in isolated client as Engine-Port S21.exe.
  before-shining-lighting-1a748a38f03a4e089be98e31ffa53e51.bak.
  REVERSE EVIDENCE: Shining callback/default lighting verifiers PASS.
  CHAIN COVERAGE: linked private correction only, not whole visual acceptance.
  ASSET PROVENANCE: stage2658base+79overlay+3root PASS with EXE/player hashes
  and CRC. FILES MODIFIED: stage verifier, acceptance matrix, this state.
  LAST STATIC CHECK: project3cpp/19headers, two Shining render verifiers PASS.
  LAST BUILD RESULT: isolated Win32 link PASS; cryptlib PDB warnings only.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: Harsh wind01/02 still fractional
  update/decrement path, wind01 has non-source alpha clamp pending audit.
  MISSING CHAIN PARTS: Harsh fixed-tick lifecycle; class/GS/runtime gates.
  BLOCKER: none for offline work. NEXT EXACT ACTION: verify1577D71..157810E
  selected Harsh child updates and common expiry, implement private whole
  ticks/Timer0 with focused lifecycle regression; no global changes.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp kHarshWind01/02Model.
  NEXT FUNCTION TO IMPLEMENT: Harsh child tick correction from dump evidence.

- Follow-up: optional AddMeshCommand bounds verified before VAOMesh access;
  false return falls through to legacy bounds guard. Both native mesh paths
  reject spin's index2 on two-mesh model. Verifier extended; no shader edit.
  NEXT EXACT ACTION: link and stage body/spin lighting correction; then
  continue remaining effect/render and class/server acceptance gaps.

- Corrected body/spin lighting: retain Calc_RenderObject BodyLight; only
  Shining pin overrides Light*Alpha. No global renderer changes.
  REVERSE EVIDENCE: reset1315EDB/1315EE2 flags6/7=1, CreateEffect143E74F
  flag5=1; shared1885EB0 terrain+Light; registry callback1887E4E.
  CHAIN COVERAGE: default lighting initialization and private callback mapping.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: runtime, mesh renderer
  verifier, Shining reverse doc, this state. LAST STATIC CHECK: callback,
  lighting-default and pin-only override checks PASS.
  LAST BUILD RESULT: isolated QA ClCompile PASS; not linked/staged.
  LAST RUNTIME QA: none; staged2EEBB97F...21CA805F unchanged.
  KNOWN DIFFERENCES: buff/special rendering and terrain sampling need QA;
  optional shader mesh guard still open. MISSING CHAIN PARTS: these plus
  class/GS/remaining runtime gates. BLOCKER: none for offline work.
  NEXT EXACT ACTION: inspect optional shader AddMeshCommand bounds, then
  link and hash-stage the private lighting correction.
  NEXT FILE TO OPEN: native shader AddMeshCommand implementation.
  NEXT FUNCTION TO IMPLEMENT: only proven private integration mismatch.

- Shining body/pin/spin callback registrations verified directly. Body/spin
  lack pin's explicit Light*Alpha, exposing current port's shared-lighting
  mismatch for further trace. S21 RenderMesh bounds guard explains third
  spin call on two-mesh asset; native legacy guard matches.
  REVERSE EVIDENCE: A1B2DF..A1B321; A49D77/A49DD2/A49EE5;
  968CB3->13312EF bounds1331321..1331330; shared lighting1885EB0.
  CHAIN COVERAGE: callback arguments and legacy invalid-mesh rejection.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: mesh renderer verifier,
  Shining reverse doc, this state. LAST STATIC CHECK: prior stage verifier
  PASS2658+79+3, server/profile static PASS with four profile divergences.
  LAST BUILD RESULT: prior isolated linked2EEBB97F unchanged this run.
  LAST RUNTIME QA: none. KNOWN DIFFERENCES: current body/spin Light*Alpha
  override is not in their callbacks; shared lighting/buff path still open.
  MISSING CHAIN PARTS: shared dispatch lighting, optional shader guard,
  other class/GS/runtime gates. BLOCKER: none for offline reverse.
  NEXT EXACT ACTION: trace remaining1887B8B draw callback invocation after
  189346D buff-color helper; audit default effect lighting flags5/6/7.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp Shining RenderEffect.
  NEXT FUNCTION TO IMPLEMENT: verified body/spin lighting correction only.

- Shockwave AD6/sub6 renderer corrected from generic Draw_RenderObject to
  native mesh0 twice, flags42, raw Light and actual BlendMeshLight/U/V.
  REVERSE EVIDENCE: A1CF92 registration -> A72AD0, subtype6 A7300C,
  calls A730C4/A73118; pinned verifier PASS. CHAIN COVERAGE: selected
  render-call contract and previously fixed seven-visit lifecycle.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: runtime, renderer verifier,
  stage hash verifier, Shining reverse doc and this state.
  LAST STATIC CHECK: renderer pins PASS; project3cpp/19headers PASS.
  LAST BUILD RESULT: isolated Win32 link PASS, cryptlib PDB warnings only.
  LAST RUNTIME QA: no gameplay; staged SHA256
  2EEBB97F6EA2AEE8731688B8BAC4D5BD4E2049E9C4565E27F34D7E1821CA805F.
  Prior2F245623 backed up as Engine-Port S21.exe.before-shining-shockwave-
  fd94a1aad32142a2be0bd6b4cf9f4e7d.bak inside isolated client.
  KNOWN DIFFERENCES: transform/GPU parity untested. Merge audit found dirty
  tracked and untracked files: project inclusion PASS is NOT Git merge PASS.
  No protected branch/source was merged or modified.
  MISSING CHAIN PARTS: remaining primitive transforms, class/GS/runtime gates.
  BLOCKER: none for offline work. NEXT EXACT ACTION: verify staged assets,
  then audit Shining body/pin/spin renderer arguments against callbacks.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp Shining mesh render branch.
  NEXT FUNCTION TO IMPLEMENT: only next proven private renderer mismatch.

- Shockwave lifecycle corrected: verified1574C65->15945E5 call1595A11
  rotate flag1,return1595C4F then common15945F1. Added privateTimer0/whole
  ticks for life6..0. No global behavior or rendering changed.
  REVERSE EVIDENCE: SHINING_PEAK_REVERSE.md shockwave follow-up.
  CHAIN COVERAGE: selected lifecycle; actual GPU/transform path untested.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: runtime ctor/comment,
  ZzzEffect.cpp,mesh test,reverse doc,this state.
  LAST STATIC CHECK: mesh20casesPASS, shockwave4 cases are lifecycle-only.
  LAST BUILD RESULT: isolated QA ClCompilePASS, not linked/staged.
  LAST RUNTIME QA: staged2F245623...5ED8AA03 unchanged; no gameplay.
  KNOWN DIFFERENCES: Draw_RenderObject private model mapping needs S21 audit.
  MISSING CHAIN PARTS: exact AD6/sub6 render contract and runtime/GS/class gates.
  BLOCKER: none. NEXT EXACT ACTION: trace S21 AD6 render registration/subtype6
  to native Calc_RenderObject/Draw_RenderObject, then link/stage if appropriate.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp RenderEffect shockwave branch.
  NEXT FUNCTION TO IMPLEMENT: only proven private shockwave renderer mismatch.

- Linked/staged three Shining mesh corrections. SHA256:
  2F245623C56DC148F123B79731287B52F8070B147250D6703CC4E86A5ED8AA03.
  Backup RuntimeQA/Client/Engine-Port S21.exe.before-shining-mesh-
  69e8942ac71e41249e9c823a3b651b5e.bak preservesAAA4F984.
  REVERSE EVIDENCE: shockwave147010E life6; common15945F1 excludes AD6
  from early decrement and checks expiry1594624 before15946E3 decrement.
  CHAIN COVERAGE: mesh fixes linked; shockwave generic lifetime mismatch lead
  now confirmed at common manager, default helper return still to check.
  ASSET PROVENANCE: unchanged79overlay. FILES MODIFIED: stage verifier,
  reverse doc,acceptance matrix,this state. LAST STATIC CHECK: stage2658+79+3,
  EXE/Player SHA/CRC PASS; project3cpp/19headersPASS.
  LAST BUILD RESULT: isolated Win32 linkPASS, known cryptlib PDB warnings.
  LAST RUNTIME QA: no new gameplay. KNOWN DIFFERENCES: shockwave remains
  native generic decrement-then-kill until next verified correction.
  MISSING CHAIN PARTS: shockwave lifetime/render QA,other class/GS/visual gates.
  BLOCKER: none. NEXT EXACT ACTION: verify1595A11 return/default continuation,
  then private shockwave Timer0/whole ticks and seven-visit regression.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp kShiningPeakShockwaveModel.
  NEXT FUNCTION TO IMPLEMENT: private shockwave tick lifecycle integration.

- Pin/spin Shining mesh alpha halves corrected from5/11 to4/10. Evidence
  157BF2A..157BFB8 and157C14B..157C1D9 has NO body's+1. Timer0 and private
  whole ticks for both; pin rotated local-50 once/tick,spinY+20 once/tick.
  Shared mesh regression now16casesPASS (body8,pin/spin8), ClCompilePASS.
  REVERSE EVIDENCE: SHINING_PEAK_REVERSE.md pin/spin correction.
  CHAIN COVERAGE: three mesh update math/life0 ordering tested; GPU/trajectory
  remains OPEN. ASSET PROVENANCE: unchanged. FILES MODIFIED: mesh helper,
  runtime,ZzzEffect.cpp,mesh test,reverse doc,this state.
  LAST STATIC CHECK: x86/W4/WX16casesPASS. LAST BUILD RESULT: QA ClCompilePASS;
  three mesh fixes not linked/staged. LAST RUNTIME QA: unchangedAAA4F984.
  KNOWN DIFFERENCES: native transform path not included in math-only tests.
  MISSING CHAIN PARTS: mesh link/stage/render comparison,class/GS/visual gates.
  BLOCKER: none. NEXT EXACT ACTION: link/stage all three mesh fixes with hash
  guards, then audit remaining Shining shockwave lifetime and render contract.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp kShiningPeakShockwaveModel.
  NEXT FUNCTION TO IMPLEMENT: only proven shockwave adapter/tick discrepancy.

- Shining body5F4 shared helper/regression completed: runtime calls
  UpdateShiningBodyTick; tests use same helper with real whole-tick contract.
  Two subtypes/fourFPS =8cases PASS13 exact alpha samples,negativefinalalpha,
  subtype scale behavior and single cleanup. REVERSE EVIDENCE: previously
  pinned157BE66..157BF07; SHINING_PEAK_REVERSE.md updated proof scope.
  CHAIN COVERAGE: body update ordering now regression tested; visual OPEN.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: GrowLancerShiningMesh.h,
  runtime helper call,test,project/filters,reverse doc,this state.
  LAST STATIC CHECK: x86/W4/WX8casesPASS; project3cpp/19headersPASS.
  LAST BUILD RESULT: QA ClCompilePASS; body fix NOT linked/staged yet.
  LAST RUNTIME QA: unchangedAAA4F984...B1F1D77E; no gameplay.
  KNOWN DIFFERENCES: negativealpha follows dump, not owner-tested GL output.
  MISSING CHAIN PARTS: pin/spin mesh timing,body link/stage,new class/GS/visual.
  BLOCKER: none. NEXT EXACT ACTION: inspect157BF0C and157C119 half-life
  arithmetic, then migrate pin/spin to verified whole ticks before linking.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp pin/spin update cases.
  NEXT FUNCTION TO IMPLEMENT: shared pin/spin mesh tick helper if verified.

- Flare5 linked/staged; stage2658+79+3/SHA/CRC and project3cpp/18headersPASS.
  Linked/staged SHA AAA4F98428D5287E46A6C07DB297C888C2F9143645BE405C12C48BD4B1F1D77E.
  Backup RuntimeQA/Client/Engine-Port S21.exe.before-flare5-
  d684b1573a624c3ab319b8061d14ac88.bak preserves13AD9A15.
  Then audited body5F4 update157BE66..157BF07: NO alpha clamp, returns15945F1.
  Removed native0..1 clamps; set bodyTimer0 and private whole ticks including
  life0. This NEW body source change awaits compile/regression/link/stage.
  REVERSE EVIDENCE: SHINING_PEAK_REVERSE.md model contract note.
  CHAIN COVERAGE: body clamp discrepancy corrected in source; pin/spin mesh
  timing still pending. ASSET PROVENANCE: unchanged79overlay.
  FILES MODIFIED: stage verifier,GrowLancerEffectRuntime.cpp,ZzzEffect.cpp,
  reverse doc,acceptance matrix,this state.
  LAST STATIC CHECK: staging/projectPASS BEFORE new body source edit.
  LAST BUILD RESULT: flare5 linkPASS; body edits NOT built yet.
  LAST RUNTIME QA: no gameplay. KNOWN DIFFERENCES: body life0/negativealpha
  follows reviewed S21; no full mesh visual evidence yet.
  MISSING CHAIN PARTS: body regression and pin/spin timing,class/GS/visual.
  BLOCKER: none. NEXT EXACT ACTION: add focused shared body helper/regression
  for13 ticks/negativealpha then compile; audit pin/spin half-life thresholds.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp kShiningPeakBodyModel.
  NEXT FUNCTION TO IMPLEMENT: shared body tick regression contract.

- Fixed private flareBlue/sub5 alpha one-tick offset: moved formula before
  decrement using whole particle ticks. REVERSE EVIDENCE:16B25E8..16B26EE
  ->16B3A77->172343C; SHINING_PEAK_REVERSE.md flare5 correction.
  CHAIN COVERAGE: alpha timing corrected; constructor/renderer unchanged.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: GrowLancerFlareParticle.h,
  test,ZzzEffectParticle.cpp,Main.vcxproj/filters,reverse doc,this state.
  LAST STATIC CHECK: x86/W4/WX fourFPS exact20 alpha samples PASS; peak.85,
  last.085,no life0 body; project3cpp/18headersPASS.
  LAST BUILD RESULT: isolated QA ClCompilePASS, not linked/staged yet.
  LAST RUNTIME QA: unchanged13AD9A15...94559161 candidate; no gameplay run.
  KNOWN DIFFERENCES: GPU sampling/pool lifecycle still unverified by tests.
  MISSING CHAIN PARTS: remaining Shining mesh children,new class/GS/visual gates.
  BLOCKER: none for offline work. NEXT EXACT ACTION: link/stage flare5 fix,
  then audit5F4/5F5/5F7 mesh child timing against native MoveEffect.
  NEXT FILE TO OPEN: GrowLancerEffectRuntime.cpp and SHINING_PEAK_REVERSE.md.
  NEXT FUNCTION TO IMPLEMENT: any proved private mesh tick mismatch.

- Xsuper renderer audited162C314..162C3C6: fullUV,Light directly,Rotation,
  textureType+ordinal. Shared1620AD0..1620B52 texture-size/Scale and RGB
  additive selection match native for decoded six RGB128x128 staged images.
  Linked and staged whole-tick frame-order correction.
  REVERSE EVIDENCE: SHINING_PEAK_REVERSE.md renderer follow-up.
  CHAIN COVERAGE: reviewed Xsuper parameters, not whole GPU/runtime parity.
  ASSET PROVENANCE: six existing OZJ payloads read-only;79overlay unchanged.
  FILES MODIFIED: stage verifier,reverse doc,acceptance matrix,this state.
  LAST STATIC CHECK: stage2658+79+3/hash/CRC PASS; project3cpp/17headersPASS.
  LAST BUILD RESULT: isolated QA Win32 linkPASS, known cryptlib PDB warnings;
  linked/staged SHA13AD9A157E67F00A0B8BBFCC7E89FAF72D007898BDA8679C3B087C2094559161.
  Backup RuntimeQA/Client/Engine-Port S21.exe.before-xsuper-
  01643394e5d34a24847d0f8cac17ca77.bak preserves previous6653FBC7.
  LAST RUNTIME QA: no new gameplay. KNOWN DIFFERENCES: base versus selected
  texture size lookup is equivalent for these six128x128 RGB assets only.
  MISSING CHAIN PARTS: remaining primitive contracts/class/GS/owner gates.
  BLOCKER: none for offline work. NEXT EXACT ACTION: review Shining Peak
  flareBlue/sub5 particle timing (currently generic pre-decrement), then
  independent mesh children. NEXT FILE TO OPEN: ZzzEffectParticle.cpp
  kFlareBlueBitmap and SHINING_PEAK_REVERSE.md.
  NEXT FUNCTION TO IMPLEMENT: only verified private flare5 timing correction.

- Shining Peak Xsuper pre-decrement bug fixed: native skipped ordinal0 at
  factor1. S21 computes6-life BEFORE decrement17017F5->172343C. Added
  GrowLancerXsuper.h and private whole-tick path with allocation reset,
  explicit six texture IDs and Light*.95. Removed old clamped FPS branch.
  REVERSE EVIDENCE:17017F5..17018FE plus known particle-manager ordering;
  SHINING_PEAK_REVERSE.md correction. CHAIN COVERAGE: texture sequence and
  decay ordering fixed; renderer sampling/runtime acceptance remains OPEN.
  ASSET PROVENANCE: unchanged six pinned Xsuper frames in79overlay.
  FILES MODIFIED: Xsuper header/test,ZzzEffectParticle.cpp,project/filters,
  reverse doc,this state. LAST STATIC CHECK: x86/W4/WX fourFPS sequence test
  PASS0..5/six decays/no life0 body; project3cpp/17headersPASS.
  LAST BUILD RESULT: isolated QA ClCompile PASS; no link/stage yet.
  LAST RUNTIME QA: unchanged staged6653FBC7...E99812E3; no new gameplay.
  KNOWN DIFFERENCES: renderer sampling at low FPS not covered by tick tests.
  MISSING CHAIN PARTS: Xsuper renderer review,other primitive/class/GS/QA gates.
  BLOCKER: none for offline work. NEXT EXACT ACTION: review Xsuper renderer
  color/UV/alpha at162C314 against native sprite adapter; link/stage if matched.
  NEXT FILE TO OPEN: ZzzEffectParticle.cpp render and GrowLancerSpriteAdapter.h.
  NEXT FUNCTION TO IMPLEMENT: only a proven private Xsuper render gap.

- Shining Peak pin renderer/field audit: selected update and renderer have no
  direct+A44 reads; do not infer an external mapping. ReverseUV3 formula and
  default face3/color paths match native. Added pinned12instruction/8constant
  verifier plus scoped operand check; PASS. Linked/staged pin correction.
  REVERSE EVIDENCE: SHINING_PEAK_REVERSE.md follow-up;1619524..1619580;
  selected update1612963..1612CB8. CHAIN COVERAGE: local pin update/render
  reviewed; indirect external state/initial geometry/runtime remain OPEN.
  ASSET PROVENANCE: unchanged79 overlay. FILES MODIFIED: pin verifier,stage
  verifier,reverse doc,acceptance matrix,this state.
  LAST STATIC CHECK: pin verifierPASS; stage2658base/79overlay/3root files,
  EXE/Player SHA/CRC PASS. LAST BUILD RESULT: isolated Win32 linkPASS,
  known cryptlib LNK4099; linked/staged SHA256:
  6653FBC797BCCA6C13B5E3FABEDDF8911687F82800FB5F8215274D35E99812E3.
  Backup RuntimeQA/Client/Engine-Port S21.exe.before-pin-
  74b8e480d5d243379ca3cc92bf406558.bak preserves previous candidate.
  LAST RUNTIME QA: no new gameplay. KNOWN DIFFERENCES: external+A44 semantics
  unproven, not needed by reviewed direct local path; no fabricated mapping.
  MISSING CHAIN PARTS: remaining primitive contracts/new class/GS/visual gates.
  BLOCKER: none for offline reverse. NEXT EXACT ACTION: audit Shining Peak
  8100/sub1 particle ctor/update against native; check tick and emission order.
  NEXT FILE TO OPEN: ZzzEffectParticle.cpp and SHINING_PEAK_REVERSE.md.
  NEXT FUNCTION TO IMPLEMENT: proven private particle mismatch if any.

- Shining Peak8073 subtype2/3/4 was wrongly using native subtype0 behavior.
  Recovered ctor15DE53A and update1612963..1612CB8; implemented private
  GrowLancerPinJoint.h, whole ticks/native CreateTail, no FPS tail-cap scaling.
  REVERSE EVIDENCE: SHINING_PEAK_REVERSE.md correction section; life20,
  velocity60,MaxTails18,reverseUV3,angleX-135,dark onset then(.6,.6,1)*.88,
  subtype-specific pitch schedule. CHAIN COVERAGE: constructor/update selected
  branch corrected; S21 word+A44=20 consumer and renderer still OPEN.
  ASSET PROVENANCE: unchanged native pin_lights asset.
  FILES MODIFIED: GrowLancerPinJoint.h,ZzzEffectJoint.cpp,Main.vcxproj/filters,
  GrowLancerPinJointTest.cpp,Circle renderer verifier guard,reverse doc,
  acceptance matrix,this state. LAST STATIC CHECK: x86/W4/WX test12casesPASS,
  project3cpp/16headersPASS,Circle verifier31pinsPASS.
  LAST BUILD RESULT: QA Win32 ClCompile PASS; no new link/stage this turn.
  LAST RUNTIME QA: unchanged isolated1C739FDD...98CB0C8B candidate.
  KNOWN DIFFERENCES: pending word+A44 consumer; do not invent native mapping.
  MISSING CHAIN PARTS: pin-field/renderer/initial geometry,remaining skills'
  runtime/class/GS acceptance. BLOCKER: none for offline reverse.
  NEXT EXACT ACTION: find8073-related reads of word+A44 in joint update/render
  and audit reverseUV3/color branch before link/staging Shining Peak fix.
  NEXT FILE TO OPEN: ZzzEffectJoint.cpp RenderJoints and pinned renderer1618F44.
  NEXT FUNCTION TO IMPLEMENT: only verified remaining pin adapter behavior.

- Circle auxiliary-pool concern resolved:10F0332 predicate only accepts
  type7FCE, so80E3 returns0 and15C6610 uses fixed500 pool. Earlier generic
  renderer auxiliary-pool observation does NOT imply Circle overflow behavior.
  S21 fixed first-dead scan/full return matches native MAX_JOINTS500.
  Completed non-blur Z geometry audit15E3B6C..15E3DD0: +/-Scale*.5,
  supplied matrix,Position,vertices2/3 agree with native CreateTail.
  REVERSE EVIDENCE: CIRCLE_SHIELD_REVERSE.md fixed-pool section and pinned
  verifier31 instructions/constants1/.5. CHAIN COVERAGE: local pool selection
  and non-blur four-vertex construction reviewed; startup/external reset and
  actual pool stress still OPEN. ASSET PROVENANCE: unchanged.
  FILES MODIFIED: reverse doc,renderer verifier,this state.
  LAST STATIC CHECK: verify_circle_joint_renderer.py PASS31 instruction pins,
  constants1/.5, Circle-only tail-cap source guard and fixed500 pool predicate.
  LAST BUILD RESULT: unchanged1C739FDD...98CB0C8B candidate (no runtime edit).
  LAST RUNTIME QA: no new gameplay. KNOWN DIFFERENCES: no Circle-specific
  overflow difference established; previous note superseded.
  MISSING CHAIN PARTS: startup/reset/reuse evidence,owner visual/class/GS gates.
  BLOCKER: none for offline analysis. NEXT EXACT ACTION: audit another skill's
  FPS-sensitive primitive allocation/update pairing, starting Shining Peak joints.
  NEXT FILE TO OPEN: GrowLancer/SHINING_PEAK_REVERSE.md and runtime emitter.
  NEXT FUNCTION TO IMPLEMENT: any proven Shining Peak private tick mismatch.

- Found/fixed Circle tail-length FPS conflict: native constructor post-switch
  divided MaxTails10 by FPS despite new whole-tick updates. S21 epilogue
  15E21B9 only caps50. Bypassed division ONLY ForcePillar/subtype2.
  REVERSE EVIDENCE:15DEC9C->15DEF1C->15E21B9; reverse doc tail audit.
  CHAIN COVERAGE: non-blur tail count/shift/XY geometry reviewed against
  native; Z pair and allocation/reset/overflow still need completion.
  ASSET PROVENANCE: unchanged79 overlay. FILES MODIFIED: ZzzEffectJoint.cpp,
  verify_circle_joint_renderer.py,verify_runtime_qa_stage.py,reverse doc,
  acceptance matrix,this state.
  LAST STATIC CHECK:21 pinned renderer/epilogue instructions and source guard
  PASS; project3cpp/15headers PASS; stage2658+79+3/hash/CRC PASS; diff checkPASS.
  LAST BUILD RESULT: isolated QA Win32 build/link PASS, cryptlib PDB warnings.
  Linked/staged SHA256:
  1C739FDDBD8623789EBFB052467CA9E9A0E5114C16E5568D40739AA598CB0C8B.
  Previous B89C candidate preserved in RuntimeQA/Client as
  Engine-Port S21.exe.before-tail-cap-088feac533eb458da4165b1c550b4052.bak.
  LAST RUNTIME QA: no new gameplay. KNOWN DIFFERENCES: fixed native pool vs
  S21 overflow; storedScale initial-tail behavior must not be guessed away.
  MISSING CHAIN PARTS: Z pair,allocation reset/overflow,real pool/visual/new
  class/GS authority. BLOCKER: none for offline evidence work.
  NEXT EXACT ACTION: finish non-blur Z pair then trace allocation before
  15C6D4F storedScale read; compare reuse without global native changes.
  NEXT FILE TO OPEN: ZzzEffectJoint.cpp CreateJoint/CreateTail.
  NEXT FUNCTION TO IMPLEMENT: only proven private initialization mismatch.

- Circle joint renderer reviewed:1618F44 live/tail/pass gates,1619089
  additive ONE/ONE,1619640 base UV,161AA94 inverseU and161ABE2 face2 quad
  agree with native RenderJoints for the scoped Circle chain. S21 color
  multiplier+A4C initializes1 at15C6858 and is unchanged by reviewed Circle
  constructor/update. No speculative renderer edit made.
  REVERSE EVIDENCE: GrowLancer/CIRCLE_SHIELD_REVERSE.md renderer audit;
  verify_circle_joint_renderer.py PASS19 pinned instructions+scalar1.
  CHAIN COVERAGE: reviewed local renderer branches; external multiplier writes,
  overflow manager and tail-construction equivalence remain unverified.
  ASSET PROVENANCE: unchanged. FILES MODIFIED: renderer verifier,reverse doc,
  this state only. LAST STATIC CHECK: pinned renderer verifier PASS.
  LAST BUILD RESULT: unchanged B89C09F6...BF4E57 linked/staged candidate.
  LAST RUNTIME QA: no new gameplay; owner acceptance remains OPEN.
  KNOWN DIFFERENCES: S21 renderer has an overflow-pool visitor; native uses
  fixed pool. No capacity/overflow policy has been changed without evidence.
  MISSING CHAIN PARTS: CreateTail non-blur/allocation, overflow/pool stress,
  full nine-skill visual/class/GS gates. BLOCKER: none for offline reverse.
  NEXT EXACT ACTION: trace CreateTail15E2756 -> non-blur15E37EF and compare
  native CreateTail/allocation initialization before extending cleanup claims.
  NEXT FILE TO OPEN: ZzzEffectJoint.cpp CreateTail.
  NEXT FUNCTION TO IMPLEMENT: only an evidenced private tail/pool discrepancy.

- Circle ForcePillar subtype2 ordering recovered from generic S21 manager:
  old-velocity move, accel2, homing10 at life18..11, pre-turn matrix tail,
  decrement and kill below0. Added private per-slot whole ticks/reset-on-alloc;
  no legacy subtype/global FPS modification. x86 /W4 /WX regression PASS
  four FPS factors,19 updates/8 turns,ordering,expiry and reuse contract.
  REVERSE EVIDENCE:15E5838..15E5A2A,1612F8F..1613037,
  1618ADA..1618B67,1327059..132729F; pinned offline module only.
  CHAIN COVERAGE: Circle controller/Shiny/child/joint timing corrections linked;
  GPU joint renderer and actual pool/visibility comparison still OPEN.
  ASSET PROVENANCE: unchanged79 overlay; native ForcePillar hash in reverse doc.
  FILES MODIFIED: ZzzEffectJoint.cpp,RISE/GrowLancerCircleJoint.h,
  Tests/GrowLancerCircleJointTest.cpp,Main.vcxproj/.filters,
  tools/grow_lancer/verify_project_integration.py,verify_runtime_qa_stage.py,
  GrowLancer/CIRCLE_SHIELD_REVERSE.md,ACCEPTANCE_GATE_MATRIX.md,this state.
  LAST STATIC CHECK: project/filter inclusion3cpp+15headers PASS; stage2658
  base+79overlay+3rootData PASS; server contracts/catalog profiles PASS;
  git diff --check PASS (CRLF warnings only).
  LAST BUILD RESULT: isolated QA Win32 Build/link PASS; known cryptlib LNK4099.
  Linked AND staged Engine-Port S21 SHA256:
  B89C09F6F71275F490364BEBD447831CB55A72DB9917EA9205E97DBA07BF4E57.
  Staged previous EE7F preserved as Engine-Port S21.exe.before-circle-
  aa7283e5f00d4e849365f871fe9f59ad.bak in isolated RuntimeQA/Client.
  LAST RUNTIME QA: NOT RUN for this candidate; no automatic game launch.
  KNOWN DIFFERENCES: fixed-tick compatibility verified in helper, not GPU parity.
  MISSING CHAIN PARTS: Circle joint renderer/pass/UV and real pool stress;
  other nine-skill runtime gates/new class/authoritative GS remain open.
  BLOCKER: none for offline reverse; external GS/runtime gates unchanged.
  INTEGRATION AUDIT: local project includes all GL compilation/header units;
  dirty/untracked code is NOT git-merged. No protected source/Pegasus writes.
  NEXT EXACT ACTION: trace S21 joint render80E3 reverseUV2/face2/blend and
  compare native RenderJoints before changing any renderer behavior.
  NEXT FILE TO OPEN: ExMain_RISE_PC/Main5.2_RISE/ZzzEffectJoint.cpp.
  NEXT FUNCTION TO IMPLEMENT: only verified Circle-private renderer gap,
  if found; do not guess or modify global SS6 joint rendering.

- Circle Shiny default sprite UV/rotation mapping closed: metadata1/1/1
  selects full UV at172732D->17274DA; mode4 sets Z rotation only at18E8EDB,
  matching native RenderSprite Angle(0,0,Rotation). No adapter needed for those
  parameters; mode4 is not blend (subtype0 controls additive separately).
  NEXT EXACT ACTION: verify visibility producer/owner deletion or move to
  remaining joint contracts; current Circle changes still await link/stage/QA.

- Circle sprite allocator metadata mapped (mode4 and default1/1/1), manager
  additive subtype0 and pass0/2 cleanup traced. Found native invisible sprite
  multiplier increases instead of S21 decrease; fixed only CircleShiny bitmap
  in zzzeffectsprite.cpp, no global SS6 behavior change. NEXT EXACT ACTION:
  compile this correction; audit metadata UV/draw mode consumer and flare01
  identification before extending any compatibility path. Visual still OPEN.

- Circle emission gates extracted into shared runtime helpers; x86 /W4 /WX
  regression PASS exact pair24..20 and child18/15/12 sequences at fourFPS
  factors,31 controller updates/one cleanup. Subtype1 local offset verified
  157654B..157659A matches subtype0 (0,-50,110). Descriptor corrected stale
  flare CreateParticle to CreateSprite and wrong bone-origin/pair description.
  NEXT EXACT ACTION: compile shared-gate integration, then audit Circle sprite
  metadata/pass and remaining joint/owner behavior. No visual gate upgrade.

- Circle controller emission audit found over-emission: child incorrectly
  spawned every frame; extra bone Shiny/flare pair absent in S21. Corrected
  pair window20..24, child ticks18/15/12 only; child position uses controller
  rotated(0,-50,110), not bone result. Controller Timer0/whole ticks eliminates
  FPS-dependent counts. Evidence1575B60..15766EE. NEXT EXACT ACTION: focused
  emission-count regression and compile; recheck subtype1/full owner transforms.

- Circle Shiny renderer owner corrected to self from15AEAA4, retaining
  Light*Alpha/position/scale/angleZ. Child circle_shields now uses whole-tick
  lifecycle with Timer0; removed non-source scale clamp after15766F3 audit.
  Isolated ClCompile PASS. Sprite wrapper extra arguments/metadata and controller
  emission timing remain open. NEXT EXACT ACTION: audit Circle controller frame
  windows and per-tick emissions; staged EE7F candidate unchanged this turn.

- Circle Shield Shiny04 audit found non-source alpha clamps in native port.
  Removed both clamps, added private whole ticks/Timer reset and shared helper.
  x86 /W4 /WX test PASS fourFPS cases (peak1.2,16 updates,end.2) plus negative
  branch; isolated ClCompile PASS. Added new header to project and filters.
  Last staged EE7F... remains unchanged (Circle correction not linked/staged).
  NEXT EXACT ACTION: audit Shiny04 renderer dispatch from15A1514 and complete
  Circle controller/child lifecycle review. All9 gate remains IN_PROCESS.

- Guarded stage complete for current primitive-correction candidate:
  EE7F2170928DE17AFDE3922F0B00CF106907E7B861E400F67C47190EA5EBCDB9.
  Verified no running Engine-Port S21 before copying; previous EXE retained
  as .before-spin-terrain-4de20c7f90984f21b8669bb772b1586d.bak beside test EXE.
  RuntimeQA verifier PASS2658 base,79 overlay,3 root Data and hashes/CRC.
  Fixed stale acceptance request to choose class: new Grow Lancer already chosen.
  This is isolated integration/staging, NOT a merge to protected base/Pegasus.
  NEXT EXACT ACTION: continue unresolved full-chain/owner and class/server work;
  no visual or server authority gate upgraded by staging.

- Full isolated RuntimeQA client Build/link PASS after hole white-RGB and
  terrain blend edits (existing cryptlib missing-PDB warnings only).
  Last staged EXE is still older; NEXT EXACT ACTION is helper regressions and
  guarded candidate hash/staging, not production deployment. No runtime PASS.

- Hole8003/sub0 ctor/update/renderer recovered; mapped to shared proven flare
  envelope and terrain renderer, separate texture. Corrected hole caller RGB
  from blue to white using157CD85 and157D247. Added explicit native additive
  enable/disable for ground/flare/hole, matching S21 render state calls.
  ClCompile PASS before final white-RGB edit. NEXT EXACT ACTION: compile/link
  all recent primitive fixes, run helper regressions, hash-pin/stage isolated
  candidate; visual QA and full9-skill/class/server gates remain IN_PROCESS.

- Ground initial scalar resolved:143E71F invokes reset1315E97, which zeros
  B4 at131607C. Implemented private Spin ground ctor/update/terrain rendering,
  whole-tick expiry; native Distance stores multiplier, StartPosition stores
  initial RGB only for this carrier. Zero-area draw skipped because native
  terrain divides by size. x86 /W4 /WX helper PASS four FPS factors, preserved
  RGB and11 update/one cleanup; ClCompile PASS before final zero-size guard.
  NEXT EXACT ACTION: compile final guard, audit additive render state and hole
  handlers, then link/stage. Terrain/culling/pass and visual parity still open.

- Recovered Spin ground8145/sub1 ctor/update/renderer: lifetime10, alpha1;
  size multiplier(11-life)*.1, alpha-=.1, RGB=preservedRGB*alpha; terrain
  renderer rotation0,alpha1,height5,flipVfalse. Generic carrier is wrong.
  Descriptor synchronized with implemented flare/particle contracts and
  pending ground correction (no false complete). NEXT EXACT ACTION: resolve
  initial ground scalarB4 and lifecycle; then implement private ground mapping.
  Ground evidence:1474E38,152894A,15289F2,15AACE9. No protected writes.

- Implemented private Spin particle7FDD/sub4 whole-tick update: movement
  before velocity jitter, alpha decay and rotation, then lifetime decrement.
  Native AngleMatrix/VectorRotate/VectorAdd avoids MovePosition FPS double
  scaling. Verified S21 initial movement1/rotation0/velocity0. Per-slot
  accumulator resets for every allocation; subtype5 and Breche stay separate.
  New shared helper test PASS16 lifetime/FPS/movement cases (/W4 /WX,x86),
  native isolated ClCompile PASS. No link/stage yet. Full visual parity open.
  NEXT EXACT ACTION: audit owner snapshot use and matrix mapping, then Spin
  hole/ground carrier handlers; link/stage after remaining primitive fixes.

- Spin particle7FDD/sub4 renderer now multiplies Light*Alpha, matching
  1628EAA..1628FEA. Discovered missing native update:16B2472..16B25D8
  alpha-=1/maxLife, independent XYZ velocity jitter and rotation rand()%16.
  Common pre-dispatch movement16AA6F1 and post-body lifetime172343C recorded.
  NEXT EXACT ACTION: verify constructor movement defaults and13272A0 before
  private whole-tick particle implementation. Prior flare renderer ClCompile
  PASS; particle renderer compilation pending. No link/stage this checkpoint.

- Implemented Spin flare subtype0 constructor/update in shared helper and
  opted only private flare into whole ticks. x86 /W4 /WX test PASS12 rotation/
  FPS cases; isolated ClCompile PASS before renderer correction. Renderer
  recovered15A1559->15ACDB8->15ACE16: terrain bitmap1765DF1, not sprite.
  Corrected native flare to existing terrain adapter, alpha1,height5,flipVfalse.
  No new EXE staged this turn; last staged hash remains3CCB32F8...E4041FD.
  NEXT EXACT ACTION: compile renderer correction, then audit particle7FDD/sub4
  and Spin hole/ground constructors/update/renderers. Full9-skill IN_PROCESS;
  server authority, new class and owner visual acceptance still incomplete.

- RuntimeQA staging verifier PASS after Spin cross build:2658 base files,
  79 overlay files,3 root Data files, executable/player hashes and CRC.
  Offline Spin flare subtype0 ctor/update now recovered (143F30D->1481CFE,
  1574C55->157D400). Generic carrier disagrees: needs initial alpha0,
  absolute rand()%180 and incremental +/-0.2 through life0, not symmetric
  sampled envelope. Recorded in SPIN_STEP_REVERSE.md; implementation pending.
  NEXT FILE TO OPEN: ZzzEffect.cpp:6927 (private whole-tick selection), then
  GrowLancerEffectRuntime.cpp carrier constructor/update. No live read needed.

- Spin cross math now shared by native runtime and GrowLancerSpinCrossTest:
  x86 /W4 /WX PASS eight scale/FPS cases (12/17, .25/.5/1/2), grow-to-shrink
  boundary, life-zero update and single destruction. Not renderer/visual QA.
  Added header to both Main.vcxproj and filters; full isolated RuntimeQA
  Build/link PASS (existing cryptlib missing-PDB warnings). Staged executable
  SHA256 3CCB32F80E6496E63988786EA852A1470B39464ABFC9B1B8935A68FD7E4041FD
  after no-running-QA-process check and recoverable adjacent EXE backup.
  This build includes the recent Spin target/cross corrections. No protected
  branch merge or production deployment performed. Nine-skill gate IN_PROCESS.
  NEXT EXACT ACTION: reverse remaining Spin flare/hole/ground carrier primitive
  constructors, updates and renderers; generic carrier envelope is not parity proof.

- Resolved Spin cross renderer contradiction: registrationA1B334 binds5F9
  toA49FEF; A1B246 binds fixed-RGB callbackA49422 to5EB, not cross. Full
  cross callback uses storedLight*Alpha, object UV,mesh0/42,blend0/alpha.
  Native renderer already matches; corrected stale descriptor, not correct
  runtime code. Added pinned registration-instruction verifier; PASS with
  descriptor JSON parse. No build/stage/visual claim. Next: shared cross
  lifetime envelope regression tests, then scoped subtype/flare effect audit.

- Spin cross update audit corrected reversed scale envelope: S21 ADDSS157D38B
  while life>5 and SUBSS157D3BE otherwise. Removed non-source scale clamp,
  opted only private cross type into audited whole ticks and reset Timer.
  Controller sub1/2 now assigns rand()%180 angle and alpha0; sub1 flare/five
  particles use verified WHITE RGB instead of inferred blue. Native ClCompile
  PASS; descriptor/reverse correction saved. No relink/stage/visual PASS.
  Next: resolve contradictory cross renderer descriptions (A49FEF versus
  descriptor A49422 fixed RGB) at actual registered callback before changing
  submitted light; then cross envelope regression tests.

- Closed Spin Step retained-target chain packet7/8 ->masked key ->lookup
  -1978 ->wrapper+34 ->metadata+8 ->effect+364 ->update target resolve.
  Corrected native fanout from caster to current target position/angle;
  subtype1/crosses target-owned, ground null-owned. Root keeps caster for
  weapon blur; CreateEffect stores native m_sTargetIndex and caster scale.
  Updated receive/QA callers to require target index; all call sites checked.
  Invalid/dead target skips fanout with consumed one-shot; no caster fallback.
  Native ClCompile PASS, docs/descriptor updated. No relink/restage or visual
  symptom-resolution claim. Next: audit cross lifetime/renderer and subtype1
  initial angle/alpha, then regression tests and isolated runtime staging.

- Spin Step source audit found concrete mismatches: cross ctor1481CED stores
  absolute120 degrees, while native added120; corrected assignment. Three
  cross calls pass identical angles; removed invented i*120 spread. Frame
  gate157CA97 is JBE skip, hence >4.7 and <=6.5; corrected inclusive lower
  bound. Float constants read directly from pinned dump. Updated descriptor
  bounds/evidence; native ClCompile PASS. No relink/stage or ring-only visual
  fix claim. Next: trace retained index+364 producer to correctly place
  subtype1/cross/ground fanout on its resolved actor rather than assuming caster.

- Updated stale Breche descriptor from unimplemented lead to actual target
  root/five children, IDs/scales/RGB/owner/timing/render contracts and explicit
  remaining QA gates. Added3 ground textures to asset_provenance.json with
  verified source/staged SHA/size; marked historical Item1130 texture excluded
  from Breche (file retained). Updated acceptance matrix to79-file overlay
  and Breche IMPLEMENTED/QA OPEN, not complete. Server catalog comment now
  reflects owner-selected new class, rather than asking recipient choice again.
  JSON/source/staged hash checks and server contract verifier PASS. No new
  server activation, build, staging or visual acceptance in this checkpoint.
  Next: compare remaining8 descriptors with native entry/update/render paths;
  retain fail-closed server learning until new class and semantics are proven.

- Extracted Breche ground tick math into runtime-shared header and added
  x86 /W4 /WX harness:48 combinations of3 layers,4 scales,4 FPS factors PASS.
  Covers life11/10 boundary, alpha overshoot/no clamp, scale/rotation,
  update at life0, one destruction and no updates after death. Native
  GrowLancerEffectRuntime.cpp ClCompile PASS. Existing effect/particle/sprite
  harness binaries rerun PASS (limited helper scopes, not GPU/pool QA).
  Integration audit found5 helper headers missing IDE/project inventory;
  added Tick/FireParticle/SpriteAdapter/TerrainAdapter/BrecheGround to vcxproj
  and filters. They were transitively compiled; now explicitly inventoried.
  No relink/restage this checkpoint: staged EXE remains6CEF9A88...6BED25.
  Next: audit9 skill descriptors against actual dispatch/resources/GS wiring
  and add concrete missing integration tests; visual acceptance remains OPEN.

- Connected native ReceiveMagic skill279 contact to resolved target (*to),
  keeping native packet19 layout and7FFF target mask. Added short/null packet
  rejection and missing-source guard before character-array access (lookup
  returns MAX_CHARACTERS_CLIENT on miss). QA F9 remains separate local test.
  Full Win32 Global Release build/link PASS, pre-existing LNK4099 only.
  Built/staged Engine SHA256
  6CEF9A88EB77B19EF273A55D57FB8CF0972376CA4678CDEF4FAE6922FA6BED25.
  No QA process running before staging; private directory reparse guards
  checked. Prior QA exe preserved as .before-breche-6afcad604e3349789270780d6e493b84.bak.
  Copied only3 proven Breche textures and new EXE into isolated runtime client.
  Stage verifier initially rejected old executable hash; updated pin to
  verified build hash (not weakened). Full stage recheck PASS:2658 base files,
  79 GrowLancer overlay files,3 root Data files, Engine/player hashes and CRC.
  Server/icon/catalog/S21 dispatch static verifiers PASS; no runtime visual
  PASS or git merge claim. Next: finish stage verification, focused lifecycle
  tests and nine-skill integration audit; no protected-source deployment.

- Implemented native Breche carriers within existing32-model private tail:
  root26, marks27, twilight02/28, twilight01/29, emitter30. Root creates five
  children with target/root.Owner (not pointer to root), exact scales/RGB,
  and integer-tick20-life updates. Ground renderer uses private textures,
  storedLight*Alpha and recovered flipV/explicit-alpha distinction. Emitter
  uses native TransformByObjectBone, two sprite/fire emissions per tick;
  bounded invalid-bone guard skips undefined source position behavior.
  Connected QA F9 contact279 to CreateBrecheHit, with separate resource gate.
  ClCompile x86 PASS after changes; no link/stage/runtime acceptance yet.
  Sprite FPS/visibility compatibility and terrain-world eligibility remain
  explicit QA differences. Source receive279 still action-only: next inspect
  validated target path before adding CreateBrecheHit there, then tests/link.
  Integration audit: three runtime CPP files are in Main.vcxproj; isolated
  outputs and disabled deployment guards exist. Work is still uncommitted;
  do not report git merge or full integration complete. User asked to keep
  working while asleep and audit all integration; protected branches untouched.

- Staged three proven Breche ground-layer textures (93214 bytes total) into
  isolated build Client/Data/RISE/GrowLancer/Breche with source/output SHA
  verification. Added private native texture IDs32967..32969 and separate
  EnsureBrecheBitmaps gate, so missing new assets do not disable other skills.
  Uses native bitmap loader, GL_LINEAR/GL_CLAMP as registration18BD40C,
  18BD42C,18BEFD8. ClCompile Global Release/Win32 PASS; git diff --check PASS
  (existing line-ending warnings). No link/live staging or visual PASS.
  Next: allocate private control carriers within existing32-entry model tail,
  then root constructor/four ground records/emitter using audited tick contract.

- Authorized full dump completed (1395943076 bytes,8 seconds), no automatic
  retry. Offline Memory64 parse4511 regions; model manager now readable.
  Bounded cache lookup5FD finds model3B3492E0 with0 bones/meshes/actions.
  Added pinned inspect_breche_full_dump.py, check PASS. Root asset question
  closed for this snapshot: empty control model, no invented BMD required.
  Recorded SHA/path/addresses in BRECHE_REVERSE.md. Live main21072 was still
  responding at post-capture check; not a long-run stability or visual PASS.
  Next: native Breche root lifecycle and five children using recovered contracts;
  do not continue root-BMD inventory. Owner goal now explicitly enough9 skills
  or usage exhaustion; full acceptance remains unproven and IN_PROCESS.

- Owner explicitly approved one full-dump attempt after pause/crash warning.
  Added capture_s21_full_once.py and Run-S21-Full-Dump-Once CMD/PS launcher.
  Uses system DbgHelp MiniDumpWriteDump flags1802 (full memory/info/thread),
  exact S21 path/SHA, query+read-only process rights,5GiB free guard, exclusive
  authorization marker and unique isolated outputs. No automatic retry,
  hook, target write, protection disable or ignore-inaccessible-memory flag.
  CLI help and PowerShell syntax PASS; capture NOT executed by agent.
  Next owner action: one Breche cast, then owner-run elevated full-dump CMD.
  Snapshot may show persistent cache, NOT transient effect timeline; parse
  resulting dump and validate actual heap coverage before drawing conclusions.

- Checked two additional existing S21 crash dumps from20260908 (39/41
  captured segments): neither contains model-manager global1E04C8C; both
  partial, no Memory64 stream. No usable heap recovered. Unrelated dumps
  excluded. Existing files do not resolve runtime model-cache question.
  Next requires owner-coordinated full-dump attempt with crash/pause risk
  acknowledged; no new live reader or capture launched automatically.

- Offline crash-dump cache coverage audited: SHA ce8c1c72...963f5dbb,
  788 memory segments, no Memory64 stream.968B22 returns pointer stored at
  1E04C8C (17952650 here), NOT the singleton's address. Actual manager heap
  is absent from captured ranges; cache presence/absence cannot be determined.
  Saved exact layout/coverage evidence in S21_LIVE_CRASH_20260913.md.
  No repeat live reading or fabricated zero-filled cache. Need a suitable
  heap evidence source for runtime5FD lookup; partial crash dump is insufficient.

- Owner no-reader baseline stayed stable. Located existing local crash dump
  main.exe.1224.dmp (230244175 bytes), parsed offline without touching live
  S21. Exception thread8136/EIP10A06765 matches IGC+0xD6765; anchored code
  explicitly executes INT29 withECX2. Caller frame-pointer lead1095EA9B,
  incomplete unwind; trigger still unresolved, no detector claim.
  Crash dump includes non-image memory, allowing offline cache investigation
  without repeating suspect live reads. Evidence in S21_LIVE_CRASH_20260913.md.
  Next: validate offline model-cache layout and caller boundary; no live
  capture armed, no cast requested, no original-client modifications.

- Owner reported crash after preflight PASS. Confirmed process exited and
  Application event1000 at01:22:44 identifies PID1224, IGC.dll+0xD6765,
  exceptionC0000409. Preflight saved01:22:26 (18 seconds earlier).
  MuError.log also contains ResourceGuard data mismatch/missing-file messages;
  neither timing nor these messages proves cause. Saved incident evidence in
  GrowLancer/S21_LIVE_CRASH_20260913.md. No hook or cast capture was armed.
  Next: coordinate no-reader baseline with owner before another read/cast run.

- Owner-run elevated preflight succeeded for current main PID1224.
  artifacts/grow_lancer/live_preflight/preflight-8144363798224e30958b1594a4c0ebff.json
  reports PREFLIGHT_PASS: exact S21 executable path and four32-byte function
  prefixes EF2A81/12C4F86/143EECA/14B76C0 match pinned dump. Process inventory
  confirms PID1224 exists. This resolves access for the owner-run elevated
  helper, not the medium-integrity agent shell. Capture is NOT armed; no
  Breche cast or model/effect/visual acceptance is implied. Next: prepare
  bounded read-only runtime collector using the same owner-run path, then
  signal readiness only after collector startup has been verified.

- Prepared owner-run read-only preflight CMD/PowerShell launcher, so restarting
  all of Codex is unnecessary. No automatic elevation or execution-policy
  bypass. Elevated script selects exactly one process by full S21 path,
  runs the pinned Python checker, saves uniquely named JSON in isolated
  artifacts/grow_lancer/live_preflight. PowerShell/Python syntax checks PASS;
  elevated execution NOT tested and no cast recorder armed. Current process
  inventory now contains no main.exe: prior5924 is gone, do not reuse PID.
  Next owner action: open S21 normally, run Run-S21-ReadOnly-Preflight.cmd
  via Run as administrator; inspect saved result before runtime capture work.

- Live-capture access blocker confirmed for third consecutive goal turn:
  PID5924 remains running, title MU - Powered by IGCN; read-only preflight
  still returns WinError5. Owner explicitly chose live Breche evidence;
  capture cannot proceed without access change. No duplicate launch, bypass,
  hook or cast requested. Goal marked blocked awaiting owner elevation;
  skill acceptance remains IN_PROCESS, not complete. Resume by revalidating
  current PID/path and running preflight before preparing capture.

- Added tools/grow_lancer/preflight_s21_live.py: pinned client/dump SHA256,
  exact process path, read-only1010 handle and four anchored32-byte code
  comparisons before any future pointer capture. No privilege adjustment,
  injection, suspension or memory/file writes. NOT a cast recorder.
  Rechecked PID5924: preflight NOT_READY, WinError5; shell still Medium.
  Prior token query confirmed target elevated=1. User action to reopen Codex
  elevated remains pending. Client on-disk SHA256 is
  f42ed0613ea0cd19d24c43c4168d1936adeb0c054b306973bd0afe6e364ccbd9.
  Next: rerun preflight for freshly verified PID after access changes, then
  arm bounded read-only model/effect capture. Do not request a cast yet.

- Owner requested live S21 Breche capture instead of further static-only
  model investigation. First direct launch reported missing Data/Local/Mix.bmd;
  file exists (2946488 bytes), likely working-directory issue, not proven.
  Subsequent main PID5924 has title MU - Powered by IGCN. Duplicate-launch
  guard prevented a second instance. Read-only OpenProcess query-limited
  succeeds, VM_READ fails Windows error5; combined1010 also fails.
  Current shell integrity is Medium. No capture armed, no cast evidence,
  hook, memory write, binary patch or production change. Live memory capture
  needs resolution of process read access; do not tell owner it is ready.
  Next exact action: resolve read access with owner, then verify binary identity
  and arm bounded model/effect capture before requesting Breche cast.

- Audited all4950 records of pinned S21 Data/Local/item.bmd using recovered
  708-byte layout, XOR and checksum: keys0..10759, no duplicates/negative keys;
  root5FD config key-4EC absent. Added verify_breche_model_config.py with
  full-file SHA pin and explicit on-disk-only scope; verifier PASS.
  This narrows root model provenance but does not prove live cache contents.
  Next exact action: trace model-cache registration/population for5FD.
  Next file: GrowLancer/BRECHE_REVERSE.md; function to inspect: EF2A81 cache
  insertion and its registration counterpart. No runtime function enabled,
  no build/stage or live executable overwrite; DK QA remains unchanged.

- Traced configuration producerD0506D: count,2C4-byte XOR-decoded records,
  trailing check, insertionD07635. Key is decoded DWORD0, not WORD; negative
  key exclusion still needs data. Config map head26CC53A0 is outside image
  coverage. Sole raw caller lead0E0E512E is a trampoline, filename unresolved.
  Evidence saved in BRECHE_REVERSE.md; no guessed decoder/asset or runtime
  modification. Next: resolve filename/trampoline or existing config payload,
  inspect keys to decide5FD cache behavior. No build/runtime PASS added.

- Closed model-manager miss behavior: absent config creates and caches a
  zeroed D0 model for valid runtime IDs. Nonnull does not prove mesh/BMD.
  For5FD config lookup key is-4EC; its absence is not yet proven because
  D06184 is a map lookup. Zero-action PlayAnimation returns true without
  frame advancement. Evidence saved; no inferred5FD asset or runtime PASS.
  Next: configuration insertion/key domain and5FD cache population; decide
  empty control root versus loaded model from evidence before implementation.
  No build or live binary change this checkpoint.

- Traced176D621 wrapper: Calc-like1886382 must return true before draw
  1887B8B. It rejects Alpha<0.01 and missing model; default route does not
  prove visible mesh. Rejected raw5FD text/cleanup/registration-list leads
  as BMD provenance; adjacent crasha models5FB/5FC cannot be substituted.
  Evidence saved in BRECHE_REVERSE.md; no code/build/runtime change.
  Next: model manager cache/lazy resolution for5FD, determine whether root
  has a populated model or control-only model using pinned runtime structure.

- Closed root5FD generic animation-call route and default render dispatch:
  model PlayAnimation at1574861, default renderer15BD5DC ->176D621.
  No dedicated root render case in this table; do not mistake default for
  invisible root. Added bounded render table check to verifier. Extracted
  model registry lacks1533; binary registration still required, no guessed BMD.
  Next:176D621 shared render internals and5FD model registration/initial fields.
  Build/runtime unchanged; live DK QA binary not overwritten.

- Traced all four direct offset-manager creation calls to a separate receive
  list and character-state reconstruction (12ED574/13F4E96), not Breche
  dispatch. Inputs come from character visual configuration322E/322C or
  alternate3234/3230. Closed the dependency boundary: this auxiliary record
  is not a skill child. Preserve native target positioning; S21-only target
  configurations remain explicit compatibility/QA cases, not global offsets.
  Evidence in BRECHE_CHILDREN_REVERSE.md; no build/runtime change.
  Next exact action: return to root5FD primary update153542A ->1538599
  ->1574472 and native root render mapping; no further generic-manager port.

- Resolved factory category2 variants1/2/3 to vtables sharing offset
  function1097AFE: reads auxiliary bone sit_pos_0, transforms into vector24,
  multiplies XYZ by auxiliary Scale. Other descriptor values have distinct
  virtual implementations; unsupported factory cases return null. Recorded
  configuration discriminator and explicit non-generalization in child audit.
  Static evidence only; no code/build/runtime change or live EXE replacement.
  Next: creator10C2FD4 callers identify equipment/config input; keep target
  positioning native and isolate unsupported configurations, not a global fix.

- Closed offset-record ownership:1089CD3 stores supplied character+3254
  as+18 and allocates auxiliary object+1C with Owner back to that character.
  Manager10C2FD4 inserts it under the same character key used by lookup.
  Update108A1BE saves character Position/Angle to the auxiliary object;
  flag+34 is Live AND NOT character+31E8. Thus bracket restores saved base
  position, optionally plus record offset, not Breche movement. Evidence in
  BRECHE_CHILDREN_REVERSE.md. No native code or live executable changed.
  Next: record factory10C464B and virtual+10 vector24 producer/configuration;
  classify which character configurations can carry this offset.

- Closed position-bracket predicate108AB01 and character-key lookup10C30B1.
  Offset is record/action-gated, not universal. Accepted action ranges:
  [38,50),[71,79),[103,111),[300,327),[335,342). Render additionally gates
  record+38 while update does not. Breche caster190 exclusion cannot imply
  target-owner offset0. Saved exact gates in BRECHE_CHILDREN_REVERSE.md.
  Checks: anchored disassembly only; no build/runtime/code activation.
  Next: record constructor links+18/+1C and flag/vector producers, using
  character-key membership to close target applicability before native mapping.

- Verified protocol difference: S21 bounded opcodeDF dispatches12FEB99
  ->12C4F86; native5.2 SkillManager emits19. S21 source3/4,skill5/6
  versus native skill3/4,source5/6; target7/8 in both. Receiver facing and
  source target-index writes corroborate secondary object's client target role.
  Do not transplant S21 packet bytes/offsets or infer server damage semantics.
  Added pinned dump SHA256 and opcode/direct-call checks to
  tools/grow_lancer/verify_breche_dispatch_tables.py; execution PASS.
  Evidence: GrowLancer/BRECHE_REVERSE.md. No client/server build or runtime
  change. Next: Breche owner position-offset bracket association and shared
  scale; native packet integration must use existing named fields/opcode19.

- Closed ordinary S21 state6 scene ordering through anchored18EE7E4:
  update18F2EBF -> characters13F3FB0 -> effects1595834 -> particles;
  later render18F60EF -> character loop13F3BA4 ->13DB9D7 -> cached pose
  evaluation1887965. Live/Visible gates verified; alternate virtual render
  branch remains outside this conclusion. Native update/render split matches,
  so do not inject an extra Breche animation evaluation. Identified additional
  native Calc_RenderObject matrix producer for non-default object paths.
  Evidence saved in BRECHE_CHILDREN_REVERSE.md. No code/build/runtime change.
  Next: close emitter owner/position-offset association and shared model scale;
  preserve native cached matrix sampling and existing DK QA binary meanwhile.

- Identified S21 Calc_ObjectAnimation counterpart1887965, including Alpha
  guard, owner scale/action/origin setup and cached/global bone selection.
  Calls132D35E with matching native frame/action/angle/Translate arguments;
  anchored character caller13DB9D7 selects mode2 for default player model.
  Closed matrix allocation/free helpers1316385/13163FB and rejected bulk
  cleanup/UI-member numeric leads as pose producers. Detailed evidence in
  BRECHE_CHILDREN_REVERSE.md. No runtime change or new visual PASS.
  Next exact action: callers13DB9D7 -> scene render ordering, compare other
  native writers, then resolve shared BodyScale at Breche emitter sampling.

- Verified S21 bone getter1316442 returns cached owner+318 matrices with
  model/index guards, without evaluating animation. Confirmed second scale
  writer142F5A3 precedes visibility rejection. Native Calc_ObjectAnimation
  writes owner matrices on the character render path, with an Alpha guard;
  effect updates run separately before rendering. Documented freshness risk
  without claiming exclusive writer or changing animation evaluation.
  Evidence: GrowLancer/BRECHE_CHILDREN_REVERSE.md cached-owner-pose section.
  Static check: anchored disassembly and native source inspection only.
  Build/runtime: not run this checkpoint; no live executable changed.
  Next: S21 owner bone-buffer writer and scene order, then remaining native
  matrix writers; emitter activation remains gated on that comparison.

- Found S21 character scale writer1424CD5: object+A0 ->selected model+74,
  with position/action copies beside it. This matches native MoveCharacter
  setup but does not prove last-writer identity. Closed108A8F5 body as a
  position-offset/copy bracket, not scale setup; applicability to Breche owner
  remains membership-dependent. Next:142F1E9 pose production/current bone
  matrices and selected-owner offset association. No guessed global offset.

- Anchored three S21 scene callers of effect pass1595834 and confirmed
  particle update16AA2ED follows via1620972. Found common10C343C(1/0)
  bracket forwarding flag to108A8F5; semantics still open. Earlier13F3FB0
  loops400 characters and writes visibility, giving a concrete character
  update anchor for scale/pose tracing. No gameplay-context label guessed.
  Next:13F3FB0 remaining loop and108A8F5 before scale producer conclusion.

- Confirmed968B22 returns global model manager, not per-owner model state.
  Anchored ordinary/secondary effect loops1595834 and10F027E: neither
  assigns owner scale/pose per effect. MoveEffect predispatch only copies
  owner position/terrain data before type dispatch. Narrowed remaining
  scale provenance to scene-level callers/character model+74 writers;
  world6 prepass remains unclassified. No guessed scale substitution.
  Next exact reverse: callers1595834 and S21 character scale writes.

- Verified native update ordering: all characters then effects then particles.
  MoveCharacter writes shared model BodyScale from each actor, so last writer
  can differ from emitter owner. S21 emitter has no branch-local scale setup;
  EF2A81 existing-model path only returns cached pointer, not owner scale.
  Recorded this concrete shared-state risk instead of blindly assigning scale.
  Next: S21 model scale producer before effect pass and owner-pose sampling.
  No runtime behavior changed or acceptance claimed in this evidence run.

- Mapped emitter bone transform to existing BMD::TransformByObjectBone:
  same owner/global matrix selection, null-relative translation, BodyScale
  and owner Position addition. No duplicate transform implemented. Need
  BodyScale/current fallback-pose provenance before wiring emitter; enforce
  positive bounded NumBones. Native DeleteParticle/map reset only clear Live;
  allocation reset path remains scoped. Extended sampler test to cover a
  queued ordinary sprite on the same texture before Breche state mutation.

- Added private GrowLancerSpriteAdapter.h: bind native texture, flush prior
  sprite batch, save wrap S/T, apply S21 CLAMP, render via native RenderSprite,
  flush before restoring both wrap values. Only tagged Breche particles opt in;
  no shared loader or global sampler default changed. Native shader queue
  stores vertices only, so flush boundaries are required. Added x86 mock-GL
  test: submission/state restore and exception path PASS. Real GL parity and
  per-particle flush overhead remain runtime QA items. Emitter not activated;
  no live binary replacement. Next: allocation/pool integration tests and
  root/emitter owner-bone chain completion.

- Closed S21 JPEG loader metadata: registration->18F8FA2->CC461D JPG
  selector->CC52F7, +214=3. Confirmed LINEAR filter and CLAMP2900 pass
  unchanged into GL WRAP_S/T. Native uses CLAMP_TO_EDGE: new known
  sampler difference, despite identical texture hashes. No shared sampler
  patch made. Next: scoped sampler/queue behavior and boundary compatibility,
  plus allocation/pool tests before emitter activation. Evidence-only run;
  no build/stage/runtime visual PASS.

- Closed S21 sprite mask4 angle initialization, UV corner order and
  component3 color/quad submission; no extra Alpha factor. Compared full
  additive-state helper (cull/depth-write/alpha-test/texture/fog) with native.
  Native JPEG loader explicitly sets Components3; S21 loaded metadata still
  open. Selected primitive source supports native renderer reuse, not yet
  runtime parity. Next: S21 texture metadata and native pool integration
  checks, then owner-bone emitter/root activation gates.

- Verified native/S21 particle render entry gates: Live, pass1 z>350 skip,
  pass2 z<=300 skip; texture dimensions*Scale, stored RGB without extra
  Alpha multiplication and fullUV submission match selected fire branches.
  Native component3 uses additive ONE/ONE and glColor3fv; S21 axis-mask4
  geometry/UV, loaded metadata and full blend-state helper remain open.
  Recorded native render-tail clamp difference (inactive for private integer
  lifetime). No renderer patch needed on this evidence yet; no runtime PASS.

- Implemented private CreateBrecheFireParticle API with explicit -1 failure,
  reusing native bitmap constructors/pool. Legacy CreateParticle preserves
  its old return convention through internal allocator. Allocation resets
  per-slot mode/remainder for both Breche and ordinary SS6 reuse. Private
  slots run whole-tick source-order alpha/scale/Z/rotation before native
  decrement; ordinary slots retain original update path. No emitter calls
  this API yet: root/bone/render gates remain open before activation.
  Extended x86 /W4 /WX tests cover all three variants, alpha overshoot,
  fade threshold, random draw count and continued writes after Live clear.
  Tests PASS; isolated client ClCompile PASS (ZzzEffectParticle.cpp).
  No link/stage/live binary overwrite or runtime visual PASS. Next: verify
  native render path and allocation/pool reuse integration tests, then emitter.

- Mapped S21 particle fields to native PARTICLE names; verified all three
  firehik01/02/03 native asset SHA256 match S21. Native constructors already
  implement selected8084/0,806E/4,8085/0 contracts, so reuse native bitmap
  families, not duplicate IDs/loaders. Found ambiguous CreateParticle
  return0 (slot0 or failure): assign private tick metadata only inside
  confirmed allocation, reset on every slot reuse. Next: scoped creation
  entry/metadata and private update before native lifetime decrement.
  Evidence recorded in BRECHE_CHILDREN_REVERSE.md; no binary changed.

- Added AdvanceParticleWholeTicks in GrowLancerTick.h: incoming-life gate,
  update then decrement even if callback clears Live; no EffectDestructor.
  External per-slot remainder must reset on native allocation. Added
  GrowLancerParticleTickTest.cpp; MSVC x86 /W4 /WX PASS for life0/1,
  constructor-life ranges, fractional/multiple ticks, early Live clear and
  slot-reset contract. Existing effect tick tests remain PASS.
  Initial cmd-shell quoting failed before compile; PowerShell cmd invocation
  corrected it. Outputs isolated under GrowLancerBuild/ParticleTickTest.
  Main.vcxproj isolated Global Release/Win32 ClCompile PASS after header
  change (ZzzEffect.cpp rebuilt). No link/stage. Not wired to native particle
  loop yet; no visual or pool runtime PASS.
  Next: native per-slot remainder ownership and private resource/type routing,
  then verified fire particle initialization/update fields and render gates.

- Confirmed retired-particle reuse:172502F reads last handle from the same
  globalA726100 vector, erases it, or allocates0xB4 when empty;172513C
  reactivates record. Active-list17255D1 relinks neighbors and decrements
  count. Found native MoveParticles decrement-before-switch mismatch with
  S21 update-before-decrement. Next implementation needs private Breche
  particle entry before native decrement and existing GL tick policy;
  never reorder lifetime globally. Refcount/failure internals remain open.
  Updated reverse evidence; no build/stage or visual PASS in this run.

- Verified particle common movement enabled (byte+68=1), but vector+70
  initialized zero and unchanged by the three selected constructors;
 13272A0 adds a rotated vector without translation, so no extra displacement
  for finite inputs. Retirement clears record byte+4 and Live; callback
 17251C5 is empty, then a handle enters globalA726100 vector-like storage.
  Do not equate this with immediate delete. Next: storage reuse/consume
  and active-list erase; then native particle lifecycle integration.
  Evidence-only change; DK executable and other worktrees untouched.

- Closed selected8084/sub0 and806E/sub4 particle update bodies from dump:
  fade thresholds15/10, shrink.05..07/.06..08, Z+=parameter,rotation+=3.
  Verified shared predispatch rejects life<=0 or !Live; decrement happens
  after update. Recorded distinction from effect-pool expiry ordering.
  Removal helpers1725235/172579F and movement gate byte+68 remain open.
  Updated BRECHE_CHILDREN_REVERSE.md; no live DK binary overwritten.
  Next: particle constructor byte+68,13272A0 and removal helper internals.

- Anchored actual particle update16AA2ED and bounded type dispatch.
  Closed8085/sub0 branch: alpha grows random.2/.3 then fades.2 at life<10,
  RGB=savedRGB*alpha, scale shrinks.07..09, Z+=6.4..8.7,rotation+=3.
  Recorded incoming-value clamp/death ordering and no early return after
  Live clear.8084/sub0 table selects16DA4CF (not adjacent first block).
  Next:16DA4CF,806E/sub4 within16B8BDD, common decrement/container release.
  No client binary change or visual PASS.

- Confirmed three particle constructor tails converge16A9524, enqueue via
 1725950->1724501 then release temporary handle; container ownership open.
  Closed selected render arguments for8084/0,806E/4,8085/0: fullUV billboard,
  texture dimensions*scale, storedRGB, rotation,mask4; no extra+44 RGB factor.
  Corrected1620984 label: it is RENDER, not update. Next update candidate
  16AA2ED must be anchored; preserve unresolved texture blend metadata and
  visibility. No build/stage or runtime acceptance in this checkpoint.

- Recovered all three fire particle selected constructors with bounded
  dispatch:8084/0 counter27..31,806E/4 counter12..16,8085/0 counter17..21;
  scale multiplier.72..1.43, rotation0..359, parameter+6C6.4..8.7.
  All copy white RGB to+7C but zero live Light+38; update must restore/fade
  color. Added table/subtype checks to focused verifier. Next exact actions:
  shared tail164D1E4/16A9524 then particle update1620984, render/cleanup.
  No runtime activation or source particle layout transplant.

- Closed flare allocator metadata: subtype0, rotation0, axis mask4,1x1
  full-UV metadata; no effect-style lifetime assigned. Sprite render pass
  clears Live for pass0/2, retains for pass1. Native pool has analogous
  ownership. Found nonvisible scale-factor difference: S21 subtracts.1,
  current5.2 adds.1*FPSfactor. Recorded scoped compatibility requirement;
  no global sprite patch. Next: visibility/pass producer, particle chains
  8084/0,806E/4,8085/0 and complete primitive axis-mask handling.

- Traced806E/sub15 as owner-bone emitter: two iterations/update, random
  bone world positions, flare7EF7 scale4 RGB(1,.2,0), plus random particle
  8084/0 or806E/4 or8085/0 at scale1.3..1.7 RGBwhite. Distinguished
  particle806E/4 from effect806E/15; five constructor children are not
  the whole visual chain. Verified three extra texture registry paths/hashes.
  Bone helper uses matrix translation*model scale+owner.Position; invalid
  bone count source path risks uninitialized position, native guard pending.
  Next: sprite1726C50 metadata/lifetime and three particle chains, native
  bone mapping. No asset staging, runtime activation or build this checkpoint.

- Implemented private GrowLancerTerrainAdapter.h: optional scoped texture
  matrix V reflection around native RenderTerrainAlphaBitmap; original
  matrix/mode restored and legacy terrain files remain byte-untouched.
  Added test_terrain_adapter.cpp; MSVC x86 contract tests PASS including
  nonidentity matrix, UV outside[0,1], opt-out and exception restoration.
  Real Main.vcxproj ClCompile isolated QA Global Release/Win32 PASS.
  First compile command used wrong project directory (MSB1009); corrected
  to ExMain_RISE_PC/Main.vcxproj and succeeded. No link/stage or visual PASS.
  Adapter not activated until remaining Breche contracts close. Next exact
  action: byte+B visibility producer and806E/sub15 update/render.

- Verified terrain alpha operation using pinned PE import names:
  1B4C774=glColor3fv,1B4C758=glColor4f;1765DF1 does not multiply
  incoming RGB by Alpha again. Native terrain wrapper matches this branch.
  Explicit FlipV overload patch was rejected by legacy header encoding;
  scoped diff confirms neither terrain file changed. No encoding conversion
  or runtime adapter claimed. Next: implement scoped UV adapter while
  preserving legacy file encoding, then byte+B visibility and806E renderer.

- Traced separate terrain render pass1596B00 for809F/sub0 and809E/sub14:
  exact dispatch/subtype and final1765DF1 calls confirmed. Both use
  Light*Alpha, explicit Alpha, height5, terrain heights=true, FlipV=true.
  This differs from81EC FlipV=false; scoped native UV option is needed,
  not a global texture inversion. Render byte+B visibility producer and
  final alpha handling remain open. No runtime binary changed.
  Next: terrain color operation, visibility producer,806E/15 renderer.

- Rejected false root secondary handler157F0A5 by auditing table bounds:
  1595660 accepts5E1..5F1 only, not5FD. Actual5FD path1574AA4 uses
  byte selector1595732=32 ->pointer1595724=15945E5 generic movement.
  Updated reverse notes and table verifier. Do NOT import the unrelated
  7F5A/11,512..515 debris,8013/4 emissions or lifetime holding behavior.
  Next: root primary/model animation/render and remaining child renderers.
  This supersedes prior state next-action references to157F0A5.

- Closed809E/sub14 primary update for both children: Angle.z+15, Scale+.1,
  alpha+.1 at life>=11 else-.1. Traced81EC/809F/809E through common
  secondary dispatch to generic rotated-vector movement1595A11, then
  update-before-life0-cleanup and subsequent decrement. Destructor has no
  type-specific cleanup for these children; resets pool record, not owner.
  Vector+F8 reset is zero, later constructor writes still require audit.
  Evidence recorded in BRECHE_CHILDREN_REVERSE.md. Next: vector writes,
  remaining renderers,806E/15,root157F0A5 grandchildren. No build/stage.

- Traced primary81EC/sub0 update: alpha +/-1/(maxLife*.5), threshold
  remainingLife>maxLife*.5, no branch-local scale/position writes or clamp.
  Traced809F/sub0 table entry and update: Angle.z +=30 if effect WORD+3C
  equals279, otherwise15; alpha +.1 at life>=11, -.1 at life<=10.
  Followed wrapper metadata: this child's+3C is explicitly0, so it selects
  +15, NOT+30 merely because the parent skill is279.
  Recorded constants, branch boundaries and common-tail caveat in
  BRECHE_CHILDREN_REVERSE.md; removed stale renderer-lead wording.
  Next: common tail1574472,809E/14 update.
  No runtime binary changed, no build or visual acceptance claimed.

- Resolved81EC terrain extra flags: finalfalse is no V-flip, precedingtrue
  uses terrain heights; both match stock5.2 geometry/UV behavior. Corrected
  "culling" lead:176A366 is attribute eligibility, not camera/frustum.
  S21 excludes bit0x200 and special world141 conditions; RISE lacks these
  checks at this render site and bounds XY differently. No global patch or
  guessed world mapping. Next exact task:81EC update dispatcher and other
  child renderers; preserve scoped terrain filter difference for runtime port.

- Traced81EC/sub0 to terrain bitmap call1765DF1: temporary RGB=Light*Alpha,
  XY/Scale/Angle.z, alpha argument1,height5, additiveONE/ONE. Native RISE
  RenderTerrainAlphaBitmap is the matching API family, not CreateSprite.
  S21 tile height true-path confirmed; final extra argument0 and tile culling
  remain differences to audit. Recorded exact call/field mapping in child
  reverse note. Next:1769C7F final extra arg and81EC update; other children
  update/render still OPEN. No live client changed.

- Closed selected child constructor tails including806E/sub15 life20. Generic
  constructor proves supplied scale<=0 becomes0.9, so two zero arguments must
  NOT be ported as invisible effects. Final selected child scales2/2.5/.9/.2/.9
  and alphas0/.1/.1/.1/1 recorded in BRECHE_CHILDREN_REVERSE.md.806E max-life
  reset and all update/render paths remain open. Next:15AEC85 renderer lead,
  child update switches, common reset; no visual PASS or live patch claimed.

- Breche's four actual child textures identified in textureRecords and SHA256
  verified from read-only S21 files. Added BRECHE_CHILDREN_REVERSE.md; confirmed
  81EC/sub0 life20 alpha0,809F/sub0 and809E/sub14 life20 alpha.1. These are
  CreateEffect pool records, not particles. Root tail emits no sixth child.
  Next exact constructor tails:145C5A5,145C315,144280E subtype15; update and
  render still OPEN. No assets staged or live client modified by this audit.

- Breche receive owner traced to packet secondary actor key bytes7/8 masked
  0x7FFF -> live-character lookup -> object; not caster Hero. Skill switch is
  normalized via BCFF9F parent chain. Constructor5FD subtype1 sets life20
  and emits five children:81EC/0,809F/0,809E/14 twice,806E/15. Recorded
  exact call VAs and supplied args in BRECHE_REVERSE; child overrides still
  unverified. Added focused table verifier. Next: constructor shared tail
  1482B26 then each child's registration/update/render/lifetime; no fake root.

- Recovered genuine Breche receive root: skill279 table index0 selects
  12CB7F5 -> CreateEffect(0x5FD, subtype1) at12CB854. Local279 independently
  confirms action190/sound1119 but skips upgraded local roots. Retracted
  blanket "no root" claim; descriptor now records unimplemented receive-root
  lead. Owner local -1948 producer, constructor and child chain remain open.
  Updated acceptance matrix to OPEN (class decision already supplied).
  Next file: pinned dump; next functions: receive local -1958 producer,
  0x5FD constructor, secondary-update path to157F0A5. No runtime patch yet.

- CRITICAL CORRECTION: traced renderer switch operand to (Type-0xAE9)%512
  at 17D085D..17D0873 -> 17D0B9A..17D0BE6. Prior "action185..196" table
  attribution is false; these are item indices. Retracted Breche equipment
  layer into excludedVisualLeads in descriptor and marked reverse note
  authoritative correction. Do NOT wire clipping adapter from this evidence.
  Next exact action: reopen real skill279 dispatch/action190 chain; generic
  model/bone results do not count as Breche visual-chain coverage.

- Closed seven-model offline tail issue using S21 loader evidence, not by
  assuming zero padding: bone-loop exit frees buffer without checking the
  remaining bytes. Tool records arbitrary <=16-byte tail under conservative
  local policy. All 29 models now parse and match referenced bone headers
  against S21/DK QA; eight focused tests PASS. No runtime/visual acceptance
  inferred. Next file: ZzzCharacter.cpp equipment Transform/RenderItemBody
  path; next implementation remains private Breche equipment rendering only
  after native transform and registration contracts are established.

- Compared referenced equipment bones plus ancestor chains against S21 and
  isolated DK QA player skeletons: 22 parsed models have zero index/name/
  parent/dummy mismatches to either 60-bone player. Added reproducible
  compare_equipment_bones.py and 5 passing comparison tests. Seven models
  still fail strict trailing-data policy; no speculative remapping introduced.
  Next investigate v0F full-block padding, then native equipment transform and
  bind-pose path before wiring Breche's actual body rendering.

- Offline Breche mesh walker added. Found model-dependent pants mesh counts
  and skeleton bone counts; traced S21 RenderMesh wrapper to anchored core
  13312EF and proved out-of-range mesh calls are skipped before rendering.
  Existing native/private guard agrees; do not synthesize absent mesh passes.
  22/29 files satisfy bounded layout+<=15-byte tail; 7 have 16-byte tails,
  and nonzero tails remain unverified, not claimed corrupt. Full native asset
  loading, bone-name mapping and runtime parity remain next requirements.

- Breche model lookup recovered beyond literal registration tables: anchored
  EF2A81 -> D06184 item record -> D05887 loader, path fields +8/+10C.
  Read-only item.bmd decoder verifies native checksum and 708-byte records;
  records prove item index is NOT model filename suffix (armor185 maps
  ArmorMale128_benzia). Added inspect_breche_item_models.py and detailed
  BRECHE_REVERSE.md evidence. Historical reference-table equality, mesh parse
  and recipient equipment remain open; no assets or live client replaced.

- DK QA diagnostics correction: timestamp UTC and PID now prefix all log
  records. Rejected no-caster/no-target and unsupported contact are explicit;
  void root/contact calls log dispatch only, not successful allocation. Breche
  records action-only with equipment layer unwired. ClCompile Global Release
  Win32 PASS (object updated 2026-09-12 23:17:50); no link or staging performed.
  Live isolated PID 24216 was responsive. Staged EXE remains 935F3EE8.
  Log baseline: 1800 bytes, last write 21:21:05, predating this session;
  historical cast entries are NOT new runtime evidence. Next: link/stage the
  diagnostic revision only after the user finishes the current client session,
  then verify new timestamped dispatch/rejection and visual behavior separately.

- Added isolated launcher with explicit client working directory and stage
  guard. Found renamed executable parser truncating at spaces; QA-only fix
  uses actual module basename, preserving Engine-Port S21.exe. Build/stage
  935F3EE8 PASS. Earlier correctly-rooted launch PID 22628 exited before UI;
  repaired binary is being rechecked. Production parser unchanged.

- Attempted isolated DK QA launch through computer-use after complete stage
  verification PASS. Engine-Port S21 PID 21060 showed an Error dialog for
  Data\RISE\Config\Mix.bmd although the staged file exists and hashes pass.
  Likely launch working-directory mismatch; not yet proven from process CWD.
  Accessibility confirmed error text; screenshot capture failed with
  SetIsBorderRequired E_NOINTERFACE (0x80004002). Enter dismissed the dialog;
  a subsequent process check found the isolated client exited. Pegasus was
  observed running and was not controlled. No login/cast/visual QA occurred.

- QA snapshot now reports sharedEnergy/sharedCloud separately from private
  glParticles, covering shared native textures used by Magic Pin, Obsidian
  and Shining Peak without claiming ownership. Current QA x86 build hash
  7400C5B9; client process absent at check, no new live acceptance.

- OBJECT construction initializes bone flag/pointer; explicit true writers
  found are character/pet objects, not ordinary Effects. No demonstrated
  bone-ownership bug; left destructor unchanged. Fixed QA pool classifier
  missing Wrath particles 32940/32941, so old glParticles counts are not
  cleanup proof for those nodes. QA x86 build/stage hash CADBD594.

- Audited S21 destructor 0x14B6820 and native SS6 EffectDestructor. Confirmed
  native dead/owner cleanup but not full reset parity. Recorded potential
  bone-flag reuse risk in WRATH_POOL_LIFECYCLE_AUDIT.md; no speculative free
  or global destructor change was made. Timer reset is explicit for both types.

- Extracted existing private tick loop into GrowLancerTick.h, called by the
  real client for brocken/travel only. Added GrowLancerTickTest.cpp and built
  x86 /W4 /WX: PASS for life 20/30, factors .25/.5/1/2, fractional remainder,
  update at zero, one destruction, large overshoot and dead-record callbacks.
  The harness uses a surrogate record/callbacks, not native destructor internals.
  Client x86 build/stage hash 0C6C4813; no gameplay/visual PASS inferred.

- Travel alpha clamp removed after complete update-block inspection.
  Added travel to the private whole-tick lifetime path with Timer initialized
  at creation; preserves S21 update-before-expiry-before-decrement ordering.
  QA x86 build/stage is now 65364DC5. No new gameplay/visual acceptance.

- Wrath travel renderer corrected from full callback disassembly: fixed
  RGB (0.37,0.57,1)*alpha, blend-mesh 0 and literal zero UV; no inherited
  object color. Descriptor and reverse evidence updated. QA x86 build/stage
  hash `65364DC5ACF439C0A100E92323C3E6FA9BA8F15F08611EB3097A5C92EBD918B4`.

- Corrected Wrath aura blend-mesh index from -2 to dump-proven 0; travel
  remains unchanged pending its own renderer audit. QA x86 build PASS at
  `7F9BEAAC26E070855D926CCAEF6205FB37E39246BC6FFDFE3786CEE175A51E32`.
  Validated the three ordinary-updater callers from function prologues;
  adjacent pool sweeps are not assumed to be render calls.

- Confirmed Wrath aura uses the ordinary S21 effect pool, and constructor tail
  returns without an immediate update. Its updater scans live slots ascending;
  newly created aura update timing can depend on allocated index. This rejects
  the unproven assumption that owner-follow always precedes first render.
  Recorded exact selectors/loop addresses in WRATH_CORRECTION_AUDIT.md.

- Implemented Wrath subtype-1 inherited light, preserving fixed subtype-0
  transition color. Current isolated build/stage hash is
  `A7A255880F22B1A3FBE7E115FFC948D5FB4C0888642D89078D0CD1539861DDD0`.
  Complete stage verifier PASS; runtime acceptance unchanged.
- Traced allocator position copy at 0x143EAAD..0x143EB0A: all three supplied
  coordinates are copied into the object before its subtype constructor.
  This does not yet resolve the caller's stack-local coordinates or first-render order.

- Verified Wrath aura two-stage dispatcher table entries from the dump:
  primary slot 0x5C -> 0x1574472, secondary index 11 -> 0x157939E.
  Added a narrowly scoped reproducible table verifier. Confirmed subtype-1
  respawn passes inherited light while the adapter resets it; initial position
  is still unresolved, not declared verified or copied from an unrelated branch.

- Latest correction: Wrath aura subtypes 0/1 now match S21 incoming-alpha
  branch order; removed post-update clamping after verifying the shared tail.
  Isolated QA x86 build/stage PASS at
  `55F50A2AF413D92D121F443B1BCC73CAFDD11DD079DB2C4614CB64DBD39A3C89`.
  Respawn stack-local reaching definition and fractional timing remain open.

- Current checkpoint: isolated brocken integer-tick/expiration correction and
  particle incoming-clamp fixes built and staged as SHA-256
  `C30B082F67D9A5152CB1CE0777895473488D9A87816949D658285A646C3A3F6F`.
  Complete stage verification PASS: 2,658 base files, 76 overlay files and
  three root Data files; no new gameplay/visual acceptance. Historical build
  hashes below identify earlier checkpoints, not the current executable.
- Fresh aura dump audit found the adapter's post-update alpha clamp differs
  from the S21 local branches, and subtype-1 respawn uses a stack-local
  position needing definition tracing. See WRATH_CORRECTION_AUDIT.md.

- Renamed every future isolated Grow Lancer client output to
  `Engine-Port S21.exe` and the QA window class/caption to `Engine-Port S21`.
  The Codex task title now matches. Legacy `Engine.*` build artifacts were
  moved into scoped `LegacyEngineBeforeRename` folders; Pegasus and production
  executables were untouched. Renamed QA build SHA-256 is
  `E24360233038436F38E6D6F2613AF396FC8074E677D293EB97E63417CE6DF325`.
- Closed Wrath bitmap children directly from the pinned dump. `0x81E4`
  (`!Combo3`) and `0x81E6` (`alpha_light`) now use native SS6 particles with
  exact subtype-0 lifetime/scale/light/update contracts; `0x81E3` (`brocken`)
  now uses the native effect pool with lifetime 30, scale 4 and its exact
  alpha/light decay. CORRECTION: Combo4 was mistakenly attributed through
  the OBJECT renderer. The particle dispatcher uses its own !Combo3 texture.
  Removed that runtime substitution and verified the sine helper uses radians.
  The resulting isolated x86 build is PASS at the hash above.
- Corrected Clash from its constructor/update/render blocks: each visible
  child now retains the target direction captured at creation instead of
  re-aiming every frame; the 27-tick front alpha uses integer split 13/14;
  renderer deltas apply to blend-texture U/V rather than object angles; both
  callbacks submit blend-mesh index 0. These fixes are included in the current
  build hash.
- Audited Magic Pin controller `0x1578288` and child constructor
  `0x14808E9` directly from the dump. Corrected the stage-0 `0x5E6` origin,
  retained-point reuse for stages 1/2, sequential stage gating, null-owned
  Thunder subtype 9/10 and exact `rand()%50-25` spread. QA x86 rebuild PASS;
  current Engine SHA-256 is `ED4AA049A3FB4E6BCD3B9EB3F64C91D8F5338927C243DC31C847B66182B90E8B`.
- Audited Obsidian directly against constructor `0x1480640` and renderers
  `0xA48B0E`/`0xA48C8C`. Corrected Thunder01 subtype 8 to the native null owner
  and replaced the provisional SS6 blend-mesh default with exact arguments 0
  for obsidiana/mesh 0 and 0/1 for obsidian meshes 0/1. QA x86 rebuild PASS;
  intermediate Engine SHA-256 was `22007E868F29815C5AFB14CF6B9CF89209C0E92C5E1B743F197821B930B72EE6`.
- Replaced Circle Shield's provisional Shiny04/flare01 approximations with dump-closed primitives. Shiny04 subtype 0 now uses lifetime 15, scale 3, world-time rotation and the exact `+0.2/-0.1` alpha envelope; byte-identical `flare01` now runs through native `CreateParticle(BITMAP_LIGHT, subtype 0, scale 10)`. The duplicate custom flare slot is no longer registered.
- Rebuilt the QA x86 client after the Circle Shield correction. Current Engine SHA-256 is `0AA00E10FB9FA2DECF4AE140FD3F5B266E415F8839853427098519E8AAE59577`; only pre-existing third-party `LNK4099` warnings remain.
- Closed Spin Step's previously open weapon-motion block directly from the pinned S21 dump. It uses caster bone 33, three animation samples per update, frame range `0.0..4.5`, endpoints `(0,0,0)`/`(0,-180,20)`, rotated offset `(0,-10,0)`, white type-1 object blur and S21 render style 1.
- Decoded the ninth S21 object-blur argument through allocator `0x15C01CC` and renderer `0x15C09DE..0x15C0BB8`. Added an isolated-compatible style-1 UV path while preserving style 0 as the default for every existing SS6 call.
- Corrected Spin Step fan-out boundaries: subtype 0 now owns three scale-17 crosses plus `magic_ground1_3`; subtype 1 owns flare scale 5 and five subtype-4 particles; internal skill 280 owns flare scale 3.5, five subtype-4 particles and three scale-12 crosses before its delayed holes.
- Closed `flareBlue` subtype-4 construction from `0x164917E..0x1649311`: lifetime `rand()%20+20`, supplied scale, alpha 1 and inclusive integral position offsets X/Y `[-50,50]`, Z `[0,100]`. The QA x86 client rebuilt successfully; Engine SHA-256 is `9544D8AC66DE3D340F601EE0C8074C45F973C428DFC7EA351BEBC625513CAC0D`.
- Captured an unmodified native S21 reference in two isolated batches: 802 frames/150093 ms for the main sequence and 272 tail frames for Wrath/Breche. Hashes, exact representative frame IDs and observations are pinned in `GrowLancer\S21_NATIVE_REFERENCE.md`; the original S21 client was not modified.
- The reference confirms Spin Step is a two-phase presentation: caster weapon/action onset followed by a distinct target-side blue vertical/contact layer. The earlier 5.2 F7 test had no selected target and therefore omitted the separately mapped internal-skill-280 F9 contact path; it remains a visual FAIL and is not a like-for-like complete-chain comparison.
- Captured native visual references for Circle Shield, Obsidian, Magic Pin, Harsh Strike, Shining Peak, Wrath and Breche. Clash remains uncaptured because it did not cast in the observed runtime state; no substitute visual evidence is claimed.
- Accepted `D:\GameServer S21` as a new read-only authoritative server source. `IGC.GameServer2_R.exe` is x64 version `21.2.3.1`, SHA-256 `134BAEBF654D103B3FD65F8262CB20D2AEC5969B3C09C68E26A87F6547F77D74`; its referenced PDB is absent and protected sections prevent treating raw static strings as a completed handler reverse. The accompanying Data confirms the Grow Lancer class columns, skill rows, seven scrolls, buff IDs and formula/config contracts.
- Diagnosed the mojibake `RISE` dialog as the hard-coded Player.bmd CRC gate. Added only the hash-pinned merged Grow Lancer Player CRC `0xE51E1780` while retaining legacy SS6 CRC `0x7D56FCEB`; rebuilt QA x86 successfully to SHA-256 `D7C0F9234FEC34057CE6E25B2734A911A8933B919096B65E6C9130B2F9C72BDF`.
- Reproduced the next login failure with a generated minidump: `0xC0000094` at `Engine.exe+0x1A105`, exactly `CSimpleModulus::EncryptBlock+0x55` division by the zero modulus. Proved the staging script omitted root-level Data files, including `Enc1.dat` and `Dec2.dat`. It now copies and hash-verifies all root Data files; the complete-stage verifier covers three root files in addition to 2,658 base RISE and 75 Grow Lancer overlay files.
- Restaged and relaunched the corrected isolated QA client as PID `22184`; the owner confirmed it reached gameplay. Startup/login is now PASS, while icon/cast/contact/pool/visual acceptance remains open.
- Corrected the Grow Lancer icon UV divisor: the dedicated atlases are 512x512, but the shared draw path still divided the cell width/height by 256 and therefore sampled a 2x2 region (four mini-icons). The Grow Lancer branch now carries its own 512 atlas size while legacy SS6 icons retain 256.
- Replaced the first F10 live-skill-array injector with a non-mutating QA panel that draws all nine enabled icons and nine disabled icons directly. The owner confirmed the corrected one-icon-per-cell result from gameplay; PID `23492` remains responsive at 120 FPS and the QA log records `icon-panel-on`.
- Disabled the optional crash-report upload handshake only under `RISE_GROW_LANCER_RUNTIME_QA`; crash dumps remain local. This prevents the unmodified server from interpreting the unsupported QA `F4:E0/E1` exchange as a logout request.
- Rebuilt and hash-pinned the current QA Engine as `547D45843E4F5B647248A5D20A755341A59B2839B6DF0F05DECA2FE82AE9E1D4`. Restaging and the complete verifier PASS for 2,658 base RISE files, 75 overlay files, three root Data files, the merged Player CRC/hash and private non-reparse Player/RISE roots.
- Ran the first owner Spin Step cast pass. Runtime log records repeated skill-271 casts without crash and a later Grow Lancer pool count of zero, but the owner screenshot shows only the blue/green ground ring. The action/spin, cross, blur/trail and contact sequence are visibly absent, so Spin Step is explicitly FAIL rather than promoted from static evidence.
- Closed only the isolated RISE QA client after verifying its exact executable path. A later direct run of the read-only S21 `main.exe` succeeded and supplied the two native-reference batches above; no S21 file was changed.
- Created isolated worktree `D:\RISE-CrossPlatform\Source\_PC_GrowLancer`.
- Created branch `feature/grow-lancer-skills-s21` from PC commit `f2e8e558874005708312becbb8273ecb8d7d61bf`.
- Confirmed the requested list is the nine base Grow Lancer skills with IDs `271..279`.
- Pinned the correct read-only S21 server sources under `C:\Users\DELL\Desktop\Skills`; the earlier mistaken upload remains rejected.
- Added a fail-closed package descriptor. Unknown scroll, action, sound, asset, movement and target contracts remain `null`.
- Recovered the exact common S21 caster dispatcher at `0x010E34E2` and all nine base-ID branches. All actions are pinned: Harsh Strike `185`, Spin Step `186`, Circle Shield `187`, Magic Pin `188`, Shining Peak `189`, Breche `190`, Obsidian `191`, Wrath `192`, and Clash `194`.
- Kept Wind Soul and Pegasus out of this worktree and package.
- Reviewed the post-port Wind Soul fixes in Pegasus read-only and recorded them in `GrowLancer\WIND_SOUL_FIX_LESSONS_FROM_PEGASUS.md`. The owner-visible failures were black `desair` children, missing persistent smoke, and trails shortened/thinned by an extra FPS factor and particle throttling. The note also captures the corrected renderer/subtype placement, dependency staging, authoritative receive ownership and target-slot handling.
- Rechecked the latest Pegasus state and owner QA sheet rather than relying on the original handoff. Expanded the Wind Soul lesson note into a mandatory Grow Lancer pre-runtime audit covering submitted `BodyLight`/mesh passes, constructor-family reachability, embedded texture closure, emission throttling, single FPS normalization, root de-duplication and direct target-slot ownership. Pegasus remained read-only.
- Reclassified `0x1577014` correctly as the Harsh Strike root update/controller branch, not its constructor. Recovered the real `CreateEffect` constructors for `0x5E0`, `0x5E1` and `0x5E2`, the action-frame gates, blur bones/offsets, all three timed child spawns, per-tick movement/fade, render passes and owner-action destruction.
- Proved the root is an invisible controller with lifetime 40 and scale 3.8. It generates a type-11 blur from bones 8/5, one `h_strike_wind01` child and two `h_strike_wind02` children at the recovered frame windows and offsets.
- Pinned `HashStrike.wav`, both BMD v0F containers, their decoded identities and all five embedded OZJ dependencies. All five OZJ files already exist in RISE with identical SHA-256; the read-only v0F originals were preserved and isolated converted copies were staged as described below.
- Added a hash-guarded, output-confined v0F-to-v0C converter. Both Harsh Strike BMDs were converted under the isolated Grow Lancer test client and decrypt back byte-for-byte to their pinned S21 plaintext payloads. The two converted SHA-256 values are `E6C1A5DB8CA857D7B98CD48990B211BE142203196EBF148C1F38E34A76A5243E` and `64B6F343A94091EB52C95B62A395FD228BC160BA758E08D378D570B26B5B8851`.
- Added and executed `stage_harsh_strike_assets.ps1`; it verifies every source hash and every copied hash, then stages two BMDs, five OZJ files and `HashStrike.wav` only under `ExMain_RISE_PC\Tests\GrowLancerBuild`.
- Started Spin Step chain recovery in `GrowLancer\SPIN_STEP_REVERSE.md`. The base/master dispatcher mapping, local action 186, remote receive root `0x5F8`, cast sound `0x455`, hit-sound registration `0x456`, invisible root renderer and adjacent `spin_cross.bmd` registration are now pinned. The adjacent skill-20 particle branch was explicitly excluded to avoid false attribution.
- Closed Spin Step's exact `MoveEffect` switch cases for controller `0x5F8` and visible child `0x5F9`, including subtype-0 cast lifetime/frame gate, subtype-1 flare/particle/hole fan-out, three cast crosses, ground layer, child scale/alpha evolution and mesh render flags.
- Proved internal Grow Lancer skill 280 `Explosion` is Spin Step's authoritative hit branch. Its received switch creates controller subtype 2 and plays `SpinStep_Hit.wav`; the delayed blue-flare, five-particle, three-cross and two-hole hit layers are recorded with exact timing/scales.
- Converted `spin_cross.bmd` to a native v0C container with exact plaintext round-trip and staged it with both WAVs plus `line_fire.OZJ`, `flareBlue.OZJ`, `hole.OZJ` and `magic_ground1_3.OZJ`. `stage_spin_step_assets.ps1` verifies all seven source/output hashes and confines output to the isolated Grow Lancer test client.
- Began Magic Pin static recovery in `GrowLancer\MAGIC_PIN_REVERSE.md`: base skill 274 local/receive roots `0x5E5` and `0x5E8`, action 188, cast sound, three controller frame gates, internal skill 281 explosion receive branch, contact Z offset 140, explosion effect `0x5B9` subtype 1 and hit sound are pinned.
- Converted and staged four Magic Pin BMDs plus two proven OZT dependencies, `Thunder01.OZJ` and two WAVs. All four converted containers pass exact plaintext round-trip. The apparent `03.tga` string was proven to be stale bytes after the NUL terminator inside the sole 32-byte texture-name record, so it is correctly excluded rather than treated as a missing asset.
- Closed Magic Pin's remaining `0x5E7` and `0x5E9` update equations and the actual `0x5B9 subtype 1` target-hit chain. The hit controller is a no-draw eight-tick owner record; below five ticks it emits six `clud64` particles and two owner-bound `Shockwave2` particles with pinned subtypes, colors and scales.
- Corrected the Magic Pin hit dependency from the unrelated subtype-0 lead `0x8000` to the actually executed subtype-1 IDs `0x7FFD` and `0x8147`. Both assets are now hash-pinned and staged, bringing Magic Pin to eleven assets.
- Began Shining Peak recovery in `GrowLancer\SHINING_PEAK_REVERSE.md`. Local skill 277 is pinned to action 189, roots `0x5F6`/`0x5F3`, and sound 1117. Root `0x5F6` emits three `pin_lights` joints; root `0x5F3` is a no-draw four-stage controller with exact frame windows `[3.3,3.8]`, `[5.0,5.6]`, `[5.8,6.6]`, `[6.8,7.6]`.
- Pinned Shining Peak models `0x5F4`, `0x5F5`, `0x5F7`, generic `shockwave01`, embedded textures, `pin_lights`, `flareBlue`, and `ShiningPeak.wav`. Four v0F BMDs passed native v0C byte-exact round trips. Dynamic child `0x8100` was resolved through its loader loop to `xsuper0001.OZJ`; thirteen assets are staged by the hash-guarded `stage_shining_peak_assets.ps1`.
- Decoded all four Shining Peak controller emissions with exact frame windows, rotated offsets, counts, random ranges, child subtypes, scales and light. Child updates are now pinned: `0x5F4` subtype-0 scale `+0.01` and half-life alpha, `0x5F5` scale `+0.15`, triangular alpha and rotated `(0,-50,0)` movement, and `0x5F7` angle-Y `+20` with triangular alpha.
- Proved the exact received base-skill selector: `cmp skillId,0x115` at `0x12CEEB0` branches to `0x12D09E7`. That handler uses only the caster character/object for action and roots; it does not consume the already decoded target index, so Shining Peak has no client-side target/contact/reaction layer in this visual chain.
- Recovered Circle Shield's authoritative received branch `0x12CB2F7`: action 187/argument 316, sound 1111 and caster-owned no-draw controller `0x5DD`. Its constructor burst, timed `Shiny04`/`flare01` layers, bone-42 emissions and visible `circle_shields.bmd` child are documented in `GrowLancer\CIRCLE_SHIELD_REVERSE.md`. Six hash-pinned assets are staged by the isolated verifier.
- Recovered both Obsidian roots from received handler `0x12CB59E`: action 191/argument 317, sound 1112, models `0x5E3`/`0x5E4`, their alpha/rotation updates, two native render contracts and the constructor `Thunder01` particle. Eight assets are converted/copied and verified by `stage_obsidian_assets.ps1`.
- Recovered base Wrath's received handler `0x12CB6AC`, action 192/argument 321, sound 1118 and no-draw staged controller `0x5EA`. Four animation-frame gates create the `!Combo3`, `wratha01`, `wratha02`, `alpha_light` and `brocken` layers. The `0x5EC subtype 1` aura recreates itself only while owner buff 424/425 exists, proving the persistent visual ownership/cleanup contract. Eight isolated assets are staged and hash verified.
- Closed Breche's base action-190 render chain in `GrowLancer\BRECHE_REVERSE.md`. The base skill creates no standalone root effect: four body-equipment jump-table paths for item groups 8 Armor, 9 Pants, 10 Gloves and 11 Boots drive native mesh/body passes, including the conditional `Item1130_L_red` texture. The earlier weapon classification was disproved by the exact `(Type-0xAE9)/512` selector. Adjacent upgraded roots 2066/2089 are explicitly excluded. `Breche.wav` and the texture are hash pinned and staged.
- Closed Clash's local and received target/facing/action chain plus roots `0x5FA`, `0x5FB` and `0x5FC` in `GrowLancer\CLASH_REVERSE.md`. The controller retains caster and target, gates once on action progress, derives the normalized caster-to-target direction, creates front/rear model flashes, and self-destroys when owner action differs from 194. Both model updates, alpha envelopes, scale/rotation, render flags and textures are pinned.
- Corrected Clash sound evidence: S21 sound ID 1115 is registered as `Data\Sound\Crash.wav` at `0x18E2E4B`; the nearby on-disk `shieldclash.wav` is not this registration and is excluded.
- Added two hash-guarded asset stagers for Breche and Clash. Seven additional files were staged without touching S21 or production data.
- Proved numeric reuse of S21 player actions is invalid: S21 `player.bmd` has 410 actions and SS6 has 284, while every S21 action 185..194 differs from the SS6 action at the same index. Added a bone-layout-guarded adapter that preserves all 284 SS6 actions byte-exact and appends S21 actions 185..194 as RISE actions 284..293. The isolated merged model has 294 actions and SHA-256 `0CC3D22D5BBD426128E6BFFE9C3766585F9F28BBDA5B3DD50122EC7AD6B9CA63`.
- Confirmed the base RISE server has `MAX_SKILL=629` and `MAX_SKILL_LIST=60`: IDs 271..279 fit the lookup/packet ID range without increasing the character-owned slot count. The nine IDs remain unimplemented in its skill dispatcher and enum.
- Pinned the accepted S21 server tables and initial contracts: base costs/ranges/delays/requirements, internal Spin Step Explosion 280 and Magic Pin Explosion 281, regular damage formulas, Obsidian value/duration, Circle Shield AG/time/rate settings and third-master brand chains. `SkillElect.hit` is a generic 36 x 21 x 21 directional mask, not per-skill damage evidence.
- Added the isolated native resource/runtime scaffold: bounded model tail, exact IDs 271..281, lazy BMD/bitmap registration, expanded sound-buffer capacity, fail-closed class catalog, native effect-pool create/move/render/destruction integration and receive dispatcher entries.
- Implemented and linked the verified Clash and Obsidian roots, actions, sounds, owner/target contracts, model children, update envelopes and BMD render passes.
- Implemented Spin Step action 285, cast controller, staged cross/ground/hole layers, internal hit skill 280 and hit sound. Implemented Magic Pin action 287, all three cast frame gates, four model types, UV/movement/alpha updates, target-Z contact controller, internal hit 281 and burst layers.
- Added a skill-local bitmap carrier because new SS21 texture IDs cannot safely enter SS6's legacy particle switch. It remains inside the native effect pool and does not change global SS6 particle behavior.
- Implemented Harsh Strike action 284, controller timing, all three visible child spawns, both movement/fade equations, exact four-pass/one-pass BMD rendering and the later-proven bone 8/5 type-11 blur path.
- Implemented Shining Peak action 288, both roots, four stage gates, verified offsets/random ranges, all three visible model child updates and render passes. The S21 joint/particle primitives currently use the isolated bitmap carrier; the two generic subtype-6 shockwaves remain fail-closed pending their exact SS6 adapter.
- Built and linked the PC x86 client repeatedly with Visual Studio 18 Insiders MSBuild. Latest isolated output is `ExMain_RISE_PC\Tests\GrowLancerBuild\Bin\Engine.exe`; only pre-existing `LNK4099` missing third-party PDB warnings remain.
- Re-ran the server-contract verifier, descriptor JSON parse and `git diff --check`: all PASS.
- Added a shared eleven-row server contract for IDs `271..281`, seven exact scroll mappings, learning-stat checks and the five regular damage formulas proven by the S21 Lua source. A focused MSVC `/W4 /WX` contract test PASSes.
- Integrated all eleven rows into the isolated GameServer skill catalog after `Skill.txt` load and added enum/item lookup mappings for the seven evidenced scrolls. Circle Shield and Clash intentionally remain without learn-scroll mappings.
- Kept every SS6 `RequireClass` column zero in the injected rows. The server therefore fails learning/casting closed instead of silently treating an existing SS6 class as Grow Lancer.
- Built the isolated Ex603 Win32 GameServer successfully to `ExGameServer\Tests\GrowLancerBuild\Bin\GameServer.exe`; no production server output or configuration was touched.
- Corrected Obsidian and Magic Pin's `Thunder01` layer to native SS6 `BITMAP_ENERGY`: the SS21 and SS6 assets are byte-identical, so the provisional custom bitmap slot was removed. Magic Pin now creates the auxiliary model for both evidenced parent subtypes.
- Decoded the complete Harsh Strike blur allocator ABI. Its first eight arguments map exactly to SS6 `CreateObjectBlur`; the ninth SS21-only byte is zero at this callsite. The frame window `2.0..4.0`, six samples, white light, type 11, bones 8/5 and local offset `(-30,0,100)` are exact. Creation remains fail-closed because the dynamic controller model type 1504 and its bone layout have not yet been provenance-closed.
- Corrected Shining Peak's three `pin_lights` emissions from provisional bitmap carriers to the native `CreateJoint` family: subtype 2 at the rotated `(0,0,35)` origin, then subtypes 3 and 4 after `Z += 80`, all scale 12 and caster-owned. The SS21 and SS6 `pin_lights` assets are byte-identical, so custom bitmap ID 32952 and its registration were removed.
- Proved Circle Shield's constructor burst also calls the 14-argument SS21 joint allocator, not the particle allocator. Its type is `force_Pillar`, subtype 2, scale 20 and the SS21-only final argument is zero. Native substitution is intentionally deferred because SS6 `MoveJoint` does not yet prove equivalent subtype-2 semantics for this bitmap.
- Rebuilt the PC x86 client after these corrections and restaged the isolated client successfully. That checkpoint `Engine.exe` SHA-256 was `23798A14C5A18AC587963758DA83558DD6B4F1147F66EBB53C08E47837CDD685`.
- Closed Circle Shield's `force_Pillar` layer as native `CreateJoint(BITMAP_FORCEPILLAR, subtype 2)`: constructor lifetime 18, scale 20, velocity 0, ten tails, target Z+100, reverse UV 2 and light `(0.2,0.45,0.65)`; update accelerates velocity by 2 and uses the native humming/tail/render/lifetime path. SS21 and SS6 assets are byte-identical, so provisional bitmap slot 32944 was removed.
- Closed Magic Pin's `clud64 subtype 19` particle as a native constructor branch that actually renders byte-identical SS6 `smoke01`, lifetime 20, alpha 1, supplied scale and independent random rotation. Corrected the hit burst to randomize one shared point once and choose each of six scales independently, matching the recovered SS21 calls.
- Closed Magic Pin's `Shockwave2 subtype 1` particle constructor/update/renderer/cleanup chain. Its isolated texture is not the native SS6 shockwave; the adapter now uses native `CreateParticle` and the native pool, with lifetime 8, alpha 0.6, expansion `1.3 + 0.133333/tick`, rendered scale `0.69 * expansion`, fade `0.08/tick` below lifetime 6, light multiplied by alpha and immediate null-owner destruction. The provisional sprite carrier was removed for this layer.
- Rebuilt/restaged after the Magic Pin native particle corrections. Current isolated client and build `Engine.exe` SHA-256 is `E0CA514D799C4D18A1D5901267BFC62C0CDB3D76B91E4528AD4C3BD56438D1E3`; server verifier, both JSON files and `git diff --check` PASS.
- Closed Shining Peak's six-frame `xsuper0001..0006` dependency and native particle family. The SS21 constructor uses subtype 1, lifetime 6, scale 1.8, random rotation and light `(0.7,0.7,1.0)`; update advances the texture ID by elapsed lifetime while fading RGB by 0.95. All six frames are hash-pinned and staged.
- Replaced Shining Peak's two provisional `flareBlue subtype 5` carriers with the exact native particle constructor/update/render path: lifetime 20, input scale 2.5, white constructor light, random rotation and a symmetric alpha envelope peaking at 0.85.
- Corrected Shining Peak stage 3 from a mistaken `0x7FDD` transcription to exact `0x7FFD clud64 subtype 19`; it uses the proven native byte-identical `smoke01` renderer, four children, light `(3,3,3)`, scale 2 and per-axis random offsets.
- Closed both Shining Peak `0xAD6 shockwave01 subtype 6` children. Exact constructor fields are lifetime 6, caller scales 0.3/1.05, alpha 0.85, angle Y/Z minus 90 and light `(0.2,0.3,1.0)`. The default SS21 update rotates and adds a zero Direction, so the children remain stationary, and the shared renderer is mapped to native object calc/draw using the isolated converted S21 model.
- Rebuilt and restaged after closing Shining Peak. Isolated build and staged `Engine.exe` SHA-256 is `68F5AA1337219F1803F0773A03F69B72FC28421CEB9689FC059634D626139D16`; descriptor/provenance JSON, server verifier and `git diff --check` PASS.
- Closed Harsh Strike controller model 1504 as an intentional empty transform carrier. S21 first evaluates the caster action into the shared bone buffer; the zero-action controller animation then returns without overwriting it, so bones 8/5 are proven caster bones transformed with controller scale/origin. Implemented the exact six-sample frame-2.0..4.0 native `CreateObjectBlur` path (type 11, subtype 3, lifetime -1, white, rotated `(-30,0,100)` offset).
- Added `find_s21_u32_refs.py` to classify every literal reference to a candidate runtime ID. It confirms 1504 has renderer registration and creation/dispatch references but no direct loader call, matching the empty-carrier behavior.
- Rebuilt/restaged after the Harsh Strike blur closure. Current isolated client and build `Engine.exe` SHA-256 is `4E5F1A223AF3673F6CAEADE98B2F0F210916A3D9E92DF0E67AF0769EECF2A315`; server verifier, focused contract test, both JSON files and `git diff --check` PASS.
- Searched the available Desktop, Codex, `D:\MU-CrossPlatform` and `D:\RISE-CrossPlatform` source/config roots for named Grow Lancer server handlers. Only the accepted S21 Lua formulas and this isolated package were found. Available RISE/MU server binaries and PDBs expose a Grow Lancer class shell but no named 271..279 handlers; no matching S21 GameServer binary or server memory dump is currently present.
- Added an explicit server dispatch safety boundary for all IDs 271..281. They can no longer fall through the target server's default `BasicSkillAttack`; every base/internal Grow Lancer cast returns false until an authoritative handler is proven and deliberately connected. The shared contract classifies base versus internal IDs and the focused test covers both boundaries plus an unrelated SS6 ID.
- Rebuilt the Ex603 Win32 GameServer after the dispatch guard. Isolated SHA-256 is `E7014699A16A85F1A530B691ADE95DF89C3525A69EB9EAC6759EB6EA86704A40`.
- Added a compile-time-only client visual QA harness and documented it in `GrowLancer\RUNTIME_QA.md`. In the isolated QA build, F6 selects IDs 271..279, F7 casts the selected local visual, and F9 emits the separately proven Spin Step/Magic Pin contact layer on the selected live target. Normal builds do not define the QA macro.
- Built and staged the QA-only client under `ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA`; its Engine SHA-256 is `B70B24AC4BC5B15671D919F4E78B0B40383639D7AF0FEFBB2C93E29B62F47C3A`. Binary marker verification PASSes in the QA Engine and is absent from the normal Engine. The QA client has 127 read-only source Data junctions plus private Player/RISE copies; it has not been launched.
- Re-audited all four Breche jump tables at their exact bases. Each subtracts action 185 and dispatches 12 entries through action 196; index 5 is action 190 in every table. The four outer selectors are proven item groups Armor/Pants/Gloves/Boots, not four weapon models. Exact normal/red mesh subsets are now recorded, while the S21-only render bit `0x200000` remains fail-closed.
- Decoded S21 render bit `0x200000` at `0x01333847..0x013338D7`: it omits a whole triangle only when the supplied V threshold is strictly above all three transformed vertex-Z values. SS6 has no matching flag and its array renderer cannot reproduce this by ignoring the bit; the exact primitive is now specified for an isolated adapter.
- Corrected the Breche transition discriminator: object `+0x2A` is the character action, so action 190 takes the `0.1 * 40` downward threshold path; the previously considered item-range interpretation was wrong. Global `0x1E05010` is a multi-state scene-mode object and value 6 enables the animated path, though its symbolic enum label is not present in the dump.
- Implemented `RenderBrecheClippedMesh` without modifying legacy `ZzzBMD.cpp`. It filters whole `Triangle_t` records using the exact strict three-vertex Z predicate, temporarily swaps the public mesh pointer/count, delegates to native `BMD::RenderMesh`, then restores the model with RAII.
- Proved the containing S21 body renderer only accepts item indices `185`, `189..199`, `205`, `217`, `246`, `264`, `311`, `346`, and `400`. Native SS6 has no matching Grow Lancer equipment records, so the skill-local renderer remains disconnected instead of applying the mesh layout to unrelated SS6 armor.
- Added a reusable pinned-dump disassembly-window tool at `tools\grow_lancer\disasm_s21_window.py`.
- Hardened `Main.vcxproj`: `GrowLancerIsolatedBuild=true` now overrides both `OutDir` and `IntDir`, while the QA variant receives a separate output/object tree. Future isolated builds no longer rely only on disabling post-build copy events.
- Added a fail-fast MSBuild target to this worktree: omitting `GrowLancerIsolatedBuild=true` now exits with an error before compilation/linking. The negative guard test returned exit 1 and the guarded incremental build returned exit 0 to the isolated path.
- Rebuilt normal and QA x86 clients with the hardened properties. Normal SHA-256 is `0B645C4BE4321E1CE769C886CF95B1071422E5FC936351E76EABFC51026AB60A`; QA SHA-256 is `48B89DB118BAFCFF366436BDC9086A95A125407FE5F1AAAA50D69C9A055CECCD` and the runtime client was restaged with 127 junctions plus private Player/RISE data.
- Safety incident: the first rebuild in this run disabled post-build deployment but inherited the legacy `..\..\Client` linker `OutDir`, creating `Engine.exe/.exp/.lib/.map/.pdb` under the forbidden base `D:\RISE-CrossPlatform\Source\Client` at 16:44. The files were not launched and their creation timestamps prove they were generated by that build. Automated deletion was rejected by the host safety policy; they remain explicitly recorded for manual cleanup. No source or Data asset in the base checkout was changed.
- Closed the active third-master catalog mapping in `GrowLancer\MASTER_SKILL_REVERSE.md`: 13 exact rows now preserve `Brand`, independent tree parent, point gates, costs/range/delay, buff IDs and pinned-Lua formula presence. The shared contract and focused test carry the same data.
- Preserved two source anomalies instead of normalizing them: Shining Peak 699 has `Brand=277` but active-tree `ParentSkill1=698`, and its Lua comment says 688 although the function/tables identify 699. Legacy Wrath 708..710 exists only in `SkillList.xml`; active tree row 895 is used and the legacy chain remains disabled.
- Extended `verify_server_contracts.py` to hash-pin and validate all 13 active master rows, the exact active-tree row set, formula-function presence and the absence of legacy Wrath 708..710 from the active tree. Verifier, `/W4 /WX` contract test and descriptor/provenance JSON parse all PASS.
- Rechecked live processes after this checkpoint. Pegasus QA `Engine.exe` PID 664 is still running from its isolated worktree, so Grow Lancer runtime QA remains deliberately unlaunched and Pegasus remains untouched.
- Closed the target 5.2 protocol-width question: attack, multi-attack and duration receive packets decode two skill bytes; corresponding sends split high/low bytes, and skill-list persistence uses a `WORD`. IDs 271..281 fit unchanged. This does not identify which packet variant each S21 skill must use.
- Closed all nine S21 local cast-packet builders in `GrowLancer\CLIENT_PACKET_REVERSE.md`. Harsh/Spin/Magic use the exact `C1:35` selected-target byte order; Circle/Obsidian/Wrath use the same packet with the local actor index; Shining Peak/Breche use shared positional `C1:57`; Clash uses `C1:59` plus its validated destination tile and local movement call.
- Proved direct opcode transplant is invalid: RISE 5.2 already assigns top-level `0x57` to guild-create cancellation and routes native skill input through `0x19`, `0x1E` and `F3:DB`. Packet translation stays disabled until the missing S21 server handler semantics can be mapped without changing existing protocol behavior.
- Added a nine-row compile-time packet-family/target-source contract to the shared header. Every `rise52Opcode` is explicitly `-1`; focused tests verify the selected/self/position families and that no base skill has an enabled 5.2 translation.
- Rebuilt Ex603 Win32 GameServer after the master/packet contract additions, with explicit isolated `OutDir` and `IntDir`. Build and relink PASS; no server was launched or deployed.
- Decoded the exact S21 icon chain: `skill.bmd` is 750 independently XOR-decoded 120-byte records and IDs 271..279 store their direct 16-bit icon indexes at offset 74. The correct normal/disabled atlas pair is `newui_skill00.OZJ` and `newui_skill00_Non_Icon.OZJ`, both 512x512 with 25 columns and 20x28 cells. The legacy `newui_skill3` path was rejected because slots 271..279 are black there.
- Added an isolated native icon renderer for IDs 271..279 only, with dedicated bitmap IDs and exact direct-index UV math. Both hash-pinned atlases were staged under `Data\RISE\GrowLancer\Interface`; no shared SS6 atlas was changed.
- Rebuilt normal and QA clients after the icon adapter. Normal SHA-256 is `392C93B113958D39BE21A16356F4D9E935EEF959BAB13890B44C0BA4F1BC2193`; staged QA SHA-256 is `2E8AA6AA82180C9C9B15F6A194F8DAFBBE8E83F37AB7496C0AD10B428042FECE`.
- Launched only the isolated staged QA client as PID 17776. Native window inspection proved it stopped before gameplay on `Data\RISE\Config\Mix.bmd - File not exist.`; therefore no cast log or visual verdict was possible. The failed isolated process was stopped after the error was captured.
- Proved and documented a version split between the exact S21 client `skill.bmd` and the supplied server `SkillList.xml`. Core rows 271,272,273,275,276 match; rows 274,277,278,279 differ in cost and/or damage/range/delay. `verify_catalog_profiles.py` pins both hashes and both value sets. The port keeps client identity/icon/action/visual evidence separate while retaining the supplied server values for the fail-closed RISE server-authority seed.
- Added a QA-only reversible F10 icon injector. It snapshots the first 64 skill, level and delay slots plus both skill counters/current selection; it injects only when nine empty visible slots exist and restores the snapshot on the next F10. It sends no packet and is absent from normal builds.
- Built the F10/F11 revision to a second isolated output without touching the running client. `RuntimeQA2\Bin\Engine.exe` SHA-256 is `679B380461BE405037DB5B3B0F32AE3F2DF32797FF53E631E9B73F1E71EA5C0F`; it is compile evidence until restaged.
- Fixed `prepare_runtime_qa_client.ps1`: the old script excluded the production client's entire `Data\RISE` tree and then copied only the private Grow Lancer overlay, which removed required base files. It now copies base RISE into the isolated target, overlays private Grow Lancer data, and fails unless `Data\RISE\Config\Mix.bmd` exists.
- Restaged the corrected isolated client first with the F10 QA build; merged player hash is unchanged and 127 non-Player/non-RISE Data junctions remain, while Player and the merged RISE tree are private copies.
- Audited task `Reverse toàn bộ skill SS21 (trừ SS6)` and its current artifacts read-only. Its inventory and gap matrix contain zero rows for IDs 271..279; global Grow Lancer asset strings are useful leads but not completed chains. `UPSTREAM_REVERSE_TASK_AUDIT.md` pins both artifact hashes and prevents importing an upstream completion claim that does not exist.
- Added `verify_runtime_qa_stage.py` to verify every base RISE file and every private Grow Lancer overlay file by relative path/hash, plus exact Engine/player hashes and non-junction Player/RISE roots before launch.
- Added QA-only F11 pool telemetry: total and Grow-Lancer-specific live effects, particles and caster-owned pin-light/force-pillar joints plus instantaneous/average FPS. This permits before/after-lifetime cleanup evidence instead of inferring cleanup from a visible frame.
- Restaged the latest F10/F11 Engine after the telemetry build. Full-stage verification PASSes for 2,658 base RISE files, 75 Grow Lancer overlay files, the exact Engine/player hashes and physical private Player/RISE roots.
- Re-ran a filename inventory over the supplied Desktop, S21 client and RISE trees. It found many RISE 5.2 GameServer builds/PDBs and isolated package outputs, but no matching S21 GameServer executable, PDB or dump; none of the RISE binaries are accepted as substitutes for S21 server behavior.
- Added `GrowLancer\ACCEPTANCE_GATE_MATRIX.md`, a requirement-by-requirement audit separating identity, assets, each of nine visual chains, class/packet/server logic, builds, startup, owner visual QA, pool cleanup and SS6 runtime regression. It explicitly prevents static/build PASS from satisfying runtime gates.

## REVERSE EVIDENCE

- `SkillList.xml` SHA-256 `3E238C786ECAB3445A0DB4756FE3D2A3923FBC0594506BB9C3FF206020A7E0A0` supplies the exact base rows.
- `RegularSkillCalc.lua` SHA-256 `F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847` proves formulas for Spin Step, Harsh Strike, Magic Pin, Breche, Shining Peak and Obsidian.
- `MasterSkillCalc_3rd.lua` SHA-256 `771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699` plus `SkillTreeData_3rd.xml` SHA-256 `CE19B7482839524A6FC8B76563F60D0D0EAEA89F4F4E534DFC835046366539F0` prove the active 13-row third-master map and formula-presence boundary recorded in `MASTER_SKILL_REVERSE.md`.
- The S21 main dump remains the authoritative client reverse source: `D:\RISE-CrossPlatform\Source\artifacts\visual_studio\season21_client_qa\memory_dump\process_23824\400000.main.exe`.
- `GrowLancer\CLIENT_PACKET_REVERSE.md` records exact local call sites and wire append order for S21 `C1:35`, `C1:57` and `C1:59`, plus the proven RISE 5.2 opcode collision.
- `GrowLancer\SKILL_ICON_REVERSE.md` and `verify_client_icon_contract.py` pin the client table, direct icon indexes, both 512x512 atlases and isolated staging.
- `GrowLancer\CLIENT_SERVER_CATALOG_DIFF.md` and `verify_catalog_profiles.py` prove the five exact and four divergent client/server core rows without silently combining versions.

## CHAIN COVERAGE

Identity/config: VERIFIED for all nine base skills. Formula: PARTIAL. Primary/received client dispatch and caster actions: VERIFIED for all nine. All nine base client visual chains: previously reported statically verified; reopened for pool-specific dispatcher and mathematical-contract audit after the Wrath misattribution. Native client implementation: all nine receive/action routes are wired; Clash, Obsidian, Spin Step, Magic Pin, Harsh Strike, Shining Peak, Circle Shield and Wrath own isolated effect-pool routes, while Breche remains action/body-equipment-render dependent. Circle Shield's force-pillar joint, Magic Pin's clud64/smoke01 and Shockwave2 particles, every Shining Peak joint/particle/model primitive, and Harsh Strike's six-sample caster-bone blur are now native subtype-exact adapters. Breche's Armor/Pants/Gloves/Boots split and triangle-rejection primitive are exact, but its S21 equipment whitelist has no native SS6 recipient model and is therefore not connected. Server catalog: IDs/costs/ranges/delays, two-byte packet capacity, 13 active master mappings and seven scroll mappings IMPLEMENTED/recorded fail-closed; proven formula contract is tested but not yet connected to attack calculation. IDs 271..281 are explicitly blocked from default attack fallthrough. Exact packet variant, server target selection, multi-hit schedule, buffs, PvP/siege rules and Grow Lancer class authorization remain OPEN. Runtime parity: OPEN.

## ASSET PROVENANCE

Harsh Strike provenance contains eight staged assets. Spin Step contains seven. Magic Pin contains eleven. Shining Peak contains eighteen. Circle Shield contains six, Obsidian eight, Wrath nine, Clash five and Breche two. Two exact icon atlases add 719,845 bytes. The isolated package contains 76 skill/interface files (2,893,532 bytes); the merged player action model adds one generated file, for 77 private files totaling 6,331,196 bytes. All are hash guarded under the test client; no S21 or production asset was modified.

## FILES MODIFIED

- `GROW_LANCER_STATE.md`
- `GrowLancer\grow_lancer_skills_descriptor.json`
- `GrowLancer\WIND_SOUL_FIX_LESSONS_FROM_PEGASUS.md`
- `GrowLancer\HARSH_STRIKE_REVERSE.md`
- `GrowLancer\asset_provenance.json`
- `GrowLancer\SPIN_STEP_REVERSE.md`
- `GrowLancer\MAGIC_PIN_REVERSE.md`
- `GrowLancer\SHINING_PEAK_REVERSE.md`
- `GrowLancer\CIRCLE_SHIELD_REVERSE.md`
- `GrowLancer\OBSIDIAN_REVERSE.md`
- `GrowLancer\WRATH_REVERSE.md`
- `tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py`
- `tools\grow_lancer\stage_harsh_strike_assets.ps1`
- `tools\grow_lancer\stage_spin_step_assets.ps1`
- `tools\grow_lancer\stage_magic_pin_assets.ps1`
- `tools\grow_lancer\stage_shining_peak_assets.ps1`
- `tools\grow_lancer\stage_circle_shield_assets.ps1`
- `tools\grow_lancer\stage_obsidian_assets.ps1`
- `tools\grow_lancer\stage_wrath_assets.ps1`
- `tools\grow_lancer\verify_server_contracts.py`
- `tools\grow_lancer\find_s21_rel32_xrefs.py`
- `tools\grow_lancer\find_s21_u32_refs.py`
- `tools\grow_lancer\disasm_s21_window.py`
- `tools\grow_lancer\prepare_runtime_qa_client.ps1`
- `tools\grow_lancer\verify_client_icon_contract.py`
- `tools\grow_lancer\verify_catalog_profiles.py`
- `tools\grow_lancer\verify_runtime_qa_stage.py`
- `GrowLancer\RUNTIME_QA.md`
- `GrowLancer\SERVER_RULES_REVERSE.md`
- `GrowLancer\MASTER_SKILL_REVERSE.md`
- `GrowLancer\CLIENT_PACKET_REVERSE.md`
- `GrowLancer\SKILL_ICON_REVERSE.md`
- `GrowLancer\CLIENT_SERVER_CATALOG_DIFF.md`
- `GrowLancer\UPSTREAM_REVERSE_TASK_AUDIT.md`
- `GrowLancer\ACCEPTANCE_GATE_MATRIX.md`
- `Shared\GrowLancerSkillContractData.h`
- `ExGameServer\GameServer\RISE\GrowLancerServerCatalog.h`
- `ExGameServer\GameServer\SkillManager.h/.cpp`
- `ExGameServer\Tests\GrowLancerServerContractTest.cpp`
- `ExGameServer\Tests\RunGrowLancerServerContractTest.cmd`
- `ExMain_RISE_PC\Main5.2_RISE\RISE\GrowLancerRuntimeCapacity.h`
- `ExMain_RISE_PC\Main5.2_RISE\RISE\GrowLancerResources.h/.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\RISE\GrowLancerEffectRuntime.h/.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\RISE\GrowLancerRuntimeQA.h/.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\NewUIHotKey.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\ZzzEffect.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\WSclient.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\ZzzOpenData.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\ZzzInfomation.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\DSplaysound.cpp`
- `ExMain_RISE_PC\Main5.2_RISE\Winmain.cpp`
- `ExMain_RISE_PC\Main.vcxproj`
- `ExMain_RISE_PC\Main.vcxproj.filters`
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\HarshStrike\*` (isolated generated/staged assets)
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\SpinStep\*` (isolated generated/staged assets)
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\MagicPin\*` (isolated generated/staged assets)
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\ShiningPeak\*` (isolated generated/staged assets)
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\CircleShield\*` (isolated generated/staged assets)
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Obsidian\*` (isolated generated/staged assets)
- `ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Wrath\*` (isolated generated/staged assets)

## LAST STATIC CHECK

Current correction: Wrath particle renderer and radian sine verified directly from dump. Other historical PASS entries below are scoped to their original checks; they do not prove complete primitive parity.

PASS: descriptor/provenance JSON parse, source/provenance server verifier including 13 active third-master rows, focused `/W4 /WX` server contract test, client icon/table/atlas verifier, separate client/server catalog-profile verifier, complete runtime-stage verifier (2,658 base RISE files plus 75 overlay files), `git diff --check` (line-ending warnings only), LEA-256 reference vector, eighteen effect-model v0C round trips, player bone-layout/action preservation guards, converted-output hashes and all copied/generated hashes for 76 isolated files (6,302,450 bytes).

## LAST BUILD RESULT

PASS: current native-reference-corrected PC x86 `Global Release` QA build is `Engine-Port S21.exe`, SHA-256 `935F3EE895904D65D806C0193CCD5B8D80CABD1DBCCD90441BF7FFE376DD5EB1`; only the pre-existing third-party `LNK4099` missing-PDB warnings remain. Ex603 Win32 GameServer isolated SHA-256 remains `CF2D00B782CF36AD15C90FE08DCFA7CCE9B9232EDDFF58456A854B1164E0685E`. These are build evidence only, not gameplay or visual acceptance.

## LAST RUNTIME QA

Latest startup recheck: repaired QA build 935F3EE8 launched PID 24216,
responsive window title Engine-Port S21; WinMain and QA upload-disabled
breadcrumbs recorded. Waiting for owner DK login. No new cast/visual acceptance.

STARTUP/LOGIN AND ICON PANEL PASS BY OWNER; SPIN STEP VISUAL FAIL. The initial missing `Mix.bmd` and merged-Player CRC gates now pass. The first login attempt produced a minidump with `0xC0000094` at `CSimpleModulus::EncryptBlock+0x55`; exact source/disassembly proved the root-level `Data\Enc1.dat` and `Data\Dec2.dat` keys were missing from the isolated stage. Staging and its verifier now cover those files. The corrected non-mutating icon panel shows one icon per cell and was accepted. Repeated F7 casts remained stable and returned Grow Lancer pool counts to zero, but owner evidence shows only the ground ring for Spin Step; its main action/cross/trail/contact presentation is missing. The isolated client was then closed to permit a lighter S21 native-reference run.

## KNOWN DIFFERENCES

RISE Main 5.2/SS6 has no native Grow Lancer class/skeleton contract. The isolated merged player supplies the verified action clips, but the recipient class remains fail-closed. The exact S21 client table is not version-identical to the supplied server configuration for Magic Pin, Shining Peak, Wrath and Breche; both profiles are therefore explicit. Wind Soul proved that successful creation/movement and compilation can still hide wrong submitted light, a misplaced subtype constructor, a missing embedded texture, emission throttling or double FPS normalization; those checks are now explicit runtime gates for every Grow Lancer layer.

## MISSING CHAIN PARTS

Circle Shield and Clash authoritative learn/activation paths; Breche S21 equipment assets/recipient mapping and renderer connection; proven formula connection to server attack calculation; Shining Peak server line selection; Magic Pin server three-hit schedule; Spin Step server hit selection; server multi-hit/AOE/buff/PvP/siege rules; target reactions; SS6 recipient-class compatibility; isolated gameplay/runtime QA.

## BLOCKER

Client reverse/implementation can continue. Server acceptance still lacks decoded authoritative handler semantics because the supplied S21 GameServer is protected and has no matching PDB or runtime memory dump; target selection, hit scheduling, buffs and Clash PvP push therefore remain open. Recipient choice is resolved: final NEW Grow Lancer class, temporary isolated DK visual QA. Numeric class/packet/persistence contracts remain evidence-gated, not awaiting another class choice.

## NEXT EXACT ACTION

Current priority: particle-specific tick helper is implemented and unit-tested;
now map native particle fields/resources and per-slot remainder reset.
Do not reuse effect AdvanceWholeTicks unchanged: it updates at life0 and
uses EffectDestructor. Preserve native pool and unaffected SS6 updates.
Finish render eligibility/blend and emitter bone mapping before activation.
The Wrath work below is retained backlog, not the current next action.

Trace Wrath aura subtype-1 respawn stack local [ebp-0x39c] used at
0x15798CE through shared allocator argument-copy and first-render order.
Both dispatcher table entries and position copy are now verified; inherited
light is corrected and built. Constructor tail and ordinary-pool scheduling
are now decoded, and updater callers are anchored. Travel renderer arguments
are corrected. Follow actual object-render callers for frame ordering, and
travel update alpha clamp/lifetime is now corrected against 0x1579227.
Shared tick-helper compiled harness is PASS; native destructor internals remain
runtime-untested. OBJECT initialization and explicit bone writers are audited;
no ordinary-pool bone bug is established. QA now counts native Wrath particles.
Continue live pool QA preparation, object-render scheduling and aura respawn
position. Shared ENERGY/CLUD64 have separate snapshot counters now; sprite/blur
submission coverage and allocation ownership remain unverified. Do not infer
full cleanup from any one zero counter.
Gameplay regression remains open.
Alpha branch-order correction is now built/staged. Then resume Breche/Clash and the isolated
visual/pool matrix after checking live process state. Reverse protected S21
server handlers separately; production activation remains fail-closed pending
the verified new Grow Lancer class and server contracts.

## NEXT FILE TO OPEN

Current: `ExMain_RISE_PC/Main5.2_RISE/RISE/GrowLancerTick.h`, native
`ZzzEffectParticle.cpp::MoveParticles`, and `GrowLancer/BRECHE_CHILDREN_REVERSE.md`.
Wrath backlog:

`GrowLancer\WRATH_CORRECTION_AUDIT.md`, then `MoveEffect` shared tail
0x15945F1 and definitions of [ebp-0x39c] in the pinned S21 dump.

## NEXT FUNCTION TO IMPLEMENT

Current: private Breche particle initialization/update routing and per-slot
remainder reset on allocation, using AdvanceParticleWholeTicks. Alpha/scale
boundary tests still required; tick helper tests already PASS.

Correct the next dump-proven primitive/ownership/render mismatch in
`GrowLancerEffectRuntime.cpp`; actual packet registration/gameplay handlers
remain disabled until the S21 server handlers and an approved recipient-class
contract supply the missing target/hit/class semantics.
