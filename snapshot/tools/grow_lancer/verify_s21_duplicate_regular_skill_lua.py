"""Prove Grow Lancer regular/master math agrees across supplied S21 Lua copies."""

import hashlib
import re
from pathlib import Path


source = Path(r"D:\GameServer S21\Data")
regular_copies = {
    "Skills/RegularSkillCalc.lua": "F735D95E32C70E38AC6E65086E4AA21CDE1CD1A1C6E4B6549D124E502F9D3847",
    "Scripts/Skills/RegularSkillCalc.lua": "78A1502C187D9EA248E682CCAA62EA8C4510D7119192510D1A966A18E6992E4D",
}
regular_names = (
    "GrowLancerSpinStep", "GrowLancerHarshStrike", "GrowLancerMagicPin",
    "GrowLancerBreche", "GrowLancerShiningPeak", "GrowLancerObsidian",
)
master_copies = {
    "Skills/MasterSkillCalc_3rd.lua": "771066684E4478A69659D46C27EEB87A9242D8B6D8566E73A4D31AD0F479F699",
    "Scripts/Skills/MasterSkillCalc_3rd.lua": "904A41053E9E4B12D0CC65D316FD661EE751B23ED7E73D1A89002ACDD810FD8C",
}
master_names = (
    "GrowLancerSpinStep_Level1", "GrowLancerSpinStep_Level2",
    "GrowLancerHarshStrike_Level1", "GrowLancerHarshStrike_Level2",
    "GrowLancerMagicPin_Level1", "GrowLancerMagicPin_Level2",
    "GrowLancerBreche_Level1", "GrowLancerBreche_Level2",
    "GrowLancerShiningPeak_Level1", "GrowLancerObsidian_Level1",
    "GrowLancerOversting_Level1", "GrowLancerWildBreath_Level1",
)


def bodies(path: str, expected_hash: str, names: tuple[str, ...]) -> dict[str, str]:
    raw = (source / path).read_bytes()
    assert hashlib.sha256(raw).hexdigest().upper() == expected_hash, path
    text = raw.decode("utf-8-sig")
    result = {}
    for name in names:
        match = re.search(r"(?m)^function\s+" + name + r"\([^\n]*\)\s*\n(.*?)^end\s*$", text, re.S | re.M)
        assert match, (path, name)
        # These six bodies contain no quoted strings. Remove Lua comments and
        # layout only, preserving every identifier/operator/numeric literal.
        body = re.sub(r"--[^\n]*", "", match.group(1))
        assert '"' not in body and "'" not in body, (path, name)
        result[name] = re.sub(r"\s+", "", body)
    return result


for copies, names, label in (
    (regular_copies, regular_names, "regular"),
    (master_copies, master_names, "third-master"),
):
    first, second = (bodies(path, digest, names) for path, digest in copies.items())
    for name in names:
        assert first[name] == second[name], f"Grow Lancer {label} formula drift: {name}"
    print(f"PASS {len(names)} Grow Lancer {label} Lua functions token-identical across hash-pinned S21 copies")
print("OPEN executable Lua load path, protected handler target/hit/rounding and class7 default authority")
