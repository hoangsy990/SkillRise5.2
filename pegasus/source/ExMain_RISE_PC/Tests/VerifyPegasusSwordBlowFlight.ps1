$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$runtime = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusSwordBlowRuntime.cpp')
$header = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusSwordBlowRuntime.h')
$character = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzCharacter.cpp')
$client = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\WSclient.cpp')
$effect = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzEffect.cpp')
$catalog = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusHighSkillCatalog.cpp')
$skillUi = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\NewUIMainFrameWindow.cpp')
$interface = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzInterface.cpp')
$iconCatalog = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusSkillIconCatalog.h')
$serverRoot = Split-Path -Parent $root
$serverSkill = Get-Content -Raw -LiteralPath (Join-Path $serverRoot 'ExGameServer\GameServer\SkillManager.cpp')
$serverCommand = Get-Content -Raw -LiteralPath (Join-Path $serverRoot 'ExGameServer\GameServer\CommandManager.cpp')

foreach ($token in @(
    'PLAYER_ATTACK_ONETOONE_FLY',
    'InstallSwordBlowFlightAction',
    'INSTALLED_14_KEYS',
    'StartSwordBlowFlight',
    'UpdateSwordBlowFlight',
    'Data\\Npc\\',
    'BloodCastle01.bmd',
    'RenderMesh(3, RENDER_TEXTURE | RENDER_BRIGHT',
    'kSwordBlowFlySmallSwordImageLight.red',
    'RETRIGGER_HELD_AIRBORNE',
    'DIVE_THRUST',
    'SWORD_BLOW_FLY'
)) {
    if (-not ($runtime + $header).Contains($token)) { throw "Missing Sword Blow flight contract: $token" }
}
if (-not $character.Contains('rise::pegasus::UpdateSwordBlowFlight(*c);')) {
    throw 'Character update does not run the Sword Blow flight state'
}
if (-not $client.Contains('case rise::pegasus::kSwordBlowFlySkill:') -or
    -not $client.Contains('rise::pegasus::StartSwordBlowFlight(*sc, *to);')) {
    throw 'Sword Blow Fly dispatch does not start PLAYER_ATTACK_ONETOONE_FLY'
}
$dispatchStart = $client.IndexOf("case rise::pegasus::kSwordBlowSkill:`r`n`t`tif (!ShouldDispatchPegasusCastRoot")
if ($dispatchStart -lt 0) {
    $dispatchStart = $client.IndexOf("case rise::pegasus::kSwordBlowSkill:`n`t`tif (!ShouldDispatchPegasusCastRoot")
}
$flyStart = if ($dispatchStart -ge 0) {
    $client.IndexOf('case rise::pegasus::kSwordBlowFlySkill:', $dispatchStart)
} else { -1 }
$original = if ($dispatchStart -ge 0 -and $flyStart -gt $dispatchStart) {
    $client.Substring($dispatchStart, $flyStart - $dispatchStart)
} else { '' }
if (-not $original.Contains('SetAction(so, PLAYER_ATTACK_ONETOONE);') -or
    -not $original.Contains('CreateSwordBlowCasterRoot(*sc);') -or
    $original.Contains('StartSwordBlowFlight')) {
    throw 'Original Sword Blow 1501 dispatch was modified by the Fly clone'
}
if (-not $runtime.Contains('flyVariant ? 2 : 1') -or
    -not $runtime.Contains('flyVariant ? 2 : 0')) {
    throw 'Fly-only child/root subtype isolation is missing'
}
foreach ($token in @(
    'fly = SkillAttribute[kSwordBlowSkill]',
    'strcpy_s(fly.Name, "Sword Blow Fly")')) {
    if (-not $catalog.Contains($token)) { throw "Fly client catalog clone missing: $token" }
}
if (-not $skillUi.Contains('FindHighSkillIconRoute(bySkillType)') -or
    -not $iconCatalog.Contains('if (skillId == kSwordBlowFlySkill)') -or
    -not $iconCatalog.Contains('skillId = kSwordBlowSkill;') -or
    -not $iconCatalog.Contains('1501.jpg')) {
    throw 'Sword Blow Fly source-backed individual icon alias is missing'
}
if (-not $runtime.Contains('SynchronizeSwordBlowFlightActionSpeed') -or
    -not $runtime.Contains('SwordBlowFlightPlaySpeed')) {
    throw 'Sword Blow Fly does not inherit native attack-speed cadence'
}
if (([regex]::Matches($interface, 'const bool swordBlowFlyHeld')).Count -ne 2 -or
    -not $interface.Contains('o->CurrentAction == rise::pegasus::PLAYER_ATTACK_ONETOONE_FLY')) {
    throw 'Held-mouse input guards do not permit airborne Sword Blow Fly retriggers'
}
if (-not $serverSkill.Contains('info.Index = rise::pegasus::kSwordBlowFlySkill') -or
    -not $serverSkill.Contains('case SKILL_SWORD_BLOW_FLY:') -or
    -not $serverCommand.Contains('/testswordblowfly')) {
    throw 'Sword Blow Fly server clone/test command is incomplete'
}
foreach ($route in @(
    'EnsureSwordBlowArchangelWingModel',
    'InitializeSwordBlowArchangelWings',
    'UpdateSwordBlowArchangelWings',
    'RenderSwordBlowArchangelWings'
)) {
    if (-not $effect.Contains("rise::pegasus::$route")) { throw "Missing wing effect route: $route" }
}
$flightBody = [regex]::Match($runtime,
    'void UpdateSwordBlowFlight\(CHARACTER& caster\)(?<body>[\s\S]*?)\n}\n\nvoid',
    [System.Text.RegularExpressions.RegexOptions]::None).Groups['body'].Value
if ($flightBody.Contains('Position[0] =') -or $flightBody.Contains('Position[1] =')) {
    throw 'Sword Blow flight state must not write actor XY'
}
Write-Output 'Pegasus Sword Blow PLAYER_ATTACK_ONETOONE_FLY contract PASS'
