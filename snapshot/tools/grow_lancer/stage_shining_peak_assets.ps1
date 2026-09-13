param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\ShiningPeak"
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
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    $copied = (Get-FileHash -Algorithm SHA256 -LiteralPath $Destination).Hash
    if ($copied -ne $ExpectedSha256) { throw "Staged SHA-256 mismatch for ${Destination}: expected=$ExpectedSha256 actual=$copied" }
}

$models = @(
    @{ File='shiningpeakbody.bmd'; SourceHash='3241DF1864E51E3CD134D4CC4AFD93BC33D39790C4026D5E20489677D7DB44DE'; Name='shiningpeakbody.SMD'; OutputHash='455BFCEF28E7163861BAACFA4AC9C41B1735793AA6ECAAA882BAD90F8AB44F50' },
    @{ File='shiningpeakpin.bmd'; SourceHash='B697B62BE57A8409AA1064C4DEAC2F2C849519463923FB68A5AE8B9454DA8A74'; Name='shiningpeakpin.SMD'; OutputHash='311C51B78C430190748164FF7CC803B5A92DCD13D81F1BB2B8C063A2251CA15A' },
    @{ File='shiningpeakspinmagic.bmd'; SourceHash='2C46BBD7ADBB65D382F81DBF6840CC8780A41F313D6090B1410FAF07D4F94561'; Name='shiningpeakspinmagic.SMD'; OutputHash='F284617B9B9A76130029A76A7779F07356BD5F6B28B4623BC320BC3889B6BA31' },
    @{ File='shockwave01.bmd'; SourceHash='9AFA42AFB08BA9A2D138B5AC961A515E73405F6C54B3C04FC7C7CDD1F8B5F579'; Name='D:\Project_Temp\KoreaProject\신'; OutputHash='27FDCD1DB5DA0D9EAAA5FEEC0EA73B0FBCBC7B957C9C04A7EF9C71D8173D65BE' }
)
foreach ($model in $models) {
    & py -3 $converter (Join-Path $sourceEffect $model.File) (Join-Path $resolvedOutput $model.File) --expected-sha256 $model.SourceHash --expected-name $model.Name
    if ($LASTEXITCODE -ne 0) { throw "$($model.File) conversion failed" }
    $outputHash = (Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput $model.File)).Hash
    if ($outputHash -ne $model.OutputHash) { throw "Converted SHA-256 mismatch for $($model.File): actual=$outputHash" }
}

$copies = @(
    @{ Root=$sourceEffect; File='shiningpeakbody.OZJ'; Hash='02CB2E283838BB1E61067A3A6552C645FF8C662BE24C6B4AC4D0541A0870742D' },
    @{ Root=$sourceEffect; File='motion_mono.OZJ'; Hash='DFD88E6D2E5BC42A7E38183942C0D2B800FDC9ECFD26395616D26D556C4FBBA9' },
    @{ Root=$sourceEffect; File='firehik_mono02.OZJ'; Hash='EA67813627C32A9F9EAE948FFFF2EE47A0FD44A49426CCE2879FCEF4A6B5F7D6' },
    @{ Root=$sourceEffect; File='marks_m03.OZJ'; Hash='AAB0CFA2A69CCEA87CB9B81341F134B08814648C0D2AE30FF193320D44173233' },
    @{ Root=$sourceEffect; File='pin_lights.OZJ'; Hash='A8C4DB1921178C7A4905F0398A744B49AB8D283DE36051584401566CE3FB2288' },
    @{ Root=$sourceEffect; File='flareBlue.OZJ'; Hash='E261B59F57FDF0C514917A388F02AC66B832C2BE9F97A3F90BC36C98D7B2FB32' },
    @{ Root=$sourceEffect; File='Damage1mono.OZJ'; Hash='A095432CB1BAA4FE21BBAB6D7A3E4422C8C114C1E9BC4B0AE563FBE7D1078D31' },
    @{ Root=$sourceEffect; File='xsuper0001.OZJ'; Hash='C6872526A7C5308F84F5A12BB7B0876CCE24249F04F5612418E303EB11CF16A2' },
    @{ Root=$sourceEffect; File='xsuper0002.OZJ'; Hash='F9F94343DC71465532B46AF231EA5F52217DB9D369F5E7FDB1F2313A7A52AA59' },
    @{ Root=$sourceEffect; File='xsuper0003.OZJ'; Hash='89BED8F4F34647CA7539F42115A8D64FDA30F04CBD97E19A05B64A84DB9C0815' },
    @{ Root=$sourceEffect; File='xsuper0004.OZJ'; Hash='616F034EBF30E3A5D201AAE001AEC56198F68B5F34A88F5B9B59DCF2C1441CC1' },
    @{ Root=$sourceEffect; File='xsuper0005.OZJ'; Hash='35A82BC5E12AF628DD5F5CC0BD722C089F4A33ACB0E022F599FB5C73B1A7D93D' },
    @{ Root=$sourceEffect; File='xsuper0006.OZJ'; Hash='21EE9A0480C230623E71B0D3E2002664564D300FFB234572E747C8CD8799FD75' },
    @{ Root=$sourceSound; File='ShiningPeak.wav'; Hash='4F1A2CB5AE4157B09FE50A163D70A73FDC118ADC50AFBE44283BFA9D6BDC0529' }
)
foreach ($copy in $copies) {
    Assert-CopyPinnedAsset (Join-Path $copy.Root $copy.File) $copy.Hash (Join-Path $resolvedOutput $copy.File)
}

Write-Output 'Status=PASS'
Get-ChildItem -LiteralPath $resolvedOutput -File | Sort-Object Name | ForEach-Object {
    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash
    Write-Output ("Asset={0}|Size={1}|Sha256={2}" -f $_.Name, $_.Length, $hash)
}
