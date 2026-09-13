# Pegasus full-chain re-audit

## Latest runtime update: QA5544 (2026-09-13 08:17..08:20)

New aura-live-guard build6B4317D8D88E1DB700C0FCCAFBE202C2CFEA3C8F4341F34909D4B602FB8E0E72
ran79dispatches, all required renders, hardfail0, BMD9/9, buff5/5active+clear,
damage6/7, reaction6/7,45captures, Elemental5/5 children, dynamic1837/1837,
SwordBlow601/601. All final pools0; no allocation failures this run.
Analyzer exit1: Bird damage/reaction and disabled sound still fail.
Connected server declaration remains unavailable. Source guard change does
not explain randomized shock counts. All12 remain IN_PROCESS. This confirms
alive-owner regressions only, not dead-owner arrival or proper-class parity.
The QA21152 details below are prior-run diagnostic evidence, not latest build.

## Current acceptance boundary (2026-09-13, after QA21152)

Latest completed QA21152: 79dispatches, all required renderer rows, BMD9/9,
45 standard images plus12 Deathside cycle images, buffs5/5active+clear,
damage6/7, reaction4/7, no allocation failures, finalpools0. This does not
erase transient Chaos particle exhaustion (228failures) from QA8352.
Proper-class/equipment, full original visual timing, interruptions/recast
and source-exact server stat formulas remain separate requirements.

Connected server revision is NOT established by the local GameServer build.
Its repeated missing Bird damage cannot independently prove an old binary.
The local target-heading fix now passes actual-route/geometry fixture tests;
new QA server declaration is self-reported and has not yet been received live.
Audio is disabled in the tested client settings; no audibility acceptance.

Bounded re-analysis of QA21152 explains the two missing native shocks:
Havoc2013 and Blow1501 each logged ten NATIVE_RANDOM_GATE observations,
with zero APPLIED. Bird241 had neither APPLIED nor gate evidence and no
positive damage. Gate counts are packet observations, not cast probabilities.
The analyzer now reports these reasons while retaining all missing-reaction
failures; its fixture explicitly guards against treating suppression as PASS.

The combined executable and 173 assets in both RuntimeClient and Delivery
were reverified against the build. SHA256:
DFC4B0B6744EA07B7CCA96ECBF8B2AE13E95B9A7F9CED8BC0FC69E440C6B8164.
This establishes isolated staging consistency, not VPS deployment or full
proper-class visual parity. Havoc small/large second passes were both seen
in actual shader execution; Dragon yaw/layer-color changes are included.

Deathside later-cycle images now show broad translucent radial ribbons;
early three-frame samples missed their wider phase. Spearstorm embedded
blades remain visible during fade. These are narrow defect checks, not full
parity of every variant. All original12 plus extras remain IN_PROCESS.

## Official charge cross-check (2026-09-13)

Webzen's [Season 16 MG renewal guide](https://muonline.webzen.com/en/gameinfo/guide/detail/244)
independently describes Strength-based ATK/DEF for Spiral, Dexterity-based
speed/DEF for Crusher, and Energy-based wizardry/DEF for Elemental. It also
describes death/logout removal and an additional explosion every third
eligible attack. The guide gives no exact stat coefficients or explosion
damage formula. It is corroboration, not proof that this pinned Pegasus
revision uses identical balancing or protocols.

Current server charge contract explicitly covers weapon gates and the
XML-proven absolute ranges only. EffectManager seeds markers with unset
values; presence/absence and range tests must not stand in for those stat
effects or the third-attack damage mechanic. These remain open integration
requirements, not optional polish. Do not import private-server formulas
or add arbitrary damage to make the QA counts pass.

## Historical runtime evidence, QA16660 (2026-09-13)

Native Sword Wrath body pass is now source-traced via shared renderer and
decoded original config; see PEGASUS_SWORD_WRATH_REGISTRY_AUDIT.md. Isolated
build3DF02FA06A390307400CE677AA86A2F01A529873FA45C9FC6E1DC2755E5EA433
completed real server QA05:14:11; pools0 at05:14:26. Required renderer rows
all present, hardfail0, animated BMD9/9, buff5/5active+clear, captures45/45,
dynamic1835/1835 and SwordBlow585/585 lifecycle balanced. Wrath root created,
rendered and released; inspected phase1 remains visible after native pass.

