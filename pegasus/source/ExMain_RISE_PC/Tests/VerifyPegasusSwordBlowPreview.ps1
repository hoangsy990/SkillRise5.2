$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
$scene = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzScene.cpp')
$login = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\LoginWin.cpp')
$server = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\ServerSelWin.cpp')
$client = Get-Content -Raw -LiteralPath (Join-Path $root 'Main5.2_RISE\WSclient.cpp')

foreach ($text in @($scene, $login, $server, $client)) {
    if ($text -notmatch 'PegasusSwordBlowPreview\.enable') {
        throw 'Focused preview marker is missing from an auto-login/cast path'
    }
}
foreach ($token in @(
    'RunSwordBlowFlyLocalPreview(g_PegasusSwordBlowPreviewCast)',
    'SWORD_BLOW_PREVIEW',
    'g_PegasusSwordBlowPreviewCast >= 3',
    '!IsPegasusSwordBlowPreviewEnabled() && IsPegasusAutoQaEnabled()')) {
    if (-not $scene.Contains($token)) {
        throw "Focused preview contract missing: $token"
    }
}
Write-Output 'Pegasus Sword Blow focused preview contract PASS'
