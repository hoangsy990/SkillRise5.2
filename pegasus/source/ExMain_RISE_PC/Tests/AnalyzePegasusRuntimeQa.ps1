param(
    [string]$LogPath = (Join-Path $PSScriptRoot 'PegasusBuild\RuntimeClient\PegasusRuntimeQA.log'),
    # Optional lower bound for large append-only logs. All run-boundary and
    # acceptance checks below remain required; never synthesizes missing rows.
    [datetime]$NotBefore = [datetime]::MinValue,
    [datetime]$NotAfter = [datetime]::MaxValue
)

$ErrorActionPreference = 'Stop'

$skills = @(
    @{ Id=288; Name='Deathside' },
    @{ Id=289; Name='Darkness' },
    @{ Id=2023; Name='Raining Arrow' },
    @{ Id=2024; Name='Dex Booster' },
    @{ Id=2012; Name='Chaos Blade' },
    @{ Id=2013; Name='Havoc Spear' },
    @{ Id=2014; Name='Spiral Charge' },
    @{ Id=2015; Name='Crusher Charge' },
    @{ Id=2016; Name='Elemental Charge' },
    @{ Id=1500; Name='Sword Wrath' },
    @{ Id=1501; Name='Sword Blow' },
    @{ Id=241; Name='Shining Bird' },
    @{ Id=242; Name='Dragon Violent' },
    @{ Id=243; Name='Spearstorm' },
    @{ Id=737; Name='Wind Soul' }
)
$rootGatedSkills = @(288,2023,2012,2013,1501,241,242,243)
$reactionSkillIds = @(2023,2012,2013,1501,241,242,243)
$reactionCastCount = 10

function Get-ExpectedCastCount([int]$SkillId) {
    if ($SkillId -eq 288) { return 2 }
    if ($reactionSkillIds -contains $SkillId) { return $reactionCastCount }
    return 1
}

if (-not (Test-Path -LiteralPath $LogPath -PathType Leaf)) {
    throw "Runtime log does not exist: $LogPath"
}

$minimumPrefix = '[' + $NotBefore.ToString('yyyy-MM-dd HH:mm:ss.fff') + ']'
$maximumPrefix = '[' + $NotAfter.ToString('yyyy-MM-dd HH:mm:ss.fff') + ']'
function Read-SharedQaLog([string]$Path) {
    # File.ReadLines uses FileShare.Read and prevents the live client from
    # opening FILE_APPEND_DATA. Explicit sharing must preserve concurrent QA.
    $stream = [System.IO.File]::Open($Path, [System.IO.FileMode]::Open,
        [System.IO.FileAccess]::Read, [System.IO.FileShare]::ReadWrite)
    $reader = [System.IO.StreamReader]::new($stream)
    try {
        while (-not $reader.EndOfStream) { $reader.ReadLine() }
    } finally {
        $reader.Dispose()
    }
}
$rows = Read-SharedQaLog (Resolve-Path -LiteralPath $LogPath).Path | ForEach-Object {
    if ($_.Length -ge 25 -and
        ([string]::CompareOrdinal($_.Substring(0, 25), $minimumPrefix) -lt 0 -or
         [string]::CompareOrdinal($_.Substring(0, 25), $maximumPrefix) -gt 0)) {
        return
    }
    if ($_ -match '^\[(?<ts>[^\]]+)\] (?<stage>\S+) value1=(?<v1>-?\d+) value2=(?<v2>-?\d+) result=(?<result>.*)$') {
        [pscustomobject]@{
            Timestamp = [datetime]::ParseExact($Matches.ts, 'yyyy-MM-dd HH:mm:ss.fff', $null)
            Stage = $Matches.stage
            Value1 = [int]$Matches.v1
            Value2 = [int]$Matches.v2
            Result = $Matches.result
        }
    }
}

$start = $rows | Where-Object {
    $_.Result -eq 'ARMED' -and
    ($_.Stage -eq 'AUTO_QA_LOGIN' -or $_.Stage -eq 'AUTO_QA_RUN')
} | Sort-Object Timestamp | Select-Object -Last 1
if (-not $start) {
    throw 'No AUTO_QA_LOGIN/AUTO_QA_RUN ... ARMED row was found'
}
$poolSummary = $rows | Where-Object {
    $_.Timestamp -ge $start.Timestamp -and $_.Stage -eq 'AUTO_QA_POOL'
} | Select-Object -First 1
$runEnd = if ($poolSummary) { $poolSummary.Timestamp } else { [datetime]::MaxValue }
$run = @($rows | Where-Object {
    $_.Timestamp -ge $start.Timestamp -and $_.Timestamp -le $runEnd
})
$failures = @()