Overall verdict remains FAIL/IN_PROCESS: damage6/7 (Bird missing on connected
GS; its binary revision unverified), native reaction4/7 (Raining/Blow/Bird missing), DirectSound0/15 disabled.
These data supersede older pool/removal observations below only for this
single harness run. They do not establish source visual parity, correct
class/equipment coverage, audibility or all lifetime/interruption variants.
Sword Blow's original motion approval does not close its later tail-color
comparison request; neither Fly nor recent visual deltas inherit approval.

This audit supersedes any earlier interpretation that a packet dispatch plus a
visible root proves a completed skill.  The reference is the pinned Pegasus
client `D:\Pegasus\MU-x86.exe` and its `Data`; SS21 is used only as a later
visual cross-check where requested.  Every row remains `IN_PROCESS` until the
complete runtime chain is observed.

## Player action and BMD evidence

Pegasus `Data\Player\player.bmd` has 60 bones and 287 actions.  The native RISE
carrier has the same 60 bones and 284 actions.  All first 284 animation
payloads match byte-for-byte at the same indices; Pegasus adds actions
284..286.  The pinned Pegasus `OpenPlayers` loop at `0x87B127..0x87B147`
assigns play speed `0.4` through action 286.

| Skill | Pegasus action evidence | Current verdict |
|---|---|---|
| Deathside 288 | first cast 172..175 by mount; repeat command 185 (`0x6604A3..0x660555`) | corrected to exact action family; live QA selected 172 then 185 and both frames progressed |
| Darkness 289 | fixed action 145 | live QA selected 145 and frame progressed |
| Raining Arrow 2023 | fixed action 286 (`0x670C50..0x670C68`) | exact 287-action Pegasus carrier is isolated/hash-staged; live QA selected 286 and frame progressed |
| Dex Booster 2024 | fixed action 67 (`0x661C42..0x661C4F`) | live QA selected 67 and frame progressed |
| Chaos Blade 2012 | action 285 when present, fallback 183 (`0x654B40..0x654B56`) | PID20372 selected 285 and progressed to frame 10.241 at 875ms; visual/speed parity remains open |
| Havoc Spear 2013 | action 284 when present, fallback 183 (`0x666CF5..0x666D0B`) | QA24588 at02:53:34 selected=actual284, speed.4, frames.327/1.326/2.493; action advancement confirmed, full timing/visual parity open |
| Spiral Charge 2014 | default67; outside safe zone mounts select157/158/159 (676F46..676F80) | unmounted QA selected67; mounted selector restored, mounted runtime still open |
| Crusher Charge 2015 | fixed action 67 (`0x65AFD3..0x65AFEC`) | live QA selected 67 and frame progressed |
| Elemental Charge 2016 | fixed action 67 (`0x664CAB..0x664CC9`) | live QA selected 67 and frame progressed |
| Sword Wrath 1500 | fixed action 67 | live QA selected 67 and frame progressed |
| Sword Blow 1501 | fixed action 71 | exact BMD payload exists and owner accepted original motion |
| Shining Bird 241 | Pegasus common equipment-aware attack selector | live QA selected native branch 39 and frame progressed; no fixed action is claimed |
| Dragon Violent 242 | fixed action 253 | live QA selected 253 and frame progressed |
| Spearstorm 243 | fixed action 80 | live QA selected 80 and frame progressed |

## Full-chain gaps still requiring direct Pegasus comparison

2026-09-13 runtime addendum: QA12480/skill241 captures at156/359/562ms
show bird approach, target crossing and departure, with smoke/stars/streaks.
The run ended03:06:22 with all three pool counters0. This verifies visible
BMD movement and cleanup in the local harness only. The harness now sets
Hero facing before injected dispatch; older stale-yaw captures cannot prove
directional parity. Equipment-selected caster animation still needs matching
class/equipment reference QA; do not infer it from one DK test character.

