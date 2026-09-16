#include "../../GrowLancer/compat/AnimationActorIdentity.h"
constexpr bool Identity()
{
    rise::growlancer::AnimationActorIdentity<unsigned, 3> identity;
    if (identity.Bind(0) || identity.Matches(0)) return false;
    if (identity.Bind(100) != 1 || identity.Bind(100) != 1) return false;
    if (!identity.Matches(100) || identity.Matches(200)) return false;
    identity.Invalidate();
    if (identity.Matches(100) || identity.Bind(100) != 2) return false;
    if (identity.Bind(200) != 3 || identity.Matches(100)) return false;
    if (identity.Bind(300) != 0 || identity.Matches(200)) return false;
    identity.Invalidate();
    return identity.Bind(100) == 0 && !identity.Matches(100);
}
static_assert(Identity(), "same address reuse, changed actor, null token and permanent exhaustion protection");