$sequenceComplete = @($run | Where-Object {
    $_.Stage -eq 'AUTO_QA_SEQUENCE' -and $_.Value1 -eq 15 -and
    $_.Result -eq 'COMPLETE'
})
if ($sequenceComplete.Count -ne 1) {
    $failures += "expected one AUTO_QA_SEQUENCE COMPLETE row, got $($sequenceComplete.Count)"
}

$jointCreates = @($run | Where-Object {
    $_.Stage -eq 'PEGASUS_JOINT_CREATE' -and $_.Result -eq 'ALLOCATED'
})
$particleCreates = @($run | Where-Object {
    $_.Stage -eq 'PEGASUS_PARTICLE_CREATE' -and $_.Result -eq 'ALLOCATED'
})
if ($jointCreates.Count -eq 0) {
    $failures += 'no source-identified Pegasus joint was allocated'
}
if ($particleCreates.Count -eq 0) {
    $failures += 'no source-identified Pegasus particle was allocated'
}
$childPoolFailures = @($run | Where-Object {
    $_.Stage -in @('PEGASUS_JOINT_CREATE','PEGASUS_PARTICLE_CREATE') -and
    $_.Result -eq 'POOL_FULL'
})
if ($childPoolFailures.Count -ne 0) {
    $failures += 'Pegasus joint or particle pool exhausted during QA'
}

if (-not ($run | Where-Object { $_.Stage -eq 'AUTO_QA_COMMAND' -and $_.Result -eq 'SENT' })) {
    $failures += 'AUTO_QA_COMMAND SENT is missing'
}

$skillListPassCount = 0
foreach ($skill in $skills) {
    $add = @($run | Where-Object {
        $_.Stage -eq 'SKILL_LIST_ADD' -and $_.Value1 -eq $skill.Id -and
        $_.Result -eq 'RECEIVED'
    })
    $present = @($run | Where-Object {
        $_.Stage -eq 'SKILL_LIST_STATE' -and $_.Value1 -eq $skill.Id -and
        $_.Result -eq 'PRESENT'
    })
    if ($add.Count -eq 0) {
        $failures += "$($skill.Name): timed server provisioning did not reach the client skill list"
    }
    elseif ($present.Count -eq 0) {
        $failures += "$($skill.Name): received skill was not PRESENT in the client skill array"
    }
    else { ++$skillListPassCount }
}
if ($run | Where-Object {
    $_.Stage -eq 'SKILL_LIST_STATE' -and $_.Result -eq 'MISSING'
}) {
    $failures += 'one or more received QA skills failed client skill-array insertion'
}

$summary = foreach ($skill in $skills) {
    $echoes = @($run | Where-Object { $_.Stage -eq 'CAST_ECHO' -and $_.Value1 -eq $skill.Id })
    $dispatches = @($run | Where-Object { $_.Stage -eq 'CAST_DISPATCH' -and $_.Value1 -eq $skill.Id })
    if ($echoes.Count -eq 0) { $failures += "$($skill.Name): CAST_ECHO missing" }
    $expectedDispatches = Get-ExpectedCastCount $skill.Id
    if ($dispatches.Count -ne $expectedDispatches) {
        $failures += "$($skill.Name): expected $expectedDispatches CAST_DISPATCH, got $($dispatches.Count)"
    }
    if ($rootGatedSkills -contains $skill.Id -and $echoes.Count -gt 1) {
        $skipped = @($run | Where-Object {
            $_.Stage -eq 'CAST_ROOT_GATE' -and $_.Value1 -eq $skill.Id -and
            $_.Result -eq 'DUPLICATE_SKIPPED'
        }).Count
        $expectedSkips = $echoes.Count - $expectedDispatches
        if ($skipped -ne $expectedSkips) {
            $failures += "$($skill.Name): expected $expectedSkips duplicate skips, got $skipped"
        }
    }
    [pscustomobject]@{
        Skill = $skill.Id
        Name = $skill.Name
        Echo = $echoes.Count
        Hit = @($echoes | Where-Object Result -eq 'HIT').Count
        Miss = @($echoes | Where-Object Result -eq 'MISS').Count
        Dispatch = $dispatches.Count
    }
}

# The first fourteen rows are Pegasus skills.  Wind Soul has its own SS21
# action contract and is intentionally excluded from this Pegasus BMD gate.
foreach ($skill in $skills | Select-Object -First 14) {
    $expectedSamples = Get-ExpectedCastCount $skill.Id
    $selected = @($run | Where-Object {
        $_.Stage -eq 'CAST_ACTION_SELECT' -and $_.Value1 -eq $skill.Id
    })
    $frames = @($run | Where-Object {
        $_.Stage -eq 'CAST_ACTION_FRAME' -and $_.Value1 -eq $skill.Id
    })
    if ($selected.Count -ne $expectedSamples) {
        $failures += "$($skill.Name): expected $expectedSamples action selections, got $($selected.Count)"
    }
    if ($frames.Count -ne $expectedSamples) {
        $failures += "$($skill.Name): expected $expectedSamples frame samples, got $($frames.Count)"
    }
    foreach ($sample in $selected) {
        if ($sample.Result -notin @('EXACT', 'SELECTED_BRANCH')) {
            $failures += "$($skill.Name): wrong caster action $($sample.Value2) ($($sample.Result))"
        }
    }
    foreach ($sample in $frames) {
        if ($sample.Result -ne 'PROGRESSED') {
            $failures += "$($skill.Name): caster animation did not progress ($($sample.Result))"
        }
    }
}