| Skill | Open evidence that blocks completion |
|---|---|
| Deathside | mount-aware summon/repeat actions, randomized Reaper attack variants 3/4/8 with matching sounds/scythes, BMD advancement/reset, direct/mirrored bone trails, owner follow, three particle cadences, ten-second dismissal and meshes 0/1/2/4 are routed; server uses native Summoner curse damage and native damage packets connect victim reaction; exact Darkness magnitude plus fresh visual timing of all variants/dismissal and final-pool QA remain open |
| Darkness | the complete `0x65B148..0x65BBC5` handler proves action 145 and marker state but no model/particle/joint/sound; the negative verifier prevents invented visuals, GameServer applies the collision-safe marker and Deathside consumes the paired curse-damage family; exact Curse/Defense formula, live action cadence, buff icon/lifetime/removal and final-pool QA remain open |
| Raining Arrow | action 286, exact volley launch/pitch, six ground textures, Cratered base, four timed impact overlays, two Stone fragments, ring08 light, native arrow-joint angle/cleanup and first-hit sound latch are routed; authoritative multi-target packets create target-follow damage sheets and native damage packets connect victim reaction; root/follow retain pointer-only ownership while launched children finish independently; `/qa15` still uses an isolated no-bow projectile fallback, so equipped-bow branch, live terrain-impact/visual cadence and final-pool evidence remain open |
| Dex Booster | action 67 and client `0x66110F..0x661C20` match Alpha-based aura/ground fade, 10/5/15 ring rotation, random owner-bone flares, opposing 0.08 sprite rotation, twenty Impack03 particles and nineteen subtype-48 spherical flare joints; the server applies the source-backed 300-second marker duration; exact speed/success stat formula, live visual cadence, buff expiry/icon and final-pool QA remain open |
| Chaos Blade | action 285 (183 fallback only), fixed cast-time bone-17 root/arc/crack cascade, five-layer target impact, meshless sword bone-sampled ribbons, ground renderer and authoritative multi-target list are routed; native damage packets connect victim reaction, and only the proven Hit-1 sound is used; server selection now uses physical damage instead of the MG wizard fallback; live visual/damage cadence, genuine every-third-hit protocol/magnitude and final-pool QA remain open |
| Havoc Spear | action 284 (183 fallback only), all six model routes, radial joints, line projectile, tick-1 and odd-tick Nova/explosion children, randomized impact lightning, target-follow orchestrator and per-model render/playback branches are source-routed; updater matches `0x665F42..0x66636A`: no global live gate, root stays fixed while helpers use current owner position and visual children finish their authored lifetimes; live visual cadence, target reaction, authoritative every-third-Havoc damage and final-pool QA remain open |
| Spiral Charge | action 67, animated root, flare/four child emitters, nine bone joints, steam/smoke particles and persistent 17-bone anchor are routed; all six models require a live owner, only the anchor follows/checks marker 169, and packet removal deletes it; the server enforces the two-handed-sword gate and authoritative marker route; exact server stats, live visual timing/removal and final-pool QA remain open |
| Crusher Charge | action 67, intentionally transparent `0x27AE` carrier, four-child timed ring cascade, exact visible mesh-0 passes and persistent two-ring 17-bone aura are routed; all models require a live owner, persistent aura follows owner and packet removal at `0x65B129` is authoritative without a local timeout; the server enforces the one-handed-sword gate and authoritative marker route; exact server stats, live visual timing/removal and final-pool QA remain open |
| Elemental Charge | caster action, three flare plus two shockwave constructor children, `05_buff_obj.bmd` frame advancement and the exact five-bone sprite/mesh passes are source-routed (`0x6647B7..0x664C42`); root/children use the exact owner gate and packet-owned buff removal is authoritative; the server enforces the source/official staff gate; live five-layer visual cadence, exact server stats, every-third-Havoc behavior, removal and final-pool QA remain open |
| Sword Wrath | this is an authoritative self-buff, so an enemy target-reaction gate is not applicable; GameServer applies native-safe marker 168 to the caster and echoes the cast, while the client routes owner-bone-17 smoke/pin-star accents, four ground/ring subtypes and thirty randomized line joints; updater matches `0x67AB1D..0x67AD8F`, and joint/particle initialization, movement, fade and authored lifetimes are explicitly gated; live buff application/removal cadence and final-pool cleanup remain open |
| Sword Blow | original motion approval is scoped to that earlier version; later SS21 tail-color request remains open. Background-subtracted tint transfer has measured evidence but is not a recovered SS21 material constant. Fly is separate and cannot inherit original approval; repeated cast, interruption and final reference comparison remain required |
| Shining Bird | equipment-selected caster action, directional server hit and non-looping `lightofbird_attack.bmd` playback are routed; the incorrect caster owner-live truncation was removed against `0x673781..0x673808`, leaving BMD completion as cleanup; the source-proven caster routine has no target-effect argument, so target reaction remains native RISE's authoritative damage packet (`ReceiveAttackDamage -> SetPlayerShock`) after the server's directional 200 ms hit; live timing, direction, visuals and pool reuse remain open |
| Dragon Violent | action 253, eight-point immediate/delayed radial chain, head JointSpirit02, crack-to-wind/layer children, target-follow end/combo/impact particles and authoritative multi-target list are routed; updater ownership matches `0x662A37..0x663006`, and native damage packets connect victim reaction through `SetPlayerShock`; live radial/impact timing, damage cadence and final-pool QA remain open |
| Spearstorm | action 80, five-tick falling-blade fan, two blade trails, forced impact, crack/stone/smoke children, expanding render layers and lifetime-64 sound latch are source-routed; the server preserves double hit plus the evidenced 5% third instance and echoes the target-centred cast, while native damage packets connect victim reaction; no global owner-live kill truncates the authored lifetimes; live collision/impact cadence, sound synchronization and final-pool QA remain open |

