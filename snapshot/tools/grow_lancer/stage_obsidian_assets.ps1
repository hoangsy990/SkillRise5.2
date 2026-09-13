param(
    [string]$OutputRoot = "D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Obsidian"
)

$ErrorActionPreference = 'Stop'
$worktreeRoot = [IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer')
$resolvedOutput = [IO.Path]::GetFullPath($OutputRoot)
if (-not $resolvedOutput.StartsWith($worktreeRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase)) { throw "OutputRoot must remain below $worktreeRoot" }
$converter = Join-Path $worktreeRoot 'tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py'
$sourceEffect = 'D:\MU FICA Season 21\Data\Effect'
$sourceSound = 'D:\MU FICA Season 21\Data\Sound'
New-Item -ItemType Directory -Force -Path $resolvedOutput | Out-Null
$env:PYTHONIOENCODING = 'utf-8'

function Assert-CopyPinnedAsset([string]$Source,[string]$ExpectedSha256,[string]$Destination) {
    $actual=(Get-FileHash -Algorithm SHA256 -LiteralPath $Source).Hash
    if($actual -ne $ExpectedSha256){throw "Source SHA-256 mismatch for ${Source}: expected=$ExpectedSha256 actual=$actual"}
    Copy-Item -LiteralPath $Source -Destination $Destination -Force
    $copied=(Get-FileHash -Algorithm SHA256 -LiteralPath $Destination).Hash
    if($copied -ne $ExpectedSha256){throw "Staged SHA-256 mismatch for ${Destination}: expected=$ExpectedSha256 actual=$copied"}
}

$models=@(
    @{File='obsidiana.bmd';SourceHash='CEA8E36B26C01DF4CA5CF1D177DF4E5BCA9D49756538082F0FB9B5DB44D69C16';Name='obsidiana.SMD';OutputHash='DF4D76E36ED05D9320224AB52B43D250B5E87EBBFCE1F08F618463BBCDF09D1A'},
    @{File='obsidian.bmd';SourceHash='0F779291DEBEF892E2FED99399769CEFF4F8CA48D5D9E86E25F2A77546AA8B94';Name='obsidian.SMD';OutputHash='1CB0C7FB5DA30FE6D7A41635517146E750BA44574807281BB36DAAA68BFC2B4F'}
)
foreach($model in $models){
    & py -3 $converter (Join-Path $sourceEffect $model.File) (Join-Path $resolvedOutput $model.File) --expected-sha256 $model.SourceHash --expected-name $model.Name
    if($LASTEXITCODE -ne 0){throw "$($model.File) conversion failed"}
    $actual=(Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput $model.File)).Hash
    if($actual -ne $model.OutputHash){throw "Converted SHA-256 mismatch for $($model.File): actual=$actual"}
}

$copies=@(
    @{Root=$sourceEffect;File='JointThunder01.OZJ';Hash='CE2FE50F1B29DF434E8C3B2518C53A5528BE2642BACB4E227BF68741215E7F90'},
    @{Root=$sourceEffect;File='alpha_motion.OZT';Hash='2708C3B1C633A8F472934342BF0CC2A4878F8BB4061D84B74B6065ADE2A78659'},
    @{Root=$sourceEffect;File='line_fire.OZJ';Hash='83B829FC5322CC5FEB4CC593A1756F95A53926540322222E843467C803A59DDF'},
    @{Root=$sourceEffect;File='flare_mono.OZJ';Hash='EAB40E848BCCD356C9A3A92BC4BFE66B148FD64A76E00168CF8B68E3A19300F5'},
    @{Root=$sourceEffect;File='Thunder01.OZJ';Hash='9A7A117E3E4D4D8A6500F614C933983D80F350305B9BC8C71FCCF7E81E6E4363'},
    @{Root=$sourceSound;File='Obsidian.wav';Hash='768965D9B58FAAE7F5DD5774A4FBFB018265996B8983E227D7EFA1E14646EE47'}
)
foreach($copy in $copies){Assert-CopyPinnedAsset (Join-Path $copy.Root $copy.File) $copy.Hash (Join-Path $resolvedOutput $copy.File)}
Write-Output 'Status=PASS'
Get-ChildItem -LiteralPath $resolvedOutput -File|Sort-Object Name|%{$h=(Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash;"Asset=$($_.Name)|Size=$($_.Length)|Sha256=$h"}
