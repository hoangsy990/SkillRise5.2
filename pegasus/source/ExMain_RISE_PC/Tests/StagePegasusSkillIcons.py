from __future__ import annotations

import hashlib
import sys
from pathlib import Path

from PIL import Image


PEGASUS_DATA = Path(r"D:\Pegasus\Data").resolve()

# Exact icon paths come from the hash-pinned Pegasus Skill.xml. 241..243 are
# stored one directory higher than the XML path, so their actual locations are
# recorded explicitly instead of guessed at runtime.
ICONS = {
    241: (r"Skill\WebZen\241.png", "F254F9D734ED71943DD2A870C59673276156781A809A9A0585A62F5F0F3FC599", "99D89A4817F2C70195301B43469813C9AB4BBEFCA1F88BDE5C5F39D3FD3E0731"),
    242: (r"Skill\WebZen\242.png", "73F4679B5D588FA35ACA0C2BAD84BC6EB4ECAE3059AB556967F9D72D41F8119F", "FED9DF3968F6BAEB0D488F49B2ED0C7125F7919647C3039DD0370589770FF2A0"),
    243: (r"Skill\WebZen\243.png", "87A681E04B7F11FB332663D104A39C53EBA6AC52557F79C77DE6DF52266CDA1D", "AEFFB80B5F13EB1A39F4DD4B99A5E9F815743EF29472E5C54C7F01ED2785CF77"),
    288: (r"Skill\WebZen\Skill\288.png", "9390ED8EC0C88C51E3B56D1ACB0702DA9CB582CFE2EDC999436293AE2B4ED9EA", "2CA03F21C3F672E8B94C268A33ABD824BE973BC61E7B535D1C1B8270647E40F6"),
    289: (r"Skill\WebZen\Skill\289.png", "1D12B1B70DE09A6BC78FB976AB5AE3A60F124D86095C70A8CA48790489AA91B3", "4E2CBBC13951F2594A089CFA22B2A9E68BF8C8CAD76AAEB3668523CE3CD4BB50"),
    1500: (r"Skill\WebZen\Skill\1500.png", "363647FCBBC994E518A29801554828F431D040BB522457D67F48533845B9BDB0", "57B84488943FDE435510E95B30C7D4CA3666AB0D373B0A6AF72FC2EFC8D82B5A"),
    1501: (r"Skill\WebZen\Skill\1501.png", "0C66916A604A74770F4D51AD193ACE0DB98E1748E9B5C9F99E3C263A16E96AC0", "34AC4E0B8C33D98055D8107AFEBACBBE4E95D5C7E59CF3D72F795A29CF788885"),
    2012: (r"Skill\WebZen\Skill\2012.png", "B7091D4390FB46B81205627B51259EC27E26B8A263E1BD476C619455F69C75AA", "7B82CB30250810AE42E7EC2217DA635D5261F4D72856B9D2991CFDCF50EBA406"),
    2013: (r"Skill\WebZen\Skill\2013.png", "DB10E613120585E1BDA7D58FBC4C40E1E2E43DFCF4996466680D3728C5F4DC88", "6B6A2D88FC602775FE507C89846928C5CE36F9EFF7BC66324A67D5957EC4EE29"),
    2014: (r"Skill\WebZen\Skill\2014.png", "D9F1C82245759DED6920B2CCA2EA1C7E3D8C37B516A571470529993BC9C33CAF", "8C4A43AFB42DC2E667B51360BFF91817CB640CCA1CF2A1EAE347201E1E4387F9"),
    2015: (r"Skill\WebZen\Skill\2015.png", "8F95F4D0F0C0D10E32A3FD6C6C7E56BECB3428B708D22757F85BDEBE04B35D1D", "5E5B24EDFF12BF211DAFD8E301735D94329F776B70BB9CF885DADF7EBC1B7143"),
    2016: (r"Skill\WebZen\Skill\2016.png", "7F0C8B84CC3B536E91AB7A46C1F6D62B53085603728F9BCF44EC322EAF677AF9", "3E68F53DF4B4754A2EAD56D1D345E954BF8DE6B860FD00844151A849488731F5"),
    2023: (r"Skill\WebZen\Skill\2023.png", "7E17DC1B966511C1E272A85EA6AE77B187CE64583A2E0B77D58527196E7CF2D3", "177A48CDEB617792A8B42DB4C4A1461282FFD0A13DDA7FD069638D2B033A7952"),
    2024: (r"Skill\WebZen\Skill\2024.png", "5215806AD94CBC81D14C08670BB602CB3CAE09AAE1A0C045395EFBC27942A255", "9E56BC44F3CDBEBF3A1956DBFC246AF1C736560083934F73CC27BC65E7CC6B32"),
}


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest().upper()


def main() -> int:
    if len(sys.argv) != 2:
        raise SystemExit("usage: StagePegasusSkillIcons.py <isolated-target>")
    target = Path(sys.argv[1]).resolve()
    target.mkdir(parents=True, exist_ok=True)

    # Remove only obsolete generated copies from the failed direct-JPG/TGA
    # experiments. Source PNGs and every unrelated asset remain untouched.
    for skill_id in ICONS:
        for extension in ("jpg", "tga"):
            stale = (target / f"{skill_id}.{extension}").resolve()
            if target not in stale.parents:
                raise RuntimeError(f"Pegasus icon target escapes isolated root: {stale}")
            if stale.exists():
                stale.unlink()

    for skill_id, (relative, source_hash, target_hash) in ICONS.items():
        source = (PEGASUS_DATA / relative).resolve()
        if PEGASUS_DATA not in source.parents or sha256(source) != source_hash:
            raise RuntimeError(f"Pegasus icon source mismatch: {source}")
        output = (target / f"{skill_id}.OZJ").resolve()
        if target not in output.parents:
            raise RuntimeError(f"Pegasus icon target escapes isolated root: {output}")
        if not output.exists():
            with Image.open(source) as image:
                import io
                encoded = io.BytesIO()
                image.convert("RGB").save(encoded, format="JPEG", quality=100,
                                          subsampling=0, optimize=False)
                jpeg = encoded.getvalue()
                # Native Convert_Format duplicates the first 24 JPEG bytes as
                # the OZJ dump header; OpenJpeg skips those 24 bytes.
                output.write_bytes(jpeg[:24] + jpeg)
        if sha256(output) != target_hash:
            raise RuntimeError(f"Converted Pegasus icon mismatch: {output}")

    print(f"Pegasus skill icons PASS: {len(ICONS)} files -> {target}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
