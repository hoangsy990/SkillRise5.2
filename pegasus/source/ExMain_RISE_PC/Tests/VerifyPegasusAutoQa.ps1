$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$login = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\LoginWin.cpp') -Raw
$servers = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\ServerSelWin.cpp') -Raw
$scene = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzScene.cpp') -Raw
$client = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\WSclient.cpp') -Raw
$blurs = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzEffectBlurSpark.cpp') -Raw

function Require-Text([string]$Text, [string]$Needle, [string]$Description) {
    if (-not $Text.Contains($Needle)) {
        throw "Missing isolated auto-QA contract: $Description"
    }
}

Require-Text $login '#if defined(PEGASUS_ISOLATED_TEST)' 'compile guard'
Require-Text $login '"PegasusAutoQA.enable"' 'explicit opt-in marker'
Require-Text $login 'GetFileAttributesA(markers[i])' 'opt-in marker filesystem check'
Require-Text $login 'this->FirstLoad = 2;' 'saved-credential native login trigger'
Require-Text $login '"AUTO_QA_LOGIN"' 'login trace'
Require-Text $servers 'GetServerGroupByBtnPos(i)' 'native server-group selection'
Require-Text $servers 'if (m_iSelectServerBtnIndex == -1 && TryPegasusAutoQaSelect())' 'packet-driven group selection'
Require-Text $servers 'SendRequestServerList();' 'native server-list refresh'
Require-Text $servers 'qaServer->m_iPercent >= 100' 'full-server rejection'
Require-Text $servers 'SendRequestServerAddress(qaServer->m_iConnectIndex);' 'native server-address request'
Require-Text $servers '"AUTO_QA_SERVER"' 'server selection trace'
Require-Text $scene '(CTLCODE_08OPERATOR | CTLCODE_20OPERATOR)' 'GM character preference'
Require-Text $scene '::StartGame();' 'native character join route'
Require-Text $scene 'Hero->Object.Live' 'joined-character readiness gate'
Require-Text $scene '(now - g_PegasusAutoQaReadyTick) >= 5000' 'post-join delay'
Require-Text $scene 'SendChat("/qa15");' 'native chat command route'
Require-Text $scene '"PegasusAutoQACastOnly.enable"' 'manual-auth cast-only marker'
Require-Text $scene '"AUTO_QA_COMMAND"' 'command trace'
Require-Text $scene 'g_PegasusQaWaitDeathsideRibbon = skill == 288;' 'Deathside event-gated capture arm'
Require-Text $scene '"TIMEOUT_NO_RIBBON_DRAW"' 'missing Deathside ribbon is explicit'
Require-Text $scene '"DRAW_SUBMITTED_NEXT_FRAME_CAPTURE"' 'Deathside draw-trigger evidence'
Require-Text $scene 'g_PegasusQaCaptureSkill == 288 ? 100 :' 'short radial slash capture interval'
Require-Text $scene 'g_PegasusDeathsideCycleFrame < 12' 'bounded Deathside full-cycle diagnostic'
Require-Text $scene 'qa_%lu_deathside_cycle_%02d.jpg' 'separate full-cycle filenames'
Require-Text $scene 'g_PegasusDeathsideCycleNextTick = GetTickCount() + 75;' 'full-cycle sample cadence'
Require-Text $scene '"DEATHSIDE_CYCLE_CAPTURE"' 'full-cycle elapsed evidence'
$objectBlurRenderer = $blurs.Substring($blurs.IndexOf('void RenderObjectBlurs()'))
if ($objectBlurRenderer -notmatch '(?s)if \(deathsideBlur\)\s+\{\s+NotifyPegasusQaDeathsideRibbonDraw\(\);\s+TraceDeathsideRibbonSubmission\(\*b\);\s+\}\s+#endif\s+glBegin\(GL_TRIANGLE_FAN\)') {
    throw 'Deathside capture must be triggered at an actual ribbon draw, after segment rejection'
}
Require-Text $scene 'g_PegasusAutoQaSequenceCompleteTick != 0' 'last-skill completion gate'
Require-Text $scene '(GetTickCount() - g_PegasusAutoQaSequenceCompleteTick) >= 15000' 'post-sequence cleanup delay'
Require-Text $scene '"AUTO_QA_SEQUENCE"' 'sequence completion trace'
Require-Text $scene '"AUTO_QA_POOL"' 'post-sequence Pegasus-pool snapshot'
Require-Text $scene 'activePegasus == 0 ? "CLEAN" : "LEAKED_PEGASUS"' 'runtime cleanup verdict'
Require-Text $scene 'IsElementalChargeChild' 'Elemental bitmap-child coverage'
Require-Text $client 'ClearPegasusAutoQaStaleBuffs(MagicNumber, so);' 'per-sample local visual isolation'
Require-Text $client 'GetFileAttributesA("PegasusAutoQACastOnly.enable")' 'cast-only cleanup isolation'
Require-Text $client 'NativeEffectForHighSkillBuff(skill)' 'current-buff retention'
Require-Text $client 'skill == rise::pegasus::kSwordBlowSkill' 'Sword Wrath retention for Sword Blow'
Require-Text $client 'skill == rise::pegasus::kSwordWrathSkill' 'current Sword Wrath marker retention'
Require-Text $client 'kDeathsideReaperModel, owner' 'Deathside visual isolation on the following sample'
Require-Text $client '"AUTO_QA_DEATHSIDE_CLEANUP"' 'Deathside cleanup telemetry'
Require-Text $client '"AUTO_QA_BUFF_CLEANUP"' 'local cleanup telemetry'
Require-Text $client 'MarkPegasusAutoQaSequenceComplete();' 'Wind Soul closes the full sequence'
$actionTrace = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusCasterActionTrace.cpp') -Raw
Require-Text $actionTrace '"CAST_ACTION_SELECT"' 'per-skill caster action selection telemetry'
Require-Text $actionTrace '"CAST_ACTION_FRAME"' 'per-skill caster BMD frame telemetry'
Require-Text $actionTrace '"STATIONARY"' 'stationary animation failure verdict'

if ($login.Contains('PegasusAutoQACastOnly.enable') -or
    $servers.Contains('PegasusAutoQACastOnly.enable')) {
    throw 'Cast-only marker must never enter saved-credential login or server selection'
}

Write-Host 'PASS: isolated auto-QA supports manual-auth cast-only mode plus full action/frame/cleanup telemetry'
