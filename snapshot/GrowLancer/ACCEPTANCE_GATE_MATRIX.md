# Grow Lancer acceptance gate matrix

Overall status: `IN_PROCESS`.

Legend: `PASS` is supported by the named evidence; `STATIC PASS` has not been
accepted in gameplay; `OPEN` has no sufficient evidence; `BLOCKED` requires an
external input or explicit compatibility decision.

| Gate | Status | Authoritative evidence | Remaining proof/action |
|---|---|---|---|
| Exact base skill identity 271..279 | PASS | `grow_lancer_skills_descriptor.json`, `CLIENT_SERVER_CATALOG_DIFF.md` | Keep client/server version profiles separate |
| Grow Lancer class mapping | OPEN | Owner chose new native Grow Lancer; DK local QA first | Implement native class; numeric encoding and full server/persistence chain remain open |
| Scroll/item mapping | PARTIAL | decoded S21 `item.bmd`; seven mappings in descriptor | Circle Shield 272 and Clash 275 have no proven scroll/learn source |
| Costs, requirements, damage, range, delay | PASS as supplied-server catalog | pinned `SkillList.xml`, shared contract tests | Four rows differ from bundled S21 client; runtime server authority still unconnected |
| Regular formulas | STATIC PASS | pinned `RegularSkillCalc.lua`, focused server contract test | Connect to real attack calculation only after handler semantics are proven |
| Master-skill mapping | STATIC PASS | `MASTER_SKILL_REVERSE.md`, 13-row verifier | Runtime master dispatch disabled |
| Client local/receive dispatch | STATIC PASS | nine reverse files, `CLIENT_PACKET_REVERSE.md` | Gameplay receive path cannot run until protocol/class adapter exists |
| Skill packet field width | PASS for skill ID only | RISE two-byte skill fields and `WORD` skill persistence | Does not cover buff IDs |
| Buff ID delivery/storage | OPEN | `verify_buff_storage_contract.py`: BYTE GS/client fields and 13-byte legacy GS/DS records | Wrath424/425 need a scoped verified extension; do not widen slots or legacy records blindly |
| S21-to-RISE opcode translation | OPEN | S21 C1:35/C1:57/C1:59 proved; RISE 0x57 collision proved | Matching server packet handlers or an approved new native protocol contract |
| Actions/animation | STATIC PASS | S21 actions 185..194 appended as RISE 284..293; merged player hash | Owner gameplay timing/facing comparison |
| Models/textures/sounds | STATIC PASS | `asset_provenance.json`, 79-file overlay verifier; Breche root cache has zero meshes | Runtime loader/render confirmation for every skill |
| Direct S21 skill icons | PASS | `SKILL_ICON_REVERSE.md`, exact atlas hashes/UV verifier, owner F10 gameplay screenshot after 512-atlas divisor fix | Preserve isolated 512 atlas branch; legacy SS6 remains 256 |
| Spin Step full visual chain | FAIL | owner F7 screenshot shows only the ground ring; native frames 37..46 prove caster onset plus target-side vertical/contact phase | Rerun selected-target F7+F9 sequence; restore any remaining action/cross/blur/trail mismatch and compare matching frames |
| Circle Shield full visual chain | PARTIAL / QA OPEN | `CIRCLE_SHIELD_REVERSE.md`, cast corrections; subtype3 constructor/update compiled | Separate buff-contact receiver NOT wired; subtype3 lifecycle/pool/renderer and paired-target visual acceptance OPEN |
| Obsidian full visual chain | IMPLEMENTED / QA OPEN | `OBSIDIAN_REVERSE.md`, 12 shared-helper cases and pinned renderer verifier | Second action317 mapping, F7 aura/rotation/ownership/lifetime and real pool/GPU acceptance |
| Magic Pin full visual chain | IMPLEMENTED / QA OPEN | `MAGIC_PIN_REVERSE.md`, controller/Thunder/smoke/Shockwave corrections;44 shared-helper cases | Live frame-window sampling, common-prefix external consumers, real particle/effect pool reuse and matching-frame visual acceptance |
| Clash full visual chain | IMPLEMENTED / QA OPEN | `CLASH_REVERSE.md`,12 tick+6 pose tests,selected renderer verifier | Model-cache compatibility, selected stationary/moving target, facing/push presentation and cleanup |
| Harsh Strike full visual chain | STATIC PASS | `HARSH_STRIKE_REVERSE.md`, native implementation | Bone 8/5 blur, child timing and cleanup acceptance |
| Shining Peak full visual chain | IMPLEMENTED / QA OPEN | `SHINING_PEAK_REVERSE.md`; corrected8073 subtype2/3/4 versus incorrect legacy subtype0; helper12casesPASS | Pin field+A44/renderer audit, link/stage, four frame gates, particle/model parity and cleanup acceptance |
| Wrath full visual chain | IMPLEMENTED / QA OPEN | `WRATH_REVERSE.md`,24 shared-helper cases and selected aura renderer verifier | Real buff-owned recurrence/removal, owner/pool reuse and matching-frame visual acceptance |
| Breche full visual chain | IMPLEMENTED / QA OPEN | Full-dump root cache evidence; native target root/five children; shared ground math48-case test; receive279/F9 contact wired | Sprite visibility/FPS, terrain eligibility, pool reuse/exhaustion and matching-frame visual acceptance; no equipment-pass attribution |
| GameServer learning/cast/damage | OPEN/fail-closed | server catalog, dispatch guard, GameServer build | Matching S21 GameServer handler evidence and recipient class |
| GameServer target/AOE/multi-hit/PvP | OPEN | client packet/target evidence is not server authority | Matching S21 GameServer binary/PDB/dump |
| Client x86 build | PASS | isolated normal and QA Engine hashes in state | Rebuild after any production-path edit |
| GameServer build | PASS | isolated GameServer hash in state | Build does not prove gameplay semantics |
| Isolated runtime startup | PASS | owner confirmed corrected PID reached gameplay; Mix.bmd, merged-Player CRC and Enc1.dat/Dec2.dat gates all pass | Preserve this exact staged manifest for visual QA |
| Owner visual comparison | OPEN | native S21 batches and representative hashes in `S21_NATIVE_REFERENCE.md`; no accepted 5.2 parity rows | Complete the matrix in `RUNTIME_QA.md` against the pinned native frames |
| Pool/lifetime cleanup | OPEN | QA-only F11 telemetry compiled/staged | Snapshot immediately after and after documented lifetime for repeated casts |
| Existing SS6 runtime regression | OPEN | normal client compiles; Grow Lancer IDs and resources isolated | In-game regression pass for representative SS6 attack/buff/projectile skills |

## Current external inputs needed

1. Provide a runtime memory dump or matching PDB for the supplied protected S21
   GameServer, or explicitly approve a new server behavior contract with its
   deviations documented.
2. No further recipient choice is pending: owner chose a NEW native Grow Lancer
   class, with isolated DK testing first. Class encoding, learning/authorization,
   equipment and persistence implementation remain open engineering work;
   this is not a request to choose the class again.

## Latest isolated candidate

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

Local project integration audit PASS: all3 GrowLancer cpp and24 headers are
present exactly once in project and filters, without build exclusions.
This is NOT git merge evidence: the implementation remains dirty/untracked
in feature/grow-lancer-skills-s21. No merge into protected source/Pegasus.
Circle joint test PASS fourFPS factors,19 updates/8 turns and ordering;
real engine pool stress and GPU trail acceptance are still OPEN.

No build/static row upgrades any runtime or owner-acceptance row automatically.