## Direct child-BMD playback audit

Pegasus custom effect models are outside the native RISE generic animation
range.  Therefore a loaded/rendered BMD is not evidence that its authored
action advances.  Only direct calls recovered from the pinned Pegasus image
are reproduced; static one-key carriers are not animated by assumption.

| Pegasus route | Carrier | Pegasus playback evidence | Isolated result |
|---|---|---|---|
| Deathside `0x65FA60..0x65FB27` | Reaper plus three scythe carriers | `max(Action.PlaySpeed, 0.02)`; completed attack actions reset to action 0 | implemented; live frames progressed for Reaper `10175` and scythe carrier `10177` |
| Deathside `0x65FD13..0x65FD57` | `ReaperHead.bmd` / `0x278C` | whole-executable immediate scan finds only the owner-cleanup call at `0x65FD48`; no creation xref | do not invent an additional head child; keep model load/cleanup addressable |
| Elemental Charge `0x664ABA..0x664B27` | `05_buff_obj.bmd` | effect velocity before bone transforms/five mesh passes | implemented; live frame progressed for model `10153` |
| Havoc Spear `0x66649F`, `0x666567`, `0x666684` | end/default, shockwave, `Magum_line.bmd` | speeds `0`, `0.3`, `1.0` respectively | only the two non-zero routes require advancement; live line frame progressed for model `10155` |
| Sword Blow `0x6795EE..0x67965A` | all `0x27B5..0x27BD` model carriers | effect velocity divided by `6.0` | implemented; multishot carriers are no longer left at frame 0 |

## Newly proven missing Pegasus layers

- Elemental Charge constructor `0x66481B..0x664949` does more than initialize
  the persistent `0x27B2` BMD. For subtype 0 it creates five child effects:
  three `0x7F20` (`Effect\\flare03.jpg`) at scales `3, 3, 4`, followed by two
  `0x7F21` (`Effect\\Shockwave.jpg`) subtypes 1 at scale `1.5`.
- Exact ABI/call-site recovery proves `0x72FC7D` is Pegasus `CreateEffect`, not
  `CreateJoint`. The isolated adapter now maps the two source roles to private,
  collision-tested texture IDs, creates all five carriers in source order and
  reproduces their `0x664771..0x664AB7` lifetime, growth, alpha and sprite
  emission fields. Runtime allocation/destruction and final-pool evidence are
  still required before this child chain is accepted.
- Runtime SHA-256
  `410F3C0EBCA6D85F09299C0574DEF941D3689B8A212F1887A3539686AF097192`
  completed one clean bounded QA sequence: all 15 dispatches, all 14 Pegasus
  caster frames progressed, 207 dynamic allocations, zero pool exhaustion and
  `AUTO_QA_POOL 0/0 CLEAN`. Elemental produced exactly five child-effect
  allocations and five matching destructor events. Its boolean renderer trace
  cannot classify the historical `FAIL` rows: native `Calc_RenderObject`
  returns false for alpha below `0.01`, including valid fade boundaries. Build
  `C38585F08069F843B7A2802CA234B21249BB4BC73082606B5E2826005761B2AD`
  now separates `TRANSPARENT_SKIP` from non-transparent hard `FAIL`; a fresh
  authenticated `/qa15` run is required before those old rows may be closed or
  treated as real renderer defects.

The bounded live `/qa15` run at `2026-09-11 13:18:00..13:19:23` closed only
the player-action gate: all fourteen Pegasus caster samples selected the
source-proven action/branch and produced `CAST_ACTION_FRAME ... PROGRESSED`;
the delayed pool snapshot was clean.  This does not close root/child visual
parity.  In particular, a late `EFFECT_RENDER ... FAIL` is not automatically
classified as a missing renderer when the same model previously rendered
`PASS`; the Pegasus lifetime and owner branch must be decoded before changing
code.

## Strengthened next-run acceptance - 2026-09-12

- Every timed QA sample must now receive `SKILL_LIST_ADD` and then report
  `SKILL_LIST_STATE ... PRESENT` from the client array. This covers all fourteen
  catalog skills plus standalone Wind Soul and rejects the former false-positive
  case where the server notice said a skill was available but the client could
  not select it.
