param(
    [Parameter(Mandatory = $true)]
    [string]$LogPath
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path -LiteralPath $LogPath -PathType Leaf)) {
    throw "FAIL: SlayerRuntimeQA.log not found: $LogPath"
}

$lines = @(Get-Content -LiteralPath $LogPath)
function Require([bool]$condition, [string]$message) {
    if (-not $condition) { throw "FAIL: $message" }
}

function AcceptedCast([int]$skillId, [string]$name, [int]$minimumEvents) {
    $pattern = "cast skill=$skillId name=$([regex]::Escape($name)) accepted=1 target=([0-9]+) events=([0-9]+)"
    $matches = @($lines | Where-Object { $_ -match $pattern -and [int]$Matches[2] -ge $minimumEvents })
    Require ($matches.Count -gt 0) "$name ($skillId) accepted cast with target and >= $minimumEvents events"
    return $matches[0]
}

AcceptedCast 292 'Sword Inertia' 3 | Out-Null
AcceptedCast 293 'Bat Flock' 3 | Out-Null
AcceptedCast 294 'Pierce Attack' 6 | Out-Null
AcceptedCast 295 'Detection' 1 | Out-Null

$sword = @($lines | Where-Object { $_ -match 'event skill=292 .*type=sword-projectile' })
Require ($sword.Count -ge 3) 'Sword Inertia projectile events'

$batHits = @($lines | Where-Object { $_ -match 'event skill=293 .*type=bat-hit' })
$batApply = @($lines | Where-Object { $_ -match 'event skill=293 .*type=bat-dot-applied .*durationMs=5000' })
$batTicks = @($lines | Where-Object { $_ -match 'event skill=293 .*type=bat-dot-tick' })
Require ($batHits.Count -ge 2) 'Bat Flock two-hit events'
Require ($batApply.Count -ge 1) 'Bat Flock five-second DOT application'
Require ($batTicks.Count -ge 4) 'Bat Flock one-second DOT ticks'

$pierceDash = @($lines | Where-Object { $_ -match 'event skill=294 .*type=pierce-dash' })
$pierceHits = @($lines | Where-Object { $_ -match 'event skill=294 .*type=pierce-hit' })
$pierceReturn = @($lines | Where-Object { $_ -match 'event skill=294 .*type=pierce-return' })
Require ($pierceDash.Count -ge 1) 'Pierce Attack dash event'
Require ($pierceHits.Count -ge 4) 'Pierce Attack four-hit Bat Flock branch'
Require ($pierceReturn.Count -ge 1) 'Pierce Attack return event'

$detection = @($lines | Where-Object { $_ -match 'event skill=295 .*type=detection-mark .*target=-1' })
Require ($detection.Count -ge 1) 'Detection self mark event'

Write-Output ('PASS: Slayer runtime QA log proves accepted cast chains for 292/293/294/295; ' +
    "Sword=$($sword.Count) BatHits=$($batHits.Count) BatTicks=$($batTicks.Count) " +
    "PierceHits=$($pierceHits.Count) DetectionMarks=$($detection.Count)")
