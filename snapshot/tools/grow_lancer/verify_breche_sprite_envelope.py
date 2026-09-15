import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SPRITE_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "zzzeffectsprite.cpp"
RUNTIME_CPP = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerEffectRuntime.cpp"
ADAPTER_H = ROOT / "ExMain_RISE_PC" / "Main5.2_RISE" / "RISE" / "GrowLancerSpriteAdapter.h"
PROVENANCE = ROOT / "GrowLancer" / "asset_provenance.json"


def main() -> None:
    sprite = SPRITE_CPP.read_text(encoding="utf-8")
    runtime = RUNTIME_CPP.read_text(encoding="utf-8")
    adapter = ADAPTER_H.read_text(encoding="utf-8")
    provenance = json.loads(PROVENANCE.read_text(encoding="utf-8"))

    required_adapter = (
        "int CreateBrecheSprite(",
        "void MarkBrecheSprite(OBJECT* sprite);",
        "void ClearBrecheSprite(OBJECT* sprite);",
        "bool IsBrecheSprite(const OBJECT* sprite);",
    )
    for needle in required_adapter:
        assert needle in adapter, needle

    # S21 1727079: visible adds .1, non-visible subtracts .1, then clamps to
    # [.2, 1].  This branch is private to the marked 7EF7 flare sprites.
    assert "if (rise::growlancer::IsBrecheSprite(o) ||" in sprite
    assert "o->AnimationFrame += o->Visible ? .1f : -.1f;" in sprite
    assert "if (o->AnimationFrame > 1.f) o->AnimationFrame = 1.f;" in sprite
    assert "if (o->AnimationFrame < .2f) o->AnimationFrame = .2f;" in sprite

    # A native subtype-0 sprite still owns additive blending.  The private
    # marker must not repurpose OBJECT::SubType or alter ordinary SS6 sprites.
    assert "CreateSprite(texture, position, scale, light, owner,\n        rotation, 0);" in sprite
    assert "rise::growlancer::ClearBrecheSprite(o);" in sprite
    assert "MarkBrecheSprite(&Sprites[result]);" in sprite
    assert "CreateBrecheSprite(BITMAP_LIGHT, position, 4.f, light, &owner, 0.f);" in runtime
    flare = next(record for record in provenance["assets"]
                 if record.get("skillId") == 279
                 and record.get("role") == "emitter_flare_sprite")
    assert flare["s21RuntimeId"] == 32503
    assert flare["riseRuntimeId"] == "BITMAP_LIGHT"
    assert flare["sha256"] == "874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8"

    print("PASS: Breche 7EF7 private sprite marker, sampler envelope and provenance")


if __name__ == "__main__":
    main()
