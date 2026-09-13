$ErrorActionPreference = 'Stop'

$root = Split-Path -Parent $PSScriptRoot
$interface = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzInterface.cpp') -Raw
$receive = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\WSclient.cpp') -Raw
$particles = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzEffectParticle.cpp') -Raw
$objects = Get-Content -LiteralPath (Join-Path $root 'Main5.2_RISE\ZzzObject.cpp') -Raw

function Get-CaseBody {
    param([string]$Text, [string]$Start, [string]$End)
    $startIndex = $Text.IndexOf($Start, [System.StringComparison]::Ordinal)
    if ($startIndex -lt 0) { throw "Missing case start: $Start" }
    $endIndex = $Text.IndexOf($End, $startIndex + $Start.Length,
        [System.StringComparison]::Ordinal)
    if ($endIndex -lt 0) { throw "Missing case end after $Start`: $End" }
    $Text.Substring($startIndex, $endIndex - $startIndex)
}

$local = Get-CaseBody $interface 'case AT_SKILL_WIND_SOUL:' 'case AT_SKILL_GAOTIC:'
$received = Get-CaseBody $receive 'case AT_SKILL_WIND_SOUL:' 'case AT_SKILL_GAOTIC:'
if ($local -notmatch 'SendRequestMagicContinue\s*\(') {
    throw 'Local Wind Soul branch no longer sends the native duration request.'
}
foreach ($call in @('CreateBase737Root','CreateJoint','CreateEffect','PlayBuffer','SendCharacterMove','ApplyBase737CasterAction')) {
    if ($local.Contains($call)) { throw "Local branch must remain send-only; found $call." }
}
foreach ($call in @('ApplyBase737CasterAction','CreateBase737Root')) {
    if ([regex]::Matches($received, "\b$call\s*\(").Count -ne 1) {
        throw "Received branch must call $call exactly once."
    }
}
if ($received -match 'SendRequestMagicContinue\s*\(') {
    throw 'Received branch must not send another cast request.'
}

$cloudConstructor = Get-CaseBody $particles 'case BITMAP_CLUD64:' 'case BITMAP_SMOKE:'
if ($cloudConstructor -match 'case\s+80\s*:') {
    throw 'Wind Soul smoke subtype 80 drifted into the cloud-bitmap constructor.'
}
$smokeConstructor = Get-CaseBody $particles 'case BITMAP_SMOKE:' 'case BITMAP_SMOKE + 1:'
foreach ($required in @('case 80:', 'o->LifeTime = 40', 'rand() % 64 + 64',
    'rand() % 32 + 60')) {
    if (-not $smokeConstructor.Contains($required)) {
        throw "Wind Soul BITMAP_SMOKE constructor is missing: $required"
    }
}

$desairRenderer = Get-CaseBody $objects `
    'else if (o->Type == MODEL_DESAIR)' `
    'else if (o->Type == MODEL_DARK_SCREAM)'
foreach ($required in @('o->SubType == 2', 'VectorCopy(o->Light, b->BodyLight)',
    'RENDER_TEXTURE | RENDER_BRIGHT')) {
    if (-not $desairRenderer.Contains($required)) {
        throw "Wind Soul desair renderer is missing: $required"
    }
}

Write-Host 'PASS: Wind Soul cast ownership, smoke constructor and luminous desair renderer are guarded.'