# Source-proven visible model contracts. Orchestrator-only and meshless
# carriers are intentionally excluded. Deathside's Reaper is visible, while
# its scythe BMDs only drive blur trails. Sword Blow uses its pre-MAX_MODELS
# model range and therefore has a dedicated stage.
$visualContracts = @(
    @{ Skill=288;  Name='Deathside';        Stage='EFFECT_RENDER';     Offsets=@(47) },
    @{ Skill=2023; Name='Raining Arrow';    Stage='EFFECT_RENDER';     Offsets=@(37,38,39,40,41,42,43,44,45,46) },
    @{ Skill=2024; Name='Dex Booster';      Stage='EFFECT_RENDER';     Offsets=@(12,14,15) },
    @{ Skill=2012; Name='Chaos Blade';      Stage='EFFECT_RENDER';     Offsets=@(16,17,18) },
    @{ Skill=2013; Name='Havoc Spear';      Stage='EFFECT_RENDER';     Offsets=@(26,27,28,29,30,31) },
    @{ Skill=2014; Name='Spiral Charge';    Stage='EFFECT_RENDER';     Offsets=@(2,3,4,5,6,7) },
    # +8 mirrors Pegasus 0x27AE: zero-scale/zero-alpha carrier with no updater.
    @{ Skill=2015; Name='Crusher Charge';   Stage='EFFECT_RENDER';     Offsets=@(9,10,52) },
    @{ Skill=2016; Name='Elemental Charge'; Stage='EFFECT_RENDER';     Offsets=@(25) },
    @{ Skill=1500; Name='Sword Wrath';      Stage='EFFECT_RENDER';     Offsets=@(0,1) },
    @{ Skill=1501; Name='Sword Blow';       Stage='SWORD_BLOW_RENDER'; Offsets=@(0,1,2,3,4,5,6,7,8) },
    @{ Skill=241;  Name='Shining Bird';     Stage='EFFECT_RENDER';     Offsets=@(11) },
    @{ Skill=242;  Name='Dragon Violent';   Stage='EFFECT_RENDER';     Offsets=@(20,21,22,23) },
    @{ Skill=243;  Name='Spearstorm';       Stage='EFFECT_RENDER';     Offsets=@(33,34) }
)
$visualSummary = foreach ($contract in $visualContracts) {
    $missing = @()
    foreach ($offset in $contract.Offsets) {
        if (-not ($run | Where-Object {
            $_.Stage -eq $contract.Stage -and $_.Value2 -eq $offset -and
            $_.Result -eq 'PASS'
        })) {
            $missing += $offset
        }
    }
    if ($missing.Count -ne 0) {
        $failures += "$($contract.Name): missing visible renderer PASS offsets $($missing -join ',')"
    }
    [pscustomobject]@{
        Skill = $contract.Skill
        Name = $contract.Name
        Required = $contract.Offsets.Count
        Passed = $contract.Offsets.Count - $missing.Count
        Missing = $missing -join ','
    }
}

# Only BMDs with more than one authored key are required to advance. Single-key
# meshes are intentionally excluded even when Pegasus calls PlayAnimation.
$animatedContracts = @(
    @{ Name='Deathside Reaper'; RenderStage='EFFECT_RENDER'; Offset=47; BmdStage='EFFECT_BMD_FRAME' },
    @{ Name='Chaos Blade';      RenderStage='EFFECT_RENDER';     Offset=16; BmdStage='EFFECT_BMD_FRAME' },
    @{ Name='Havoc Spear line'; RenderStage='EFFECT_RENDER';     Offset=27; BmdStage='EFFECT_BMD_FRAME' },
    @{ Name='Spiral Charge';    RenderStage='EFFECT_RENDER';     Offset=2;  BmdStage='EFFECT_BMD_FRAME' },
    @{ Name='Elemental Charge'; RenderStage='EFFECT_RENDER';     Offset=25; BmdStage='EFFECT_BMD_FRAME' },
    @{ Name='Shining Bird';     RenderStage='EFFECT_RENDER';     Offset=11; BmdStage='EFFECT_BMD_FRAME' },
    @{ Name='Sword Blow multishot01'; RenderStage='SWORD_BLOW_RENDER'; Offset=4; BmdStage='SWORD_BLOW_BMD_FRAME' },
    @{ Name='Sword Blow multishot02'; RenderStage='SWORD_BLOW_RENDER'; Offset=5; BmdStage='SWORD_BLOW_BMD_FRAME' },
    @{ Name='Sword Blow multishot03'; RenderStage='SWORD_BLOW_RENDER'; Offset=6; BmdStage='SWORD_BLOW_BMD_FRAME' }
)
$bmdProgressCount = 0
foreach ($contract in $animatedContracts) {
    $modelIds = @($run | Where-Object {
        $_.Stage -eq $contract.RenderStage -and $_.Value2 -eq $contract.Offset -and
        $_.Result -eq 'PASS'
    } | ForEach-Object { $_.Value1 } | Sort-Object -Unique)
    $progress = @($run | Where-Object {
        $_.Stage -eq $contract.BmdStage -and $modelIds -contains $_.Value1 -and
        $_.Value2 -ge 250 -and $_.Result -eq 'PROGRESSED'
    })
    if ($progress.Count -eq 0) {
        $failures += "$($contract.Name): authored multi-key BMD did not progress"
    }
    else { ++$bmdProgressCount }
}

