# Pegasus high-skills — new Codex task prompt

Continue the Pegasus high-skill import in the already isolated worktree:

- Worktree: `D:\RISE-CrossPlatform\Source_PC_Pegasus`
- Branch: `feature/pegasus-high-skills`
- Read first: `PEGASUS_IMPORT_STATE.md`
- Evidence: `D:\RISE-CrossPlatform\Source\docs\pegasus_high_skills\IMPORT_AUDIT.md`
  and `SWORD_BLOW_CHAIN.md`

Strict boundary: do not modify `D:\RISE-CrossPlatform\Source` (RISE Editor),
`Source_PC_SkillPreview`, ExMain_RISE Android, original Pegasus files, or any
production client. Build and stage only inside the isolated Pegasus test path.

Goal: port all twelve Pegasus group-15 items 102..113 and their high skills.
Start with Sword Blow, item 15:110 -> client skill 1501. Reuse the recovered
Pegasus chain and native RISE 5.2 runtime APIs; do not guess visual layers and
do not copy SS21 addresses or object layouts blindly.

Current build evidence: the latest isolated client build completed with exit 0
after adding `PegasusHighSkillCatalog` and separating the client skill-ID bound
as `MAX_SKILL_TYPES`. Existing Crypto++ LNK4099 missing-PDB warnings are the
only reported linker warning class. This is not runtime acceptance.

Next exact action: add a focused catalog/provenance test; split the GameServer
skill-ID capacity from its skill-list slot count; add item mappings for
15:102..113 and corresponding skill data; then complete Sword Blow cast,
large-sword, child swords, target layers, bone lights, learning/cast/damage,
isolated client+server builds, and isolated runtime QA. Keep every skill
IN_PROCESS until dispatch, action/target, root, children/assets, renderer,
movement/collision, reaction, lifetime, ownership/pool, and runtime validation
are evidenced. Never claim visual parity from compilation alone.
