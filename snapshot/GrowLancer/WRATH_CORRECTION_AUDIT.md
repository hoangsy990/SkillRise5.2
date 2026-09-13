# Wrath correction audit — 2026-09-12

Status: IN_PROCESS. This supersedes the earlier Combo4 dependency and degree-sine claims.

The previous turn made implementation progress but introduced an incorrect cross-pool renderer attribution. Current dump inspection disproves it:

- Wrath calls particle allocator 0x1724176 for 0x81E4.
- Particle renderer comparisons 0x1621347..0x16213A7 do not select the OBJECT callback at 0x15AAA0A. The default at 0x1640DD2 submits the particle texture, stored light and rotation to 0x18E8B0C.
- The Combo4 substitution at 0x15AAA0A reads OBJECT fields, so it cannot establish the rendering of the particle allocated by Wrath. Its runtime registration and substitution have been removed. The staged copy is retained as an excluded lead in provenance.
- Helper 0x96A03C forwards its float input as double unchanged to 0x1A22CC0. The fallback 0x1A36D8F contains FSIN at 0x1A36DAD. Input 30 is radians. Particle scale adds sin(30)*0.18; aura subtype 0 adds -sin(30)*0.012 or -sin(30)*0.0009.

Current isolated x86 build passed: 935F3EE895904D65D806C0193CCD5B8D80CABD1DBCCD90441BF7FFE376DD5EB1. Runtime visual acceptance remains open.

Follow-up correction: the brocken adapter now accumulates normalized ticks in its private OBJECT Timer field and updates once per whole tick. S21 shared tail 0x1594624 checks expiration before the normal lifetime decrement at 0x15946E3; the adapter preserves that order, including the update at remaining life zero before destruction. The exceptions 0x413/0x7EF7 do not apply to 0x81E3.

Particle blocks 0x17095FD..0x1709744 and 0x1709749..0x17097C4 clamp incoming scale/alpha, not the result of subtraction. Corrected those branch orders. Fractional particle updates still require separate validation; this is not a claim of complete timing parity.

Arithmetic diagnostic only: 30 normalized ticks partitioned as factors 0.25, 0.5, 1 and 2 produced identical brocken recurrence results (life 0, alpha approximately -0.1, light approximately -1.0998867e-6). This did not run compiled C++, did not test the 31st-tick destructor, and is not runtime FPS or visual evidence.

Next: audit remaining pool-specific dispatchers and particle tick/lifetime ordering, and validate the compiled brocken destructor boundary. Do not restore the broad all-chains-verified claim based on build or staging checks.

Fresh aura audit: 0x1579435..0x1579471 and 0x1579731..0x157976D clamp incoming alpha to 1 only in the first phase, otherwise add the increment. Fade blocks 0x15794F9 and 0x15797C9 subtract without a local lower clamp before reaching shared tail 0x15945F1. The current adapter instead clamps the result to [0,1] after both phases. This remains a confirmed local-branch mismatch to correct after checking the shared tail for any additional alpha handling. Subtype-1 owner-position following is already implemented and matches 0x1579690..0x1579712. Its respawn position uses stack local [ebp-0x39c] at 0x15798CE, unlike subtype 0; that local still needs definition tracing before claiming identical respawn coordinates.

Stage verification for C30B082F: PASS, 2,658 base RISE files, 76 private overlay files, three root Data files, Engine/player hashes and private Player/RISE roots. No game was launched during this audit.

Follow-up implementation: shared tail 0x159463B..0x15946F0 only handles special types 0x413/0x7EF7, expiry and decrement; no alpha clamp is executed for 0x5EC. Corrected aura subtypes 0/1 to test incoming alpha >=1 during rising phase and removed post-update [0,1] clamping. This closes that branch mismatch at unit animation factor, not fractional timing or total lifetime parity. QA x86 build and complete isolated stage verifier PASS at 55F50A2A; existing cryptlib LNK4099 warnings only. No runtime was launched.

Respawn tracing remains open: function entry candidate 0x14B76C0 was decoded from its prologue through the initial dispatcher; common pre-dispatch code initializes [ebp-0x2c4] and owner-ground position but does not establish [ebp-0x39c]. Byte searches for that displacement found 752 candidates across the large function region, including unrelated particle/bone branches. They are not definitions on Wrath's executed path. Next follow the exact 0x5EC dispatch path and determine whether that local has a reaching definition; do not substitute a bone position from an unrelated branch.

## Two-stage dispatch and respawn argument audit

- Primary table: 0x14B7B12 subtracts 0x4D3; 0x5EC gives index 0x119. Byte at 0x1594BE0+0x119 is 0x5C, whose dword entry at 0x1594A6C+4*0x5C points to 0x1574472 (not an unrelated bone/particle branch).
- After the shared exclusion/model-animation path, 0x15748C1 reloads object type. Comparisons select 0x1574A79, subtracting 0x5E1; entry 11 at 0x159568C points to 0x157939E. The small verifier checks these two table entries only.
- No direct assignment to [ebp-0x39c] was found in the decoded common path or subtype-1 branch. Calls and conditional paths still require a complete reaching-definition audit before labeling the source local uninitialized. Do not claim stack residue as proven behavior.
- At 0x15798B0, the new aura receives current object's light vector (+0xEC), not the fixed subtype-0 transition light. Helper 0xD2DFF6 returns its input this pointer unchanged. Current adapter resets transition light for both subtypes; this is another argument mismatch, although it may be visually neutral when light is unchanged.
- Constructor subtype 1 at 0x1480F07..0x1480F79 sets lifetime/scale/alpha/UV, with alpha zero; it does not locally replace the supplied position. Update 0x1579690..0x1579712 follows owner position. Whether this hides the unresolved initial position depends on allocator/first-render order and must be verified, not assumed.