$damageContracts = @(
    @{ Skill=2023; Name='Raining Arrow' },
    @{ Skill=2012; Name='Chaos Blade' },
    @{ Skill=2013; Name='Havoc Spear' },
    @{ Skill=1501; Name='Sword Blow' },
    @{ Skill=241;  Name='Shining Bird' },
    @{ Skill=242;  Name='Dragon Violent' },
    @{ Skill=243;  Name='Spearstorm' }
)
$damagePassCount = 0
$reactionPassCount = 0
$reactionGateSummary = @()
foreach ($contract in $damageContracts) {
    $damageRows = @($run | Where-Object {
        $_.Stage -eq 'DAMAGE_PACKET' -and $_.Value1 -eq $contract.Skill -and
        $_.Value2 -gt 0 -and $_.Result -eq 'POSITIVE'
    })
    if ($damageRows.Count -eq 0) {
        $failures += "$($contract.Name): no positive authoritative damage packet reached the cast target"
    }
    else { ++$damagePassCount }
    $reactionRows = @($run | Where-Object {
        $_.Stage -eq 'TARGET_REACTION' -and
        $_.Value1 -eq $contract.Skill -and $_.Result -eq 'APPLIED'
    })
    # Explain native suppression without converting it into acceptance. These
    # are observed packet gates, not an inferred per-cast probability or count.
    $gateReasons = @($run | Where-Object {
        $_.Stage -eq 'TARGET_REACTION_GATE' -and $_.Value1 -eq $contract.Skill
    } | Group-Object -Property Result | Sort-Object -Property Name)
    $gateDetail = ($gateReasons | ForEach-Object { "$($_.Name):$($_.Count)" }) -join ','
    if (-not $gateDetail) { $gateDetail = 'NONE_OBSERVED' }
    $reactionGateSummary += "REACTION_GATE skill=$($contract.Skill) applied=$($reactionRows.Count) reasons=$gateDetail"
    if ($reactionRows.Count -eq 0) {
        $failures += "$($contract.Name): native SetPlayerShock reaction was not applied to the cast target"
    }
    else { ++$reactionPassCount }
}

# The five self-buffs in item range 15:102..113 must reach the character's
# native buff map and must then be removed by the per-sample QA isolation.
# Checking BUFF_STATE (after mutation), rather than only the packet-facing
# BUFF_REGISTER/BUFF_UNREGISTER calls, prevents a false PASS when an unknown
# native effect ID is received but rejected by the character buff container.
$buffContracts = @(
    @{ Skill=2024; Effect=172; Name='Dex Booster' },
    @{ Skill=2014; Effect=169; Name='Spiral Charge' },
    @{ Skill=2015; Effect=170; Name='Crusher Charge' },
    @{ Skill=2016; Effect=171; Name='Elemental Charge' },
    @{ Skill=1500; Effect=168; Name='Sword Wrath' }
)
$buffActiveCount = 0
$buffClearedCount = 0
foreach ($contract in $buffContracts) {
    $active = @($run | Where-Object {
        $_.Stage -eq 'BUFF_STATE' -and $_.Value1 -eq $contract.Effect -and
        $_.Result -eq 'ACTIVE'
    })
    $cleared = @($run | Where-Object {
        $_.Stage -eq 'BUFF_STATE' -and $_.Value1 -eq $contract.Effect -and
        $_.Result -eq 'CLEARED'
    })
    if ($active.Count -eq 0) {
        $failures += "$($contract.Name): native buff effect $($contract.Effect) never became ACTIVE"
    }
    else { ++$buffActiveCount }
    if ($cleared.Count -eq 0) {
        $failures += "$($contract.Name): native buff effect $($contract.Effect) was not CLEARED by QA isolation"
    }
    else { ++$buffClearedCount }
}
if ($run | Where-Object {
    $_.Stage -eq 'BUFF_STATE' -and
    $_.Result -in @('REGISTER_FAILED', 'UNREGISTER_FAILED')
}) {
    $failures += 'one or more Pegasus native buff-map transitions failed'
}

