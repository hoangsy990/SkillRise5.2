"""Correct exact S21/RISE Wrath BMD material attribution; no pixel claim."""
from hashlib import sha256
from pathlib import Path
from convert_s21_bmd_v0f_to_rise_v0c import (
    parse_s21_container, decrypt_s21_payload, decrypt_rise_v0c
)

ORIGINAL = Path(r"D:\MU FICA Season 21\Data\Effect")
STAGED = Path(r"D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC"
              r"\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Wrath")
ROWS = (
    ("wratha01.bmd",
     "9aeb30a8ac05874d3647627990973c2729a41bebaa04d4ec2ef845144fbca54d",
     "6fa8a7b82b738ea30de8f9b5ddc53c7721bd20c51cba711825a296e8b8b3834c",
     b"alpha_line.jpg", b"dust01.tga"),
    ("wratha02.bmd",
     "a1a748becc7c868ec1a7239d4ad1636d95dc8597923d596278b8d9e9e095c726",
     "4ad476195c01b3e35de2bea2ce08917e8dbd5668337276d2fd30ac8aa049fd84",
     b"dust01.tga", b"alpha_line.jpg"),
)


def main() -> None:
    for name, original_hash, stage_hash, material, absent in ROWS:
        source = (ORIGINAL / name).read_bytes()
        stage = (STAGED / name).read_bytes()
        assert sha256(source).hexdigest() == original_hash, f"S21 {name} drift"
        assert sha256(stage).hexdigest() == stage_hash, f"RISE {name} drift"
        decoded = decrypt_s21_payload(parse_s21_container(source))
        assert decrypt_rise_v0c(stage) == decoded, f"converted {name} payload differs"
        assert decoded.count(material) == 1 and absent not in decoded
        assert b"alpha_line.tga" not in decoded
        print(f"PASS: {name} exact embedded material {material.decode()}, "
              "S21/RISE decoded payload equal")
    assert (ORIGINAL / "dust01.OZT").is_file()
    assert not (ORIGINAL / "alpha_line.OZT").exists()
    print("Correction: wratha02 does not request alpha_line.tga; the only source material is dust01.tga")
    print("Scope: model material identity only; native BMD GPU binding/black-square owner pixels OPEN")


if __name__ == "__main__":
    main()
