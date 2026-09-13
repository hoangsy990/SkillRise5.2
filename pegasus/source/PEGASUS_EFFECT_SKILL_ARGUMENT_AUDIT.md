# Native CreateEffect skill arguments

The RISE signature in ZzzEffect.h is PKKey, SkillIndex, Skill,
SkillSerialNum, Scale. ZzzEffect.cpp stores SkillIndex into OBJECT.Skill.
The adjacent argument called Skill is not interchangeable with SkillIndex.
Inspect each pinned Pegasus call's arguments before changing an adapter.

## Source-verified corrections, 2026-09-13

- Havoc: six paths fixed, including impact shockwave. QA18436 actually
  submitted base+override shader passes for small and caster rings.
- Chaos: 6547C4 loads2012,6547C9 passes it to27BF(subtype2,scale.9);
  65483E passes it to27C0(subtype19,scale2);654DF5 passes2012 to27BE
  (subtype0,scale1.2). All three adapter calls moved skill to SkillIndex.
- Spearstorm: 67558B loads243 intoEBX,6755D3 pushesEBX in SkillIndex
  position for27C8(subtype0,scale.9), call6755EC. Root adapter corrected.

Chaos and Spearstorm currently do not branch on effect.Skill in their
dedicated runtime files. Thus restoring their tag is source fidelity, NOT
evidence a visual symptom is fixed. Runtime regression remains required.
Source routing guards check all corrected argument sequences; both pass.

## Pending individual comparison

### Charge zero-tag exceptions

Native CreateEffect also stores the adjacent Skill argument in OBJECT.Kind.
Unlike attacks, Spiral677012..677014, Crusher65B051..65B053 and
65B077..65B079, Elemental664D5B..664D5D explicitly pass three zeros.
Original Spiral EDI is zeroed676FF5 before the call; Crusher EAX is zeroed
65B049/65B06B. Corrected the adapter's nonzero adjacent Kind arguments to
zero, retaining zero SkillIndex, serial and authored scales. Public entry
signatures remain compatible, unused parameter unnamed. Anchor/aura zero
tags were already correct. Three math/routing gates and new zero-tag guards
pass; runtime regression pending. No buff stat formula is added by this.

### Bird / Dragon follow-up

Bird672B11->672B4C proves241 in SkillIndex for27C1(scale1.3); fixed.
Dragon root663587, impact663C42, head6639F0, crack663B3B, wind663A8E,
layer663BC5 each pass242 in SkillIndex; fixed all six adapter paths.
Also source6639CE..6639F8 randomizes head yaw before subtype selection;
adapter wrongly passed zero yaw. Restored that draw in the same order.
Layer source663BD2/663BDF selects B6B16C for subtype1, B6B184 otherwise;
decoded floats are (.0785,.314,.92) and(.4,.5,.92). Adapter had these
groups reversed; corrected and guarded. These changes need runtime QA.

Dex calls661C95/661CBA pass zeros at661C83..85/661CA9..AB; removed
unproven cast ID from adjacent Kind in both adapters. Deathside root6605D2
and scythe helper660658 pass288 in SkillIndex; corrected both slots and
added guards. This closes the listed pattern candidates, not all effect
constructor provenance or runtime acceptance. Some original constructors
intentionally pass zero; do not mechanically rewrite every match.
Keep Fly separate and leave unrelated runtime branches untouched.

All source addresses refer only to hash-pinned Pegasus MU-x86.exe
02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA.
