param(
    [string]$BuildRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild'
)

$ErrorActionPreference = 'Stop'

# Explicit allowlist: these are short-lived pure helper tests.  Do not scan
# and launch every .exe under the build tree: Engine, RiseQA and GameServer
# are interactive/runtime binaries and require a fully staged client/server.
$allowed = @(
    'GrowLancerFlareParticleTest.exe',
    'GrowLancerHarshWindTest.exe',
    'GrowLancerMagicPinTickTest.exe',
    'GrowLancerObsidianTickTest.exe',
    'GrowLancerParticleTickTest.exe',
    'GrowLancerShiningMeshTest.exe',
    'GrowLancerSpinParticleTest.exe',
    'GrowLancerTickTest.exe',
    'GrowLancerWrathParticleTest.exe',
    'MagicFrameTransitions.exe',
    'MagicParticleOptions.exe',
    'breche-ground.exe',
    'circle-joint.exe',
    'circle-shiny.exe',
    'Clash.exe',
    'effect.exe',
    'flare5.exe',
    'GiantPulse.exe',
    'harsh-wind.exe',
    'magic-pin.exe',
    'MagicPin.exe',
    'Obsidian.exe',
    'particle.exe',
    'pin-joint.exe',
    'shining-mesh.exe',
    'spin-cross.exe',
    'spin-flare.exe',
    'spin-ground.exe',
    'spin-particle.exe',
    'sprite.exe',
    'Wrath.exe',
    'xsuper.exe',
    'test.exe'
)

$found = @()
foreach ($name in $allowed) {
    $candidate = Get-ChildItem -LiteralPath $BuildRoot -Recurse -File -Filter $name |
        Where-Object { $_.FullName -notmatch '\\RuntimeQA\\Client\\|\\RuntimeQA2\\Client\\|\\BodyCandidates\\' } |
        Select-Object -First 1
    if ($candidate) { $found += $candidate }
}
if ($found.Count -eq 0) { throw "No offline harnesses found below $BuildRoot" }

$failed = @()
foreach ($exe in $found) {
    $process = Start-Process -FilePath $exe.FullName -WorkingDirectory $exe.DirectoryName -PassThru -Wait -WindowStyle Hidden
    Write-Output ("{0}: exit={1}" -f $exe.Name, $process.ExitCode)
    if ($process.ExitCode -ne 0) { $failed += $exe.Name }
}
if ($failed.Count -gt 0) { throw ("Offline harness failure: " + ($failed -join ', ')) }
Write-Output ("PASS: {0} explicit offline harnesses; no Engine/RiseQA/GameServer launched" -f $found.Count)