$soundContracts = @(
    @{ Name='Deathside appear'; Offset=3 },
    @{ Name='Deathside disappear'; Offset=4 },
    @{ Name='Raining Arrow cast'; Offset=5 },
    @{ Name='Raining Arrow hit'; Offset=6 },
    @{ Name='Havoc Spear cast'; Offset=7 },
    @{ Name='Havoc Spear explosion'; Offset=10 },
    @{ Name='Spiral Charge cast'; Offset=11 },
    @{ Name='Crusher Charge cast'; Offset=12 },
    @{ Name='Elemental Charge cast'; Offset=13 },
    @{ Name='Chaos Blade cast'; Offset=14 },
    @{ Name='Chaos Blade hit'; Offset=15 },
    @{ Name='Dragon Violent cast'; Offset=18 },
    @{ Name='Shining Bird cast'; Offset=20 },
    @{ Name='Spearstorm cast'; Offset=21 },
    @{ Name='Spearstorm hit'; Offset=22 }
)
$soundPassCount = 0
foreach ($contract in $soundContracts) {
    if (-not ($run | Where-Object {
        $_.Stage -eq 'SOUND_PLAY' -and $_.Value2 -eq $contract.Offset -and
        $_.Result -eq 'PASS'
    })) {
        $failures += "$($contract.Name): DirectSound playback PASS missing"
    }
    else { ++$soundPassCount }
}
$havocHitSound = @($run | Where-Object {
    $_.Stage -eq 'SOUND_PLAY' -and ($_.Value2 -eq 8 -or $_.Value2 -eq 9) -and
    $_.Result -eq 'PASS'
})
if ($havocHitSound.Count -eq 0) {
    $failures += 'Havoc Spear hit: neither source hit buffer reached DirectSound PASS'
}
$deathsideAttackSound = @($run | Where-Object {
    $_.Stage -eq 'SOUND_PLAY' -and $_.Value2 -in @(0,1,2) -and
    $_.Result -eq 'PASS'
})
if ($deathsideAttackSound.Count -eq 0) {
    $failures += 'Deathside attack: none of the three source attack buffers reached DirectSound PASS'
}

$terrainImpacts = @($run | Where-Object {
    $_.Stage -eq 'PROJECTILE_IMPACT' -and $_.Value1 -eq 2023 -and
    $_.Result -eq 'TERRAIN'
})
if ($terrainImpacts.Count -eq 0) {
    $failures += 'Raining Arrow: no projectile reached the terrain impact branch'
}
# Pinned Pegasus launches upward at remaining20..16, then down at15..10.
# Its +600 cleanup is required for the upward phase, not an escaped volley.
$arrowTerminations = @($run | Where-Object { $_.Stage -eq 'RAINING_ARROW_TERMINATION' })
foreach ($phase in 10..20) {
    $expectedEnd = if ($phase -ge 16) { 'ABOVE_LIMIT' } else { 'TERRAIN' }
    $expectedLaunch = if ($phase -ge 16) { 'ASCENDING' } else { 'FALLING_VOLLEY' }
    $launched = @($run | Where-Object {
        $_.Stage -eq 'RAINING_ARROW_LAUNCH_PHASE' -and $_.Value1 -eq $phase -and
        $_.Result -eq $expectedLaunch
    })
    $ended = @($arrowTerminations | Where-Object {
        $_.Value1 -eq $phase -and $_.Result -eq $expectedEnd
    })
    if ($launched.Count -ne $reactionCastCount -or $ended.Count -ne $launched.Count) {
        $failures += "Raining Arrow: phase $phase launch/end $($launched.Count)/$($ended.Count), expected $reactionCastCount $expectedEnd"
    }
}
if ($arrowTerminations | Where-Object {
    $_.Value1 -lt 10 -or $_.Value1 -gt 20 -or
    ($_.Value1 -lt 16 -and $_.Result -ne 'TERRAIN') -or
    ($_.Value1 -ge 16 -and $_.Result -ne 'ABOVE_LIMIT')
}) { $failures += 'Raining Arrow: unknown launch phase or incorrect phase termination' }
$rawArrowEnds = @($run | Where-Object {
    $_.Stage -eq 'PROJECTILE_IMPACT' -and $_.Value1 -eq 2023
})
if ($rawArrowEnds.Count -ne $arrowTerminations.Count) {
    $failures += 'Raining Arrow: unclassified projectile termination'
}
$movementContracts = @(
    @{ Skill=241;  Result='FORWARD_OFFSET';      Count=$reactionCastCount; Name='Shining Bird' },
    @{ Skill=243;  Result='FORCED_IMPACT';       Count=($reactionCastCount * 5); Name='Spearstorm' },
    # Sword Blow's movement trace is intentionally latched once per client
    # process inside each updater; it proves the route, not per-cast volume.
    @{ Skill=1501; Result='SMALL_SWORD_ADVANCE'; Count=1; Name='Sword Blow small swords' },
    @{ Skill=1501; Result='BIG_SWORD_ADVANCE';   Count=1; Name='Sword Blow big sword' }
)
$movementPassCount = 0
foreach ($contract in $movementContracts) {
    $count = @($run | Where-Object {
        $_.Stage -eq 'MOVEMENT_MILESTONE' -and $_.Value1 -eq $contract.Skill -and
        $_.Result -eq $contract.Result
    }).Count
    if ($count -ne $contract.Count) {
        $failures += "$($contract.Name): expected $($contract.Count) $($contract.Result) milestone(s), got $count"
    }
    else { ++$movementPassCount }
}

