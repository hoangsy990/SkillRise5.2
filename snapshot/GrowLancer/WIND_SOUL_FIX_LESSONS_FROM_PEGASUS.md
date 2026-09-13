# Wind Soul fixes observed in the Pegasus worktree

Read-only source reviewed: `D:\RISE-CrossPlatform\Source_PC_Pegasus`.
No Pegasus or Wind Soul source was copied into this Grow Lancer package.

## Owner-reported failures after the initial port

- The three Wind Soul roots existed and moved, but the `desair` children rendered black.
- The persistent cyan smoke envelope was absent.
- The resulting trails were shorter and visually sparser than the Season 21 reference.

These were runtime visual failures even though the focused tests and isolated build passed.

## Fixes now present in Pegasus

1. `MODEL_DESAIR` subtype 2 has a skill-local luminous renderer. It copies the effect light to `BodyLight` and uses `RENDER_TEXTURE | RENDER_BRIGHT`. Legacy `MODEL_DESAIR` subtypes retain the old behavior.
2. Smoke subtype `0x50`/80 is constructed in the real `BITMAP_SMOKE` switch, not an adjacent bitmap family. Its Season 21 contract is lifetime 40 with randomized scale, rotation and gravity.
3. `dark_skill01.OZJ`, required by decoded `desair.bmd`, was added to the isolated hash-pinned asset manifest. A BMD alone was not a complete asset package.
4. Periodic smoke uses direct scheduled particle allocation. The previous FPS-checked wrapper dropped most scheduled puffs.
5. Homing movement uses the recovered per-step velocity increments `+0.4` and late `+0.1`, plus the unscaled post-rotation direction add. Applying another FPS factor shortened the effect.
6. The exact recovered lights are joint `(0.4, 0.6, 0.7)` and smoke `(0.2, 0.65, 0.75)`; they are contract constants rather than renderer guesses.
7. The local input path is send-only. The accepted server echo owns caster action, sound and exactly three visual roots, preventing duplicate roots.
8. The received target slot is carried directly into the roots and guarded for range/live state. It is not converted through a second key-to-slot lookup.
9. Root lifecycle is contract-pinned: subtype 4, scale 60, 12 tails, lifetime 35, initial velocity 10, target follow at Z+100, impact distance 100 and impact tick 5.
10. The isolated QA matrix currently records Wind Soul as owner-accepted for bird color, blend/smoke and stationary cast. This acceptance belongs to Wind Soul only and is not inherited by Grow Lancer.

## Rules carried into Grow Lancer

- Trace constructor, update and renderer switches separately; placing a subtype in the wrong native family can compile while producing no visible layer.
- Resolve every BMD mesh texture dependency and pin its hash before declaring asset coverage.
- Do not apply an extra FPS multiplier or FPS-throttled wrapper unless the source chain proves it.
- Keep local request ownership separate from authoritative receive ownership; each root and child must have exactly one creator.
- Verify submitted material light/render passes, not only the effect object's stored RGB.
- Treat focused tests and successful builds as static evidence only. Every Grow Lancer skill still needs isolated frame-matched owner QA and pool cleanup evidence.

## Mandatory Grow Lancer pre-runtime audit derived from those fixes

| Wind Soul regression | Grow Lancer check before owner QA | Current status |
|---|---|---|
| Child BMD rendered black although it existed | Check the final `BodyLight`, mesh pass and blend values submitted by every Grow Lancer model renderer; do not accept the color stored only on the effect object. | Required for all model children; no owner QA yet. |
| Smoke child never appeared because subtype 80 was initialized in the wrong bitmap switch | Match every new layer to the native constructor family that actually owns its bitmap/model ID, then confirm its update and render switch are both reachable. | The generic bitmap carrier is still provisional and must be audited layer by layer. |
| `desair.bmd` was staged without its embedded `dark_skill01.OZJ` | Decode and hash every BMD texture record, including indirect textures, before the asset manifest can be closed. | Asset provenance is hash-pinned, but every runtime-loaded dependency still needs loader evidence. |
| FPS-checked particle wrapper dropped scheduled smoke | Do not replace an exact per-tick/per-frame emission with `rand_fps_check` or another throttled wrapper. | Must be verified for Circle Shield, Shining Peak, Wrath and Magic Pin child emissions. |
| Movement was shortened by applying a second FPS factor | Record whether each recovered constant is per-update or time-normalized; apply `FPS_ANIMATION_FACTOR` exactly once and only when the mapped SS6 API requires it. | Must be checked against every moving Grow Lancer child before visual parity. |
| Local cast and accepted echo could both own roots | Local input sends only; the accepted receive/echo path owns action, sound and roots unless the recovered S21 chain explicitly proves another owner. | Receive dispatch is wired; duplicate-root runtime trace is still required. |
| Target identity risk from a second key-to-slot conversion | Carry the packet-resolved target slot directly, validate bounds/live state once, and keep the target index on the pooled root/child that needs it. | Implemented for target-bearing roots; moving-target QA remains open. |

This audit is a prevention checklist, not proof that the Grow Lancer implementation
already matches Season 21. Any row without runtime trace and frame-matched owner
comparison remains `OPEN`.

## Evidence locations

- `PEGASUS_IMPORT_STATE.md`, sections `Wind Soul bird color and smoke ownership correction`, `Wind Soul SS21 cast-motion correction`, and `Wind Soul 737 and combined ingame QA provisioning`.
- `PEGASUS_RUNTIME_QA15.md`, Wind Soul row.
- `ExMain_RISE_PC\Main5.2_RISE\ZzzObject.cpp`: skill-local `MODEL_DESAIR` subtype-2 render path.
- `ExMain_RISE_PC\Main5.2_RISE\ZzzEffectParticle.cpp`: `BITMAP_SMOKE` subtype-80 constructor.
- `ExMain_RISE_PC\Main5.2_RISE\ZzzEffectJoint.cpp`: subtype-4 constructor, target follow, smoke scheduling, movement and impact.
- `ExMain_RISE_PC\Main5.2_RISE\WSclient.cpp`: accepted receive path owns action/root creation.
- `ExMain_RISE_PC\Main5.2_RISE\RISE\WindSoulContract.h`: pinned runtime constants.
