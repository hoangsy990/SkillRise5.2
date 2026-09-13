$ErrorActionPreference = 'Stop'

$runtime = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\RISE\PegasusHavocSpearRuntime.cpp') -Raw
$updateStart = $runtime.IndexOf('void UpdateHavocSpear(')
$renderStart = $runtime.IndexOf('bool RenderHavocSpear(', $updateStart)
if ($updateStart -lt 0 -or $renderStart -lt 0) {
    throw 'Havoc Spear updater boundaries not found'
}
$update = $runtime.Substring($updateStart, $renderStart - $updateStart)
$failures = @()
# Original 666B35/666B9F/667219 pass 2013 in SkillIndex, not Skill.
# Impact shockwave needs this tag for the alpha_line2mono3 second pass.
foreach ($arguments in @(
    '0, &owner, -1, kHavocSpearSkill, 0, 0, 1.6f)',
    '0, &owner, -1, kHavocSpearSkill, 0, 0, scaleFactor * 0.85f)',
    '0, root.Owner, -1, kHavocSpearSkill, 0, 0, scale)',
    '0, &target, -1, static_cast<WORD>(skill), 0)',
    '9, &target, -1, static_cast<WORD>(skill), 0)',
    '0, &target, -1, static_cast<WORD>(skill), 0, 0,')) {
    if (-not $runtime.Contains($arguments)) {
        $failures += "Havoc child/root lost original SkillIndex slot: $arguments"
    }
}
$lineRenderStart = $runtime.IndexOf('case kHavocLineModel:', $renderStart)
$lineRenderEnd = $runtime.IndexOf('case kHavocShockwaveModel:', $lineRenderStart)
$lineRender = $runtime.Substring($lineRenderStart, $lineRenderEnd - $lineRenderStart)
if (-not $lineRender.Contains('model.RenderMesh(0, RENDER_TEXTURE | RENDER_BRIGHT,') -or
    -not $lineRender.Contains('model.RenderMesh(1, RENDER_TEXTURE | RENDER_BRIGHT,')) {
    $failures += 'Magum_line must draw mesh0 and mesh1 per 6666F2/666830'
}
foreach ($scale in @('0.7f', '8.0f')) {
    if (-not $runtime.Contains("0, root.Owner, -1, kHavocSpearSkill, 0, 0, $scale)")) {
        $failures += "Havoc root child scale=$scale lost native SkillIndex2013"
    }
}
$dispatch = Get-Content -LiteralPath (Join-Path $PSScriptRoot `
    '..\Main5.2_RISE\WSclient.cpp') -Raw
if (-not $dispatch.Contains('CreateHavocSpearRoot(*so, MagicNumber);') -or
    $dispatch.Contains('CreateHavocSpearRoot(*to, MagicNumber);')) {
    $failures += 'Havoc cast root must belong to caster, not attack target'
}
if (-not $dispatch.Contains('Models[so->Type].NumActions > rise::pegasus::kPegasusHavocSpearExtendedAction') -or
    -not $dispatch.Contains('SetAction(so, rise::pegasus::kPegasusHavocSpearExtendedAction);')) {
    $failures += 'available action284 must take precedence over fallback183'
}
if ($update.Contains('Owner->Live')) {
    $failures += 'non-Pegasus Owner::Live gate remains'
}
if ($update.Contains('VectorCopy(effect.Owner->Position, effect.Position);')) {
    $copyCount = ([regex]::Matches($update,
        [regex]::Escape('VectorCopy(effect.Owner->Position, effect.Position);'))).Count
    if ($copyCount -ne 1) {
        $failures += 'only target-impact may follow owner position'
    }
} else {
    $failures += 'target-impact owner follow is missing'
}
if (-not $update.Contains('if (!effect.Owner)')) {
    $failures += 'source pointer guards are missing'
}
if (-not $runtime.Contains('CreateEffect(kHavocLineModel') -or
    -not $runtime.Contains('CreateHavocRootJoints(*root.Owner)') -or
    -not $runtime.Contains('CreateEffect(kHavocShockwaveModel') -or
    -not $runtime.Contains('CreateEffect(kHavocNovaModel') -or
    -not $runtime.Contains('CreateEffect(kHavocEndModel')) {
    $failures += 'source-proven line/Nova/shockwave/end visual chain is incomplete'
}
if (-not $runtime.Contains('for (int n = 0; n < 4; ++n)') -or
    -not $runtime.Contains('BITMAP_LIGHTNING_MEGA1 + (rand() % 3)')) {
    $failures += 'four-particle randomized impact burst is missing'
}
if (-not $runtime.Contains('for (int n = 0; n < 2; ++n)') -or
    -not $runtime.Contains('effect.Type == kHavocImpactModel')) {
    $failures += 'target-follow two-lightning orchestrator is missing'
}
if (-not $runtime.Contains('HavocCreatesRootBurstAtTick(tick)') -or
    -not $runtime.Contains('HavocCreatesOddBurstAtTick(tick)')) {
    $failures += 'tick-1 root and odd-tick explosion cadence is missing'
}
if (-not $runtime.Contains('playSpeed = kHavocLinePlaySpeed') -or
    -not $runtime.Contains('playSpeed = kHavocShockwavePlaySpeed') -or
    -not $runtime.Contains('model.PlayAnimation(')) {
    $failures += 'source-proven line/shockwave BMD playback is missing'
}
if ($failures.Count -ne 0) {
    Write-Host 'FAIL: Havoc Spear ownership routing drifted:'
    $failures | ForEach-Object { Write-Host "  $_" }
    exit 1
}
Write-Host 'PASS: Havoc Spear ownership, line/Nova/explosion children and playback match Pegasus'
