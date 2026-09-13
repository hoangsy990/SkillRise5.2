# Wrath native pool lifecycle audit

Status: IN_PROCESS. Static inspection, not live leak/cleanup acceptance.

## S21 destructor

`0x14B6820..0x14B6925` optionally cleans a buffer when byte +0xD is one,
then dispatches type-specific cleanup and calls object reset `0x1315E97`.
Types 0x5EB and 0x81E3 take no listed type-specific cleanup branch.
`0x13163FB..0x131642F` passes non-null +0x318 to `0x19B9043` and clears it.
Do not transplant this offset or assume identical allocation ownership in SS6.
Reset begins by clearing live and other state bytes; this is more than merely
clearing the owner reference.

## Native SS6 recipient

`ZzzEffect.cpp::EffectDestructor` has two special cleanup cases, then sets
Live=false and Owner=NULL. It does not reset Timer or all bone/render fields.
`CreateEffect` scans dead slots, restores common parameters and vectors, calls
the private InitializeEffect and returns. Both private brocken/travel constructors
now explicitly initialize Timer=0, so their tick remainder is not inherited.

Private GrowLancerEffectRuntime does not allocate per-effect BoneTransform
storage. However, `ZzzObject.cpp` chooses object.BoneTransform versus the shared
BoneTransform buffer based on EnableBoneMatrix. The creation/destruction paths
inspected here do not reset that flag. This is a reuse risk to investigate, not
a demonstrated leak/crash and not authority to free an unknown pointer.

Native SkillEffectMgr admission is owner-gated and enumerates legacy types;
the private path must remain separate from changes to those global admissions.

## Next

Inspect OBJECT constructor and all EnableBoneMatrix writers, including pool
initialization, to determine whether a reused ordinary effect slot can carry
owned bone storage. Then test repeated allocation/destruction in the isolated
client. The compiled tick harness still covers callback ordering only; it does
not execute native destructor internals, BMD loading, or bone memory release.

## Follow-up source inspection

OBJECT constructor calls Initialize (`w_ObjectInfo.cpp:80`), which explicitly
sets EnableBoneMatrix=false and BoneTransform=NULL. The ordinary Effects array
is an OBJECT array. The only explicit true writers found by source search are
CreateCharacterPointer (its argument is CHARACTER, object is c->Object) and
CSPetSystem::CreatePetPointer (m_PetCharacter.Object); neither is an ordinary
Effects slot. No bone-pointer allocation/assignment or whole-object assignment
was found in ZzzEffect.cpp. Thus a stale owned-bone condition is not demonstrated
for this pool; no speculative reset/free is warranted. This remains static
source evidence, not a proof against every indirect write or runtime corruption.

QA coverage correction: IsGrowLancerParticle omitted the newly native Wrath
Combo and AlphaLight IDs (32940/32941). Added both to the existing pool snapshot
classifier. Previous glParticles=0 readings cannot prove those nodes were gone.
The metric still is not full skill ownership accounting: shared native particles
may serve several skills, and sampling can miss activity between snapshots.
QA x86 build/stage updated to CADBD594; no new gameplay acceptance.

Shared-particle coverage follow-up: package calls also create BITMAP_ENERGY
(Magic Pin/Obsidian) and BITMAP_CLUD64 (Magic Pin/Shining Peak). Added separate
sharedEnergy/sharedCloud live counts to QA snapshots, not glParticles, since
other native skills can create the same textures. Snapshot baselines must be
captured in a quiet scene before cast and after expiration. Counts do not
attribute individual allocations to the cast, and do not cover sprite/blur
render submissions or prove absence of between-snapshot leaks. Current QA
client process check found no Engine-Port S21.exe running. Build 7400C5B9.
