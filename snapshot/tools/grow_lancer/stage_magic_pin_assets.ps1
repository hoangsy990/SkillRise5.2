param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\MagicPin"
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

function Assert-CopyPinnedAsset([string]$Source, [string]$ExpectedSha256, [string]$Destination) {
    $actual = (Get-FileHash -Algorithm SHA256 -LiteralPath $Source).Hash
    if ($actual -ne $ExpectedSha256) { throw "Source SHA-256 mismatch for ${Source}: expected=$ExpectedSha256 actual=$actual" }
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    $copied = (Get-FileHash -Algorithm SHA256 -LiteralPath $Destination).Hash
    if ($copied -ne $ExpectedSha256) { throw "Staged SHA-256 mismatch for ${Destination}: expected=$ExpectedSha256 actual=$copied" }
}

$models = @(
    @{ File='magicpin01.bmd'; SourceHash='2C1F5FD48EA22FD4497A749F46397F31769BFAED74AA3F0672E52C78EF1CF292'; Name='magicpin01.SMD'; OutputHash='63C557190E4675476B6BCD3D91826078A4B01430A0C056B88A33D34E5BBA25A9' },
    @{ File='magicpin03_new.bmd'; SourceHash='6F3BFA98A5B8BC1A792E92130B72142BC3FB84C9B882AA4BA3DA7CE883DA9B67'; Name='magicpin03_new.SMD'; OutputHash='20400EFCBD810E82B63F9BE511D812B7C3EB8BEF3F885C8B82B261EE27C4ED9F' },
    @{ File='magicpina01_new.bmd'; SourceHash='2CF882BE345DC9088EF6345CAF65FBCD64D8F5C0AFDE78877C89EE9E7282B977'; Name='magicpina01_new.SMD'; OutputHash='4963E0278FE8F3D4B42F683CFA24BCF8E178DBB210FC23FE8DA10FB3D5A90D72' },
    @{ File='magicpina02.bmd'; SourceHash='DC913B31FE27293687DB38352F74D52785179636D8EB713227A7AE39A206B95C'; Name='magicpina02.SMD'; OutputHash='04EBD89D5A28A2C583B8FFE99BA8DC9F155772D4B78327EB072847F4A4AD4986' }
)
foreach ($model in $models) {
    & py -3 $converter (Join-Path $sourceEffect $model.File) (Join-Path $resolvedOutput $model.File) --expected-sha256 $model.SourceHash --expected-name $model.Name
    if ($LASTEXITCODE -ne 0) { throw "$($model.File) conversion failed" }
    $outputHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput $model.File)).Hash
    if ($outputHash -ne $model.OutputHash) { throw "Converted SHA-256 mismatch for $($model.File): actual=$outputHash" }
}

$copies = @(
    @{ Root=$sourceEffect; File='wind1.OZT'; Hash='098E001E87FE37CDCD2D29D96E806F7009395E338ADA2294EF0590B4F9C277AC' },
    @{ Root=$sourceEffect; File='alpha_line03.OZT'; Hash='0FC49D015E42754A5A0B809B6C08413B4941877BC4668C4065BE93BD34562306' },
    @{ Root=$sourceEffect; File='Thunder01.OZJ'; Hash='9A7A117E3E4D4D8A6500F614C933983D80F350305B9BC8C71FCCF7E81E6E4363' },
    @{ Root=$sourceEffect; File='clud64.OZJ'; Hash='64C359CE760E517B1D5201DF10AE3A0EFFCA4F08A3690CAB6FEEF1C34FFE379D' },
    @{ Root=$sourceEffect; File='Shockwave2.OZJ'; Hash='F47DC1F04D0DDCA6DC50E8EA09D44F430EB4B80D2B2C52D44BEC3BC8D846FEA2' },
    @{ Root=$sourceSound; File='MagicPin.wav'; Hash='06B2C74BBDA737DC06FB3EE4C65FE0C1C07B3F5343056CDEBFD16AC95BBA3B72' },
    @{ Root=$sourceSound; File='MagicPinExplode.wav'; Hash='F119E7BB0DB96E84D406D945314C70D598A56CDAA0FE5D7E26CE4414D761D17A' }
)
foreach ($copy in $copies) {
    Assert-CopyPinnedAsset (Join-Path $copy.Root $copy.File) $copy.Hash (Join-Path $resolvedOutput $copy.File)
}

Write-Output 'Status=PASS'
Get-ChildItem -LiteralPath $resolvedOutput -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    Write-Output ("Asset={0}|Size={1}|Sha256={2}" -f $_.Name, $_.Length, $hash)
}
