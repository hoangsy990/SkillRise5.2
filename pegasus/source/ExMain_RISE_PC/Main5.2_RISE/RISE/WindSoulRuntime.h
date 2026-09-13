#pragma once
#include "WindSoulContract.h"

class CHARACTER;

namespace rise { namespace windsoul {

constexpr int kSkillId = Base737Contract::SkillId;

void ApplyBase737SkillCatalog();

// Native SS6 compatibility actions are used because S21 action 144 is not a
// portable RISE 5.2 object/action index.
void ApplyBase737CasterAction(CHARACTER& caster);

// The caller supplies an already-resolved CharactersClient[] slot, never a
// network key.
bool CreateBase737Root(CHARACTER& caster, int targetSlot);

}}
