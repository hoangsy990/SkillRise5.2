#pragma once

namespace rise { namespace growlancer {

// Source clip identity ONLY. Never pass these values to native SetAction.
// Mapping/staging and the outer mount/dead/hit/owner gates remain separate.
enum class S21MountedReactionClip : unsigned short
{
    Unarmed = 335, Hand0 = 336, Hand1 = 337, BothHands = 338,
    Book = 339, Orb = 340, Elixir = 341
};

struct MountedReactionEquipment
{
    bool hand0Present;
    bool hand1Present;
    unsigned char hand0KindB;
    unsigned char hand1KindB;
};

// 10C3B1A -> Fenrir vtable2C/30 ->108AC04/108AC77, action argument6.
// Use stored character state; do not recompute or write global SafeZone here.
constexpr S21MountedReactionClip SelectS21MountedReaction(
    bool storedSafeState, MountedReactionEquipment equipment)
{
    const unsigned char kind0 = equipment.hand0Present ? equipment.hand0KindB : 0;
    const unsigned char kind1 = equipment.hand1Present ? equipment.hand1KindB : 0;
    if ((!equipment.hand0Present && !equipment.hand1Present) ||
        (storedSafeState && kind0 != 90))
        return S21MountedReactionClip::Unarmed;
    if (kind1 == 89) return S21MountedReactionClip::Book;
    if (kind0 == 90) return S21MountedReactionClip::Orb;
    if (kind1 == 114) return S21MountedReactionClip::Elixir;
    if (equipment.hand0Present && equipment.hand1Present)
        return S21MountedReactionClip::BothHands;
    return equipment.hand0Present ? S21MountedReactionClip::Hand0
                                  : S21MountedReactionClip::Hand1;
}

// Candidate manifest24EC40D7 only; caller must validate that package before
// use. Count alone is NOT package identity. A negative result means no mapping.
constexpr int MountedCandidateAction(S21MountedReactionClip clip,
                                     int loadedActionCount)
{
    if (loadedActionCount < 298) return -1;
    switch (clip)
    {
    case S21MountedReactionClip::Unarmed: return 294;
    case S21MountedReactionClip::Hand0: return 295;
    case S21MountedReactionClip::Hand1: return 296;
    case S21MountedReactionClip::BothHands: return 297;
    default: return -1; // Other classes' clips are not in this package.
    }
}

}}
