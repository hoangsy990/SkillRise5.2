#pragma once

namespace rise { namespace growlancer {

// S21 CalcCharacter.lua pins base class 7. This private native selector
// follows CLASS_TEMPLENIGHT (17) without renumbering the SS6 CLASS_TYPE enum.
constexpr unsigned kGrowLancerSourceBaseClass = 7;
constexpr unsigned kGrowLancerNativeClassType = 18;
constexpr unsigned kUnmappedGrowLancerEvolutionClassType = 0xff;

} }
