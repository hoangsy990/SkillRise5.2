#!/usr/bin/env python3
"""Check that every imported Slayer id has a complete 5.2 source surface.

This is a static wiring check only.  It does not claim that a legacy character
class has been migrated or that an owner has accepted the visuals in-game.
"""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SKILLS = {
    292: ("kSwordInertia", "SkillSlayerSwordInertia"),
    293: ("kBatFlock", "SkillSlayerBatFlock"),
    294: ("kPierceAttack", "SkillSlayerPierceAttack"),
    295: ("kDetection", "SkillSlayerDetection"),
    297: ("kDemolish", "SkillSlayerDemolish"),
}


def read(relative: str) -> str:
    path = ROOT / relative
    if not path.is_file():
        raise FileNotFoundError(path)
    return path.read_text(encoding="utf-8", errors="replace")


def require(text: str, needle: str, label: str) -> None:
    if needle not in text:
        raise AssertionError(f"missing {label}: {needle}")


def main() -> int:
    shared = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerSkillContractData.h")
    server_catalog = read("ExGameServer/GameServer/RISE/SlayerServerCatalog.h")
    server_object = read("ExGameServer/GameServer/ObjectManager.cpp")
    server_ds = read("ExGameServer/GameServer/DSProtocol.cpp")
    require(server_catalog, "const int stageBits = (dbClass - kS21SlayerDbClass) * 16;",
            "Slayer wire class retains its S21 persisted stage")
    require(server_catalog, "stageBits - stageBits / 32",
            "Slayer wire class uses the existing 5.2 stage packing")
    for db_class, expected_server, expected_client in (
            (144, 0xE0, 7), (145, 0xF0, 15), (146, 0xFF, 31)):
        stage_bits = (db_class - 144) * 16
        server_byte = 0xE0 + stage_bits - stage_bits // 32
        client_byte = (((server_byte >> 4) & 1) << 3) | \
            (server_byte >> 5) | (((server_byte >> 3) & 1) << 4)
        if (server_byte, client_byte) != (expected_server, expected_client):
            raise AssertionError(f"Slayer class stage wire drifted for DB {db_class}")
    print("PASS: Slayer DB 144/145/146 encode E0/F0/FF and decode client 7/15/31")
    require(server_catalog, "stage == 2 ? 0x18 : stage == 1 ? 0x10 : 0",
            "Slayer viewport preview stage flags spare equipment low bits")
    require(server_object, "SlayerPreviewClassByteForDbClass(DbClass)",
            "Slayer viewport class retains Royal/Master stage")
    if server_ds.count("ClientClassByteForDbClass(") < 2:
        raise AssertionError("Slayer DS character-list/create stage wire missing")
    for db_class, expected_server, expected_client in (
            (144, 0xE0, 7), (145, 0xF0, 15), (146, 0xF8, 31)):
        stage = db_class - 144
        server_byte = 0xE0 + (0x18 if stage == 2 else 0x10 if stage == 1 else 0)
        client_byte = (((server_byte >> 4) & 1) << 3) | \
            (server_byte >> 5) | (((server_byte >> 3) & 1) << 4)
        if (server_byte, client_byte) != (expected_server, expected_client):
            raise AssertionError(f"Slayer viewport stage drifted for DB {db_class}")
    print("PASS: Slayer viewport DB 144/145/146 encode E0/F0/F8 without equipment-bit spill")
    server = read("ExGameServer/GameServer/SkillManager.cpp")
    server_protocol = read("ExGameServer/GameServer/Protocol.cpp")
    attack = read("ExGameServer/GameServer/Attack.cpp")
    server_overlay = read("ExGameServer/GameServer/RISE/SlayerServerCatalog.h")
    client_receive = read("ExMain_RISE_PC/Main5.2_RISE/WSclient.cpp")
    client_use = read("ExMain_RISE_PC/Main5.2_RISE/ZzzInterface.cpp")
    client_skill_manager = read("ExMain_RISE_PC/Main5.2_RISE/SkillManager.cpp")
    character_render = read("ExMain_RISE_PC/Main5.2_RISE/ZzzCharacter.cpp")
    minimap = read("ExMain_RISE_PC/Main5.2_RISE/NewUIMiniMap.cpp")
    runtime = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/client/SlayerNativeRuntime.cpp")
    resources = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/client/SlayerSkillResources.cpp")
    effect_allocator = read("ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.cpp")
    effect_header = read("ExMain_RISE_PC/Main5.2_RISE/ZzzEffect.h")
    joints = read("ExMain_RISE_PC/Main5.2_RISE/ZzzEffectJoint.cpp")
    packet = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/server/SlayerPacketContract.h")
    converter = read("tools/slayer/convert_s21_slayers.py")
    player_model_loader = read("ExMain_RISE_PC/Main5.2_RISE/ZzzOpenData.cpp")
    character_manager = read("ExMain_RISE_PC/Main5.2_RISE/CharacterManager.cpp")
    client_defines_path = ROOT / "ExMain_RISE_PC/Main5.2_RISE/_define.h"
    client_defines = client_defines_path.read_text(encoding="utf-8", errors="replace") if client_defines_path.is_file() else ""
    project = read("ExMain_RISE_PC/Main.vcxproj")
    if project.count("<PostBuildEvent Condition=\"'$(SlayerIsolatedBuild)'!='true'\">") != 2:
        raise AssertionError("isolated Slayer builds can run shared-client post-build xcopy")
    require(converter, "mesh_textures = mesh_texture_references(",
            "imported S21 model BMD mesh textures checked against Slayer manifest")
    require(server_overlay, "kBatFlockMastery = 782",
            "S21 Pierce mastery prerequisite uses pinned skill 782")
    require(server_overlay, "kBatFlockStrengthener = 781",
            "S21 Bat Flock mastery parent uses pinned skill 781")
    require(server_overlay, '"Bat Flock Strengthener", 22, 25, 9, 6',
            "GS mastery-781 catalog preserves S21 SkillList row")
    require(server_overlay, '"Bat Flock Mastery", 23, 30, 12, 6',
            "GS mastery-782 catalog preserves S21 SkillList row")
    require(server, "IsSlayerBatMasterySkill(index)",
            "GS mastery learning has an explicit persisted Slayer class gate")
    require(server, "kBatFlockStrengthener) < 10",
            "GS mastery 782 learning requires ten points in 781")
    mastery_tree = read("ExGameServer/GameServer/MasterSkillTree.cpp")
    learning = mastery_tree.split("void CMasterSkillTree::CGMasterSkillRecv", 1)[1]
    learning = learning.split("void CMasterSkillTree::GCMasterInfoSend", 1)[0]
    require(mastery_tree, "first.Slot != 58 || first.Group != 1 || first.Rank != 6",
            "GS accepts only the pinned S21 Bat 781 tree slot/group/rank")
    require(mastery_tree, "second.Slot != 62 || second.Group != 1 || second.Rank != 7",
            "GS accepts only the pinned S21 Bat 782 tree slot/group/rank")
    require(mastery_tree, "info.ReplaceSkill = shape.Brand;",
            "Master Slayer active Bat skill replacement follows the S21 Brand chain")
    require(mastery_tree, "info.RequireClass[rise::slayerserver::kSlayerLegacyArrayClass] = shape.Slot;",
            "Master Slayer learned-node packet reports its S21 UI slot")
    require(mastery_tree, "shared.RequireClass[rise::slayerserver::kSlayerLegacyArrayClass] = shape.Slot;",
            "shared numeric mastery IDs report the actor's S21 slot, not DK slot")
    require(mastery_tree, "if (index == 631)\n\t\treturn false;",
            "S21 Rush/legacy 631 collision remains excluded")
    require(mastery_tree, "shared.RelatedSkill != shape.Brand ||\n\t\t\tshared.ReplaceSkill != shape.Brand",
            "shared mastery row brand checked before class-scoped slot remap")
    require(mastery_tree, "!rise::slayerserver::IsSlayerBatMasterySkill(index))",
            "new S21 Bat learning info is not exposed to other classes")
    require(learning, "this->GetInfoForActor(lpObj, lpMsg->MasterSkill, &MasterSkillTreeInfo)",
            "Slayer learning uses the per-actor S21 info overlay, not only legacy GetInfo")
    require(learning, "const CSkill originalBat = activeBat ? *activeBat : CSkill();",
            "Master Slayer learning snapshots the active Bat skill before replacement")
    require(learning, "*activeBat = originalBat;\n\t\t\treturn;",
            "Master Slayer learning rolls back an orphaned cast ID on slot failure")
    require(mastery_tree, "this->GetInfoForActor(lpObj, lpObj->MasterSkill[n].m_index, &MasterSkillTreeInfo)",
            "Master Slayer acquired nodes remain visible to rank checks and skill-list sends")
    require(learning, "rise::slayerserver::IsSlayerBatMasterySkill(MasterSkillTreeInfo.Index)",
            "GS mastery learning packet recognizes the two S21 Bat nodes")
    require(learning, "!rise::slayerserver::IsSlayerDbClass(lpObj->DBClass)",
            "GS mastery learning packet rejects non-Slayer DB classes")
    require(learning, "this->GetMasterSkillLevel(lpObj,",
            "GS mastery learning packet enforces 781/10 before 782")
    delay_header = read("ExGameServer/GameServer/SkillManager.h")
    allocator = read("ExGameServer/GameServer/MemoryAllocatorInfo.cpp")
    require(delay_header, "#define MAX_SKILL 800",
            "GS per-user delay capacity covers S21 active Master Slayer IDs")
    require(allocator, "m_SkillDelay = new DWORD[MAX_SKILL]",
            "GS delay allocation follows the expanded numeric-ID capacity")
    require(server, "index < 0 || index >= MAX_SKILL",
            "GS delay accessor fails closed before numeric-ID indexing")
    mastery_cast = server.split("case rise::slayerserver::kBatFlockStrengthener:", 1)[1].split(
        "case rise::slayerserver::kPierceAttack:", 1)[0]
    require(mastery_cast, "return this->SkillSlayerBatFlock(aIndex, bIndex, lpSkill);",
            "S21 Brand-canonicalized Bat mastery casts use the Bat graph")
    require(resources, "int CanonicalSlayerVisualSkill(int skillId)",
            "client has an isolated Master Slayer Brand resolver")
    require(resources, "const DWORD brand = SkillAttribute[current].SkillBrand;",
            "client resolver follows the S21 SkillList Brand chain")
    for material_default in (
        "o->HiddenMesh = -1;", "o->BlendMesh = -1;",
        "o->BlendMeshLight = 1.f;", "o->BlendMeshTexCoordU = 0.f;",
        "o->BlendMeshTexCoordV = 0.f;",
    ):
        require(effect_allocator, material_default,
                "5.2 Slayer effect keeps the native material allocator defaults")
    require(resources, "model.RenderBody(renderFlags, effect.Alpha,",
            "5.2 model draw forwards native per-object alpha")
    require(resources, "effect.BlendMesh, effect.BlendMeshLight,",
            "5.2 model draw forwards native blend-material fields")
    require(resources, "return IsSlayerSkill(skillId) || skillId == 781 || skillId == 782;",
            "only five base casts and S21 Bat 781/782 have client/GS routes")
    require(resources, "skillId >= 779 && skillId <= 794 &&",
            "unported exclusive S21 mastery rows are not generic 5.2 casts")
    require(client_receive, "CanonicalSlayerVisualSkill(MagicNumber)",
            "remote ReceiveMagic canonicalizes high Bat mastery visuals")
    require(client_receive, "IsUnportedSlayerExclusiveMasterSkill(MagicNumber)",
            "remote ReceiveMagic drops unported mastery before actor mutation")
    require(client_use, "if (!rise::slayer::IsPortedSlayerRawCastSkill(requestedSkill))",
            "local Slayer cast fails closed before sending unsupported raw ID")
    require(client_use, "IsUnportedSlayerExclusiveMasterSkill(Skill)",
            "selected exclusive mastery cannot fall through to 5.2 attack")
    require(client_skill_manager, "IsUnportedSlayerExclusiveMasterSkill(SkillType)",
            "client demand rejects tooltip-only exclusive mastery IDs")
    require(client_skill_manager, "if (!rise::slayer::MeetsStats(base, level, strength, dexterity))",
            "client base Slayer demand uses the same S21 stats as GS")
    require(client_skill_manager, "tree->GetSkillLevel(782) >= 10",
            "client Pierce demand reads GS-sent ten-point Bat mastery prerequisite")
    require(client_skill_manager, "this->FindHeroSkill(static_cast<ActionSkillType>(782))",
            "client Pierce demand accepts learned Bat mastery lineage")
    require(client_use, "if (!gSkillManager.DemendConditionCheckSkill(\n\t\tstatic_cast<WORD>(requestedSkill)))",
            "deferred Slayer movement cast rechecks current requirements before packet/graph")
    require(client_skill_manager, "SkillType == 781 || SkillType == 782",
            "client Bat mastery checks the pinned S21 level/STR/DEX requirements")
    movement_cast = client_use.split("case MOVEMENT_SKILL:", 1)[1].split(
        "case AT_SKILL_SPEAR:", 1)[0]
    require(movement_cast, "case 781: // S21 Bat Flock Strengthener",
            "pathfinding arrival accepts acquired Bat strengthener raw ID")
    require(movement_cast, "case 782: // S21 Bat Flock Mastery",
            "pathfinding arrival accepts acquired Bat mastery raw ID")
    require(client_use, "SendRequestMagic(requestedSkill, target->Key);",
            "local Slayer cast retains authoritative raw mastery skill ID")
    require(server_overlay, "IsSlayerBatMasterySkill(id) || id == kPierceAttack",
            "GS physical scaling recognizes both Bat mastery IDs")
    require(attack, "rise::slayerserver::IsSlayerBatMasterySkill(skill));",
            "GS Bat mastery damage remains a half-strike")
    require(client_receive, "CanonicalSlayerVisualSkill(skillId) != rise::slayer::kBatFlock",
            "private Bat fanout accepts Brand-resolved mastery cast IDs")
    require(client_receive, "!rise::slayer::IsPortedSlayerRawCastSkill(skillId)",
            "Bat fanout cannot invent an unported Brand-derived cast")
    require(client_receive, "!rise::slayer::IsSlayerClientClass(CharactersClient[casterIndex].Class)",
            "private Bat fanout rejects non-Slayer source classes")
    require(server, "this->GetSkill(lpObj, rise::slayerserver::kBatFlock) == 0",
            "GS Pierce gate requires learned base Bat Flock")
    require(server, "rise::slayerserver::kBatFlockStrengthener) == 0",
            "GS Pierce gate accepts the 5.2 stored Bat mastery lineage")
    require(server, "gMasterSkillTree.GetMasterSkillLevel(lpObj,",
            "GS Pierce gate uses the displayed mastery level, not the zero-based stored level")
    require(server, "rise::slayerserver::kBatFlockMastery) < 10",
            "GS Pierce gate requires 10 Bat Flock mastery points")
    qa = read("ExMain_RISE_PC/Main5.2_RISE/RISE/SlayerRuntimeQA.cpp")
    if "if (!object || !CharactersClient)\n\t\treturn Hero;" in qa:
        raise AssertionError("Slayer QA offensive target silently falls back to caster")
    require(qa, "auto-sequence paused=no-live-monster-target; no offensive cast sent",
            "unattended offensive QA fails closed without live monster")
    require(resources, "actor.Position[2] += 5.f;",
            "S21 Pierce action initializer lifts the selected actor by five units")
    require(resources, "gPierceActorBaselineZ[&actor] = actor.Position[2];",
            "5.2 Pierce lift records its pre-cast actor Z")
    require(resources, "RetirePierceActorLift(actor);\n    actor.CurrentAction = action;",
            "a repeated Pierce cast or another Slayer action retires the prior lift")
    require(resources, "if (fabsf(actor.Position[2] - (it->second + 5.f)) < 0.01f)",
            "Pierce lift retirement cannot overwrite newer movement/server Z")
    require(resources, "if (actor.CurrentAction != kPierceAttackAction)\n        RetirePierceActorLift(actor);",
            "Pierce actor lift retires after the cast action exits")
    require(character_render, "rise::slayer::UpdatePierceActorLift(*o);",
            "normal 5.2 animation transition drives private Pierce lift retirement")
    require(runtime, "if (localSlot >= 0 && gPendingLocalGraphs[localSlot] > 0)",
            "local 0x19 acknowledgment cannot repeat the Pierce action lift")

    for skill_id, (name, handler) in SKILLS.items():
        enum_name = name
        require(shared, f"{name} = {skill_id}", f"shared id {skill_id}")
        require(server_catalog, f"{{ {enum_name},", f"server catalog row {skill_id}")
        require(server, f"case rise::slayerserver::{name}:", f"server dispatch {skill_id}")
        require(server, handler, f"server handler {skill_id}")
        require(client_receive, f"case {skill_id}:", f"client receive case {skill_id}")
        require(client_use, f"rise::slayer::{name}", f"client use route {skill_id}")
        require(runtime, f"case {enum_name}:", f"native runtime case {skill_id}")
        require(resources, f"case {enum_name}:", f"resource case {skill_id}")
        require(packet, f"{{ {name},", f"packet row {skill_id}")
        print(f"PASS: skill={skill_id} name={name[1:]} source-surface=shared/server/receive/use/runtime/resources/packet")

    use_body = client_use.split("void UseSkillSlayer(", 1)[1].split("\n#endif", 1)[0]
    require(use_body, "if (rise::slayer::ApplyCastAction(*pObj, visualSkill))",
            "Slayer local cast uses the imported S21 action immediately")
    require(use_body, "rise::slayer::DispatchNativeLocalCast(pCha, visualTarget, visualSkill);",
            "Slayer local action branch seeds native roots immediately")
    require(use_body, "if (pCha->MovementType == MOVEMENT_SKILL)\n\t\tpCha->MovementType = MOVEMENT_MOVE;",
            "completed Slayer movement intent cannot re-spawn buff roots each frame")
    require(client_use, "if (!MouseRButton && !MouseRButtonPush)\n\t\tslayerSelfBuffHeld = false;",
            "Slayer self-buff input latch re-arms only after right-button release")
    require(client_use, "if ((MouseRButton || MouseRButtonPush) && slayerSelfBuffHeld)\n\t\t\t\t\treturn;",
            "held right-button cannot stack Slayer buff graphs each frame")
    require(runtime, "if (localSlot >= 0 && gPendingLocalGraphs[localSlot] > 0)",
            "GS 0x19 acknowledgment does not duplicate a local Slayer graph")
    require(client_receive, "rise::slayer::ResetNativeRuntime(HeroKey);",
            "re-login clears local cast acknowledgment state")
    require(use_body, "CharactersClient[targetIndex].Object.Kind != KIND_PLAYER",
            "Slayer hostile-player cast route remains available for server PvP checks")
    if "SetPlayerAttack(pCha);" in use_body:
        raise AssertionError("Slayer cast falls back to a legacy weapon action")
    require(server_overlay, "(7 * static_cast<__int64>(strength) +",
            "S21 Lua fractional STR/8 damage numerator")
    require(server_overlay, "2 * static_cast<__int64>(dexterity) + 6720)",
            "S21 Lua fractional DEX/28 damage numerator")
    require(server_overlay, "batFlockHalfStrike ? 11200 : 5600",
            "Bat Flock half strike applies after full fractional rate")
    require(attack, "rise::slayerserver::ScaleSlayerDamage(damage,",
            "Sword/Bat/Pierce damage uses the fractional S21 rate")
    require(server_overlay, "inline int BatFlockDotDamage(int energy)",
            "S21 Bat Flock has a distinct Energy-based DOT formula")
    require(server_overlay, "inline int BatFlockDotDurationSeconds(int skillId)",
            "S21 Bat Flock base/mastery DOT duration selector")
    require(server_overlay, "skillId == kBatFlockMastery ? 15 : 5",
            "S21 Bat Flock mastery 782 adds ten seconds to base five")
    require(server_overlay, "((stat - 800.0) * (stat - 500.0) / 200.0)",
            "S21 FormulaData Character 9 offset terms")
    require(server_overlay, "const double result = formula9 / 100.0;",
            "S21 Lua Bat Flock player/monster DOT result")
    require(server, "rise::slayerserver::BatFlockDotDamage(\n",
            "GS Bat Flock DOT uses S21 distinct calculator")
    require(server, "rise::slayerserver::BatFlockDotDurationSeconds(\n",
            "GS Bat Flock DOT uses raw replacement-skill duration")
    if re.search(r"EFFECT_SLAYER_BAT_FLOCK,\s*5,", server):
        raise AssertionError("Bat Flock DOT duration regressed to fixed base five seconds")
    require(server, "classStage < seed->classRequirement",
            "GS Slayer DB class stage requirement")
    require(server, "rise::slayer::MeetsStats(index, lpObj->Level,",
            "GS Slayer level and STR/DEX requirements")
    require(client_receive, "if (Success)\n\t\t\trise::slayer::DispatchNativeReceive",
            "Slayer visual graph requires an accepted 5.2 cast packet")
    local_cast = runtime.split("bool DispatchNativeLocalCast(", 1)[1].split(
        "bool DispatchNativeReceive(", 1)[0]
    receive_cast = runtime.split("bool DispatchNativeReceive(", 1)[1].split(
        "bool DispatchBatFanout(", 1)[0]
    require(local_cast, "(!target || !target->Object.Live)",
            "new offensive input requires a living selected target")
    require(receive_cast, "if (skillId != kDetection && skillId != kDemolish &&\n        !target)",
            "accepted offensive cast keeps its graph after target death")
    if "(!target || !target->Object.Live)" in receive_cast:
        raise AssertionError("accepted Slayer cast still suppresses visuals after target death")

    for banned in (
        "ak_skill_sword.bmd",
        "ak_skill_sword_s01.bmd",
        "kSwordFlightController",
        "kPierceBurstController",
        "kPierceFanController",
    ):
        if banned in resources or banned in runtime or banned in converter:
            raise AssertionError(f"placeholder residue: {banned}")

    graph_tokens = (
        "kSwordSecondaryController", "kSword68BController",
        "kSword68CController", "kSword68DController",
        "kSword68EController", "kSword68FController", "kSword690Controller",
        "kPierce67AController", "kPierce67BController",
        "kPierce67CController", "kPierce67DController",
        "kPierce67EController", "kPierce680Controller",
        "kBatFlock686Controller", "kBatFlock687Controller",
    )
    for token in graph_tokens:
        require(resources, token, f"native graph node {token}")

    # S21 0x143E57C allocates effect objects, not particle-pool objects.
    # This check is intentionally separate from the five-skill wiring PASS.
    header = read("ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/client/SlayerSkillResources.h")
    require(header, "kDetectionAction = 287,", "native Detection action slot")
    require(header, "kDemolishAction = 288", "distinct native Demolish action slot")
    if "kDemolishAction = kDetectionAction" in header:
        raise AssertionError("Detection/Demolish action IDs were aliased")
    bitmap_block = header.split("enum BitmapId", 1)[1].split("};", 1)[0]
    bitmap_ids = [int(value) for value in re.findall(r"^\s*k\w+Bitmap\s*=\s*(\d+)", bitmap_block, re.M)]
    if len(bitmap_ids) != 36 or len(set(bitmap_ids)) != len(bitmap_ids):
        raise AssertionError("Slayer bitmap IDs are incomplete or duplicated")
    if min(bitmap_ids) != 32983 or max(bitmap_ids) != 33018:
        raise AssertionError("Slayer bitmap IDs overlap Grow Lancer or exceed the reserved tail")
    global_bitmap = read("ExMain_RISE_PC/Main5.2_RISE/GlobalBitmap.cpp")
    require(global_bitmap, "kSlayerLastReservedBitmap = 33018",
            "unnamed allocator private range boundary")
    require(global_bitmap, "m_uiTextureIndexStream = kSlayerLastReservedBitmap",
            "unnamed allocator skips Slayer fixed slots")
    require(global_bitmap, "candidate >= BITMAP_black_gold_crom05 &&",
            "Slayer unnamed allocator detects compiled fixed-loader texture range")
    require(global_bitmap, "candidate = BITMAP_LOADER_TEXTURES_END + 1;",
            "Slayer unnamed allocator skips fixed-loader texture range")
    require(global_bitmap, "m_mapBitmap.find(candidate) != m_mapBitmap.end()",
            "Slayer unnamed allocator cannot reuse a loaded named texture ID")
    require(resources, "RegisterSlayerBitmap(kMarksM04Bitmap,",
            "S21 Pierce 0x81CE marks_m04 private bitmap registration")
    require(resources, "RegisterSlayerBitmap(kMarksM03Bitmap,",
            "S21 Pierce 0x81CD marks_m03 private bitmap registration")
    require(resources, "RegisterSlayerBitmap(kFlare01Bitmap,",
            "S21 0x80BA subtype-6 flare01 private bitmap registration")
    require(resources, "RegisterSlayerBitmap(kShockWaveBitmap,",
            "S21 0x8012 subtype-17 ShockWave private bitmap registration")
    require(resources, "RegisterSlayerBitmap(kClud64Bitmap,",
            "S21 Pierce flare particle 0x7FFD Clud64 private bitmap registration")
    require(header, "kBetGrilsShot2GoldBitmap = 33017",
            "S21 Bat subtype-4 gold overlay owns isolated bitmap ID")
    require(resources, "RegisterSlayerBitmap(kBetGrilsShot2GoldBitmap,",
            "S21 Bat subtype-4 gold overlay bitmap registered")
    require(resources, '"Data\\\\RISE\\\\Slayer\\\\Effect\\\\bet_grilsshot2gold.jpg"',
            "S21 Bat gold overlay exact model texture path")
    require(header, "kFlareRedBitmap = 33018",
            "S21 Bat subtype-2/3 bone sprite owns isolated bitmap ID")
    require(resources, "RegisterSlayerBitmap(kFlareRedBitmap,",
            "S21 Bat bone flareRed bitmap registered")
    require(resources, '"Data\\\\RISE\\\\Slayer\\\\Effect\\\\flareRed.jpg"',
            "S21 Bat bone flareRed texture path")
    require(resources, "if (modelId != kPierceMarksCylinderModel)\n        return true;",
            "registered S21 0x688/0x691/0x694 RGB materials are not keyed")
    if not re.search(r"case kBatFlockTrailModel:\s*// Native 0x1490F57[^\n]*\n(?:\s*//[^\n]*\n)*\s*if \(effect.SubType == 0\)\s*\{\s*effect.Scale = incomingScale;\s*effect.Alpha = 0.f;", resources):
        raise AssertionError("S21 0x688 Bat Flock trail must write incoming scale and zero alpha")
    require(resources, "float NativeRandomUnitStep(int lower, int upper)",
            "native Bat Flock unit-step integer random adapter")
    require(resources, "NativeRandomUnitStep(60, 120) * 0.01f",
            "S21 0x678 inclusive unit-step model scale envelope")
    require(resources, "NativeRandomUnitStep(-10, 10) *",
            "S21 0x678 subtype-0 discrete per-frame yaw jitter")
    require(resources, "NativeRandomUnitStep(upwardZ ? 0 : -100, 100)",
            "S21 Bat Flock orbit/joint signed unit-step spread")
    require(resources, "const float x = NativeRandomUnitStep(-100, 100);\n    const float z = NativeRandomUnitStep(upwardZ ? 0 : -100, 100);",
            "Bat Flock spread samples X before Z like native blocks")
    require(resources, "NativeRandomUnitStep(40, 90)",
            "S21 Bat Flock main pulse unit-step spread magnitude")
    if "NativeRandomRange(" in resources:
        raise AssertionError("continuous random adapter remains in S21 unit-step Slayer graph")
    if not re.search(r"case kDetectionImpactModel:\s*// Native 0x14926CC[^\n]*\n(?:\s*//[^\n]*\n)*\s*effect.Scale = incomingScale;\s*effect.Alpha = 0.f;", resources):
        raise AssertionError("S21 0x694 both modes must write incoming scale and zero alpha")
    require(header, "kPierceMarksCylinderModel = MAX_MODELS + 41,",
            "native Pierce 0x5D8 child model allocated to private unused slot")
    require(resources, '{kPierceMarksCylinderModel, "marks_cylinder.bmd"}',
            "native Pierce 0x5D8 model resource mapping")
    for token in (
        "CreateEffect(kPierce81CDController,",
        "SpawnBitmapChild(kPierce81CEEffect, effect, effect.Owner,",
        "SpawnBitmapChild(kPierce80BAEffect, effect, effect.Owner,",
        "SpawnBitmapChild(kPierce8149Effect, effect, effect.Owner,",
        "effect.Timer = WorldTime;",
        "WorldTime - effect.Timer > 6000.f",
        "bitmap = effect.SubType == 3 ? kMarksM04Bitmap :",
    ):
        require(resources, token, f"first native Pierce 0x81CD/0x81CE graph leg {token}")
    print("PASS: Slayer bitmap IDs 32983..33018 do not overlap Grow Lancer or unnamed allocation")
    require(header, "kPierce8149Effect = MAX_MODELS + 79",
            "native Pierce 0x8149 subtype-2 sprite object private slot")
    require(resources, "CreateSprite(kGroundStarBitmap, effect.Position, effect.Scale,",
            "native Pierce 0x8149 renders through sprite queue, not terrain bitmap")
    require(resources, "if (effect.LifeTime <= 2.f)",
            "native Pierce 0x8149 near-zero lifetime refresh")
    require(resources, "SpawnBitmapChild(kMagicGround12Effect, effect, effect.Owner,",
            "native Pierce 0x81CF subtype-2 ring parent creation")
    require(header, "kPierceShockWaveEffect = MAX_MODELS + 80",
            "native Pierce nested 0x8012 subtype-17 effect private slot")
    require(resources, "SpawnBitmapChild(kPierceShockWaveEffect, effect, 0,",
            "native 0x81CF nested 0x8012 has no owner")
    require(resources, "gMapManager.InHellas() && !gMapManager.InHiddenHellas()",
            "native 0x8012 draw is Kalima-only")
    require(resources, "RenderWaterTerrain(kShockWaveBitmap, effect.Position[0],",
            "native 0x8012 water-terrain pass adapter")
    require(resources, "case kMagicGround12Effect: return effect.SubType == 2 ? 15.f : 30.f;",
            "native Pierce 0x81CF subtype-2 15-tick envelope")
    require(resources, "effect.Scale += 0.15f * animationFactor;",
            "native Pierce 0x81CF subtype-2 scale expansion")
    require(resources, "effect.Alpha = effect.LifeTime / 15.f;",
            "native Pierce 0x81CF subtype-2 lifetime opacity")
    require(resources, "SpawnChild(kPierceMarksCylinderModel, cylinder, effect.Owner, 1, 0.f);",
            "native Pierce 0x5D8 subtype-1 model child creation")
    require(resources, "case kPierceMarksCylinderModel: return 30.f;",
            "native Pierce 0x5D8 subtype-1 30-tick life")
    require(resources, "else if (effect.Type == kPierceMarksCylinderModel)",
            "native Pierce 0x5D8 map-independent bright model callback")
    require(resources, "const float phase = static_cast<float>(WorldTime) * 0.005f;",
            "S21 float-clock phase before double-precision sine")
    require(resources, "return (sinusoid + 1.f) * 0.25f + 0.2f;",
            "native Pierce cylinder time-wave light")
    if resources.count("const float wave = NativeSlayerLightWave();") != 2:
        raise AssertionError("S21 cylinder and 0x80BA flare must share the pinned light wave")
    require(resources, "VectorScale(effect.Light, wave, model.BodyLight);",
            "native Pierce cylinder callback scales object RGB light")
    require(resources, "Vector(180.f, 0.f, 0.f, turn);",
            "native Pierce three 0x80BA subtype-7 lanes use fixed 180-degree transform")
    require(resources, "SpawnBitmapChild(kPierce80BAEffect, flareLane, effect.Owner,",
            "native Pierce three separate subtype-7 flare objects")
    require(resources, "CreateParticle(kSmokeLines01Bitmap + rand() % 3,",
            "native Pierce subtype-7 first per-frame smokeline child")
    particle = read("ExMain_RISE_PC/Main5.2_RISE/ZzzEffectParticle.cpp")
    require(particle, "if (SubType == 9)",
            "S21 smokelines subtype-9 constructor")
    require(particle, "if (o->SubType == 9)",
            "S21 smokelines subtype-9 updater")
    if resources.count("CreateParticleTexture(BITMAP_CLUD64, kClud64Bitmap,") != 2:
        raise AssertionError("Pierce must emit two distinct native Clud64 children")
    require(particle, "o->SubType == 17 && o->TexType == rise::slayer::kClud64Bitmap",
            "S21 Clud64 subtype-17 private constructor/updater")
    for token in (
        "kFlare01RedEffect", "kRingOfGradation2Effect",
        "kEnemyRing01Effect", "kMagicGround12Effect",
        "kFlareBlueEffect", "kFlareEffect",
    ):
        require(header, token, f"secondary-pool effect id {token}")
        require(resources, f"SpawnBitmapChild({token}",
                f"secondary-pool creation {token}")
    for bitmap in (
        "kRingOfGradation2Bitmap", "kEnemyRing01Bitmap",
        "kMagicGround12Bitmap", "kFlareBlueBitmap",
        "kFlareBitmap", "kMarksM04Bitmap", "kMarksM03Bitmap",
    ):
        if f"CreateParticle({bitmap}" in resources:
            raise AssertionError(f"effect object accidentally allocated as particle: {bitmap}")
    require(resources, "if (IsBitmapEffect(effect.Type))", "bitmap effect runtime")
    require(effect_header, "void ClearSlayerOwnerEffectGraph(OBJECT* owner);",
            "private Slayer two-pool owner-effect cleanup declaration")
    require(effect_allocator, "rise::slayer::ReleaseEffectSidecars(*o);",
            "Slayer target-list sidecars released at effect destruction")
    require(effect_allocator, "if (clearPierceChildren)",
            "S21 0x81CD destructor-owned child cleanup")
    require(effect_allocator, "void ClearSlayerOwnerEffectGraph(OBJECT* owner)",
            "S21 Pierce owner-effect cleanup bridge")
    require(effect_allocator, "OBJECT* child = &Effects[i];",
            "S21 Pierce primary effect-pool cleanup")
    require(effect_allocator, "OBJECT* child = g_SkillEffects.GetEffect(i);",
            "S21 Pierce secondary effect-pool cleanup")
    require(effect_allocator, "rise::slayer::IsEffectType(child->Type)",
            "Pierce cleanup remains private to Slayer effect nodes")
    slayer_move = effect_allocator.split(
        "void MoveEffect(OBJECT* o, int iIndex)", 1)[1].split("#endif", 1)[0]
    if not re.search(
            r"rise::slayer::UpdateEffect\(\*o, FPS_ANIMATION_FACTOR\);"
            r"[\s\S]*?if \(o->LifeTime <= 0\.0f\)\s*"
            r"EffectDestructor\(o\);\s*else\s*"
            r"o->LifeTime -= FPS_ANIMATION_FACTOR;", slayer_move):
        raise AssertionError("S21 Slayer effect life must be checked before final tick decrement")
    pierce_cast_action = resources.split("bool ApplyCastAction", 1)[1]
    pierce_cast_action = pierce_cast_action.split(
        "if (skillId == kPierceAttack)", 1)[1].split("return true;", 1)[0]
    lift = pierce_cast_action.find("actor.Position[2] += 5.f;")
    clear = pierce_cast_action.find("ClearSlayerOwnerEffectGraph(&actor);")
    root = pierce_cast_action.find("CreateEffect(kPierce81CDController,")
    if not (0 <= lift < clear < root):
        raise AssertionError("S21 Pierce pre-root owner cleanup is out of action-init order")
    pierce_mark_init = resources.split("void InitializeEffect", 1)[1]
    pierce_mark_init = pierce_mark_init.split("case kPierce81CEEffect:", 1)[1]
    pierce_mark_init = pierce_mark_init.split("case kPierce80BAEffect:", 1)[0]
    require(pierce_mark_init, "effect.Alpha = 1.f;",
            "S21 Pierce 0x81CE modes 3/4/5 effective initial alpha one")
    require(resources, "case kFlareBlueEffect: // 0x7FDD subtype 2, update 0x157D57E",
            "S21 Detection high-code flare updater")
    flare_blue_update = resources.split(
        "case kFlareBlueEffect: // 0x7FDD subtype 2, update 0x157D57E", 1)[1]
    flare_blue_update = flare_blue_update.split("case kMagicGround12Effect:", 1)[0]
    require(flare_blue_update, "half * animationFactor;",
            "S21 Detection flare alpha follows normalized 5.2 lifetime")
    require(resources, "effect.Angle[2] = 0.f;",
            "S21 Detection 0x7FDD subtype-2 initial zero rotation")
    require(flare_blue_update, "effect.Angle[2] += 5.f * animationFactor;",
            "S21 Detection 0x7FDD five-degree normalized 5.2 rotation")
    bitmap_render = resources.split("bool RenderEffect(OBJECT& effect)", 1)[1]
    bitmap_render = bitmap_render.split("if (IsController(effect.Type))", 1)[0]
    blend_begin = bitmap_render.find("EnableAlphaBlend();")
    terrain_submit = bitmap_render.find("RenderTerrainAlphaBitmap(bitmap,")
    blend_end = bitmap_render.find("DisableAlphaBlend();")
    if not (0 <= blend_begin < terrain_submit < blend_end):
        raise AssertionError("Slayer terrain bitmap is not isolated in an additive blend pass")
    require(resources, "SpawnChild(kDetectionChildController, effect, effect.Owner",
            "0x693 caster owner")
    require(resources, "SpawnChild(kDemolishChildController, effect, effect.Owner",
            "0x696 caster owner")
    require(resources, "SpawnBitmapChild(kMagicGround12Effect, effect, &effect",
            "0x81CF root owner")
    require(resources, "SpawnBitmapChild(kRingOfGradation2Effect, effect, 0",
            "0x82F6 null owner")
    require(resources, "SpawnChild(kDetectionChildController, effect, effect.Owner, 0, 0.f)",
            "0x693 native zero-scale seed")
    require(resources, "SpawnChild(kDemolishChildController, effect, effect.Owner, 0, 0.f)",
            "0x696 native zero-scale seed")
    require(resources, "4, 0.f);\n        SpawnBitmapChild(kMagicGround12Effect",
            "0x81CF mode 4 native zero-scale seed")
    require(resources, "5, 0.f);\n        SpawnBitmapChild(kFlareBlueEffect",
            "0x81CF mode 5 native zero-scale seed")
    require(resources, "SpawnChild(kPierceSwordLineModel, effect, owner, 0",
            "0x679 center 0x681 caster owner")
    pierce_root_update = resources.split("case kPierceController:", 2)[-1].split(
        "case kPierce67AController:", 1)[0]
    require(pierce_root_update, "if (!owner || !owner->Live)",
            "0x679 stale owner destruction")
    require(pierce_root_update, "VectorCopy(effect.StartPosition, effect.Position);",
            "0x679 saved launch-position restore")
    require(pierce_root_update,
            "effect.AnimationFrame += effect.Velocity * animationFactor;",
            "0x679 isolated dispatcher advances the native effect frame")
    require(pierce_root_update, "effect.AnimationFrame >= 4.f",
            "0x679 center root-frame gate")
    require(pierce_root_update, "effect.AnimationFrame >= 7.f",
            "0x679 mirrored flank root-frame gate")
    if "owner->AnimationFrame >=" in pierce_root_update:
        raise AssertionError("0x679 incorrectly gated on owner animation")
    pierce_67a = resources.split("case kPierce67AController:", 2)[2].split(
        "case kPierce67BController:", 1)[0]
    require(pierce_67a, "!owner->Owner->Live",
            "0x67A two-level owner liveness gate")
    require(pierce_67a, "effect.AnimationFrame >= 6.f",
            "0x67A native frame-six alpha gate")
    require(pierce_67a, "effect.AnimationFrame / 7.f",
            "0x67A native seven-frame fade denominator")
    require(pierce_67a, "effect.AnimationFrame >= 1.f",
            "0x67A native child launch frame")
    require(pierce_67a,
            "SpawnChild(kPierce67BController, launch, owner, 0, 0.7f);",
            "0x67A one 0x67B child with native owner and scale")
    if pierce_67a.count("SpawnChild(kPierce67BController") != 2:
        raise AssertionError("0x67A must contain one normal and one early-end fallback launch")
    pierce_67b = resources.split("case kPierce67BController:", 2)[2].split(
        "case kPierce67CController:", 1)[0]
    require(pierce_67b, "effect.Velocity *",
            "0x67B native velocity-driven flight")
    require(pierce_67b, "VectorDistance3(effect.Position, effect.StartPosition)",
            "0x67B outbound distance state")
    require(pierce_67b, "VectorDistance3(effect.Position, owner->Position)",
            "0x67B return distance state")
    require(pierce_67b, "CreateParticle(kImpack03Bitmap",
            "0x67B native 0x8021 particle route")
    require(pierce_67b, "CreateParticle(kPinStarBitmap",
            "0x67B native 0x8108 particle route")
    require(pierce_67b, "SpawnChild(kPierce680Controller",
            "0x67B per-frame 0x680 child")
    require(pierce_67b, "effect.LifeTime += animationFactor;",
            "0x67B lifetime hold until return completion")
    pierce_67e = resources.split("case kPierce67EController:", 2)[2].split(
        "case kSword68CController:", 1)[0]
    require(pierce_67e, "NativeRandomUnitStep(-30, 30)",
            "0x67E native signed XY particle spread")
    require(pierce_67e, "NativeRandomUnitStep(-15, 15) + 20.f",
            "0x67E native raised Z spread")
    require(pierce_67e,
            "CreateParticle(kPinStarBitmap, position, angle, light, 4, 1.f",
            "0x67E native 0x80F0 subtype-four particle")
    buff_update = resources[resources.index("case kDetectionController:",
        resources.index("void UpdateEffect")):].split("default:", 1)[0]
    require(buff_update, "VectorCopy(effect.StartPosition, effect.Position);",
            "0x692/0x695 root smoke cast-origin anchor")
    require(buff_update, "const bool smoke01 = (rand() & 1) == 0;",
            "S21 0x692/0x695 smoke family selected before coordinates")
    require(buff_update, "position[0] += static_cast<float>((rand() % 61) - 30);",
            "S21 0x692/0x695 independent signed X offset")
    require(buff_update, "position[1] += static_cast<float>((rand() % 61) - 30);",
            "S21 0x692/0x695 independent signed Y offset")
    if not (buff_update.index("const bool smoke01 = (rand() & 1) == 0;") <
            buff_update.index("position[0] += static_cast<float>((rand() % 61) - 30);") <
            buff_update.index("position[1] += static_cast<float>((rand() % 61) - 30);")):
        raise AssertionError("S21 buff smoke RNG order drifted")
    if "cosf(yaw)" in buff_update or "sinf(yaw)" in buff_update:
        raise AssertionError("S21 buff smoke reverted to nonnative circle")
    require(buff_update, "static_cast<float>(rand() % 361) :",
            "S21 0x691 even-life wave uses inclusive 0..360 angle")
    require(buff_update, "-static_cast<float>(1 + rand() % 360);",
            "S21 0x691 odd-life wave uses negative inclusive 1..360 angle")
    require(buff_update, "VectorCopy(effect.StartPosition, bat.Position);",
            "0x693/0x696 child 0x678 cast-origin anchor")
    if "VectorCopy(owner->Position, effect.Position);" in buff_update or \
       "VectorCopy(owner->Position, bat.Position);" in buff_update:
        raise AssertionError("buff effects incorrectly follow moving owner")
    require(resources, "SpawnChild(kPierceSwordLineModel, swordLine, &effect, 3",
            "0x68B 0x681 immediate-parent owner")
    require(resources, "effect.Type == kMagicGround12Effect ? 1.f : 0.f",
            "0x81CF subtype 4/5 starts fully lit")
    bitmap_update = resources[resources.index("if (IsBitmapEffect(effect.Type))",
              resources.index("void UpdateEffect")):]
    bitmap_update = bitmap_update[:bitmap_update.index("if (IsController(effect.Type))")]
    require(bitmap_update, "effect.LifeTime > half ? 1.f : -1.f",
            "native half-life bitmap alpha triangle")
    require(bitmap_update, "effect.Alpha -= animationFactor / initialLife",
            "0x81CF linear alpha decrease")
    require(bitmap_update, "effect.SubType == 4 ? 0.3f : 1.f",
            "0x81CF independent ring expansion")
    if "fadeIn" in bitmap_update or "effect.Owner->Position" in bitmap_update:
        raise AssertionError("dump-unproven bitmap quarter-fade or owner-follow rule")
    print("PASS: ten S21 bitmap effect-object nodes use the 5.2 effect pool, not particle pool")

    for texture in ("kGhostMark02Bitmap", "kGhostMark02RedBitmap"):
        require(resources, f"CreateJoint({texture}",
                f"Bat Flock native joint creation {texture}")
        require(joints, f"case rise::slayer::{texture}:",
                f"private joint constructor/update {texture}")
    require(joints, "o->NumTails = -1;", "native joint initial tail sentinel")
    require(joints, "o->MaxTails = 10;", "0x82EF native joint tail capacity")
    require(joints, "o->MaxTails = 5;", "0x82F3 native joint tail capacity")
    require(joints, "o->Weapon * 0.5f", "native joint half-life fade")
    require(resources, "CreateJoint(BITMAP_FORCEPILLAR, effect.Position",
            "0x678 subtype 1 native 0x80E3 joint-pool child")
    require(resources, "effect.LifeTime >= initialLife",
            "0x683 guaranteed first-frame red joint")
    require(server, "BatFanoutWire fanout = {};",
            "Bat Flock distinct server fanout envelope")
    require(server, "int candidates[rise::slayer::kBatFanoutMaxTargets] = { bIndex };",
            "Bat Flock server multi-target candidate list")
    require(server, "this->CheckSkillTarget(caster, index, -1, type)",
            "Bat Flock server native legal-target gate")
    require(server, "fanout.count = static_cast<BYTE>(affectedCount);",
            "Bat Flock visual packet uses exact affected set")
    bat_server = server.split("bool CSkillManager::SkillSlayerBatFlock", 1)[1].split(
        "bool CSkillManager::SkillSlayerPierceAttack", 1)[0]
    require(bat_server, "gMap[caster->Map].CheckAttr(gObj[index].X,",
            "Bat Flock fanout excludes 5.2 safe-zone targets")
    if bat_server.count("this->BasicSkillAttack(aIndex, index, lpSkill, false)") != 2:
        raise AssertionError("Webzen Bat Flock contact damage must hit each target twice before DOT")
    require(server, "DetectionWire reveal = {};",
            "Detection server-authoritative minimap reveal start")
    require(client_receive, "case rise::slayer::kDetectionSub:",
            "Detection private reveal receiver")
    require(runtime, "bool DispatchDetectionReveal(",
            "Detection local-Slayer reveal bridge")
    require(minimap, "void SEASON3B::CNewUIMiniMap::RenderSlayerDetection()",
            "Detection full-minimap moving life-form markers")
    require(minimap, "GetTickCount() - m_SlayerDetectionStartTick >=",
            "Detection server-supplied duration expiry")
    require(client_receive, "case rise::slayer::kBatFanoutSub:",
            "Bat Flock distinct client fanout receiver")
    require(runtime, "bool DispatchBatFanout(",
            "Bat Flock supplemental two-root graph")
    require(resources, "SetBatFlockTargets(OBJECT& effect",
            "Bat Flock private target-list sidecar")
    require(resources, "pulse.m_sTargetIndex = PickBatFlockTargetIndex(effect);",
            "0x682 random list target per child pulse")
    require(resources, "EmitBatTargetJoint(effect, PickBatFlockTarget(effect));",
            "0x683 random list target per joint pulse")
    require(resources, "if (effect.SubType == 0)",
            "packet-born 0x682 subtype1 skips 0x688 initializer")
    bat_graph = runtime.split("void CreateBatFlockGraph", 1)[1].split(
        "void CreatePierceAttackGraph", 1)[0]
    sword_graph = runtime.split("void CreateSwordInertiaGraph", 1)[1].split(
        "void CreateBatFlockGraph", 1)[0]
    require(sword_graph, "SpawnModel(kPierceController,",
            "base Sword action creates S21 0x679 root")
    if "SpawnModel(kSwordInertiaController," in sword_graph or "SpawnModel(kSwordSecondaryController," in sword_graph:
        raise AssertionError("0x818-only 0x68A/0x689 leaked into base Sword")
    require(bat_graph, "kBatFlock, 0, 0.f);",
            "0x682 canonical root has an empty target list")
    require(bat_graph, "caster, kBatFlock, 0, 0.f);",
            "0x683/0x684 canonical roots have empty target lists")
    if "kBatFlock, &target, 0.f);" in bat_graph:
        raise AssertionError("packet target leaked into Bat Flock visual roots")
    pierce_graph = runtime.split("void CreatePierceAttackGraph", 1)[1].split(
        "void CreateDetectionGraph", 1)[0]
    require(pierce_graph, "SpawnModel(kSwordInertiaController,",
            "base Pierce 0x126 uses the native 0x68A root")
    if "SpawnModel(kPierceController," in pierce_graph:
        raise AssertionError("upgrade-only 0x679 leaked into base Pierce")
    require(pierce_graph, "kPierceAttack, 0, 0.f);",
            "base Pierce 0x68A root has no target-list adapter")
    pierce_wire = read(
        "ExMain_RISE_PC/Main5.2_RISE/RISE/Slayer/shared/SlayerPierceFanoutWire.h")
    require(pierce_wire, "kPierceFanoutSub = 0xE5",
            "private Pierce list-bearing packet subtype")
    require(pierce_wire, "kPierceLaneRequestSub = 0xE6",
            "private Pierce per-lane request subtype")
    require(pierce_wire, "static_assert(sizeof(PierceLaneRequestWire) == 12",
            "Pierce per-lane packet remains unpadded")
    require(client_receive, "case rise::slayer::kPierceFanoutSub:",
            "Pierce supplemental receive parser")
    require(runtime, "bool DispatchPierceFanout(",
            "Pierce 0x689 list-bearing supplemental root")
    require(runtime, "SetPierceTargets(*listRoot, targetIndexes, targetCount, castSerial);",
            "Pierce resolved target-list and cast serial owner")
    require(client_receive, "wire.serial);",
            "Pierce accepted fanout serial reaches effect sidecar")
    require(resources, "SendPierceLaneRequest(effect, effect.m_sTargetIndex,",
            "local 0x689 emits one request per selected lane")
    require(resources, "effect.Owner != &Hero->Object",
            "remote 0x689 effects never send skill requests")
    require(resources, "gPierceLaneDirection >= 50 ? 1 : gPierceLaneDirection + 1",
            "Pierce native outbound direction byte cycles through 1..50")
    require(resources, "gPierceCastSerial.erase(&effect);",
            "Pierce cast serial cleared on effect-pool reuse")
    require(server_protocol, "gSkillManager.CGSlayerPierceLaneRecv(lpMsg, size, aIndex);",
            "GS routes private F4:E6 lane request")
    require(resources, "gPierceTargets.erase(&effect);",
            "Pierce sidecar cleared on effect-pool reuse")
    require(resources, "PierceTargetAt(effect, effect.CurrentAction)",
            "Pierce 0x689 list traversal")
    require(resources, "Vector(0.f, -1.f, 0.f, nativeForward);",
            "0x689 native forward basis")
    require(resources, "VectorRotate(nativeForward, launchMatrix,",
            "0x689 launch uses native angle-matrix vector rotation")
    require(resources, "launch.Position[0] -= rotatedForward[0] * 100.f;",
            "0x689 launch backs from target rather than overshooting it")
    require(resources, "const float sourceScale = effect.Scale;",
            "0x689 passes root effect scale to Sword/Pierce child models")
    pierce_root = resources.split("case kPierceController:", 2)[2].split(
        "case kPierce67AController:", 1)[0]
    require(pierce_root, "SpawnChild(kPierce67AController, flank, &effect, subtype,\n                    owner->Scale);",
            "0x679 flank children use actor scale, unlike 0x689 lanes")
    if "if (rise::slayer::IsEffectType(Type))\n                o->Scale = Scale;" in effect_allocator:
        raise AssertionError("Slayer effect allocator bypasses S21 0.9 zero-scale fallback")
    require(effect_allocator, "if (Scale <= 0.0f)\n                o->Scale = 0.9f;",
            "Slayer effects retain S21/5.2 non-positive scale fallback")
    require(effect_allocator, "rise::slayer::InitializeEffect(*o, Scale);",
            "Slayer initializer receives raw CreateEffect scale")
    if not re.search(r"case kDetectionMarkModel:\s*// 0x1492259[^\n]*\n(?:\s*//[^\n]*\n)*\s*if \(effect.SubType == 0\)\s*\{\s*effect.Scale = incomingScale;\s*effect.Alpha = 0.f;", resources):
        raise AssertionError("0x691 subtype-0 must restore raw scale and zero alpha after allocator fallback")
    require(resources, "if (effect.SubType == 4 || effect.SubType == 5)\n            effect.Scale = incomingScale;",
            "0x81CF buff rings start at raw zero scale")
    require(resources, "if (pierceList && effect.SubType == 2)",
            "Pierce lane advances only on child completion state")
    require(resources, "owner->SubType = 2;",
            "0x68B child completes owning Pierce 0x689 lane")
    require(resources, "gPierceCastTargetIndex[&effect] = list.front();",
            "5.2 Pierce fanout keeps the clicked target separate from lane index")
    require(resources, "!ResolveTarget(original->second)",
            "0x689 validates its fixed original cast target each frame")
    require(resources, "model.PlayAnimation(&effect.AnimationFrame,",
            "0x68B completion follows native 5.2 BMD action keys")
    require(character_render, "rise::slayer::ApplyPlayerActionSpeeds(CharacterAttribute->AttackSpeed);",
            "5.2 attack-speed refresh applies S21 Slayer-specific action speeds")
    require(resources, "const float term = attackSpeed * 0.002f;",
            "S21 Slayer action speed uses its decoded attack-speed multiplier")
    require(resources, "Actions[kSwordInertiaAction].PlaySpeed = 0.43f + term;",
            "S21 Sword E0 native base action speed")
    initializer = resources[resources.index("void InitializeEffect("):
                            resources.index("void UpdateEffect(")]
    sword_root = initializer[initializer.index("case kPierceController:"):
                             initializer.index("case kPierceSwordLineModel:")]
    require(sword_root, "effect.Velocity = Models[MODEL_PLAYER].Actions[",
            "0x679 Sword controller follows native E0 PlaySpeed")
    require(sword_root, "kSwordInertiaAction].PlaySpeed;",
            "0x679 native E0 action selection")
    require(resources, "Actions[kBatFlockAction].PlaySpeed = 0.40f + term;",
            "S21 Bat E3 native base action speed")
    require(resources, "Actions[kPierceAttackAction].PlaySpeed = 0.40f + term;",
            "S21 Pierce E4 native base action speed")
    require(resources, "Actions[kDetectionAction].PlaySpeed = 0.10f + term;",
            "S21 Detection E8 native base action speed")
    require(resources, "Actions[kDemolishAction].PlaySpeed = 0.10f + term;",
            "S21 Demolish E9 native base action speed")
    require(resources, "effect.Velocity = Models[MODEL_PLAYER].Actions[\n                effect.Owner->CurrentAction].PlaySpeed;",
            "0x681 subtype 3 copies its owner-current-action PlaySpeed")
    require(resources, "effect.Velocity = 0.40f + attackSpeedTerm;",
            "0x681 E1/E2 flank modes use native speed formula without extra player clips")
    if "effect.Velocity = 0.35f;" in resources:
        raise AssertionError("provisional fixed .35 animation speed returned to S21 0x681")
    require(resources, "Models[kPierceSwordLineModel].Actions[0].PlaySpeed =\n                effect.Velocity;",
            "0x681 copies selected player speed into its zero-mesh model")
    require(resources, "actor.Velocity = Models[MODEL_PLAYER].Actions[action].PlaySpeed;",
            "Slayer cast actor follows decoded action speed instead of fixed .35")
    require(resources, "model.Actions[model.CurrentAction].PlaySpeed",
            "0x68B animation speed comes from target action")
    require(resources, "const float frame = owner->AnimationFrame;",
            "0x68A opacity follows owning actor's frame")
    require(resources, "frame / 3.5f : 1.f - (frame - 3.5f) / 3.5f",
            "0x68A S21 3.5-frame opacity triangle")
    require(resources, "if (rand() % 6 == 0)",
            "0x68A 0x691 child uses native one-in-six gate")
    require(resources, "markAnchor.Angle[2] = NativeRandomUnitStep(0, 360);",
            "0x68A 0x691 child rotates around the native yaw range")
    require(resources, "SpawnChild(kDetectionMarkModel, markAnchor, &effect,",
            "0x68A randomized model child retains its root owner")
    if "effect.Timer >= 8.f" in resources:
        raise AssertionError("provisional eight-tick Pierce lane timer regressed")
    # 0x81CE subtype 4 and 0x81CF subtype 2 both refresh an exact decoded
    # one-tick envelope. Keep the generic Pierce-lane fallback prohibited:
    # only those two bitmap-object update cases may contain the condition.
    scoped_update = resources[resources.index("void UpdateEffect("):]
    if resources.count("effect.LifeTime <= 1.f") != 2 or not re.search(
        r"case kPierce81CEEffect:.*?if \(effect\.LifeTime <= 1\.f\).*?case kFlare01RedEffect:",
        scoped_update, re.S,
    ) or not re.search(
        r"case kMagicGround12Effect:.*?if \(effect\.LifeTime <= 1\.f\).*?default: break;",
        scoped_update, re.S,
    ):
        raise AssertionError("provisional lifetime Pierce helper fallback regressed")
    require(server, "rise::slayer::PierceFanoutWire fanout = {};",
            "server-authoritative Pierce target-key fanout")
    pierce_server = server.split("bool CSkillManager::SkillSlayerPierceAttack", 1)[1].split(
        "bool CSkillManager::SkillSlayerDetection", 1)[0]
    initial_pierce, lane_pierce = pierce_server.split(
        "void CSkillManager::CGSlayerPierceLaneRecv", 1)
    require(initial_pierce, "kPierceFanoutMaxTargets; ++n)",
            "Pierce AOE victim selection respects S21 list cap")
    require(initial_pierce, "this->CheckSkillTarget(caster, index, -1, type)",
            "Pierce multi-target adapter retains 5.2 legal-target gate")
    require(initial_pierce, "gMap[caster->Map].CheckAttr(gObj[index].X,",
            "Pierce fanout excludes targets its delayed lane would reject")
    require(initial_pierce, "gDuel.GetDuelArenaBySpectator(index) == 0",
            "Pierce fanout excludes duel spectators before sending lanes")
    if "BasicSkillAttack(aIndex, index, lpSkill" in initial_pierce:
        raise AssertionError("Pierce damage applied before S21 0x689 lane request")
    require(initial_pierce, "casts.push_back(pending);",
            "Pierce accepted cast retains concurrent GS authorization sessions")
    require(initial_pierce, "casts.size() >= kSlayerPierceMaxPendingPerCaster",
            "Pierce concurrent cast queue is bounded and fails closed")
    require(initial_pierce, "prior.serial == pending.serial",
            "Pierce cast serial cannot collide with an in-flight cast")
    require(server, "Keep fully consumed and previous-map sessions as serial tombstones",
            "Pierce replay serials remain reserved until the 15-second window expires")
    require(initial_pierce, "fanout.serial = pending.serial;",
            "Pierce fanout binds client lanes to GS cast serial")
    require(initial_pierce, "fanout.count = static_cast<BYTE>(candidateCount);",
            "Pierce fanout count matches authorized GS targets")
    require(lane_pierce, "pending.connectedAt != caster->ConnectTickCount",
            "Pierce lane cannot survive a reused GS connection")
    require(lane_pierce, "cast.serial == lane.serial",
            "Pierce lane matches its accepted cast")
    require(lane_pierce, "lane.direction < 1 || lane.direction > 50",
            "Pierce lane direction uses S21 helper's 1..50 sequence range")
    require(lane_pierce, "matched->consumed[ordinal] = true;",
            "Pierce target lane consumed before damage")
    require(lane_pierce, "matched->openedAt != pending.openedAt",
            "Pierce lane cannot consume a replacement cast after serial wrap")
    require(lane_pierce, "PruneSlayerPierceCasts(it->second",
            "Pierce expired cast sessions release bounded queue slots")
    require(lane_pierce, "const bool batMarked = gEffectManager.CheckEffect",
            "Pierce Bat mark captured when an authorized lane lands")
    require(lane_pierce, "const int strikes = batMarked ? 4 : 2;",
            "Pierce two/four S21 strike contract remains GS authoritative")
    require(lane_pierce, "gAttack.Attack(caster, target, skill, 1, 0, 0, 0, false);",
            "Pierce delayed lane applies native 5.2 damage after saved-origin range check")
    require(server, "if (!gEffectManager.AddEffect(target, 0, EFFECT_SLAYER_DEMOLISH,",
            "Demolish does not broadcast a rejected authoritative buff")
    demolish_server = server.split("bool CSkillManager::SkillSlayerDemolish", 1)[1]
    demolish_apply = demolish_server.split("auto ApplyTo =", 1)[1].split(
        "if (OBJECT_RANGE(lpObj->PartyNumber)", 1)[0]
    if "GCSkillAttackSend" in demolish_apply:
        raise AssertionError("party Demolish duplicates its caster cast graph per recipient")
    require(character_render, "struct ScopedSlayerActionOpacity",
            "Sword and Pierce action alpha scoped to character draw")
    require(character_render, "visual->CurrentAction == rise::slayer::kSwordInertiaAction",
            "S21 Sword action 0xE0 draw-opacity branch")
    require(character_render, "frame <= 5.f ?",
            "Sword alpha transitions at native frame five")
    require(character_render, "1.f - frame / 10.f : 0.f;",
            "Sword action uses native fade then zero alpha")
    require(character_render, "object->Alpha = 0.3f;",
            "S21 Pierce action 0xE4 translucency")
    require(character_render, "object->Alpha = previous;",
            "ordinary alpha restored after Pierce draw")
    require(resources, "RandomBatSpread(spread, false);",
            "0x683 signed X/Z localized joint spread")
    require(resources, "RandomBatSpread(spread, true);",
            "0x684 upward X/Z localized child spread")
    require(resources, "SpawnChild(kBatFlockOrbitController, pulse, 0, 0, 0.f);",
            "0x684 null-owner zero-scale 0x685 child")
    require(resources, "VectorRotate(spread, matrix, rotated);",
            "0x682 native angle-rotated random spread")
    require(resources, "SpawnChild(kBatFlockModel, pulse, &effect, 2, 2.5f);",
            "0x682 immediate-parent owner for subtype-2 bat")
    require(resources, "Vector(0.5f, 0.5f, 0.5f, pinLight);",
            "0x682 native 0x810B light")
    require(resources, "Vector(0.65f, 0.65f, 0.65f, impactLight);",
            "0x682 native 0x8070 light")
    require(resources, "0.26f * particleScale, &effect);",
            "0x682 native 0x8070 scale")
    require(resources, "40.f * effect.Scale", "0x80E3 native bat-scale multiplier")
    require(joints, "o->SubType == 6 && o->Target &&",
            "0x80E3 subtype 6 private joint constructor/update guard")
    require(joints, "o->MaxTails = 50;", "0x80E3 native joint tail capacity")
    require(joints, "o->LifeTime = 5;", "0x80E3 native five-tick life")
    require(joints, "VectorCopy(o->Target->Position, o->Position);",
            "0x80E3 native bat-owner position update")
    require(resources, "Calc_RenderObject(&effect, false, 0, 0)",
            "native generic model Calc wrapper")
    require(resources, "if (effect.Type == kBatFlockModel)",
            "S21 0x678 Bat authored one-mesh subtype-specific renderer")
    require(resources, "model.RenderBody(RENDER_TEXTURE, effect.Alpha,",
            "S21 0x678 subtype 0..4 first ordinary mesh pass")
    require(resources, "effect.SubType == 4 ?\n            kBetGrilsShot2GoldBitmap : kBetGrilsShot2RedBitmap",
            "S21 0x678 subtype 4 gold, subtype 2/3 red override selection")
    require(resources, "effect.SubType == 4 ? 1.f : 0.6f",
            "S21 0x678 gold/red overlay blend-light contrast")
    require(resources, "model.RenderBody(RENDER_TEXTURE | RENDER_BRIGHT, effect.Alpha,",
            "S21 0x678 subtype 2/3/4 second luminous mesh pass")
    require(resources, "model.TransformPosition(BoneTransform[7], relative, anchor,",
            "S21 0x678 subtype 1..4 sprite follows bat bone 7")
    require(resources, "kFlareBlueBitmap : effect.SubType == 4 ?\n                kFlareBitmap : kFlareRedBitmap",
            "S21 0x678 0x7FDD/0x7FE0/0x7F78 sprite bitmap selector")
    require(resources, "effect.SubType == 3 ? 0.3f : 0.6f",
            "S21 0x678 subtype-specific bone sprite scale")
    require(resources, "0.2f + (rand() % 20) / 25.f",
            "S21 0x678 subtype-3 sprite-light random envelope")
    require(resources, "CreateSprite(spriteTexture, anchor, spriteScale, spriteLight,",
            "S21 0x678 bone sprite uses 5.2 sprite pool")
    require(resources, "effect.Type == kBatFlockTrailModel ||",
            "S21 0x688 registered dark draw light attenuation")
    require(resources, "effect.Type == kDetectionMarkModel ||",
            "S21 0x691 registered dark draw light attenuation")
    require(resources, "effect.Type == kDetectionImpactModel;",
            "S21 0x694 registered dark draw light attenuation")
    require(resources, "VectorScale(effect.Light, effect.Alpha, model.BodyLight);",
            "native 0x688/0x691/0x694 multiply model RGB light by object alpha")
    require(resources, "effect.Type == kDetectionMarkModel) && effect.SubType != 0)\n        return true;",
            "native 0x688/0x691 nonzero subtype is handled without dark mesh draw")
    require(resources, "const int quarterTicks = static_cast<int>(initialLife) / 4;",
            "S21 0x691 mark fade uses integer quarter-window ticks")
    if resources.count("const int thirdTicks = static_cast<int>(initialLife) / 3;") != 2:
        raise AssertionError("S21 0x678 bat subtype 0/2 needs two integer-third fade windows")
    if resources.count("const int halfTicks = static_cast<int>(initialLife) / 2;") != 1:
        raise AssertionError("S21 0x694 impact fade needs one integer half-window")
    mark_update = resources.split("else if (effect.Type == kDetectionMarkModel)", 1)[1]
    mark_update = mark_update.split("effect.AnimationFrame +=", 1)[0]
    if "effect.SubType == 0" not in mark_update or \
       mark_update.count("else") != 1 or \
       "else if (effect.LifeTime < quarter)" not in mark_update:
        raise AssertionError("S21 0x691 nonzero subtype must exit without fade")
    require(resources, "nativeDark ? (RENDER_TEXTURE | RENDER_DARK) :",
            "native 0x82 is textured subtractive dark, not textured bright")
    if "effect.Type == kPierceMarksCylinderModel ? RENDER_TEXTURE :" in resources:
        raise AssertionError("old opaque 0x5D8 cylinder fallback remains active")
    require(resources, "(RENDER_TEXTURE | RENDER_BRIGHT);",
            "Slayer shaped bat model retains isolated additive body pass")
    require(resources, "model.RenderBody(renderFlags, effect.Alpha,",
            "Slayer S21 models retain single complete RenderBody pass")
    require(resources, "bool EnsurePierceCylinderMaterial(int modelId, BMD& model, int mesh)",
            "0x5D8 private material readiness guard")
    require(resources, "if (_stricmp(material, \"lines2.JPG\") != 0)",
            "S21 Pierce 0x5D8 authored lines2 material guard")
    require(resources, "(modelId == kPierceMarksCylinderModel && model.NumMeshs != 1)",
            "S21 Pierce cylinder resident mesh shape checked before draw")
    require(resources, "(modelId == kBatFlockTrailModel && model.NumMeshs != 3)",
            "S21 Bat trail resident three-mesh shape checked before draw")
    require(resources, "if (modelId != kPierceMarksCylinderModel)\n        return true;",
            "registered dark RGB models bypass speculative alpha key")
    require(resources, "return bitmap && bitmap->Components == 3;",
            "S21 Pierce 0x5D8 keeps GL_RGB material in resident and fresh paths")
    require(resources, "if (!EnsurePierceCylinderMaterial(modelId, model, mesh))",
            "resident and newly-loaded model paths both check private material")
    if resources.count("!EnsurePierceCylinderMaterial(modelId, model, mesh)") != 2:
        raise AssertionError("Slayer material guard missing from resident or fresh model path")
    bitmap_header = read("ExMain_RISE_PC/Main5.2_RISE/GlobalBitmap.h")
    bitmap_source = read("ExMain_RISE_PC/Main5.2_RISE/GlobalBitmap.cpp")
    if "ApplySlayerBlackKeyAlpha" in resources + bitmap_header + bitmap_source:
        raise AssertionError("5.2 RGBA black-key experiment remains in active Slayer source")
    if "model.RenderMesh(mesh, RENDER_TEXTURE, effect.Alpha," in resources:
        raise AssertionError("Slayer model render bypasses BeginRender/EndRender")
    object_renderer = read("ExMain_RISE_PC/Main5.2_RISE/ZzzObject.cpp")
    require(object_renderer, "b->RenderBody(RENDER_TEXTURE, o->Alpha,",
            "5.2 generic Draw_RenderObject textured body")
    bmd_renderer = read("ExMain_RISE_PC/Main5.2_RISE/ZzzBMD.cpp")
    require(bmd_renderer, "void BMD::RenderBody(int Flag, float Alpha,",
            "5.2 RenderBody transform wrapper")
    if "model.BodyLight[0] = effect.Light[0] * effect.Alpha;" in resources:
        raise AssertionError("model body light incorrectly multiplied by alpha twice")
    print("PASS: Bat Flock 0x82EF/0x82F3 joint-pool routes initialized")

    update_start = resources.index("void UpdateEffect")
    sword_start = resources.index("case kSwordSecondaryController:", update_start)
    sword_end = resources.index("case kSword68BController:", sword_start)
    sword_base = resources[sword_start:sword_end]
    require(sword_base, "CreateAngle(effect.StartPosition[0]", "0x689 target yaw")
    require(sword_base, "SpawnChild(kSword68BController, launch, &effect, 0",
            "0x689 single base 0x68B subtype")
    require(sword_base, "SpawnChild(kSword68EController, launch, &effect, 0",
            "0x689 base 0x68E subtype")
    if "SpawnChild(kBatFlock686Controller" in sword_base:
        raise AssertionError("0x817-only 0x686 leaked into base Sword Inertia")
    require(resources, "swordLine.Angle[1] += 180.f", "0x68B angle lane 180")
    require(resources, "swordLine.Angle[1] += 90.f", "0x68B angle lane 90")
    require(resources, "CreateParticle(kWaterFall4Bitmap", "0x678 native 0x8020 trail")
    require(resources, "Vector(0.5f, 0.05f, 0.f", "0x678 native 0x8020 light")
    require(resources, "VectorDistance3(effect.Position, target->Position) <= 50.f",
            "0x678 subtype 2 real target expiry radius")
    require(resources, "terrainOrigin[2] = RequestTerrainHeight",
            "0x678 subtype 2 terrain-projected flight origin")
    require(resources, "VectorSubtract(target->Position, terrainOrigin",
            "0x678 subtype 2 native target direction")
    require(resources, "NativeRandomUnitStep(80, 100) * 0.5f",
            "0x678 subtype 2 unit-step speed before half-scale")
    if "destination[2] += 35.f;" in resources:
        raise AssertionError("dump-unproven Bat Flock target +35 Z leaked into port")
    require(converter, "4650F6571447C1A4D4CCA81E9387A70B9B489AA4989820AF7D375855895B44D2",
            "0x8020 WATERFALL4 asset hash")
    require(resources, "swordLine.TransformPosition(BoneTransform[3]",
            "0x681 first ribbon bone")
    require(resources, "swordLine.TransformPosition(BoneTransform[1]",
            "0x681 second ribbon bone")
    require(resources, "CreateObjectBlurBitmap(&effect", "0x681 explicit S21 blur")
    require(resources, "frame > 5.5f", "0x681 native frame ceiling")
    require(resources, "float frame = (initialLife - effect.LifeTime) * actionSpeed",
            "0x681 native elapsed-life frame origin")
    require(resources, "frame <= 5.f ? 1.f :",
            "0x681 native full-alpha frame gate")
    require(resources, "1.f - frame / static_cast<float>(samples)",
            "0x681 native seven-key alpha tail")
    require(resources, "effect.SubType == 1 ? 140.f : 60.f",
            "0x681 native flank Angle Y offsets")
    require(converter, "055F4D6F94807F3F507DC84A9CBED509F05E4F3C65E1CF0D49E0BF040AB18FB9",
            "0x82EC blur02_mono_long_van asset hash")
    require(resources, "CreateParticle(kGroundStarBitmap, position, angle, light",
            "0x68C native randomized 0x8149 emitter")
    if "case kSword68CController:\n            AttachToTarget" in resources:
        raise AssertionError("0x68C incorrectly attached to selected target")
    print("PASS: placeholder carriers removed and decoded 0x679..0x690 graph nodes present")
    if re.search(r"#define\s+MAX_CLASS\s+7\b", client_defines) and \
       re.search(r"#define\s+MODEL_BODY_NUM\s+24\b", client_defines) and \
       "const int slayerBaseBody = MODEL_BODY_NUM - 3;" in player_model_loader and \
       "return static_cast<BYTE>(MODEL_BODY_NUM - 3 + stage);" in character_manager:
        for part in ("HELM", "ARMOR", "PANTS", "GLOVES", "BOOTS"):
            require(player_model_loader, f"MODEL_BODY_{part} + skin",
                    f"private Class09 {part.lower()} body loader")
            require(player_model_loader,
                    f"gLoadData.OpenTexture(MODEL_BODY_{part} + skin, \"Player\\\\\");",
                    f"private Class09 {part.lower()} body texture loader")
        require(player_model_loader,
                "const int skin = MODEL_BODY_NUM - 3 + stage;",
                "Class09/209/309 texture loop uses the same vacant body slots")
        print("PASS: isolated Class09/209/309 body models and textures use vacant 5.2 slots 21..23 without changing MAX_CLASS=7")
        print("OPEN: Class09 avatar visibility and five-skill visuals still need genuine Slayer ingame acceptance")
    elif re.search(r"#define\s+MAX_CLASS\s+7\b", client_defines):
        print("OPEN: class09 body models are staged but the 5.2 MAX_CLASS=7 loader does not load them")
    elif not client_defines:
        print("OPEN: partial snapshot lacks _define.h; native class09 body-loader capacity cannot be certified by this verifier")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
