#pragma once

class CHARACTER;

namespace rise { namespace pegasus {
void BeginCasterActionTrace(CHARACTER& caster, int skill, int expectedAction);
void UpdateCasterActionTrace(CHARACTER& caster);
}}
