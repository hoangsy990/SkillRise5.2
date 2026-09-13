# Pegasus high-skill port

## I/O recovery and deployment reconciliation - 2026-09-13 10:13

- Owner reports I/O resolved. Previously failing Qa15Contract source reads
  successfully; no disk events found in preceding5min. Full39/39 source
  suite now completesexit0, including former C1084 gate37.
- Before incident, Deathside first-summon mount switch was corrected to
  respect SafeZone (6604F2), with regression token guard. Spiral action/
  trace selector guard was also added. Build had succeeded before I/O fault;
  canonical Client EXE still matches recorded B923614B... hash.
- Found RuntimeClient/Delivery EXEs different despite same length/time:
  DBFA6B4C56C64CD4B999E7D0C3D9E0B7B22E993B490F095ECF8C5996784E4601
  and749AEE909CC2401277B8A1EF62486713E60FDB2A12BEB487A9DE3F72DDB48F19.
  Repeated hashes stable; byte differences1508886/4294389. Cause unproven.
  Preserved each locally as Engine-Pegasus.pre-restage-20260913-1013.bin.
- Restaged both from verified Client build; EXE+173assets now match:
  B923614BC008793288F3E50DB8D4AF2DE8126B6C46E0F67ECA787C64013F46EA.
  No production/VPS change. Latest actual runtime remainsQA5544 on earlier
  6B4317...; mounted Spiral/Deathside changes still require runtime evidence.

## Spiral mounted action branch restored - 2026-09-13

- Complete676F0D..676FC2 disproves old fixed67 claim for Spiral:
  676F46 tests safe-zone byte; helper model1E95/1E96/1EB8 selects157/158/159,
  otherwise67. Same helper predicates appear in recovered Deathside6604F2.
  Native enum names are SLEEP_UNI/DINO/FENRIR; first284 BMD actions exact.
- Added native SpiralChargeCasterAction with compile-time eight-case checks;
  WSclient action and trace both use it. Crusher65AFE4 and Elemental664CC1
  explicitly remain67; no blanket mounted-action rewrite.
- BMD provenance + Spiral routing testsPASS, isolatedclient buildexit0
  knownLNK4099, deployed and173assets verified. No runtime for this build;
  latest actualQA5544 remains preceding version. Mounted/properMG QA open.
- New lead: original Deathside6604F2 also tests safe zone but current
  SelectDeathside action switch does not. Inspect/fix separately next;
  do not mark that extra skill or full mounted coverage accepted.

## Charge input/predicate leads narrowed - 2026-09-13

- Pinned dispatch7F4E70..7F4EC9 selects Spiral676270,Crusher6599E4,
  Elemental663CE7. Anchored Spiral entry after67626D RET4; recorded
  pre-serialization helper676F0D, not a recovered weapon predicate yet.
- Skill-to-buff214/215/227 helper44F4A1 calls membership6E9E47, whose
  callers45102D/45414E use interval/time state. This is not weapon gating
  or proof that manual recast must be rejected. Recorded exact boundaries
  and discarded unaligned exploratory decode in charge audit.
- All scans terminal, no client launched/no edits to gameplay this turn.
  Latest live evidence remains QA5544. Next common input equipment gates
  or full timed-callers; no guessed weapon/counter policy introduced.

## QA5544 live regression completed - 2026-09-13 08:20

- Launched isolated6B4317... engine PID5544 at08:17:02; sequence complete
  08:20:21.492, final three pools0 at08:20:36.482. Verified exact PID path,
  stopped client and restoredLocalVisualQA.enable. Analyzer53516 terminal
  exit1 with bounds08:17:02..08:20:37; no pending process from this run.
-79dispatches, all required render rows, hardfail0, BMD9/9, Elemental5/5,
  learned15/15, buffs5/5active+clear, damage6/7,reaction6/7,45captures,
  Blow601/601,dynamic1837/1837, no pool allocation failures, all final0.
- Missing Bird positive damage/reaction remains; server declaration absent.
  Sound remains disabled intentionally, so audio acceptance fails. Havoc/
  Blow had2APPLIED each in this sample; do not attribute random variation
  to aura guards or claim a shock gameplay fix.
- Viewed Elemental phase1 image: light visible around owner. Alive-owner
  buff add/remove regression passed; dead-owner arrival not runtime-tested,
  and this DK harness is not proper-MG/class/reference parity.
- XML project inventory confirms all14 Pegasus/WindSoul runtime.cpp files
  included in compilation. This is build integration, not all-chain parity.
  Continue source/server formula/third-hit and properclass comparison gaps.

## Charge aura live guards restored - 2026-09-13

- Full39/39 source suite completedexit0 before this small gameplay change.
- Original Spiral676D12, Crusher65B0BD, Elemental664D9C all reject nonlive
  owners before buff-add effects. Added the missing early returns to three
  native adapter entrypoints, before Delete/Search/Create operations.
- Added static first-operation guard testPASS, integrated in Spiral runner.
  Isolatedclient buildexit0 knownCryptoLNK4099; staged and173asset verified
  RuntimeClient/Delivery SHA256
  6B4317D8D88E1DB700C0FCCAFBE202C2CFEA3C8F4341F34909D4B602FB8E0E72.
- No runtime launched for this new build. Last actualQA remains21152 on
  DFC4B0... . Full acceptance still IN_PROCESS. Buff callbacks inspected
  only replace their own model; they do not prove cross-charge exclusivity.

## Charge clear/save executable fixture - 2026-09-13

- Traced real user respawn ClearAllEffect, DS save EffectByteConvert and
  character-load FF rejection/slot clearing. Type0 clearing occurs at
  respawn, not proven immediate death. Save0 emits13FF bytes.
- Added native-body lifecycle generator/test, using actual seed Save/Type;
  three charges clear once and do not persist; saved non-Type0 control
  survives and roundtrips. Focused /W4 /WX compiled testPASS twice.
- Added gate to source suite (now39 listed; full39 not run this turn).
  Removal side effects/network are stubbed: no stat or live logout parity
  claim. No gameplay/binary update. Continue original replacement/third-hit
  evidence; distinct seed groups alone do not enforce stance exclusivity.

## Charge renderer lead classified - 2026-09-13

- Pinned227 comparison84C2DA is structurally the Dark Spirit rendering
  branch: mesh3, level40/20 variants, chrome RGB(.3,.6,1), terrain shadow.
  Matched native ZzzObject branch; not Elemental buff/counter evidence.
  Recorded bounded decoding and explicitly rejected unaligned84C1A0 lead.
- No gameplay edit or new build/runtime this turn. Actual server marker
  seeds Save0/Type0 and distinct groups need lifecycle/replacement audit;
  SkillPegasusSelfBuff still supplies zero stats. Continue clear/save call
  paths and original charge replacement, not numeric-ID based guessing.
  All original12 remain IN_PROCESS; previous turn was concrete QA progress.

## QA21152 reaction diagnosis and staging reconciliation - 2026-09-13

- Re-analyzed the same bounded run, no repeat gameplay: Havoc2013 and
  Blow1501 each have ten NATIVE_RANDOM_GATE and zero APPLIED observations.
  Bird241 has neither gate nor positive damage. Added per-skill reason counts
  to analyzer without waiving any missing-reaction failure (still exit1).
- Added TestPegasusReactionGateReport.ps1 exercising the actual analyzer
  block; fixturePASS, integrated in QaEvidence runner. No engine/GS change.
- Reverified RuntimeClient and Delivery executable plus173assets match
  combined DFC4B0... build. Updated full-chain audit's stale QA9688 headline
  to QA21152; previous transient Chaos exhaustion remains open.
- Continue original-chain gaps (charge stat/third-hit semantics, properclass
  visual/recast/interrupt) rather than rerunning merely for randomShock.
  Connected GS revision still unknown; local Bird server fix not deployed.

## Combined QA21152 terminal - 2026-09-13 07:53

- Verified live21152, sequencecomplete07:53:18.588; all3pools0 at
  07:53:33.579. Checked exactPIDpath, stopped, restoredLocalVisualQA.enable.
  Analyzer92544 finishedexit1 bounds07:49:47..07:53:34. No live process
  or pending analyzer from this run. BuildDFC4B0... now runtime-tested.
-79dispatches,requiredrendererallpresent,hardfail0,BMD9/9,Elemental5/5,
  learned15/15,buffs5/5active+clear,damage6/7,reactions4/7,45captures+
  12Deathsidecycles,Blow588/588,dynamic1834/1834,poolfail0,finalallpools0.
  Chaos echo39hit/1miss; do not treat ordinary hit/miss alone as bug.
- Havoc both actual shader pairs observed07:51:09 atscale1.87806 and
  9.08019, same origin perpair, ONE/ONE depthLEQUAL write0. Small and
  caster secondpasses preserved in combinedbuild. ViewedDeathcycle07:
  broad dark translucent slash remains. ViewedDragonphase1/2: layers
  appear and fade, no full reference/color/motion acceptance from stills.
- Analyzerfail: Havoc/Blow nativeShock absent in sample, Bird damage+
  reaction missing, sounds disabled. Server declarationUNAVAILABLE;
  connectedrevisionunestablished. No repeat merely to randomizeShock.
- Exactbuffstats/third-Havoc and properclass/reference/interrupt/recast
  remain open; source integration and this DK/no-bow run are narrower.

## Dex/Deathside slots fixed; combined QA21152 running - 2026-09-13

- Dex661C83..85/661CA9..AB zero-tag calls confirmed; removed cast ID from
  adjacent Kind. Deathside6605D2/660658 tag288 confirmed; corrected root
  and scythe SkillIndex. Added individual regression guards.
- Full38/38 source suite rerunexit0, clientbuildexit0 knownLNK4099;
  RuntimeClient+Delivery exe and173assets verified SHA
  DFC4B0B6744EA07B7CCA96ECBF8B2AE13E95B9A7F9CED8BC0FC69E440C6B8164.
- Combined runtimeQA launched PID21152 at2026-09-13 07:49:47; latest poll
  confirms exactisolatedpath and RespondingTrue. NOT terminal yet.
  LocalVisualQA.enable temporarily moved to
  PegasusLocalVisualQA.disabled-for-server-qa; restore ONLY after verified
  terminal and stopping this exact client. Do not launch duplicateQA.
- Next poll21152 and currenttimestamp tail, wait sequencecomplete+final3
  pool0 then analyze bounded07:49:47..terminal. Inspect Dragon images and
  Havoc both scale pairs; this run covers combined changes sinceQA18436.
  Full12 acceptance still open, no serverstat/thirdHavoc implementation.

## Charge constructors retain original zero tags - 2026-09-13

- Verified source Spiral677012..14, Crusher65B051..53/65B077..79,
  Elemental664D5B..5D use zero SkillIndex/Skill/serial. Native RISE stores
  adjacent Skill in OBJECT.Kind; removed inappropriate cast ID from Kind
  instead of incorrectly moving it into SkillIndex as for attack models.
- Four calls corrected; anchor/aura alreadyzero. Three charge math/routing
  gatesPASS, added zero-tag guardsPASS, isolatedbuildexit0 knownLNK4099.
  Staged+173assets verified RuntimeClient/Delivery SHA
  9E8F9B18FDFDCEA2C1996C594C059A64A4114AC1FD08A39BB4DFA232416F50AD.
- No runtime launched; latest actualQA18436 remains pre-Chaos/Dragon/charge
  changes. Finish Dex/Deathside individual call audit then combined QA.
  All processes complete. Exactserverstats/thirdHavoc still unresolved;
  no visual or damage acceptance inferred from these tag corrections.

## Bird/Dragon constructor and visual provenance fixes - 2026-09-13

- Pinned672B11 proves Bird241 SkillIndex; Dragon663587,663C42,6639F0,
  663B3B,663A8E,663BC5 prove242 across six paths. Corrected seven calls.
- Dragon head yaw incorrectly zero: original6639CE..6639F8 draws yaw
  before subtype. Restored rand360 in that order. Layer colors inverted:
 663BD2/663BDF CMOVNE selects subtype1 B6B16C(.0785,.314,.92), others
  B6B184(.4,.5,.92). Decoded constants and corrected conditional.
- Added slot/yaw-order/color guards; Dragon and Bird testsPASS, isolated
  clientbuildexit0 knownLNK4099, staged+verifiedexe+173assets SHA
  82D9CD543FC1D71566B19DCFB97CFC8193E1DB5564B326D85416D559A2A9415D.
- No runtime test this turn. Changes afterQA18436 still need regression,
  particularly Dragon orientation/color. Client Bird tag does not establish
  serverdamage fix. All scans/builds completed; no process left running.
- Next finish per-constructor buff slot checks then runtime these combined
  changes. Keep original12 full acceptance and third-Havoc/stat gaps open.

## Chaos/Spearstorm native SkillIndex slots corrected - 2026-09-13

- Continued Havoc-adjacent argument audit. Pinned Chaos6547C9,65483E,
  654DF5 prove2012 in SkillIndex for arc/crack/root; Spearstorm6755D3
  proves243 in root SkillIndex. Corrected four calls and added guards.
- Chaos routing and Spearstorm math/routingPASS. Isolatedclientbuildexit0
  knownCryptoLNK4099; staged+verifiedRuntimeClient/Delivery exe+173assets
  SHA90A74A5C43A570B52B72C0763B5AEEAA45E691BA5A1E783081F4387434EE9341.
- No runtime launch this turn: latest tested engine remains QA18436's
  1DB37D... . These adapters do not locally branch on effect.Skill; do not
  claim restored visual layers from tag correction alone. Added
  PEGASUS_EFFECT_SKILL_ARGUMENT_AUDIT.md with remaining per-skill candidates.
- Next compare ShiningBird/Dragon/buff constructors individually before any
  further slot edits; no blind bulk rewrite. All12 still IN_PROCESS.

## QA18436 verifies Havoc small-ring second pass - 2026-09-13

- Extended existing actual shader pair probe to one pair per scale band
  (<4 and>=4), not only first ring. No render-state/gameplay mutation.
  Native CreateEffect maps SkillIndex argument into OBJECT.Skill; verified
  before QA. Buildexit0 knownLNK4099, staged+173assets hashverified SHA
  1DB37D263C804E18937213645F57E287A3B6939B4BC98F97F39E46D06F89A3C5.
- QA18436 launched07:25:09, terminal pool0 at07:28:57.667. Checked exact
  PIDpath then stopped and restored localvisual marker; analyzer3508 done
  exit1 bounds07:25:09..07:28:58. No pending process/tool from this run.
- Actual shader pairs07:26:35.674 scale1.78248 at(20050,15550,174.5)
  and07:26:35.713 scale8.75112 at(19850,15350,169.5): base35215 +
  override35209, program3 flags66 ONE/ONE, depthLEQUAL write0; smallRGB
  (.6,.85,1),large(.55,.8,1). Confirms small impact and caster second pass
  execute after SkillIndex fix. Viewed2013phase2: small luminous rings at
  impacts plus larger caster ring. Narrow fix only, not reference parity.
-79dispatches,required renderer all present,hardfail0,BMD9/9,Elemental5/5,
  learned15/15,buff5/5active+clear,damage6/7,reaction6/7,45captures,
  Blow581/581,dynamic1836/1836,poolfail0,finalallpools0,Raining60impacts.
- Remaining analyzer failures: Bird positive damage/reaction, sounds disabled.
  Server declarationUNAVAILABLE: connectedserver revision unestablished.
  No unchangedQA repeat solely for Bird/audio. Every-third-Havoc damage,
  exactbuffstats,properclass/fullreference gates remain IN_PROCESS.

## Havoc child SkillIndex propagation fixed - 2026-09-13

- Tracing actual Nova creation found a real adapter gap: impact shockwave,
  end layer and odd-tick Nova passed SkillIndex0; root/target impact/target
  Nova put2013 in adjacent Skill instead. Native CreateEffect signature
  explicitly orders SkillIndex then Skill. Fixed all six paths.
- Pinned source:666B35->27A4 and666B9F->27A7,667219->27A2,
  666D57->27A5,6668C3->27A6,666959->27A2 all pass2013 in SkillIndex.
  In particular target/odd-tick impact shockwaves now qualify for the
  alpha_line2mono3 second pass; previous root-only fix missed these children.
- Havoc routing guard now checks each corrected argument sequence. Havoc
  math+routing testPASS; isolated client buildexit0, knownCryptoLNK4099.
  Deployed/verifiedRuntimeClient+Delivery exe and173assets SHA
  427A11DD9BBBAEE7A17893846E888E83E60168645C81D07520E17FF9DBAC7324.
- No game process running during staging. New build not runtime-tested yet;
  next QA should verify impact/odd-tick second-pass draws and image changes.
  No server damage changes; every-third-Havoc mechanics still unresolved.


## Elemental third-Havoc requirement pinned locally - 2026-09-13

- Original BuffEffect.xml227 explicitly describes third-Havoc Nova, so the
  requirement is pinned-Pegasus evidence, not merely newer official prose.
- Traced214/215/227 buff dispatch. Elemental live/existing-root check passes
  boolean0 into664CF8, whose XOR1 creates subtype1: current aura semantics
  confirmed; not a third-attack gate. Spiral wrapper tail-jump confirmed.
- Discarded unrelated227-immediate bitwise generator loop; renderer xref
  remains unclassified. Updated third-attack audit with exact boundaries.
  Next trace charge attack/other incoming effect logic; no guessed counter.
- Read-only research/docs only; scans completed, no runtime/build/deployment
  change. All12 remain IN_PROCESS with authoritative damage still open.

## Charge outgoing call arguments corrected - 2026-09-13

- Followed actual arguments, not just shared helper body: Chaos653EC6 and
  Havoc665879 PUSH0 to812810, causing immediate zero return with NO serial
  increment. Both append that zero. Prior1..50 statement describes only
  the helper's non-null branch, not actual Chaos/Havoc outgoing values.
- Updated charge audit with this correction and outgoing value origins:
  Chaos two literal zeros; Havoc packed coordinate delta and heading byte.
  No identified field here supports treating packet serial/list index as
  the charge counter. Next inspect additional incoming effect/charge paths
  and authoritative server logic rather than invent outgoing flags.
- Evidence research/docs only. No running tools, gameplay or deployment
  changes; exact every-third counter/reset/damage still IN_PROCESS.

## Charge third-attack protocol leads narrowed - 2026-09-13

- Added PEGASUS_CHARGE_THIRD_ATTACK_AUDIT.md with hash-pinned callbacks,
  input entrypoints, bounded target-list parser and direct xrefs.
- Resolved shared input helper812810: global1..50 sequence, not a three-hit
  counter. Havoc665CF4 packs coordinate deltas, not an explosion flag.
  Cast callbacks and inspected target-list route contain no third-hit gate;
  this is a scoped finding, not proof the entire client lacks another route.
- Next: complete outgoing field semantics and other incoming charge attack
  paths. Do not use sequence modulo3 or target list index as damage trigger.
  Exact explosion/stat formulas and counter/reset policy remain unproven.
- Read-only binary research and docs only; all scan processes completed.
  No client/server build, deployment, runtime session or acceptance change.

## Merge recheck and official charge evidence - 2026-09-13

- Re-ran all38 source gates against the latest integrated changes. First
  run correctly stopped at gate5: RuntimeTrace verifier still expected the
  old leading Havoc capture ternary, before Deathside's100ms branch was
  inserted. Updated verifier to require BOTH Deathside100 and Havoc150,
  preserving Bird200/default400 checks; did not weaken capture coverage.
- Full rerun completed38/38 in order, exit0. RuntimeClient and Delivery
  executable plus173assets each match build hashes. No gameplay/build/VPS
  change or runtime session this turn. Latest engine F27B1D... retained.
- Cross-checked official Webzen MG renewal guide; full-chain audit now
  explicitly separates charge weapon/range/marker routes from still-missing
  exact stats and every-third-attack damage. Official descriptions confirm
  stat families/third attacks but supply no exact coefficients. Do not use
  private-server formulas or infer the pinned Pegasus protocol from prose.
- Source integration PASS is not full merge/visual acceptance: server stats,
  third-hit mechanics, connected server Bird damage, proper-class references,
  Chaos transient allocation failure and full visual comparison remain open.
  All skills remain IN_PROCESS. No running test process left from this turn.

## Full-chain audit contradictions corrected - 2026-09-13

- Re-read original12 gap table and color audit. Updated FullChain audit's
  leading acceptance boundary to QA9688, preserved prior evidence as
  historical. Removed unproven assertion that connectedGS is an old binary;
  local build/tests do not identify the deployed server.
- Corrected stale Chaos/Havoc gap rows from183 to285/284 with183fallback.
  Replaced misleading SwordBlow regression-only claim: later tail-color
  comparison and separateFly acceptance remain open despite early owner
  motion approval. No readiness inflated from source/test PASS.
- Evidence priority: exact server stat formulas and properclass/equipment
  remain missing; connectedGS Bird damage unverified; Chaos transient
  allocation failure unresolved; full color/timing/recast/interruption
  comparison not closed by current DK/no-bow harness. Latest Deathside
  radial ribbon and Spearstorm embedded-blade checks only close narrow
  visual absence hypotheses. Original D:/Pegasus top-level dirs are client
  Data/Logs/ScreenShots/x64/x86, not evidence of supplied GS source.
- Docs only this turn. No client/runtime/server deployment or active tools.

## QA connected-server declaration added - 2026-09-13

- Isolated /qa15 now additionally sends QA15 SERVER bird-heading=target-v1
  compiled=<CommandManager compilation date/time> to its caller after safe
  start. Existing reaction-sample-v4 sequence notice remains compatible.
- Client records SERVER_QA_BUILD_DECLARATION only during serverQA, result1,
  exact12byteprefix and NUL inside header-declared notice bytes. Analyzer
  prints declaration or UNAVAILABLE. This is self-reported capability and
  module compile time, NOT cryptographic EXE identity or proof server matches
  every local source change. Do not assert liveVPS revision from localbuild.
- Client+server isolatedbuildexit0; clientknownLNK4099. QAsafety, actual
  ShiningBird route/geometry, QA reader/phase testsPASS. Deployed+verified
  RuntimeClient/Delivery executable+173assets SHA
  F27B1D0D50EC1CC2F97FBB4AC1B38831176B948991AE5C5DE86B01F95080A13E.
- New isolatedserver SHA06982BFA2C4236DD9C93FFEECA97F848B67B2260C2FBF8E6A655BCD95BD2D143.
  No VPS deployment or runtime launch. Server stamp reception remains
  untested until that server build runs. No reason to repeat unchangedVPS
  QA merely to reconfirm known Bird missing damage. Full12/15 IN_PROCESS.

## Shining Bird full route fixture added - 2026-09-13

- Expanded GenerateShiningBirdGeometryTest.py to compile actual current
  SkillShiningBird body plus actual CheckSkillRadio and native geometry.
  Attack-count macro is extracted from SkillManager.h. External GetInfo,
  target authorization, outbound echo and delay queue are explicit fixtures;
  this is not a live GameServer/packet/damage acceptance test.
- Verified exactly one accepted target queues message50 after200ms for241;
  invalid bIndex emits nothing, denied/missing-info/zero-radius/inactive-
  viewport/far targets queue no damage, behind viewport target excluded,
  combo forwarded and combo echo emitted,12 valid targets capped at10.
  Existing16direction geometry cases stillPASS; angle0 misses14/16.
- First compile caught fixture localgObj shadowing global; corrected fixture
  setup, final /W4 /WX compile+testexit0. Existing full-suite entry includes
  expanded test automatically. No gameplay/executable/VPS change this turn.
- Server source-route evidence strengthened; deployed-server version and
  positive live Shining Bird damage still required. Current client901381...
  retained; no live runtime/process from this turn. AllskillsIN_PROCESS.

## QA9688 terminal and Spearstorm image check - 2026-09-13 06:46

- QA finalpools0 at06:45:30.857. Checked exactPIDpath/stopped9688 and
  restoredlocalmarker. Analyzer29719 completeexit1, bounds06:41:44..06:45:31.
  79dispatches,all required renderer rows,hardfail0,BMD9/9,Elemental5/5,
  learned15/15,buffs5/5active+clear,damage6/7,reaction5/7,45standard images
  plus12Deathside sidecars,Blow578/578,dynamic1836/1836,Raining60terrain.
- Model/joint/particle pool failures0. Havoc native shock absent, Bird
  damage/reaction absent, disabled sound remain failures. QA8352 transient
  exhaustion unresolved; source/reference parity not established.
- Viewed QA16160 Spearstorm phases1/2 from previous verified runtime:
  multiple spears remain at impact patches during fade. This supports prior
  narrow fix preventing post-impact underground drift. No claim for exact
  reference scale/color/timing; no further spear motion tuning needed from
  the old "no embedded spears" screenshot alone.
- Currentdeployed901381... retained; no liveclient or tool session. Next
  advance other original12/reference gaps, keep Deathside later-cycle
  evidence instead of revisiting the disproven missing-early-frame inference.

## Deathside broad ribbon now captured in full-cycle window - 2026-09-13 06:42

- QA9688 emitted12 sidecars at62,140,218,297,375,453,531,609,687,
  765,843,922ms after first ribbon draw. Viewed cycle03/07/11: early
  small arcs develop into broad dark radial ribbons by609..922ms.
  Ground remains visible through these bands: not the old opaque strip.
- This changes the next action: do not "fix missing ribbon" from the
  original3phase0..200ms capture, which omitted its wider later phase.
  No geometry/lifetime/tint change was needed to expose this sample.
- Still NOT full reference parity: other scythe variants, exact density,
  fade/smoke synchronization and screenshot-induced frame pacing need
  separate evidence. Existing blue reference/background is uncontrolled.
- QA9688 remains running; finish existing sequence/finalpools then bounded
  analyzer from06:41:44, exactPIDpath stop, restore local marker. Keep current
  build901381... and do not relaunch identical QA to randomize shock checks.

## Deathside full-cycle capture staged - 2026-09-13 06:42

- Added12 bounded sidecar framebuffer samples at75ms intervals starting
  after actual Deathside ribbon draw. Standard3phase skill captures retain
  priority; sidecars use qa_PID_deathside_cycle_XX.jpg and separate elapsed
  DEATHSIDE_CYCLE_CAPTURE rows. Map join resets sidecar state. No gameplay,
  animation, blend or geometry change; screenshot time can affect frame pace.
- Purpose: inspect beyond the initial13point partial ribbon; an ~825ms+
  window spans nominal authored attack duration (~667ms) plus early tail.
  This is measured diagnostic coverage, not reference/variant acceptance.
- Buildexit0 knownLNK4099, focusedguardPASS. Verified RuntimeClient/Delivery
  executable+173assets SHA90138154E34C9831202D259861749526F1EEC50F5F7CAD67986ECBC05D0ADC26.
- StartedQA9688 at06:41:44; localmarker moved to
  PegasusLocalVisualQA.disabled-for-server-qa. Next view cycle0..11 and
  timings, finish this same QA/finalpools, checkedPIDpathstop and restore
  marker. No server deployment; full15/original12 acceptance remains open.

## Original scene update ordering recovered - 2026-09-13

- Exact original MoveCharacters entry713FE0 verified by prologue and loop.
  Four direct calls found884D30,884E2A,884ECE,8850C7. First two belong to
  distinct branches:884D87 jumps884E70, skipping the884E2A branch.
- Main branch order after characters is78F44C effects,7ABC03 joints,
  7BB992 particles. Alternative branch uses the same order at884E45..4F.
  78F44C verified iterating live effect objects and calling741068. This
  agrees with current RISE character/blur->effects->joints->particles order.
- Do not infer two blur updates per frame from two main-scene callsites;
  they are mutually exclusive. Outer frame limiter cadence remains a
  separate unverified question, not justification for changing lifetimes.
- No runtime launch or gameplay edits this turn; source-call scan11505
  finished. Current deployedDE79067... retained. Next useful validation is
  full slash progression across the authored action, not another copy of
  the initial13point sample or a speculative FPS multiplier.

## Deathside blur update caller narrowed - 2026-09-13

- RISE MoveCharactersClient ends with MoveBlurs, which decrements native
  blur lifetime/number and calls MoveObjectBlurs. Scene update invokes
  characters before effects/particles. Object-blur Deathside decrement is
  per call; only Chaos textures have accumulator gating.
- Pinned original direct-call scan794376 found0 because this entry is
  reached by tail jump794371, not directcall. Correct MoveBlurs entry is
  7942EB: nativeblur decrement then jump into objectblur updater.
- Exact caller7140D4 follows the character loop calling7112ED per object;
  original mirrors the characters->blur ordering. Helper65494F still gates
  only Chaos7F3B/7F3C, so no source proof for adding Deathside-only FPS gate.
- This narrows a suspected port timing difference, not full validation of
  outer scene cadence or ribbon coverage. No gameplay/lifetime edits made.
  Need whole-cycle/variant coverage or original outer timing evidence before
  modifying decay. Both scan sessions2898/61077 completed; no runtime live.

## QA16160 terminal audit - 2026-09-13 06:31

- Final pools0 at06:30:40.877; verifiedPIDpath, stopped16160 and restored
  localmarker. No Engine-Pegasus process left. Analyzer97202 completeexit1
  for06:26:56..06:30:41:79dispatches,required renderscomplete,hardfail0,
  BMD9/9,Elemental5/5,learned15/15,buffs5/5active+clear,damage6/7,
  reaction4/7,45captures,Blow587/587,dynamic1836/1836,Raining60terrain.
- Both model and joint/particle allocation failures0; budget snapshot not
  triggered. QA8352 exhaustion remains unresolved, not disproven.
- Current remaining run failures: Havoc/Blow native shock absent, Bird
  damage/reaction absent, disabled audio. New Deathside state/bounds data
  are valid for sampled carrier10177 only, not all3 attack variants.
- No live client or pending tool session. Latest deployedDE79067...;
  subsequent source edit corrected only obsolete alpha-test comment.

## Deathside authored elevation checked - 2026-09-13 06:30

- Original asset is Data/Skill/ScytheEffAni01.bmd (not Effect): SHA256
  27064560626BBE55181FD2711B1B5098ACFF623AD396C4ED56E7EB0BCA917411.
  Meshless,12bones,action0 has11 unlocked keys. Root Group06 parent-1
  hasZ180 andYrotation2degrees at every key; descendant trail points are
  authored around that elevated rotating hierarchy.
- Recovered original6FFBEA helper: chooses object bonepointer if present
  else global;6FFC85..6FFCC3 multiplies transformed XYZ by modelscale and
  adds objectposition XYZ. Current standalone carrier samples global bones
  then TransformPosition(true) with assigned modelorigin/scale follow that
  form. High ribbonZ alone is not evidence for subtracting root height.
- Corrected obsolete source comment claiming alpha-test; implemented minus
  blend unchanged. No geometry/color/playback change justified yet.
- QA16160 still running at06:30:17 (Spearstorm). Next wait for finalpools,
  boundedanalyze06:26:56 onward, checkedpathstop and restore localmarker.
  Then inspect whole slash progression/variant rather than repeatedly
  sampling only initial13point ribbons. No new build for comment-only edit.

## Deathside submitted geometry and GL-state probe - 2026-09-13 06:27

- Added isolated TraceDeathsideRibbonSubmission at actual object-blur
  glBegin, after segment rejection. At8+points samples once per carrier
  variant/subtype (bounded3x7), records program/texture/activeunit,
  blend factors/enabled, depthtest/func/write, alphatest, RGB/pointcount/life,
  ownerposition and complete current ribbon AABB plus first edge endpoints.
  Read-only GL queries and logging only; no render-state/geometry changes.
- Scoped QA routing guardPASS; buildexit0 knownLNK4099. Deployed+verified
  RuntimeClient/Delivery executable and173assets SHA
  DE79067B604C353398D62F2C3ADDE249A75771AB9FF22948A23EE079DCA7D27E.
- Live QA PID16160 started06:26:56, verified exactRuntimeClient path.
  Localmarker moved to PegasusLocalVisualQA.disabled-for-server-qa.
  Next read DEATHSIDE_RIBBON_DRAW_STATE/BOUNDS after first skill, finish
  this existing QA and finalpools; stop only checkedPIDpath then restore
  localmarker. All skills IN_PROCESS, no server or production changes.
- Live06:27:20.048: carrier10177, subtypes1/3/4/5/6 all13points,
  program0, texture2622, unit33984(TEXTURE0), blend0/769(ZERO/
  ONE_MINUS_SRC_COLOR), enabled1, depth1/515(LEQUAL), depthwrite0,
  alphatest0, RGB.32/.32/.32. Not an inherited active shader or alpha-test.
- Owner(19850,15350,249.50); direct ribbons sampledZ411.77..417.65,
  XY about19513..19731/14902..15150; mirrored ribbons19961..20084/
  15538..15665,Z411.68..415.44. Geometry exists above owner at this sample;
  need authored bone/pose comparison before calling height erroneous.
  Runtime16160 still running; no completed acceptance yet.

## QA4072 complete, budget failure not reproduced - 2026-09-13 06:22

- Verified original livePID4072, waited for terminal QA pools (all0 at
  06:22:06.904), checked exact executable path before stopping, restored
  local marker. Bounded analyzer06:18:35..06:22:07 session58688 exit1.
- 79dispatches; required renderscomplete, hardfail0, BMD9/9, Elemental5/5,
  learned15/15,buffs5/5active+clear, damage6/7,reaction4/7,45captures,
  Blow588/588,dynamic1835/1835,Raining60terrain,pools0.
- Both model and joint/particle allocation failures0 in this run. Therefore
  no first-failure budget snapshot emitted. This does NOT resolve QA8352's
  228failures, and does NOT establish adaptive-budget causation. Do not
  repeat identical QA solely to randomize a failure or count this as a fix.
- Deathside draw trigger06:18:48.044 at2109ms. Raining/Chaos shock missing,
  Bird damage/reaction missing and disabled audio remain analyzerfailures.
  Dex/Chaos stationary sample failure did not recur; not permanently waived.
- Original immediate7F39/7F3A scan found emitter655043/655090 and loaders
  86E6BF/86E6DC only; constructor/update dispatch uses arithmetic/table paths.
  No sound/particle density or lifetime change justified from that scan.
- Current build694AED... unchanged. No live client or pending analyzer
  session left. Next work should inspect Deathside submitted geometry/state
  or original particle switch mapping, not another unchanged random QA run.

## Chaos particle budget failure instrumentation - 2026-09-13 06:18

- Rechecked root emitter: 10 integer frames x11 radial samples x2 smoke
  types, timer progression once per crossed frame. Original655043/48 and
  655090/95 call7AECCD directly for7F39/7F3A, not FPS probabilistic wrapper.
  Current direct calls agree; do not add random suppression without evidence.
- Native RISE allocator searches only GetParticleCapacity(Position) slots.
  EffectBudget330 reduces this prefix below3000 when measured FPS<25, then
  distance-scales it (floor600). A POOL_FULL row does not distinguish prefix
  exhaustion from all physical slots live. No bypass/increase implemented.
- Added isolated first-failure snapshot: actual searched capacity, MAX,
  liveInside/liveOutside, Pegasus-live count, FPS/FPS_AVG/factor and world
  position/camera. No RNG, lifetime, emission, budget or allocation mutation.
- Buildexit0 knownLNK4099. RuntimeClient/Delivery and173assets verified:
  694AED913991D3BF8421F6005128CCB21F0F446A51A637C43DB2DB4A187CFA0B.
- QA PID4072 launched06:18:35; local enable moved to
  PegasusLocalVisualQA.disabled-for-server-qa. Next inspect first
  PEGASUS_PARTICLE_BUDGET_FAILURE if reproduced, complete QA/pools then
  stop checked exactPIDpath and restore marker. No server deployment.
  All skills remain IN_PROCESS; runtime/reference acceptance unproven.

## QA8352 completed; Chaos particle exhaustion newly observed - 2026-09-13

- Final three pools zero06:11:54.950. Checked PID8352 exact RuntimeClient
  executable path, stopped and restored local enable marker. No client left
  running from this QA; bounded analyzer06:08:14..06:11:55 exit1.
- 79dispatches, all required model render rows, hardfail0, BMD9/9,
  Elemental5/5, learned15/15, buffs5/5active+clear, damage6/7,reaction4/7,
  Blow571/571, dynamic1838/1838, Raining60terrain,45frames,pools0.
- New actual failure: Chaos Smoke_line32970/subtype1 and smoke_line0332971/
  subtype0 emitted POOL_FULL at06:09:13.949. End pools0 do NOT erase that
  transient exhaustion. Analyzer already failed it but printed POOL_FULL0
  for model pool only; added separate JOINT_PARTICLE_POOL_FULL count.
  Re-run bounded analyzer completed exit1 with228 joint/particle allocation
  failures; model-pool failures0. Analyzer tool session52390 is complete.
- Dex and Chaos caster samplers were STATIONARY this run. Raining/Havoc
  shock absent, Bird authoritative damage/reaction absent; sounds disabled
  as before. Do not rerun unchanged just to randomize failures away.
- Deathside original794376..794450 blur lifetime gating was rechecked:
  helper65494F recognizes only7F3B/7F3C(Chaos), not7F03(Deathside).
  Current Deathside un-gated decrement matches that branch; do not add
  a speculative FPS gate. Original794728 minus blend and794878 RGB*tail
  fraction agree with current code. Missing visible radial pixels still
  require submitted world-coordinate/material/depth evidence.
- Next prioritize Chaos emission density/lifetime against original cascade
  and inspect Deathside submitted ribbon geometry/state. Client hashADC5...
  remains current; no source gameplay change or server deployment this turn.

## Deathside draw-triggered framebuffer QA - 2026-09-13 06:08

- Inspected all3 QA22552 Deathside frames: Reaper/scythe poses progress,
  but these timed1500/1900/2300ms frames do not capture a broad radial slash.
  Previous opaque-ribbon fix already exists; do not reapply it from the old
  owner screenshot. Need stronger capture evidence, not a guessed tint fix.
- Isolated capture now arms skill288 and waits for a Deathside object-blur
  segment immediately before its glBegin (after segment rejection). The
  notification records DRAW_SUBMITTED_NEXT_FRAME_CAPTURE; next two samples
  are100ms apart. At5seconds without notification it emits explicit
  TIMEOUT_NO_RIBBON_DRAW. New cast/map reset clear pending state.
- Analyzer now fails missing/timeout/duplicate draw-trigger evidence;
  routing guard scopes notification to RenderObjectBlurs. This is draw
  submission, not guaranteed visible pixels or proof of all three variants.
- Initial patch matched another glBegin and failed compilation C2065;
  corrected to the Deathside-scoped object-blur renderer before rebuilding.
  Final buildexit0 (knownLNK4099), focused auto-QA/evidence testsPASS.
- Deployed+verified RuntimeClient/Delivery executable and173assets:
  ADC5C9D96588D1245EB91E4B2C430244F9F9938BC5808C1DE86C39DE64D3EAF9.
- Started isolated runtime PID8352 at2026-09-13 06:08:14. Local marker moved
  to PegasusLocalVisualQA.disabled-for-server-qa; leave separate preexisting
  .enable.disabled unchanged. Next inspect trigger+skill288frames, complete
  boundedQA/pools, verify exactPIDpath before stopping, restore local marker.
  No server deployment or gameplay/material change; all skills IN_PROCESS.
- Live8352 trigger observed06:08:37.076 at2125ms after arm; frames at
  2125/2234/2343ms. Inspected all3: scythe poses change but broad radial
  slash is still not visibly established. This narrows missing timer-event
  hypothesis: a ribbon segment was submitted, not proof it covered visible
  pixels. Next inspect actual ribbon coordinates/texture/color/depth and
  variant rather than merely moving capture times again. PID8352 still
  running; finish its QA/pool audit before another launch.

## Integration suite execution hardened - 2026-09-13

- Observed old CALL/:run batch output print its overall PASS before returning
  to RuntimeTrace and subsequent gates, with several routes repeated. Exit0
  alone was not a trustworthy ordered suite verdict. Do not retroactively
  interpret that output as complete runtime or visual acceptance.
- Replaced that batch orchestration with a PowerShell gate list and isolated
  child cmd per gate, BEGIN/END cardinalities, missing/duplicate checks and
  fail-fast exit handling. Added Deathside, Darkness, Wind Soul, input,
  dynamic-model/shader capacity and auto-QA to the existing original-12/Fly
  and server tests. The suite completed all 38/38 gates in order, exit0;
  the only overall PASS was after END38/38. This is source-test evidence.
- Deployment verifier passed: RuntimeClient and Delivery executable plus
  all 173 staged Pegasus assets match. Client SHA remains
  20D2167CEEA8F68C309D4A8324F0E185AE9829F232D6776504D8906CC23E6F6D.
  No gameplay, assets, server deployment or live runtime changed here.
- Original depth evidence: 87EF24 enables DEPTH_TEST, 87EF32/34 writes
  depth true; 87EF3A supplies203(LEQUAL) to glDepthFunc at87EF62.
  Helper87F78B..87F7A3 conditionally disables depth writes using cache
  A571903. This is original global/helper evidence, not proof of every
  Havoc draw's state. No speculative Z offset or NODEPTH fix applied.
- Havoc striping and remaining full visual/proper-class acceptance stay
  IN_PROCESS; previous QA22552 shortcomings are not waived by source gates.

## Havoc static carrier / transform path narrowed - 2026-09-13

- Inspected original Shorkwave_gun01 action0: one key, unlocked; sole bone
  Cylinder005 parent-1, position(0,0,0), rotation(0,0,0). Together with
  planar verticesZ0, no hidden animated vertical displacement was omitted.
  Native PlayAnimation returning early for one-key carriers is expected.
- Traced current Calc_RenderObject(effect,false,0,0): Animation receives
  Translate=true and embeds BodyScale/BodyOrigin in rootmatrix; Transform
  receivesfalse, forwarded through AddBoneTransform into queued Transform.
  VAOMesh::SendIndexBone withfalse uses scale1/translation0. This path does
  not apply BodyScale/BodyOrigin twice. No transform compensation added.
- AddMeshCommand stores matrix pointer; with scopeDepth0 it flushes at once.
  Do not infer a deferred pointer lifetime bug from that assignment alone.
  Havoc render uses directRenderMesh; any enclosing scope/state still needs
  actual evidence before a general queue/renderer change.
- No runtime run/build/client changes this turn. Remaining striping needs
  depth/terrain or original-frame comparison; one-key/transform explanations
  above are narrowed, not proof of source visual parity or completion.

## QA22552 executed material observations - 2026-09-13

- Havoc paired rows05:42:25.104 share origin(19850,15350,169.5),
  scale8.83858, program3,flags66,blendONE/ONE,depthtest1,func515(LEQUAL),
  depthwrite0,RGBA(.55,.8,1,1). Base35215 LINEAR/REPEAT;
  override35209 LINEAR/CLAMP_TO_EDGE. Neither pass writes depth, so this
  sample does not support self-depth-write fighting between these passes.
  Ground/depth precision and the visible striping cause remain unproven.
- Constructor665E23..665E33 only sets lifetime12 and stores initialscale;
  common665EC0 setsblend-2/alpha1. No shockwaveZ lift there. No arbitrary
  height/depth change made. Immediate8037(POLYGON_OFFSET_FILL) xref scan0
  is only a scan result, not proof against indirect/global state.
- Corrected Wrath sampler emitted all3bands05:42:59: RGB(.30,.06,.057),
  (.37,.074,.07030),(.65,.13,.12350). Each is (1,.2,.19)*pulse;
  program6,texture35195,flags66,ONE/ONE,alpha1,unlit,textured. This closes
  the sampled queued-pulse transport question, not composited/reference parity.
-22552 final3pools0 at05:44:48.043; checked exactpath/stopped and restored
  localmarker. Bounded analyzer05:41:03..05:44:49 completedexit1:
  79dispatches,required renderercomplete,hardfail0,BMD9/9,Elemental5/5,
  learned15/15,buffs5/5active+clear,damage6/7,reaction5/7,45captures,
  Blow587/587,dynamic1841/1841,Raining60terrain,pools0,noexhaustion.
  Failures are Raining shocksample, Bird damage/reaction and disabledaudio.
  No live tool session left. Staged build20D2167... unchanged. Next return
  to remaining source/visual gaps, not repeated identical random-shock QA.

## Havoc paired shader diagnostics / live QA22552 - 2026-09-13

- Added isolated HAVOC_SHOCKWAVE_SHADER_PAIR diagnostic before actualGPU
  draw, caching base texture command and logging matched override by exact
  BodyOriginXYZ/BodyScale. Records program/texture/flags/blend/depthtest/
  depthfunc/depthwrite/filter/wrap/RGBA. Stops after first matching pair.
  No depth/blend/asset/material state changed; match is geometry metadata,
  not an independently verified object-identity or per-pixel comparison.
- Buildexit0 (existingLNK4099); deployedRuntimeClient/Delivery and verified
  executable+173assets matchSHA20D2167CEEA8F68C309D4A8324F0E185AE9829F232D6776504D8906CC23E6F6D.
- StartedQA22552 at05:41:03 in RuntimeClient. Local marker moved to
  PegasusLocalVisualQA.disabled-for-server-qa; preserve separate preexisting
  PegasusLocalVisualQA.enable.disabled. No duplicateprocess/serverchange.
- Next poll22552: HAVOC_SHOCKWAVE_SHADER_PAIR and corrected
  SWORD_WRATH_SHADER_DRAW pulsebands, then finish bounded analyzer/final
  pools. Do not infer striping cause from log availability alone.

## Havoc ring texture/pass provenance inspected - 2026-09-13

- Read original Shorkwave_gun01.bmd:1mesh,1bone,1action,28vertices/
  28triangles, texture0=alpha_line2mono2.jpg, planar localZ0; SHA
  334BB0265B0C8BF76FF388C8BB9E37B588E14726247A20A99351D792054F1AE6.
- Viewed original OZJ JPEG payloads (skip24 wrapper, memory-only, no edited
  assets): Effect/alpha_line2mono3 and Item/Wings/alpha_line2mono2 are
  continuous smoky bands, without the sharp horizontal scanline-like edge
  observed in QA21304. This does not establish rasterization cause.
- Stage manifest maps original Item/Wings alpha_line2mono2 into isolated
  Effect sibling path; existing pinned hashE5F26A... verified by manifest.
  Original override loader86E2BF uses7F17, filter2601 andwrap812F, matching
  adapter GL_LINEAR/GL_CLAMP_TO_EDGE. No guessed texture substitution.
- Rechecked66660B skill2013 conditional secondpass with texture7F17,
  blend-2, blendlight1 andobjectAlpha/UV. Existing adapter preserves these.
  Next isolate first/second executed GPU pass/depth evidence or compare
  original rendered frames before attributing striping to UV/depth/filter.
  No runtime/source rendering change or new QA run this turn.

## Havoc visual presence review / reference provenance - 2026-09-13

- Reviewed actual QA21304 skill2013 phases0/1/2: bright caster arcs and
  ground circular wave are visible; outer ring expands across those frames.
  Distinct horizontal striping at outer ring edge remains a visual question,
  not a proven bug or reference match. No material/texture guessed or changed.
- Reference https://www.youtube.com/watch?v=ycd1GEL53Dg opened in hidden
  in-app tab5/browser1. Observed title: [Mu Online Season 16] Renewal Magic
  Gladiator - Havok Spear Skill, MU ONLINE GUIDES, duration25s. This is
  supplemental Season16 visual reference, not pinned Pegasus/SS21 evidence.
  Browser yielded page/player metadata only; no reference video frames were
  observed, so no frame-by-frame parity claim. Webfetch failed; no download.
- Source RenderHavocSpear remains per-model branches6663F7..66683D:
  Nova0/1pluschrome1, Line0/1differentRGB, Shockwave plus skill2013-only
  alpha-line pass. Next trace striping/material/UV against original or obtain
  viewable reference frames. No client/GS builds or runtime launch this turn.

## Shining Bird native server geometry regression - 2026-09-13

- Added GenerateShiningBirdGeometryTest.py + RunShiningBirdGeometryTest.cmd.
  Generator extracts actual current GetSkillAngle/GetSkillFrustrum/
  CheckSkillFrustrum functions and native Include/Math.cpp AngleMatrix/
  VectorRotate; exact heading assignment is extracted from SkillShiningBird.
  Disposable compiled TU stays in ExGameServer/Tests/PegasusBuild. No server
  gameplay change or alternate math implementation introduced.
- /W4 /WX testPASS:8directions at origins20/200 (16targets) are inside;
  reflected behind and4x-distance targets rejected. Old constantangle0
  misses14/16, reproducing directional defect. Test checks current native
  frustum1.5x6 arguments and is added to full-chain gate suite.
- Verified existing isolated serverSHA C58A6D3C56378E4F0BB157675676D950A832EB1C6B7928A8D8EDB9C720609479.
  No VPS/server process touched and no new runtimeQA started. Geometry test
  proves the current source correction's direction, not packet/viewport/
  damage timing on the live server or original visual parity.
- Next meaningful gaps: server runtime acceptance with the corrected GS,
  remaining matched class/equipment and source visual layers; audio remains
  disabled. Do not convert compiled geometry coverage into ingame PASS.

## QA21304 finished / Wrath executed color established - 2026-09-13

-21304 completed05:22:39.131; final3pools0 at05:22:54.139. Checked exact
  processpath, stopped and restored localmarker. No new client launched.
- Executed GPU row05:21:04.172: program6 texture35195 flags66 blend1,1
  enabled1 alpha1 RGBA(.30000,.06000,.05700,1) lit0 textured1. This equals
  authoredRGB(1,.2,.19)*pulse.30, not white. New_ModelBMD.cpp688..705
  selects bright for blend slot, multiplies Color by blendMeshLight and sets
  Alpha1. Thus flags66/Alpha1 here are expected, not lost pulse.
- First diagnostic incorrectly selected pulse bands by command.Alpha,
  so only one row with band2 was emitted despite actual lowpulse. Fixed
  sampler to executed red (authored red1). No gameplay/material delta.
  Buildexit0; stagedSHA CC29C07A9E8C3F11867DF6BBF1AE19AB91EB96A87085102B019821E470B875F0;
  executable+173assets match. This sampler-only build not runtime tested yet.
- Bounded analyzer05:19:18..05:22:55 exit1:79dispatches, allrequiredrender,
  hardfail0,BMD9/9,Elemental5/5,learned15/15,buffs5/5active+clear,
  damage6/7,reaction6/7,45captures,Blow572/572,dynamic1836/1836,
  Raining60terrain,allfinalpools0. OnlyBird damage/reaction and disabled
  audio fail. Raining/Blow shock passed without any reaction-code change;
  this supports earlier native-random-gate diagnosis, not a new fix.
- Next meaningful work: remaining source/reference/full-chain gaps and
  staged server coverage. Do not rerun identicalQA solely for green shock.
  Optional nextvisual run can gather corrected3-band diagnostic.

## Sword Wrath queued GPU diagnostic / live QA21304 - 2026-09-13

- Added isolated-only SWORD_WRATH_SHADER_DRAW in New_RenderBMD.cpp after
  uniforms/bones/VAO binding, immediately before actual draw region. Samples
  mesh0 at alpha bands(0,.35),[.35,.65),[.65,+), once each per process.
  Logs actualGL program/blend, texture, flags, commandalpha/RGBA/lit/textured.
  No color, shader, gameplay or common production behavior changes.
- Client buildexit0 and runtime trace testPASS; deployed and hashverified
  executable+173assets. NewSHA39CA6F32750E9CDBC95DEBC1B6F59A40BD754D2B4386476F0AC5874DB02D498E.
- StartedQA21304 at05:19:18. Verified exactRuntimeClient executable path,
  Responding=True; RainingArrow dispatch already progressing05:19:44.
  Local marker moved to .disabled-for-server-qa; restore after finalpools0
  and checkedstop. No duplicates/restart. PreviousQA16660 stays authoritative
  completed baseline; this newrun measures pulse in executed GPU command.
- Next: poll21304, inspect SWORD_WRATH_SHADER_DRAW threebands and compare
  RGB/alpha plus actualblend to native slot0 contract; finish boundedQA.

## QA16660 completed / native Wrath pass retained - 2026-09-13

- Sequence finished05:14:11.169; final pools05:14:26.174 all0. Checked
  exact16660 path then stopped; restored PegasusLocalVisualQA.enable.
  Bounded analyzer05:10:37..05:14:27 exit1 (gaps below, not a renderer fail).
-79dispatches, required model renderer coverage complete (Wrath2/2),
  hardfail0,BMD9/9,Elemental5/5,learned15/15,buffs5/5active+cleared,
  damage6/7,reactions4/7,sound0/15,45/45captures,Blow585/585,
  dynamic1835/1835,Raining60terrain,all finalpools0,noexhaustion.
- Missing: Bird authoritative damage/reaction (VPS still old GS), Raining
  andBlow native shock samples, disabled DirectSound. No gate relaxed.
- Wrath root10128 create05:12:37.175, renderPASS05:12:37.180,
  destroy05:12:45.170. Viewed qa_16660_skill_1500_phase_1.jpg against
  qa_25308_skill_1500_phase_1.jpg: red field visible, new sample has less
  white core; no missing root/black rectangle visible in inspected frame.
  Different runs/random layers are not controlled parity or quantitative
  color proof. All skills remain IN_PROCESS. StagedSHA3DF02F... unchanged.
- Next: actual queued-draw instrumentation if needed to verify Wrath pulse
  at GPU submission, then remaining full-chain/reference comparisons. Do
  not repeat same QA hoping for random reactions to turn green.

## Sword Wrath native body pass staged / QA16660 - 2026-09-13

- Rechecked67AECA..67AF78: accepted owner action, atlas calculation,
  BlendMesh0 at67AF37, BlendMeshLight=Alpha at67AF54. Shared default
  850D0D forwards object fields ->84DDFF RenderBody(flags2). Native RISE
  RenderBody preserves hidden mesh, texture slot blend, alpha/UV and scope.
- Replaced SwordWrath root forced-bright RenderMesh loop (-1 blend) with
  native RenderBody(RENDER_TEXTURE, Alpha, BlendMesh0, pulse, UV,
  HiddenMesh,-1). No changes to children/ground layers or other skills.
- Added focused source regression guarding slot0/body pass and rejecting
  forced bright/RenderMesh bypass. SwordWrath tests exit0; client build
  exit0 (existing Crypto++LNK4099 warning class). RuntimeClient/Delivery
  executable+173assets hash-equal. NewSHA:
  3DF02FA06A390307400CE677AA86A2F01A529873FA45C9FC6E1DC2755E5EA433.
- Launched isolated runtime PID16660 at05:10:37; verified exact executable
  path and Responding=True. Startup icon loads pass. Local marker moved
  to PegasusLocalVisualQA.disabled-for-server-qa (restore only after run).
  No duplicate process launched. Real server autoQA enabled; final/runtime
  evidence pending. Next poll16660/log, capture SwordWrath1500 and finish
  bounded analyzer; no visual-parity claim yet.

## Sword Wrath renderer config recovered - 2026-09-13

- Concrete source loader found:6C0FED passes global106B9D8 and literal
  Data/MultiLocal/Pegasus.bmd to64266E. Native size185F4C, destination+4,
  decode loop6426F8..642713. Added read-only narrow inspector; no secrets
  dumped or decoded config written. Original fileSHA DBE6F972CC699CF18BEDF9869BFE2FD2B31C3144B2A5BE3ED4CCF0DD72A5550B.
-1182914 renderer table contains4records allmodel381, thenFFFFFFFF.
  No direct27B3 or +285 match.1128D58 classifier contains8keys
  1ADC/1ADD/1B2C..1B31, thenFFFFFFFF; neither27B3 nor2320 exists.
  Rechecked4941F6 absent-key result -1. Earlier unknown-population notes
  are superseded for this pinned file's initial load, not arbitrary mutations.
- Full evidence in PEGASUS_SWORD_WRATH_REGISTRY_AUDIT.md. This is progress
  toward default material proof, no runtime acceptance or client patch yet.
- Next exact action: compare known default84DDFF RenderBody flags2 and
  forwarded BlendMesh0/Alpha/UV with current SwordWrath adapter; verify
  remaining preconditions, add regression, build/stage isolated if justified.

## Sword Wrath known handler entry guards narrowed - 2026-09-13

- Inspected all14 callback14 render entries plus complete map predicates.
  Every known handler entry guard rejects QA map33 independently of vector
  contents;4D48C0 always returnsfalse. Predicates52984B..5298A6 read the
  same CBB440 map via ECX=CBB43C. Full table in registry audit.
- Important distinction:513691 allows39/3A/41 even though its registration
  adds39/3A only. Do not use registration membership as a global behavior
  proof. Other maps still need relevant model27B3 branch analysis.
- Callback5 loader493D99 rechecked: atmost100 stride1C records, stop on
  unsigned firstfield>99. Source population1182914 and classifier1128D58
  still unknown. String xref search MainInfo/CustomModel/EffectInfo found
  no useful source loader; no assumption of empty runtime tables.
- No new build/runtime run this turn. Next locate source table population
  or inspect remaining map-specific callbacks; keep visual acceptance open.

## Registry constructors and full regression recheck - 2026-09-13

- Follow-up decoded all remaining13 factories: each zeroes map vector
  +4/+8/+C before its ownership wrapper. Exact6EFBEC xrefs found14 direct
  calls, all in known registration routine. Decoder confirmed6EFF23 checks
  current map via exact-equality6F02D5 before virtual offset+14. Recorded
  all14 vtables/render pointers in PEGASUS_SWORD_WRATH_REGISTRY_AUDIT.md.
- Known initialized registry has no map33; later mutations/wrappers still
  need exclusion before claiming original runtime callback14 is absent.
  Callback5 table source remains unresolved. No renderer guessed/changed.
- Re-ran RunPegasus12FullChainGateSuite.cmd: exit0. Re-ran deployment
  verification: RuntimeClient and Delivery executable+173assets match.
  These checks establish regression/deployment consistency, not full merge
  completeness against original effects, nor visual/runtime acceptance.
- Next: inspect ownership wrappers/later map mutations or the14 known
  render functions for27B3; retain actual QA damage/reaction/audio gaps.

## Sword Wrath registry provenance advanced - 2026-09-13

- Previous turn progressed via final25308 QA/login reset. No new test client
  launched this turn; resumed direct original renderer evidence instead.
- Original SHA verified. Narrowed callback14 construction:6C33B8 calls6EF74E,
 6BF7CB merely moves pointer/control block. Actual factory calls6EECCD then
 6EF23E. Decoded full registration6EF23E..6EF74D:14 handler factories and17
  explicit map additions recorded in PEGASUS_SWORD_WRATH_REGISTRY_AUDIT.md.
- First factory5136A9 ->512614 initializes an empty map vector+4/+8/+C;
  other13 constructors and other registration callers still open. Callback14
  not globally excluded. Callback5 table1182914 remains runtime-populated/
  unproven, not assumed empty from absent file-backed raw bytes.
- No guessed renderer patch or build made. Next exact step: inspect remaining
  factory constructors and6EFBEC callers, then applicable virtual render slots;
  separate task is provenance of table1182914. Latest staged build unchanged.

## QA25308 finished / login reset staged - 2026-09-13

-25308 completed04:44:00, final3pools0 at04:44:15.265; checked/stopped and
  restored local marker. No active test client intentionally left running.
- Bounded analyzer04:40:44..04:44:16:79dispatches, all required render rows,
  BMD9/9,15/15 learned,buffs5/5 active+clear,45captures,dynamic1835/1835,
  Blow578/578. Raining phase validation passes. All five tested F3:35 skills
  have10 CAST_ROOT_BOUNDARY markers each. Still FAIL: Bird old-GS damage,
  native reaction samples and disabled audio. No visual-parity promotion.
- Added QaCastBoundary.Reset and ResetPegasusCastRootTracking on native
  CreateLogInScene reset path, clearing both notice token and root stamps.
  Test proves first cast accepted again after reset, while repeated/old notice
  remains rejected within one session. Isolated gate test/build exit0.
- Latest staged executable SHA256
  721C836C2358F1CD30B1E671CB54A1EB4F5F8E8184A6FDAAB5518C45A6E01D12,
  executable+173assets equal in RuntimeClient/Delivery. This post-QA delta
  is login-reset only; reconnect runtime acceptance not yet established.
- Next meaningful work: resume unresolved direct Pegasus renderer evidence
  (Sword Wrath callback5 table493CE8/loader5291D0 and callback14 registry),
  or matched class/equipment visual comparisons. Do not repeat identical QA
  just to hope random shock passes; preserve existing native behavior.

## Existing target-list cast terminator / live25308 - 2026-09-13

-10272 completed04:39:31, pools0 at04:39:46.297; checked/stopped.
  QA-notice version restored79dispatches, all required render rows,BMD9/9,
  buff5/5active/clear,45captures,dynamic1837/1837,Blow599/599,pools0.
  Still FAIL: Bird old-GS damage, native reaction samples and disabled sound.
- Inspected real multi-target server paths: SkillSwordBlow and shared
  SkillDragonViolent send ALL GCSkillAttackSend echoes before one F3:35
  GCPegasusTargetsSend. Existing packet thus provides an end-of-cast marker
  for Blow/Fly/Dragon/Havoc/Raining/Chaos, without adding/changing transport.
- Moved root stamps to shared storage; exact nonempty F3:35 payload, accepted
  skill and matching caster complete that stamp. Truncated/overdeclared lists
  cannot close it. No root is created by the list. Next cast can be accepted
  immediately even outside QA notices, while intra-batch echoes remain gated.
  This implementation remains inside isolated compile boundary like the gate.
  Other skills without an explicit end marker retain legacy behavior.
- Gate test adds same-tick next-cast acceptance after completion and wrong-
  caster rejection. Tests/source guard/build exit0. Deployed verified hash
  4A2C84B83ADE91577BC2B68E05101F82E803AD1B455BD5BD2A8F5742AA1AF333,
 173assets+exe equal in both targets. No VPS/server edits deployed.
- LIVE25308, local marker saved as disabled-for-server-qa. Actual Raining
  CAST_ROOT_BOUNDARY TARGET_LIST_COMPLETE already observed. Await final
  sequence/pools before stop/restore, then bounded analyzer. QA-notice root
  identification is also active; do not call this a forced-lag gameplay test.

## Explicit QA cast boundary / live10272 - 2026-09-13

- Previous turn progressed by complete14692 QA and identifying coalesced
  Dragon cast loss. Inspected server CommandManager: numbered QA CAST notice
  is sent immediately before each SendSkillAttack, on the same connection.
- Added QaCastBoundary exact catalog-generated notice matcher (step/name/
  skill/ordinal/count), monotonic token and one consumed root per token.
  ReceiveNotice uses it only for system notices while server AutoQA active.
  Root gate uses boundary only for Hero under active server QA; otherwise
  legacy250ms behavior is unchanged. No guessed target-count or clock tweak.
- Tests cover two same-time casts, three repeated target echoes, duplicate/
  old/invalid notice, unmatched skill fallback. Gate test, runtime guard and
  isolated build exit0. Deployment+173 assets verified; current hash
  7E7338291CC91166140A8FF75D1D81B0ACD2CD0204C0ADC205E9C4907A91CBCC.
- LIVE PID10272, local marker saved as disabled-for-server-qa. Await boundary
  logs and final QA; stop checked process then restore marker, no early restart.
- This is QA root identity only, NOT a complete gameplay protocol repair.
  Ordinary coalesced cast echoes still lack explicit serial; general solution
  needs authoritative protocol contract and matched server deployment. Also
  simultaneous casts can replace an in-flight caster-animation sample; do
  not invent per-cast animation PASS when no rendered interval was observed.

## QA14692 completed / coalesced casts identified - 2026-09-13

- Previous turn fixed real reader sharing regression. Added actual shared
  reader regression to RunPegasusQaEvidenceTest.cmd, plus negative Raining
  analyzer tests (valid phases PASS; wrong endpoint, unknown phase and missing
  termination FAIL). All pass; no fixtures retained.
-14692 completed04:30:40, pools0 at04:30:55.326; stopped checked14692 and
  restored local marker. Bounded analyzer04:27:21..04:30:56 remains FAIL.
  Chaos10/10 and all renderer offsets restored, BMD9/9, buff5/5 active/clear,
 45captures, dynamic1775/1775, SwordBlow587/587,3pools0. Phase-aware Raining
  validation passes. Sound disabled, Bird old-GS damage and probabilistic
  reactions remain open. Deployment hashes still match173 assets+exe.
- NEW concrete runtime defect: Dragon40echoes but9dispatches/31duplicate skips.
  At04:30:02.324 normal group; next at04:30:06.406..409 has8echoes compressed
  into one receive burst (two2-second casts). Current250ms wall-clock gate
  accepts one root then skips7, collapsing a legitimate cast. No log loss:
  all40echoes exist, allocations/releases balance. Do NOT merely lower the
  timeout or infer fixed4targets per cast. Need a real cast-boundary/serial
  contract (and distinguish repeated-hit echoes) to preserve both casts.
  Next inspect server QA cast notices and actual packet root identity before
  altering the gate. No fresh process left running; no VPS changes made.

## QA reader sharing regression fixed / live14692 - 2026-09-13

- Previous turn progressed by phase-classifying Raining and running3024.
 3024 completed04:23:16, final3pools0 at04:23:31.342; checked/stopped,
 original local marker restored before next launch.
- Its analyzer FAIL included Chaos8/10 dispatches, no capture log rows despite
 real qa_3024_skill_2012_phase_0.jpg created04:20:46, missing first2 casts and
 one-time render/BMD rows; Dex clear row absent; destroys exceeded creates.
 Inspection identifies an agent-introduced diagnostic regression: File.ReadLines
 opens FileShare.Read, blocking live FILE_APPEND_DATA opens during old-run
 analysis. Missing log interval coincides with that concurrent reader.
 Do NOT modify skill dispatch/render from this incomplete trace.
- Replaced ReadLines with explicit FileStream Read + FileShare.ReadWrite,
 StreamReader disposed in finally. NotBefore/NotAfter bounds retained.
 Added AST-based TestPegasusQaLogSharing.ps1 using actual analyzer function:
 attempts writer append while reader is live, verifies third appended line.
 PASS. Regression guard forbids old ReadLines call / missing ReadWrite.
 No log rows reconstructed or missing data treated as PASS.
- Started same game binary D68963... live PID14692. Local marker preserved
 as disabled-for-server-qa. Concurrent bounded old3024 analysis session50949
 is running to exercise real shared log reads; its expected FAIL is old data,
 NOT14692 verdict. Await live14692 completion then bounded fresh analysis.
 Restore marker only after checked live process is stopped. No server deploy.

## Raining phase classification / live QA3024 - 2026-09-13

- Previous turn progressed by adding native reaction diagnostics. QA11340
  completed04:18:28, final3pools0 at04:18:43.387, then checked/stopped.
  Analyzer bounded04:15:13..04:18:44 remains FAIL: damage6/7,reactions3/7,
  sound disabled; visible render all required, BMD9/9,45captures,pools0.
  Many real TARGET_REACTION_GATE=NATIVE_RANDOM_GATE rows observed; no forced
  reactions. Added optional NotAfter bound to analyzer so a later login
  cannot enter a completed-run analysis. Replayed old bounded run successfully
  (expected FAIL), preserving all original non-Raining acceptance gates.
- Revalidated original executable SHA. Decoded complete6720C6..6722C0 stack,
  accounting for callee stack cleanup: ascending{270,random(-10,10),180},
  followed by source terrain+500 falling volley. Original66FDAD..66FDC2
  explicitly cleans up above terrain+600. This is authored upward termination.
- Added QA-only pointer/phase association for the known native arrow slot,
  launch and termination rows; unknown slots remain explicitly unclassified.
  No velocity, position, pitch, lifetime or collision semantics changed.
  Isolated build and Raining math/renderer tests pass. Deployed verified hash
  D68963D23ADA80FCADFE38B23E4B135C0B639589D1C66115410E473294F3C105.
- LIVE PID3024, local marker still preserved as disabled-for-server-qa.
  Fresh phase evidence:20..16 each10 ABOVE_LIMIT;15..10 each10 TERRAIN.
  Updated analyzer to require every phase launch/end count, correct endpoint,
  no unknown phase and exact raw/classified termination coverage. Not a
  blanket waiver of above-limit errors; a falling arrow above-limit FAILS.
  Source guard passes; await3024 final sequence/pools and bounded analyzer.
  Do not restart live run. After finish stop checked3024 and restore marker.

## Native reaction diagnostics - 2026-09-13

- Previous turn progressed by correcting verified trace bugs and rerunning
  authoritative4948. Current source inspection: normal victim damage branch
  uses rand_fps_check(2), excludes monster275. SetPlayerShock can return for
  dead/mounted/busy actors or leave attacking monsters in attack action.
- Old TARGET_REACTION APPLIED logged merely because the function was called.
  Now APPLIED requires current PLAYER_SHOCK/MONSTER01_SHOCK action; otherwise
  NATIVE_CALL_WITHOUT_SHOCK_ACTION. Added TARGET_REACTION_GATE diagnostics
  for EXCLUDED_MONSTER_275 and NATIVE_RANDOM_GATE. Native RNG call count,
  probability, shock implementation and damage behavior remain unchanged.
  No forced reaction or analyzer waiver. Mounted branch coverage still open.
- Source guard and isolated client build exit0, existing LNK4099. Deployed
  E32D8646EB20C1D8A4F07F05626728318975952E08773FB300BE82619A2FF319;
  executable+173 assets equal in RuntimeClient/Delivery. Fresh server QA
  launched PID11340; local marker is saved as disabled-for-server-qa. Await completion
  then stop checked process and restore marker. Do not restart a live run.

## QA4948 final diagnostic verification - 2026-09-13 04:11

- Completed04:10:38, final3pools0 at04:10:53.393. Stopped only checked4948,
  restored local marker; no test client intentionally left running.
- Analyzer -NotBefore04:07:23 exit1, but false STATIONARY failures are gone,
  Sword Blow visible renderer9/9 and authored multi-key BMD progress9/9
  (all tracked models) now evidenced. No animation/render implementation
  was changed for these results: corrected cache identity and wrap metric.
- Learned15/15, buffs5/5 active+cleared,79dispatches,45/45captures,
  dynamic1835/1835, SwordBlow586/586, Elemental5/5, pools0; no hard render
  failures or exhaustion. Missing visible renderer offsets resolved by end
  of run (partial mid-run output must not be treated as final failure).
- Still FAIL: damage6/7(Bird; corrected GS not deployed), reactions4/7
  (Raining/Blow/Bird in this run), all sounds disabled by shared user config,
  Raining above-limit cleanup. Full original/SS21 visual cadence, class and
  equipment comparisons remain independent open acceptance requirements.
- Next: classify Raining launch/cleanup against pinned source, and inspect
  native shock probability plus per-target damage trace correlation; do not
  force reactions or suppress legitimate misses merely to make QA green.

## QA evidence identity/frame-wrap fixes - 2026-09-13

- Previous turn made concrete progress (scoped QA facing/captures and server
  Bird heading fix). Current inspection confirmed two diagnostic defects:
  CAST_ACTION_TIMELINE03:58:26 shows286 advancing5.977 ->1.006 ->2.076 ->
  5.386 ->.495 ->2.633, yet maxFrame-startFrame reported STATIONARY.
  New QaFrameProgress sums forward increments, ignores downward wrap itself.
  Constant frames and reset-only remain FAIL in focused executable tests.
- Render and BMD trace caches were indexed by relative model offset only;
  dynamic offset0 suppressed Sword Blow offset0. QaEvidenceOnce keys by
  (rangeBegin,modelId), preserving separate PASS/transparent/failure states.
  Added unit test and full-suite entry, strengthened source trace guard.
  No gameplay/model/animation changed to obtain diagnostic PASS.
- Focused test+trace guard and isolated client build exit0. Deployed hash
  48BBBF395334E834D7CCE591C8BB1ACC7A5DA7181B40A200C2464A0A732B27FB,
  executable+173 assets match both isolated targets. Server QA PID4948
  armed04:07:24.844. Local marker preserved as disabled-for-server-qa;
  restore only after this live run completes and checked process is stopped.
- Already seeing multiple formerly hidden Sword Blow render rows; await
  final analyzer, not all offsets are visible PASS yet.
- Read-only original SHA revalidated. Raining above-limit branch EXISTS
  at66FDAD..66FDC2; B4AE38 bytes00001644=float600. Do not arbitrarily remove
  cleanup. Need distinguish ascending launch vs falling volley before
  deciding whether the current blanket analyzer failure is appropriate.

## QA24208 results - 2026-09-13 04:01

- Run armed03:58:01.936, completed04:01:21, final pools0 at04:01:36.441.
  Stopped checked isolated PID24208 and restored local marker. No client
  remains intentionally running from this test.
- Exactly45 QA_CAPTURE rows and45 files; repeated-step overwrite failures
  are gone. SERVER_QA_FACING present. Bird phase1 visually crosses toward
  selected target instead of stale yaw. These are QA-precondition fixes only.
- Analyzer -NotBefore03:58:00 exit1:15/15 learned,5/5 buffs active/cleared,
  79dispatches,1836/1836dynamic,565/565SwordBlow,5/5Elemental,pools0.
  Remaining: Raining/Chaos STATIONARY samples; Sword Blow renderer offsets
  0..7 and3 BMD progress traces; damage6/7(Bird missing); reactions4/7
  (Havoc/Blow/Bird missing in this run); sound0/15(config off); Raining
  above-limit projectile branch. Preserve all failing gates, not a PASS.
- Newly built server with Bird target-angle fix has SHA256
  C58A6D3C56378E4F0BB157675676D950A832EB1C6B7928A8D8EDB9C720609479.
  It is NOT deployed;24208 still ran against prior remote GS. Next safe
  local work: inspect STATIONARY traces and Sword Blow render/BMD diagnostics
  against actual paths; add directional server regression tests. Do not
  request broad server mutation or infer visual parity from this QA.

## Server QA facing/capture fixes and sound diagnosis - 2026-09-13

- Previous turn made progress: authoritative run15420 exposed real failing
  gates; no completion was claimed. Current fixes affect QA only.
- SchedulePegasusQaCapture now suppresses repeats until skill changes
  (reset on map join), eliminating the15-second window shorter than a full
  ten-cast step. Phase timings unchanged; timestamp retained for elapsed log.
- IsPegasusServerQaActive requires command sent, incomplete sequence,
  enabled AutoQA and no local/Fly marker. Accepted nonduplicate root dispatch
  restores Hero yaw toward its live nonself target only inside this scope.
  Never moves actor, never rotates subsequent multi-target duplicate echoes.
  This does not change server directional damage; test that independently.
- Added source guards. Client build exit0 (existing LNK4099), staged isolated
  SHA2565C9BCF696BBE2AEF6AFC7C8FE62C840E36B9D47AD8EE1EAF092C8A405B7B016F;
  executable+173 assets verified in RuntimeClient and Delivery.
- Sound diagnosis: latest log SOUND_PLAY=DISABLED. Read-only registry check
  HKCU/SOFTWARE/Webzen/Mu/Config has SoundOnOff=0 and VolumeLevel=0.
  Winmain skips InitDirectSound when off. Did NOT change shared registry or
  user volume; missing playback proof is not evidence of missing sound assets.
- Started fresh server QA with local marker preserved under
  PegasusLocalVisualQA.disabled-for-server-qa. Await final run before stop,
  restore marker only after checked process ends. Current executable above.
- Fresh client PID24208, SERVER_QA_FACING observed on accepted roots.
- Separate real server bug found: CustomAttack::SendSkillAttack ->
  CGSkillAttackRecv -> target UseSkill path -> RunningSkill passes angle0.
  SkillShiningBird used that constant for its directional frustum. Fixed
  ONLY this target skill to validate target index and derive byte heading
  via native GetSkillAngle(caster,target)*255/360, same encoding as duration
  packets. Frustum1.5x6, target checks and200ms damage delay unchanged.
  Added source guard; isolated server build and guard pass. NOT deployed to
  VPS, so ongoing24208 cannot validate that server change. Do not credit its
  damage results to the newly built server until deployment is authorized.

## Latest authoritative QA FAIL - actionable handoff - 2026-09-13

- Analyzer for server run15420 returned exit1. Added optional NotBefore
  timestamp filter (default unchanged) and streaming ReadLines to avoid
  allocating historical108MB log objects. Invoked with03:43:21; retained
  normal latest ARMED boundary and every existing acceptance check.
- Confirmed15/15 skill-list entries,5/5 buff active+cleared,79 dispatches,
  dynamic1835/1835, Sword Blow591/591, Elemental5/5; final3pools0, no pool
  exhaustion, no hard renderer failure.45 capture files exist.
- NOT a PASS: damage6/7, reactions5/7, BMD progression6/9, sound0/15.
  Shining Bird has no positive target damage/reaction; Raining reaction
  missing and one projectile took above-limit cleanup. Some Raining/Chaos
  caster samples STATIONARY. Sword Blow renderer offsets0..7 and its3
  multi-key model progression traces missing. Sound playback all missing.
- Seven repeated attack steps emitted6 capture log rows each (phases0..2
  twice), overwriting the same45 filenames; directory count alone does NOT
  establish exactly45 captures. Existing15000ms suppression is shorter than
  ten casts at2-second intervals. Fix the QA step boundary, not the verdict.
- QA-only facing precondition still missing for server-driven casts:
  ReceiveMagic skips Hero yaw (normal mouse input supplies it), but /qa15
  directly triggers server casts. Local harness has its own facing fix;
  server harness does not. This invalidates directional visual conclusions
  from15420 and may explain Bird direction; server damage cause still needs
  independent tracing. Do NOT rotate all real player echoes globally.
- Next: scope facing/capture fixes to active server QA, diagnose sound
  configuration/playback and missing runtime traces; rebuild isolated and
  rerun authoritative QA. No gameplay or source parity gate was waived.

## Server-authoritative QA restart and merge verification - 2026-09-13

- Local run20704 completed03:36:36.954 with blur/children/effect pools all0.
  This closes its cleanup observation, not full parity or server damage.
- Rechecked pinned-image callsites in0x650000..0x680000: only66139D calls
  FPS wrapper7BB8A0. No speculative mass FPS-gating changes were applied.
- VerifyPegasusDeployment.ps1 passed again: current executable plus173
  Pegasus asset hashes match Client -> RuntimeClient and Delivery.
- Temporarily renamed RuntimeClient/PegasusLocalVisualQA.enable to
  PegasusLocalVisualQA.disabled-for-server-qa (content preserved). Stopped
  completed isolated process20704 only after checking its executable path.
  Launched unchanged Engine-Pegasus.exe PID15420 from RuntimeClient.
- Server QA run armed03:43:22.240, /qa15 sent03:43:27.233. Real skill-list
  add packets and CAST_ACTION_TIMELINE are arriving; local injection is off.
  Await final sequence/pool/analyzer result. Do not call this full parity.
  Restore the saved local marker only after stopping this completed run:
  changing that marker while it is live could activate local injection.
- Full source-chain gate suite returned exit0 again. Isolated GameServer
  build returned exit0 to ExGameServer/Tests/PegasusBuild/Server only.
  No server executable or configuration was deployed to the VPS.
- Server run15420 completed all15 at03:46:36.505; final blur/children/effect
  pools all0 at03:46:51.502. Stopped only this checked isolated process and
  restored original local marker. Analyzer is evaluating this bounded run.
  Screenshot qa_15420_skill_288_phase_1 shows the Reaper beside the player
  without the former huge opaque ribbon, but does not cover every attack
  variant or prove reference timing. No acceptance status promoted.

## Dex Booster omitted FPS-normalized flare wrapper - 2026-09-13

- QA19756 completed03:29:22/all pools0. Dex flare emitter66131C..6613A8
  calls7BB8A0, NOT direct7AECCD. Decoded7BB8A0..7BB8D9:6FA6F2(1) gate
  then particle allocation.6FA6F2 uses random[0,1] <=min(1,frameFactor);
  inspected constants1,0,1. Native RISE rand_fps_check(1) is equivalent.
- Port had direct CreateParticle on each unlocked update. Replaced only
  Dex flare-emitter call with native CreateParticleFpsChecked; keeps random
  scale draw before gate, subtype5, source RGB/position/lifetime unchanged.
  Added guard forbidding the direct-call regression. Focused tests and
  isolated client build exit0 (existing LNK4099). Runtime density pending.
- Deployed isolated only SHA256CA34B4D522900CF59484A86DD7EA72124C87AEE462853EF42944201BC627018B,
  PID20704. Per AUTO_QA_LOGIN-bounded log, prior03:26:49 run allocated326
  texture32988/subtype5 flares; fresh03:34:03 run allocated54 and had already
  advanced past Dex by03:34:53. This approaches expected~60 for two30-tick
  emitters with random FPS gate, instead of frame-rate-amplified density.
  Earlier unbounded AUTO_QA_RUN-only count was invalid because these runs
  use AUTO_QA_LOGIN boundaries; use both marker types in future analysis.
  Final pools were all0 at03:36:36.954. Full visual parity remains pending.


## Dragon Violent actual head draw probe - 2026-09-13

- QA13888 completed03:24:32/all pools0. Revalidated original executable SHA.
- Re-read853853..8538AF: head27C3 copies object RGB and draws mesh0,
  textured flags2, blendMesh0, alpha as blend gain. Staged dragonhead.bmd
  SHA80694ACF42C65E5741D50F9A1B8F5F8B5E619B515438EEEF146B6D342DE9CB15
  has1mesh/7bones/1action, texture index0 drhead.JPG. Current source matches
  that pass; New_ModelBMD should add RENDER_BRIGHT for matching blend texture.
- Added one bounded DRAGON_HEAD_SHADER_DRAW immediately before actual
  queued draw to verify GL blend, material colour and lighting. No visual
  constants, model or gameplay changed. Isolated client build exit0.
  Fresh runtime draw evidence pending; do not assume transparency from source.
- Deployed SHA256D837A8C889ED2CB44B478000E62ED6E1A1F73ADB0459BD5D3A8D0A2E88CC2D38,
  PID19756, isolated runtime/delivery only. Existing live QA reached241 at
  03:28:22; wait for242 draw and final pools, no premature restart.
- Added read-only VerifyPegasusDeployment.ps1: checks nonempty build asset
  tree, executable SHA and every staged Pegasus file against both targets.
  Passed executable +173 asset hashes each for RuntimeClient and Delivery.
  This confirms build->staging consistency, not completeness against original
  Pegasus or full runtime acceptance. Extra target files are not deleted.
- Actual draw03:28:37 DRAGON_HEAD_SHADER_DRAW: program6, texture35202,
  flags66, ONE/ONE blend enabled, RGB.0785/.314/.92 alpha1, lightingoff.
  Confirms current head uses additive blend rather than opaque fallback;
  no material change justified by the old opacity report alone. Original
  brightness/radial timing and full visual parity remain unverified.


## Spiral Charge missing particle render alpha - 2026-09-13

- QA12624 completed03:17:46/all pools0. Revalidated pinned Pegasus SHA.
- Original emitter67747B..6774DF emits7F1C/subtype12 and7F1B/subtype4
  (Spiral smokelines03 and Steam_01). Render dispatch7CDE67..84 selects
  both IDs for7CDE94..7CDECC: temporary RGB*particle Alpha then normal
  sprite draw, no dark-blend override. Port updater changed Alpha correctly
  but default RenderSprite ignored it, leaving these particles too bright.
- Added scoped two-texture render branch with temporary spiralRenderLight,
  preserving stored Light/default blend. Added source guard and bounded
  SPIRAL_PARTICLE_RENDER_ALPHA for both variants. Spiral/Chaos guards pass;
  isolated client build exit0, existing LNK4099. Runtime verification pending.
- Deployed isolated runtime/delivery SHA256
  B2340BB78F672C8ED8958E9089E099A4768632681AFD4CACCAA3998C6A321639,
  launched PID13888. Process confirmed responsive; let its existing QA finish.
  Baseline12624/2014/phase1 inspected (bright orange caster aura), but one
  composite screenshot cannot isolate steam/smoke alpha contribution.
- Runtime03:23:00 verified both variants: smoke Alpha.0364 turns stored
  RGB1/.1/.05 into.0364/.0036/.0018; steam Alpha.8969 turns.55/.3/.1 into
  .4933/.2691/.0897. This proves both missing per-draw alpha branches are
  exercised. Full composite appearance/timing acceptance remains OPEN.


## Chaos Blade missing smoke03 dark-blend route - 2026-09-13

- Inspected QA12480/2012/phase0: purple ribbon exists but center heavily
  additive/white. Do not call missing purple ribbon or full visual PASS.
- Pinned Pegasus SHA revalidated. Emitter655050..655095 selects7F3A for
  smoke_line03/subtype0. Particle renderer7CDE67 subtract chain resolves
  7F3A ->7CD03D ->87F992, which sets ZERO/ONE_MINUS_SRC_COLOR;7CD042
  passes stored RGB unchanged. This was absent from the imported renderer.
- Added a dedicated kChaosBladeSmokeLine03Bitmap render branch using
  EnableAlphaBlendMinus and stored Light (NO extra alpha unlike Deathside).
  No other Chaos texture or colour changed. Added bounded runtime
  CHAOS_SMOKE03_BLEND and scoped source guard. Focused guard passes;
  isolated client build in progress. Runtime/visual validation pending.
- Build completed exit0 (existing Crypto++ LNK4099 only), isolated runtime/
  delivery SHA2569FB134D7107675119D15C6534219985F2B459A66929AF1222CE02499D3D01AF7.
  Launched PID12624 after QA12480 final pools0. Wait for this run's2012
  captures and CHAOS_SMOKE03_BLEND before any next restart.
- QA12624 at03:15:44 confirmed CHAOS_SMOKE03_BLEND src0/dst769 ENABLED.
  Inspected2012 phase0: previous central white wash becomes purple smoke;
  purple outer ribbon remains. This verifies the missing branch correction
  visibly, not full Chaos animation/intensity/timing parity. Full source-chain
  suite rerun exit0. QA12624 still running other skills at last inspection;
  wait for its final pool snapshot before redeploying again.


## Shining Bird corrected capture evidence - 2026-09-13

- Inspected QA12480 skill241 phase0/1/2 at156/359/562ms. Visible authored
  bird enters from upper-left/rear, crosses the selected target in phase1,
  and departs to the right in phase2; smoke/star/wind streaks remain visible.
  This closes the narrow missing-BMD-in-these-samples question, not full
  source colour/brightness/timing parity or GS-authoritative damage.
- Previous phase1 at650ms had missed the crossing, and older stale-yaw QA
  could not establish direction. Retain corrected captures as new evidence;
  do not retroactively call the earlier captures visual PASS.
- QA12480 reached final cleanup03:06:22, all three pool counters0. No
  restart needed. Source-chain analyzer remains server-authoritative and
  requires AUTO_QA_COMMAND/CAST_DISPATCH/skill provisioning/damage; do not
  weaken it to accept LOCAL_VISUAL_DISPATCH. The local captures above are
  renderer evidence only. Full class/equipment action parity remains open.
- Rerunning local-QA source guard caught a stale contiguous condition after
  the separate Fly-preview exclusion had been added. Updated guard to
  require BOTH local and Fly exclusions before /qa15 send (not a relaxation).
  Local-QA guard now passes; Shining math/routing tests exit0. No gameplay
  code changed during this verification turn.


## Local QA target-facing correction - 2026-09-13

- Shining Bird review found local injected QA bypassed the input-side turn.
  ReceiveMagic only rotates sc!=Hero, intentionally: real Hero input already
  faces target. Local QA was calling ReceiveMagic directly with stale yaw,
  producing unreliable directional screenshots (also affected Sword Blow).
- Added FacePegasusLocalVisualTarget immediately before local dispatch in
  QA15 and separate Fly preview. Uses native CreateAngle, skips self buffs,
  changes only yaw (no movement/target-position writes), isolated-test scope.
  Added trace LOCAL_VISUAL_FACING and guard for call order/no movement.
- Guard/build exit0; deployed B2390A7C28C0EDD70E18574DD34A8CD356046DF83CC9F1B06E6C944FBA80A9CD
  only isolated runtime/delivery, PID8016. Fresh trace02:59:46 shows yaw135,
  source19850,15350 target20050,15550. Await corrected directional captures.
- Read-only pinned Pegasus SHA reconfirmed. Shining renderer0x673945..
  0x6739F4 confirms four lights/bone, scale(1-i*.25)*4.5, offset i*22 and
  frame marker int(frame*1000). Constants verified from image, no change
  needed. Staged bird BMD8A347D42..FD5F41 has1mesh/75bones/2actions.
- Prior QA directional screenshots are not proof of game-facing failure or
  parity. Do not change production dispatch based on injected QA yaw error.
- QA8016 completed03:01:48/all pools0. Inspected241 phase0: bright bird body
  visible behind caster; phase1 shows departing streaks. BMD action0 has
  13keys, bone1 localY749->523->362->209->-12->-307..-1994, i.e. authored
  rear approach/forward departure. No basis to remove this animation.
- Improved QA241 capture window to150/350/550ms (was250/650/1050) to catch
  the key4 crossing near320ms. Capture-only source change, guard updated;
  candidate build pending deployment/runtime. Full visual parity still open.
- Capture-window candidate build/test exit0 and deployed isolated only:
  SHA2562BC62FE1F173AFEDEEBC3ABB0D1174B998246C14E53CED08FAE20A03051F3470,
  launched PID12480. Wait for this live QA run, do not restart just because
  captures have not yet reached skill241. Next inspect its three241 frames.


## Sword Blow isolated framebuffer calibration - 2026-09-13

- Added one bounded glReadPixels before/after the actual tail mesh draw,
  excluding saturated samples and blue delta<3. QA8800/build950F0737 at
  02:50:30 measured3529 pixels, summed RGB delta48938/53061/79862,
  ratio.61278/.66441/1. This matches the previous143/233,155/233 material:
  shader does NOT erase the tint. Reference video background-subtracted
  samples instead measured about.394/.476/1 across two frames.
- Updated mesh1-only candidate ratio to.394/.476/1, blue energy unchanged.
  This is measured image-derived compatibility tuning, NOT a recovered SS21
  constant or full visual acceptance. Added exact candidate guard; focused
  math test and client build exit0. Corrected misleading .65 provenance
  comments in source/color audit. QA8800 completed02:51:46/all pools0 before
  deployment of new candidate. Fresh runtime measurement pending.
- Candidate build7CC6252EAE692C0F61004ED55B442AE6FCED9F8D4FE89E5266CF4783CFBD8CEA
  deployed only RuntimeClient/Delivery, PID24588. At02:53:53 new isolated
  framebuffer sample1634 pixels gives delta10358/12501/26610 and ratio
  .38925/.46979/1. This is substantially closer to reference delta than the
  previous.613/.664/1. Inspected1501 phase1: projected blades are near-vertical
  and no clean long-tail comparison; full-scene visual acceptance still OPEN.
- Isolated GameServer build exit0, SHA256
  5DDE88FE82EAB7D598D09D9F7728FD1DAA03E27139BB57277BD9757E2416ED3B.
  No server process launched or production/VPS deployment performed.
- Fresh Havoc action trace02:53:34 selected=actual284, speed.4, frame
  .327->1.326->2.493; confirms imported action advances. Full timing and
  visual acceptance remains separate from this action probe.


## Actual queued Sword Blow draw and gate refresh - 2026-09-13

- QA19824 completed02:39:45/all pools0. Post-RenderMesh GL sample had
  program0/blend disabled/currentColor white: this is outside the queued
  draw, NOT evidence of lost shader colour.
- Added isolated-only bounded diagnostic immediately before glDrawElements
  in New_RenderBMD.cpp. Build exit0 (existing Crypto++ LNK4099), deployed
  SHA2566042EC8FAD9F64BB532F0255FBC9A52087C4C9EC2E8FD747CC6DAA232FE2E0B3,
  runtime PID19956. At02:43:27 SWORD_BLOW_TAIL_SHADER_DRAW records program6,
  texture35453/GL3299, additive ONE/ONE enabled, RGBA
  .04838/.05244/.07882/1, lighting disabled, texture enabled, flags66.
  Palette was NOT changed. This verifies draw arguments/state, not the
  resulting framebuffer colour or SS21 visual parity. Next isolate layers
  and compare against registered reference frames.
- Full-chain suite rerun caught a stale framebuffer capture guard requiring
  literal +400. Havoc now intentionally samples every150ms (other skills400),
  so guard updated to require that conditional. Failed run is NOT PASS;
  rerun in progress. Asset-preflight source guard passed.
- Rerun completed exit0 after guard update. QA19956 completed02:44:43:
  all three pool counters0. Read-only merge check found all14 Pegasus/
  WindSoul runtime translation units in Main.vcxproj. Dependency check on
  actual RuntimeClient recovered assets resolved all33 embedded texture
  references. These are scoped wiring/assets checks, not proof all visual
  layers, timing, gameplay or authoritative server behavior are complete.


## Sword Blow reference provider located read-only - 2026-09-13

- Rechecked New_ModelBMD MakeShaderType and New_RenderBMD SendUniform:
  command.Color reaches BodyLight, including blend multiplier; no source-
  proven RGB loss. Added bounded SWORD_BLOW_TAIL_GL_STATE after mesh1 draw
  (normal1501 alpha>.5): current program, GL texture, blend factors/enabled,
  fixed-pipeline current color. This is a post-call state diagnostic, not
  necessarily the submitted shader uniform if the renderer queues/restores.
  Build exit0, existing LNK4099; fresh local QA pending result. Gameplay and
  palette unchanged. QA5560 had completed02:25:36/pools0 before restart.

- Located video_reference/media2_contact_sheet.jpg and full1280x960 frames
  under Source/artifacts/visual_studio/qa/season21/sword_blow. Inspected sheet:
  pre-cast frame00001 and trail frame00031 provide same-ground comparison.
  Added read-only MeasureSwordBlowReferenceDelta.py; ROI800,560..1250,880,
  excluding saturated pixels, frame31 produces33345 blue-delta samples with
  median display-space delta ratio .392/.479/1. Quiet pixels88976 mean abs
  delta3.93. This is exploratory screenshot evidence, NOT linear material
  RGB or proof of exact registration; do not copy ratios into shader blindly.
  Next compare additional reference frames and a layer-isolated local shot.

- Original SS21 trace located read-only at Source/artifacts/visual_studio/
  season21_client_qa/Logs/season21_skill_trace.log and sword_blow_injected.log.
  Confirmed6A3 mesh0flags2/mesh1flags42, blendMesh0/1, blendLight1 and
  textureOverride=-1 (e.g.tick929017421, alpha.012346). These records do NOT
  include resolved texture binding or final GL color. Existing .65 transfer
  is preview compatibility behavior, not a literal SS21 caller parameter.
- QA5560 finished02:25:36 with all pools0. Inspected1501 phase0/1: no clean
  isolated long-tail color sample; do not infer parity or failure from the
  strong overlapping caster/target lights. Next need SS21 resolved-material
  evidence or controlled layer-isolation diagnostic, not another blind tint.

- QA5560 at02:24:20 logged texture35453 at all3 alpha bands bound to
  Data/RISE/Pegasus/SwordBlow/Effect/Swordeff_mono.jpg. Encoded asset hash
  F655B5FC0C3FC8B930C6B11EA2A1A3B1C473FEC42A7427A9CAD579B3168AE8B0.
  No custom-item texture collision at this sampled tail binding.
- Re-inspected preview small2 BMD hashE25ED37C: embedded mesh1 still
  Swordeff_mono.jpg, but preview directory has mono2/mono2line, not mono.
  OpenTexture's same-name fallback means asset inventory alone cannot
  establish its actual SS21 material. Need final live SS21 binding/trace or
  layer-isolated framebuffer comparison; do not rename/swap texture blindly.
- Current isolated buildB53AFA47560861889F96F9DCA2888831E1A2E0E54ED81FF6F6B77542C54599FA
  PID5560. Runtime white-tail visual acceptance remains OPEN.

- Read preview ZzzObject.cpp2648..2680: same split body/bright mesh, tail
  Alpha*.65. LoadData.cpp OpenTexture uses embedded filename, then same-name
  fallback only; no mono2 alias found in that binding path. Do not substitute
  its implementation as a new fix; current Pegasus already has the split.
- Current shader snapshots BodyLight and applies blend multiplier; native
  fixed pipeline also uses BodyLight*blend. No proven color discard there.
- Added bounded SWORD_BLOW_TAIL_BINDING alongside existing submit trace to
  print live bitmap FileName (not just ID). Build exit0, existing LNK4099.
  QA19468 completed02:17:21 with pools0 before path-validated restart;
  new runtime pending trace. White-tail owner rejection remains OPEN.

- The editor Source/tools sword_blow_provider_smoke.py lists mono2 assets
  but is only a manifest, not binding proof. Actual isolated provider is in
  Source_PC_SkillPreview/ExMain_RISE_PC/Main5.2_RISE/RISE/
  SkillPreviewRuntimeBindings.cpp (LoadSwordBlowPreviewAssets near660).
  Read only; never modify/rebuild/deploy that worktree from this task.
  Next trace its AccessModel/texture registration and actual render contracts
  for kSwordBlowBladeModel; do not swap mono2 based on inventory alone.

## Havoc omitted second mesh recovered - 2026-09-13

- Inspected early QA19468 skill2013 phase0/2: ring grows from near caster
  feet to a broad surrounding arc by350ms. Narrow presence/expansion evidence
  now exists; do not confuse650ms capture absence with a missing layer.
  Full video-reference color/opacity/rotation timing remains OPEN. Build
  96AB4B053E0610DD61CF68E5FFC4C6953F03ECC7204FC825E896C7EA59D4699C.

- Inspected qa_9272_skill_2013_phase_0.jpg (250ms): broad bright circular
  arc visible around caster. Phase1 at656ms was too late for12-tick root
  shockwave (~480ms), so its absence there is not missing-render proof.
  Added Havoc-specific capture50/200/350ms; gameplay unchanged. Build exit0.
  QA9272 completed02:11:56 with pools0 before restart for early captures.

- Inspected fresh qa_9272_skill_2013_phase_1.jpg: visible blue caster/target
  layers and Nova clusters, but cannot yet confirm full rotating ring/outward
  explosion. Do not label the source mesh correction visual acceptance.
- Rechecked shockwave66653D..66664F: playback.3, RGB*Alpha, mesh0 bright,
  gain1 and skill2013-gated7F17 override agree with current source.
  Shorkwave_gun01 BMD is a single flat mesh (localZ0). Original667086..667116
  stores terrain height directly, no added Z offset; constructor665E23..33
  preserves initial scale/lifetime12 with no height lift. Do not invent a
  +Z workaround. Next inspect native depth/material submission and geometry
  state if the root ring remains obscured.

- Rechecked complete line renderer66664F..666747 and shared tail666830.
  First pass6666F2 draws mesh0; second666747 jumps666830 which pushes1.
  Port incorrectly drew mesh0 twice. Restored mesh1 for second blue pass,
  keeping exact colors/alpha/flags/default texture and native animation.
  Added focused guard requiring both mesh0 and mesh1 in line renderer.
- Test/build exit0, existing LNK4099. QA15156 previously completed02:06:05
  with pools0 before path-validated restart. Staged build
  782248432E2133FDD8247BE0A161A93BE1D094602E8C0E95544523F36EDD57FA,
  isolated PID9272. Pending fresh Havoc framebuffer, ring/explosion acceptance.

## Spearstorm post-impact generic movement defect - 2026-09-13

- Corrected build96DE9DA42790A2ABF6D24D2905486AF14C9B793ED7F912D6DAB56593E6AD0E1A
  QA15156 now proves retained impact position: at02:05:35.494 life65.58 and
  at02:05:35.844 life56.83 both pos19621.0,15402.9,229.5, vertex heights
  -215..205.5 and265/384 above terrain. No continuing underground drift.
  Inspected qa_15156_skill_243_phase_2.jpg: multiple embedded spear models
  visibly remain at impact patches during fade. Narrow missing-embedded-
  spear defect is runtime-confirmed fixed; full original parity still OPEN.

- Added bounded three-phase blade pose diagnostic (world vertex heights
  using the native translated bone matrices, no gameplay tuning). QA7508
  at02:00:43..44: airborne384/384 vertices above terrain; immediate impact
  198/384 above, range-270..158; fading blade0/384 above, range-1334..-906.
  This established actual sub-terrain rendering rather than missing model.
- ZzzEffect post-update switch fell through to MoveParticle(o,true) for
  Spearstorm. Its recovered updater already owns Direction motion and stops
  it at impact; generic motion kept applying/rotating it afterwards.
  Added root/blade/crack bypass alongside existing owned-motion exceptions.
  Focused routing regression/test/build pass; existing LNK4099 only.
- QA7508 completed02:01:13 with pools0 before path-validated stop/redeploy.
  Fresh corrected runtime launched; verify position retention and framebuffer
  embedded-spear appearance before calling this visual issue accepted.
- Asset audit: startup preflight passes and all33 embedded texture references
  resolve beside staged BMDs. This is packaging evidence, not full merge parity.

## Elemental sustained flare diagnostic - 2026-09-13

- QA5292 completed01:51:11 with all Pegasus pools0 before path-validated
  stop/deploy. Diagnostic build A82EC84EC7A291CF0C3756F86A8C603604B9B57EEF13EFECBE80A8B2DF4B7E2E
  is running as isolated PID17996. Build exit0, existing LNK4099 only.
- Added five bounded ELEMENTAL_TIP_INPUT events and moved Elemental capture
  to3000/3400/3800ms, after initial bright cast layers. No visual tuning.
  At01:54:20 all five source bones3/6/9/12/15 emit sprite slots12..16,
  textureIDs32959..32963, pulses>.75, with tips within~110 units of root.
  This rules out absent submissions/double-world-origin for those samples,
  not final texture binding or correct relative placement against arrow mesh.
- Inspected qa_17996_skill_2016_phase_1.jpg: red/green point flares visible
  near feet at sustained phase. Original tip alignment/intensity acceptance
  remains OPEN; do not call the owner's missing-flare report resolved.
- Rechecked Spearstorm675848..6758E1 renderer against current branch: scale,
  texture flag2, alphaBlendLight/.7, mesh0 and default material agree. Latest
  QA5292 phase1 shows impact layers but no clearly embedded spear; next
  inspect actual blade pose/geometry after impact, not arbitrary lifetime.

## Deathside missing particle render overrides recovered - 2026-09-13

- Next concrete investigation: Elemental arrow-tip flares. Rechecked original
  664B27..664C48: Calc_RenderObject(false), TransformPosition(false), owner
  carrier/scale1 sprite and five bright mesh passes match current source.
  Do not revert to Translate=true or add guessed flare layers. QA5292 skill2016
  phase1 is dominated by initial gold cast layers, not sufficient to judge
  sustained arrow-tip anchors. Capture later persistent phase and inspect
  per-layer sprite allocation/world position/texture before changing it.

- Staged smoke build F30D0A9B9AB1F4A0C5471F582E1637368ACA49AF8DF1B9A324FB542EB1F771BA
  and launched isolated PID5292. Inspected qa_5292_skill_288_phase_1.jpg:
  previously blown-out white ground cloud is absent; radial translucent
  ribbon remains. This narrow framebuffer regression check passed; complete
  smoke density, all three scythe variants and original-reference timing
  are still not accepted. QA5292 remains running at this checkpoint.

- Prior QA22336 ended01:42:59 with all Pegasus pools0. Its radial ribbon
  framebuffer passed the narrow opacity defect check, not full visual parity.
- Found remaining bright-white cloud cause: native generic particle rendering
  uses additive RGB without Alpha. Pegasus waterfall7E22/subtype116 dispatch
  7CDBD1..7CDC1B multiplies RGB by particle Alpha and calls87F992 (minus).
  Ground smoke7F06/smoke7F07 dispatch7CDE67..7CDE76 to7CDC71..7CDCAE does
  the same, then RenderSprite88129C with original size/rotation/UV.
- Added isolated Deathside particle renderer branch for only those three
  resources: temporary Light*Alpha + EnableAlphaBlendMinus. Stored Light,
  assets, spawn density and unrelated Deathside ground-star/damage untouched.
  Routing guard/build exit0 (existing LNK4099); fresh runtime pending.

## Deathside opaque ribbon regression recovered - 2026-09-13

- Deployed blend/capture build SHA256
  8B3B9F1E55618D0A2F62AFB97B10279D43F2C500A831303AED01CBBD6625C55C,
  live isolated QA PID22336. Inspected qa_22336_skill_288_phase_1.jpg:
  broad radial dark slash now transparent over terrain, no opaque black strip.
  Phase0/2 show Reaper animation progressing. Ground cloud remains excessively
  bright white versus owner reference: investigate particle subtype renderer,
  texture and blend before retinting. Full Deathside visual parity remains OPEN.
- RunPegasus12FullChainGateSuite.cmd completed exit0 this turn, including
  item learning and server routing. Static integration gates are not proof
  that all original visual layers have been recovered/merged.

- Owner e2ecf946 screenshot confirms ribbons now appear but are opaque black.
  Recovered 65FF4D recognizes type7F03; 79471F jumps794728, calling87F992.
  At87F9B0..B7 the blend factors are ZERO / ONE_MINUS_SRC_COLOR, not alpha-test.
  Fixed Deathside-only RenderObjectBlurs branch to native EnableAlphaBlendMinus.
  Strengthened routing guard to check this specific branch (not another skill).
- Pose/action-speed build B5AE5327 was staged and QA2416 emitted trails at
  01:34:15.692; pool cleanup01:36:45 was zero. This does not imply visual PASS.
- QA captures previously ended1094ms after summon; repeat cast1250ms and
  first trail1367ms were missed. Deathside capture now1500/1900/2300ms.
- Blend fix and capture timing build exit0; existing Crypto++ LNK4099 only.
  Runtime visual comparison and smoke/synchronization acceptance remain OPEN.

## Deathside omitted model action setup recovered - 2026-09-13

- Live QA23200 with action-speed setup now emitted DEATHSIDE_TRAIL for
  model10177 at01:27:50.480; earlier trace search had no such event. This
  proves emitter reached its trail branch, not visual parity/pool allocation.
- Additional coordinate defect: original65F140/65F141 push0/0 before
  Animation6FB753. Port omitted flags, native defaultTranslate=true; then
  TransformDeathsideBone(...,true) applied origin again. Set explicit
  false,false in scythe pose sampling, retaining world conversion once.
  Routing guard/build pass. This second correction is built but not yet
  staged while QA23200 (preceding action-speed build) is running.

- Native Open2 allocates Action_t and reads keys/locks but does not assign
  PlaySpeed. Deathside loaded these models without skill-specific setup;
  its scythe emitter reads Reaper action.PlaySpeed directly and rejects<=0.
  Missing initialization is a source-proven defect, independent of random
  heap values seen by any one run. Prior BMD-frame logs were insufficient.
- Recovered explicit original setup660019..6600FF: Reaper actions0/1/2=.1,
  action6=.08,7=.13; attacks3/4/8 use keys/16.675 when keys>1, else.02.
  Loop=false for0/2, true for other first9 actions. Scythe action0=.2.
  FloatB6B128=16.6749992 and B55564=.0199999996 verified. Action5 speed
  is untouched by original setup and not used by current attack selection.
- Implemented after successful model load, preserving animation keys/assets.
  Added pure speed tests and initialization routing guards. Updated stale
  blur guard to retain existing Chaos+Deathside direct texture routes.
  Tests/build exit0; linker warning class remains Crypto++LNK4099.
- Previous diagnostic QA24476 finished01:13:11 with pools0; path-validated
  process stopped for new deployment/relaunch. This build also includes the
  pending Havoc root SkillIndex/zero-angle fix. Fresh trail visibility,
  synchronization, smoke and cleanup acceptance remain pending.

## Owner runtime rejection queue and reference videos - 2026-09-13

- Owner image006c8ca2 clarifies Deathside missing broad dark radial slash
  bands with purple/black smoke, not merely Reaper BMD visibility. Audit
  scythe-carrier bone ribbons, mirrored layers, blend mode and synchronized
  player/reaper frame timing against this image and the provided video.
- Elemental Charge2016: owner reports missing arrow-tip flare. Existing
  five bone sprite submissions in RenderElementalCharge are not proof of
  tip placement/visibility; check actual bone anchors and emitted frames.
- Spearstorm clarification: multiple spears must remain embedded after
  descent. Do not treat explosion/crater visibility as embedded-spear PASS.
- Havoc concrete fix built: root Magum_line and scale8 shockwave previously
  passed SkillIndex0. Original666E94/6670D9 pass2013; renderer66660B gates
  second alpha_line2mono3 pass on OBJECT.Skill2013. Restored the native
  SkillIndex argument (immediately after PKKey, not the following Kind/Skill
  argument) and root shockwave zero angle6670D0..6670ED. Added routing
  guards; test/build exit0 with existing LNK4099 only. Not yet staged or
  visually accepted; missing rotating ring may have additional causes.

- Havok/Havoc Spear2013: owner reports missing ring rotating around caster
  and outward circular explosion. Reference https://www.youtube.com/watch?v=ycd1GEL53Dg
  opened in browser; title identifies Season16 Renewal Magic Gladiator,
  duration25s. Metadata verified, NOT yet frame-by-frame visual comparison.
  Channel https://www.youtube.com/@muonlineguides4813/videos is additional
  reference, not an SS21-equivalence authority.
- Current24476 QA logs show Magum_line10155 at caster(20150,15650,169.5),
  scale.700/frame.368 and shockwave10156 at(20150,15750,174.5),scale1.932.
  Both passed model/pose input gates. These first-draw logs do not establish
  root-ring/explosion visibility, final draw flags or temporal coverage.
  Investigate layers and caster/target distinction, not just asset presence.
- Spearstorm243: owner accepts downward flight but rejects missing embedded
  spear after impact. Inspect contact pose, terrain origin and retained
  lifetime/rendering, not another flight-direction change.
- Death Scythe/Deathside288: owner reports missing model slash and smoke
  synchronized with player animation. Reference https://www.youtube.com/watch?v=7IzQyUFtL_k
  web fetch failed; no claim of viewing/comparing this clip yet.
- SwordBlow1501: repeated owner rejection "still white" stands. New live
  SWORD_BLOW_TAIL_SUBMIT at01:11:46.466 proves mesh1/material1/texture35453
  alpha.9073/gain.5897/RGB(.07239,.07846,.11795) at submission only.
  Shader copies BodyLight; does not prove framebuffer tint or identify the
  white visual layer. Inspect all overlapping layers and texture binding.
  SS21 preview asset inventory includes Swordeff_mono2, BUT its inspected
  small-sword BMD still names Swordeff_mono.jpg on mesh1. Do not swap the
  texture based on inventory name alone; resolve provider alias first.

## Owner rejects white/yellow small-sword tails - 2026-09-13

- Owner subsequently reports "Sword Blow van trang": the143:155:233
  compatibility change is NOT accepted. Do not describe it as a color fix.
  Launched local QA PID23412;1501 captures exist but phase1/2 do not isolate
  long tails sufficiently for a color verdict. Run ended with pools0 at
  01:07:39.661. Inspected shader AddMeshCommand: copies BodyLight into
  queued command, multiplies blendMeshLight for material1. No evidence yet
  of shader discarding tint; image-background/additive effects unresolved.
- Added bounded SWORD_BLOW_TAIL_SUBMIT logging (two variants, three alpha
  bands), recording material/texture/alpha/gain/submittedRGB. This measures
  renderer input, NOT GPU framebuffer. Diagnostic build exit0; deployed
  after finished PID23412 and launched fresh local QA. No new RGB guess.

- Owner image923ebe7c shows pale/yellow long tails; SS21 reference is
  blue/violet. Original1501 still submitted neutral(.2,.2,.2) to tail
  mesh1; scalarAlpha*.65 cannot introduce hue into Swordeff_mono.jpg.
  BMD inspection confirms mesh1/material1 owns this long8-triangle plane.
- Added material-only transfer using previously recorded image RGB ratio
  143:155:233, normalized to preserve current blue-channel energy. Applied
  only before small-sword mesh1 draw, restoring BodyLight immediately after.
  Mesh0, OBJECT lights, trajectories, gain .65/.32 and big sword unchanged.
  Fly already carries this ratio: no intentional further tint change there.
- This is image-derived compatibility tuning, NOT an SS21 engine constant
  or proof of framebuffer parity. Earlier OBJECT/PASS MATCH is insufficient
  for the rejected1501 appearance. Test guards verify channel ordering,
  blue-energy preservation and zero-input behavior, not visual acceptance.
- Focused math test and isolated client build exit0; existing Crypto++
  LNK4099 warnings only. Build SHA256
  5F30EA04F90B73ED6892FD899DFF0BDB060649B5C1D5513F6BFDAECCB081DD7D.
  No Engine-Pegasus process found before staging; no live client was killed.
  Restart/runtime comparison of revised1501 tail remains pending.

## Sword Wrath shared callback chain narrowed - 2026-09-13

- Callback12 map33 path now ends false:4F9B46 reloads27B3, excludes
  71/72/79/469/307/30A and returns4F9E95..4F9E9B. Virtual paths on
  maps49/4A/5F still need their implementations; count remains11/14
  globally, not12/14.
- Extended FindPegasusImmediateXrefs.py with opt-in --absolute-memory:
  includes only memory operands without base/index registers, labels them
  mem:, preserves existing immediate-only invocation and pinned hash guard.
  One completed scan returned13 hits for67FD31C/FF2EFC/FF2F00/FF2F04.
  Whole-section decoding remains a lead finder, not completeness proof.
- Found callback5 table loader5291D0: calls493D99 with1182914, then
  493D6E with1183404.493D99 examines at most100 records of stride1C,
  stops at first unsigned entry[0]>99, appends via493B74 into vector+4.
  Need provenance/content of1182914; do not infer static/empty contents.
  Same initialization region calls494322 with1128D58 for classifierFF2F18.
- Registry global67FD31C construction reference is6C33BF ->6BF7CB;
  6C07FC/6C0805 are teardown, not registration. Next recover constructor
  input before6C33BF and follow actual handler registration. No source
  renderer change or runtime acceptance in this investigation pass.

- Follow-up now excludes11/14 callbacks for27B3: added9/10/11/13.
  Ninth50E337 map27 branch excludes19A..19D/45A and delegates50C460;
  that excludes2FB/2FC/2FE and returns false50C892 ->50CD96.
  Tenth4C2993 converges4C29EA across map query outcomes for this type;
  nested4C25B4 excludes2E0/2E2/2E3/303, returns false4C25E3 ->4C298C.
  Eleventh6ED5BF false map query returns false; true excludes464/465/466
  and returns false6EDB14 ->6EDFD1. Thirteenth4FECE1 only accepts
  model310..312;27B3 returns false4FF2EF.
- Remaining callbacks are5,12,14. Twelfth4F98E8 runs only on maps
  33/49/4A/5F (4F884F);49/4A/5F dispatch virtual slot14 via4F991E.
  Map33 direct type path currently traced to4F9B46 (reloadmodeltype).
  Do not generalize a false result on other maps to these four maps.
- Fourteenth6EFF23 traverses handlers from singleton[67FD31C] (6F0266).
  6F02D5 checks each handler's map-ID vector; accepted maps invoke virtual
  slot14 at6EFFD5 with object/model/extra arguments. It returns true on
  the first handler that handles the draw, false only when exhausted.
  Need handler registrations/map vectors or live state, not a guessed
  empty list. Default pass flags2 is proven conditionally, not globally.
- No client renderer mutation or rebuild this pass; next steps remain
  table-loader provenance for callback5, map33 path4F9B46, and registry
  construction for callback14. All twelve acceptance gates remain open.

- Fifth callback unresolved state is now precise:493CE8 scans entries
  [FF2EFC+4,FF2EFC+8), stride1C, comparing entry+4 to model27B3.
  4C586A then scans the same entries comparing entry+4+285 to model;
  it can alter BodyLight and BlendMeshLight. If neither match, switch
  4C5950 excludes2EA/2EB/2EE/347/348 and returns false4C597A.
  Need actual table contents (or loader provenance), not an empty-table
  assumption. No original MU-x86 process was present during this check;
  isolated Engine-Pegasus PID11416 was still present.
- Callbacks6/7/8 are excluded for27B3:4D3DFB true map branch delegates
  4D23FF, which excludes2E2/2E3/2E4/2E7 and returns false4D297F;
  false map branch returns false directly.5043D9 map outcomes converge
  to502804, excludes2F6/34A/34B/34C/34D, returns false502FC9.
  508180 converges5081D0 for27B3, delegates506256, excludes
  2F7/2F8/2F9/458/459, returns false50628D ->506871.
- Seven of fourteen extension handlers are now excluded (1..4,6..8),
  not all. Next inspect ninth50E337 map27 branch50EBEC and delegate
  at50E3A8. Default tail84DDFF explicitly calls RenderBody6FDAE9 with
  flags2 then enters buff overlays84D3A3. No visual parity claimed.

- Follow-up: first four callbacks now excluded for27B3. Nested4ED7C2
  passes comparison-only path4ED8A2..4EED8B, returning CL=0 established
  at4ED7DA. Nested4CC99D subtract-switch excludes2CE/2CF/2DB/447/44E
  and returns BL=0 at4CD128; false map query also returns false.
  Third4F4E0B delegates4F2EEB:27B3>2D8, excludes2D9/2DA/2DC,
  returns false4F349E ->4F3C5C. Fourth44C950 is XOR AL,AL; RET.
- Fifth4C7FF8 converges4C8435 for27B3 regardless of map5E, then
  calls4C57DE. That function first queries493CE8 using model type;
  query result is not yet established. Do not count it as excluded.
- Conditional default path (remaining callbacks false and classification
  not6/8/5):8507C2 ->850BED ->852062 (27B3-46E>7) ->8523FE
  ->853D19; type27B3 !=27E, branches850D0D with texture override-1
  already pushed. This locates the default parameter forwarding block,
  but does not prove unknown classifier/extension outcomes in runtime.
- Default850D0D forwards HiddenMesh(+48), UV(+7C/+78), BlendMeshLight
  (+74), BlendMesh(+50), Alpha(+A8), then jumps84DDFF. This preserves
  root BlendMesh0, unlike adapter's current explicit bright loop with-1.

- For model27B3 the map/type branches after84BC42 converge at84CC1D;
  object buff37/38/56 may still affect BodyLight before84CD2B. Do not
  silently assume those object states when evaluating material parity.
- Shared draw84CD2B..84CEBC invokes fourteen extension render handlers.
  True normally skips to84D3A3 (buff overlays), with separate exits for
  4C2993/6ED5BF. Thus84D3A3 is not evidence of a default material pass.
- First callback4F0DF0 excludes types2A6/E8/158/15A for27B3; both outcomes
  of map query529820 converge at4F16F3 because27B3>9F. Crucially this
  calls4ED7C2, NOT an unconditional false return. Next block4ED8A2.
- Second callback4CD52E excludes C/11/33/42/32/53/54/55/13 and range
  3D..40, but then delegates to4CC99D at4CDAAD when map query is true.
  Its nested map/type switch starts4CC9CF; inspect before claiming false.
- Conditional on all callbacks declining, model comparisons84CEC2 ->
  84D9ED ->84DB03 ->84ED20 ->84EDCD ->850443 exclude27B3 from the
  explicit branches. At85045F it again calls item classification4941F6
  with27B3-493=2320; result6 selects a special path. This classification
  remains unresolved, so no guessed direct renderer replacement was made.
- Source-only evidence added; no client changes/build or visual acceptance
  claimed in this pass. Resume callback4ED8A2 and nested4CC9CF, then
  resolve classification/default draw. All original12 remain IN_PROCESS.

## Sword Wrath shared-renderer audit in progress - 2026-09-13

- Continued known model comparisons84AF17 ->84B0E9 ->84B171 ->84B223
  ->84B41E ->84B4D4 ->84B55B. Model27B3 is below the special27B5..27BB
  range and outside the intervening explicit model cases. Current stop is
  map/state query52983B at84B560; inspect its following conditions next.
- Added FollowPegasusKnownComparisons.py, hash-pinned and bounded200 steps,
  traversing only known register/immediate comparisons and direct branches.
  Stops on calls, memory reads or unsupported instructions, so no assumed
  external state is silently promoted to evidence. This is inspection tooling,
  not a runtime acceptance test or model semantic emulator.
- Following52983B, both outcomes converge to84BC42 for type27B3: false
  jumps directly; true only handles types20/27/29/2E/3E/43/44/40/41/42/50.
  Thus this map query does not require an assumed result for this model.

- Follow-up traversal: wrapper85B1BE ->849BD9; general draw consults item
  classification4941F6 (map lookup returning-1 if absent) and object render
  flags before the model-specific chain84A822. For ordinary flags and type
  27B3, excluded explicit branches1A7/1A8/1A9/195/228/27F/280/281/32D;
  next model-specific block84AD77. This is a conditional path audit, not a
  claim that external item classification/state has been runtime resolved.
  Keep direct bright-loop replacement unmodified until the terminal path is
  proven. RenderBody call84A7C7 demonstrates object blend/alpha/UV forwarding
  for a separate flags0x80 branch; do not transplant that branch blindly.

- Exact root block67AECA..67AF78 permits actions67/157/158/159, sets
  BlendMesh0 at67AF37 and BlendMeshLight=Alpha at67AF54, then calls85B1BE.
  That wrapper calls Calc_RenderObject84832F then shared draw849BD9.
  The adapter instead directly loops mesh bright passes with blendmesh-1.
  Must trace shared default draw/material semantics before claiming equivalence
  or changing flags. No guessed renderer patch made from the wrapper alone.
- Staged Fire_wall_knight.bmd is in SwordWrath/Effect, not Recovered/Effect:
  hash5E3E3642D5B5D1F6968C5BF1CE8026E2B9BA7675C95D897EA5B91243F8E1E682,
  one mesh34 vertices/32 triangles, one bone/action, texturefire_15fmono.JPG.
- Immediate27B3 xrefs only identify skill-local handlers/registration, not
  the shared draw's default path. Continue at849BD9 with the known object
  type and render parameters; do not treat absent exact xref as no rendering.

## Spiral aura lights independently verified - 2026-09-13

- Full renderer67766E: pulse calls6776D2/677714 use SIN/AF45F0, not the
  floor helper. Existing sine pulse functions stay unchanged. Bone index
  split677826..677861 matches port (high5/7/8/9/14/15/16; no12).
- Bone sprite call6778C8 takes aura ESI as owner at6778A6, whereas the
  three center sprites use actor owner. Corrected only bone-light sprite
  ownership to&effect; actor bone transform and center sprite owner unchanged.
- This establishes pointer-role parity, not changed appearance or cleanup
  acceptance. Current PID23948 QA is still the preceding staged build.
- Extracted emitter predicate into shared contract for executable tests:
  quarter-tick boundary/non-boundary, exact integer, zero-step and40 emitted
  crossings across160 quarter-steps. Contract/routing tests pass; no inference
  of actual particle density from the arithmetic test alone.
- Isolated build exit0; staged after PID23948 completed00:26:19 with pools0,
  then launched fresh QA. Original twelve visual/server acceptance is open.

## Spiral Charge emission was using sine instead of floor - 2026-09-13

- Full67704B..6770B1 callsAF4800 for life+step and life, then SETA on
  first>second. AF480F is ROUNDSD immediate9 (round down), not SIN.
  Both callers67747C/6774AF skip emission when false. Correct predicate is
  floor(life+step)>floor(life), not sin(life+step)<sin(life).
- Corrected only the two emitter cadence gate. Expanded routing and pinned
  helper-identity checks. Other sinusoidal buff lights need their own caller
  evidence and were not changed. Guards/build exit0 (existing LNK4099 only).
  Staged after PID18864 finished at00:21:30 with pools0; fresh QA launched.
  Emission-density/runtime parity remains pending.

## Chaos Blade directed smoke trig corrected - 2026-09-13

- Re-read emitter654E3E..655222:654FCE/COS writes positionX and654FF1/SIN
  writes positionY. Corrected the reversed pair in the authored directional
  smoke sweep; no changes to random range, radial ordering or BMD ribbons.
  This is direction-dependent, not just a symmetric random scatter.
- Corrected stale target-owned comment for Chaos caster root. Raining Arrow
  runtime adapter has no direct sin/cos pair; no speculative change there.
- PID16492 completed local QA at00:16:29 with pools0. New Chaos and prior
  Spearstorm spawn/scatter corrections passed focused guard/build (existing
  LNK4099 only), combined stage completed and fresh isolated QA launched.
  Directional visual acceptance remains open.

## Spearstorm spawn/scatter trig and prevention guard - 2026-09-13

- Full helpers675C19 and675B28 use COS into X, SIN into Y at respective
  calls675C77/675C9D and675B82/675BA5. Corrected reversed pairs in blade
  spawn and expanding crack smoke; random ranges and timing unchanged.
- Added hash-guarded VerifyPegasusTrigHelpers.py proving fallback call and
  fsin/fcos instruction identities. Guard, Spearstorm tests and isolated
  build exit0. Updated lessons with the false-positive test failure and
  independent cardinal-angle/range checks; addresses remain non-portable.
- New spawn/scatter correction is in build output only; current PID16492
  still runs the preceding staged Dragon/Shining Bird correction.
- Inspected qa_16492_skill_241_phase_0: cyan feather cluster now overlaps
  the forward monster area nearer caster after heading correction. Bright
  overlap and monster/dust occlusion still prevent full pillar/material
  comparison; no visual PASS from this single frame.

## Dragon radial trig and early Spearstorm image - 2026-09-13

- Original radial helper66279B..662874 calls COS at662802 into X and SIN
  at662834 into Y. Corrected swapped axes for all eight randomized quadrants.
  Lightning scatter likewise calls COS662F3B/SIN662F5D; corrected that pair
  only. Uniform random clouds may look similar despite axis-order error;
  do not infer a large visual improvement from this source correction.
- PID5844 finished at00:11:48 with all pools0. Inspected Spearstorm phase0
  after trig fix: visible shaped/textured spear geometry beside caster,
  unlike previously indistinct impact-only frames. Orange patch still behind
  caster; not yet identified. Need isolate that draw, no material guess.
- Dragon guard/build exit0 (existing LNK4099 only). Combined Dragon and
  Shining Bird corrections staged after completed PID5844, fresh QA launched;
  original twelve remain IN_PROCESS.

## Shining Bird trig callsites corrected - 2026-09-13

- Root6735E8 calls SIN/AF45F0 at673659 into Position.x plus250, then
  COS/AF4050 at673684 subtract250 from Position.y. Port had cos/-sin.
- Pillar-follow helper673A08 calls SIN at673A84 into anchor.x plus130,
  COS at673AA6 into anchor.y minus130, retaining PKKey*90+heading and Z+60.
  Corrected both pairs only. This supersedes prior forward-offset claims.
- Root and side pillars can now be compared on the recovered heading axes;
  no size/colour changes or broad trig replacement. Runtime acceptance open.
- Focused guard and isolated build exit0 (existing Crypto++ LNK4099 only).
  Bird correction remains in build output while current PID5844 completes
  the staged Spearstorm trajectory run; stage afterward, not mid-sequence.

## Spearstorm sine/cosine identification corrected - 2026-09-13

- Recovered math helpers conclusively: AF45F0 fallback atAF47A4 calls
  B0B47F containing fsin atB0B49D; AF4050 fallbackAF41F7 callsB0AE9F
  containing fcos atB0AEBD. Earlier notes and tests reversed these names.
- Full blade initializer675ECB..675F42 therefore yields
  X=sin(yaw)*sin(pitch)*speed, Y=cos(pitch)*speed,
  Z=cos(yaw)*sin(pitch)*speed. Previous adapter yielded almost horizontal
  flight (Z0 at yaw0), then relied on the timed forced-impact snap.
- Corrected math and independently updated expected vector samples; at
  pitch60/yaw0, direction is(0,-100,-173.20508), not(-100,-173.20508,0).
  Added a descending-Z check throughout the authored pitch/yaw range.
  Old tests passing was not evidence of correct helper identification.
- Math/routing tests and isolated build exit0; staged after PID6412 finished
  at00:08:41 with pools0, launched fresh QA. Visual trajectory still pending.
  Other recovered math using
  these helpers must also be audited; do not globally swap sin/cos blindly.

## Spearstorm flight capture coverage - 2026-09-13

- Inspected staged3cha_Kskill.bmd: hash3A62098CC01196A18D89951ED65A51DCBE60347806A6585F8863B7A965432EC3,
  one mesh384 vertices/591 triangles, three bones, one action, texture0
  ConmocionLance.jpg. Blade is not the flat crack BMD. This does not identify
  the orange geometry in the prior capture by itself.
- Default first QA sample250ms can miss six-tick flight. Spearstorm alone
  now requests50/450/850ms samples; all other skills retain250/650/1050.
  Added QA_CAPTURE_ELAPSED to report actual arm-to-capture-request timing.
  It measures capture scheduling, not exact GPU presentation or impact time.
- New capture change and preceding Dragon twilight flag built exit0 and
  staged after completed PID23276; fresh isolated QA launched. Visual
  diagnosis of the blade/matrix/material still remains open.

## Dragon twilight flags and native blending semantics - 2026-09-13

- Full0x663634..0x663779 renderer also uses flags2 (0x663762), not0x42.
  Corrected twilight layer flag and expanded scoped guard. RGB constants
  atB6B16C/B6B184 and subtype2 scale atB4EE50 match current adapter.
- Important: native ZzzBMD.cpp prioritizes BlendMesh<=-2 or matching mesh
  texture before testing RENDER_BRIGHT. Matching blendmesh0 remains additive
  with either flags2 or0x42. Thus these flag corrections establish source
  fidelity but do NOT by themselves prove reduced brightness/transparency.
- Re-read Havoc line0x66664F..0x66674A: speed1, two mesh0 flags0x42 passes,
  colours(.15,.5,.7) then(.15,.15,.6), alpha/blendalpha and blendmesh-2
  match adapter. No speculative Havoc colour change made.
- Current live QA PID23276 uses staged Spearstorm correction hash
  06649AE490188661179E51D103DD7ED1E47A2F85AEC17D4859266142D8D139C7.
  New twilight guard/build exit0 (existing LNK4099 only); stage still pending
  to avoid interrupting the current QA. All visual acceptance stays open.
- PID23276 subsequently completed at00:03:05 with all pools0. Spearstorm
  phase0 shows bright gold/blue impact rings and stones, but also orange
  rectangular-looking geometry overlapping caster/monsters. Do not accept
  blade visuals yet: next inspect blade BMD material/mesh mapping and earlier
  flight frames to distinguish this geometry from unrelated scene effects.

## Spearstorm blade accidentally used crack material - 2026-09-12

- Decoded dispatch0x676242: blade27C9 ->0x675848, crack27CA ->0x675605.
  Full blade renderer0x675848..0x6758E1 preserves object scale, sets white
  BodyLight, draws mesh0 flags2, alpha=min(BlendMeshLight/.7,1), blendmesh-1,
  blendlight1 and texture override-1. Port incorrectly copied the crack's
  normalized3x scale, grey light and BITMAP_CRATER override to the blade.
  Restored this blade-specific contract; did not alter crack base material.
- Crack layer0x67577D loads alpha1 and only subtracts progress for reverse
  layer. Fixed Combo layer's invented progress fade-in to constant1.
- Scoped renderer checks and isolated build exit0 (existing LNK4099 only).
  PID7536 completed with pools0 at23:59:52; staged the correction afterward
  and launched fresh isolated QA. Fresh visual evidence pending; no full skill
  acceptance inferred from recovered draw parameters.

## Dragon wind renderer flag mismatch recovered - 2026-09-12

- Read full wind renderer0x6637CA..0x66387D: call0x663874 receives flags2
  at0x663871, mesh0, blendmesh0, Alpha for body/blend, Light*6. Port added
  RENDER_BRIGHT (0x40). Removed only that extra flag and added a scoped
  regression check. No speculative alpha/light reduction.
- PID22800 phase0 images: Dragon heads/lightning visible, with bright
  overlapping regions; Shining Bird has cyan feather/light cluster left of
  caster. Neither image establishes complete layer/transparency parity.
  Wind focused guard and isolated build exit0 (existing LNK4099 warnings).
  Fresh runtime comparison is pending; no visual PASS inferred from flags.

## Fresh Elemental and Crusher image observations - 2026-09-12

- PID22800 remained live/responding at the exact isolated Engine-Pegasus
  path. Completed QA15 at23:54:07 and blur/children/total pools0 at23:54:22.
- Inspected qa_22800_skill_2016_phase_1/2: yellow/cyan lights and curved
  coloured trails now surround the caster rather than the monster cluster;
  visible light positions differ between these frames. This establishes
  visible motion near the corrected owner, not all five layers or matched
  original timing/colour. Elemental visual parity remains IN_PROCESS.
- Inspected qa_22800_skill_2015_phase_1: green/cyan ground glow around
  caster; no large opaque black square in this frame. Full Crusher chain
  comparison still open. Re-read original0x65AD5B..0x65AF32: three terrain
  sizes4/3/2, Calc_RenderObject(false), mesh0 bright pass. No explicit frame
  advancement in this renderer; do not invent playback here without tracing
  the updater/global animation path. Spiral/Crusher cast roots already use
  *so and buff-sync paths *o; no additional owner change was needed.

## Elemental cast owner recovered from caller - 2026-09-12

- Direct callers of helper0x664CF8 are cast0x664CEC and buff sync0x664DBB.
  Full cast handler0x664C95 assigns ESI from its object argument, sends that
  same ESI to SetAction0x6F9E51 at0x664CC4 and cast creation at0x664CEC.
  Corrected WSclient cast root from *to to animated caster *so. Independent
  CreateElementalChargeBuffAura(*o) still follows the buff recipient.
- Extended the focused routing guard to distinguish cast and buff ownership.
  Focused guard and isolated build exit0 (existing Crypto++ LNK4099 only).
  Staged RuntimeClient/Delivery SHA256
  D282799D4129FA8CE7E33314572C630E5F79F2288C5174859E87EDC8DA0555A7,
  launched PID22800 after completed prior run. Fresh visual comparison is
  still pending; IN_PROCESS.
- Previous staged run completed23:47:11 with blur, child and total pools0.
  Clean pool evidence does not establish visual parity or server acceptance.

## Spearstorm transition is not yet evidence of interruption - 2026-09-12

- Read staged Player BMD: action80 has7 keys. Native enum11 is
  PLAYER_STOP_FLY. PID4120 trace: action80 speed1.622, frame4.731 at109ms,
  then idle11 at172ms. This is consistent with completing the short action;
  do not label it an erroneous override or force a longer animation without
  proving the actual transition caller and matching original attack speed.
- PID4120 completed local run at23:42:18 with blur/children/total pools0.
  Stopped that path-validated client only after completion and staged the
  already-built Elemental bone-light correction, then started fresh local QA.

## Elemental Charge bone lights corrected - 2026-09-12

- Decoded0x664ABA..0x664C04: Calc_RenderObject(false) at0x664B2D,
  bone extraction Translate=false at0x664B74, sprite ownerESI at0x664BB8.
  Corrected five layer bone extractions from true tofalse and sprite owner
  nullptr to&effect. Native model playback was already present; this does
  not prove the reported motion/visual parity complete.
- Expanded translated-anchor guard. Elemental cast target selection remains
  unchanged pending caller evidence; unlike proven MG attack roots, buff
  target semantics must not be inferred from the one-object helper alone.
- Guard and isolated build exit0. Latest Elemental changes are in Client
  build output only for now; PID4120 still runs the preceding staged binary.
- Live PID4120 trace reveals a separate Spearstorm issue: at844ms selected80
  but actual11, frame4.667. Must investigate downstream action transitions;
  initial action-selection checks alone miss this override.

## Dragon head bone flare translation corrected - 2026-09-12

- Decoded0x66304A..0x66315F: translated pose at0x66309F, then both bone3/4
  TransformPosition calls use false (0x6630AF/0x663119). Native head draw
  already calculates translated pose but flare extraction used true.
  Changed to false and added bird/dragon anchor regression guard.
- Fresh Shining Bird QA PID19308 remains live; no bird capture was available
  at the first check. Do not claim runtime validation from this source fix.
- Later inspected qa_19308_skill_241_phase_0: cyan feather/light cluster
  visible beside monsters; prior huge stretched sheets absent in this frame.
  Monster occlusion prevents full geometry comparison; no visual PASS.
- Anchor guard/build exit0; staged Dragon correction and restarted isolated QA.

## Shining Bird double world translation corrected - 2026-09-12

- Decoded render0x67380B: translated Animation call0x6738A2 followed by
  TransformPosition Translate=false at0x6738A7 (bone1 pillar anchor) and
  0x67390C (bone lights). Native Calc_RenderObject(false) also calls Animation
  with Translate=true. Port passed true again when extracting both positions,
  adding BodyOrigin/scale twice. Changed these two transforms to false.
- This explains a concrete off-world anchor defect; fresh image evidence is
  still required before claiming the pillar geometry or complete bird fixed.
- Isolated build exits0; staged/relaunched local QA with this correction.

## Dragon Violent and Spearstorm caster-root correction - 2026-09-12

- Exact dispatch0x663475: ESI receives action253 at0x663502, then feeds
  radial positions0x663532 and all root helpers. Spearstorm0x6754B6:
  ESI receives selected action0x675567, then root owner/position0x6755D6/E0.
  Both are animated casters. Corrected WSclient *to -> *so for both roots;
  victim impact routes unchanged. Added four-root caster regression guard.
- Spearstorm also selects mounted variants82/92 before default80; this
  remains open until native equipment predicates are mapped, not guessed.
- Four-root guard and isolated build exit0; staged/relaunched local QA.
  Runtime validation for these latest two root changes is pending.

## MG cast roots were incorrectly owned by attacked target - 2026-09-12

- PID8200 diagnostics submitted all four Havoc render model types:
  Nova10154 meshes2/bones9, shockwave10156 meshes1/bones1,
  end10159 meshes2/bones5, line10155 meshes2/bones5. Initial alpha1;
  line frame0.434. No LOAD_FAILED/POSE_REJECTED was observed in this sample.
  This rules out a missing model load at those draws, not visual correctness.
- Original Havoc0x666CB6 uses ESI for both SetAction and root owner/position;
  Chaos0x654AF9 passes that same animated ESI to0x654DC3, which samples
  its bone17 for root creation. Both are CASTERS, not victim objects.
  WSclient incorrectly passed *to to both root helpers. Changed to *so;
  separate target-impact/list routes are unchanged.
- Added caster-vs-target dispatch guards; both focused guards and isolated
  build exit0. Staged/relaunched diagnostic QA; fresh visuals still pending.
  Existing target-owned wording in older audit sections is superseded here.

## Havoc model draw diagnostics - 2026-09-12

- PID23356 now selected/held action284; trace shows frame wrap from6 to0.327
  around687ms, speed0.400. Animation looping/speed still needs comparison.
  Inspected phases0/2: blue ground/light details and smoke are visible,
  but Nova/spear geometry is not clearly identifiable, so no visual PASS.
- Re-decoded Nova renderer0x66674C..0x66683D: mesh0/1 flags0x42 and
  mesh1 flags0x44 with chrome override. Current adapter expresses these
  passes; no speculative material change made from this evidence.
- Added bounded per-model HAVOC_RENDER_MODEL LOAD_FAILED,
  HAVOC_RENDER_POSE REJECTED and HAVOC_RENDER_INPUT DRAW telemetry with
  meshes/bones/actions/frame/scale/alpha/lifetime/position. This distinguishes
  failed resource load, skipped pose and a submitted-but-invisible model.
- Isolated build exits0; staged and relaunched diagnostic client. Runtime
  results pending; these log statements alone do not prove rendering parity.

## Havoc Spear extended action and Chaos live selection - 2026-09-12

- PID20372 Chaos trace selects/holds 285, speed0.400 and frame10.241 at
  875ms. Inspected phase1: pose differs from former action183; no opaque
  black blur sheet. This is not full visual/attack-speed acceptance.
- Decoded Havoc handler 0x666CB6..0x666D9F: queries model action284 via
  0x6FC9C5 and falls back to183 only when unavailable. Changed native
  dispatch/expected-action tracing accordingly. Build exits0, staged hash
  8B547F8BA8899EB0CDDFFA04FE744E6C56E806DC0CCE977AB22F7812C348C6A8;
  restarted path-validated isolated client as PID23356. Real action284 QA
  and missing spear children remain open.

## Chaos Blade extended caster action recovered - 2026-09-12

- PID18044 phase0 now shows curved purple ribbons without the opaque black
  sheet. Still not a complete reference visual PASS. At 23:17:47 the local
  harness reported AUTO_QA_POOL_BLURS=0, CHILDREN=0 and total POOL=0 CLEAN.
- Recovered dispatch 0x654AF9..0x654BA0: queries action 285 through
  0x6FC9C5 and falls back to 183 only if unavailable. Decoded 0x6FC9C5:
  signed bounds check against NumActions then returns Actions + index*16.
  Previous 'fixed action 183' audit claim was wrong. Updated WSclient to
  select 285 when present and trace the actual expected selection.
- New action selection passed isolated build and focused routing guard;
  staged and relaunched local QA. Not yet runtime-accepted.
  Must check extended-action playback speed and pose in fresh QA/video.

## Chaos Blade first-frame black overlay diagnosed - 2026-09-12

- Inspected PID19512 phase0: purple curved ribbon now exists, but an opaque
  black sheet obscures the scene. Phase1 no longer shows the previous radial
  white cross-joints. This is a rejected intermediate visual result.
- Previous note incorrectly called 0x87F992 alpha-test. Decoded its body:
  glBlendFunc arguments are ZERO / ONE_MINUS_SRC_COLOR (0x87F9B0..B7).
  Corrected only Chaos black blur to native EnableAlphaBlendMinus and added
  a regression guard. Deathside's similar existing claim needs separate audit.
- Added CountPegasusObjectBlurs for Chaos/Deathside live slots; local QA now
  logs AUTO_QA_POOL_BLURS and includes them in total CLEAN determination.
  Focused guard and build exit 0; staged the new build and relaunched local QA.
  Fresh visual result and blur-pool cleanup evidence remain pending.

## Chaos Blade native object-blur adapter staged - 2026-09-12

- Replaced Chaos-only cross-joint calls with native CreateObjectBlur using
  recovered owner/subtype reuse, white input RGB and explicit lifetime 8.
  Original zero subtype intentionally follows the native owner-first reuse
  semantics; no invented texture-key pool matching was introduced.
- Restored action 0 on the meshless carrier, two Animation calls per radial
  sample (Translate false then true), sample increment inside the eleven-step
  loop, >=2 emission gate and TransformPosition false for already-translated
  bone matrices. Retains the original particle ordering.
- Native blur renderer now binds only the two Chaos private bitmap IDs
  directly, alpha-tests the black layer (0x65492D), uses LifeTime/limit for
  the colored layer (0x65493E) and gates their expiration with accumulated
  FPS_ANIMATION_FACTOR (0x65494F/0x79439D). Other native blur types unchanged.
- Updated regression guard to reject the old joint substitution. Focused
  routing guard and isolated build exit 0; Crypto++ LNK4099 warnings remain.
  Staged RuntimeClient/Delivery SHA256:
  72EB41C56704B93E4C16F77C4D1BB94CA779F0D866292548D60E53C590BDB7F6.
  Path-validated old QA PID 21848 stopped; fresh visible QA PID 19512 launched.
- Still IN_PROCESS: inspect fresh captures, compare video pose/material,
  account for the newly used object-blur pool in QA cleanup telemetry, and
  verify gameplay/server acceptance. Prior joint-pool CLEAN cannot prove
  cleanup of this newly routed pool.

## Chaos Blade missing ribbon pool confirmed - 2026-09-12

- PID 21848 is live; inspected qa_21848_skill_2012_phase_1.jpg: excessive
  white radial lines remain, not the expected ribbon. CAST_ACTION_TIMELINE
  holds action 183 and progresses to frame 12.197 at 891 ms. This rules out
  an immediate action overwrite in this local sample, not incorrect pose.
- Exact root emitter 0x654E3E..0x655222 calls 0x794152 three times with
  (owner,p1,p2,white,type,true,subtype,lifetime): 0x7F3C/0/8,
  0x7F3B/1/8 and 0x7F3B/2/8. 0x794152 is the owner/subtype-reusing
  OBJECT_BLUR allocator, not JOINT creation. Current CreatePegasusRibbonSegment
  makes separate 8-wide, 15-life cross-quads; that is the wrong pool/geometry.
- Native CreateObjectBlur has the matching argument and reuse contract.
  Port must also translate render/material and FPS lifetime behavior, not
  only replace the emitter call. Renderer 0x7946B5 uses direct bitmap IDs
  above 0x7D25; helper 0x65493E selects 0x7F3B for lifetime/limit brightness
  (0x79474F..0x79476D) instead of per-tail brightness. Current native renderer
  only supports direct IDs for Deathside, so needs a scoped Chaos route.
- Root sampling differs too: original advances animationFrame by 0.109090917
  INSIDE the eleven-iteration radial loop and emits blur after frame >=2;
  port emits all particles first then runs an independent sample loop. Preserve
  original ordering and both Animation calls before changing the visual result.
- Inspected new Raining Arrow phase 1: captures exist, but this frame does not
  isolate the target damage-sheet fleck; no fresh target-size visual PASS yet.

## Raining Arrow target atlas geometry corrected - 2026-09-12

- Confirmed original sprite branch from function boundary 0x7CE86A:
  bitmap 0x7F09 at 0x7CE8FA, dimensions from bitmap at 0x7CE909/911,
  BOTH dimensions multiplied at 0x7CE93C/940 by float 0xB4AD94 = 0.25.
  The port cropped UVs but omitted these geometry multipliers, making each
  dimension four times the recovered value. Restored Width*cell/Height*cell
  only in the Raining Arrow sprite branch; emitter scale is unchanged.
- Added a focused renderer regression guard. Guard and isolated build exit 0;
  linker output contains the existing Crypto++ LNK4099 missing-PDB warnings.
- Staged RuntimeClient/Delivery SHA256:
  247D2A6081B4FCF2A269C45D2D978080755055779C63EF892E1CC0CAFD796FFA.
  Replaced path-validated isolated process 19044 with 21848 for fresh local QA.
  New runtime imagery and owner visual acceptance are still pending.

## Owner clarification and video comparison queue - 2026-09-12

- Owner identifies the incorrect MG skill as Chaos Blade (2012): wrong
  animation and missing purple trail, leaving white rods. Darkness (289)
  remains a separate rejected animation case.
- Owner also rejects Raining Arrow (2023) target-hit golden fleck size as
  larger than the original. Keep this as an open visual defect.
- Opened the official MuPegasus video in the browser:
  https://www.youtube.com/watch?v=NCzSgZkYB-c (6:07).
  Its expanded description lists the original twelve skills; the 0:30
  frame introduces Raining Arrow / Dex Booster. This is not yet a complete
  frame-by-frame comparison of twelve skills or a visual acceptance.
- Raining Arrow investigation: follow effect emits two arrow_damge sprites,
  scale random(1..2)*1.8. zzzeffectsprite.cpp selects a 4x4 atlas cell but
  currently passes full bitmap Width/Height times sprite scale to RenderSprite.
  Verify original sprite renderer geometry before changing either this size
  convention or the recovered scale constant. Atlas-size mismatch is a lead,
  not a confirmed cause; no guessed reduction has been applied.

## Owner animation rejection and expanded caster trace - 2026-09-12

- Owner reports MG animation wrong, missing purple trail (white rods only),
  and Summoner Darkness animation wrong. Both MG routes remain under review
  until the reported skill is identified; do not treat old numeric-action
  EXACT/PROGRESSED results as visual acceptance.
- Re-decoded Darkness 0x65B185..0x65B1AB: original handler selects action 145
  and applies skill state 289. Current dispatch selects 145, so changing that
  number without tracing downstream behavior is not justified.
- Added CAST_ACTION_TIMELINE in the isolated caster tracer: elapsed time,
  selected/current action, current/prior frames, BMD speed, model and class,
  every 100 ms and at observed action transitions through the 900-ms window.
  This can distinguish initial selection from early overrides/timing defects.
- Build exits 0; deployed and relaunched isolated QA. Prior PID 16656 run
  completed all 15 local visual steps and reported zero root/child pool usage
  after cleanup. That is local lifecycle evidence only, not visual parity or
  GameServer acceptance. Shining Bird geometry remains visibly incorrect.

## Shining Bird wind joint initialization recovered - 2026-09-12

- wind01 0x7F3E had no native adapter for its reserved texture ID. Restored
  initializer 0x79CA5B..0x79CAD5: lifetime 12, 5..8 tails, random width
  (20..139)*0.01*input scale, initial velocity 60, saved RGB and MultiUse=1.
  It starts with NumTails=-1 (0x796FE8), then builds native joint tails.
- Restored acceleration +2*animationFactor AFTER common movement, matching
  0x7ABA69..0x7ABA89 and float 0xB4ADD8=2. This path has no local RGB fade.
- Focused Shining Bird tests and isolated build exit 0. RuntimeClient and
  Delivery hash: 11F3E8AA8C7AB8C8E3CE298DFA17C25D2F34880437AB8563C4A9C2A16EAD60F0.
- Enabled the existing isolated local-visual harness and launched the staged
  client to obtain new renderer evidence. This mode does not prove server
  casting, damage, buff stats or full skill acceptance.
- PID 6776 actually reached ingame and emitted LOCAL_VISUAL_STEP/dispatch
  telemetry. The old inspector's protocol=0/null Hero output is INVALID for
  this build: InspectPegasusRuntimeState.cpp contains fixed VAs, not live
  PDB symbol resolution. Do not reuse its result for runtime acceptance.
- The hidden launch armed captures but did not save framebuffer images.
  Stopped only the path-validated PID 6776 and relaunched the isolated game
  with a normal visible window for the authorized interactive QA run.
- Visible run PID 16656 reached Tien in Elbeland and saved new framebuffer
  captures under RuntimeClient/PegasusQACaptures/qa_16656_skill_*.
  Inspected Dex Booster phases 1/2: the prior large black quads are absent.
  Inspected Shining Bird phases 0/1: bird appears, but pillar geometry is now
  visibly wrong (large pale/gold sheets at the left), so restoring the pool
  alone is NOT visual completion. Next audit must trace the original joint
  render/tail layout and color route; do not accept generic native rendering.

## Shining Bird force pillars were in the wrong pool - 2026-09-12

- Re-decoded 0x6735E8..0x67377E: all three 0x7F3D children call joint
  constructor 0x796B7A. The port incorrectly allocated PARTICLEs and scanned
  Particles for bone anchoring. Replaced these with native joints and Joints
  anchoring using PKKey +/-1, matching 0x673A08..0x673AF0.
- Restored pitch 90 (0x42B40000), central width 200, side widths 100 and
  initial side Z+60. Joint initializer 0x79CADA..0x79CB2B gives lifetime 23,
  tails 8 for subtype 0 / 6 for subtype 1, zero velocity, MultiUse 1 and
  saved source RGB. 0x796FE8 starts NumTails at -1 with no initial tail.
  Native tail creation/rendering now receives these children; removed the
  invented 26-tick particle initializer. The updater falls through to the
  shared tail/lifetime path (0x7ABA50 -> 0x7A4B1F -> 0x79E01C).
- Shared object renderer 0x8538B4 routes bird 0x27C1 to the same special
  mesh-0 additive draw as Dragon head. Added that material pass explicitly
  instead of relying on RISE's default draw for an unknown imported ID.
- Focused math/routing tests pass. Fresh visual parity, wind-joint behavior,
  and full runtime lifecycle remain IN_PROCESS.

## Dragon head visibility and movement are joint-owned - 2026-09-12

- Closed the preceding Alpha transition question by decoding the pinned
  joint updater at 0x7AA3A1..0x7AA511. Subtype 18 accelerates/turns the joint;
  0x7AA466..0x7AA49D sets target Alpha to 1 when target LifeTime > 5 and copies
  joint Position into target Position. Native RISE ZzzEffectJoint.cpp already
  performs both operations for BITMAP_JOINT_SPIRIT2 subtype 18.
- Restored head initialization Alpha=0, matching 0x662991. Removed the
  invented AlphaTarget=0 assignment and corrected the misleading claim that
  the head remains at its terrain point. It does not follow the actor owner,
  but it DOES move with its attached joint. No invented fade was added.
- This is source evidence, not fresh runtime acceptance; movement, reveal,
  smoke and expiration still require a new framebuffer comparison.
- Focused Dragon contract/routing tests and isolated client build exit 0.
  RuntimeClient and Delivery were restaged with the corrected initialization.

## Dragon Violent shared-renderer branch recovered - 2026-09-12

- Revalidated the isolated worktree; the previously reported Engine-Pegasus
  process is no longer running. No new runtime acceptance is claimed.
- Hash-guarded disassembly of original MU-x86.exe found the missing head
  special case in shared Draw_RenderObject: 0x853853..0x8538AF copies object
  Light into BMD BodyLight, then 0x850BE5 -> 0x8503F8 -> 0x84DBC0 calls
  RenderMesh (0x6FDF73) with mesh 0, flags 2, object Alpha, blend mesh 0,
  blend light Alpha, object UV and texture override -1. RISE's matching
  texture/blend-mesh branch enables additive blending. Generic object draw
  did not contain this imported-ID case and rendered an opaque head.
- RenderDragonViolent now expresses this recovered mesh pass directly after
  native pose calculation, preserving the subsequent bone flares.
- Corrected the prior misleading comment: Pegasus offset 0xA8 is visibility
  Alpha, proven by Calc_RenderObject 0x84836F..0x848382. Its initial zero and
  shared update envelope still require tracing; the existing visible Alpha
  envelope is unchanged and remains an explicit parity gap.
- Focused Dragon contract/routing tests and isolated client build exit 0;
  only existing Crypto++ LNK4099 warnings. Visual validation is IN_PROCESS.

## Isolated executable identity and Dex Booster black-quad correction - 2026-09-12

- The isolated client output is now `Engine-Pegasus.exe`; its runtime window
  class/title uses the `PEGASUS QA -` prefix. Deployment removes only the
  obsolete isolated `Engine.exe`, and crash-dump/max-instance routing is also
  keyed to `Engine-Pegasus.exe`. This prevents the Pegasus QA client from
  colliding by name/title with other RISE tasks.
- A current 15-skill contact sheet identified the reported full black terrain
  square as Dex Booster `2024`, phases 0/1/2. Its ground renderer incorrectly
  called `EnableAlphaTest()` before `RenderTerrainAlphaBitmap` while closing
  with `DisableAlphaBlend()`. The pinned Pegasus block
  `0x661781..0x6617F0` uses the alpha-blend begin/end pair; the isolated port
  now does the same and the focused verifier rejects reintroduction of the
  alpha-test opener.
- Focused Dex Booster contract/routing tests and the full isolated client build
  exit 0. Only the known Crypto++ LNK4099 missing-PDB warning class remains.
  RuntimeClient and Delivery contain byte-identical `Engine-Pegasus.exe`,
  SHA-256
  `B8FC4247FFCD932CFC67DC7C514CE186BBBEDC11DA627CC87D974A4A9351B48A`.
- The rebuilt client auto-authenticated as `Tien`, reached World1 and sent the
  isolated `/qa15` request, but the connected remote GameServer did not return
  a new QA sequence. Therefore the source-proven blend correction is built and
  staged, while fresh post-fix visual confirmation remains `IN_PROCESS`.
- The same contact-sheet review confirms three separate open visual defects:
  Shining Bird has its bird model but lacks the dense reference pillar/wind
  presentation; Dragon Violent's dragon heads are too opaque; Spearstorm shows
  its ground impacts but the falling `3cha_Kskill.bmd` blades are not visible
  in the current +250/+650/+1050 ms capture windows. These remain visual-chain
  failures and are not accepted from model-load or GPU-upload telemetry.

## Manual-auth runtime QA bridge - 2026-09-12

- Added isolated marker `PegasusAutoQACastOnly.enable`. It is recognized only
  after authentication by the character/ingame QA and stale-effect cleanup
  paths; `LoginWin` and `ServerSelWin` intentionally do not recognize it.
- This preserves manual credential entry while still selecting the QA
  character, sending `/qa15`, recording per-skill action/frame telemetry and
  reporting delayed Pegasus-pool cleanup after the owner reaches character
  selection. The verifier rejects any future leak of this marker into the
  saved-credential login route.
- Focused auto-QA verification PASSes. The isolated client build exits 0 with
  only the existing Crypto++ LNK4099 warning class. RuntimeClient and Delivery
  are staged at SHA-256
  `DF6157C01C1714675162CD1A2C73A3CB34D2550466AA83E34F5A79BC3F44ED04`.
  RuntimeClient has the cast-only marker and is responsive as PID 19248; the
  symbol-backed inspector currently reports protocol 0 / null Hero, so it is
  waiting for manual authentication and has not produced new visual QA proof.

## Darkness no-BMD route proof - 2026-09-12

- Decoded the complete pinned-Pegasus Darkness handler
  `0x65B148..0x65BBC5`. It selects player action 145, applies the skill
  timing/state route and builds its normal command data, but contains no calls
  to the recovered model, particle, joint or sound constructors.
- Therefore Darkness is not missing a dedicated persistent BMD effect in the
  isolated client; adding one would diverge from Pegasus. Added
  `VerifyPegasusDarknessRouting.ps1` to require action 145 and reject invented
  `CreateEffect`, `CreateParticle`, `CreateJoint` or `PlayBuffer` calls inside
  its dispatch block.
- This closes the client-root/assets question as source-proven absence, not
  visual completion. Darkness remains `IN_PROCESS` pending the unavailable
  original server Curse/Defense formula plus live action, buff icon,
  expiration/removal and pool validation.

## Spiral/Crusher/Sword Wrath ownership and position re-audit - 2026-09-12

- Re-decoded Spiral Charge initializer/updater `0x6770B2..0x677589`.
  Unlike the independent-projectile skills, all six `0x27A8..0x27AD`
  effects have a source owner/live gate. Only persistent anchor `0x27AD`
  additionally checks buff 214 and follows the owner. The isolated adapter
  already matches this behavior; a focused routing verifier now prevents a
  later blanket ownership cleanup from changing it.
- Re-decoded Crusher Charge `0x65A46E..0x65A948` and packet removal
  `0x65B129..0x65B145`. Its persistent `0x27B1` updater follows a live owner
  but does not poll buff 215 locally. Removed the invented native-marker
  check; add/remove packets remain authoritative, preventing premature aura
  loss during delayed add/relog state restoration.
- Re-decoded Sword Wrath updater `0x67AB1D..0x67AD8F`. Root `0x27B3` and
  layer subtypes 0/1 follow the owner; subtype-2 ground star and subtype-3
  expanding wave remain at their cast-time position. The adapter previously
  dragged every layer with the owner and is now corrected per subtype.
- Spiral Charge, Crusher Charge and Sword Wrath focused math/provenance and
  routing tests PASS. Raining Arrow's source-proven pointer-only
  orchestrators and independent child lifetimes are also locked by its
  focused verifier. A single batch of all twelve group-15 focused tests PASSes.
- The lifecycle aggregate initially failed because it encoded a generic
  owner-cleanup rule and required Crusher/Elemental to self-poll native buff
  markers. Updated it to enforce the recovered per-skill contracts instead:
  Chaos Blade/Shining Bird launch independently, Spiral has its anchor-local
  marker gate, and Crusher/Elemental use packet-owned removal. The corrected
  lifecycle, runtime-trace, auto-QA, 12-row routing and asset-preflight suites
  all PASS.
- Full isolated client and GameServer builds exit 0. Client output contains
  only the existing Crypto++ LNK4099 missing-PDB warning class. RuntimeClient
  and Delivery are deployed at client SHA-256
  `6D24C51BCEB3BA8549515E57B0D540142661EBCE8868B7C99AC2FE233B66941E`;
  GameServer SHA-256 is
  `6732F6636D1EF93BA2482BCFFCA1A590BEB3DC4946FF223A0170752BF8C60CD4`.
  The isolated client starts responsive as PID 664.
- Windows Computer Use identified the exact isolated Engine window but both
  capture attempts failed with `SetIsBorderRequired failed: No such interface
  supported (0x80004002)`. No blind input was sent. The client has an
  established server connection but has not produced a new runtime QA log,
  so all rows remain `IN_PROCESS` pending fresh live visual, reaction and
  final-pool acceptance; build/static evidence is not visual parity.

## Havoc Spear per-type ownership correction - 2026-09-12

- Re-audited updater `0x665F42..0x66636A`. Pegasus has no global
  `Owner::Live` gate. The invisible root and target-impact orchestrator check
  only that an owner pointer exists; Nova/line/shockwave/end children finish
  their authored lifetimes independently.
- Removed the adapter's global owner-live truncation. The root no longer
  copies owner position into itself; its burst helpers still use the actor's
  current position exactly as recovered. Target-impact alone follows the
  stored owner, with a null-pointer termination guard.
- Focused constructor/joint/tick/movement/fade/ownership tests PASS. Full
  isolated client build exits 0 with only Crypto++ LNK4099 warnings.
  RuntimeClient and Delivery were deployed at SHA-256
  `B988B52401A6FFACCB3F7602A9AF8267AF23E206214CBAF948498DE5AE81815B`;
  the isolated client starts responsive. Havoc Spear remains `IN_PROCESS`
  pending visual projectile/explosion, third-hit, reaction and final-pool QA.

## Shining Bird launched-lifetime correction - 2026-09-12

- Re-audited updater `0x673781..0x673808`. It only advances blend light and
  the non-looping BMD action; it never reads or gates on the caster owner.
- Removed the adapter's global owner-live kill. Once launched, Shining Bird
  and its three root-owned force pillars now complete the authored animation
  even if the caster dies or leaves the viewport; BMD completion remains the
  cleanup gate.
- Focused geometry/color/animation/ownership tests PASS. Full isolated client
  build exits 0 with only Crypto++ LNK4099 warnings. RuntimeClient and Delivery
  were deployed at SHA-256
  `9963C895E8ACB9C1F8A0735162DA1CC9091F57A8117A090981CFD4E334D929CC`;
  the isolated client starts responsive. Shining Bird remains `IN_PROCESS`
  pending live direction, hit reaction, visual timing and final-pool QA.

## Elemental Charge packet-owned lifetime correction - 2026-09-12

- Re-audited initializer/updater `0x664771..0x664AB7`. Both root and child
  routes terminate on missing/dead owner, but the persistent root performs no
  delayed local `g_isCharacterBuff` check. Pegasus keeps the root attached to
  bone 17 until the separate buff-remove path deletes it.
- Removed the adapter's invented sixty-tick marker timeout. This prevents a
  valid Elemental Charge aura from silently disappearing due to packet timing
  or viewport/relog restoration while preserving authoritative removal through
  `DeleteElementalChargeAuras`.
- Focused ownership/five-layer/pulse/lifetime tests PASS. Full isolated client
  build exits 0 with only Crypto++ LNK4099 warnings. RuntimeClient and Delivery
  were deployed at SHA-256
  `F33EF6B4AF8557248EF7F7E6C5B65BF8DB9508A1D63A5AFBD6E46E04E9A1BD36`;
  the isolated client starts responsive. Elemental Charge remains
  `IN_PROCESS` pending live five-layer appearance, third-Havoc/server stats,
  removal and final-pool QA.

## Spearstorm independent cascade lifetime correction - 2026-09-12

- Re-audited the full updater `0x676041..0x67623F`. It contains no owner or
  owner-live termination. The root helper explicitly chooses `root.Owner`
  when present and otherwise the root itself; blade and crack then complete
  their authored 72/50-tick lifetimes.
- Removed the adapter's global owner-live kill, which could erase falling
  blades and crack overlays when the original target died. Focused vector,
  impact, sound, fan, layer and ownership-routing tests PASS.
- Full isolated client build exits 0 with only Crypto++ LNK4099 warnings.
  RuntimeClient and Delivery were deployed at SHA-256
  `AADA313D2582059765F5A4D4BC6BBDFBC50316337531E8CE4BD66212B97D2C38`;
  the isolated client starts responsive. Spearstorm remains `IN_PROCESS`
  pending live direction/collision/visual/sound and final-pool QA.

## Dragon Violent per-type owner/lifetime correction - 2026-09-12

- Re-decoded the complete updater `0x662A37..0x663006`. Only target-impact
  model `0x27C7` checks `Owner && Owner->Live` and follows the target. The
  radial head/crack/wind/layer branches do not, and the delayed base consumes
  its stored owner without checking the actor's live flag.
- Removed the adapter's incorrect global owner-live kill. Target-impact still
  terminates exactly on missing/dead owner; already-created radial children
  now finish their own lifetimes instead of disappearing when the target dies.
  A null-only guard protects the delayed RISE pointer without changing the
  Pegasus live-state behavior.
- Focused ownership tests PASS. Full isolated client build exits 0 with only
  Crypto++ LNK4099 warnings. RuntimeClient and Delivery were deployed at
  SHA-256
  `6D5CB653227E067A448012ECF00DC8887C3F210AB8211255165F7E7AD182179C`;
  the isolated client starts responsive. Dragon Violent remains `IN_PROCESS`
  pending visual radial timing, target reaction and final-pool QA.

## Chaos Blade fixed-position lifetime correction - 2026-09-12

- Re-audited initializer/updater `0x6543B5..0x65459E`. The recovered updater
  contains no owner-live gate and never copies the owner's moving position
  back into the root. The prior adapter did both, which dragged the ground
  cascade with the target and truncated root/arc/crack children on target
  death.
- Removed those two non-Pegasus behaviors and made the source-proven alpha
  initialization explicit for all three models. The root remains anchored at
  the cast-time bone-17 position while its ten-frame cascade advances; arc and
  crack children finish their authored lifetimes independently.
- Focused lifecycle routing PASSes. Full isolated client build exits 0 with
  only Crypto++ LNK4099 warnings. RuntimeClient and Delivery were deployed at
  SHA-256
  `AEC09BD2D9E971EEF229701B4E16279920F4DF3BE21F0D39ED0319E96A13D71A`;
  the isolated client starts responsive. Chaos Blade remains `IN_PROCESS`
  pending visual target-reaction and final-pool QA.

## Dex Booster flare-joint subtype isolation - 2026-09-12

- Recovered the missed joint initializer/updater pair `0x661804` and
  `0x661980`. Pegasus's nineteen `BITMAP_FLARE` subtype-48 joints are
  twenty-tick spherical orbits with scale `30`, light `(1,.75,.3)` and an
  eighty-entry ribbon. They are not RISE's existing permanent subtype-48
  equipment flare attached to bone 24.
- The cast now tags only these joints with skill `2024`; initialization and
  movement intercept that marker without changing any native RISE subtype-48
  user. The exact phase/time multipliers `0.9351`, `1.7`, `1.3`, `2.2`, radius
  `95` and height `100` are compiled into a focused numeric test.
- Focused contract/routing tests PASS. Full isolated client build exits 0 with
  only the known Crypto++ LNK4099 warnings. RuntimeClient and Delivery were
  deployed at SHA-256
  `CFD734B4C2C5331CB9CF92AA3649F0E6D704BC2CF376BB43E1FE9C2324357C2E`;
  the isolated client starts responsive. Live visual and final-pool QA remain
  open, so Dex Booster remains `IN_PROCESS`.

## Dex Booster alpha/bone/ground renderer correction - 2026-09-12

- Re-audited the complete pinned client block `0x66110F..0x6617F5`. The
  existing adapter incorrectly treated OBJECT `+0xA8` as `BlendMeshLight`;
  Pegasus stores and consumes `Alpha`. Aura fade and all three ground rings
  now update/render from the correct field.
- Restored the exact ground behavior: subtypes 9/10/11 rotate by `10/5/15`
  per animation factor, subtype 10 keeps scale while 9/11 shrink by `0.1`,
  alpha fades by `0.00625`, light becomes `(alpha,alpha*0.5,0)`, and terrain
  rendering uses `-AngleZ`, alpha `1`, height `5` under alpha-test state.
- The two `0x27A1` flare emitters now spawn at two independently selected
  owner bones through RISE's native `TransformByObjectBone`, rather than both
  at the orchestrator origin. Their particles remain unowned and finish the
  exact thirty-tick lifetime. Aura sprite rotation is corrected from guessed
  `0.02` to Pegasus `0.08` in opposing directions.
- The focused Dex Booster constructor/ground/particle/routing tests PASS. Full
  isolated client build exits 0 with only the known Crypto++ LNK4099 warning
  class; SHA-256 is
  `780979BB742C3A6B92F781FD2CF7C61993CD50B002C11B518B604AA9C1F18749`.
- Exact server stat magnitudes are not present in the pinned client evidence,
  and no Pegasus GameServer binary is available under `D:\Pegasus`; the
  attack-speed/attack-success formula remains open rather than guessed. Live
  visual/removal/pool QA is also open, so Dex Booster remains `IN_PROCESS`.

## Raining Arrow full updater/renderer correction - 2026-09-12

- Reconstructed the complete pinned-Pegasus initializer/update/renderer chain
  `0x670DB9..0x671986`, not only the visible root. The impact pulse is the
  exact `Pulse(LifeTime-30,20)` window; the earlier `Pulse(LifeTime,30)` port
  could never illuminate the four overlay layers during their render window.
- Replaced the generic all-mesh bright pass with the exact routes: `0x2792`
  and `0x2793` remain non-rendering orchestrators, `0x2795` uses its six-row
  ground texture/color table, `0x2794` renders `Cratered` plus the four
  `pin_star03/Spark04/bostar3_R/damage01mono` overlays, and the other eight
  BMD children use the native object draw path. All recovered texture roles
  are dynamically allocated; raw Pegasus bitmap IDs are not copied.
- Restored the two omitted native Stone1/Stone2 subtype-10 impact fragments,
  the impact `Z+3` placement, the subtype-3 ring's zero-alpha start, exact
  root-Light ownership for ring08, one-volley-per-update cadence, and Pegasus
  owner-null lifetime behavior. Native projectile joints now follow the
  projectile angle and are retired at impact/height termination; the hit sound
  is latched once on the live root instead of replaying for every arrow. The
  six additional recovered texture assets are hash-guarded in the isolated stage.
- Focused Raining Arrow math/renderer/asset contract PASSes. The full isolated
  client build exits 0 with only the known Crypto++ LNK4099 warnings; SHA-256
  is `609CAD3DDCE1DEBBF6A8EB8CF2CD57E9132B3EDE1378059D584AA75E18AE45E1`.
- Windows Computer Use returned no native app inventory, so this build has not
  received a fresh character-session `/qa15` visual, collision or pool run.
  Raining Arrow remains `IN_PROCESS`; compilation/static parity is not runtime
  acceptance.

## Deathside scythe-carrier and object-blur ABI correction - 2026-09-12

- Reconstructed the complete pinned-Pegasus scythe update block
  `0x65EF97..0x65F4C4`. The three `ScytheEffAni*.bmd` files are meshless
  animation carriers: each update samples the authored action-key count,
  transforms the exact bone pairs and emits two direct plus three mirrored
  trail layers. Treating them as ordinary renderable meshes was incorrect.
- Classified call `0x794152` from its eight arguments, `0x386C` pool stride,
  allocator and renderer as `CreateObjectBlur`, not `CreateJoint`. The adapter
  now uses RISE's native object-blur pool with exact subtypes `1/3/4/5/6`,
  direct/mirrored bone endpoints and lifetime limits `100/100/100/100/-1`.
  The custom recovered `blur02_mono.jpg` ID is bound directly and follows the
  Pegasus alpha-test branch instead of being added to `BITMAP_BLUR`.
- The focused Deathside contract PASSes. The full isolated client build exits
  0 with only the known Crypto++ LNK4099 missing-PDB warnings; RuntimeClient
  and Delivery SHA-256 are
  `820E87700C527BDCFAEBCFFBCA42C7B19F00D21D1E87650E242A041A9BADD5DE`.
- A hash-guarded whole-executable immediate scan finds `0x278C`
  (`ReaperHead.bmd`) exactly once, at cleanup call `0x65FD48`; there is no
  Deathside creation xref. It remains loaded/cleanup-addressable but is not
  fabricated as an extra visual child.
- The isolated client currently starts and remains responsive, but no fresh
  character-session `/qa15` log exists because Windows UI control is not
  available in this run. Therefore Deathside remains `IN_PROCESS`; visual
  trail parity, attack variants, reaction and final pool cleanup are not yet
  runtime accepted.

## Full-chain comparison correction - 2026-09-11

- The owner explicitly rejected symptom-by-symptom patching. The acceptance
  method is now the complete pinned-Pegasus chain for every skill: caster
  dispatch/action/target, root constructor, every child asset, child BMD
  playback, update/movement/collision, renderer/pass, target reaction,
  lifetime/destruction and ownership/pool. A successful `/qa15` dispatch is
  diagnostic coverage only.
- Deployed the latest isolated client SHA-256
  `DD7CC6208CC2C4191C91467ED28F18BFEC2E9BE9F707D89B41C45BAAEB9C20B2`.
  One bounded live sequence reached all 15 dispatches; all 14 Pegasus caster
  frames progressed; 208 effects allocated; pool exhaustion was zero and the
  delayed dynamic-pool snapshot was clean. Havoc's exact `Magum_line.bmd`
  playback now produced `EFFECT_BMD_FRAME value1=10155 ... PROGRESSED`.
- The same run still emitted repeated late render failures for Raining Arrow,
  Crusher Charge, Dragon Violent and Spearstorm. They are not classified or
  patched until their exact Pegasus lifetime/owner exits are compared.
- Full Elemental Charge constructor comparison found a major omitted chain.
  Pegasus `0x66481B..0x664949` creates five child effects around the persistent
  `05_buff_obj.bmd`: three `0x7F20` flare03 effects at scales `3,3,4`, then two
  subtype-1 `0x7F21` Shockwave effects at scale `1.5`. Binary load evidence maps
  those roles to `Effect\\flare03.jpg` and `Effect\\Shockwave.jpg`. The
  isolated adapter currently creates none of them. Their dedicated Pegasus
  init/update block `0x664771..0x664AB7` is implemented with native RISE effect
  fields and isolated texture IDs; raw Pegasus IDs were not copied. Runtime
  allocation/destruction and final-pool evidence remain open.
- All skills remain `IN_PROCESS`; the new runtime evidence closes only the
  tested dispatch/caster-frame/pool and selected child-BMD playback gates.

## Elemental child-effect ABI and lifecycle proof - 2026-09-11

- Corrected the earlier classification after reconstructing the complete
  call ABI: Pegasus `0x72FC7D` is `CreateEffect`, not `CreateJoint`.
  `0x66481B..0x664949` creates three flare03 effect carriers and two subtype-1
  Shockwave effect carriers; the isolated adapter now follows that exact
  order, subtype, scale and authored source-light recipe.
- Added lifecycle telemetry and extended the delayed QA pool gate to count
  these bitmap-backed effects as well as dynamic model IDs. A clean isolated
  run created exactly five Elemental children, destroyed exactly the same five
  (two Shockwave, then three flare03), progressed the `05_buff_obj.bmd` frame,
  reached all 15 dispatches and all 14 Pegasus caster frames, allocated 207
  dynamic effects with zero pool exhaustion, and ended `AUTO_QA_POOL 0/0
  CLEAN`.
- Full isolated client build exits 0; only the known Crypto++ LNK4099 warning
  class remains. Build, RuntimeClient and Delivery SHA-256:
  `410F3C0EBCA6D85F09299C0574DEF941D3689B8A212F1887A3539686AF097192`.
- This closes only Elemental's five-child creation/lifetime/pool route. Its
  complete Pegasus visual comparison, exact server formula and every-third-
  Havoc behavior remain open, so it stays `IN_PROCESS`.

## Pegasus child-BMD animation re-audit - 2026-09-11

- Owner report that Elemental Charge and Deathside models were standing still
  exposed a shared porting omission: the isolated Pegasus model IDs are above
  native `MODEL_SKILL_END`, so RISE's generic effect loop never calls
  `BMD::PlayAnimation` for them. `Calc_RenderObject` evaluates the current
  frame but does not advance it.
- Complete Pegasus blocks prove this is skill-owned behavior, not a generic
  guessed animation. Elemental Charge `0x664ABA..0x664B27` advances
  `05_buff_obj.bmd` with the effect velocity before bone transforms/render.
  Deathside `0x65FA60..0x65FB27` advances Reaper and all three scythe carrier
  BMDs with `max(Action.PlaySpeed, 0.02)`, then resets completed actions
  `7/3/4/8` to action `0` except during dismissal.
- Those exact routes are now implemented. The former Deathside code also
  incorrectly froze action 7, manually returned attacks to 7, skipped scythe
  animation and skipped dismissal animation; all four consequences are
  removed by following the recovered block.
- Focused Deathside/lifecycle tests PASS. Full isolated build exits 0 with
  only the known Crypto++ LNK4099 warning class. Deployed RuntimeClient and
  Delivery SHA-256 are
  `52C58FA1C97C64B60CA337FE17A303241A55ED1D418A184745715B27D11FDBB8`.
  A fresh bounded `/qa15` run reached all 15 dispatches, all caster frames
  progressed and the pool finished clean. New child-model telemetry proves
  frame progression for Reaper `10175`, a scythe carrier `10179`, and
  Elemental Charge `10153`. This closes only their BMD-frame route; visual
  parity and the remaining child/collision/reaction/lifetime gates stay
  `IN_PROCESS`.
- Raining Arrow's later-volley pitch was also corrected from a provisional
  `asin` interpretation to the actual Pegasus `atan(vertical/horizontal)`
  block at `0x672228..0x672267`; equal vertical/horizontal deltas now produce
  45 degrees. The exact float random offsets, +500 launch height and root-Z
  target are retained. Equipped-bow and visual/collision parity remain open.

## Full Pegasus player-action re-audit - 2026-09-11

- Compared the pinned Pegasus `Data\Player\player.bmd` against native RISE
  structurally and byte-for-byte. Both have 60 bones; the first 284 action
  payloads are identical. Pegasus has three additional actions 284..286, and
  the exact Raining Arrow action is 286. The isolated runtime now hash-stages
  the 287-action Pegasus carrier without changing production `Data\Player`.
- Re-decoded every Pegasus caster handler instead of assigning generic attack
  animations. Deathside now uses mount-aware actions 172..175 on summon and
  action 185 on repeat; Raining Arrow uses 286; the other twelve handlers use
  their recovered fixed or equipment-aware branches. Sword Blow Fly was moved
  outside the Pegasus tail to custom action 287 so it cannot overwrite action
  286.
- Added isolated action-selection and delayed-frame telemetry. One bounded
  live `/qa15` run selected the source-proven branch for all fourteen Pegasus
  skills and every sample reported `CAST_ACTION_FRAME ... PROGRESSED`.
  `AnalyzePegasusRuntimeQa.ps1` reports all 15 dispatches, 300 allocations,
  zero pool exhaustion and final dynamic pool clean. The focused auto-QA and
  player-BMD contract tests PASS.
- This closes only the caster action/BMD-motion gate. It does **not** establish
  visual parity. Every skill remains `IN_PROCESS` while root/children/assets,
  renderer/pass, movement/collision, target reaction, lifetime and ownership
  are compared independently against complete Pegasus basic blocks. Late
  renderer failures are not patched from symptoms alone when the same model
  previously rendered successfully; its exact lifetime/owner exit must first
  be proven.
- The next complete-block comparison found a concrete Sword Wrath ownership
  error at Pegasus `0x67AADD -> 0x67A6F2`: the accent helper receives the
  player owner and resolves that owner's model bone 17. The adapter had looked
  up bone 17 on the dynamic `0x27B3` effect carrier instead. It now follows
  `effect.Owner`, validates the owner's live model/bone transform, and calls
  `TransformByObjectBone` on that owner. Lifecycle and Sword Wrath provenance
  tests PASS; the isolated client build exits 0 with only the known Crypto++
  LNK4099 warnings, SHA-256
  `EDE8B4C2C01A25E05DC4235C3C3B4E816582E0848881AB4EBEF93A9B57C3AC12`.
  Staging remains PASS at 166 files. Runtime deployment is pending because the
  currently running isolated Engine holds its executable open.
- The same audit rejected and corrected an earlier Raining Arrow assumption.
  Stack reconstruction across the cdecl terrain-height call and the
  `ret 8` random helper proves the first-volley angle at
  `0x6720C6..0x6722C0` is exactly
  `(270, Random(-10,10), 180)`, not the guessed native-RISE
  `(90, integer-random(-10,10), 180)`. The adapter now uses 270 degrees and
  the exact Pegasus `rand()%1001 * 0.001` distribution. The focused math test
  locks all four recovered constants.
- Replaced the auto-QA wall-clock-only pool snapshot with a last-skill gate.
  Wind Soul now records `AUTO_QA_SEQUENCE ... COMPLETE`; only then does the
  harness wait 15 seconds and inspect the dynamic pool. This prevents a lag or
  blocked render pump from producing a false mid-sequence leak verdict.
  A fresh run on deployed SHA-256
  `0554455A04B7BE076DD86575E7F99C3F09466EF6B4986E987527D0967988BA97`
  reached all 15 dispatches, all fourteen Pegasus caster frames progressed,
  allocated 264 effects with zero pool exhaustion, then reported
  `AUTO_QA_POOL 0/0 CLEAN`. RuntimeClient and Delivery hashes match.
- `/qa15`'s no-bow native-arrow fallback remains diagnostic coverage only and
  is not Pegasus visual-parity evidence. Raining Arrow remains `IN_PROCESS`
  pending the exact equipped-bow/later-volley/collision comparison.

## Havoc Spear tick-1 Nova correction - 2026-09-11

- Re-decoded the complete Pegasus root update at `0x666129..0x6661AF` and
  child constructor at `0x667147..0x667268`. At tick 1 Pegasus first creates
  the radial root burst, then falls through to the same randomized
  Nova/explosion child used at ticks `3,5,...15`.
- Corrected the isolated adapter, which previously started its odd-tick loop
  at tick 2 and therefore omitted the first Nova/explosion child. The exact
  schedule is now one root burst plus eight Nova/explosion bursts at ticks
  `1,3,5,7,9,11,13,15`.
- Extended the focused Havoc test with the complete tick schedule; it PASSes.
  Full isolated client build exits 0 with only the existing Crypto++ LNK4099
  warning class. Asset staging PASSes 165 files, and build/RuntimeClient/
  Delivery copies are byte-identical, SHA-256:
  `6DDA5CB28892E8D9C1C32C787871B8818B05B33A950FDD11636B822F8D2AD087`.
- This closes a source-proven missing Havoc child, not visual/runtime
  acceptance. Havoc Spear remains `IN_PROCESS` pending a fresh `/qa15`
  observation, target-loss/repeated-cast cleanup, and authoritative evidence
  for Elemental Charge's every-third-Havoc server damage behavior.

## Sword Blow Fly isolated clone - 2026-09-11

- Owner customization is now a separate isolated skill: `Sword Blow Fly`
  (`1502`). Recovered Sword Blow remains `1501`, retains
  `PLAYER_ATTACK_ONETOONE`, its original root/child subtypes and original
  neutral-light/material transfer. It no longer enters the flight state.
- Only `1502` uses the new re-keyed `PLAYER_ATTACK_ONETOONE_FLY` action,
  Blood Castle Archangel wing attachment, airborne hold/landing state,
  target-directed dive and blue-violet small-sword material subtype.
- Added isolated GameServer skill-data clone, attack route and GM commands
  `/testswordblowfly` / `/testswordblowflyoff`. The client clones the 1501
  requirements into a separately named `Sword Blow Fly` attribute row.
- Replaced the invalid high-ID atlas lookup with 14 source-backed Pegasus
  individual icons. The logical loader route remains `.jpg`, while staging
  now emits native 24-byte-header `.OZJ` packages; runtime reports
  `SKILL_ICON_LOAD ... PASS` for all 14 IDs. `1502` intentionally shares the
  exact 1501 bitmap route while retaining its separate tooltip/identity.
- Recentered BloodCastle01 mesh 3 on native player back bone 47 using its
  measured BMD-space center `(4.534477,111.72551,-32.5404)` after applying the
  effect rotation/scale. This is built and live but still needs owner visual
  confirmation of the final wing pose.
- Removed both held-input stand-action guards for the isolated 1502 action and
  synchronized the 14-key clip to the live native one-to-one AttackSpeed.
  Runtime 1502 dispatch intervals are now approximately `0.28..0.41 s` with
  repeated `RETRIGGER_HELD_AIRBORNE`; the old runtime was approximately
  `2.38 s` and waited for landing/complete before accepting another cast.
- Full isolated client and GameServer builds exit 0. Client has only the known
  Crypto++ LNK4099 warning class. Latest client SHA-256:
  `9B9638B3E5573F42A0DEEB89236C7D1B2209D478CFE54F71CA99A9E8320026FB`.
  GameServer SHA-256:
  `6732F6636D1EF93BA2482BCFFCA1A590BEB3DC4946FF223A0170752BF8C60CD4`.
- Focused runtime preview emitted three `1502` Fly casts, held the flight state
  across retriggers, completed landing and ended with `AUTO_QA_POOL 0/0 CLEAN`.
  The immediately following 1501 QA casts emitted only their original
  `CAST_DISPATCH` route and no Fly state. This proves identity/runtime
  separation. Latest live logs also prove 1502 presence in slot 15, source
  icon load/render and held-airborne retriggers; final icon appearance, wing
  pose and dive presentation remain pending owner visual confirmation.

## Deathside unlocked-FPS density correction - 2026-09-11

- Exact-window captures at `CAST_DISPATCH +200 ms/+900 ms` isolated the large
  white wash to Deathside's three ambient particle branches. Pegasus samples
  those `1/34`, `1/13` and `1/10` branches on its fixed simulation update;
  calling the same `rand() % N` logic from RISE's unlocked updater multiplied
  emission density at the observed 150+ FPS.
- Routed all three branches through native RISE `rand_fps_check(N)`, preserving
  the recovered probabilities per reference simulation tick. Fresh live
  capture at 151-162 FPS shows the opaque white wash is gone while the Reaper
  and thin smoke layer remain. Lifecycle and Deathside contract tests PASS;
  full isolated client build exits 0 with only the existing Crypto++ LNK4099
  warning class. Runtime/Delivery client SHA-256:
  `646863E0FAB645784B4CF2DD672349493CFB46A4254415CFEAEC60D70FDF11D3`.
- Re-ran the complete focused client/server suite after the change: all 26
  commands PASS (`TOTAL=26 FAILED=0`).
- This is a verified client visual-density correction, not full Deathside
  acceptance. The remote server is still older than `deathside-repeat-v3`, so
  it does not provide the second logical cast/scythe or authoritative rejection
  logs. Deathside and the other requested rows remain `IN_PROCESS`.

## Live terrain-blend and QA isolation correction - 2026-09-11

- A timestamp-aligned 16-frame capture identified the large opaque black
  terrain square exactly after Crusher Charge, not Elemental Charge or a
  custom-item texture collision. The recovered Pegasus renderer brackets its
  terrain-alpha calls with the native blend-state pair; the isolated adapter
  had omitted that pair.
- Restored `EnableAlphaBlend`/`DisableAlphaBlend` around every Pegasus
  terrain-alpha route in Crusher Charge, Dex Booster, Dragon Violent and Sword
  Wrath. A fresh live run shows the Crusher square is gone and the intended
  translucent rings remain. The focused blend-state regression gate PASSes.
- The remote GameServer is still the older single-Deathside build and did not
  send buff-removal packets. Added an isolated-client QA fallback that removes
  stale marker visuals before each sample, retains Sword Wrath through Sword
  Blow, and releases the persistent Deathside Reaper before Darkness. This is
  diagnostic isolation only; it does not replace validation of the staged v3
  GameServer cleanup packets.
- Live traces now prove both Crusher and both Elemental subtypes were deleted,
  Sword Wrath stayed active through Sword Blow and was removed at Shining Bird,
  and the delayed dynamic-pool snapshot reached `0/0 CLEAN`. All 26 focused
  tests PASS. Client Runtime/Delivery SHA-256:
  `AB53A36D7888CF2D69EB069C749AE646AA07E0059C04AAA1AA7A5005A6343D80`.
  Staged v3 GameServer SHA-256:
  `C86F98C626D9AAB2ECA92E90D909F59D17DCD0B69234258BAC17AEC10752AC71`.
- This closes the opaque-terrain defect and client-side QA contamination, but
  the twelve requested skills remain `IN_PROCESS`: server v3 deployment,
  Deathside second-cast/scythe evidence and owner comparison of the remaining
  visual chains are still open.

## Raining Arrow impact and one-run Deathside coverage - 2026-09-11

- The first no-pool-exhaustion run proved Raining Arrow's caster rings but no
  native arrow/terrain impact. `/qa15` uses one GM for every class, and the
  native bow-aware helper correctly created no arrow when that GM wore no bow.
- Added an isolated-only fallback native arrow only when the real equipment-
  aware helper creates nothing. The next live run logged 11 fallback arrows,
  11 terrain impacts, and all four previously absent impact-layer models
  `MAX_MODELS+42..45`; 303 Pegasus effects allocated with `POOL_FULL=0`.
  Production behavior still uses only the native equipment contract. Client
  Runtime/Delivery SHA-256:
  `3917F50DC9AA54A8D125C426CFF44FEB5B66B28B5DBF207ED1369ACE50A7A710`.
- Deathside requires two distinct logical casts by recovered design: summon
  the Reaper, then select attack action `3/4/8` and its scythe child. Updated
  only the isolated `/qa15` schedule to repeat step 01 after one second and
  stamped it `deathside-repeat-v3`; all other steps remain single-cast. The
  focused QA contract and isolated GameServer build PASS. Staged server
  SHA-256:
  `123EBEF82ECAB76C0E5F5A36FE5B743F0A75E6593A08D92E374612CBCD35FA55`.
- The v3 server still requires deployment to the isolated test VPS before the
  Reaper/scythe follow-up can receive runtime evidence. Raining Arrow and all
  other rows remain `IN_PROCESS` until visual parity/lifetime/cleanup review.
- Re-ran all 25 focused Pegasus/Wind Soul client and server test commands;
  every command exits 0. The latest isolated client remains running in the
  saved character session for owner visual review.

## Live QA15 root de-duplication result - 2026-09-11

- The isolated opt-in client logged in with the saved account, selected the
  GM character and sent `/qa15` once. All fourteen Pegasus skills plus Wind
  Soul produced live `CAST_ECHO` rows; this is packet evidence, not visual
  parity.
- The previous run recreated an attack root for every multi-target `0x19`
  damage row, producing 747 allocations and 309 `POOL_FULL` failures. Added a
  250 ms per-source/per-skill root gate for the eight target attack families;
  the dedicated `F3:35` packet still owns individual target impacts.
- The post-fix run produced exactly one `CAST_DISPATCH` for every one of the
  twelve requested `15:102..113` skills, 225 successful allocations, 41
  dynamic-renderer PASS checkpoints and **zero `POOL_FULL` rows**. Duplicate
  target echoes were explicitly logged as `CAST_ROOT_GATE ...
  DUPLICATE_SKIPPED`.
- Corrected trace terminology from `ACCEPTED/REJECTED` to `HIT/MISS`: that bit
  describes the target result, while GameServer `ECHO_SEND` is the acceptance
  proof. Full isolated client build exits 0 with only existing Crypto++
  LNK4099 warnings. Runtime/Delivery client SHA-256:
  `3917F50DC9AA54A8D125C426CFF44FEB5B66B28B5DBF207ED1369ACE50A7A710`.
- This closes the pool-exhaustion cause of the earlier missing BMDs, but all
  skills remain `IN_PROCESS` until the new run is visually reviewed for
  direction, color/blend, child cadence, reaction, lifetime and cleanup.

## Per-skill client dispatch trace - 2026-09-11

- Added isolated-only `CAST_DISPATCH` checkpoints to all twelve requested
  `15:102..113` client skill branches. Runtime diagnosis now follows
  `CAST_ECHO -> CAST_DISPATCH -> EFFECT_CREATE -> EFFECT_RENDER`, separating
  packet receipt, skill-switch routing, pool allocation and renderer entry.
- Extended `VerifyPegasusRuntimeRouting.ps1` so every row must contain its
  exact `DISPATCHED` trace as well as server dispatch and client lifecycle
  routing. The verifier PASSes all twelve rows.
- Full isolated client build exits 0 with only the existing Crypto++ LNK4099
  missing-PDB warning class. Build, RuntimeClient and Delivery client copies
  are byte-identical, SHA-256:
  `A64E41FC6E7D14A9CC881DA4161A2CC108C4361F72DE61C33ADA15FE26243074`.
  The new RuntimeClient is running as PID 5500 and startup asset preflight
  PASSes all requested families, but the remote GameServer remains unavailable
  and no live `CAST_ECHO` has been captured. All skills remain `IN_PROCESS`.

## QA15 packet-burst guard - 2026-09-11

- The first live `/qa15` attempt was followed by loss of the character session
  and subsequent GameServer rejection. This is not enough to prove a crash,
  but the command-start path did synchronously provision all fifteen skills
  and send fifteen add-skill packets before the timed cast sequence began.
- Removed that eager provisioning loop. `ProcessPegasusRuntimeQa` now remains
  the only QA15 provisioning site and adds or refreshes exactly the current
  sample immediately before its four-second cast step. Start logging now
  records `START_REQUEST`, target-check outcome, and spawn PASS/FAIL so a later
  server log can distinguish command receipt, target preparation, and cast.
- Added a native safe-zone preflight because the stock `0x19` receive path
  rejects self-buffs as well as attack skills while the caster is on a safe
  tile. `/qa15` now stops immediately with a visible notice instead of running
  fifteen misleading no-effect samples. Its start notice/log carries build
  stamp `step-provision-v2`, allowing the remote test to prove the new isolated
  GameServer is actually deployed before casting.
- Added `VerifyPegasusQaCommandSafety.ps1`; it guards the no-burst command
  contract and the per-step provisioning route. This verifier, the 15-step QA
  contract, charge-range contract, and Sword Wrath contract all PASS. Full
  isolated GameServer build exits 0. The built and both staged server copies
  are byte-identical, SHA-256:
  `CDCBB95E64180F748B01BF0AC365F90EBBF34CFB16465473AD0CA9CC3E931F23`.
- This is a safer diagnostic build, not runtime acceptance. Deploy it only to
  the isolated Pegasus test GameServer, then rerun one `/qa15` recording after
  the character server accepts login. All skills remain `IN_PROCESS`.

## Twelve-skill lifecycle regression gate - 2026-09-11

- Added `VerifyPegasusLifecycleCoverage.ps1` for the requested item range
  `15:102..113`. Every row must expose its isolated runtime source, initializer,
  updater, renderer, lifetime path, owner-death cleanup, asset/load guard and a
  visible child/layer path, and all three lifecycle functions must be connected
  through the native `ZzzEffect.cpp` dispatcher.
- The new gate PASSes all twelve rows. The existing runtime-routing gate also
  PASSes all twelve server dispatch plus client create/init/update/render
  routes. This prevents a later edit from leaving a catalog-only or root-only
  skill while still compiling.
- This is static lifecycle coverage only. Reaction semantics and visual parity
  still require the isolated `/qa15` run, so no row is promoted from
  `IN_PROCESS`.

## Runtime QA reconnect blocked by GameServer - 2026-09-11

- Launched the latest isolated client (`Engine.exe` SHA-256
  `6F0E886992D1A35FDC61F59F63D2D8F81CE649CE963459A08FFD6D04E4A0BF20`)
  and confirmed live TCP sessions to `103.90.224.58` on ports `44408` and
  `55858`.
- The character was initially visible ingame, but the session returned to the
  region screen before `/qa15` produced any new trace. Selecting `Viet Nam`
  again consistently produced the native `Mat ket noi may chu roi` dialog.
  Therefore no cast, renderer, or visual result is claimed from this attempt.
- The client remains the current diagnostic build and does not need rebuilding
  for this environment failure. Resume the one-run `/qa15` capture after the
  remote GameServer accepts a character session. Every skill remains
  `IN_PROCESS`.

## Authoritative server cast-gate trace - 2026-09-11

- The isolated GameServer now writes `[PegasusCast]` rows for every one of the
  fifteen QA skill packets. `RECEIVED` is followed by an exact rejection reason
  (`SOURCE/TARGET`, safe zone, duel, missing skill, delay, kill point, guild or
  class), while `ECHO_SEND` proves the accepted `0x19` response was emitted.
  This closes the old gap where `/qa15` logged `SUBMIT` even when a later native
  gate silently rejected the skill.
- Full isolated GameServer build exits 0. SHA-256:
  `B02B01FB5D18274D53AC9E216570ED10C992363A3BEE0D721686B3F9B9673C83`.
  Runtime server logs are still required; this instrumentation itself is not
  cast or visual acceptance.

## Runtime asset-loader defect fixed by startup preflight - 2026-09-11

- Added isolated startup `ASSET_PREFLIGHT` for all twelve non-Darkness asset
  families. It runs after native `OpenImages`, eagerly exercises the real BMD,
  embedded-texture and fixed-bitmap loaders, and records a skill ID plus PASS
  or FAIL before login. Darkness has no separate asset family and remains a
  cast/visual-chain runtime gate.
- The first real run proved three failures: Dex Booster bitmaps, Chaos Blade's
  two animation-only carriers, and Spearstorm assets. The shared cause for Dex
  Booster/Spearstorm also affected Spiral, Elemental and Raining Arrow layers:
  adapters passed physical `.OZJ` names to `CGlobalBitmap`, whose dispatcher
  accepts logical `.jpg` and then resolves the encrypted `.OZJ` sibling.
- All Pegasus runtime texture calls now use the native logical `.jpg` contract.
  Elemental Charge also now binds its five textures to the reserved fixed IDs
  actually consumed by its renderer, instead of loading anonymous IDs.
  Chaos Blade recognizes `M_sword_effect.bmd` as the proven 0-mesh/15-bone/
  1-action carrier, matching the existing Spiral carrier rule.
- Fresh isolated startup now reports PASS for every one of the twelve families;
  all visible BMD families reach GPU upload, while Spiral and Chaos carrier
  `NO_MESH` rows are expected and paired with family PASS. Full client build
  exits 0 with only the existing Crypto++ LNK4099 warning class. Running Engine
  SHA-256:
  `6F0E886992D1A35FDC61F59F63D2D8F81CE649CE963459A08FFD6D04E4A0BF20`.
- This is real loader/GPU evidence and fixes a common reason the 13 skills had
  no/partial visuals. Cast, renderer output and visual parity still require a
  fresh `/qa15`; all rows remain `IN_PROCESS`.

## One-run QA buff isolation - 2026-09-11

- The isolated `/qa15` sequence now removes all six Pegasus native buff
  markers before each sample so Darkness, Dex Booster and the MG charge auras
  cannot contaminate later visual/pool observations. Sword Wrath is the only
  explicit exception: it is retained for the immediately following Sword Blow
  sample to exercise the proven +1 range/radius path, then removed.
- Starting, stopping, dying during, or completing QA also clears these markers.
  The server log records `BUFF_CLEANUP` counts, making the cleanup visible in
  the same evidence stream without changing normal production skill behavior.
- The expanded QA15 contract, charge-range contract and Sword Wrath contract
  PASS. Full isolated GameServer build exits 0; SHA-256:
  `D74C70BF17F0167310F6E5F0503B846BFC61317BA3C7BC906D444462CD2C1984`.
  This improves the next runtime recording but is not runtime acceptance; all
  skills remain `IN_PROCESS`.

## Per-model renderer acceptance trace - 2026-09-11

- Added isolated-only one-shot `EFFECT_RENDER` PASS/FAIL telemetry for every
  dynamic Pegasus model routed through the common effect renderer. PASS and
  FAIL are tracked independently per model, so an early unavailable frame does
  not hide a later successful render.
- The next `/qa15` log can now prove the complete diagnostic sequence per
  model: `MODEL_ENSURE` failure or `GPU_UPLOAD`, then `EFFECT_CREATE`, then
  `EFFECT_RENDER`. This separates missing BMD/texture, pool exhaustion,
  frustum/pass routing and renderer rejection without visual guesswork.
- Focused load-guard verifier PASSes and the full isolated client build exits
  0 with only the existing Crypto++ LNK4099 warning class. The staged/running
  Engine SHA-256 is
  `ADCC9DF97B115D2AE346BD7FE7415F89F93FFFAC64F59AF255C72B84A2584D9D`.
  Runtime visual acceptance remains open.
- Added `VerifyPegasusSoundAssets.py`. All 23 staged WAV files resolve from the
  compiled provenance catalog, match their SHA-256 values and expose readable,
  non-empty PCM headers. Every DirectSound allocation, bounds check and cleanup
  loop already uses the extended `kSoundBufferCapacity`; sound runtime playback
  still awaits ingame evidence.

## Complete model-family load guards and diagnostic client - 2026-09-11

- Audited the common `CreateEffect` boundary after the dynamic-tail allocation,
  initialization and shader-upload fixes. Eleven Pegasus model families had an
  explicit lazy-load guard, but Deathside models `MAX_MODELS+47..51` did not.
  The common boundary now guards all twelve dynamic families, including
  Deathside, before allocating the effect.
- A failed model or embedded-texture load now emits isolated-only
  `MODEL_ENSURE value1=<model> value2=<subtype> result=FAIL`. This closes the
  diagnostic gap where failure occurred before `GPU_UPLOAD` and
  `EFFECT_CREATE`, producing no prior trace row.
- Added `VerifyPegasusModelLoadGuards.ps1`. It PASSes all twelve loader families
  and the failed-load trace. Runtime routing, model storage, all 53 dynamic IDs
  and shader-tail tests also PASS.
- Full isolated client build exits 0 with only the existing Crypto++ LNK4099
  warning class. The staged and running Engine hash is
  `51212A7DBE96EAC65F838A9C7BAD9E2996007D15950E5E4A711C213F683C790B`.
  Startup log proves `MODEL_STORAGE_INIT value1=10128 value2=10256 PASS`; the
  process is responding and connected to both ports 44408 and 55858.
- Windows Graphics Capture still fails with `0x80004002`, so no blind input was
  sent and no `/qa15` runtime result is claimed. All rows remain `IN_PROCESS`.
- Ran every focused Pegasus client test script, not only the aggregate route
  verifier: all 17 scripts PASS. The RuntimeClient asset tree now matches all
  151 staged Pegasus files by relative path and SHA-256, and all 43 embedded
  BMD texture references resolve. Its stale Sword-Blow-only README and checksum
  were replaced with the current `/qa15` workflow and running Engine hash.

## MG charge cast-range contract - 2026-09-11

- Added a focused server contract for the exact absolute ranges proven by the
  hash-pinned Pegasus `Skill.xml` and `BuffEffect.xml`: Chaos Blade remains at
  its native range 3 without a charge, becomes range 6 under Spiral Charge and
  range 5 under Crusher Charge; Havoc Spear remains range 6 normally and
  becomes range 7 under Elemental Charge.
- `SkillDragonViolent` now resolves those marker states before its target-range
  gate. Dragon Violent, Raining Arrow and every unrelated skill retain their
  configured native range. The contract also tests accepted/rejected boundary
  tiles and PASSes under `/W4 /WX`.
- Full isolated GameServer build exits 0. SHA-256:
  `C00A94896E668EE95191C5095187F6578148AB1403001795AA9E572F8ADFDAD5`.
  The QA15, Sword Wrath, charge-range and twelve-row runtime-routing tests all
  PASS. No client source changed in this step, so the already staged client is
  still current and did not require rebuilding.
- This is exact server range behavior only. The undocumented attack, defense,
  speed and success formulas, Elemental third-Havoc Nova behavior, and ingame
  runtime validation remain `IN_PROCESS`; no values were guessed.

## High-buff formula client-callsite audit - 2026-09-11

- Added hash-guarded `FindPegasusCallContexts.py` and scanned every direct call
  to Pegasus client buff lookup `0x6E9E47`. Among the new effect IDs, exact
  callsites were found for `0xD6`/Spiral at `0x677418` and
  `0xE4`/Darkness at `0x65BD41`; both sit in recovered visual/object handlers.
  No direct lookup call using `0xD7`/Crusher, `0xE3`/Elemental or
  `0xE5`/Dex Booster was found.
- The XML continues to prove roles/descriptions only and contains no numeric
  stat magnitudes. This client-side negative evidence does not prove a server
  formula, so the isolated GameServer deliberately retains marker state rather
  than inventing attack/defense/speed/success values. Those formula gates stay
  `IN_PROCESS` pending authoritative server or runtime evidence.

## Dynamic BMD initialization-boundary fix - 2026-09-11

- Found a second common model-tail defect after the shader upload fix.
  `OpenPlayers` allocated `MAX_MODELS + 1024 + kDynamicModelCapacity` BMD
  records but zero-initialized only `MAX_MODELS`. Pegasus loaders use
  `NumMeshs`/`Meshs`/`IndexTexture` as the loaded-state guard, so garbage in
  `MAX_MODELS+0..127` could make a dynamic BMD appear already loaded or carry
  partial stale state.
- The initialized range now uses `RuntimeModelLimit(MAX_MODELS)`, matching the
  reserved active model range. `VerifyPegasusModelStorage.ps1` proves both the
  allocation and initialization callsites cover the isolated tail and rejects
  the legacy-only boundary.
- Added `VerifyPegasusRuntimeRouting.ps1`; it PASSes all twelve group-15 rows,
  proving each has a GameServer dispatch case plus client skill switch and
  create, initialize, update and render callsites. This is a regression guard
  for route omissions, not proof that the visual parameters match Pegasus.
- Added isolated startup trace `MODEL_STORAGE_INIT`. The freshly staged client
  emitted `value1=10128 value2=10256 result=PASS`, proving the running binary
  initialized the complete 128-slot tail. Full client build exits 0 with only
  the known Crypto++ LNK4099 warning class. Runtime Engine SHA-256:
  `AC29C14956DB4C08E508245D970173B1BED45CFFEE3F64B620DB4717EE06CFCC`.
- This fixes a real load-state defect but is not visual acceptance. A complete
  `/qa15` cast trace and ingame review are still required; every skill remains
  `IN_PROCESS`.

## Runtime effect-pool tracing build - 2026-09-11

- Extended the isolated-only runtime trace at the common `CreateEffect` pool
  boundary. Every Pegasus dynamic model now records `EFFECT_CREATE` with
  `ALLOCATED` or `POOL_FULL`; together with `CAST_ECHO` and `GPU_UPLOAD`, one
  `/qa15` run can distinguish server dispatch, CPU/GPU model upload and effect
  pool allocation without inferring the failure from screenshots.
- Full isolated client build exits 0. The only linker-warning class remains the
  existing Crypto++ LNK4099 missing-PDB warning. The 53-ID dynamic-model test,
  shader-tail test and all 43 staged BMD sibling-texture references PASS.
- The staged isolated runtime client SHA-256 is
  `DF1E5B0BBCC42D4713A6B3D9AA34D47C200A3E594F87DA1D86C2C2F983FCB39A`.
  It is running from `ExMain_RISE_PC/Tests/PegasusBuild/RuntimeClient` and has
  established ConnectServer/GameServer sockets, but no `CAST_ECHO` has yet
  been captured. This is build/diagnostic evidence only, not runtime visual
  acceptance; all skills remain `IN_PROCESS`.

## Shader model-tail runtime fix - 2026-09-11

- Owner QA showed Sword Blow and Wind Soul rendering while the other thirteen
  skills had missing or partial models. Source debug found a common renderer
  boundary: all thirteen Pegasus families use the isolated model tail at
  `MAX_MODELS+0..52`, but `OGL330MODEL::ConvertOldMeshToVaoMesh` rejected every
  ID at or above `MAX_MODELS`. Their BMDs could therefore load on the CPU while
  receiving no VAO/GPU upload on the shader renderer. Sword Blow stayed visible
  because its nine model IDs are native enum entries below `MAX_MODELS`.
- The shader upload gate and shutdown cleanup loops now use the explicitly
  allocated `MAX_MODELS + kDynamicModelCapacity` boundary. A focused capacity
  test proves the first and last reserved tail slots are accepted and the first
  out-of-range slot is rejected.
- The staged BMD dependency audit also found and restored two exact sibling
  textures that caused independent loader failures: `alpha_line2mono2.OZJ` for
  Havoc Spear and `ground_wind.OZJ` for Dragon Violent. All 43 embedded texture
  references and 61 static Pegasus runtime paths now resolve in staging.
- Focused shader-tail test PASSes and the full isolated client build exits 0;
  only the existing Crypto++ LNK4099 missing-PDB warning class appears.
- Added an isolated-build-only runtime trace. Every accepted/rejected high-skill
  `0x19` echo writes `CAST_ECHO`, and every dynamic Pegasus BMD reaching the
  shader uploader writes `GPU_UPLOAD` with its model ID, mesh count and result
  to `PegasusRuntimeQA.log`. The trace strings are verified present in the
  isolated binary and are not enabled for ordinary builds. Latest traced
  isolated Engine SHA-256:
  `409A2EF2E2213584E343E358D678775E9B9C4B2094E679A3EA5AC43D41625FEA`.
- This fixes a source-proven common cause, not runtime acceptance. All affected
  skills remain `IN_PROCESS` until a fresh isolated `/qa15` run proves their
  dispatch, complete visuals, motion/reaction, lifetime and cleanup. The first
  retry reached the saved character but the Vietnam server then disconnected,
  so the post-fix runtime pass is still pending.

## Sword Wrath joint/atlas ownership correction - 2026-09-11

- Recovered the exact Sword Wrath `0x7F29` call chain. The thirty
  `Swordeff_mono2line` objects are joints (`CreateJoint` at `0x796B7A`), not
  particles. They now use the native joint pool with the recovered 20-tick
  lifetime, 5..10 tails, 10..50 scale, +/-500 XY and -10..20 Z spawn box,
  distance-times-0.06 velocity, centre heading and `LifeTime/20` RGB fade.
- Corrected accent ownership from `0x67AB1D..0x67AD8F`: only the `0x27B3`
  root may emit `pin_star02_red` and `smoke01_strong`. The earlier adapter let
  every `0x27B4` layer emit them and could multiply the source particle count
  fivefold. Exact 20/30 lifetimes, 160 smoke threshold, bone 17, 0.45 pin-star
  multiplier and per-tick movement constants are now locked in the focused
  test.
- Added the missing 15-frame `fire_15fmono` 4x4 atlas offsets and restored all
  four recovered render-action gates (`0x43`, `0x9D..0x9F`). Focused Sword
  Wrath, 45 bitmap-ID, 53 model-ID, 14-row catalog and `/qa15` contract tests
  PASS. Hash staging now reports 149 unique files after removing one duplicate
  `flareRed` manifest row.
- Full isolated client and GameServer builds exit 0; only the existing
  Crypto++ LNK4099 warning class appears. Staged SHA-256 values:
  `Engine.exe` =
  `AB1DDC3B4FC6B688626B28606CDE08D2DF83C343C3B82DF028D0D4BE0758BDE6`,
  `GameServer.exe` =
  `2F0F63CECB4C1263E30B277F52CFAE35A85B59216944AC0714DF423EBE86B05C`.
- Sword Wrath remains `IN_PROCESS` until `/qa15` proves its cast, atlas,
  joint convergence, buff duration and cleanup in the isolated runtime.

## Raining Arrow target damage-sheet completion - 2026-09-11

- Recovered the missing `0x2793` target-follow update from the hash-pinned
  Pegasus block `0x6710E6..0x671261`. The target child follows bone 0, not
  merely the object's ground origin, and emits exactly two transient damage
  sprites per update.
- Added the source-owned `Effect\\arrow_damge.OZJ` resource (SHA-256
  `923BE2238E33DDC7059618CC87EB7C732CD1DA265F98BEAB3DB058CFB4676546`)
  through RISE's dynamic bitmap allocator. It deliberately does not consume
  numeric ID 33000: that value is `BITMAP_EFFECT_TEXTURE_END`, immediately
  before the custom/nonamed stream, so assigning it would repeat the texture
  collision class found during Sword Blow.
- Ported the exact 4x4 atlas envelope: elapsed frame clamped to 0..15,
  light `LifeTime/16`, two sprites per tick, random rotation 0..359, and
  scale `1.8 * random(1..2)` using the recovered `rand()%1001` interpolation.
  The sprite renderer now keeps atlas frame separate from blend mode and uses
  alpha blend for every one of the sixteen frames.
- Added a focused Raining Arrow math/atlas test. It PASSes, as do the 45-ID
  collision test, 53-model collision test and 150-file hash staging. Full
  isolated client build exits 0 with only the existing Crypto++ LNK4099
  warnings. Staged `Engine.exe` SHA-256:
  `17156DF6DF062A48E5A8C52F15C8A776181F64357EFDA3268BA3306ADD16C7DF`.
- Raining Arrow remains `IN_PROCESS` until `/qa15` proves target centering,
  impact timing, full visual parity and cleanup in the isolated runtime.

## Elemental Charge cast/buff ownership correction - 2026-09-11

- Recovered the exact ownership callers `0x664C95..0x664DF0`. The accepted
  cast creates subtype 0 at owner bone 17; buff add creates subtype 1 only
  when subtype 0 is absent (viewport/relog restoration); buff removal deletes
  both subtypes.
- Added that native marker-171 bridge and preserved the existing exact
  five-bone/five-colour renderer. This prevents duplicate permanent roots and
  closes the missing buff removal/death cleanup path without guessing the
  still-unproven third-Havoc damage formula.
- Focused Elemental ownership/layer test PASSes. Full isolated client build
  exits 0 with only existing Crypto++ LNK4099 warnings. Staged
  `Engine.exe` SHA-256:
  `20EB444F10CA318E0F20F2866C3BB6960458F79B42739964886029FC1D413594`.
- Elemental Charge remains `IN_PROCESS` pending server stat/third-Havoc
  evidence and owner `/qa15` runtime validation.

## Crusher Charge persistent-aura completion - 2026-09-11

- Recovered the previously missing Pegasus `0x27B1` family from exact
  constructor/update/renderer and buff add/remove callers
  `0x65A46E..0x65B145`. Buff `0xD7` owns one subtype-1 aura and deletes it
  authoritatively when the buff ends.
- Added the persistent owner-following aura with exact lifetime `9999999`,
  scale `1.5`, light `(0.2,0.3,0.7)`, two `flare_mono` terrain passes, and
  the recovered 17-bone table
  `{12,17,5,10,36,27,37,28,11,35,2,3,36,20,27,4,26}` with its three exact
  pulse groups.
- Connected native collision-safe marker 170 to aura creation/removal. The
  one-shot cast core/ring/children remain separate, matching Pegasus
  ownership rather than leaving a permanent cast artifact.
- Reserved isolated dynamic model slot `MAX_MODELS+52`; the original
  `MAX_MODELS+11` candidate was rejected because Shining Bird already owns
  it. Added a cross-skill model-ID gate proving all 53 current dynamic model
  IDs uniquely and contiguously cover `MAX_MODELS+0..52`, so later imports
  cannot silently repeat this collision. A focused aura/bone/pulse test PASSes.
  Full isolated client build exits
  0 with only the existing Crypto++ LNK4099 warning class. Staged
  `Engine.exe` SHA-256:
  `82CE009FEB874A803CDABF5FBE427D6A67F16419ED02B2804D5ECB5E30C22FFE`.
- Crusher Charge remains `IN_PROCESS`: original server stat formula and
  owner `/qa15` visual/lifetime/death cleanup validation remain open.

## Havoc Spear root/joint provenance correction - 2026-09-11

- Corrected the target-scale branch from Pegasus `0x66685F..0x6668A9`:
  values below `0.8` now clamp up to `0.8`, normal values remain unchanged,
  and values above `2.5` clamp down to `2.5`. The earlier adapter had this
  condition reversed.
- Removed the extra root impact burst that was not present in Pegasus. The
  root now creates the line, exactly six recovered custom joints, one
  scale-8 terrain shockwave, and both recovered cast/explosion sounds.
- Rebuilt the exact six-row joint table, radial target coordinates,
  subtype/light values, scales `32.4, 30, 24, 58.5, 58.5, 58.5`, 10-tick
  lifetime, 4..6 velocity, eight-tail ownership and final-five-tick fade.
- Added fixed isolated bitmap IDs `32955/32956` for `somasi01mono_R` and
  `ad001`, below the custom/nonamed loader stream. Hash staging now covers
  149 files. Havoc math/joint provenance and 45-ID collision tests PASS.
- Full isolated client build exits 0; only the existing Crypto++ LNK4099
  missing-PDB warnings were emitted. Staged isolated `Engine.exe` SHA-256:
  `440CC878276ECD84E10C91EC9B822F3638756F80BD878C5F4F5726E432E006BC`.
- Havoc Spear remains `IN_PROCESS` pending the one-run ingame `/qa15`
  observation; build/tests are not runtime or visual acceptance.

## One-run full 15-skill runtime QA harness - 2026-09-11

- Corrected the acceptance denominator: fourteen Pegasus group-15 item rows
  `15:100..113`, plus standalone Wind Soul as step 15.
- Added isolated GameMaster command `/qa15`. It provisions all 15 skills and
  submits them through the real GameServer skill packet path at four-second
  intervals: target skills select the nearest valid monster; buff skills use
  the caster. `/qa15stop` cancels the sequence, and disconnect/death cleanup
  prevents a stale run from transferring to another object slot.
- Hardened the command for a true one-command recording: when no valid target
  exists it now creates eight isolated monster-275 targets automatically, and
  the selected QA monster is raised to one-billion life before attack steps
  so an early high-damage cast cannot turn later steps into `NO_TARGET`.
  Mana/BP are also refilled immediately before every real receive-path
  submission. Manual `/spot` is no longer required.
- Added numbered ingame notices and `[PegasusQA15]` server-log rows for every
  step. Isolated GM class and weapon bypasses cover only these 15 QA skills;
  production builds remain unchanged because all routes are gated by
  `PEGASUS_ISOLATED_TEST`.
- Added `PEGASUS_RUNTIME_QA15.md` with the single-run procedure and a 15-row
  evaluation sheet. Focused sequence/uniqueness contract PASSes, and the
  isolated Ex603 GameServer build exits 0. Staged isolated `GameServer.exe`
  SHA-256: `2F0F63CECB4C1263E30B277F52CFAE35A85B59216944AC0714DF423EBE86B05C`.
- Re-ran the complete focused suite after Havoc, Crusher, Elemental and QA
  harness corrections: all 16 client/server contract tests exit 0. This is
  static/integration evidence only, not the requested ingame evaluation.
- Runtime results remain `NOT RUN`: the Windows-control surface exposed no
  targetable game window in this session, and the updated isolated GameServer
  has not yet been launched/deployed. No skill is promoted from `IN_PROCESS`
  based on the harness or build.

NEXT EXACT ACTION: run the matching isolated server/client, record once,
enter `/qa15`, then review the numbered 01/15..
15/15 casts against the evaluation sheet.

## Spiral Charge animation/emitter/anchor renderer recovery - 2026-09-11

- Completed the next bounded decode through Pegasus
  `0x676D9C..0x6779C3`. `01spa_skill.bmd` is an animation-only carrier with
  0 meshes, 9 bones and 1 action; the old adapter incorrectly required a
  renderable mesh, so it reopened/rejected the valid model. It now accepts
  the bone/action contract and advances action 0 at the recovered speed 0.3.
- Corrected child coordinates from `0x67719B..0x677342`: only the `0x27A9`
  flare is raised by 40. The two shockwaves and the Steam/Smoke carriers stay
  at the root point; the prior adapter mutated the root and shifted every
  child upward.
- Replaced the guessed fixed Steam/Smoke sprites with the source-owned
  emitter behavior. The two invisible 40-tick carriers use the exact
  descending-sine gate, emit particle subtypes 4/12, and those particles now
  use the recovered 10/12-tick lifetimes, scale growth and 6-tick smoke
  fade split from `0x676D9C..0x676F0A`.
- Recovered the persistent `0x27AD` renderer from
  `0x67766E..0x6779C3`: three owner-centred `flare01_red` pulses plus the
  exact 17-entry owner-bone table `{12,17,5,10,36,27,37,28,11,35,2,3,36,
  20,27,4,26}` with its three time-varying light groups. RISE's native
  `BITMAP_LIGHT_RED` is the matching `effect\\flare01_red` resource role, so
  no unsafe extra texture ID was allocated.
- The apparent "nine auxiliary particles" were resolved through the real
  `CreateJoint` caller at `0x796B7A`, its `0x7F19` initializer
  `0x79C9E4 -> 0x67739D`, and mover `0x7ABA9E -> 0x67758C`. They are nine
  flareRed joints, not particles. Each carries bone index 0..8, activates
  after root animation frame 0.75, follows that bone, uses scale 20,
  lifetime 30 and a five-tail ribbon. The isolated RISE joint path now owns
  this exact lifecycle.
- Focused Spiral contract, 14-row catalog, 43-ID collision and 147-file
  staging tests PASS. Full isolated client build exits 0 with only the
  existing Crypto++ LNK4099 warnings. `Engine.PegasusImport.exe` and isolated
  `Engine.exe` SHA-256:
  `69F1043C7260438C9441FA2D5B003DC72DE2458B8C107460BF42F28D04997C91`.
- Spiral Charge remains `IN_PROCESS`: the original server stat formula and
  owner ingame comparison/cleanup evidence remain open. No visual parity is
  inferred from this build.

## Spiral Charge native-buff bridge correction - 2026-09-10

- Confirmed the authoritative Pegasus ownership chain: skill `0x7DE` (2014)
  uses buff `0xD6` (214), and the persistent visual routine
  `0x676CFA..0x676D99` removes the prior `0x27AD`, resolves owner bone 17,
  raises Z by 10 and recreates the anchor. Its update at
  `0x6773E4..0x677589` keeps the anchor only while that buff is present.
- RISE 5.2 cannot copy Pegasus buff 214: its native client enum ends at 209
  and the server skill table already uses 214 for Drain Life. The existing
  collision-safe semantic mapping `214 -> 169` is therefore retained.
- Fixed the missing client bridge for the isolated markers. Buff states
  168..173 can now register/unregister even though the stock
  `BuffEffect.bmd` has no entries for them. Spiral marker 169 creates the
  bone-17 anchor on registration and deletes it on removal; the anchor now
  checks marker 169 rather than the unrelated cast action.
- Added a focused source/native translation and anchor-lifetime contract
  test. It PASSes, as do the 14-row catalog, 43-ID collision test and
  147-file hash staging. Full isolated client build exits 0 with only the
  existing Crypto++ LNK4099 warning class. `Engine.PegasusImport.exe` and
  isolated `Engine.exe` SHA-256:
  `3AFA625CB49ECCBFA10F3C652CD50A3918B6C6B04ED6AA3D7B5107643ED0ADEC`.
- Spiral Charge remains `IN_PROCESS`: the remaining model/bone renderer
  emission block, exact stat formula, removal/pool behavior and owner ingame
  visual/timing validation are still open. Compilation is not runtime
  acceptance.

## Dex Booster initial-particle completion and attribution correction - 2026-09-10

- Corrected an audit-label hazard before extending the effect: Pegasus block
  `0x676D9C..0x6779C3` belongs to skill `0x7DE` (2014 Spiral Charge), not Dex
  Booster. The authoritative Dex Booster skill check is `0x7E8` (2024) at
  `0x661972`, with its effect chain in `0x66110F..0x661CC6`.
- Recovered the omitted Dex Booster initializer layers from the correct block:
  nineteen native `BITMAP_FLARE` joints (subtype 48, scale 40), then twenty
  `Impack03` particles with light `(0.4,1,1)` and input scale `0.005`, followed
  by the already-connected three ground rings and two flare roots.
- Implemented the specialized `Impack03` subtype-0 lifecycle from
  `0x66184D..0x661C20`: 60-tick lifetime, randomized radial velocity/Z drift,
  the exact 30-tick alpha/scale phase change, and alpha-based release below
  `0.1`. Generic particle movement is disabled for this route to avoid double
  displacement.
- Corrected the `0x27A1` role from the same bounded block: it is an invisible
  30-tick emitter, not a directly rendered fixed-scale sprite. Each update now
  emits the registered `flare01` particle, subtype 5, at randomized scale
  `2.5..4.4`. The aura's `Impack03`/`Shiny02`/`hikorora` layers now use the
  native owner-aware sprite API instead of bypassing sprite ownership with
  direct bitmap rendering.
- The one-shot `0x279F` orchestrator now expires at initialization as in the
  source instead of lingering for a guessed tick. A focused constructor/
  particle contract test PASSes. Full isolated client build exits 0; warnings
  remain only Crypto++ LNK4099. The 14-row catalog, 43-ID collision test and
  147-file hash staging PASS. `Engine.PegasusImport.exe` and isolated alias
  SHA-256: `F5410FAEBC18AE23270FD580DB0B96314ED91FBE253613B7F9A35B0B51D6EB81`.
- Dex Booster remains `IN_PROCESS`: its original server stat formula, buff
  removal/pool behavior and owner ingame visual/timing validation are open.

## Chaos Blade whole-image sound-xref audit - 2026-09-10

- Scanned the entire hash-pinned Pegasus executable for immediate references
  to sound buffers `0x3DC` (Hit 2) and `0x3DD` (Explosion), extending the
  earlier bounded effect-handler search. `0x3DC` appears only in the sound
  registration block and the generic sound-ID mapping switch at `0x834D4F`;
  there is still no proven Chaos Blade runtime play call.
- `0x3DD` appears in registration and at `0x5F5716`, outside the recovered
  Chaos Blade `0x654000..0x655300` chain. That routine has not been proven to
  be owned by skill 2012, so the adapter deliberately does not play it. The
  exact `0x3DB` Hit-1 call at `0x654858` remains the only runtime sound added.
- No code was changed from this evidence audit. Chaos Blade remains
  `IN_PROCESS`; attach Hit-2/Explosion only if a real caller/ownership path or
  ingame trace proves them, never from filenames alone.

### Caller classification follow-up - 2026-09-12

- Decoded the complete caller at `0x5F4C96`: it enters a UI render state and
  invokes the neighboring routines `0x5F51D9`, `0x5F53BB`, `0x5F5666`,
  `0x5F57EF` and `0x5F4D06` as one interface-render pass.
- Decoded `0x5F5666`: the `0x3DD` lookup at `0x5F5716` is followed by string
  formatting and a text-render call using screen coordinates. It is therefore
  a numeric UI text resource lookup, not a call that plays sound buffer
  `0x3DD`, and is not owned by Chaos Blade.
- The focused Chaos Blade verifier now rejects any future
  `PlayBuffer(kChaosBladeHit2Sound...)` or
  `PlayBuffer(kChaosBladeExplosionSound...)` addition until a genuine combat
  caller is recovered. Hit-1 at `0x654858` remains the sole proven runtime
  sound. This closes the earlier `0x3DD` false positive without inventing
  cadence; runtime visual/reaction/pool acceptance remains open.

## Dragon Violent radial-head ownership correction - 2026-09-10

- Scope remains **15 skills total**: the fourteen Pegasus group-15 items
  `15:100..113`, plus standalone Wind Soul. Wind Soul is not a fifteenth
  group-15 row.
- Re-decoded the Dragon Violent head update at `0x662E8F`: the eight `0x27C3`
  heads keep their radial spawn positions and do not follow the target. The
  earlier adapter incorrectly copied the target position every update, which
  collapsed the ring onto the victim. Only the `0x27C7` target-impact route
  follows the target.
- Recovered the omitted head child at `0x662966..0x6629E1`: native
  `BITMAP_JOINT_SPIRIT2`, subtype 18, Z offset -140, scale 75, light
  `(0.0785,0.314,0.92)`, owned by its head. The isolated RISE adapter starts
  the head at renderable alpha because the Pegasus field at old offset `0xA8`
  is not blindly treated as the native RISE alpha field.
- Focused Dragon Violent ownership/joint test PASSes; the 14-row catalog,
  43-ID collision test and 147-file hash-guarded staging also PASS. The full
  isolated client rebuild exits 0 with only the existing Crypto++ LNK4099
  warnings. `Engine.PegasusImport.exe` and isolated `Engine.exe` SHA-256:
  `4592B053982E5445BDECFDD7E06848F2A4766224EBCC3134B10159EF3A00377B`.
- Dragon Violent remains `IN_PROCESS` pending owner ingame validation of the
  radial placement, renderer/alpha, target impact, timing, damage and pool
  cleanup. Compilation and focused tests are not visual acceptance.

## Shining Bird animation/cleanup completion - 2026-09-10

- Re-decoded the previously omitted update tail at
  `0x673781..0x673808`. Pegasus sets model action from the effect, calls the
  native BMD animation routine with speed `0.5`, and releases the effect when
  the non-looping animation finishes. The isolated dynamic model ID does not
  pass through RISE's built-in model animation range, so the adapter now owns
  this exact call explicitly.
- Hash-pinned `lightofbird_attack.bmd` has action 0 with 13 keys; the recovered
  speed `0.5` aligns with the existing 26-tick root lifetime. The existing
  server route already uses the native 1.5x6 directional frustum, one delayed
  hit per accepted target and a bounded attack count; no target-centered
  substitute was added.
- Focused Shining Bird geometry/color/animation test PASSes. Full isolated
  client build exits 0 with only the existing Crypto++ LNK4099 warnings.
  `Engine.PegasusImport.exe` and the isolated `Engine.exe` alias SHA-256:
  `57F0B8C27BA2ADD1CBCD8872DB050CE0DE0705BB42EA4F0242A5EE431EAF97B8`.
- Shining Bird now has source evidence for dispatch, action/sound, directional
  target selection/damage, root, three owner-tracked pillars, model animation,
  bone lights, secondary particles and cleanup. It remains `IN_PROCESS` until
  owner ingame direction, hit timing, visual and pool-reuse acceptance.

## Spearstorm direction and blade-fade correction - 2026-09-10

- Reconfirmed the authoritative denominator after the owner's correction:
  **14 Pegasus skills (`15:100..113`) plus standalone Wind Soul = 15 total**.
  Wind Soul is not one of the fourteen Pegasus group-15 rows.
- Re-decoded Pegasus `0x675ECB..0x675F42`. The falling-blade direction is
  `X=cos(yaw)*cos(pitch)*-200`, `Y=sin(pitch)*-200`, and
  `Z=sin(yaw)*cos(pitch)*-200`; the previous adapter had the sine/cosine axes
  transposed. The initial blade position now also uses the recovered
  `StartPosition-Direction*6.0` lead instead of the provisional `0.5` lead.
- Recovered blade rendering at `0x675848`: alpha is
  `min(BlendMeshLight/0.7,1)`. The blade and crack now initialize velocity to
  `1.0`, and the blade fade is driven by the existing recovered
  `BlendMeshLight -= 0.02331*dt` envelope.
- The focused Spearstorm test now locks the corrected vectors at yaw/pitch
  samples, the six-tick lead, lifetime/impact fan, layer provenance, and blade
  alpha. It PASSes. The isolated client rebuild exits 0; warnings remain only
  the existing Crypto++ LNK4099 missing-PDB class. `Engine.PegasusImport.exe`
  and its isolated `Engine.exe` alias both have SHA-256
  `50796343286881AC882EA311B5E0612B58C9C3F1B58561B12DBEECF8DC0E7718`.
- GameServer's Spearstorm attack cap was checked rather than guessed:
  `CHECK_SKILL_ATTACK_COUNT(count)` expands to `++count`, so its loop already
  advances the count and no server edit was made for that false lead.
- Continued through Pegasus `0x676131..0x676201`. Impact is not terrain-only:
  the original also forces the one-shot impact whenever the next lifetime
  (`current-dt`) reaches `66`; this matters for yaw 0 where blade Z velocity is
  zero. The adapter now preserves that fallback. The hit sound crossing was
  also corrected from the reversed post-decrement test to the original
  `current > 64 && current-dt <= 64` condition. Focused test and the isolated
  rebuild PASS; staging verifies 147 files, the 14-row catalog and 43-ID
  collision tests PASS. Latest client and isolated alias SHA-256:
  `19B7B84B994CAB4305E6F6C70CC6DEB86572A6F02B3213486499C1A985F8FE3B`.
- Spearstorm remains `IN_PROCESS`; this build is ready for isolated ingame
  direction, impact, fade, sound, collision and pool-cleanup validation.

## Authoritative scope expansion: 14 Pegasus skills plus Wind Soul - 2026-09-10

- Owner confirmed that the changelog scope is fourteen Pegasus skills, with
  standalone SS21 Wind Soul as item fifteen.  This supersedes the earlier
  twelve-row stopping boundary; it does not turn existing `IN_PROCESS` rows
  into completed skills.
- Expanded the shared group-15 catalog from items `15:102..113` to the complete
  contiguous `15:100..113` table.  The two added hash-pinned rows are
  `15:100 -> 288 Deathside` and `15:101 -> 289 Darkness`.
- Imported exact Pegasus `Skill.xml` fields for both Summoner skills. Deathside
  is damage skill 288 (damage 75, mana 120, AG 20, range 6, energy 930).
  Darkness is buff skill 289 (mana 100, AG 50, range 5, delay 5000, energy
  300); `BuffEffect.xml` identifies effect 228/group 132 and the Curse-damage
  plus defense role.
- Catalog and input classification tests now require 14 mappings. Darkness is
  classified as self-buff and Deathside as target attack; their dedicated
  action/effect/server semantics are still being recovered and are not yet
  runtime-ready.
- Recovered Deathside's model registration block `0x65FF7D..0x660105`: model
  roles `0x278B..0x278F` map to `Reaper`, `ReaperHead`, and three
  `ScytheEffAni` BMDs.  Decoded every embedded texture dependency and added
  all 12 files to hash-guarded isolated staging; staging PASSes 138 files.
- Fixed a separate existing 12-row dispatch defect found during this audit:
  Shining Bird entered through the ordinary target-skill request but its GS
  handler never echoed the accepted `0x19` cast.  It now sends one authoritative
  echo, allowing the client action/sound/`0x27C1` root to run.  A source-level
  verifier guards cast echo ownership for every currently implemented route.
- Focused 14-row catalog/input tests and the 12-runtime-route server verifier
  PASS. Full isolated client and GameServer builds exit 0. Client warnings are
  still only the existing Crypto++ LNK4099 class. SHA-256 values:
  - client: `4BF0B1BDDDF33F67147169E212AC4C3D999695F0EDF58B30DE78CDDCC781F0D3`
  - GameServer: `7827BD013800DFF1EF5555B4AC305DB0B570BE091201F9FC3EB0BAB2CFFEE8D0`
- Completion denominator is now **15 total: 14 Pegasus + Wind Soul**. Every
  entry remains `IN_PROCESS` until its full runtime gate is evidenced.
- Added explicit GameServer dispatch for both newly restored IDs. Darkness
  now enters the marker-only self-buff route and can no longer fall through to
  `BasicSkillAttack`; Deathside preserves native target/range/damage checks and
  emits its accepted `0x19` cast for the recovered client Reaper chain. The
  14-ID dispatch/cast-ownership verifier PASSes and the isolated GameServer
  build exits 0, SHA-256:
  `1DEC7872D4AE62018C6A3340DEB20A0F4C6EFD40AD7BB6A0BB2ED12B55233865`.
  This is server-route evidence only; Deathside/Darkness remain `IN_PROCESS`
  until their client chains and runtime QA are complete.
- Recovered the Darkness cast pose directly from Pegasus `0x65B148`: skill
  `0x121` selects player action `0x91` (145). The isolated client `0x19`
  receive route now selects that exact action when the loaded player BMD owns
  it, otherwise it uses the native 5.2 self-buff pose rather than indexing an
  absent action. No Darkness model root was invented: current exact xrefs
  identify it as a buff/action route, while the server-owned effect marker
  supplies the native buff state. Catalog/input tests PASS and the isolated
  client build exits 0 with only existing Crypto++ LNK4099 warnings. SHA-256:
  `BD1F0162BF92D079DC91A84077C7B5D0D22FAC92BC53B1AED832CC72BEE11468`.
  Darkness still needs owner ingame cast/icon/lifetime validation and the
  original stat formula before full acceptance.
- Added the first executable Deathside client chain without borrowing SS21
  layouts: authoritative `0x19` dispatch now creates/reuses a caster-owned
  Reaper root, using isolated model IDs `MAX_MODELS+47..51`. All five
  hash-pinned `Reaper`/`ReaperHead`/`ScytheEffAni` BMDs and their seven
  textures load only from the isolated recovered asset tree. The translated
  root contract preserves Pegasus height `+80`, follow factor `0.35`, idle
  action `7`, repeat-cast actions `{3,4,8}`, lifetime `999`, and texture-pass
  mesh set `{0,1,2,4}` with its `0.6..0.7` pulse.
- Focused Deathside model/action contract PASSes; staging remains PASS at 138
  files. Full isolated client build exits 0 with only the existing Crypto++
  LNK4099 warning class. SHA-256:
  `F3491AAC2A81CC72FCE227EA40088D6DDDD776D8E770DAFE934DB1E28B822415`.
  This establishes a testable Reaper root, not full parity: exact scythe child
  bone particles, sounds, attack-to-dismiss transition, and owner ingame QA
  remain open, so Deathside stays `IN_PROCESS`.
- Continued the Deathside child chain from exact blocks
  `0x66046E..0x66049E`, `0x660640..0x660682`,
  `0x65E81D..0x65E920`, and `0x65EF97..0x65F48F`. Repeat casts now create
  one root-owned `ScytheEffAni01/02/03` child matched to action `3/4/8`; each
  copies the root transform/light, uses lifetime 60 and scale 0.9, and emits
  `blur02_mono` trails from the recovered per-variant bone spans. Added the
  four exact texture dependencies (`ground_star`, `damage01mono`,
  `ground_smoke_mono`, `blur02_mono`) to hash-guarded staging. Staging PASSes
  142 files, Deathside contract PASSes, and the isolated client rebuild exits
  0 with only Crypto++ LNK4099 warnings. Current client SHA-256:
  `713D63CDD39D7BDE88A316A369C82F80989829D267C9FF9AF50F3647152A029E`.
  Root/child rendering is now testable; exact particle cadence, sounds and
  owner ingame comparison are still required before acceptance.
- Completed the next source-evidenced Deathside layer from
  `0x65F6EA..0x65FA1B` and `0x66046E..0x660573`. The root now emits the exact
  `rand()%34`, `rand()%13`, and `rand()%10` WATERFALL4/ground-smoke/upper-smoke
  groups with recovered subtype, spread, color, scale and remaining-life
  envelopes. Dismissal rendering also uses the recovered fixed velocity alpha
  instead of the normal idle pulse. Pegasus resource `0x7E22` was resolved through its registration
  string to native RISE `BITMAP_WATERFALL_4`; its numeric value was deliberately
  not copied because the two enum layouts differ.
- Added all five hash-pinned Reaper WAVs to the isolated sound catalog and
  staging. Repeat casts select `ReaperAttack1/2/3` with their matching
  `{3,4,8}` action; first creation plays `ReaperAppear`; after the exact
  10,000 ms renewed deadline, action 6 receives lifetime 20, velocity 0.5 and
  plays `Reaperdisappear`. The RISE sound tail is now 23 isolated buffers;
  original Pegasus buffer numbers remain provenance only.
- Asset staging PASSes 147 files; the 14-row/23-sound catalog test and expanded
  Deathside contract PASS. The isolated client build exits 0 with only the
  existing Crypto++ LNK4099 warning class. Current client and `Engine.exe`
  alias SHA-256: `166DB8EF3F8CD7760E441BBC1681B9D2454E09EBC2F7B9451875CFCC11E57EAD`.
  Deathside remains `IN_PROCESS`: owner ingame appearance, attack variants,
  particle envelopes, sound timing and ten-second dismissal still need runtime
  acceptance.

## Wind Soul bird color and smoke ownership correction - 2026-09-10

- Owner runtime QA confirmed that the three roots now appear and travel, but
  the `desair` bird children were black and the persistent smoke envelope was
  absent.  This is a runtime FAIL for visual parity, not an accepted build.
- Traced the black bird to native 5.2 `MODEL_DESAIR` rendering, which forced
  `BodyLight=(0,0,0)` for every subtype.  Wind Soul subtype 2 now copies its
  cyan/white effect light and renders with `RENDER_TEXTURE|RENDER_BRIGHT`;
  legacy `MODEL_DESAIR` subtypes retain their original renderer.
- Found the subtype-80 constructor in the wrong bitmap switch.  It is now in
  the actual `BITMAP_SMOKE` constructor with the SS21-confirmed lifetime 40,
  random scale/rotation and gravity ranges.  The existing subtype-80 update
  envelope remains unchanged.
- Added the decoded `desair.bmd` mesh texture `dark_skill01.OZJ` to the pinned
  isolated asset manifest.  Five Wind Soul files now stage with verified
  length and SHA-256.
- Wind Soul contract test and the strengthened cast-path/renderer verifier
  PASS.  Full isolated client build exits 0 with only the existing Crypto++
  LNK4099 missing-PDB warnings.  Client SHA-256:
  `4A770C9FA63864FB2012235B4D49839B2D8A904BBA30DF81C1559FD298AF4DDE`.
- Wind Soul remains `IN_PROCESS` pending owner ingame comparison of bird
  color/blend and smoke density against SS21.

## Wind Soul SS21 cast-motion correction - 2026-09-10

- Compared the owner-provided SS21 cast capture against the isolated 5.2
  capture.  SS21 shows three thick, curved cyan/white ghost trails plus a
  persistent cyan smoke envelope; the previous 5.2 result was shorter and
  visually sparse.
- Re-read the hash-pinned SS21 constructor/move blocks
  `0x15CBBD3..0x15CBE87` and `0x15EE884..0x15EEF2D`, including their referenced
  float constants.  The periodic smoke subtype is confirmed as `0x50` (80);
  `0x67` belongs to the preceding unrelated branch.  Exact joint light is
  `(0.4,0.6,0.7)` and smoke light is `(0.2,0.65,0.75)`.
- Corrected the native 5.2 adapter to use direct scheduled smoke allocation,
  direct impact particles, per-step velocity increments `+0.4/+0.1`, and the
  unscaled post-rotation direction add used by SS21.  The old adapter applied
  an extra FPS factor and an FPS-throttled particle wrapper, shortening the
  trails and dropping most of the smoke envelope.
- Extended the focused contract test with tail, lifetime, movement, pitch,
  impact, subtype and color assertions.  Contract test PASS; authoritative
  cast-path verifier PASS.  Isolated client build exits 0 with only the existing
  Crypto++ LNK4099 missing-PDB warnings.  Client SHA-256:
  `F40F728CBCE48EAD63E068DAAC11E72F3D17D7ABFBD0264885B4B328F235F710`.
- Wind Soul remains `IN_PROCESS` pending owner comparison of the corrected
  isolated runtime against SS21; compilation/static evidence is not visual
  acceptance.

## Isolated QA cast-gate correction - 2026-09-10

- Owner runtime report showed the newly provisioned skill could not cast. The
  command path added the skill slot correctly, but the standard receive paths
  still rejected it at `CheckSkillRequireClass` before `RunningSkill`; this is
  the same add-versus-use separation previously exposed by Sword Blow QA.
- Added a `PEGASUS_ISOLATED_TEST`-only packet gate for connected GM level 1.
  It recognizes only the twelve compiled Pegasus IDs plus standalone Wind Soul
  737 and bypasses only the class/change-up check. Normal GameServer builds are
  unchanged; mana, BP, delay, map, target, range and damage checks remain live.
- Applied the scoped gate to multi-target, ordinary target and duration packet
  receive paths so `/testpegasus` and `/testhighskills` do not silently add
  unusable QA slots. Rebuilt the isolated GameServer exit 0. SHA-256:
  `A3C6A74E36077A9FA5246680D32A981C8D0792E4B8E60317EFB7B0BC5D18CD95`.
- Runtime re-test is required; this build result is not cast acceptance.

## Wind Soul 737 and combined ingame QA provisioning - 2026-09-10

- Added Wind Soul as a standalone package under `WindSoul/`; it does not share
  Sword Blow dispatch or runtime state. Identity is skill 737, scroll 12:303,
  Dark Lord tier 3, level 400, strength 717, regular prerequisite 238 and
  master prerequisite 523 level 10. Related skills 1099/2054/2096 are excluded.
- Client catalog seeds mana 35, BP 17, damage 130, range 6 and the verified
  Dark Lord class gate. Local cast is send-only and emits no position/movement
  packet. The authoritative 0x19 response owns the native mount-aware action,
  `caotic.wav`, and exactly three `BITMAP_2LINE_GHOST` subtype-4 roots.
- Added the traced 35-tick homing root, 12 tails, target follow at Z+100,
  `(0.4,.6,.7)` light envelope, periodic subtype-80 smoke, contact smoke, and
  the subtype-2 native `desair` child. Target slots are carried directly and
  guarded; no second key-to-slot lookup is performed.
- GameServer seeds skill 737 independently of the Pegasus catalog, maps scroll
  12:303, enforces the verified learning gates, sends one accepted cast echo,
  and applies exactly three primary-target attacks. The native Dark Lord
  multiplier path supplies `Energy/25+200` percent. AOE remains unresolved and
  therefore fails closed to the selected target only.
- Added isolated GM level-1 QA commands: `/testwindsoul` adds skill 737,
  `/testpegasus` adds all twelve Pegasus group-15 skills, and
  `/testhighskills` adds all twelve plus Wind Soul. Existing skills are
  refreshed without consuming duplicate slots; the command reports added,
  existing and failed counts.
- Wind Soul contract test PASS; cast-path verifier PASSes the send-only/local
  and authoritative-root split. Four native-compatible assets stage into the
  isolated `PegasusBuild` tree with pinned hashes. Full isolated client and
  GameServer builds exit 0. Client warnings remain only the existing Crypto++
  LNK4099 missing-PDB class. SHA-256 values:
  - client: `6A9D398703C2353E2B7DA5620CC0B501F6C4F39A9FBEB7A6E8FC60C08AC9013E`
  - GameServer: `AFDE0752DFB66449B841D1D96C2E29500E179559D0D7A9FD272F89AC9F4A21D0`
- Wind Soul and every Pegasus row remain `IN_PROCESS`; build/test provisioning
  is not owner ingame acceptance.

## Crusher Charge renderer correction - 2026-09-10

- Re-decoded the exact Pegasus constructor/update range
  `0x65A46E..0x65A948`, renderer range `0x65A94B..0x65AF24`, and loader
  range `0x86E330..0x86E3E5`. Bitmap IDs `0x7F1D..0x7F1F` resolve to the
  hash-pinned `Steam_01.jpg`, `smokelines03.jpg`, and `flare_mono.jpg`.
- Corrected the old guessed renderer: subtype `0x27B0` is a bitmap-only steam
  shard; subtype-0 `0x27AF` draws exactly three terrain-alpha smokeline layers
  at sizes 4/3/2; the shared `0x27AE/0x27AF` model path draws mesh 0 exactly
  once with `RENDER_TEXTURE|RENDER_BRIGHT` and uses `Alpha` for both body alpha
  and blend transfer. The former all-mesh loop was not source-faithful.
- Corrected update envelopes: core/ring alpha starts at zero, ring angle is
  randomized and advances `10*dt`, subtype-5 shard Z advances `5*dt`, shard
  alpha uses the recovered grow/fade split at lifetime 17, and ring lifetime
  splits are 17/10. Subtype-0 also emits the recovered lifetime
  32/28/24/20 crosses and subtype-1 ring.
- Isolated staging PASSes 126 hash-checked files. Full isolated client build
  exits 0 with only the existing Crypto++ LNK4099 missing-PDB warning class.
  `Engine.PegasusImport.exe` SHA-256:
  `92CB27CE4908E396B572A7E565907FCBA5F4C9F3674E21F31DF9FBCC0F4EBD8C`.
- The loaded `flare_mono` belongs to the still-unrepresented persistent
  `0x27B1` family. That family, complete charge-state semantics, and owner
  runtime QA remain open, so Crusher Charge stays `IN_PROCESS`.

## Sword Blow SS21 material-color correction - 2026-09-10

- Owner comparison disproved the earlier conclusion that copying traced
  `(0.2,0.2,0.2)` into the RISE renderer was sufficient for color parity. The
  vector is the SS21 `CreateEffect` controller/object light; it is not the
  final framebuffer color after material submission.
- Rechecked the complete SS21 trace and the completed RISE Visual Studio
  compatibility renderer. SS21 submits root/small-sword mesh 0 exactly once as
  `RENDER_TEXTURE`, then mesh 1 as `RENDER_TEXTURE|RENDER_BRIGHT`. The isolated
  client incorrectly added a second white mesh-0 pass, directly explaining
  the rejected silver-white lanes.
- Removed that duplicate white pass. Mesh 0 now receives the traced unit blend
  light. Mesh 1 alone owns the long tail; the root uses `Alpha`, while each
  small sword uses the proven native-5.2 compatibility multiplier
  `Alpha*.65`. Neutral object light remains `(0.2,0.2,0.2)` for state parity
  and is no longer mistaken for the submitted material color.
- All eight focused tests PASS, including the new compile-time guard for the
  `.65` small-sword transfer and the 43-ID collision guard. Full isolated
  client build exits 0; only the existing Crypto++ LNK4099 missing-PDB warning
  class remains. `Engine.PegasusImport.exe` SHA-256:
  `0C9162AD153FD5D7985E109C2CEC4742928348BC54E2D9B4C4BABC7BBDECC365`.
- Color remains owner-runtime QA pending. Do not call Sword Blow 100% from the
  build; compare the revised tail against the supplied SS21 frame before
  closing visual parity.

NEXT EXACT ACTION: owner-test the two-pass small-sword renderer and compare its
blue/violet tail intensity against SS21; continue exact Crusher Charge overlay
recovery while that runtime check is pending.

## Authoritative target dispatch, damage contracts and Sword Wrath renderer - 2026-09-10

- Rebuilt the isolated GameServer after the Spearstorm main-target correction;
  build exits 0. Spearstorm now always includes the selected target, performs
  two guaranteed damage applications and preserves the source-documented 5%
  third application. Dragon Violent performs two hits only for skill 242;
  Shining Bird performs one hit for every valid target in its native 1.5x6
  directional area.
- Corrected authoritative target delivery for Raining Arrow and Chaos Blade.
  Both now use the server-produced F3:35 target list; Raining Arrow creates its
  target child once per returned target and Chaos Blade no longer duplicates
  impact layers inside the root while omitting secondary targets.
- Corrected high-skill buff lifetime contracts: Dex Booster is 300 seconds;
  Spiral, Crusher and Elemental Charge use native count 0 and therefore persist
  until death/logout. Stat formulas and third-cast damage remain open where no
  authoritative server evidence is available.
- Re-decoded Sword Wrath from the hash-guarded Pegasus binary. Layer subtypes
  0/1 use `mzine_irondefence`, subtype 2 uses `ground_star`, and subtype 3 uses
  `wave`; these are terrain alpha bitmaps, not BMD mesh texture overrides. The
  old generic BMD rendering path was therefore wrong and has been replaced.
- Restored exact layer colors `(1,.2,.19)` and `(1,.158,.119)`, exact root and
  layer pulse formulas, subtype-2 grow/fade split at lifetime 15, and subtype-3
  `.05` alpha loss plus `.2` scale growth. Added the source 30-particle
  `Swordeff_mono2line` radial burst with `(1,.15,.05)`, 20-tick lifetime,
  source `distance*.06` convergence and dynamic path-keyed bitmap allocation.
  No Pegasus numeric bitmap ID was copied into the RISE texture domain.
- Added five hash-pinned Sword Wrath assets. Staging PASSes 125 files. Added a
  focused Sword Wrath math/color/provenance test; all seven focused tests PASS,
  and the runtime-ID test still PASSes 43 collision-free fixed IDs.
- Isolated client and GameServer builds exit 0. Client warnings remain only the
  existing Crypto++ LNK4099 missing-PDB class. Client SHA-256:
  `047BDB1A3F14F50652F6DF8BAEA47CD09E6A84E098FB6346DDBB7872900322DF`;
  GameServer SHA-256:
  `C430A1638DF0A9672B78A67504A58974B92DF44A6E8B00D4DDF747CBE581DC3C`.
- Color audit boundary: Sword Blow has exact SS21 runtime-debug colors. For the
  other group-15 skills, current colors are exact Pegasus binary constants;
  the existing SS21 Visual Studio artifacts provide constructor/resource leads
  but do not yet contain equivalent complete runtime color traces. Do not call
  their SS21 color parity verified until those traces are captured.
- Added `PEGASUS_SS21_COLOR_AUDIT.md` as the per-skill evidence matrix. It also
  records the SS21 Chaos Blade candidate `(0.9,.8,1)` vector without applying
  it prematurely: the downstream provider argument/render semantics are still
  open, so changing the current Pegasus colors now would be another guess.

NEXT EXACT ACTION: runtime-check Sword Wrath's restored ground/line/smoke/star
chain and capture equivalent SS21 runtime color telemetry for each remaining
visual skill. All skills remain `IN_PROCESS`; compile/static evidence is not
owner ingame acceptance.

## Dragon-head renderer and Spearstorm falling-blade continuation - 2026-09-10

- Completed the previously open Dragon Violent `0x27C3` head renderer from
  `0x66377C/0x66304A`: native RISE object draw plus two `flare01` sprites on
  bones 3 and 4, local offset `(8,0,0)`, scale `.6`, and light
  `Alpha*(1,.12,.08)`. This replaces the incorrect generic all-mesh bright
  fallback for the head.
- Re-decoded Spearstorm `0x675C19..0x67626D`. The former adapter incorrectly
  made `0x27C8` a visible 40-tick rotating model. It is now the source-proven
  invisible five-tick orchestrator and emits a falling `0x27C9` blade at each
  crossed integer tick over random radius `100..639`.
- Ported the blade's exact 72-tick constructor, random pitch/yaw, `-200`
  direction vector, terrain/start-height calculation, two `sharke_arrows`
  trails, movement to ground contact, 66-tick impact transition, 64-tick hit
  sound and `.02331*dt` fade/cleanup. Ground contact creates the leading
  explosion/smoke particles and the source `StartZ-57` crack origin.
- Ported the `0x27CA` 50-tick crack alpha, periodic expanding smoke, native
  crater base pass, and the timed `damage01mono`/`!Combo3` overlay envelopes.
  Exact `sharke_arrows.OZJ` and `!Combo3.OZJ` are hash-pinned; staging PASSes
  120 files. Runtime-ID test PASSes 43 collision-free IDs.
- Added a focused Spearstorm vector/lifetime/layer test; all six focused
  catalog, ID, Sword Blow, Havoc, Elemental and Spearstorm tests PASS. Isolated
  client and GameServer builds exit 0; only Crypto++ LNK4099 appears on the
  client link. Latest client SHA-256:
  `716F8F028316664B2872CBD964D9EF2F19F5574D9A5AB588634F8DBB216836E1`.

NEXT EXACT ACTION: recover the remaining Spearstorm impact fan children and
prove its full lifetime/pool cleanup ingame. Spearstorm and Dragon Violent
remain `IN_PROCESS`; compilation is not visual/runtime acceptance.

## Havoc/Elemental exact-table validation - 2026-09-10

- Added a focused Havoc Spear pure-math test. It immediately rejected an
  incorrect `+10` inside the Nova rise ratio and a misleading closed-form
  shockwave scale helper. The runtime now follows the decoded Pegasus order:
  Nova starts below terrain and reaches `terrain+10` over five ticks; each
  shockwave update adds `initialScale*0.5*dt`. Havoc test now PASSes.
- Read the Elemental Charge renderer tables directly from hash-guarded
  `MU-x86.exe` (`02922A0C...DEA0AA`): bones `3,6,9,12,15`, pulse rates
  `.009,.008,.007,.006,.005`, and ordered bitmap roles `0x7F22..0x7F26`.
  Loader blocks map those roles to the five already hash-pinned blue, yellow,
  red, purple and green flare assets; foreign numeric texture IDs are not
  copied into RISE.
- Corrected the five model passes from `0x664C04..0x664C33`: each mesh uses
  the pulse for both alpha and blend light, with BlendMesh 0 and native
  bright-texture flags. The prior adapter incorrectly used fixed alpha 1 and
  BlendMesh -1. A focused Elemental bone/bitmap/pulse provenance test PASSes.
- Isolated client and GameServer builds exit 0. Client warnings remain only
  Crypto++ LNK4099. Latest client SHA-256 is
  `246BCF2516D355AC326CDB53A5E46753E67D081C51244C54C170C3211A420E61`;
  GameServer SHA-256 is
  `44C58337414FD6016F1A4D6E683EC5C955420D5BB7F3A86E073E6E12BAC1CBCA`.
- Pegasus client/config proves the semantic rule “every third Havoc Spear
  explodes a Nova”, but the available Pegasus package contains no GameServer
  binary/source proving its damage formula. The counter/damage path therefore
  remains open instead of receiving guessed values.

NEXT EXACT ACTION: continue recovering authoritative server evidence for the
third-cast Nova while completing the next bounded child/renderer chain. Havoc
Spear and Elemental Charge remain `IN_PROCESS`; builds are not runtime QA.

## Sword Blow V12 blend-tail and SS21 color correction - 2026-09-10

- Owner initially accepted Sword Blow as 100%, then supplied the missing
  visual detail: each flying small sword lacks the long blue blend tail shown
  in the Pegasus reference video. Therefore final acceptance is reopened;
  preserve the already accepted placement/direction while changing only the
  missing render passes.
- Exact Pegasus paths `0x679687` and `0x84AF17..0x84B0E4` prove that big/small
  sword models first draw the base mesh, then add a white blended mesh-0 pass
  and a bright `RENDER_TEXTURE|RENDER_BRIGHT` mesh-1 pass whose BodyLight is
  `effect.Light * effect.Alpha`. The adapter previously drew only the base
  mesh 0, explaining why the sword body appeared without its trailing blend.
- The reference MKV confirms the expected result is the long tapered blue
  streak attached behind every small sword; it is not a separate guessed
  texture or particle.
- The completed RISE Visual Studio guarded SS21 trace was then audited instead
  of estimating color from the video. Its 39 ordinary root casts and 936
  small-sword allocations prove model families `0x6A2` root, `0x6A3` blade
  and `0x6A4` plane all use neutral object light `(0.2,0.2,0.2)`. The current
  adapter was wrong for the blade `(0.35,0.35,1.2)` and plane
  `(0.25,0.35,0.55)`; both are now corrected. Blue-white output remains the
  responsibility of the recovered material/bright passes.
- The same trace confirms Plan/glint `(0.7,0.7,1.0)`, multishot 1/2
  `(0.4,0.5,1.2)`, multishot 3 `(0.35,0.4,0.48)`, target lightning
  `(0.3,0.6,1.0)` and flare `(0.65,0.7,0.9)`. These values are centralized
  and compile-time guarded by the focused Sword Blow test.
- Focused Sword Blow color/math/protocol test, runtime-ID test and complete
  12-row catalog/provenance test PASS. Full isolated client build exits 0;
  warnings remain only the existing Crypto++ LNK4099 missing-PDB class.
  `Engine.exe` SHA-256:
  `E6B097558E1B70ACBA22B5B4AD819532E1C1D62B8C5379C15E60D7C972DE3BAD`.
- Verified one-file owner-QA ZIP:
  `Delivery/SwordBlow_Client_BlendTail_SS21Color_v12_2026-09-10.zip`,
  SHA-256 `F9C09735BD2AFF8E1EDDE9E91CDB5C54CBF2DF2FB41F51CD28F712A5ED901318`.
  Its sole entry is `Engine.exe` (10,817,536 bytes). Superseded V10/V11 ZIPs,
  old staging folders and the temporary 177 MB video-frame extraction were
  moved to the Recycle Bin after verification.
- The detailed engineering gates (multi-target membership, target-loss/death,
  repeated-cast cleanup, lifetime and pool release) remain recorded as
  regression checks and must still be checked before a broad technical
  `VERIFIED` claim.

NEXT EXACT ACTION: build and owner-test V12's recovered mesh-1 blend tail plus
SS21-traced color correction without changing the accepted trajectory; resume
Havoc Spear afterward.

## Havoc Spear exact initializer/update/renderer continuation - 2026-09-10

- Re-decoded the complete Pegasus type switch at `0x665D50..0x66683D`.
  Replaced the generic all-mesh bright fallback with the exact per-model
  contract: Nova renders mesh 0/1 plus mesh-1 chrome; Magum line renders two
  mesh-0 passes with `(0.15,0.5,0.7)` then `(0.15,0.15,0.6)`; shockwave uses
  alpha-scaled object light plus its Havoc-only alpha-line pass; the end model
  uses the ground-crack override.
- Loader registrations `0x86E2BF..0x86E2F7` resolve source IDs `0x7F17` and
  `0x7F18` to `alpha_line2mono3` and `ground_crack_light02`. The adapter does
  not copy those numeric IDs: it stages hash-pinned assets and lets the native
  RISE path-keyed loader allocate them safely. Staging PASSes 118 files.
- Corrected the specialized initializer/update values from the same bounded
  blocks: Nova life/distance 20 with terrain-relative 234-height envelope;
  line life/distance 30; shockwave life 12, scale expansion
  `Distance*0.5*dt`, alpha `LifeTime/12` and Z rotation `6*dt`; end life 24
  with the recovered 75% fade window. The earlier generic shockwave lifetime
  20 was disproved by the exact constructor and removed.
- Full isolated client build exits 0 with only the existing Crypto++ LNK4099
  warning class. Latest continuation `Engine.exe` SHA-256:
  `A626835E494C69E33A6A7CC509FE801FE1DF237EE90773B1F9AB1CEEE03BBC1A`.

NEXT EXACT ACTION: add focused pure Havoc envelope/provenance tests, then
runtime-test multi-target cleanup and recover the GameServer third-Nova damage
counter. Havoc Spear remains `IN_PROCESS`; build evidence is not runtime
acceptance.

## Changelog scope discrepancy: 14 total skills - 2026-09-10

- The official Pegasus changelog lists 11 skills/items on 2026-08-15, then
  three more class skills on 2026-08-21: 14 total.
- The original table has two contiguous rows immediately before the existing
  12-row scope: item `15:100` -> skill `288` Deathside (Summoner damage) at
  file offset `0x76C660`, and item `15:101` -> skill `289` Darkness (Summoner
  buff) at `0x76C674`. Existing items `15:102..113` begin at `0x76C688`.
- Therefore the current catalog is complete only for the explicitly requested
  `15:102..113` scope, not for the complete 14-skill changelog set. Do not
  silently expand mappings or acceptance counts until the owner confirms
  whether `15:100..101` are added to this task.

## Sword Blow V11 owner direction PASS and reusable lessons - 2026-09-10

- Owner confirms V11 small-sword direction is correct. This accepts the
  recovered launch/destination, pitch, yaw and per-object movement chain.
- Prior owner evidence also accepts stationary casting from V7 and visibility
  of the caster/small-sword effect. Earlier reports established cast and
  server damage plus target-impact visibility.
- The complete V1..V11 failure analysis and the mandatory prevention workflow
  for remaining Pegasus skills and future SS21 behavioral imports are recorded
  in `PEGASUS_SKILL_IMPORT_LESSONS.md`.
- Sword Blow remains `IN_PROCESS`: this confirmation does not yet prove all
  multi-target membership, target reaction, repeated-cast cleanup,
  lifetime/pool release or final complete visual parity gates.

NEXT EXACT ACTION: apply the recorded prevention workflow to the remaining
skills; continue Havoc Spear's evidenced renderer/damage-counter work while
Sword Blow's still-open cleanup and multi-target gates await runtime evidence.

## Sword Blow small-sword pitch-role correction - 2026-09-10

- Rechecked the original `MU-x86.exe` instruction order rather than relying on
  the prior variable labels. At `0x677CA2..0x677D1C`, Pegasus transforms the
  common destination from local `(0,-1500,0)`. At `0x677D24..0x677DE1`, it
  then creates each launch position from `(random lateral,+600..+1000,0)` and
  adds random `-100..+250` only to that launch position's Z component.
- `0x677E19..0x677E84` calculates `(launchZ-destinationZ)/1500`, then stores
  `asin(...)` as angle X. The V10 interpretation had put random Z on the
  destination, which inverted this pitch. Corrected the adapter and focused
  test so random Z belongs to the launch and destination Z stays zero.
- Yaw remains the recovered native `CreateAngle(launchX,launchY,
  destinationX,destinationY)`. Movement remains the recovered per-object
  rotation of local `(0,-Velocity*frame,0)` at `0x679334`; no guessed 90/180
  degree model offset was added. The staged small-sword BMD is byte-identical
  to Pegasus (SHA-256
  `EB1E324E292102273AE4DCFD1126173CE47DC4527DD98ED808EC2D40A83500FB`).
- Focused Sword Blow math/protocol test PASS. Full isolated client build exits
  0 with only the existing Crypto++ LNK4099 warning class. `Engine.exe`
  SHA-256:
  `55EB2B42D6272D7A36E6D1C8906BC2C9E718E66F1DCE7ED01297E68AA7D24BEC`.
- Verified one-file QA ZIP:
  `Delivery/SwordBlow_Client_SmallSwordDirectionFix_v11_2026-09-10.zip`,
  SHA-256 `D9B8ACC078E93C3A4AC7D9D5F28E88A95D227608A9C3E1A09B37CDCCC5BED259`.
  Its sole entry is `Engine.exe` (10,817,536 bytes).

NEXT EXACT ACTION: owner runtime QA of small-sword pitch and convergence using
the corrected client. Sword Blow remains `IN_PROCESS`; static/build evidence
does not establish visual parity.

## Elemental Charge five-bone renderer continuation - 2026-09-10

- Hash-guarded decode of `0x664ABA..0x664C55` recovered all five model-bone
  layers: bones `3,6,9,12,15`, pulse rates `0.009,0.008,0.007,0.006,0.005`,
  and bitmap routes `0x7F22..0x7F26`.
- Loader block `0x86E403..0x86E497` resolves those routes to the dedicated
  ElementalCharge blue flare plus yellow, red, purple and green flares. Added
  five isolated IDs `32959..32963`, hash-guarded all five source assets, and
  staged them only under the isolated recovered-data path.
- The runtime renderer now calculates the recovered absolute sine pulse per
  layer, transforms the exact model bone, emits its colored scale-1 sprite,
  and renders the corresponding mesh with that pulse light. The existing
  owner bone-17 root tracking remains unchanged.
- Runtime-ID test PASSes 41 isolated IDs; staging PASSes 116 entries. Full
  isolated client build exits 0 with only existing Crypto++ LNK4099 warnings;
  `Engine.exe` SHA-256:
  `A6C97224A780257BF6A8278AEDBAD67C3C4ABA87C948AF1105D3D55EE8BF3729`.

NEXT EXACT ACTION: recover the server-side third-Havoc-Spear Nova damage
formula/counter before enabling it; the BuffEffect text alone is insufficient
to guess damage. Elemental Charge remains `IN_PROCESS` pending that formula,
buff cleanup and runtime QA.

## Havoc Spear target-list and recovered burst continuation - 2026-09-10

- Hash-guarded decode of `0x665D50..0x667268` corrected the six object roles:
  `0x27A5/0x27A6` are invisible orchestrators and are no longer rendered as
  generic BMDs; shockwave lifetime is 20, while both orchestrators live 15.
- Root tick 1 now creates the recovered Magum line, terrain lightning burst
  and scale-8 shockwave, then plays explosion `0x3D6`. Subsequent odd ticks
  create the recovered random `-350..349` XY Nova points, terrain height,
  scale `0.22..0.38`, lightning burst, shockwave and plan-crack layer.
- Generalized bounded `F3:35` target transport to skill 2013. GameServer uses
  the same native range/target/radio validation as the proven Dragon list;
  each live client target creates its own `0x27A6`, Nova and impact cascade,
  with Hit-1/Hit-2 selected by target-list ordinal exactly as `0x666840`.
- The invisible `0x27A6` follows its owner and emits exactly two native
  `lighting_mega01..03` particles per update from the recovered 80x80x120
  volume and scale `0.22..0.91`.
- Focused target-protocol test PASSes skills 1501, 242 and 2013; runtime-ID
  test PASSes 36 isolated IDs; catalog test PASSes all 12 rows/18 sounds;
  staging PASSes 111 files. Isolated client and Ex603 GameServer builds exit
  0. Client SHA-256 `27107E3F0FEE86F570070DF6D294565945D683075A76729F105299FE5E910075`;
  server SHA-256 `44C58337414FD6016F1A4D6E683EC5C955420D5BB7F3A86E073E6E12BAC1CBCA`.
  Client warnings remain only the existing Crypto++ LNK4099 class.
- Rebuild after connecting the final `0x27A6` updater also exits 0; updated
  client SHA-256 `F98BBE75454678846B2C4C66CAAFDA7F760298FC2C13AC9AABE5491B8C60F0D0`.

NEXT EXACT ACTION: recover its remaining specialized model fade/renderer passes and runtime-test
multi-target cleanup. Havoc Spear remains `IN_PROCESS`; build evidence is not
visual/runtime acceptance.

## Sword Blow per-child trajectory correction - 2026-09-10

- Re-decoded the complete child setup at `0x677CA2..0x677DE1`. Pegasus uses
  local start `(random lateral, 600..1000, 0)` and a separately randomized
  local endpoint `(0, -1500, -100..250)` for every small sword. The adapter
  had assigned the height randomization to the start and reused one flat
  endpoint, which changed both pitch and visible travel direction.
- Moved random Z to the per-child endpoint and locked the six recovered local
  geometry constants in the focused Sword Blow test. The earlier recovered
  `startZ-endZ` pitch sign and native yaw/movement path remain unchanged.
- Sword Blow math/protocol test PASS, runtime-ID test PASSes 36 IDs, asset
  staging PASSes 111 files, and the full isolated client build exits 0 with
  only the existing Crypto++ LNK4099 warning class. `Engine.exe` SHA-256:
  `4F8B3EAA5207A43BDEC675AB08EBF924DEA12B17B882BDFD674354A3415F7725`.
- Verified one-file QA ZIP:
  `Delivery/SwordBlow_Client_SmallSwordTrajectoryFix_v10_2026-09-10.zip`,
  SHA-256 `B46E0155F618E47D91CAEA0F1028D63B5695216C4AF31DF6597647026DBEC629`.
  Its sole entry is `Engine.exe` (10,812,928 bytes); superseded V9 was removed.

NEXT EXACT ACTION: owner QA of V10 small-sword origin, direction and endpoint.
Sword Blow remains `IN_PROCESS`; compilation is not visual acceptance.

## Dragon Violent recovered eight-point cascade and particles - 2026-09-10

- Recovered the exact `0x66279B/0x663475` spatial chain: eight radial points
  in repeated quadrants. Points 0..3 use radius `100..350` and immediately
  create elevated head plus terrain crack; points 4..7 use radius `150..350`
  and create subtype `0..3` delayed orchestrators.
- The four delayed `0x27C2` objects now fire head/crack pairs after recovered
  delays `6..9` and self-terminate. Head height is terrain `+80`, the live head
  follows its owner at `+30`, and crack/wind/layer lifetime, alpha, rotation,
  scale and child cadence follow `0x662877..0x662FF8`.
- Corrected renderer dispatch scope: `0x27C2` and `0x27C7` are orchestration-only
  and no longer render the shared fallback BMD.
- Recovered loader routes at `0x86E807..0x86E8C2` for exact resources
  `flare01`, `lighting_mega03`, `smoke01`, `Impack03`, `Combo4`,
  `twlighthik01`, `twlighthik02` and `ring_of_gradation`. Added isolated IDs
  in the native effect tail and exact `0x7F41..0x7F45` particle
  initializer/update behavior. The crack now uses its recovered two-layer
  terrain renderer rather than the generic BMD fallback.
- Runtime-ID test PASSes 36 isolated IDs; hash staging PASSes 111 files.
  Catalog/provenance and Sword Blow focused tests PASS. Full isolated client
  build PASS with only Crypto++ LNK4099 warnings. `Engine.exe` SHA-256:
  `9B43101B4AD3639E9B965F8CE116FE4B732619A48B14F2A20824E7EAC02993C3`.
- Recovered and connected the primary-target `0x663C14` path: target-owned
  `0x27C7` at height `+30`, immediate `0x7F45` combo layer at `+100`, and the
  exact integer-tick `0x7F44` impact/occasional `0x7F43` smoke emission.
  The recovered list can address up to ten targets; the current RISE `0x19`
  path supplies only the authoritative primary target, so multi-target
  propagation remains open rather than being inferred.
- Replaced the shared renderer fallback for `0x27C5/0x27C6` with their
  recovered specialized passes. Wind renders mesh 0 with the exact six-times
  BodyLight multiplier. Layer subtype 2 uses fixed scale `1.2` and
  `twlighthik01`; the other subtypes use their growing scale and
  `twlighthik02`, all through the recovered `0.5681818128` model factor and
  subtype light selection. Latest isolated `Engine.exe` SHA-256 after these
  additions: `0A4F24EF5C48F9662033A722A12F88A3AEF47C3525ED4E4701D2C877BBB9405D`.

NEXT EXACT ACTION: recover/port the remaining specialized `0x27C3` head
renderer/bone layers and generalize the validated target-list transport for `0x27C7`.
Dragon Violent stays
`IN_PROCESS`; build/static evidence is not runtime acceptance.

CONTINUATION UPDATE — Dragon Violent multi-target transport:

- Generalized the isolated bounded `F3:35` payload without changing its wire
  layout: it still carries 16-bit skill/caster/count and at most ten 16-bit
  target keys. The focused protocol test now proves both skill `1501` and
  Dragon Violent skill `242` decode through the same bounded contract.
- GameServer skill `242` now uses native `CheckSkillRange`, `CheckSkillTarget`
  and configured `CheckSkillRadio`, damages the validated primary plus up to
  nine validated radio targets, and sends exactly that list. The client emits
  recovered `0x27C7/0x7F45` target effects once per live visible list member;
  the earlier primary-only shortcut was removed to prevent duplication.
- Isolated client and Ex603 GameServer builds both exit 0. Client SHA-256:
  `F23EC52AD108890364E6CC1D52FDB4923DA1BD1D25933FB9872CA4689A8B2267`;
  GameServer SHA-256:
  `124A4791D6EAE5BECD661BC330B58F6A1AFDDFB97301919EB7971B770B4483FB`.
  Client warnings remain only Crypto++ LNK4099. This is not runtime PASS.

## Sword Blow small-sword direction correction - 2026-09-10

- Owner confirms the small swords now render but their travel direction still
  differs from Pegasus.
- Exact decode at `0x677DE6..0x677E84` shows Pegasus computes child pitch from
  `(worldStartZ - worldEndZ) / 1500` before clamp/asin. The RISE adapter had
  the operands reversed, mirroring the vertical pitch while its yaw still
  correctly used `CreateAngle(worldStart -> worldEnd)`.
- Corrected the pitch subtraction only. Origin, common endpoint, random spread,
  per-child scale, velocity and yaw remain on their recovered paths.
- Sword Blow focused math/protocol test PASS and full isolated client build
  PASS; only the existing Crypto++ LNK4099 warning class was emitted.
  `Engine.exe` SHA-256:
  `E67B10766DA96CB1C72DA9760BDC0CF8993B5126F36A16C5653FC1F9A7713C00`.
- Verified one-file QA ZIP:
  `Delivery/SwordBlow_Client_SmallSwordDirectionFix_v9_2026-09-10.zip`,
  SHA-256 `A22324E1A0D8A4F92BEBD13BD3BDEF0BAAEC228A6E08A4EBF4D439906E3591A5`.
  Its sole entry is `Engine.exe` (10,810,880 bytes); superseded V8 was removed.

NEXT EXACT ACTION: owner QA of small-sword origin plus direction with V9.
Sword Blow remains `IN_PROCESS` until that runtime check and the remaining
full-chain acceptance evidence pass.

## Sword Blow owner QA: stationary cast PASS, small-sword origin correction - 2026-09-10

- Owner confirms the V7 client no longer jumps/repositions the caster and the
  Sword Blow effect is visible. This is targeted runtime acceptance for the
  stationary cast and visible root only, not full visual parity.
- Owner reports the small swords render far behind the target. Re-decoding the
  pinned Pegasus child helper at `0x677C13..0x677EB6` found the exact sign
  error in the RISE adapter: Pegasus constructs each child start with local Y
  `+RandomRange(600,1000)`, while the adapter used the negative of that value.
  The common endpoint remains recovered local `(0,-1500,0)`.
- Corrected only that child-start sign. This moves the small swords to their
  recovered launch side so their native per-child angle/movement crosses
  toward the endpoint instead of spawning beyond it.
- Catalog/provenance, runtime-ID and Sword Blow math/protocol tests PASS;
  hash-pinned staging PASSes 107 files. Full isolated client build PASS with
  only the pre-existing Crypto++ LNK4099 warning class. `Engine.exe` SHA-256:
  `02E5805C6B2F8A8F2C9B7FA14D6C0DEB99705EACF0BF8C432E36C7C2A92467EC`.
- One-file QA ZIP (only `Engine.exe`):
  `Delivery/SwordBlow_Client_SmallSwordOriginFix_v8_2026-09-10.zip`, SHA-256
  `1A25ACB6460341856483EAACED17ED45A7BCCEA0CDDFCCCF45FA6E58821772DB`.
  Superseded V7 ZIP was removed after V8 was verified.

NEXT EXACT ACTION: owner-check the V8 small-sword stream position. Sword Blow
remains `IN_PROCESS`; the child position correction is not accepted until
ingame QA.

## Chaos Blade recovered root/arc/crack cascade - 2026-09-10

- Completed the bounded `0x6543B5..0x655221` effect slice against the pinned
  Pegasus x86 hash. Root `0x27BE` now emits frame 0 during initialization and
  frames 1..9 as its timer crosses integer boundaries.
- Each root frame uses the recovered ten-entry radius table
  `0,320,355,560...`, 11 samples, heading/random-angle formula, smoke-line and
  smoke-line03 particles, plus the animated model bone pairs 7->11 and 6->10
  for the black/long blade joints and exact subtype combination.
- Arc `0x27BF` now emits its recovered timer-gated shockwave pulses and the
  six-particle smoke burst after lifetime 5. Crack `0x27C0` emits the native
  `0x7DE0` subtype-19 five-joint burst at initialization and timer crossings
  1..5. Owner death still terminates all three effect objects.
- Added the exact recovered lifetime, rotation, scale, position drift and RGB
  fade behavior for Chaos Blade resources `0x7F33`, `0x7F35..0x7F3A`, routed
  through their isolated bitmap IDs rather than raw Pegasus texture IDs.
- Catalog/provenance, 28-ID isolation and Sword Blow math/protocol tests PASS;
  hash-pinned asset staging PASSes 107 files. Full isolated client build PASS
  with only Crypto++ LNK4099 warnings. After adding the native RISE static
  ribbon adapter for the recovered blade bone pairs, `Engine.exe` SHA-256:
  `D1DF84E4370D7367C9828DD77D5D850ADFCC3F81CFA375F82A5372DC49C8D8DC`.

NEXT EXACT ACTION: trace and implement Chaos Blade hit-2/explosion sound
cadence and runtime-check root/arc/crack cleanup. Chaos Blade remains
`IN_PROCESS`; build/static recovery is not ingame acceptance.

CONTINUATION UPDATE — Chaos Blade sound xrefs:

- Hash-guarded disassembly of `0x654000..0x655300` proves cast buffer `0x3DA`
  at the skill handler and Hit-1 buffer `0x3DB` exactly once at `0x654858`,
  after the five immediate target layers are created. The isolated adapter now
  plays `kChaosBladeHit1Sound` at that same boundary.
- No executable call-site for registered buffers `0x3DC` (Hit-2) or `0x3DD`
  (Explosion) was found in this recovered slice; both appear at the loader
  registration block. They remain loaded but are not played speculatively.
- Full isolated client build exits 0 with only Crypto++ LNK4099 warnings.
  `Engine.exe` SHA-256:
  `382948BA40A5E502DDB5418F31E82186D50E2E09CE7540EDBC45563F8926FAD6`.

NEXT EXACT ACTION: locate any indirect runtime references to `0x3DC/0x3DD`
before deciding whether those sounds are active, then runtime-check cleanup.

## Sword Blow server-position jump root cause and V7 - 2026-09-10

- Owner confirms V6 makes the Sword Blow effect visible, validating the
  debugger-derived normal render-pass correction.
- The remaining jump was not `PathFinding2`: after `SendRequestMagic`, native
  `UseSkillWarrior` still allowed skill 1501 into its final `SendPosition`
  block. That packet requests a position beside the target even while
  `c->Movement` is false, explaining the apparently contradictory debugger
  capture and the server-side snap/lunge.
- Skill 1501 is now excluded from `SendPosition`, alongside the native
  one-to-one/death-stab family. It still faces the target, plays Pegasus action
  71, sends the damage skill request and creates the recovered effects.
- Catalog/provenance, 28-ID isolation and Sword Blow math/protocol tests PASS.
  Full isolated client build PASS / exit 0; only Crypto++ LNK4099 missing-PDB
  warnings remain. `Engine.exe` SHA-256:
  `3DAA0E0FE70A405323EF648450912E7A4B422B5839BDEC07F121AA8C774F8202`.
- Verified one-file QA ZIP:
  `Delivery/SwordBlow_Client_NoServerReposition_v7_2026-09-10.zip`, SHA-256
  `896F8D14619BB4814776736A1B3C059BD93A7D758B5147E2E6FE16C832FF553A`.
  Superseded V6 ZIP was removed.

NEXT EXACT ACTION: owner tests V7 at in-range targets and confirms the caster
tile remains fixed while effects and damage still fire. Sword Blow remains
`IN_PROCESS` until owner runtime PASS.

## Sword Blow live-debug renderer root cause and V6 - 2026-09-10

- Attached the matching isolated PDB to the exact V5 `Engine.exe` running in
  the test client. Runtime proved skill 1501 reaches `CastWarriorSkill`, sends
  `0x19`, enters `ReceiveMagic`, creates the caster root and children, and
  successfully loads model 10119 plus all four dedicated Sword Blow bitmaps.
- For an in-range cast, `c->Movement` remained false when the request was sent;
  the visible lunge is therefore not evidence of coordinate pathfinding in
  that captured cast. Action 71 is retained because the pinned Pegasus caster
  chain explicitly requests action `0x47`.
- The actual missing-sword cause was captured in `RenderEffects`: the live,
  visible root reached the renderer with a valid lifetime and transform, but
  the Pegasus renderer was gated on `bRenderBlendMesh == true`. Normal world
  rendering calls `RenderEffects(false)`; the true pass is invoked only inside
  the water-terrain block. All twelve Pegasus custom model renderers now run
  in the normal pass and skip the water-only duplicate pass.
- Catalog/provenance, 28-ID isolation and Sword Blow math/protocol tests PASS.
  Full isolated client build PASS / exit 0; only the existing Crypto++ LNK4099
  missing-PDB warning class remains.
- V6 `Engine.exe` is 10,805,248 bytes, SHA-256
  `003BB85C9AFDC3BDF7F5576E7B0FC879977304D66AF8B28C68E5E8E1EEF3B0DD`.
  Verified one-file QA ZIP:
  `Delivery/SwordBlow_Client_RenderPassFix_v6_2026-09-10.zip`, SHA-256
  `CF7BC62F0F7ABACD139B627C72755735E4C1DDD3C009E8AFE9A025C07771F63D`.
  Its only entry is `Engine.exe`; superseded V4/V5 ZIPs were removed.

NEXT EXACT ACTION: package V6 and owner-test Sword Blow in Elbeland. Confirm
the large/small sword stream separately from character world-position motion.
Sword Blow remains `IN_PROCESS`; debugger/build evidence is not visual parity.

## Sword Blow V4 owner FAIL and cast/root correction V5 - 2026-09-10

- Owner ingame QA reports V4 is still visually/behaviorally unchanged. V4 is
  therefore a runtime FAIL and is not accepted.
- The remaining chase path was identified: when the target was outside the
  five-tile range, the special `CastWarriorSkill` branch returned false and
  input fell through to native `AttackKnight`, which starts melee pathfinding.
  Sword Blow now stops movement and consumes the input in both in-range and
  out-of-range cases; it sends the cast only when the exact range check passes.
- The caster sword root had also been attached to the custom `F3:35` target
  list. Target impacts proved that packet was received, but its independent
  caster lookup/timing could still suppress the root. The root now starts in
  the normal `0x19` skill-1501 receive branch that already drives the cast
  action. `F3:35` is restricted to its multi-target impact responsibility, so
  it cannot suppress or duplicate the caster root.
- Catalog, 28-ID isolation and Sword Blow math/protocol tests PASS. Full
  isolated client build PASS / exit 0; only Crypto++ LNK4099 warnings remain.
  `Engine.exe` SHA-256:
  `10778A38C819282832061FBF090B9B091CB953C68176710E39C5F90B9A9F142A`.
- Verified one-file QA ZIP:
  `Delivery/SwordBlow_Client_CastRootFix_v5_2026-09-10.zip`, SHA-256
  `0DC4F61F1E334582A1213389703D1318BFE7654AB67963FDD15839B676704B8E`.
  Its only entry is `Engine.exe`.

NEXT EXACT ACTION: owner replaces only `Engine.exe`, fully closes the old
client process, relaunches and retests Sword Blow both inside and outside five
tiles. Sword Blow remains `IN_PROCESS` until stationary cast and the recovered
large/small sword stream are visible in owner runtime QA.

## Chaos Blade exact impact/renderer recovery - 2026-09-10

- Recovered the pinned Pegasus Chaos Blade handler/root/initializer/update,
  renderer and impact helper at `0x654DC3`, `0x6543B5`, `0x654481`,
  `0x6545A1` and `0x654702`; these layers are taken from the isolated
  Pegasus binary evidence rather than substituted from SS21.
- Added dedicated bitmap IDs `32965..32974`, all below the dynamic custom-item
  texture stream. The focused runtime-ID test PASSes with 28 unique Pegasus
  IDs. Hash-pinned staging PASSes with 107 files.
- The immediate target impact now anchors at target bone 17 and creates the
  exact recovered five layers: blue flare, pin star, subtype-2 arc, steam and
  subtype-19 ground crack, with recovered colors/scales/owner and skill 2012
  attribution.
- Root/arc/crack initialization now uses the recovered lifetimes, velocity,
  timer and scale values. The renderer correctly treats root and arc as
  orchestration-only and renders only the crack mesh with the recovered
  `Magic_Ground2` bitmap override and bright textured state.
- Catalog, runtime-ID and Sword Blow math/protocol tests PASS. Full isolated
  client build PASS / exit 0; only the existing Crypto++ LNK4099 warning class
  was emitted. `Engine.PegasusImport.exe` is 10,805,248 bytes, SHA-256:
  `5581BC0AAB42B0A9F716D22AC2B65D41E6DDF151BF3C67466DB6AD835C7BFCC2`.

NEXT EXACT ACTION: finish the recovered Chaos Blade root frame cascade and
secondary particle lifetime/ownership path, then validate it ingame. Chaos
Blade remains `IN_PROCESS`; this build is not runtime or visual acceptance.

## Sword Blow owner-video QA failure and correction - 2026-09-10

- Side-by-side frame extraction from owner-provided `Goc.mp4` and
  `Pegasus.mp4` confirms the V3 runtime still fails parity: RISE walks the
  caster into the monster group and shows target impact layers, while Pegasus
  keeps the cast position and sends the large/small sword stream across the
  targets. This is an owner runtime FAIL, not acceptance.
- Root cause of the displacement is the provisional client dispatch: skill
  1501 had been inserted into the native warrior pathfinding/chase group.
  Sword Blow now casts only when its exact catalog distance check passes and
  never starts that melee chase path.
- The missing caster root also had an unsupported viewport-only visibility
  gate. The decoded `F3:35` caster key now resolves the authoritative local
  `Hero` first, falls back to the normal character table for remote casters,
  and requires only a live owner before creating the recovered large-sword
  root. The action-71 BMD remains unchanged because its payload is already
  byte-identical to Pegasus.
- Catalog, runtime-ID and Sword Blow math/protocol tests PASS. Full isolated
  client build PASS / exit 0; only the existing Crypto++ LNK4099 warning class
  was emitted. `Engine.exe` SHA-256:
  `26EEA433C7305C869977D5C94699EDD6C00CD02447144FD3B6E9A5DC2BC2D52E`.
- Verified client-only QA ZIP:
  `Delivery/SwordBlow_Client_StationarySwordFix_v4_2026-09-10.zip`, SHA-256
  `11C8D8BCD55051AC72409262FB795487BE8657099C439D0BE3123FA060403173`.
  It contains only `Engine.exe`.

NEXT EXACT ACTION: owner replaces only `Engine.exe`, keeps the existing staged
Pegasus Data and V3 isolated GS, relogs, then retests within Sword Blow's exact
five-tile range.
Sword Blow remains `IN_PROCESS` until the stationary cast and sword stream are
visible and accepted.

## Shining Bird secondary renderer recovery - 2026-09-10

- Recovered the pinned Pegasus renderer at `0x67380B..0x673A05` and its
  frame callback at `0x6726D7..0x67288F`; no visual layer in this slice is a
  guessed native substitute.
- The renderer now emits four `BITMAP_LIGHT` sprites at local X offsets
  `0/22/44/66` on each of the exact 12 bones
  `21,50,36,43,46,47,42,57,60,63,56,70`, with scales
  `4.5/3.375/2.25/1.125` and light `(0.1,0.25,0.4)`.
- The callback runs only when `int(AnimationFrame*1000)` changes and after
  frame `2.4`. It emits the recovered smoke/pin-star/wind mix, probabilities,
  colors, scales and randomized bone/angle selection.
- Added dedicated bitmap IDs `32975..32977` for exact recovered
  `wind01`, `smoke01` and `pin_star03`; runtime ID test PASSes with 18 unique
  IDs below the dynamic custom-item texture stream. Hash staging PASSes for
  101 files.
- Full isolated client build PASS. `Engine.exe` SHA-256:
  `420275BD96177AA731D2550436088C0A6BD234B94D9304CC18752348DAC0762A`.
  The authoritative MSBuild TLog records `Unsuccessful=False` and a valid
  `lastbuildstate`; this is build evidence only.

NEXT EXACT ACTION: recover and implement Shining Bird target reaction/collision
and validate the full cast/root/children/cleanup chain ingame. Shining Bird
remains `IN_PROCESS`.

## Sword Blow caster/sword-layer runtime correction - 2026-09-10

- Owner runtime QA after the texture collision fix proves target impact layers
  render, but the caster-side large sword/light layer is absent. The observed
  attack also looks like a lunge/stab and is not owner-accepted as Pegasus
  parity.
- Corrected a missing renderer dependency: the recovered Pegasus bone-light
  helper uses its own `lightmarks` and `flare01` bitmaps. The port had substituted
  native bitmap IDs. Both assets now load under dedicated IDs `32978..32979`,
  outside the custom/nonamed texture stream, and the 15-ID uniqueness/range
  invariant test PASSes.
- The caster root is now created from the decoded Sword Blow `F3:35` packet's
  caster key, alongside its target layers. This removes reliance on a separate
  generic magic-render path that owner QA showed was not producing a visible
  caster effect.
- Action provenance was rechecked rather than guessed: Pegasus `player.bmd`
  action 71 and isolated RISE `player.bmd` action 71 have identical seven-key
  animation payload SHA-256
  `104B27A38CB0C5D1D8BA684A8C92392198FC41BA94D998365F56C2A982A77D2D`.
  Pegasus caster code passes action `0x47` with blending enabled, matching the
  native RISE setter. Therefore the action is not replaced without stronger
  runtime evidence; the owner's lunge/stab observation remains an open visual
  FAIL to retest with the restored sword layer visible.
- Catalog and Sword Blow math/protocol tests PASS; 98-file hash staging PASS;
  full isolated client build PASS / exit 0. Executable SHA-256:
  `1CC0558688EB8A0DB5EB4CF0893BCC306E2E318773DE063F987D0EA1FD0770B1`.
- Verified client-only QA ZIP:
  `Delivery/SwordBlow_Client_CasterSwordFix_v3_2026-09-10.zip`, SHA-256
  `0BE075E79E7C17DEBB2DBEE2BE111E3F3D8AFF150194F86E0C27BDF48F47B1BF`.
  It contains only `Engine.exe`; V2 was moved to Recycle Bin.

NEXT EXACT ACTION: owner replaces only `Engine.exe`, keeps the existing staged
Pegasus `Data` assets and isolated V3 GS, relogs and casts Sword Blow. Capture a
short clip showing the caster, sword layer, travel and target impact. Sword Blow
remains `IN_PROCESS`; compilation and BMD equality are not owner visual PASS.

## Sword Blow custom texture-ID collision correction - 2026-09-10

- Owner runtime QA now proves cast and server damage execute, but the rendered
  result is visually invalid: repeated foreign/custom-item-looking texture
  sheets cover the screen. This is a runtime visual FAIL, not parity.
- Confirmed the cause in source: provisional fixed bitmap IDs `34900..34902`
  sat inside RISE's `BITMAP_NONAMED_TEXTURES_BEGIN` stream (`>=33001`), which
  dynamically supplies custom-item/model textures. Lazy skill loading could
  therefore reuse or replace an already assigned custom texture.
- Moved Sword Blow smoke/Spark03 and Shining Bird force-pillar IDs to the unused
  native effect-texture tail `32990..32992`, below the dynamic stream and below
  `BITMAP_EFFECT_TEXTURE_END`. Added `PegasusTextureIds.h` and a focused
  `/W4 /WX` runtime-ID boundary test; it PASSes and locks uniqueness plus all
  three range invariants.
- Catalog test, Sword Blow math/protocol test and 98-file hash staging PASS.
  Full isolated client build PASS / exit 0; only the existing Crypto++ LNK4099
  warning class remains. Corrected executable SHA-256:
  `9BEC2057C6EC908F20B1CE1ED22F7BB2FD92D0499E9C50859633A4A431F5C370`.
- Verified replacement package:
  `Delivery/SwordBlow_Client_TextureFix_v2_2026-09-10.zip`, SHA-256
  `DD645F9B9854824C089C835B09C326FC9BB32972A9A92EAACBBDB7014BDF9ED3`.
  It contains only `Engine.exe` and guidance. The superseded cast-fix ZIP,
  package staging and generated build output were moved to Recycle Bin.

NEXT EXACT ACTION: owner replaces only `Engine.exe` with texture-fix V2, keeps
the existing asset Data and V3 isolated GS, relogs, runs `/testswordblow`, and
captures a new cast screenshot. Sword Blow remains `IN_PROCESS` until the
foreign-texture issue is proven gone and the full visual/lifetime gate passes.

## Sword Blow local-cast dispatch correction - 2026-09-10

- Owner runtime QA proves the isolated GM command and client skill-list update
  work: skill 1501 is visible/selectable, but clicking a monster did not cast.
- Root cause was client-side: `SkillWarrior` and the movement continuation
  switch did not classify high skill 1501 as a targeted warrior skill, while
  `UseSkillWarrior` would also derive an invalid animation from the high ID.
- Added Sword Blow to the initial target/cast classifier, native sword weapon
  guard, mounted input allowlists, movement continuation, and an explicit
  `PLAYER_ATTACK_ONETOONE` action. The existing WORD `SendRequestMagic`
  contract remains unchanged.
- Catalog test PASS (12 mappings), Sword Blow math/protocol test PASS, asset
  staging PASS (98 files), and full isolated client build PASS / exit 0.
  Binary SHA-256:
  `709B7A18D92513E5CD665A4CADCB849C2D8BC3F0B51A45B7662892D72732004F`.
  Link warnings remain only the existing Crypto++ LNK4099 missing-PDB class.
- Created a client-only delta QA package containing `Engine.exe` and deployment
  guidance: `Delivery/SwordBlow_Client_CastFix_2026-09-10.zip`, SHA-256
  `F11B31D2617A839037542104CD4335E4E124CB180A14DAAA9A67E35B10F3FD4F`.
  Archive entries and embedded executable hash were verified. Generated build
  output and package staging were then moved to the Windows Recycle Bin.
- Sword Blow remains `IN_PROCESS`: this build evidence does not replace owner
  ingame validation of cast, damage, target layers, visuals and cleanup.

NEXT EXACT ACTION: deploy the corrected isolated client executable over the
previous Sword Blow QA client, keep the already deployed V3 isolated GS, relog,
run `/testswordblow`, target a monster and verify cast/action, damage and visual
cleanup.

## Runtime QA correction and Shining Bird recovery - 2026-09-10

- Per owner request, both generated QA ZIPs were moved to the Windows Recycle
  Bin after verification to keep the isolated worktree light. `Delivery` is
  currently empty; source changes and recorded SHA-256 evidence remain.
- Owner runtime QA proved `/testswordblow` was rejected for a connected GM.
  The cause is native `CheckGameMasterLevel`: it requires an exact
  account/character/IP row, while character login independently marks a GM by
  `Authority` bit 32 and appends an in-memory level-0 row. The isolated QA
  commands now accept either the authoritative connected-GM bit or a matching
  configured level-1 row. Normal GameServer builds remain unaffected because
  the commands are still guarded by `PEGASUS_ISOLATED_TEST`.
- Isolated Ex603 GameServer rebuild PASS / exit 0. V3 delivered binary SHA-256:
  `C94CCFFBC48AA1E6C68CFFA1E1FB476C099FAEBA2E959ABFA58C8C8560CB85C3`.
  Verified ZIP: `Delivery/SwordBlow_GS_Patch_2026-09-10-v3.zip`, SHA-256
  `9DAF22B408A9E4917257F240731DF342E02FC88B17D2E6C1A5D8166918765A70`.
- Hash-pinned Pegasus PE evidence at `0x6735E8..0x67377E` corrected the
  Shining Bird root: it advances 250 units along cast heading, starts alpha 1
  with blend mesh -1/light 0, and creates the three proven `0x7F3D`
  force-pillar children at scales 200/100/100. The two subtype-1 pillars keep
  recovered variants -1/+1 and follow the bone-1 anchor at radius 130 and
  height +60 through the `0x673A08` contract.
- Added custom bitmap 34902 for exact `Effect/force_Pillar2.OZJ`, SHA-256
  `5525FCDC5293C9F6088DF4C9EC1943185D772C0F33CB95575F434772D688DA66`.
  Staging PASS with 98 files; catalog and Sword Blow protocol/math tests PASS.
  Full isolated client rebuild PASS / exit 0, SHA-256
  `41469F64F46C4431525B3C0E62052E51E785A0A473EC43AC1F72D80D590FBBCB`;
  warnings remain only Crypto++ LNK4099. Shining Bird stays `IN_PROCESS`
  because its remaining bone-sprite/secondary-particle renderer, collision,
  target reaction and ingame validation are open.

NEXT EXACT ACTION: owner replaces the rejected V2 GameServer with verified V3,
restarts the GS, relogs the GM, runs `/testswordblow`, and reports whether the
skill is added. If added, capture cast/action, visible root/children, target
layers, damage/multi-target and cleanup. Compilation is not visual PASS.

## Owner QA delivery - Sword Blow - 2026-09-10

- Added isolated-only, GameMaster-level-1 commands `/testswordblow` and
  `/testswordblowoff`. The first
  force-adds client skill 1501 to a free native skill-list slot and sends the
  normal `F3:11` skill-list update, avoiding an unsupported guess at the still
  absent group-15 item definition. The command is compiled only when
  `PEGASUS_ISOLATED_TEST` is supplied by the isolated GS build script; normal
  GameServer builds do not contain them. The second removes 1501 and sends the
  native skill-delete packet so the owner can relog cleanly before restoring
  an older GS that does not support high skill IDs.
- Isolated Ex603 GameServer rebuild: **PASS / exit 0**. Delivered binary SHA-256:
  `E54B986CA2523FDA6391BFF68926866BA543A5BAAF390F7E084399ACE23E7020`.
- Verified delivery:
  `Delivery/SwordBlow_GS_Patch_2026-09-10-v2.zip`, ZIP SHA-256
  `DDA4B414C0344B649DFBB5BBF75676419FAE3635CB5E355DE7B5A92C58E52558`.
  It contains only `GameServer.exe`, Vietnamese deployment/rollback guidance,
  the binary checksum, and mandatory pre-rollback cleanup guidance. The
  superseded V1 ZIP was moved to the Recycle Bin.
- Removed 234 MB of generated GameServer Obj/output plus verification staging
  to the Recycle Bin after validating the ZIP. Source changes and the compact
  delivery ZIP remain.
- This enables owner ingame testing but is not itself runtime acceptance.
  Sword Blow stays `IN_PROCESS` until the owner reports cast/action, target
  layers, damage/multi-target and visual/lifetime behavior from the matching
  isolated client.

## Sword Blow static-chain correction and client QA package - 2026-09-10

- Re-read the pinned constructor/update jump tables and extracted every used
  float constant directly from the hash-pinned Pegasus PE. Existing threshold
  13, cadence 5, 0.7/1.3 buff scales, plane/ring/multishot rates and squared
  opacity envelope were confirmed.
- Corrected the large-root construction order: small swords now inherit the
  original caster light before the root changes to `(0.2,0.2,0.2)`; the root
  is scale 1 and advances by transformed local `(0,200,0)`; its two planes are
  emitted afterward from local `(0,-200,0)`. Random ranges now use the exact
  inclusive discrete Pegasus precision instead of continuous interpolation.
- Caster dispatch now preserves actor X/Y but obtains root Z from the equipped
  weapon link bone when valid. The large-sword cadence plane receives the
  recovered Direction field, not the unrelated Light field.
- Target roots now preserve the constructor's sole lifetime-15 override and
  no longer invent alpha/blend initialization. Target BC/BD positions use the
  recovered random owner-bone selection and exact normalized direction
  vectors. Large-sword bone lights now use reordered first-key bone anchors,
  matching `0x677A57`, rather than animated-bone positions.
- Added isolated bitmap IDs for the exact staged `Spark03` and
  `ground_smoke_dark` assets. Spark subtypes 36/37 now follow the recovered
  16..27-tick gravity, terrain collision, timestep-aware bounce and movement;
  ground smoke follows the recovered 25-tick growth/fade. The prior native
  Spark call fell through to a two-tick default and was not equivalent.
- Focused catalog and Sword Blow math/protocol tests PASS; staging PASS for 97
  hash-pinned files. Latest isolated client build PASS / exit 0, SHA-256
  `E8ED294CD33C77FE4439FACD8CCBEF631DB9F698E55AD93431B126A6DF930424`;
  linker warnings remain only Crypto++ LNK4099.
- Verified client delivery ZIP:
  `Delivery/SwordBlow_Client_Patch_2026-09-10.zip`, SHA-256
  `9C9EC906DFE1E32C6FD5C666E6EBFB4204E1F0E29BDBF3AE763220DD6361F2DF`.
  It contains `Engine.exe`, all 97 isolated Pegasus assets and QA/rollback
  guidance. Generated client Obj/output and verification staging (about 304
  MB) were moved to the Recycle Bin after ZIP verification.

## Codex task handoff — 2026-09-10

This work is intentionally separated from the RISE Editor task. Continue only
in this worktree (`D:\RISE-CrossPlatform\Source_PC_Pegasus`) on branch
`feature/pegasus-high-skills`. Do not modify the main RISE Editor checkout,
`Source_PC_SkillPreview`, the Android client, or any production client.

Latest isolated client build (former exec session 66093): **PASS / exit 0**.
It includes the high-skill catalog and the `MAX_SKILL_TYPES` client-capacity
split. The only observed link warnings were the existing Crypto++ LNK4099
missing-PDB warnings. This is compilation evidence only, not gameplay or visual
acceptance.

CURRENT TASK: import all twelve Pegasus group-15 items 102..113 and their
skills. Sword Blow (15:110 -> client skill 1501) remains the first vertical
slice.

LATEST CODEX TASK — Sword Blow packet/cast/damage slice: **PASS for focused
tests and isolated builds; runtime acceptance remains open**.

- Added one shared 12-row catalog contract with item 15:102..113 mappings,
  Pegasus skill fields, reverse lookup, pinned Pegasus EXE hash and pinned
  `Data/Local/xml/Skill.xml` hash. The focused MSVC `/W4 /WX` test passes.
- Corrected client catalog application to run after the native skill file is
  loaded. Client skill-ID capacity remains 2048 while the native file row count
  remains 650.
- GameServer now separates 2048 skill IDs (`MAX_SKILL_TYPES`), 60 learned-skill
  slots (`MAX_SKILL_LIST`) and the existing 629 catalog-entry editor limit.
  Skill-delay allocation/clear and skill set/delay access are bounds-safe for
  IDs through 2024.
- GameServer imports the exact transferable Pegasus XML fields for all twelve
  skills and maps every group-15 item 102..113 through the shared catalog.
  Server-only route/effect adapters are intentionally still native generic
  defaults; dedicated buff/multi-target semantics have not been claimed.
- Isolated client build exit 0, SHA-256
  `D05AF8A57C16B0B64E10ADDD372F5B7FD2057630ABAA47B89641657D43C79579`.
  Warnings remain only the existing Crypto++ LNK4099 missing-PDB class.
- Isolated Ex603 GameServer build exit 0, SHA-256
  `ED8D5536BDAFE52E333D4B818B631EA02D5814CAC8BCDF68EEE1CECBE56858ED`.
  Output is confined to `ExGameServer/Tests/PegasusBuild`.
- Hash-guarded Sword Blow staging revalidated 21/21 files in the isolated
  client path. Math and catalog tests both exit 0.
- Added the recovered bounded Sword Blow wire contract: normal `0x19` skill
  cast plus `F3:35` payload (`skill`, `caster`, `count`, max ten target keys),
  explicit little-endian helpers and packet-size tests.
- GameServer now routes skill 1501 through a dedicated attack function. It
  validates the primary target/range, applies native damage to the primary and
  up to nine native-valid nearby targets, then sends `F3:35` containing exactly
  the target indices that were attacked. Geometry currently uses the native
  skill radio around the primary target and remains runtime-QA evidence, not a
  claimed original-server match.
- Client `ReceiveMagic` now binds skill 1501 to recovered action 71
  (`PLAYER_ATTACK_ONETOONE`) and creates the large-sword root. `F3:35` is
  length/count bounded, resolves only live visible characters and creates both
  recovered target roots for each resolved key.
- Native pooled effect hooks now load/render the full nine-model reserved range.
  The recovered large/small sword, plane, ring, three multishot and two target
  constructors/updates are connected. This includes 24/32 Sword Wrath child
  spread, lifetime-13 multishot burst, five-frame plane cadence, three-frame
  target burst, mega-lightning target layer and the big-sword bone-light layer.
  Native pool lifetime decrement/release remains authoritative.
- Sword Wrath no longer reuses Pegasus numeric buff 213, which is outside the
  RISE 5.2 client buff range. A shared native-safe unused effect 168 is seeded
  in GameServer; skill 1500 now applies that self buff for a documented
  provisional 60-second balance duration, and Sword Blow reads that state for
  the recovered 24-child/0.7-scale branch. Cast visuals and owner balance
  acceptance remain open.
- Sword Wrath client dispatch now maps recovered Pegasus action 67 to the
  native RISE self-buff pose (`PLAYER_SKILL_VITALITY`) and uses recovered root
  light `(1,.2,.19)`. A compile-time comparison caught and removed the earlier
  incorrect Fury Strike label before runtime acceptance.
  Two isolated dynamic model slots load the hash-pinned
  `Fire_wall_knight.bmd`/`fire_15fmono.OZJ`; the root and subtype 0..3 layer
  cascade use recovered lifetimes/scales (including two subtype-2 copies),
  owner following and action-gated root rendering. The unresolved Pegasus
  particle resources `0x7F27..0x7F2C` are deliberately not substituted with
  guessed native layers, so Sword Wrath remains `IN_PROCESS`.
- Bounded caster handlers now provide native action dispatch for 11/12 rows:
  Dex Booster and the three MG Charge buffs use the native vitality pose;
  Chaos Blade/Havoc Spear map action 183 to `PLAYER_SKILL_GIGANTICSTORM`;
  Dragon Violent maps 253 to `PLAYER_SKILL_DRAGONLORE`; Spearstorm maps 80 to
  `PLAYER_ATTACK_STRIKE`; Shining Bird calls the native equipment-aware attack
  selector. Sword Wrath and Sword Blow retain their dedicated paths. Raining
  Arrow's Pegasus-only action 286 is beyond native `MAX_PLAYER_ACTION` and is
  deliberately not copied as a raw index.
- Recovered sound registration is cataloged as 18 exact skill/role/path/hash
  rows (Pegasus buffer IDs `0x62/0x63` and `0x3D3..0x3E2`). RISE now reserves
  an isolated 18-slot tail after native `MAX_BUFFER`, sizes/initializes/releases
  that tail with upper-bound checks, loads all 18 staged WAVs and plays the
  proven cast sound for Raining Arrow, the five MG skills, Shining Bird,
  Dragon Violent and Spearstorm. Hit/explosion sounds are not attached without
  target/reaction evidence.
- Hash-guarded isolated staging now validates 72 files: 21 Sword Blow assets,
  two Sword Wrath assets, 18 high-skill WAV files and 31 recovered root-model/
  embedded-texture candidates under the isolated `Recovered` tree.
- The randomized BMD allocation now reserves a separate 128-model tail beyond
  its existing 1024-entry randomization pad, preventing the Sword Wrath dynamic
  slots (and subsequent isolated high-skill slots) from crossing the allocation
  when the random offset is 1023.
- Rebuilt isolated client exit 0, SHA-256
  `E3B6CF2194B5476FBE379C7F72F73CD14D586E7CC11035D8013B948F832BF8BB`;
  warnings remain only the existing Crypto++ LNK4099 class.
- Rebuilt isolated Ex603 GameServer exit 0, SHA-256
  `D09315C3039DC4FD3C7F051C759DA0E4791E823BC595C2724E96ED3D6A5DC1AF`.

CONTINUATION UPDATE — recovered runtime slices (isolated only):

- Spiral Charge now reserves native-safe model slots `MAX_MODELS+2..+7`, loads
  the exact `01spa_skill.bmd`, and dispatches from skill 2014 at owner bone 17.
  The recovered 30-tick root creates the 20/40-tick flare, expanding double
  shockwave, steam and smoke layers with the exact scale/light values. Bitmap
  registrations `0x7F19..0x7F1C` were resolved to hash-pinned
  `flareRed/ Shockwave2/ Steam_01/ smokelines03`; no unrelated native bitmap
  was substituted. The four files raise isolated staging to 76 hash-checked
  assets. Particle-object behavior beyond these visible sprite layers remains
  open, therefore this is not a visual PASS.
- Crusher Charge now reserves `MAX_MODELS+8..+10`, loads the exact
  `02_buff_obj/03_buff_obj` pair and dispatches skill 2015 to the recovered
  bone-0 relative root plus owner ring. The recovered 20/35-tick core/ring and
  four subtype 4/5 children, colors, scales, growth, fade and pool ownership
  are connected. The auxiliary `0x7F1D..0x7F1F` sprite overlays and GameServer
  charge-state semantics remain open.
- Shining Bird now reserves `MAX_MODELS+11`, loads the hash-pinned
  `lightofbird_attack.bmd`, and dispatches skill 241 after the native
  equipment-aware attack selector. The recovered 26-tick, scale-1.3,
  `(0.5,0.7,0.8)` root uses native pooled update/render and owner cleanup.
  Its three `0x7F3D` particle children and target reaction remain open.
- Latest catalog and Sword Blow math/protocol tests both PASS. `git diff
  --check` reports only line-ending warnings. Latest isolated client build
  exits 0, SHA-256
  `BF91675C70967DFFBB8C8738FD8DD8604146AB21CB6B38DF1FD8000D885526B0`;
  the only linker warning class remains Crypto++ LNK4099. All twelve rows stay
  `IN_PROCESS`; compilation is not runtime acceptance.

CONTINUATION UPDATE — buff provenance, Dex Booster and Chaos Blade root:

- Hash-pinned Pegasus `BuffEffect.xml` proves the four self-buff roles:
  Spiral Charge attack+defense/two-handed, Crusher Charge attack-speed+
  defense/one-handed, Elemental Charge wizard/third-Havoc-Spear Nova, and
  Dex Booster attack-speed+attack-success. Shared catalog tests now validate
  all 12 item mappings, 18 sounds and these 4 provenance rows.
- The GameServer routes skills 2014/2015/2016/2024 through a dedicated
  self-buff path instead of zero-damage basic attacks. Collision-guarded
  isolated marker effects 169..172 preserve packet/effect state for QA; stat
  magnitudes, weapon-range transformations and Elemental's third-hit Nova are
  deliberately still open rather than guessed.
- Dex Booster now uses slots `MAX_MODELS+12..+15` and six exact hash-pinned
  bitmaps. Its recovered 80-tick owner aura, three rotating/fading ground
  layers and 30-tick flare roots are connected; the initial 20-particle layer
  and detailed flare motion remain open. Asset staging passes with 82 files.
- Chaos Blade now uses slots `MAX_MODELS+16..+18`, loads the exact
  `M_sword_effect.bmd` and `knight_plancrack_a.bmd`, and creates the recovered
  target-owned root from Pegasus handler `0x654DC3` with light `(0.2,0.2,0.2)`,
  scale 1.2 and 100-tick lifetime. The 8/5-tick child cascade and bitmap
  particles `0x7F33..0x7F38` remain open.
- Latest isolated client build exits 0, SHA-256
  `308D4A16DC1764D3870E63F0551329E5897E6D56C83AEA49352145ED5324B699`.
  Latest isolated GameServer build exits 0, SHA-256
  `250A3515A63CE7B1C299D0CE6C2A6987C4D1550DA7304E28E05D070E6965039A`.
  Client link warnings remain only the existing Crypto++ LNK4099 class;
  `git diff --check` reports only line-ending warnings. These are build facts,
  not runtime or visual acceptance.

CONTINUATION UPDATE — Dragon Violent and Elemental Charge roots:

- Dragon Violent reserves `MAX_MODELS+19..+24` and routes the exact Pegasus
  `dragonhead.bmd`, `knight_plancrack_a.bmd`, and `wind_foce.bmd` models.
  Cast dispatch creates four recovered random-quadrant terrain positions,
  each with a 30-tick scale-1.15 elevated head and a 15-tick scale-0.9 crack.
  The delayed `0x27C2..0x27C7` cascade, movement, particles and hit reaction
  remain open.
- Elemental Charge reserves `MAX_MODELS+25`, loads exact `05_buff_obj.bmd`,
  resolves target bone 17, and creates the persistent owner-bound `0x27B2`
  subtype-0 root proven by `0x664CF8`. Its five bone particle layers and the
  third-Havoc-Spear Nova behavior remain open.
- Hash-guarded staging now passes 83 files. Both catalog and Sword Blow
  protocol/math tests PASS. Latest isolated client build exits 0, SHA-256
  `E72D4BF84904EE6437D810D1E0DFD4484EA79CB9E51C051FBE3FDAFA2276F55C`;
  only Crypto++ LNK4099 warnings were emitted. Nine rows now have a compiled
  client runtime slice, not nine completed skills; runtime QA remains open.

CONTINUATION UPDATE — all twelve rows now enter a compiled runtime path:

- Havoc Spear reserves `MAX_MODELS+26..+31`, loads the exact Pegasus
  `Nova_arrow_sim02`, `Magum_line`, `Shorkwave_gun01`, and plan-crack models,
  and creates the target-owned `0x27A5` root proven by `0x666CB6`. Its
  projectile/explosion cascade and Elemental third-hit branch remain open.
- Spearstorm reserves `MAX_MODELS+32..+34`, loads exact `3cha_Kskill` and
  plan-crack models, and creates the recovered target-owned scale-0.9,
  40-tick `0x27C8` root. Blade/crack children, collision and reaction remain
  open.
- Raining Arrow reserves `MAX_MODELS+35..+46` for `0x2792..0x279D`. The
  `0x2792` root is caster-owned (corrected from the earlier target assumption),
  emits the recovered timed native-arrow volley, and uses terrain collision to
  create target impact layers. The full ring/ground model cascade and exact
  assets are staged; isolated ingame validation remains open.
- Therefore 12/12 catalog rows now have item mapping, packet-visible skill ID,
  cast dispatch and a compiled client runtime entry/root. This is a coverage
  milestone only: all rows remain `IN_PROCESS` because several child layers,
  movement/collision, reaction and all isolated ingame validation are open.
  Latest isolated client build exits 0, SHA-256
  `A2678AFBC3E2A528C5350D2BCA33DC91A2D0A9AAFB61E21217CC9C164FA1DB91`;
  staging (83 files), catalog test and Sword Blow math/protocol test PASS;
  only Crypto++ LNK4099 linker warnings remain.

NEXT EXACT ACTION: owner deploys the verified client and GS QA ZIPs, runs
`/testswordblow`, and captures action, effect-count, target, cleanup and damage
results. Tune only from that runtime evidence; compilation is not visual PASS.
Sword Blow and all twelve rows remain `IN_PROCESS` until owner runtime PASS.

| Item | Skill | Status | Current isolated evidence |
|---|---:|---|---|
| 15:100 | 288 Deathside | IN_PROCESS | target dispatch + exact Reaper/model assets + recovered client root/children; renderer/reaction/runtime QA open |
| 15:101 | 289 Darkness | IN_PROCESS | exact cast action + source-proven absence of any dedicated BMD/particle/joint/sound + marker-only self-buff route/provenance; original stat formula and runtime QA open |
| 15:102 | 2023 Raining Arrow | IN_PROCESS | guarded action/sound + pointer-only caster/target orchestrators + native-arrow volley + bone-0 target follow + exact two-sprite 4x4 `arrow_damge` sheet + independent 0x2792..0x279D impact/ground layers; runtime QA open |
| 15:103 | 2024 Dex Booster | IN_PROCESS | action + marker/provenance route + exact aura, 19 flare joints, 20 moving Impack03 particles, 3 ground rings and 2 flare roots; stat formula/buff cleanup/runtime QA open |
| 15:104 | 2012 Chaos Blade | IN_PROCESS | action/proven Hit-1 sound + target-owned root + recovered root/arc/crack child cascade, particles and two BMDs; apparent `0x3DD` hit was classified as UI text lookup, while genuine third-hit protocol and runtime QA remain open |
| 15:105 | 2013 Havoc Spear | IN_PROCESS | action/sound + target-owned root + six exact model routes; projectile/explosion/third-hit/runtime QA open |
| 15:106 | 2014 Spiral Charge | IN_PROCESS | action/sound + recovered root/layers + exact global live-owner gate + anchor-only buff follow + GS marker/provenance; stat formula/runtime QA open |
| 15:107 | 2015 Crusher Charge | IN_PROCESS | action/sound + recovered root/four children + exact persistent two-ring/17-bone aura + packet-owned cleanup without local marker timeout; stat formula/runtime QA open |
| 15:108 | 2016 Elemental Charge | IN_PROCESS | action/sound + exact five-layer bone-17 root + cast/relog subtype ownership + marker cleanup; stat formula/third-hit Nova/runtime QA open |
| 15:109 | 1500 Sword Wrath | IN_PROCESS | GS self-buff 168 + client action/root/layer cascade; root/rings follow owner while ground-star/wave stay cast-time fixed; particles/runtime QA open |
| 15:110 | 1501 Sword Blow | IN_PROCESS | cast/F3:35/damage + full nine-model effect chain + 21 assets; runtime QA open |
| 15:111 | 241 Shining Bird | IN_PROCESS | native selector/sound + 1.5x6 directional damage + corrected 250-unit root + 3 tracked pillars + exact animation/bone lights/secondary particles/cleanup; ingame timing/visual/pool QA open |
| 15:112 | 242 Dragon Violent | IN_PROCESS | action/sound + stationary eight-point head/crack cascade + exact JointSpirit02 child + seven particle routes/models + target-follow impact; ingame renderer/timing/damage/pool QA open |
| 15:113 | 243 Spearstorm | IN_PROCESS | action/sound + 5-tick target orchestrator + five falling blades + exact forced impact/crack/stone/smoke/layer/fade chain; ingame collision/sound/pool QA open |
| standalone | 737 Wind Soul | IN_PROCESS | isolated item/cast route + homing bird/tails/target layers + corrected cast motion/color/smoke ownership; owner SS21 visual/timing/pool QA open |

No row has passed the full dispatch -> action/target -> root -> children/assets
-> renderer -> movement/collision -> reaction -> lifetime -> ownership/pool ->
runtime-validation gate.

Owner acceptance is still open. Do not report the twelve skills, Sword Blow,
or SS21/Pegasus visual parity as complete merely because the isolated build
passes.

LATEST: session68040 completed exit0. Added EnsureSwordBlowModel using native
BMD::Open2/Bitmaps, private Data/RISE/Pegasus/SwordBlow/Effect path, native
Calc_RenderObject/RenderMesh adapter, and small-sword loading/render dispatch.
Follow-up builds exited0; no build session remains active. Resource loading
and rendering are connected for the small-sword type only. The full cast,
big sword, target effects, bone lights and live visual validation remain open.
Next: provision hash-pinned assets to an isolated test client/private directory,
extend the remaining recovered effect cases, then wire skill1501 dispatch.
Do not run a normal client expecting a complete skill yet.

CURRENT: appended nine model IDs after the old MAX_MODELS boundary, preserving
existing IDs and extending native allocation/cleanup bounds. Small-sword native
CreateEffect and MoveEffect cases now invoke the adapter, with the generic
MoveParticle step excluded to prevent double displacement. Pool lifetime
decrement/destruction remain native. Asset loading, render and skill dispatch
are still pending, so the feature is not usable ingame yet.

BUILD IN PROGRESS: session68040 is the new full rebuild after enum changes.
Poll it before launching another build. Older successful hashes below predate
these lifecycle hooks. Legacy file encodings were restored after patching;
git diff shows only intended hunks and final-newline normalization.

Baseline session 5984 completed with exit 0. Native small-sword adapter added
afterward and a second full build also exited 0. Latest executable SHA256:
93D264F14868CBBDC79BF9D786053BB0FA149A8BC35B1300E9D31ABEB6F6EDB7.
Only warning class observed at link: missing Crypto++ PDB (LNK4099).
`Tests/BuildPegasusIsolated.cmd` confines outputs to Tests/PegasusBuild and
disables post-build deployment. No build is currently running.

Native adapter `RISE/PegasusSwordBlowRuntime.cpp` initializes small-sword
lifetime/speed/light and updates position with native angle transforms plus
the recovered opacity curve. It is compiled in the PC target but is not yet
called from gameplay. NEXT: reserve model/texture IDs without moving existing
IDs; connect creation/update/render and root/target dispatch, then validate
actual gameplay. Compilation does not prove Pegasus/Rise transform parity.

Branch: feature/pegasus-high-skills. Base: f2e8e558 (PC).
Checkout is sparse: PC client, GS/DS and their shared dependencies.
Original source, SkillPreview checkout and production clients remain separate.

Goal: import all twelve Pegasus group-15 items 102..113 and associated skills.
Sword Blow (15:110 -> 1501) is the first implementation. Skill Editor is paused.

Evidence/extraction workspace:
`D:\RISE-CrossPlatform\Source\docs\pegasus_high_skills\SWORD_BLOW_CHAIN.md`
and `artifacts\pegasus_high_skills\sword_blow` under that same Source root.

Implemented here: pure Sword Blow opacity and buff-dependent spread functions
in `ExMain_RISE_PC/Main5.2_RISE/RISE/PegasusSwordBlowMath.h`.
Test: `ExMain_RISE_PC/Tests/RunPegasusSwordBlowMathTest.cmd` — PASS, MSVC x86,
C++17 /W4 /WX. Checks lifecycle boundaries, symmetry, zero-duration fallback,
and observed 24/32 spread selection. This is not a gameplay/visual pass.

NEXT: map native OBJECT fields and create/update/render extension points;
reserve noncolliding resource IDs; add the recovered runtime behavior and
asset registration using RISE APIs. Full client build, GS learning/cast/damage,
shared lightning animation and live gameplay validation remain pending.

Compiler: `C:\Program Files\Microsoft Visual Studio\18\Insiders`.
The separately registered BuildTools instance does not contain MSVC; use
Insiders VsDevCmd with x86 target as in the test runner.
# MG charge weapon-gate correction - 2026-09-12

- Webzen's official Season 16 Magic Gladiator renewal guide
  (`https://muonline.webzen.com/en/gameinfo/guide/detail/244`) independently
  confirms the pinned Pegasus XML semantics: Spiral Charge requires a
  two-handed sword, Crusher Charge a one-handed sword, and Elemental Charge a
  staff; their enhanced ranges are 6, 5 and 7 respectively. The guide also
  confirms the every-third Chaos Blade/Havoc explosion behavior, but publishes
  no ATK/DEF/speed magnitude, so those values remain deliberately unguessed.
- Found a native-5.2 integration hole: `CheckSkillRequireWeapon` returned true
  before inspecting all three new IDs. Non-QA casts could therefore activate
  the wrong stance with an unrelated weapon. Added a pure weapon-family
  contract and a server adapter using only native `CItem` fields: section 0 +
  `m_TwoHand` for Spiral/Crusher and section 5 for Elemental. The existing
  isolated GM `/qa15` bypass remains separate so one-run visual QA still works.
- Expanded the focused charge-range test with positive/negative weapon-family
  cases and added an integration verifier proving the new gate executes before
  the native early return. Both PASS under `/W4 /WX`; the isolated GameServer
  rebuild exits 0. `GameServer.PegasusImport.exe` and the isolated staged
  `GameServer.exe` both have SHA-256
  `289D505DC20C27208FF29D23F043B2552E2E14198B1A5782F81B71C7D47E188F`.
  Runtime stance rejection and visual QA remain open.

## Symbol-backed runtime-state inspection - 2026-09-12

- Added a read-only isolated runtime inspector using the exact current linker
  map and `ReadProcessMemory`; it does not inject input or modify the process.
  The RuntimeClient binary was first hash-matched to the mapped build at
  `6D24C51BCEB3BA8549515E57B0D540142661EBCE8868B7C99AC2FE233B66941E`.
- Live PID 664 reports `CurrentProtocolState=0 (REQUEST_JOIN_SERVER)`,
  `Hero=0`, and all auto-QA flags zero. This proves the current process is not
  logged into a character/map, explaining why no fresh QA log exists. Windows
  capture independently failed twice with `0x80004002`, so no blind UI input
  was attempted. Fresh `/qa15` visual evidence remains pending a real joined
  character.
## Shining Bird native target-reaction proof - 2026-09-12

- Rechecked the complete pinned caster routine `0x672A17`: its ABI receives
  only the caster character/object and creates the launched bird root; it has
  no target-effect argument and therefore provides no evidence for inventing
  a dedicated impact BMD on the victim.
- The isolated server already preserves the 1.5-by-6 directional frustum and
  schedules each authoritative hit after 200 ms. Native RISE
  `ReceiveAttackDamage` owns the actual victim reaction through
  `SetPlayerShock`, so the reaction path is connected without adding a false
  Pegasus visual layer.
- Expanded `VerifyPegasusShiningBirdRouting.ps1` to lock the frustum, delayed
  hit and generic damage-packet reaction together. The verifier passes;
  visual direction/timing and pool reuse still require a fresh live run.

## Animated-BMD coverage audit - 2026-09-12

- Scanned all 35 BMDs in the staged isolated Pegasus tree and inspected their
  action-key counts. The non-trivial carriers include Sword Blow multishots,
  Chaos Blade's meshless sword, Shining Bird, Deathside Reaper/scythes,
  Elemental Charge, Spiral Charge, and Havoc line/shockwave models.
- Added `VerifyPegasusAnimatedBmdRouting.ps1`. It requires an explicit native
  `PlayAnimation`/bone-sampling route for every carrier whose playback was
  proven in the pinned executable. It also preserves the negative evidence
  for `ReaperHead.bmd`: loaded and cleanup-addressable, but not fabricated as
  a child because the whole image has no creation xref.
- The new verifier passes. Multi-key files are not animated merely because
  they contain keys: Havoc Nova/default, Raining Arrow ring01 and shared
  plan-crack models stay governed by their recovered per-type handlers until
  the original executable proves a playback call. This avoids both the old
  “BMD stands still” omission and a blanket animation guess.

# Pegasus server damage-family correction - 2026-09-12

- Audited the native RISE `CAttack::Attack` selector instead of treating the
  imported `SkillInfo::Type` field as a physical/magic discriminator. The
  generic MG branch was sending every new damage skill through
  `GetAttackDamageWizard` unless its ID appeared in the native sword-skill
  exclusion list.
- Corrected only the evidenced mismatch: Chaos Blade 2012 now falls through
  to native physical damage while Havoc Spear 2013 remains wizard damage.
  The hash-pinned Pegasus `Skill.xml` distinguishes the pair with zero Energy
  requirement for Chaos Blade versus 1073 Energy for Havoc Spear, consistent
  with the recovered sword-versus-wizard charge roles. No unproven damage
  multiplier or third-hit magnitude was invented.
- The same selector audit found Deathside 288 incorrectly entering the generic
  Summoner wizard path. It now uses native curse damage, matching the pinned
  Darkness effect description (`Increases Curse damage and Defense`) instead
  of leaving the paired buff disconnected from its attack family.
- Added `VerifyPegasusDamageFamilyRouting.ps1`, which checks both the server
  selector and the pinned original XML. All focused Pegasus server tests pass,
  the isolated GameServer rebuild exits 0, and both the build artifact and
  staged isolated `GameServer.exe` have SHA-256
  `E1334EC20B8C400E4C404BB42AF6C1CC12D8B24D8BB8447E655BCF082F0ED0B5`.
  Runtime damage numbers remain an open acceptance gate.

## Sword Wrath particle/joint coverage closure - 2026-09-12

- Re-audited the implemented Sword Wrath chain and corrected the stale
  full-chain row: the root already emits the recovered bone-17 smoke and
  pin-star accents, and cast creation already emits all four ground/ring
  subtypes plus thirty randomized line joints.
- Expanded `VerifyPegasusSwordWrathRouting.ps1` to lock root-only accent
  emission, bone-17 placement, the exact thirty-joint burst, joint
  lifetime/movement/fade, particle initialization and the authored smoke and
  pin-star lifetimes. This closes the static particle/joint routing gap without
  claiming a runtime pass.
- Sword Wrath is a self-buff, so an enemy target-reaction gate is not
  applicable. It remains `IN_PROCESS`; cast/buff application cadence,
  authoritative removal and final-pool cleanup still require a fresh isolated
  `/qa15` observation.

## Havoc Spear visual-chain audit closure - 2026-09-12

- Rechecked the pinned `0x666129..0x6661AA`, `0x666840..0x666BDC` and
  `0x666E54..0x667147` call paths against the isolated adapter. The line
  projectile, six radial joints, root shockwave, tick-1/odd-tick Nova and end
  explosion, four randomized impact particles, and target-follow two-particle
  orchestrator are already represented; the audit row was stale.
- Expanded `VerifyPegasusHavocSpearRouting.ps1` to require those visual
  children, the recovered tick cadence, and explicit line/shockwave BMD
  playback in addition to the ownership rules. No new visual layer or server
  damage magnitude was guessed.
- Havoc Spear remains `IN_PROCESS`; live visual cadence, target reaction,
  authoritative every-third-Havoc damage and final-pool QA remain open.

## Elemental Charge animated-layer audit closure - 2026-09-12

- Re-decoded `0x6647B7..0x664C42` and confirmed the isolated runtime carries
  the three Flare03 plus two Shockwave constructor children, their growth/fade
  sprite updates, animated `05_buff_obj.bmd`, and all five bone-indexed
  sprite/bright-mesh passes.
- Expanded `VerifyPegasusElementalChargeRouting.ps1` to fail if any of those
  children, BMD playback, bone transforms, sprite layers, mesh layers or the
  packet-owned removal path disappears. This corrects another stale static
  gap without treating source equivalence as visual acceptance.
- Elemental Charge remains `IN_PROCESS`; live five-layer cadence, exact server
  stats, every-third-Havoc behavior, removal and final-pool QA remain open.

## Dragon Violent full visual/reaction routing gate - 2026-09-12

- Expanded the focused Dragon Violent verifier beyond ownership. It now locks
  the eight immediate/delayed radial points, head JointSpirit02, crack-to-wind
  and three-layer cascade, target-follow end/combo/impact particles, the
  authoritative server target list, and native damage-packet victim reaction.
- This closes the source-level target-reaction connection without inventing a
  separate victim BMD. Dragon Violent remains `IN_PROCESS`; live radial and
  impact cadence, damage timing and final-pool reuse still require `/qa15`.

## Spearstorm impact/reaction routing gate - 2026-09-12

- Expanded the Spearstorm verifier to cover the five-tick blade fan, two
  blade trails, forced terrain impact, crack plus five stones and fifteen
  smoke children, expanding impact renderer layers, lifetime-64 sound latch,
  authoritative server damage/cast echo and native damage-packet reaction.
- This closes the source-level forced-impact and reaction paths while keeping
  runtime synchronization separate. Spearstorm remains `IN_PROCESS`; live
  collision/impact cadence, sound synchronization and final-pool reuse still
  require `/qa15`.

## Raining Arrow target/reaction routing gate - 2026-09-12

- Extended the existing renderer/asset verifier to require the authoritative
  server multi-target list and native damage-packet victim reaction in the
  same chain as volley, projectile-joint cleanup and target-follow damage
  sheets.
- Raining Arrow remains `IN_PROCESS`; the real equipped-bow branch, live
  terrain-impact and visual cadence, and final-pool reuse still require an
  isolated runtime pass. The QA-only no-bow fallback is not production proof.

## Chaos Blade visual/reaction routing gate - 2026-09-12

- Expanded the Chaos Blade verifier to lock its bone-17 five-layer impact,
  fixed root/arc/crack cascade, meshless sword bone-sampled ribbons, ground
  renderer, authoritative multi-target list and native damage-packet reaction.
  It still rejects the unproven Hit-2/Explosion sounds.
- Chaos Blade remains `IN_PROCESS`; live visual/damage cadence, the genuine
  every-third-hit protocol and magnitude, and final-pool QA remain open.

## Focused full-chain gate batch and live-window retry - 2026-09-12

- Re-ran the focused Sword Wrath, Havoc Spear, Elemental Charge, Dragon
  Violent, Spearstorm, Raining Arrow and Chaos Blade test/routing gates after
  expanding them; all PASS. The global twelve-skill lifecycle and
  source-proven animated-BMD coverage gates also PASS.
- The symbol-backed inspector reconfirmed live isolated PID 19248 at
  `REQUEST_JOIN_SERVER`, `Hero=0`, with no active QA sequence. Computer Use
  found the exact isolated `RISE` window but its supported capture API failed
  with `0x80004002`; no blind coordinates or authentication input were used.
- This is a verified runtime wait, not acceptance. A joined character is still
  required before the isolated auto-QA can produce fresh `/qa15` evidence.

## Charge-buff and Dex Booster full routing gates - 2026-09-12

- Expanded Dex Booster coverage to include its complete visual burst and the
  authoritative 300-second server marker. The contract and routing verifier
  PASS; exact attack-speed/success stat magnitude remains deliberately open.
- Expanded Spiral Charge coverage to lock the animated root, flare/four child
  emitters, nine joints, steam/smoke particles, persistent 17-bone anchor,
  server marker and packet-owned removal. Both focused tests PASS.
- Expanded Crusher Charge coverage to lock the four-child timed ring cascade,
  mesh-0 renderer, persistent two-ring/17-bone aura, server marker and
  packet-owned removal. Both focused tests PASS.
- All three remain `IN_PROCESS` pending exact server stat formulas and fresh
  isolated runtime cadence/removal/final-pool evidence.

## Deathside and Darkness full routing gates - 2026-09-12

- Expanded Deathside coverage to lock all three randomized Reaper attack
  actions/sounds/scythe carriers, BMD reset, object-blur trails, owner follow,
  three particle cadences, ten-second dismissal, mesh renderer, authoritative
  cast, Summoner curse damage and native victim reaction.
- Expanded Darkness coverage to lock action 145, its collision-safe server
  marker, the paired Deathside curse-damage route, and the source-proven
  absence of dedicated BMD/particle/joint/sound calls.
- Both remain `IN_PROCESS`; exact Darkness Curse/Defense magnitude and fresh
  runtime coverage of all Deathside variants, dismissal, buff removal and
  final-pool reuse remain open.

## Unified twelve-skill source-chain suite - 2026-09-12

- Added `RunPegasus12FullChainGateSuite.cmd`, which runs the shared catalog,
  resource-ID, icon, player-BMD, cast-root, lifecycle and animated-BMD gates;
  one focused route for every group-15 item 102..113; terrain render-state
  coverage; and all Pegasus GameServer contracts.
- Its first run exposed an incorrect assertion in the terrain-state verifier:
  native RISE validly pairs both `EnableAlphaBlend` and `EnableAlphaTest` with
  `DisableAlphaBlend`, whose implementation restores blend, alpha-test,
  depth-mask and cull state. Updated the verifier to count both valid opening
  modes instead of falsely rejecting Dex Booster's translucent pass.
- Re-ran the unified suite after that correction: every shared gate, all twelve
  item-specific routes, the terrain-state check and all five GameServer
  contracts PASS. This proves repeatable source-chain coverage only; the
  runtime acceptance rows remain `IN_PROCESS`.

## Hash-pinned Pegasus metadata provenance gate - 2026-09-12

- Added `VerifyPegasusPinnedMetadata.ps1` and its command runner to the unified
  twelve-skill suite. The gate reads the original Pegasus data read-only,
  verifies the exact SHA-256 of `Skill.xml` and `BuffEffect.xml`, then checks
  all fourteen group-15 skill rows and six related self-buff rows field by
  field.
- The gate explicitly distinguishes skill `Magic_Icon` values from buff-effect
  indices. In particular, Crusher Charge and Elemental Charge use skill-icon
  selectors 366/367 while their buff-effect rows are 215/227; these domains
  must never be merged or treated as native RISE effect IDs.
- The pinned buff XML proves semantics and ranges but contains no numeric stat
  magnitude for Darkness, Dex Booster, Spiral Charge, Crusher Charge or
  Elemental Charge. The gate rejects any later claim that those XML rows
  authorize Attack/Speed/Defense/Curse percentages or values. Exact server
  formulas remain open pending executable/server evidence rather than guesses.
- Re-ran `RunPegasus12FullChainGateSuite.cmd` with the new provenance gate:
  every catalog, metadata, resource-ID, icon, action-BMD, lifecycle, focused
  client route and GameServer contract PASS. This remains source/static
  evidence; it does not change any runtime `IN_PROCESS` state.

## Runtime renderer telemetry correction - 2026-09-12

- Investigated every historical `EFFECT_RENDER ... FAIL` from the last bounded
  15-skill run. Native `Calc_RenderObject` returns false when object alpha is
  below `0.01`, so the old boolean trace incorrectly conflated legitimate
  fade-in/fade-out skips with missing model/texture/renderer failures.
- Added `TraceRuntimeModelRenderStateOnce`: the isolated runtime now records
  `TRANSPARENT_SKIP` separately and reserves `FAIL` for a non-transparent hard
  render failure. `AnalyzePegasusRuntimeQa.ps1` reports both counters and now
  rejects any hard renderer failure, including the affected model offsets.
  The runtime-trace verifier and unified twelve-skill suite both PASS.
- Rebuilt the isolated client successfully. The only linker warning class is
  the existing Crypto++ LNK4099 missing-PDB warning. Deployed only to the
  isolated RuntimeClient and Delivery trees; SHA-256 is
  `C38585F08069F843B7A2802CA234B21249BB4BC73082606B5E2826005761B2AD`.
- Restarted exact isolated runtime as PID 2356. The symbol-backed inspector
  reports `REQUEST_JOIN_SERVER`, `Hero=0`, and no active QA sequence. Fresh
  renderer classification and `/qa15` acceptance therefore remain waiting for
  a manually authenticated character.

## Per-skill visible-render QA gate - 2026-09-12

- Strengthened `AnalyzePegasusRuntimeQa.ps1` from a dispatch/action/pool check
  into a per-skill visible-model gate for every requested item 15:102..113.
  It now requires all source-proven renderable model offsets for Raining Arrow,
  Dex Booster, Chaos Blade, Havoc Spear, Spiral Charge, Crusher Charge,
  Elemental Charge, Sword Wrath, Shining Bird, Dragon Violent and Spearstorm.
  Orchestrator-only and source-proven meshless carriers are intentionally not
  counted as missing visuals.
- Added a dedicated `SWORD_BLOW_RENDER` bounded-range trace for all nine Sword
  Blow BMD roles. These IDs precede `MAX_MODELS`, so the previous dynamic-tail
  tracer silently excluded them even though the owner-approved effect was
  visible.
- Re-analysis deliberately rejects the old run: it cannot separate eight
  historical alpha skips from hard failures, lacks the new Sword Blow render
  rows, and has no PASS for Crusher Charge core offset 8. This prevents the old
  broad `/qa15` result from being reused as false full visual acceptance.
- Rebuilt and deployed the isolated client only. Current SHA-256 is
  `68C5A8E953F6FAF45F8D236F8B221687D258D62B7E2C5D4D293B9B41DB73A18A`;
  the only linker warning class remains Crypto++ LNK4099. Exact isolated PID
  25484 is responsive but still at `REQUEST_JOIN_SERVER`, with `Hero=0`.
- The full twelve-skill source-chain suite, including the strengthened runtime
  trace contract, passes after this change. Runtime rows remain `IN_PROCESS`
  until a fresh authenticated run satisfies the new per-skill matrix.

## Crusher Charge transparent-carrier correction - 2026-09-12

- The first visual matrix intentionally exposed historical offset `+8` as
  never producing a render PASS. Re-read the complete pinned initializer,
  updater and renderer blocks instead of making that carrier visible.
- Pegasus type `0x27AE` initializes with `Scale=0` and `Alpha=0`; updater
  dispatch at `0x65A7EF` handles only `0x27AF..0x27B1`. The renderer reaches
  the generic mesh call but supplies the unchanged zero scale/alpha. Therefore
  `+8` is an intentionally transparent child-orchestration carrier, not a
  missing visible layer.
- Removed only `+8` from Crusher Charge's required-visible matrix, retained
  `+9`, `+10` and persistent aura `+52`, and expanded the focused route gate
  to reject any invented core updater/fade. Focused and full twelve-skill
  suites PASS. No product behavior was changed from Pegasus here; only the QA
  classification and provenance comments were corrected.

## Automatic framebuffer QA capture - 2026-09-12

- Added an isolated-only framebuffer capture scheduler to the real `/qa15`
  cast echo path. Each requested item 15:102..113 now schedules three native
  `SaveScreen` frames at approximately 250, 650 and 1050 ms after dispatch;
  duplicate target echoes cannot restart an active capture sequence.
- `AnalyzePegasusRuntimeQa.ps1` now requires 36 fresh, non-empty JPG files
  (three frames for each of the twelve requested skills) in addition to the
  dispatch, action, renderer, child/lifetime and pool gates. These captures are
  evidence for manual visual comparison, not an automatic parity claim.
- Runtime trace contract and the complete twelve-skill source-chain suite PASS.
  The isolated client rebuilt with exit 0; the only linker warning class is the
  existing Crypto++ LNK4099 missing-PDB warning.
- A post-build source audit found that native `MainScene` rewrote
  `GrabFileName` immediately before `SaveScreen`, which would have redirected
  QA frames into the normal screenshot path despite `QA_CAPTURE ... ARMED`.
  Added an isolated framebuffer-owner flag so only normal user screenshots run
  that rewrite/chat branch; QA captures retain their deterministic filename.
  The verifier now locks this ownership guard and the complete suite PASSes.
- Deployed only to isolated RuntimeClient and Delivery. Current executable
  SHA-256 is `CD38E874AE4AC8CE0F5C95507813EA23D82C0994036EDACB18E0FD1DB9CF120C`.
  Exact isolated runtime PID 3760 is running and a watcher is waiting for the
  fresh `AUTO_QA_POOL` marker. Authentication remains manual; runtime rows stay
  `IN_PROCESS` until that run and frame review complete.
- Rebuilt the isolated GameServer after the client capture correction. It exits
  0 and produces `GameServer.PegasusImport.exe` with SHA-256
  `E1334EC20B8C400E4C404BB42AF6C1CC12D8B24D8BB8447E655BCF082F0ED0B5`.
  This is isolated build evidence only; it does not prove live VPS behavior.

## Multi-key BMD runtime progression gate - 2026-09-12

- The previous renderer matrix proved mesh submission but could still accept a
  visible BMD frozen on its first key. Added thresholded runtime frame telemetry
  and analyzer requirements for only the source assets with multiple authored
  keys: Chaos Blade root, Havoc line, Spiral root, Elemental buff, Shining Bird,
  and Sword Blow multishot01/02/03. One-key BMDs are deliberately excluded.
- The same audit now requires exactly three `QA_CAPTURE` ARMED rows per skill,
  phases 0/1/2, and opens every resulting file through the JPEG decoder instead
  of accepting a non-empty but corrupt file. A contact-sheet builder is staged
  to assemble the 36 validated frames immediately after a passing live run.
- The first incremental build exposed three missing trace-header includes; they
  were corrected before deployment. The subsequent full isolated client build
  and complete twelve-skill gate suite exit 0. Link warnings remain only the
  existing Crypto++ LNK4099 missing-PDB class.
- Current isolated RuntimeClient/Delivery SHA-256 is
  `A07F044BBF85F37BEC2C851B19E2FAB117B2B007887C0280800B9694CC8CCF58`.
  Exact runtime PID 21384 and watcher session are live, still awaiting manual
  authentication before `/qa15` can generate authoritative runtime evidence.

## Authoritative attack-damage runtime correlation - 2026-09-12

- A successful cast echo does not by itself prove that the target received a
  native damage packet. Added isolated-only correlation from the first target
  echo to `ReceiveAttackDamage`, bounded to the same target viewport and a
  three-second window so unrelated combat cannot satisfy the gate.
- The analyzer now requires a positive authoritative damage packet for all
  seven attacking skills in the requested 15:102..113 range: Raining Arrow,
  Chaos Blade, Havoc Spear, Sword Blow, Shining Bird, Dragon Violent and
  Spearstorm. The five self-buffs are correctly excluded from victim reaction.
- Isolated client build and the complete twelve-skill gate suite exit 0; only
  the known Crypto++ LNK4099 missing-PDB warnings remain. Current deployed
  RuntimeClient/Delivery SHA-256 is
  `7FF653FAE820F6611ECB161A715117022D1658B09D5DF5F90F12101C539F8123`.
  Exact runtime PID 5640 and watcher session remain live at the manual-login
  boundary. No damage/reaction runtime PASS is claimed before the fresh run.

## Fixed-ID Sword Blow ownership/pool gate - 2026-09-12

- Found that the delayed pool snapshot counted dynamic Pegasus models and
  Elemental native-texture children but not Sword Blow's nine fixed IDs below
  `MAX_MODELS`. That could report a false CLEAN result while a Sword Blow
  carrier remained live.
- Added isolated allocation/destruction traces for all nine fixed Sword Blow
  roles, requires at least one allocation and exact allocation/release parity
  per offset, includes fixed Sword Blow objects in `AUTO_QA_POOL`, and includes
  their pool-full failures in the analyzer.
- Isolated client build and complete twelve-skill gate suite exit 0. Only the
  existing Crypto++ LNK4099 missing-PDB warnings remain. Current deployed
  RuntimeClient/Delivery SHA-256 is
  `9B8CF568B59A73097079CFEB69E0E9A4FAB98645A56F05D48D99F3E0EF1D3034`.
  Exact runtime PID 23756 and its watcher are live at manual login; runtime
  acceptance remains unclaimed.

## Per-model dynamic lifecycle gate - 2026-09-12

- Strengthened the aggregate pool snapshot into per-model lifecycle evidence.
  Every requested dynamic model offset 0..46 plus Crusher aura offset 52 must
  now be allocated at least once and have exact create/completion-release
  parity during the bounded run. This covers non-rendering orchestrators as
  well as visible model roles.
- Shining Bird legitimately ends by non-looping BMD completion and sets its
  object non-live before the generic destructor sees it. Added an explicit
  `BMD_COMPLETE` lifecycle record at that source-proven transition rather than
  inventing a generic destructor event.
- Isolated client build and full twelve-skill suite exit 0; linker warnings are
  still only Crypto++ LNK4099. Current RuntimeClient/Delivery SHA-256 is
  `BC865D09F6DD697EF6BC66F0DE0C16DFC0D63965E0E816E2AF0505D72CDD75A7`.
  Exact runtime PID 4956 and watcher remain live at manual authentication.

## DirectSound runtime playback gate - 2026-09-12

- File staging and source `PlayBuffer` calls did not prove that DirectSound
  owned a valid buffer or successfully started playback. Added isolated traces
  at the native `PlayBuffer` boundary for disabled audio, missing buffers,
  playback failure and PASS.
- The runtime analyzer now requires actual DirectSound PASS for the proven cast
  and impact buffers used by Raining Arrow, Havoc Spear, Spiral Charge, Crusher
  Charge, Elemental Charge, Chaos Blade, Dragon Violent, Shining Bird and
  Spearstorm. For Havoc's alternating hit variants, either proven hit buffer is
  sufficient in a one-target QA sample; unused catalog entries are not guessed.
- Isolated client build and full twelve-skill suite exit 0; warnings remain only
  Crypto++ LNK4099. Current RuntimeClient/Delivery SHA-256 is
  `BFEBAEA102619CAE29BCDF295EF1DBF952E7A3CAA7BA16A521AC76F528D819D6`.
  Exact runtime PID 5356 and its watcher remain live at manual authentication.

## Movement and collision runtime milestones - 2026-09-12

- Completion audit separated stationary buff/ground effects from the four
  requested skills with source-proven travel or collision. The analyzer now
  requires Raining Arrow terrain impact with no above-limit escape, exactly
  five Spearstorm forced impacts, Shining Bird's 250-unit forward offset, and
  actual positive displacement for both Sword Blow big and small swords.
- The first build exposed missing direct shared-catalog/trace includes in three
  runtime translation units. They were added, then the isolated client build
  and complete twelve-skill suite both exit 0. Only Crypto++ LNK4099 warnings
  remain.
- Current RuntimeClient/Delivery SHA-256 is
  `65AAD50D747448FA793DFDFDB4E05642CA7FC04ED1C9A5262EFDC4F6DADF6DE7`.
  Exact runtime PID 772 and watcher are live at manual authentication; these
  movement/collision gates have not yet received live PASS evidence.

## Native buff-map application/removal gate - 2026-09-12

- Strengthened the five requested self-buff checks beyond packet receipt.
  `BUFF_STATE` is now emitted after the native character buff-map mutation,
  and the analyzer requires both `ACTIVE` and `CLEARED` for Dex Booster,
  Spiral Charge, Crusher Charge, Elemental Charge and Sword Wrath. Any
  `REGISTER_FAILED` or `UNREGISTER_FAILED` transition rejects the run.
- The focused concurrent-trace contract, isolated client build and complete
  twelve-skill source-chain suite all exit 0. The linker warning class remains
  only the existing Crypto++ LNK4099 missing-PDB warning.
- Deployed only to isolated RuntimeClient and Delivery. Current SHA-256 is
  `E651D6E3FA0B711467EC3311B765D54EDDCFB50F6B58789DD0EBDD04DE23111A`.
  Exact isolated runtime PID 12488 and watcher session 99248 are live. Runtime
  acceptance remains `IN_PROCESS` until manual authentication permits the
  fresh `/qa15` run, analyzer PASS and framebuffer comparison.

## Skill learning and live skill-list presence gate - 2026-09-12

- Added a focused server learning-route verifier for all item rows 15:102..113.
  It locks the catalog lookup into native `CharacterUseScroll`, requirement
  checks, `AddSkill` slot insertion and `GCSkillAddSend`; the new test passes.
- Strengthened runtime QA so every one of the fifteen timed samples must receive
  its individual `SKILL_LIST_ADD` packet and then be observed `PRESENT` in the
  client's skill array before cast evidence can pass. This directly guards the
  historical case where a test command announced a skill but it was absent from
  the usable client list. Wind Soul 737 was added explicitly to this telemetry
  because it is outside the fourteen-row Pegasus item catalog.
- The focused runtime-trace contract, isolated client build and complete
  twelve-skill source-chain suite exit 0. Only the existing Crypto++ LNK4099
  warning class remains. RuntimeClient and Delivery now carry SHA-256
  `0C083891D13F05433507B8547042965A0DC65B28B53D27F0FDFF8B655F0324EC`.
  Exact isolated PID 24976 and watcher session 26149 are live at the manual
  authentication boundary; no new runtime acceptance is claimed yet.

## Full fifteen-skill framebuffer capture correction - 2026-09-12

- Found that the one-run command dispatched fifteen skills but the framebuffer
  scheduler and contact sheet covered only the twelve item rows 15:102..113.
  Deathside, Darkness and standalone Wind Soul therefore had no comparable
  visual frames even though their cast rows were present.
- Expanded the scheduler, analyzer and contact-sheet order to all fourteen
  Pegasus catalog rows 15:100..113 plus Wind Soul. A passing run now requires
  exactly 45 decodable frames: phases 0/1/2 for every one of the fifteen
  samples. Deathside's second cast lands inside the first cast's bounded
  capture window, preserving summon and repeat-attack evidence without adding
  another manual run.
- The first compile correctly rejected an unavailable local Wind Soul enum;
  the route was corrected to the already shared, tested
  `kRuntimeQaWindSoulSkill` constant. The subsequent isolated build and full
  twelve-skill suite exit 0; warnings remain only Crypto++ LNK4099.
- Deployed only to isolated RuntimeClient and Delivery. Current SHA-256 is
  `D1307615F52943E91003F8CD20673DDD2B09D6FDBB774C825788FE6DED696C9B`.
  Exact isolated PID 17772 and watcher session 68291 are live at manual
  authentication. Runtime acceptance remains `IN_PROCESS`.

## Deathside repeat-capture race guard - 2026-09-12

- Found a timing race after expanding capture to all fifteen skills. Deathside
  intentionally casts twice one second apart; if the repeat echo arrived just
  after phase 2 completed, the old active-window-only duplicate check could
  schedule a second three-frame set and make the exact-count analyzer fail.
- Capture scheduling now retains the last skill/start tick and suppresses same-
  skill echoes for 3500 ms. This covers delayed repeat and late target echoes
  while remaining below the four-second transition to the next QA sample.
- The first suite run exposed a stale verifier token for the old direct
  `GetTickCount()+250` expression. The verifier was corrected to the shared
  captured `now` value; focused trace test, isolated client build and complete
  source-chain suite then exit 0. Only Crypto++ LNK4099 warnings remain.
- Deployed RuntimeClient/Delivery SHA-256 is
  `AA6A2E81056E77247174A37FDF48482998502069A1D4CBE592926721DB46DEF4`.
  Exact isolated PID 8500 and watcher session 47109 are live at manual login;
  runtime acceptance remains `IN_PROCESS`.

## Deathside renderer, sound and randomized-child QA closure - 2026-09-12

- Found a false-positive hole in the expanded `/qa15` analyzer: Deathside was
  dispatched and framebuffer-captured, but its visible Reaper model offset 47,
  authored BMD advancement, appear/disappear sounds and randomized meshless
  scythe offsets 49..51 were not part of the strict renderer/lifecycle gates.
- The analyzer now requires Reaper renderer PASS and multi-key progression,
  DirectSound PASS for appear/disappear plus at least one of the three source
  attack sounds, root offset-47 allocation/release parity, and allocation plus
  release parity for every randomized scythe branch actually selected during
  the bounded run. Offset 48 remains deliberately excluded because the pinned
  executable has no creation xref for `ReaperHead.bmd`.
- Focused runtime-trace verification and PowerShell parser validation pass.
  This changes only post-run evidence analysis, so the already deployed
  isolated PID 8500 remains valid; no production path was touched. Deathside
  and all requested skills remain `IN_PROCESS` until fresh authenticated
  runtime evidence and frame comparison pass.

## Joint/particle final-pool gate and reference-video extraction - 2026-09-12

- The prior delayed pool snapshot covered dynamic/fixed `Effects` but could not
  reject a leaked Pegasus `JOINT` or `PARTICLE`. Added isolated final scans for
  the reserved Pegasus texture namespace and the exact Wind Soul two-line
  ghost subtype. The child snapshot logs every live match and contributes to
  the same `AUTO_QA_POOL` CLEAN/LEAKED verdict.
- The analyzer now requires exactly one `AUTO_QA_SEQUENCE COMPLETE`, a clean
  `AUTO_QA_POOL_CHILDREN` row with zero joints and zero particles, and reports
  the child counts separately. The focused trace verifier, analyzer parser,
  complete source-chain suite and isolated client build all pass. Build output
  contains only the existing Crypto++ LNK4099 warning class.
- Extracted reproducible contact sheets from the supplied original/current
  videos into `ExMain_RISE_PC/Tests/PegasusBuild/VideoAudit`. These are review
  evidence only; the older QA15 capture visibly predates the current routes and
  is not used to accept the new build.
- Deployed only to isolated RuntimeClient and Delivery. Current SHA-256 is
  `24144D562C5D37AC6E00EFCF387758EEDE76CC2AFA7F5B2B920A78C3E1139ACE`.
  Exact isolated PID 16128 and watcher session 69926 are live at manual login.
  No runtime/visual PASS is claimed yet.

## Exact child-slot ownership telemetry - 2026-09-12

- Strengthened the joint/particle cleanup gate so native shared texture IDs no
  longer escape classification. Each source-identified Pegasus joint slot is
  tagged on allocation; particle creation also accepts an isolated ownership
  tag for the native lightning/light/explosion routes used by Havoc Spear,
  Spearstorm and Sword Blow. Slot reuse resets the tag, avoiding stale-owner
  false positives.
- Runtime telemetry now proves that at least one Pegasus joint and particle
  was actually allocated, rejects joint/particle pool exhaustion, and scans
  those exact tagged slots after the cleanup delay. This covers private atlas
  children, Wind Soul roots, Dex Booster flares, Dragon Violent's spirit joint,
  Chaos Blade ribbons/joint and the tagged native particle branches without
  counting unrelated ambient effects.
- Focused trace verification, the complete twelve-skill source-chain suite and
  the isolated client build all exit 0. Linker warnings remain only the known
  Crypto++ LNK4099 class. RuntimeClient/Delivery SHA-256 is
  `14FE921E8AEA8FF8EAD2B449EC8D988D338BDB956867C5A27C5CE8FE6D01D7FF`.
  Exact isolated PID 16480 and watcher session 40275 are live at manual login;
  runtime acceptance remains `IN_PROCESS`.

## Dynamic-texture child ownership correction - 2026-09-12

- Re-auditing the slot tags found that Deathside and Sword Wrath load several
  textures dynamically, so a fixed private-ID range alone could not identify
  their particles/joints. Added handle-based predicates after the guarded
  asset load: all four Deathside particle textures, Sword Wrath smoke/pin-star
  particles and its line joint are now tagged without hardcoding allocator IDs.
- Chaos Blade's native `0x7DE0` subtype-19 joint is additionally stamped with
  skill 2012 at its existing call boundary, allowing the generic joint allocator
  to distinguish it from unrelated native uses. The visual parameters and
  update path are unchanged.
- Focused trace verification, full twelve-skill source-chain suite and isolated
  client build all exit 0; warnings remain only Crypto++ LNK4099. Deployed
  RuntimeClient/Delivery SHA-256 is
  `5B319A9C2F3C85A01308E930AB5C842946C55FB97BBE815527995A43D03BC6A6`.
  Exact isolated PID 1264 and watcher session 98362 are live at manual login.
  Runtime acceptance remains `IN_PROCESS`.

## Manual-login runtime boundary correction - 2026-09-12

- Found that the cast-only marker correctly avoids automated authentication,
  but the analyzer still required LoginWin's `AUTO_QA_LOGIN ARMED` row. That
  row can never exist in cast-only mode, so a complete manual-login `/qa15`
  run would have been rejected before analysis.
- Added `AUTO_QA_RUN 15/<pid> ARMED` only after a live hero has joined the map
  and immediately before the five-second command delay. The analyzer accepts
  the newest login/run boundary, so stale rows from an earlier process cannot
  satisfy the new run. Authentication itself remains entirely manual.
- Focused trace/parser verification, full twelve-skill source-chain suite and
  isolated client build all exit 0; linker warnings remain only Crypto++
  LNK4099. Deployed RuntimeClient/Delivery SHA-256 is
  `CE97C541CA4263BCDF394B10BF6C6CDF4512D54196ECD660013E3A2E1C25E23E`.
  Exact isolated PID 20068 and watcher session 10585 are live at manual login.
  Runtime acceptance remains `IN_PROCESS`.

## Native target-reaction runtime gate - 2026-09-12

- Found that a positive damage packet was previously treated as target-reaction
  evidence even though native RISE calls `SetPlayerShock` only on the actual
  reaction branches. In particular, one non-success branch explicitly skips
  monster class 275, so packet receipt alone could produce a false PASS.
- Added bounded `TARGET_REACTION ... APPLIED` telemetry immediately after the
  real `SetPlayerShock` calls in `ReceiveAttackDamage`. The analyzer now
  requires that event independently for each of the seven attacking requested
  skills, in addition to their positive authoritative damage packets.
- Focused trace/parser verification, full twelve-skill source-chain suite and
  isolated client build all exit 0. Only the known Crypto++ LNK4099 warnings
  remain. Deployed RuntimeClient/Delivery SHA-256 is
  `72A957CFCEB77E3035618F76AF4F55D5AA2B34BA8A5FB0C1A34556F2F270E927`.
  Exact isolated PID 10848 and watcher session 72909 are live at manual login.
  Runtime reaction/visual acceptance remains `IN_PROCESS`.

## Reaction-capable QA target selection - 2026-09-12

- Confirmed the native client deliberately suppresses `SetPlayerShock` for
  monster class 275. The previous `/qa15` fallback spawned that exact class,
  so it could never satisfy the new reaction gate reliably.
- Added an isolated-only target selector which preserves the native range,
  radio and target checks while excluding class 275. When no suitable monster
  exists, `/qa15` now spawns eight class-0 QA monsters; each submission logs
  `target_class` so the runtime evidence identifies the selected target.
  Native combat reaction logic is unchanged.
- The focused QA-command safety test, isolated GameServer build and complete
  twelve-skill source-chain suite pass. New isolated GameServer SHA-256 is
  `3D119919BA5E7FC7C64D81050CEA5A69F1E598D7030370314B3DF972218A179A`.
  PID 10848 exited before QA and watcher 72909 reported
  `CLIENT_EXITED_BEFORE_QA`; no new runtime or visual PASS is claimed.

## Native reaction sampling correction - 2026-09-12

- Audited the complete native `ReceiveAttackDamage` branch after excluding
  monster 275. For ordinary monster hits with the success flag clear,
  `SetPlayerShock` still passes through `rand_fps_check(2)`. A single cast per
  attacking skill therefore made the seven-skill reaction gate predominantly
  random rather than a reliable runtime check.
- The isolated server now submits ten ordinary casts for each of Raining
  Arrow, Chaos Blade, Havoc Spear, Sword Blow, Shining Bird, Dragon Violent and
  Spearstorm. It does not force the damage flag and does not patch the native
  reaction branch. Framebuffer scheduling suppresses repeated same-skill
  windows for 15 seconds, preserving exactly phases 0/1/2 and 45 final images.
- Updated the compiled QA contract, action/dispatch analyzer and verifier to
  require the same cast cardinalities. Focused tests, both isolated builds and
  the complete source-chain suite pass. Only the known Crypto++ LNK4099 client
  warnings remain. Deployed isolated client SHA-256 is
  `B2707DA3173C0ECF4C8BCD7AC6906809932CBDB94D048799684D79718D263FF2`;
  isolated GameServer SHA-256 is
  `35A1AE08EBE35C04E7EDD6A44E0947ED0644AF1178107B420521914F088F831E`.
  Runtime/visual acceptance remains `IN_PROCESS` until this matched pair runs.
- Follow-up timing audit moved the repeat interval from 1000 to 1250 ms. The
  previous interval could start a second root before the third framebuffer
  phase at about +1050 ms and only 100 ms after the action sampler. The new
  interval keeps all three images and the action verdict attributable to one
  cast while remaining below the four-second step gap.
- Rebuilt and redeployed only the isolated artifacts after that correction.
  Current client SHA-256 is
  `8EB0E5B59A6F3C0087A1BC2A05B9CA5589B8B21808ACB22560E10916D0A5CA55`;
  current GameServer SHA-256 is
  `5DDE88FE82EAB7D598D09D9F7728FD1DAA03E27139BB57277BD9757E2416ED3B`.
  The auxiliary dispatch recorder now waits 180 seconds and records each
  unique skill only once, so repeated reaction samples cannot consume its
  fifteen-skill quota. Full source-chain and focused runtime-trace gates pass.
- Staged a temporary isolated delta package containing only the matched client
  and GameServer executables plus a hash/readme manifest at
  `ExMain_RISE_PC/Tests/PegasusBuild/DeliveryPatch/Pegasus_QA15_reaction_sample_v4_delta.zip`.
  Archive SHA-256 is
  `699FEC3CABC973B46576FCBA300AC843560972528B1AF23741A2F0BB7F3D7CA9`;
  both embedded binaries were re-hashed directly from the ZIP and match the
  documented pair. No configuration changed in this delta. Delete the package
  after owner transfer/runtime use as requested.
- Re-auditing analyzer cardinalities found that movement checks still expected
  the old single-cast counts. Updated the runtime contract to require ten
  Shining Bird forward milestones and fifty Spearstorm forced impacts (five
  per each of ten casts). Sword Blow remains one small-sword and one big-sword
  milestone because those two diagnostic sites are intentionally latched once
  per client process; the underlying movement still updates every object.
  Without this distinction the matched runtime would have produced a
  deterministic analyzer false failure despite correct movement.
- Runtime launch-path failure was reproduced rather than inferred. Opening the
  isolated executable through Codex/Explorer inherited an unrelated current
  directory, so native relative `Data\\...` opens first reported `Player.bmd`
  and then `Mix.bmd` as missing although both staged files existed. The
  isolated build now installs a forced `.CRT$XIB` initializer (verified present
  in `Winmain.obj`) before C++ global constructors and repeats the module-path
  anchor at `WinMain`. This does not affect non-Pegasus builds.
- `CGToolKit::BmdRISE()` was a second independent startup gate: it still
  required native `Data/Player/Player.bmd` CRC `0x7D56FCEB` while the isolated
  renderer intentionally loads the pinned Pegasus action superset. The
  isolated branch now verifies the staged Pegasus file at
  `Data/RISE/Pegasus/Recovered/Data/Player/player.bmd` with CRC32 `0xA7C3C76D`
  and retains the native check outside the isolated build.
- RuntimeClient was confirmed to be an overlay, not a standalone client. A
  read-only copy from the existing RISE 5.2 client populated missing baseline
  assets only into the isolated RuntimeClient, then Pegasus deployment was
  applied again. It now contains 20,424 files / 1,910,022,370 bytes. An
  adversarial launch from the Codex task directory stayed responsive for 45
  seconds and reached the authenticated character `Tien` in World4. Current
  isolated client SHA-256 is
  `31C3406574E55DAA2BC9435022AD5E34078B6F52C5F7B86CCB52976F609691CB`.
  This proves startup-path recovery only; QA15 visual acceptance remains
  `IN_PROCESS`.
