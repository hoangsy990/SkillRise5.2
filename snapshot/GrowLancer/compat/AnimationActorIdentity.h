#pragma once
namespace rise { namespace growlancer {
// Identity for one active private actor. Same address after Invalidate is a new
// generation. Generation exhaustion disables binding instead of reusing IDs.
template<class Token, unsigned Limit = 0xffffffffu> class AnimationActorIdentity
{
    Token actor_ = 0;
    unsigned generation_ = 0;
public:
    constexpr bool Matches(Token actor) const { return actor && actor == actor_; }
    constexpr void Invalidate() { actor_ = 0; }
    constexpr unsigned Bind(Token actor)
    {
        if (!actor) return 0;
        if (Matches(actor)) return generation_;
        actor_ = 0;
        if (generation_ == Limit) return 0;
        ++generation_;
        actor_ = actor;
        return generation_;
    }
};
} }
