$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$trace = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusRuntimeTrace.h')
foreach ($token in @(
    'FILE_APPEND_DATA',
    'FILE_SHARE_READ | FILE_SHARE_WRITE',
    'OPEN_ALWAYS',
    'WriteFile(file, line',
    'CloseHandle(file)',
    'TraceRuntimeModelRenderStateOnce',
    'TraceRuntimeModelRangeRenderStateOnce',
    'object.Alpha < 0.01f',
    '"TRANSPARENT_SKIP"',
    'TraceRuntimeModelRangeAnimationOnce',
    'evidence.Claim(modelBegin, modelId, event)',
    'evidence.Claim(modelBegin, modelId, 1u)',
    'animationFrame < 0.25f'
)) {
    if (-not $trace.Contains($token)) { throw "Missing trace contract: $token" }
}
$analyzer = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'AnalyzePegasusRuntimeQa.ps1')
if ($analyzer.Contains('$rows = [System.IO.File]::ReadLines(') -or
    -not $analyzer.Contains('[System.IO.FileShare]::ReadWrite')) {
    throw 'Analyzer must allow live FILE_APPEND_DATA writers while reading'
}
foreach ($token in @('RENDER_TRANSPARENT_SKIP', 'RENDER_HARD_FAIL',
    'hard renderer failures', 'SWORD_BLOW_RENDER',
    'missing visible renderer PASS offsets', 'expected 3 flare + 2 shockwave',
    'expected 5 released child carriers', 'animated buff BMD did not reach frame 0.25',
    'FRAMEBUFFER_CAPTURES=$captureCount/45', 'missing framebuffer capture phase',
    'expected exactly QA_CAPTURE phases 0,1,2',
    '[System.Drawing.Image]::FromFile', 'corrupt framebuffer capture phase',
    'MULTIKEY_BMD_PROGRESS=$bmdProgressCount/$($animatedContracts.Count)',
    'authored multi-key BMD did not progress', 'SWORD_BLOW_BMD_FRAME',
    'ATTACK_DAMAGE_PACKETS=$damagePassCount/$($damageContracts.Count)',
    'TARGET_REACTIONS=$reactionPassCount/$($damageContracts.Count)',
    'no positive authoritative damage packet reached the cast target',
    'native SetPlayerShock reaction was not applied to the cast target',
    'SKILL_LIST_PRESENT=$skillListPassCount/$($skills.Count)',
    'timed server provisioning did not reach the client skill list',
    'received skill was not PRESENT in the client skill array',
    'one or more received QA skills failed client skill-array insertion',
    'BUFF_STATE_ACTIVE=$buffActiveCount/$($buffContracts.Count)',
    'native buff effect $($contract.Effect) never became ACTIVE',
    'native buff effect $($contract.Effect) was not CLEARED by QA isolation',
    'one or more Pegasus native buff-map transitions failed',
    'SWORD_BLOW_FIXED_POOL=$($swordBlowCreates.Count)/$($swordBlowDestroys.Count)',
    'fixed model offset $offset allocated $created but released $destroyed',
    'DYNAMIC_LIFECYCLE=$($dynamicCreates.Count)/$($dynamicDestroys.Count)',
    'expected one AUTO_QA_SEQUENCE COMPLETE row',
    'AUTO_QA_POOL_CHILDREN',
    'PEGASUS_JOINT_CREATE', 'PEGASUS_PARTICLE_CREATE',
    'no source-identified Pegasus joint was allocated',
    'no source-identified Pegasus particle was allocated',
    'Pegasus joint or particle pool exhausted during QA',
    'PEGASUS_CHILD_CREATE=',
    'Pegasus joint/particle cleanup snapshot is missing',
    'ACTIVE_JOINTS_PARTICLES_AFTER_QA=',
    '$requiredDynamicOffsets = @(0..47) + 52',
    'dynamic model offset $offset allocated $created but completed/released $destroyed',
    "Skill=288;  Name='Deathside'",
    "Name='Deathside Reaper'",
    "Name='Deathside appear'; Offset=3",
    "Name='Deathside disappear'; Offset=4",
    'Deathside: no randomized scythe carrier was allocated',
    'Deathside scythe offset $offset allocated $created but completed/released $destroyed',
    'DEATHSIDE_ATTACK_SOUND=',
    'CUSTOM_SOUND_PLAYBACK=$soundPassCount/$($soundContracts.Count)',
    'DirectSound playback PASS missing',
    'neither source hit buffer reached DirectSound PASS',
    'MOVEMENT_COLLISION=$movementPassCount/$($movementContracts.Count)',
    'no projectile reached the terrain impact branch',
    'unknown launch phase or incorrect phase termination',
    'unclassified projectile termination',
    'SMALL_SWORD_ADVANCE', 'BIG_SWORD_ADVANCE', 'FORCED_IMPACT',
    'FORWARD_OFFSET')) {
    if (-not $analyzer.Contains($token)) { throw "Missing render-analysis contract: $token" }
}
$scene = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzScene.cpp')
$client = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\WSclient.cpp')
foreach ($token in @('"CAST_ROOT_BOUNDARY"',
    'g_PegasusCastRootStamps[rootSlot].Complete(decoded.caster)',
    'rise::pegasus::ReadWordLE(payload + 4) == decoded.targetCount',
    'rise::pegasus::SwordBlowPayloadSize(decoded.targetCount)')) {
    if (-not $client.Contains($token)) { throw "Missing exact end-of-cast marker guard: $token" }
}
foreach ($token in @('"NATIVE_CALL_WITHOUT_SHOCK_ACTION"',
    '"TARGET_REACTION_GATE"', '"NATIVE_RANDOM_GATE"',
    '"EXCLUDED_MONSTER_275"', 'else if (rand_fps_check(2))')) {
    if (-not $client.Contains($token)) { throw "Missing native reaction diagnostic: $token" }
}
$serverFaceStart = $client.IndexOf('static bool ShouldDispatchPegasusCastRoot(WORD skill, int sourceKey)')
$serverFaceEnd = $client.IndexOf('#else', $serverFaceStart)
$serverFacing = $client.Substring($serverFaceStart, $serverFaceEnd - $serverFaceStart)
foreach ($token in @('IsPegasusServerQaActive() && Hero && Hero->Key == sourceKey',
    'Hero->TargetCharacter < MAX_CHARACTERS_CLIENT', '&target != Hero && target.Object.Live',
    '"SERVER_QA_FACING"', 'source.Angle[2] = CreateAngle(')) {
    if (-not $serverFacing.Contains($token)) { throw "Missing scoped server QA facing: $token" }
}
if ($serverFacing.IndexOf('"SERVER_QA_FACING"') -lt $serverFacing.IndexOf('"DUPLICATE_SKIPPED"') -or
    $serverFacing -match 'Position\[[012]\]\s*[+\-*/]?=') {
    throw 'Server QA must face only accepted roots and never move actors'
}
if ($scene.Contains('(now - g_PegasusQaCaptureLastStartTick) < 15000')) {
    throw 'QA capture suppression must span the entire skill step, not 15 seconds'
}
$faceStart = $client.IndexOf('static void FacePegasusLocalVisualTarget(')
$qaStart = $client.IndexOf('bool RunPegasusLocalVisualQaStep(', $faceStart)
if ($faceStart -lt 0 -or $qaStart -le $faceStart) {
    throw 'Local injected QA must restore input-side target facing'
}
$facing = $client.Substring($faceStart, $qaStart - $faceStart)
if (-not $facing.Contains('if (&target == Hero)') -or
    -not $facing.Contains('source.Angle[2] = CreateAngle(') -or
    -not $facing.Contains('"LOCAL_VISUAL_FACING"') -or
    $facing -match 'Position\[[012]\]\s*[+\-*/]?=') {
    throw 'QA facing must skip self-buffs, set heading, trace, and never move positions'
}
foreach ($call in @('FacePegasusLocalVisualTarget(*target, skill);',
    'FacePegasusLocalVisualTarget(*target, rise::pegasus::kSwordBlowFlySkill);')) {
    if (-not $client.Contains($call + "`n`t" + 'g_PegasusLocalVisualInjection = true;') -and
        -not $client.Contains($call + "`r`n`t" + 'g_PegasusLocalVisualInjection = true;')) {
        throw 'QA heading must be established before injected dispatch'
    }
}
$dispatchCapture = Get-Content -Raw -LiteralPath (Join-Path $PSScriptRoot 'CapturePegasusDispatchFrames.ps1')
foreach ($token in @('void SchedulePegasusQaCapture(int skill)',
    'TraceRuntime("AUTO_QA_RUN", 15',
    'static_cast<int>(GetCurrentProcessId()), "ARMED"',
    'seed->itemIndex >= 100 && seed->itemIndex <= 113',
    'skill != rise::pegasus::kRuntimeQaWindSoulSkill',
    'g_PegasusQaCaptureLastSkill == skill',
    'bool IsPegasusServerQaActive()',
    'g_PegasusAutoQaSequenceCompleteTick == 0',
    'PegasusQACaptures\\qa_%lu_skill_%d_phase_%d.jpg',
    'TraceRuntime("QA_CAPTURE"', 'g_PegasusQaCaptureNextTick = now + 250',
    'g_PegasusQaCaptureNextTick = GetTickCount() +',
    '(g_PegasusQaCaptureSkill == 288 ? 100 :',
    'g_PegasusQaCaptureSkill == 2013 ? 150 :',
    'g_PegasusQaCaptureSkill == 241 ? 200 : 400)',
    'g_PegasusQaFramebufferCapture = true',
    'const bool isPegasusQaCapture = g_PegasusQaFramebufferCapture',
    'if (GrabEnable && !isPegasusQaCapture)',
    'if (GrabEnable && !isPegasusQaCapture && !addTimeStampToCapture)',
    'g_PegasusQaFramebufferCapture = false')) {
    if (-not $scene.Contains($token)) { throw "Missing framebuffer-capture contract: $token" }
}
foreach ($token in @("Stage -eq 'AUTO_QA_LOGIN'", "Stage -eq 'AUTO_QA_RUN'",
    'No AUTO_QA_LOGIN/AUTO_QA_RUN ... ARMED row was found')) {
    if (-not $analyzer.Contains($token)) {
        throw "Missing manual-login/cast-only run boundary: $token"
    }
}
foreach ($token in @('$reactionSkillIds = @(2023,2012,2013,1501,241,242,243)',
    '$reactionCastCount = 10',
    '$expectedDispatches = Get-ExpectedCastCount $skill.Id',
    '$expectedSamples = Get-ExpectedCastCount $skill.Id',
    "Result='FORWARD_OFFSET';      Count=`$reactionCastCount",
    "Result='FORCED_IMPACT';       Count=(`$reactionCastCount * 5)",
    "Result='SMALL_SWORD_ADVANCE'; Count=1",
    "Result='BIG_SWORD_ADVANCE';   Count=1")) {
    if (-not $analyzer.Contains($token)) {
        throw "Missing native-reaction sampling contract: $token"
    }
}
if (-not $analyzer.Contains('all 15 skill steps and 79 expected dispatches')) {
    throw 'Runtime PASS summary still reports the obsolete single-cast total'
}
foreach ($token in @('[int]$TimeoutSeconds = 180',
    '$seenSkills = [System.Collections.Generic.HashSet[int]]::new()',
    'if (-not $seenSkills.Add($skill))')) {
    if (-not $dispatchCapture.Contains($token)) {
        throw "Missing repeated-dispatch capture guard: $token"
    }
}
foreach ($token in @('activeSwordBlow', 'AUTO_QA_POOL_SWORD_BLOW',
    'activePegasusJoints', 'activePegasusParticles',
    'AUTO_QA_POOL_JOINT', 'AUTO_QA_POOL_PARTICLE',
    'AUTO_QA_POOL_CHILDREN',
    'activeDynamic + activeElementalChildren +')) {
    if (-not $scene.Contains($token)) { throw "Missing fixed Sword Blow pool contract: $token" }
}
$effect = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzEffect.cpp')
foreach ($token in @('SWORD_BLOW_CREATE', 'SWORD_BLOW_DESTROY',
    'traceSwordBlowModel ? Type - MODEL_PEGASUS_SWORD_BLOW_BIG',
    'EFFECT_LIFECYCLE_CREATE', 'EFFECT_LIFECYCLE_DESTROY')) {
    if (-not $effect.Contains($token)) { throw "Missing fixed Sword Blow ownership trace: $token" }
}
$joint = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzEffectJoint.cpp')
$particle = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzEffectParticle.cpp')
foreach ($token in @('g_pegasusQaJointSlots', 'IsPegasusQaJointSlot',
    'PEGASUS_JOINT_CREATE', 'IsSwordWrathLineBitmap')) {
    if (-not $joint.Contains($token)) { throw "Missing joint ownership trace: $token" }
}
foreach ($token in @('g_pegasusQaParticleSlots', 'IsPegasusQaParticleSlot',
    'PEGASUS_PARTICLE_CREATE', 'PegasusOwned',
    'IsDeathsideParticleBitmap', 'IsSwordWrathSmokeBitmap',
    'IsSwordWrathPinStarBitmap')) {
    if (-not $particle.Contains($token)) { throw "Missing particle ownership trace: $token" }
}
$sound = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\DSplaysound.cpp')
foreach ($token in @('TraceRuntime("SOUND_PLAY"', '"DISABLED"',
    '"BUFFER_MISSING"', '"PLAY_FAILED"', '"PASS"')) {
    if (-not $sound.Contains($token)) { throw "Missing DirectSound runtime trace: $token" }
}
if (-not $client.Contains('SchedulePegasusQaCapture(MagicNumber);')) {
    throw 'Cast echo does not schedule isolated framebuffer capture'
}
foreach ($token in @('ArmPegasusQaDamageTrace(MagicNumber, TargetIndex)',
    'TracePegasusQaDamagePacket(Index, Damage)',
    'TracePegasusQaTargetReaction(Index)',
    'TraceRuntime("TARGET_REACTION"',
    'TraceRuntime("DAMAGE_PACKET"',
    'targetIndex != g_PegasusQaDamageTargetIndex',
    '(GetTickCount() - g_PegasusQaDamageArmTick) > 3000')) {
    if (-not $client.Contains($token)) {
        throw "Missing authoritative damage-correlation trace: $token"
    }
}
foreach ($token in @('TraceRuntime("SKILL_LIST_ADD"',
    'TraceRuntime("SKILL_LIST_STATE"', '"PRESENT" : "MISSING"')) {
    if (-not $client.Contains($token)) {
        throw "Missing post-insertion client skill-list trace: $token"
    }
}
foreach ($token in @('TraceRuntime("BUFF_STATE"', '"ACTIVE" : "REGISTER_FAILED"',
    '"UNREGISTER_FAILED" : "CLEARED"')) {
    if (-not $client.Contains($token)) {
        throw "Missing post-mutation native buff-state trace: $token"
    }
}
$animationRoutes = @(
    @{ File='PegasusChaosBladeRuntime.cpp'; Token='TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS, animationFrame);' },
    @{ File='PegasusSpiralChargeRuntime.cpp'; Token='TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,' },
    @{ File='PegasusShiningBirdRuntime.cpp'; Token='TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,' },
    @{ File='PegasusHavocSpearRuntime.cpp'; Token='TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,' },
    @{ File='PegasusElementalChargeRuntime.cpp'; Token='TraceRuntimeModelAnimationOnce(effect.Type, MAX_MODELS,' },
    @{ File='PegasusSwordBlowRuntime.cpp'; Token='TraceRuntimeModelRangeAnimationOnce("SWORD_BLOW_BMD_FRAME"' }
)
foreach ($route in $animationRoutes) {
    $source = Get-Content -Raw -LiteralPath (Join-Path $root "Main5.2_RISE\RISE\$($route.File)")
    if (-not $source.Contains($route.Token)) {
        throw "Missing runtime BMD progression trace: $($route.File)"
    }
}
$movementRoutes = @(
    @{ File='PegasusRainingArrowRuntime.cpp'; Token='"PROJECTILE_IMPACT"' },
    @{ File='PegasusShiningBirdRuntime.cpp'; Token='"FORWARD_OFFSET"' },
    @{ File='PegasusSpearstormRuntime.cpp'; Token='"FORCED_IMPACT"' },
    @{ File='PegasusSwordBlowRuntime.cpp'; Token='"SMALL_SWORD_ADVANCE"' },
    @{ File='PegasusSwordBlowRuntime.cpp'; Token='"BIG_SWORD_ADVANCE"' }
)
foreach ($route in $movementRoutes) {
    $source = Get-Content -Raw -LiteralPath (Join-Path $root "Main5.2_RISE\RISE\$($route.File)")
    if (-not $source.Contains($route.Token)) {
        throw "Missing runtime movement/collision trace: $($route.File) $($route.Token)"
    }
}
if ($trace.Contains('fopen_s(&file, "PegasusRuntimeQA.log"')) {
    throw 'Runtime trace still uses process-local CRT sharing semantics'
}
Write-Output 'Pegasus concurrent runtime trace contract PASS'
