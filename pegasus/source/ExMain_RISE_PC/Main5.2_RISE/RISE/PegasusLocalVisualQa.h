#pragma once

#if defined(PEGASUS_ISOLATED_TEST)
// Isolated renderer harness only. This never sends a cast packet and must not
// be treated as GameServer, damage, buff-stat, or production acceptance.
bool RunPegasusLocalVisualQaStep(unsigned step, unsigned castOrdinal);
bool RunSwordBlowFlyLocalPreview(unsigned castOrdinal);
#endif
