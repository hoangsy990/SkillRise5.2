param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\CircleShield"
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
$env:PYTHONIOENCODING = 'utf-8'

function Assert-CopyPinnedAsset([string]$Source, [string]$ExpectedSha256, [string]$Destination) {
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath $Source).Hash
    if ($actual -ne $ExpectedSha256) { throw "Source SHA-256 mismatch for ${Source}: expected=$ExpectedSha256 actual=$actual" }
    if (Test-Path -LiteralPath $Destination) {
        $existing = (Get-FileHash -Algorithm SHA256 -LiteralPath $Destination).Hash
        if ($existing -ne $ExpectedSha256) {
            throw "Refusing to overwrite different staged asset ${Destination}: actual=$existing"
        }
        return
    }
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    $copied = (Get-FileHash -Algorithm SHA256 -LiteralPath $Destination).Hash
    if ($copied -ne $ExpectedSha256) { throw "Staged SHA-256 mismatch for ${Destination}: expected=$ExpectedSha256 actual=$copied" }
}

& py -3 $converter (Join-Path $sourceEffect 'circle_shields.bmd') (Join-Path $resolvedOutput 'circle_shields.bmd') `
    --expected-sha256 '6A985AC9EBF517DD86C164F23DBF6D0F66DD527BFEF1D3144B75D77168FA1A98' `
    --expected-name 'circle_shields.SMD'
if ($LASTEXITCODE -ne 0) { throw 'circle_shields.bmd conversion failed' }
$outputHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput 'circle_shields.bmd')).Hash
if ($outputHash -ne '79D241ACBAB24D8B31D8DB357D2D75B453431A6AF6D27CA48B2CC2A971DC7BDB') {
    throw "Converted SHA-256 mismatch for circle_shields.bmd: actual=$outputHash"
}

$copies = @(
    @{ Root=$sourceEffect; File='circle_shields.OZJ'; Hash='F5ED70BAF0746EB40CC7687A49242D2F0E6DAEB26B3B4C442C65CF9225B83A0C' },
    @{ Root=$sourceEffect; File='force_Pillar.OZJ'; Hash='921F24FEA42D3182130BACAED610720640F71B5D46155A775D54F5BA61691898' },
    @{ Root=$sourceEffect; File='shiny04.OZJ'; Hash='568A27A2B3F0E8B8004FDA7DA9CBC9F1C3A83E933AC159554CB5F3A573CC3B13' },
    @{ Root=$sourceEffect; File='flare01.OZJ'; Hash='874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8' },
    @{ Root=$sourceEffect; File='firehik_mono01.OZJ'; Hash='C3E18C474BA5AE9F06B9E7159AFC0DAB34869B78EAB425CB5538B94FE0E7138F' },
    @{ Root=$sourceSound; File='CircleShield.wav'; Hash='3B2A719E6A69DBEEC590AF595A98D96C32E3E8262315E51D47CD48EBE020E7B3' }
)
foreach ($copy in $copies) {
    Assert-CopyPinnedAsset (Join-Path $copy.Root $copy.File) $copy.Hash (Join-Path $resolvedOutput $copy.File)
}

Write-Output 'Status=PASS'
Get-ChildItem -LiteralPath $resolvedOutput -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    Write-Output ("Asset={0}|Size={1}|Sha256={2}" -f $_.Name, $_.Length, $hash)
}
