#pragma once

namespace rise { namespace growlancer {

// Runtime SkillAttribute lookup index, not Skill.bmd's fixed record count or
// CHARACTER_ATTRIBUTE::Skill's slot array. The active S21 master ID 895
// requires an exclusive upper bound of 896. Activation is a separate gate.
constexpr int kSkillAttributeIdCapacity = 896;

} }
