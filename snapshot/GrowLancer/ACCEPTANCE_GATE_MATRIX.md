# Grow Lancer acceptance gate matrix

Overall status: `IN_PROCESS`.

Legend: `PASS` is supported by the named evidence; `STATIC PASS` has not been
accepted in gameplay; `OPEN` has no sufficient evidence; `BLOCKED` requires an
external input or explicit compatibility decision.

| Gate | Status | Authoritative evidence | Remaining proof/action |
|---|---|---|---|
| Exact base skill identity 271..279 | PASS | `grow_lancer_skills_descriptor.json`, `CLIENT_SERVER_CATALOG_DIFF.md` | Keep client/server version profiles separate |
| Grow Lancer class mapping | OPEN / boundary audited | Owner chose native GL base7/DB112; source contract/class-data verifiers pin S21 Lua/rate keys and class wire algebra. `CalcCharacter.ini` explicitly delegates extra defaults to unsupplied original S21 `MuOnline.dbo.DefaultClassType`; local QA class112 row is not S21 authority. `CLASS_CONSUMER_CAPACITY_AUDIT.md` maps remaining create/login/packet/stat/skill consumers. Equipment and quests1–3 are excluded, not base learning/cast. | Original S21 default row plus native class7 create/login/persistence/packet and rate/skill authority before activation; no RF/DK fallback |
| Isolated DB class112 seed/create/rollback | QA DB PASS / S21 authority OPEN | Current `GrowLancer_QA` `DefaultClassType` row and `dbo.WZ_CreateCharacter` were inspected; `verify_local_db_class_seed.sql` created a synthetic class112 character in an outer transaction and proved rollback left account/character 0/0. `LOCAL_DB_CLASS_PROFILE_REVERSE.md` records QA stats 25/27/32/20, different from the older `MuOnline52` candidate 30/30/25/24. | Native GS/DS create/login/character persistence still untested; neither local row is verified S21 default-stat authority |
| Native class-body resource/GPU plumbing | PASS (isolated probe) / gameplay OPEN | Latest private RuntimeQA PID25532: shader init, 3 texture readbacks, 5 body parts upload/cache, native draw samples, owner guard matrix and VAO/texture release all pass; clean sampler-conflict and missing-material rollback fixtures also pass; `verify_class_body_gpu_runtime.py` | Class selector/evolution/equipment caller, gameplay activation and S21 pose/pixel parity remain open |
| Scroll/item mapping | PASS for seven scrolls / PARTIAL overall | decoded S21 `item.bmd` plus pinned `GameServer S21` `ItemList.xml`; verifier corroborates names, group/index, GrowLancer/stat/drop gates; `verify_s21_no_circle_clash_scroll.py` pins `ItemSkill=0` and the absence of named Circle/Clash scroll rows | Circle Shield 272 and Clash 275 have no proven class/default/siege learn source |
| Costs, requirements, damage, range, delay | PASS as supplied-server catalog | pinned `SkillList.xml`, shared contract tests | Four rows differ from bundled S21 client; runtime server authority still unconnected |
| Regular formulas | SOURCE/BOUNDS STATIC PASS / RUNTIME AUTHORITY OPEN | Pinned S21 `RegularSkillCalc.lua`; existing shared double helper corrected invalid Spin/Harsh/Magic barrage indices to Lua zero/default and added fractional Obsidian effect/time output. `verify_regular_skill_lua_formula_port.py` and expanded x86 W4/WX server contract PASS; isolated Ex603 GS `FormulaAudit` link SHA67391E59... PASS. No duplicate formula package and no guessed rounding. | Connect only after S21 target/hit-count/recipient/duration/rounding handler semantics and real GL class authorization are proven. |
| Master-skill mapping | STATIC PASS | `MASTER_SKILL_REVERSE.md`, 13-row verifier | Runtime master dispatch disabled |
| Client local/receive dispatch | STATIC PASS | nine reverse files, `CLIENT_PACKET_REVERSE.md` | Gameplay receive path cannot run until protocol/class adapter exists |
| Skill packet field width | PASS for skill ID only | RISE two-byte skill fields and `WORD` skill persistence | Does not cover buff IDs |
| Buff ID delivery/storage | OPEN | `verify_buff_storage_contract.py`: BYTE GS/client fields and 13-byte legacy GS/DS records | Wrath424/425 need a scoped verified extension; do not widen slots or legacy records blindly |
| S21-to-RISE opcode translation | OPEN | S21 C1:35/C1:57/C1:59 proved; RISE 0x57 collision proved | Matching server packet handlers or an approved new native protocol contract |
| Actions/animation | STATIC PASS | S21 actions 185..194 appended as RISE 284..293; merged player hash | Owner gameplay timing/facing comparison |
| Models/textures/sounds | STATIC PASS / 18 VISIBLE BMD GPU UPLOAD PASS | `asset_provenance.json`, complete private overlay verifier; native package loader bypasses stock `modelId >= MAX_MODELS` converter without changing SS6 code. Hidden PID4008 proves exact Wrath material GPU pixels; subsequent staged PID3248 exit0 and `verify_private_model_gpu_log.py` prove all registered visible BMD IDs 10119..10136 opened with live texture, geometrical VAO/VBO/IBO/index and stable release. Breche root cache has zero meshes. | Runtime draw/pixels/source parity and non-BMD layer provenance confirmation for every skill; GPU upload is not visual PASS or GS authority |
| Direct S21 skill icons | PASS | `SKILL_ICON_REVERSE.md`, exact atlas hashes/UV verifier, owner F10 gameplay screenshot after 512-atlas divisor fix | Preserve isolated 512 atlas branch; legacy SS6 remains 256 |
| Spin Step full visual chain | NATIVE SAMPLER/CROSS/POSE FIXTURE PIXELS/CLEANUP PASS / OWNER VISUAL FAIL | S21 motion-blur NEAREST/CLAMP and private slot32986 GPU isolation PASS. Hash-pinned S21 `spin_cross` v0F/private v0C equal plaintext; current hidden cross PID3532/Magic PID4448 regressions PASS. Native merged-player action285/bone33 pose ribbon PID17052 has nonzero minWidth181.108, 114 fixture pixels, GL0/stable release; synthetic style1 blur PID22320 6,379 fixture pixels. `verify_spin_blur_world_transform.py` pins S21 translated endpoint/rotated-offset map to native BMD API. `verify_spin_caster_bone_33.py` pins source/native `knife_gdf` bone33 and byte-identical action186→285 clip; raw header padding is noncanonical. Older target-bound PID28160 had 20 stock-texture32019 blur draws/19 cross submits but owner saw only ring. New historical-cadence verifier pins action285 speed0.706, frame4.706 stage0 at250ms then frame5.001 stage1 at266ms, before action exit: old fanout gate was reached, not current gameplay pixel proof. Current QA-only cross gameplay query built, no new F7 row. Source receiver and pinned S21 frame41 confirm **base271 cast plus separate internal280 target contact**; QA F7/F9 are distinct, two-message verifier PASS. | Fresh local-only target-bound F7 **then F9 once on the same target**, F11: root ribbon/cross query/world/depth and separate contact fixed/360 owner pixels, probe ON/OFF, repeat/map and S21 same-frame comparison. F9 fixture is not GS hit authority; fixture raster is not caster/gameplay visual PASS. |
| Circle Shield full visual chain | NATIVE BITMAP GPU/ALLOCATOR QA BUILT / OWNER VISUAL OPEN | S21 buff216/221/222 branch `13ECB52..13ECFBC` proves five flare01 bones and four `firehik_mono01` subtype12 particles, two per UpperArm26/35. Source/emission verifiers PASS. New private QA Engine EEF727AE... hash-staged with E3EC7999... rollback and full Data verifier PASS; hidden PID10688 exit0 shows 64x64 native mono CPU/GPU equality, GL error0 and stable release. QA-only bounded owner F7/F11 records now distinguish ready/bones and 0..4 successful native particle indices plus FPS/distance/options; F11/map snapshots count `circleMono`/`circleContact`. | Fresh local-only Circle cast for actual emission result, buff receipt, paired shoulder pixels, cadence, subtype12 pool/repeat/map cleanup and matching S21 frames. GPU/allocator wiring does not prove owner effect visibility; separate buff-contact receiver NOT wired. |
| Obsidian full visual chain | IMPLEMENTED / QA OPEN | `OBSIDIAN_REVERSE.md`, 12 shared-helper cases and pinned renderer verifier | Second action317 mapping, F7 aura/rotation/ownership/lifetime and real pool/GPU acceptance |
| Magic Pin full visual chain | NATIVE FIXTURE/Calc_RenderObject/FLAT-DEPTH RASTER PASS / OWNER VISUAL FAIL | `MAGIC_PIN_REVERSE.md`; PID28160 stage0 skipped, PID36328 controlled QA reached two stage gates/all four caster BMD submits but owner still saw no foot triangle. Corrected package upload gives native VAOs; direct PID24324 and actual `Calc_RenderObject(false)` PID10200 fragments PASS. New private flat-depth PID3596 compares 16 Magic01/Root source-scale0.7 native draws, real depth-writing synthetic Z0 plane (readback1.0→0.554251 at a covered ground pixel) and positive samples in both passes/GL0/release stable1. This rules out entire model below a flat fixture plane, not real terrain/owner pixels. Current opt-in gameplay query covers both models but no F7 cast on candidate `89C8B7A3...`. | Actual owner foot/root GPU samples/world Z/angle/terrain depth/cull and fixed/360 gameplay pixels; intermittent narrow-window creation, third stage/subtype1 and internal281 target explosion remain unaccepted. Fixture raster cannot close visual FAIL or GS hit authority. |
| Clash full visual chain | IMPLEMENTED / OWNER VISUAL OPEN | `CLASH_REVERSE.md`,12 tick+6 pose tests,selected renderer/model-pipeline verifier; PID 23952 selected-target QA had 871 native mesh-submit successes and only alpha-zero calc skips | Stationary/moving target pixels, facing/push presentation, GPU/cache reuse and cleanup; no siege GameServer PASS inferred |
| Harsh Strike full visual chain | STATIC PASS | `HARSH_STRIKE_REVERSE.md`, native implementation | Bone 8/5 blur, child timing and cleanup acceptance |
| Shining Peak full visual chain | IMPLEMENTED / QA OPEN | `SHINING_PEAK_REVERSE.md`, `verify_pin_joint_contract.py`, `verify_pin_seed_texture.py`; subtype2/3/4, common seed geometry/history and selected renderer audited. Hash-pinned sweep identified 65 canonical direct `+A44` operands but none in selected body, complete renderer or called tail-shift; nearby reader owner-model0xAE4 gated. Selected update/tail have no indirect calls; all 48 renderer indirect calls resolve through mapped OpenGL IAT (`map_s21_joint_renderer_imports.py`), not a joint vtable. | External global pool/registration callbacks may still read `+A44`; mixed-pool seed pixels, four frame gates, particle/model parity and cleanup remain OPEN. Numeric xrefs/local call audit are not gameplay parity. |
| Wrath full visual chain | NATIVE MODEL GPU UPLOAD / GROUND PROBE BUILT / OWNER VISUAL OPEN | `WRATH_REVERSE.md`: source-proven ONE/ONE ground draw restores native blend; hash-pinned edge/material verifiers PASS. `wratha01` sole `alpha_line.jpg`, `wratha02` sole `dust01.tga`; corrected native package BMD VAOs/GPU and release hidden PASS. Old PID28160 ground recorded ONE/ONE/texture3207 but no material match/pixels. Current QA-only four-draw/five-point native ground pixel/fragment query and expected `brocken` texture comparison built/staged FB4C8E42...; source-bound verifier/Data stage PASS, no new owner F7. | Actual Wrath F7 owner square frame, bound-material match/ground fragment+pixel delta to attribute layer, buff recurrence/removal, pool reuse and S21 matching frames OPEN. Probe compilation and hidden model GPU upload are not visual parity or GS buff proof. |
| Breche full visual chain | OWNER RENDER PASS / FULL QA OPEN | Owner accepted corrected caster placement, yellow slash and ground spiral; prior staged QA PID 23952 logged 256/256 bounded native submissions and 64 action-release boundaries. Target-bearing C1:1E receive and early stale-packet rejection now build in isolated Bin, not restaged | S21 matching-frame parity, post-map movement/pool cleanup, repeat casts, target reaction and authoritative GS damage remain OPEN; no equipment-pass attribution |
| GameServer learning/cast/damage | OPEN/fail-closed | server catalog, dispatch guard, GameServer build, direct `D:\GameServer S21` data-tree verifier | Matching S21 GameServer handler evidence and recipient class |
| GameServer target/AOE/multi-hit/PvP | OPEN | client packet/target evidence is not server authority. The supplied protected x64 `IGC.GameServer2_R.exe` SHA256 134BAEBF... retains RSDS GUID30c7611f... age3 with embedded `Season 20\GameServer\Release` PDB path, but the supplied tree has no PDB; `verify_supplied_gs_codeview.py` PASS. This path does not establish actual skill-handler version. | Matching server handler evidence/PDB/runtime dump or an explicitly approved new authoritative contract; XML S21 and Circle client opcode75 are insufficient |
| Client x86 build | PASS | isolated normal and QA Engine hashes in state | Rebuild after any production-path edit |
| GameServer build | PASS | isolated GameServer hash in state | Build does not prove gameplay semantics |
| Isolated local runtime startup | LOCAL FOUR-STACK/TCP PASS / GAMEPLAY QA OPEN | Current 32-bit `GrowLancer_QA` ODBC DSN connected; verified private CS/DS/JS/GS PIDs14992/19932/23180/11656 each owned listeners 44412/55964/55974/55910 and CS/GS accepted 127.0.0.1 TCP. All four task-owned test PIDs were stopped after smoke; no VPS socket/client login/cast. Historical PID25016 CS greeting/server-list group0 evidence was narrower than GS login. Current QA launcher requires `-LocalServer` and guarded loopback ports. | Open only private stack/client for an actionable controlled cast; verify GS login/movement/cast, native new-class recipient, post-map pool cleanup and no VPS route. Transport alone is not GameServer skill PASS. |
| Owner visual comparison | PARTIAL | Owner accepted Breche's current 5.2 render; other skill visual acceptance and synchronized S21 matching-frame parity remain unproven | Complete per-skill frame/camera matrix in `RUNTIME_QA.md` against pinned native frames; do not promote Breche render approval to global visual PASS |
| Pool/lifetime cleanup | OPEN | Prior QA F11 pool snapshots; new QA-only map before/after native pool snapshots compiled in private Bin but not staged; Spin Step controller now retires its own style-1 blur before native slot reuse | Run isolated repeated casts/map travel and compare main/private effect plus blur draw/retirement records; no static build can establish live cleanup |
| Existing SS6 runtime regression | OPEN | normal client compiles; Grow Lancer IDs and resources isolated | In-game regression pass for representative SS6 attack/buff/projectile skills |