# Elemental Charge 0x66481B..0x664949 creates exactly three flare03 subtype-0
# children followed by two Shockwave subtype-1 children. They are texture
# carriers outside the model-offset matrix and must also release cleanly.
$elementalCreates = @($run | Where-Object { $_.Stage -eq 'ELEMENTAL_CHILD_CREATE' })
$elementalDestroys = @($run | Where-Object { $_.Stage -eq 'ELEMENTAL_CHILD_DESTROY' })
$elementalFlareCreates = @($elementalCreates | Where-Object {
    $_.Value2 -eq 0 -and $_.Result -eq 'ALLOCATED'
})
$elementalShockwaveCreates = @($elementalCreates | Where-Object {
    $_.Value2 -eq 1 -and $_.Result -eq 'ALLOCATED'
})
if ($elementalFlareCreates.Count -ne 3 -or $elementalShockwaveCreates.Count -ne 2) {
    $failures += "Elemental Charge: expected 3 flare + 2 shockwave children, got $($elementalFlareCreates.Count) + $($elementalShockwaveCreates.Count)"
}
if ($elementalDestroys.Count -ne 5 -or
    @($elementalDestroys | Where-Object { $_.Result -ne 'RELEASED' }).Count -ne 0) {
    $failures += "Elemental Charge: expected 5 released child carriers, got $($elementalDestroys.Count)"
}
$elementalModelIds = @($run | Where-Object {
    $_.Stage -eq 'EFFECT_RENDER' -and $_.Value2 -eq 25 -and
    $_.Result -eq 'PASS'
} | ForEach-Object { $_.Value1 } | Sort-Object -Unique)
$elementalFrame = @($run | Where-Object {
    $_.Stage -eq 'EFFECT_BMD_FRAME' -and
    $elementalModelIds -contains $_.Value1 -and $_.Value2 -ge 250 -and
    $_.Result -eq 'PROGRESSED'
})
if ($elementalFrame.Count -eq 0) {
    $failures += 'Elemental Charge: animated buff BMD did not reach frame 0.25'
}

$captureRoot = Join-Path (Split-Path -Parent (Resolve-Path -LiteralPath $LogPath).Path) `
    'PegasusQACaptures'
$captureCount = 0
$deathsideRibbonTriggers = @($run | Where-Object {
    $_.Stage -eq 'QA_DEATHSIDE_RIBBON_TRIGGER' -and $_.Value1 -eq 288
})
if ($deathsideRibbonTriggers.Count -ne 1 -or
    $deathsideRibbonTriggers[0].Result -ne 'DRAW_SUBMITTED_NEXT_FRAME_CAPTURE') {
    $failures += 'Deathside: framebuffer samples not triggered by an actual ribbon draw (missing/timeout/duplicate)'
}
Add-Type -AssemblyName System.Drawing
foreach ($contract in $skills) {
    $armed = @($run | Where-Object {
        $_.Stage -eq 'QA_CAPTURE' -and $_.Value1 -eq $contract.Id -and
        $_.Result -eq 'ARMED'
    })
    $armedPhases = @($armed | ForEach-Object { $_.Value2 } | Sort-Object -Unique)
    if ($armed.Count -ne 3 -or ($armedPhases -join ',') -ne '0,1,2') {
        $failures += "$($contract.Name): expected exactly QA_CAPTURE phases 0,1,2, got $($armedPhases -join ',') ($($armed.Count) rows)"
    }
    foreach ($phase in 0..2) {
        $pattern = "qa_*_skill_$($contract.Id)_phase_$phase.jpg"
        $capture = @(Get-ChildItem -LiteralPath $captureRoot -Filter $pattern -File `
            -ErrorAction SilentlyContinue | Where-Object {
                $_.LastWriteTime -ge $start.Timestamp -and $_.Length -gt 0
            } | Sort-Object LastWriteTime -Descending | Select-Object -First 1)
        if ($capture.Count -eq 0) {
            $failures += "$($contract.Name): missing framebuffer capture phase $phase"
        }
        else {
            $image = $null
            try {
                $image = [System.Drawing.Image]::FromFile($capture[0].FullName)
                if ($image.Width -le 0 -or $image.Height -le 0) {
                    $failures += "$($contract.Name): invalid framebuffer dimensions in phase $phase"
                }
                else { ++$captureCount }
            }
            catch {
                $failures += "$($contract.Name): corrupt framebuffer capture phase $phase"
            }
            finally {
                if ($null -ne $image) { $image.Dispose() }
            }
        }
    }
}

