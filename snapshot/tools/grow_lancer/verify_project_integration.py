"""Read-only project inclusion audit, NOT proof of a git merge or gameplay."""
from pathlib import Path
import xml.etree.ElementTree as ET
import re

ROOT = Path(__file__).resolve().parents[2]
PC = ROOT / "ExMain_RISE_PC"
NS = {"m": "http://schemas.microsoft.com/developer/msbuild/2003"}

def entries(path, tag):
    tree = ET.parse(path)
    result = {}
    for node in tree.findall(f".//m:{tag}", NS):
        name = node.get("Include")
        if name:
            result.setdefault(name.replace("\\", "/"), []).append(node)
    return result

def main():
    for tag, suffix in (("ClCompile", ".cpp"), ("ClInclude", ".h")):
        project = entries(PC / "Main.vcxproj", tag)
        filters = entries(PC / "Main.vcxproj.filters", tag)
        files = sorted((PC / "Main5.2_RISE" / "RISE").glob("GrowLancer*" + suffix))
        assert files, f"missing {tag} sources"
        for file in files:
            relative = file.relative_to(PC).as_posix()
            assert len(project.get(relative, [])) == 1, f"project missing/duplicate {relative}"
            assert len(filters.get(relative, [])) == 1, f"filters missing/duplicate {relative}"
            for child in project[relative][0]:
                assert not (child.tag.endswith("ExcludedFromBuild") and
                            (child.text or "").strip().lower() == "true"), relative
        print(f"PASS {len(files)} GrowLancer {tag} entries: project + filters, no exclusion")
    gs = ROOT / "ExGameServer" / "GameServer"
    gs_entries = entries(gs / "GameServer.vcxproj", "ClCompile")
    consumers = ("SkillManager", "ItemManager", "MasterSkillTree", "Quest",
                 "QuestObjective", "QuestReward", "QuestWorld", "QuestWorldObjective",
                 "QuestWorldReward", "ItemBagEx")
    for stem in consumers:
        name = stem + ".cpp"
        assert len(gs_entries.get(name, [])) == 1, f"GS project missing/duplicate {name}"
        for child in gs_entries[name][0]:
            assert not (child.tag.endswith("ExcludedFromBuild") and
                        (child.text or "").strip().lower() == "true"), name
        content = (gs / name).read_text(encoding="utf-8-sig", errors="replace")
        assert '#include "../../Shared/LegacySkillClassColumns.h"' in content, name
    # Resolve only task-specific literal includes; this is not a C++ preprocessor.
    # Header-only Shared/compat dependencies need not be ClInclude project entries.
    roots = [PC / "Main5.2_RISE", gs, ROOT / "Shared", ROOT / "GrowLancer" / "compat"]
    checked = 0
    for directory in roots:
        for file in directory.rglob("*"):
            if file.suffix not in (".cpp", ".h"):
                continue
            content = file.read_text(encoding="utf-8-sig", errors="replace")
            for include in re.findall(r'^\s*#\s*include\s*"([^"]+)"', content, re.M):
                if not any(key in include for key in
                           ("GrowLancer", "LegacySkillClassColumns", "S21CastAnimationSpeed")):
                    continue
                search_dirs = [file.parent]
                if file.is_relative_to(PC):
                    search_dirs.append(PC / "Main5.2_RISE")
                elif file.is_relative_to(gs):
                    search_dirs.append(gs)
                candidates = [(base / include.replace("\\", "/")).resolve() for base in search_dirs]
                target = next((path for path in candidates if path.is_file()), candidates[0])
                assert target.is_relative_to(ROOT.resolve()), f"external task include {file}: {include}"
                assert target.is_file(), f"missing task include {file}: {include}"
                checked += 1
    assert checked > 0
    print(f"PASS 10 GS loader project entries; {checked} task-specific literal includes resolve inside worktree")
    print("Scope: local project inclusion only; uncommitted/untracked files are NOT merged.")

if __name__ == "__main__":
    main()