- Dex Booster, Spiral Charge, Crusher Charge, Elemental Charge and Sword Wrath
  must each report post-mutation `BUFF_STATE ... ACTIVE`, followed by
  `BUFF_STATE ... CLEARED` during bounded QA cleanup. Packet receipt alone no
  longer passes buff ownership.
- Item 15:102..113 learning is locked to the native `CharacterUseScroll` path:
  class/stat requirement check, catalog-backed `GetSkillNumber`, `AddSkill`,
  then `GCSkillAddSend`. This is source evidence; actual item consumption still
  requires an authenticated isolated gameplay sample.
- Re-audited every staged BMD key count after the earlier standing-model report.
  Multi-key payloads are not animated merely because they contain keys. The
  pinned Pegasus renderer for Raining Arrow (`0x6715F0..0x671984`), for example,
  calls the ordinary transform/draw route for its ring models without a direct
  `PlayAnimation` call. Runtime advancement remains required only for carriers
  whose pinned update/render blocks actually call playback; inventing playback
  for other models would diverge from Pegasus.
- Corrected the framebuffer scope from twelve to the complete one-run set:
  Deathside, Darkness, item rows 15:102..113 and Wind Soul. The analyzer and
  generated contact sheet now require 45 decoded frames instead of 36.
- Closed a separate Deathside analyzer omission: the next run must prove the
  visible Reaper renderer and BMD progression, appear/disappear and one real
  randomized attack buffer at DirectSound, offset-47 lifecycle parity, and
  lifecycle parity for whichever meshless scythe offsets 49..51 were actually
  selected. Offset 48 is not fabricated into the gate because the pinned
  executable exposes no creation xref for that loaded head asset.
- Final ownership now covers more than the `Effects` pool. The isolated client
  also scans live joints/particles in the private Pegasus texture namespace and
  Wind Soul's exact native two-line subtype after the bounded cleanup delay.
  `AUTO_QA_POOL_CHILDREN 0/0 CLEAN` plus one explicit sequence-complete row are
  mandatory; ambient native effects outside those source-identifiable routes
  are deliberately not mislabeled as Pegasus leaks.
- Child ownership is now slot-based at allocation time, including Pegasus
  branches that reuse native lightning/light/explosion textures. A later slot
  reuse clears the ownership tag. The analyzer requires real joint and particle
  allocations, rejects their pool-full paths, then requires zero tagged live
  slots at the delayed snapshot; an empty child implementation can no longer
  satisfy cleanup merely by producing nothing.
- The slot classifier uses loaded handles for Deathside and Sword Wrath rather
  than assuming those assets fall in a fixed ID range. Chaos Blade's shared
  native joint is classified by type, subtype and the skill ID supplied at its
  existing constructor boundary. This closes the dynamic-texture/native-ID
  holes while preserving the original render/update semantics.
- Cast-only runs now establish their own post-authentication run boundary after
  a live hero joins the map. The analyzer selects the newest
  `AUTO_QA_LOGIN`/`AUTO_QA_RUN` ARMED row. This preserves manual authentication
  while preventing both the former guaranteed cast-only rejection and stale
  prior-process evidence from entering the bounded result.
- A positive server damage packet is no longer accepted as proof of target
  reaction. The next run must record the actual native `SetPlayerShock` branch
  for each of Raining Arrow, Chaos Blade, Havoc Spear, Sword Blow, Shining Bird,
  Dragon Violent and Spearstorm. This will also expose an unsuitable QA target
  such as native monster class 275 when its non-success reaction path is
  intentionally suppressed.
- `/qa15` now prevents that unsuitable-target case at the source: its isolated
  selector applies the same native target/range/radio checks but excludes class
  275, falls back to eight class-0 QA monsters, and logs the chosen
  `target_class` on every submission. The focused safety test, isolated server
  build and full source-chain suite pass; runtime reaction and visual evidence
  remain open.
- The reaction sample is no longer a single randomized attempt. Each of the
  seven attacking requested skills receives ten unmodified native casts, while
  same-skill framebuffer scheduling remains one three-phase window. The
  analyzer requires all ten dispatch/action samples and at least one actual
  `SetPlayerShock`; neither damage flags nor client reaction behavior are
  forced by the harness.
- Repeated casts are separated by 1250 ms. This is later than the third
  framebuffer phase and the +900 ms action sample, preventing a subsequent
  root from contaminating either piece of per-cast evidence.