$swordBlowCreates = @($run | Where-Object {
    $_.Stage -eq 'SWORD_BLOW_CREATE' -and $_.Result -eq 'ALLOCATED'
})
$swordBlowDestroys = @($run | Where-Object {
    $_.Stage -eq 'SWORD_BLOW_DESTROY' -and $_.Result -eq 'RELEASED'
})

$dynamicCreates = @($run | Where-Object {
    $_.Stage -eq 'EFFECT_LIFECYCLE_CREATE' -and $_.Result -eq 'ALLOCATED'
})
$dynamicDestroys = @($run | Where-Object {
    $_.Stage -eq 'EFFECT_LIFECYCLE_DESTROY' -and
    ($_.Result -eq 'RELEASED' -or $_.Result -eq 'BMD_COMPLETE')
})
$requiredDynamicOffsets = @(0..47) + 52
foreach ($offset in $requiredDynamicOffsets) {
    $created = @($dynamicCreates | Where-Object { $_.Value2 -eq $offset }).Count
    $destroyed = @($dynamicDestroys | Where-Object { $_.Value2 -eq $offset }).Count
    if ($created -eq 0) {
        $failures += "Pegasus dynamic model offset $offset was never allocated"
    }
    elseif ($created -ne $destroyed) {
        $failures += "Pegasus dynamic model offset $offset allocated $created but completed/released $destroyed"
    }
}

# Offset 48 (ReaperHead) is load/cleanup-addressable but has no creation xref
# in the pinned executable. Offsets 49..51 are the three randomized meshless
# scythe carriers, so a bounded run must create and release at least the branch
# selected by its Deathside repeat cast without pretending all three were used.
$deathsideScytheOffsets = @(49,50,51)
$deathsideScytheCreates = @($dynamicCreates | Where-Object {
    $deathsideScytheOffsets -contains $_.Value2
})
$deathsideScytheDestroys = @($dynamicDestroys | Where-Object {
    $deathsideScytheOffsets -contains $_.Value2
})
if ($deathsideScytheCreates.Count -eq 0) {
    $failures += 'Deathside: no randomized scythe carrier was allocated'
}
foreach ($offset in $deathsideScytheOffsets) {
    $created = @($deathsideScytheCreates | Where-Object { $_.Value2 -eq $offset }).Count
    $destroyed = @($deathsideScytheDestroys | Where-Object { $_.Value2 -eq $offset }).Count
    if ($created -ne $destroyed) {
        $failures += "Deathside scythe offset $offset allocated $created but completed/released $destroyed"
    }
}

foreach ($offset in 0..8) {
    $created = @($swordBlowCreates | Where-Object { $_.Value2 -eq $offset }).Count
    $destroyed = @($swordBlowDestroys | Where-Object { $_.Value2 -eq $offset }).Count
    if ($created -eq 0) {
        $failures += "Sword Blow: fixed model offset $offset was never allocated"
    }
    elseif ($created -ne $destroyed) {
        $failures += "Sword Blow: fixed model offset $offset allocated $created but released $destroyed"
    }
}

