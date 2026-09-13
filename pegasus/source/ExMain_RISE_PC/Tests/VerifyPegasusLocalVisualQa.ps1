$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$scene = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzScene.cpp')
$client = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\WSclient.cpp')
$header = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\RISE\PegasusLocalVisualQa.h')

$requiredScene = @(
    'PegasusLocalVisualQA.enable',
    'RunPegasusLocalVisualQaStep',
    'RuntimeQaCastCount',
    'LOCAL_VISUAL_QA',
    '!IsPegasusLocalVisualQaEnabled() &&'
    '!IsPegasusSwordBlowPreviewEnabled() && IsPegasusAutoQaEnabled()'
)
$requiredClient = @(
    'LOCAL_VISUAL_ECHO',
    'LOCAL_VISUAL_DISPATCH',
    'LOCAL_VISUAL_TARGET',
    'FindPegasusLocalVisualTarget',
    'CreatePegasusLocalVisualTargetLayers',
    'NativeEffectForHighSkillBuff',
    'ReceiveMagic(reinterpret_cast<BYTE*>(&packet)'
)
foreach ($token in $requiredScene) {
    if (-not $scene.Contains($token)) { throw "Missing scene contract: $token" }
}
foreach ($token in $requiredClient) {
    if (-not $client.Contains($token)) { throw "Missing client contract: $token" }
}
if (-not $header.Contains('renderer harness only')) {
    throw 'Local QA header must retain visual-only acceptance boundary'
}
$guardMatch = [regex]::Match($scene,
    'else if \(!IsPegasusLocalVisualQaEnabled\(\) &&\s*!IsPegasusSwordBlowPreviewEnabled\(\) && IsPegasusAutoQaEnabled\(\) &&')
$guard = if ($guardMatch.Success) { $guardMatch.Index } else { -1 }
$send = $scene.IndexOf('SendChat("/qa15");', $guard)
if ($guard -lt 0 -or $send -lt $guard) {
    throw 'Local visual mode does not prove that /qa15 server send is suppressed'
}
Write-Output 'Pegasus isolated local visual QA contract PASS'