Next: trace shared CreateEffect argument-copy and first-render order for subtype-1 respawn; preserve current source until that evidence distinguishes a real visual requirement from a source-local artifact. Correct inherited-light argument separately with focused regression coverage.

Inherited-light correction implemented: recurring subtype 1 copies effect.Light; initial subtype 0 retains the fixed (0.65,0.75,1) transition. Braces are required around native VectorCopy/Vector macros: the first compile caught C2181 due to their multi-statement expansion, corrected before the successful x86 rebuild. No coordinate change was made. Compiled gameplay regression and visual acceptance remain pending.

Allocator tracing started at exact wrapper entry 0x143E57C: it forwards position argument [ebp+0xC] to inner allocator 0x143E5F0 at [ebp+0x10]. The inner allocator searches the pool and handles allocation registration before copying arguments. Continuation from 0x143E719 verifies light copy 0x143E9EF..0x143EA4C, angle copy 0x143EA4E..0x143EAAB and all three position coordinates at 0x143EAAD..0x143EB0A into object +0x158. Thus the constructor receives the supplied coordinates; this is not evidence that the unresolved caller local equals owner position. Next follow constructor tail 0x14B2E0C and update/render scheduling. Full stage verifier PASS at A7A25588; no live QA in this run.

## Pool scheduling evidence

- Constructor tail 0x14B2E0C returns the allocated object; 0x14B2E1B..0x14B2E33 restores the frame and returns. There is no immediate MoveEffect call on this tail.
- Allocation selectors 0x10EF919 and 0x10EFBA1 both return zero for type 0x5EC. First selector reaches 0x10EFB47; the second either exits early or follows 0x10EFC85..0x10EFCCF to 0x10EFDF6. Thus this aura stays in the ordinary pool rather than the alternate manager.
- Ordinary updater 0x1595834 loops ascending indexes at 0x159589B..0x15958DA, tests each live byte at visit time, and calls MoveEffect at 0x15958D3. Limit is 200 normally or 150 under its external predicate; do not import these capacities into SS6.
- Allocation scans from index zero (0x143E627..0x143E6AA). Inference from these two loops: a newly allocated slot later than the current index can be updated within the same sweep; an earlier slot has already been visited. Therefore the claim that owner-follow ALWAYS precedes first render is not established. Alpha-zero construction alone is not proof of render/culling equivalence for unresolved coordinates.
- Direct rel32 candidates for ordinary updater: 0x18F3C30, 0x18F3D6A and 0x18F4158. They still need caller-boundary/control-flow validation; these numeric references are not completed render-order evidence.

Next: decode the caller paths around those updater calls and establish render boundaries. No new runtime or source change is claimed for this scheduling audit; current build remains A7A25588.

Follow-up caller validation: decoded from prologues 0x18F2EBF, 0x18F3CE9 and 0x18F40BE; all three listed calls really invoke 0x1595834 with ordinary-pool base 0xA5B14F0. Adjacent 0x1618EF4 is another pool sweep and 0x1620972 wraps 0x16AA2ED, so adjacency alone does not prove rendering occurs there. Full frame ordering is still open.

Renderer correction discovered during this audit: aura subtype 0 at 0xA49676 and subtype 1 at 0xA4978B push blend-mesh index 0 before the alpha/render-flags/mesh arguments to 0x968CB3. The adapter had retained -2. Corrected only aura to 0, retaining travel's existing value pending its separate audit. Flags stay 0x42 and per-subtype light calculation is unchanged. Isolated x86 build PASS at 7F9BEAAC, with existing cryptlib warnings; not visual acceptance.

Travel renderer audit completed at callback scope: full 0xA49422..0xA49553 fixes RGB from floats 0x1B502A8=0.37, 0x1B502CC=0.57 and 0x1B4DD48=1, multiplied by alpha. It never reads object.Light for these channels. Pushes at 0xA49504..0xA49511 are zero UV, and 0xA49527 is blend-mesh zero. Corrected travel's previous inherited RGB/-2/object-UV assumptions; aura behavior retained. QA x86 build PASS at EB3B10C7, existing cryptlib warnings only. This callback audit does not close scheduling, respawn coordinates or live visual acceptance.

Travel update audit: complete 0x1579227..0x1579399 subtracts alpha 0.065 (float VA 0x1B9E708), rotates local (0,0,-65) (Z float VA 0x1B76090), adds all three components to position, then jumps directly to the shared lifetime tail. There is no local or shared alpha clamp for 0x5EB. Removed the adapter's lower clamp. Travel now initializes Timer=0 and shares the existing isolated whole-tick update/expiry path with brocken, preserving the update at life zero before destruction. This changes only those two private effect types; actual wall-clock cadence, render-negative-alpha behavior and pool reuse still need runtime QA. Build/stage hash 65364DC5; existing linker warnings only.