$poolFull = @($run | Where-Object {
    ($_.Stage -eq 'EFFECT_CREATE' -or $_.Stage -eq 'SWORD_BLOW_CREATE') -and
    $_.Result -eq 'POOL_FULL'
})
if ($poolFull.Count -ne 0) { $failures += "effect pool exhausted $($poolFull.Count) times" }
$renderFailures = @($run | Where-Object {
    $_.Stage -eq 'EFFECT_RENDER' -and $_.Result -eq 'FAIL'
})
$transparentSkips = @($run | Where-Object {
    $_.Stage -eq 'EFFECT_RENDER' -and $_.Result -eq 'TRANSPARENT_SKIP'
})
if ($renderFailures.Count -ne 0) {
    $failedOffsets = ($renderFailures | ForEach-Object { $_.Value2 } |
        Sort-Object -Unique) -join ','
    $failures += "hard renderer failures $($renderFailures.Count) at model offsets $failedOffsets"
}
if (-not $poolSummary) {
    $failures += 'delayed AUTO_QA_POOL cleanup snapshot is missing'
} elseif ($poolSummary.Result -ne 'CLEAN' -or $poolSummary.Value1 -ne 0) {
    $failures += "dynamic effects still live after QA: $($poolSummary.Value1)"
}
$childPool = @($run | Where-Object {
    $_.Stage -eq 'AUTO_QA_POOL_CHILDREN'
} | Select-Object -Last 1)
if ($childPool.Count -ne 1) {
    $failures += 'Pegasus joint/particle cleanup snapshot is missing'
} elseif ($childPool[0].Result -ne 'CLEAN' -or
    $childPool[0].Value1 -ne 0 -or $childPool[0].Value2 -ne 0) {
    $failures += "Pegasus joints/particles still live after QA: $($childPool[0].Value1)/$($childPool[0].Value2)"
}

$summary | Format-Table -AutoSize
$visualSummary | Format-Table -AutoSize
Write-Host "ALLOCATED=$(@($run | Where-Object { $_.Stage -eq 'EFFECT_CREATE' -and $_.Result -eq 'ALLOCATED' }).Count)"
Write-Host "RENDER_PASS=$(@($run | Where-Object { $_.Stage -eq 'EFFECT_RENDER' -and $_.Result -eq 'PASS' }).Count)"
Write-Host "RENDER_TRANSPARENT_SKIP=$($transparentSkips.Count)"
Write-Host "RENDER_HARD_FAIL=$($renderFailures.Count)"
Write-Host "ELEMENTAL_CHILDREN=$($elementalCreates.Count)/$($elementalDestroys.Count)"
Write-Host "MULTIKEY_BMD_PROGRESS=$bmdProgressCount/$($animatedContracts.Count)"
Write-Host "ATTACK_DAMAGE_PACKETS=$damagePassCount/$($damageContracts.Count)"
Write-Host "TARGET_REACTIONS=$reactionPassCount/$($damageContracts.Count)"
$reactionGateSummary | ForEach-Object { Write-Host $_ }
Write-Host "SKILL_LIST_PRESENT=$skillListPassCount/$($skills.Count)"
Write-Host "BUFF_STATE_ACTIVE=$buffActiveCount/$($buffContracts.Count) CLEARED=$buffClearedCount/$($buffContracts.Count)"
Write-Host "CUSTOM_SOUND_PLAYBACK=$soundPassCount/$($soundContracts.Count) HAVOC_HIT=$($havocHitSound.Count -gt 0)"
Write-Host "DEATHSIDE_ATTACK_SOUND=$($deathsideAttackSound.Count -gt 0) SCYTHE_POOL=$($deathsideScytheCreates.Count)/$($deathsideScytheDestroys.Count)"
Write-Host "MOVEMENT_COLLISION=$movementPassCount/$($movementContracts.Count) RAINING_IMPACTS=$($terrainImpacts.Count)"
Write-Host "SWORD_BLOW_FIXED_POOL=$($swordBlowCreates.Count)/$($swordBlowDestroys.Count)"
Write-Host "DYNAMIC_LIFECYCLE=$($dynamicCreates.Count)/$($dynamicDestroys.Count)"
Write-Host "FRAMEBUFFER_CAPTURES=$captureCount/45"
Write-Host "POOL_FULL=$($poolFull.Count)"
Write-Host "JOINT_PARTICLE_POOL_FULL=$($childPoolFailures.Count)"
$serverDeclarations = @($run | Where-Object { $_.Stage -eq 'SERVER_QA_BUILD_DECLARATION' })
if ($serverDeclarations.Count -eq 0) {
    Write-Host 'SERVER_BUILD_DECLARATION=UNAVAILABLE (connected server revision is not established)'
} else {
    foreach ($declaration in $serverDeclarations) {
        Write-Host "SERVER_BUILD_DECLARATION=$($declaration.Result) (self-reported, not EXE hash)"
    }
}
Write-Host "PEGASUS_CHILD_CREATE=$($jointCreates.Count)/$($particleCreates.Count)"
if ($poolSummary) {
    Write-Host "ACTIVE_DYNAMIC_AFTER_QA=$($poolSummary.Value1)"
}
if ($childPool.Count -eq 1) {
    Write-Host "ACTIVE_JOINTS_PARTICLES_AFTER_QA=$($childPool[0].Value1)/$($childPool[0].Value2)"
}

if ($failures.Count -ne 0) {
    Write-Host 'FAIL: runtime QA contract:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}

Write-Host 'PASS: one bounded live QA run reached all 15 skill steps and 79 expected dispatches; all 14 Pegasus caster actions progressed; no effect-pool exhaustion'
