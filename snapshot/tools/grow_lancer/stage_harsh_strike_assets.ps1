param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\HarshStrike"
)

$ErrorActionPreference = 'Stop'
$worktreeRoot = [System.IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer')
$resolvedOutput = [System.IO.Path]::GetFullPath($OutputRoot)
if (-not $resolvedOutput.StartsWith($worktreeRoot + [System.IO.Path]::DirectorySeparatorChar, [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputRoot must remain below $worktreeRoot"
}

$converter = Join-Path $worktreeRoot 'tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py'
$sourceEffect = 'D:\MU FICA Season 21\Data\Effect'
$sourceSound = 'D:\MU FICA Season 21\Data\Sound'
New-Item -ItemType Directory -Force -Path $resolvedOutput | Out-Null

function Assert-CopyPinnedAsset {
    param(
        [Parameter(Mandatory = $true)][string]$Source,
        [Parameter(Mandatory = $true)][string]$ExpectedSha256,
        [Parameter(Mandatory = $true)][string]$Destination
    )

    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath $Source).Hash
    if ($actual -ne $ExpectedSha256) {
        throw "Source SHA-256 mismatch for ${Source}: expected=$ExpectedSha256 actual=$actual"
    }
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    $copied = (Get-FileHash -Algorithm SHA256 -LiteralPath $Destination).Hash
    if ($copied -ne $ExpectedSha256) {
        throw "Staged SHA-256 mismatch for ${Destination}: expected=$ExpectedSha256 actual=$copied"
    }
}

& py -3 $converter `
    (Join-Path $sourceEffect 'h_strike_wind01.bmd') `
    (Join-Path $resolvedOutput 'h_strike_wind01.bmd') `
    --expected-sha256 '9ADE987E7FC4DC3476545B033886307F2BBFFD9555631D934F53E89AB5A1A9FB' `
    --expected-name 'h_strike_wind01.SMD'
if ($LASTEXITCODE -ne 0) { throw 'h_strike_wind01 conversion failed' }

& py -3 $converter `
    (Join-Path $sourceEffect 'h_strike_wind02.bmd') `
    (Join-Path $resolvedOutput 'h_strike_wind02.bmd') `
    --expected-sha256 'ACF415AE0B0EA77AFD0A4F78C9B85142B5CBFC9DC2C887C2E39E9B1CA3B8956E' `
    --expected-name 'h_strike_wind02.SMD'
if ($LASTEXITCODE -ne 0) { throw 'h_strike_wind02 conversion failed' }

$assets = @(
    @{ Source = (Join-Path $sourceEffect 'Magic_Ground2.OZJ'); Hash = '2BF23AC3EFF0B01E43FA88CE20CFFB175B082F98F4F22A2C970F18CC4FA465F1' },
    @{ Source = (Join-Path $sourceEffect 'flare01_red.OZJ'); Hash = '8878E3200C74BFDDC7B56AB07FAA30AC5E6E1995B9671C204B509B865C7C6190' },
    @{ Source = (Join-Path $sourceEffect 'Impack03.OZJ'); Hash = '22F9F2481C6B7CB6CE4A4D674E72A35D72DC20D20B850674B3C06AE3BE84019D' },
    @{ Source = (Join-Path $sourceEffect 'magic_ground3.OZJ'); Hash = '3D62346D751E068044C34EA11DC716D130D871BB7608AC1C490ED37F781F0E68' },
    @{ Source = (Join-Path $sourceEffect 'Damage1mono.OZJ'); Hash = 'A095432CB1BAA4FE21BBAB6D7A3E4422C8C114C1E9BC4B0AE563FBE7D1078D31' },
    @{ Source = (Join-Path $sourceSound 'HashStrike.wav'); Hash = '325282E05124188A9FE28AA4F05F57DE1709AB0B08C76E3813300E3184308AE6' }
)

foreach ($asset in $assets) {
    Assert-CopyPinnedAsset -Source $asset.Source -ExpectedSha256 $asset.Hash -Destination (Join-Path $resolvedOutput (Split-Path $asset.Source -Leaf))
}

$expectedOutputHashes = @{
    'h_strike_wind01.bmd' = 'E6C1A5DB8CA857D7B98CD48990B211BE142203196EBF148C1F38E34A76A5243E'
    'h_strike_wind02.bmd' = '64B6F343A94091EB52C95B62A395FD228BC160BA758E08D378D570B26B5B8851'
}
foreach ($entry in $expectedOutputHashes.GetEnumerator()) {
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput $entry.Key)).Hash
    if ($actual -ne $entry.Value) {
        throw "Converted SHA-256 mismatch for $($entry.Key): expected=$($entry.Value) actual=$actual"
    }
}

Write-Output 'Status=PASS'
Write-Output "OutputRoot=$resolvedOutput"
Get-ChildItem -LiteralPath $resolvedOutput -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    Write-Output ("Asset={0}|Size={1}|Sha256={2}" -f $_.Name, $_.Length, $hash)
}