## Current external inputs needed

1. Provide a runtime memory dump or matching PDB for the supplied protected S21
   GameServer, or explicitly approve a new server behavior contract with its
   deviations documented.
2. No further recipient choice is pending: owner chose a NEW native Grow Lancer
   class, with isolated DK testing first. Class encoding, learning/authorization
   and persistence remain open engineering work; equipment and quests 1–3 are
   excluded from this skill gate;
   this is not a request to choose the class again.

## Latest isolated candidate

### Current versus historical candidates — audited2026-09-15

Current staged RuntimeQA candidate is
`6DE3A1FECF7A868F428A962851084470FA6F0687527CB575210A5F75740218BF`;
`verify_runtime_qa_stage.py` checked the private executable and complete Data.
Exact prior staged `2E69CECC...` rollback is retained. Hidden native
merged-player Spin pose ribbon PID17052 rasterized 114 nonblack pixels from
nondegenerate bone33 endpoints (minWidth181.108), GL0/stable release;
synthetic blur PID22320, Spin cross PID3532 and Magic Pin PID4448 regressions
exit0/PID-bound verifiers PASS. The earlier hidden PIDs are historical. A new
bounded Spin target-cross gameplay query is built, but no owner
gameplay cast has been performed on this candidate; fixture pixels are not
visual acceptance. The older body adapter candidate
`DD72E9056FC2A93EFDFD833F6157623A91FB9192D99E4552E30294889305D20C`
is historical, not the staged executable. Prior rollback candidates remain in
`RuntimeQA/Bin/`. Its adapter probe PID20404 exited0: all5 body loads/cache
repeats passed, bitmap count/bytes restored after model table destruction. The
latest native shader/body draw probe PID25532 also exited0 and is verified by
`verify_class_body_gpu_runtime.py`; clean sampler-conflict and missing-material
rollback fixtures pass as well. This is not gameplay activation
or class selector/evolution/equipment parity; broader class/runtime failure paths
remain separate.
Offline native geometry probe PID964 exited0 with10 load and10 release-return
records. This is NOT game startup, texture/render or gameplay acceptance.
Separate texture probe PID24084 exited0:3 native bitmap uploads, expected GPU
dimensions, glError0 and bitmap count/byte counters restored. No rendered-body
visual comparison or comprehensive leak claim follows from this probe.
Follow-up PID12168 exit0: all3 GPU readbacks match native CPU decoded buffers;
hair alpha0..255 preserved, missing file rejected, counters restored. This proves
texture upload fidelity on the observed NVIDIA context, not S21 render parity.
Candidate adds opt-in Magic40ms frame ownership/legacy exclusion and bounded
Wrath ground readback plus opt-in native body Open2/shader/draw probes. Modes are
OFF unless the isolated QA process explicitly opts in.
No fresh runtime/visual acceptance exists. Safe staging script refuses a live
target; latest exact-path process check returned no running target. The older candidate descriptions below are
historical checkpoints, not the currently running or newest built binary.

