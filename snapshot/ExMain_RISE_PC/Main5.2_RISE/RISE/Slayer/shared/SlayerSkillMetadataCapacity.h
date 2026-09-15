#pragma once

// Only the dynamically allocated SKILL_ATTRIBUTE metadata pointer grows.
// MAX_SKILLS remains 650 for Skill.bmd, CharacterAttribute::Skill[], skill
// slots and the native 5.2 packet/character ABI.
namespace rise { namespace slayer {
#ifdef RISE_SLAYER_PORT
constexpr int kSkillAttributeCapacity = 800; // SS21 Master Slayer max is 794.
#else
constexpr int kSkillAttributeCapacity = MAX_SKILLS;
#endif
}}
