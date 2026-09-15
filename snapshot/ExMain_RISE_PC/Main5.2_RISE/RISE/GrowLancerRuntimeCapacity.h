#pragma once

namespace rise { namespace growlancer {

// CRC32 of the isolated Player.bmd produced by merge_player_actions.py.
// Keep the legacy CRC accepted in ZzzToolKit as well so an unmodified SS6
// Player.bmd remains valid; this value authorizes only the hash-pinned
// Grow Lancer animation package.
constexpr unsigned int kMergedPlayerBmdCrc32 = 0xE51E1780u;

// Models is randomized by up to 1023 BMD entries in OpenPlayers. The Grow
// Lancer package owns only this bounded tail and never changes legacy IDs.
// Preserve the original32-entry skill tail (including spare31). Body models
// have a separate range and must never enter skill-effect predicates.
constexpr int kClassBodyModelOffset = 32;
constexpr int kClassBodyModelCount = 5;
constexpr int kDynamicModelCapacity = kClassBodyModelOffset + kClassBodyModelCount;

constexpr int ClassBodyModelId(unsigned part, int legacyModelLimit)
{
    return part < kClassBodyModelCount ? legacyModelLimit + kClassBodyModelOffset + static_cast<int>(part) : -1;
}

constexpr int RuntimeModelLimit(int legacyModelLimit)
{
    return legacyModelLimit + kDynamicModelCapacity;
}

constexpr bool IsRuntimeModelId(unsigned int modelId, int legacyModelLimit)
{
    return modelId < static_cast<unsigned int>(RuntimeModelLimit(legacyModelLimit));
}

}}