Audit correction: Shining's common initial-tail prefix, seed retention through
tick17, constructor/deletion Scale preservation and pin texture seed visibility
were already documented and verified. Their gap is mixed-pool runtime comparison,
not missing reverse of that prefix. Obsidian191/317 selection/suppression source
is also extensively recovered; mapping the auxiliary state into native5.2 remains
OPEN. Neither is a reason to invent an animation alias or initialize Scale anew.

### Historical implementation/build checkpoints

Latest server candidate additionally rejects271..281 at CheckSkillDelay before
RF bypass or timestamp mutation. Four source-order checks PASS; isolated Ex603
build/link exit0, SHA256
`C85CEF9419D6602D6E5921CF0306B287DD41CC8CC14E7318F6BFF709A32CF1C2`.
This supersedes the server hash below only; client candidate is unchanged.
No server deployment or live cooldown rejection test was performed.

Server-only update: early GL rejection now precedes wrapper combo/Nova handling,
duration broadcast and RunningSkill shield/invisibility changes. Both m_index
and m_skill are checked against the existing271..281 unimplemented range.
Contract range test2048 IDs and three source-order checks PASS; Ex603 Win32
isolated build/link PASS, SHA256
`87F0B1C284124E871C954AE847AA87660232CA483ACC5CB8602E44FD5F113D4E`.
No server was launched or deployed. Receiver cooldown ordering, master aliases
and live no-side-effect rejection remain separate OPEN checks. This is not
server-authoritative Grow Lancer skill completion and does not change client QA.

