param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\SpinStep"
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
    (Join-Path $sourceEffect 'spin_cross.bmd') `
    (Join-Path $resolvedOutput 'spin_cross.bmd') `
    --expected-sha256 'B009704237F2B98BD8A1778CAB1FD3A35C6C926B4857E9D8CFDEA7549B7A566B' `
    --expected-name 'spin_cross.SMD'
if ($LASTEXITCODE -ne 0) { throw 'spin_cross conversion failed' }

$convertedHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput 'spin_cross.bmd')).Hash
if ($convertedHash -ne '6E0B4051D4D972EB1FEFCA8BA14A037FBB90A882D3B4B3026E9150503130C15D') {
    throw "Converted SHA-256 mismatch for spin_cross.bmd: actual=$convertedHash"
}

Assert-CopyPinnedAsset `
    -Source (Join-Path $sourceSound 'SpinStep_Cast.wav') `
    -ExpectedSha256 '534A4310981B943D5646E76684AFD394AEDB694439EBB7B53517381AE09069D5' `
    -Destination (Join-Path $resolvedOutput 'SpinStep_Cast.wav')
Assert-CopyPinnedAsset `
    -Source (Join-Path $sourceSound 'SpinStep_Hit.wav') `
    -ExpectedSha256 '6C7873F5CA8885AF0B725813D6FDB7288113D07E069A0C216817C902BED65A26' `
    -Destination (Join-Path $resolvedOutput 'SpinStep_Hit.wav')

$textures = @(
    @{ Name = 'line_fire.OZJ'; Hash = '83B829FC5322CC5FEB4CC593A1756F95A53926540322222E843467C803A59DDF' },
    @{ Name = 'flareBlue.OZJ'; Hash = 'E261B59F57FDF0C514917A388F02AC66B832C2BE9F97A3F90BC36C98D7B2FB32' },
    @{ Name = 'hole.OZJ'; Hash = '01A8B92665F07213D6C5807AC6CA038AB5257A9A1C395A86EFB8113DF92B42F0' },
    @{ Name = 'magic_ground1_3.OZJ'; Hash = 'CACB425B05C68D1279A3B30419C5DC5626947AD66C126D73C4B5911F42C47D5F' }
)
foreach ($texture in $textures) {
    Assert-CopyPinnedAsset `
        -Source (Join-Path $sourceEffect $texture.Name) `
        -ExpectedSha256 $texture.Hash `
        -Destination (Join-Path $resolvedOutput $texture.Name)
}

Write-Output 'Status=PASS'
Write-Output "OutputRoot=$resolvedOutput"
Get-ChildItem -LiteralPath $resolvedOutput -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    Write-Output ("Asset={0}|Size={1}|Sha256={2}" -f $_.Name, $_.Length, $hash)
}
