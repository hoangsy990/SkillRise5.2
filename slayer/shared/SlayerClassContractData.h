#pragma once

#include "SlayerSkillContractData.h"

namespace rise { namespace slayer {

struct SlayerClassProfile
{
    int classId;
    const char* firstClassName;
    const char* secondClassName;
    const char* thirdClassName;
    const char* startingLocation;
    int startingHp;
    int startingMp;
    int startingStrength;
    int startingDexterity;
    int startingVitality;
    int startingEnergy;
    bool createdByDefault;
};

// Values transcribed from the public Slayer guide. This is metadata for the
// eventual class-creation migration; it does not write the legacy class
// arrays while MAX_CLASS remains seven.
static const SlayerClassProfile kSlayerClassProfile =
{
    kS21ClassSlayer,
    "Slayer",
    "Royal Slayer",
    "Master Slayer",
    "Lorencia",
    130,
    10,
    28,
    30,
    15,
    10,
    true
};

inline bool IsSlayerClass(int classId)
{
    return classId == kSlayerClassProfile.classId;
}

inline bool IsSlayerPartyClass(int classId)
{
    return classId == kS21ClassSlayer || classId == 1 || classId == 5;
}

}}