Spin corrections plus Circle emission/Shiny/child/sprite/joint corrections
compiled and linked; staged EXE SHA256
`90416CB2616CC265D650BBFDD1E62E596D0F785B21CAB90A059F31A617DFA18C`.
Includes Clash controller/child whole ticks, XYZ direction and rear initial-pose
corrections, plus missing-target retry;12 tick+6 pose cases PASS.
Includes Obsidian model whole ticks, integer-half alpha, unclamped aura alpha,
Thunder8 private particle updates and rotating/aura blend indices0/0/1.
12 shared-helper cases and pinned renderer verifier PASS; action317 and live QA OPEN.
Includes Wrath controller/aura/particle whole-tick corrections and exact life1
continuation gate;24 shared-helper cases PASS,real buff/runtime acceptance OPEN.
Includes MagicPin controller snapshot transform/ownership and whole-tick stage/hit
cadence; Thunder9/10, smoke19 and Shockwave2 subtype1 native particle corrections.
44 shared-helper cases PASS; actual pool/visual acceptance remains OPEN.
Includes four Magic Pin child tick/scalar corrections and exact01/root UV/blend arguments.
Includes Harsh controller integer ticks, gated bone sampling and private blur clock.
Includes Harsh wind01/02 whole ticks and wind01 fixed material RGB/zero UV.
Includes Shining shockwave seven-visit lifetime and two-pass mesh renderer,
and body/spin shared lighting preservation (pin-only Light*Alpha override).
Both legacy and optional shader spin mesh bounds verified statically.
Includes flareBlue5 and all three Shining mesh alpha/tick corrections.
Includes Xsuper six-frame pre-decrement ordering correction.
Also includes corrected Shining Peak pin-light2/3/4 constructor/update.
Includes Circle-only bypass of FPS MaxTails scaling after whole-tick migration;
S21 MaxTails stays10. Previous isolated candidate retained as a backup.
Stage verifier PASS2658 base files,79 overlay,3 root Data, EXE/Player hashes
and merged Player CRC. Spin helper tests cover40 cases across cross, flare,
particle and ground. None cover full engine pool allocation or visual parity.
Spin Step remains FAIL against the last owner observation until retested.

Local project integration audit PASS: all3 GrowLancer cpp and33 headers are
present exactly once in project and filters, without build exclusions.
This is NOT git merge evidence: the implementation remains dirty/untracked
in feature/grow-lancer-skills-s21. No merge into protected source/Pegasus.
Circle joint test PASS fourFPS factors,19 updates/8 turns and ordering;
real engine pool stress and GPU trail acceptance are still OPEN.

No build/static row upgrades any runtime or owner-acceptance row automatically.
