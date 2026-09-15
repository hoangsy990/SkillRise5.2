param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Breche\Owner"
)

$ErrorActionPreference = 'Stop'
$worktreeRoot = [System.IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer')
$resolvedOutput = [System.IO.Path]::GetFullPath($OutputRoot)
if (-not $resolvedOutput.StartsWith($worktreeRoot + [System.IO.Path]::DirectorySeparatorChar,
        [System.StringComparison]::OrdinalIgnoreCase)) {
    throw "OutputRoot must remain below $worktreeRoot"
}

$sourceEffect = 'D:\MU FICA Season 21\Data\Effect'
$sourceSkill = 'D:\MU FICA Season 21\Data\Skill'
$converter = Join-Path $worktreeRoot 'tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py'
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

# Owner-side 0x80BC/0x81EC/0x81EB bitmap inputs.  The .jpg name in the
# descriptor is intentional: RISE's OpenJpeg resolver exchanges it for .OZJ.
$textures = @(
    @{ Name = 'ring_of_gradation.OZJ'; Hash = '2E3FAD479009A7686C51CB2719B52CE0FED452C494A9105B946C01CB3B20C284'; Root = $sourceEffect },
    @{ Name = 'firering01.OZJ'; Hash = '2A5EEEA9579CE765E5BD133FD672F3692D1E7B32F3C5DB8BD0FC40459A3B80A6'; Root = $sourceEffect },
    @{ Name = 'lightmarks_red.OZJ'; Hash = 'E447C3F15664B277A2278360317C08C86CCF8921DA1746FE0084E012A5D7608E'; Root = $sourceEffect },
    @{ Name = 'twlighthik01.OZJ'; Hash = '6BE0AE4DC839CD40C6B63AA1CC74847E33326CE441194714440EA93FB1DC7A8E'; Root = $sourceSkill },
    @{ Name = 'twlighthik02.OZJ'; Hash = '9872B87E5A87A965B61A3865F217A5973C39FB58D54E12A7858F560FD9C04B69'; Root = $sourceSkill },
    @{ Name = 'ground_wind.OZJ'; Hash = '8FB62BE821614F63C046C5BEECE31386B9A9C56D91CF71DA0D52EACEF5DDC4C8'; Root = $sourceEffect },
    @{ Name = 'pin_lights.OZJ'; Hash = 'A8C4DB1921178C7A4905F0398A744B49AB8D283DE36051584401566CE3FB2288'; Root = $sourceEffect }
)
foreach ($texture in $textures) {
    $source = Join-Path $texture.Root $texture.Name
    if (-not (Test-Path -LiteralPath $source -PathType Leaf)) {
        throw "Missing owner asset: $source"
    }
    Assert-CopyPinnedAsset $source $texture.Hash (Join-Path $resolvedOutput $texture.Name)
}

$windSource = Join-Path $sourceEffect 'wind_foce.bmd'
$windOutput = Join-Path $resolvedOutput 'wind_foce.bmd'
& py -3 $converter $windSource $windOutput `
    --expected-sha256 '275FDC22CBA39FE2D64A08E05589A30C48585FC3BFB8D299EBFADF6E66844491' `
    --expected-name-hex '443A5C50726F6A6563745F54656D705C4B6F72656150726F6A6563745CBDC500'
if ($LASTEXITCODE -ne 0) { throw 'wind_foce conversion failed' }

Write-Output 'Status=PASS'
Get-ChildItem -LiteralPath $resolvedOutput -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    Write-Output ("Asset={0}|Size={1}|Sha256={2}" -f $_.Name, $_.Length, $hash)
}
