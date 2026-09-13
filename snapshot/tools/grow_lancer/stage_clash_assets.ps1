param([string]$OutputRoot="D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Clash")
$ErrorActionPreference='Stop'
$worktreeRoot=[IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer');$resolvedOutput=[IO.Path]::GetFullPath($OutputRoot)
if(-not $resolvedOutput.StartsWith($worktreeRoot+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw "OutputRoot must remain below $worktreeRoot"}
$converter=Join-Path $worktreeRoot 'tools\grow_lancer\convert_s21_bmd_v0f_to_rise_v0c.py';$sourceEffect='D:\MU FICA Season 21\Data\Effect';$sourceSound='D:\MU FICA Season 21\Data\Sound'
New-Item -ItemType Directory -Force -Path $resolvedOutput|Out-Null;$env:PYTHONIOENCODING='utf-8'
function Copy-Pinned([string]$s,[string]$h,[string]$d){if((Get-FileHash -Algorithm SHA256 -LiteralPath $s).Hash-ne$h){throw "Source hash mismatch: $s"};Copy-Item -LiteralPath $s -Destination $d -Force;if((Get-FileHash -Algorithm SHA256 -LiteralPath $d).Hash-ne$h){throw "Staged hash mismatch: $d"}}
$models=@(
 @{File='crasha01.bmd';SourceHash='F951A42CA7DFAD14CAE8C50A1F55E11A166CD2AD8923A2366D40C91C62E073F6';Name='crasha01.SMD';OutputHash='10B3A1733D01046ECE7038DC7EA631DF28F674AD638D8D4938E9193D80020D23'},
 @{File='crasha02.bmd';SourceHash='A7124169FD10F98C3DBE97AD343494B8FDD47EB94A477C065E577EF89DC2CD75';Name='crasha02.SMD';OutputHash='315E549CBC13846EE2921CD9141EEAAC60A4C2810CD6B495978D92FD16D6C191'}
)
foreach($m in $models){& py -3 $converter (Join-Path $sourceEffect $m.File) (Join-Path $resolvedOutput $m.File) --expected-sha256 $m.SourceHash --expected-name $m.Name;if($LASTEXITCODE-ne 0){throw "$($m.File) conversion failed"};if((Get-FileHash -Algorithm SHA256 -LiteralPath (Join-Path $resolvedOutput $m.File)).Hash-ne$m.OutputHash){throw "$($m.File) output hash mismatch"}}
$copies=@(
 @{R=$sourceEffect;F='alpha_line2.OZJ';H='D2BBB29A580CE0B2ABFC69689691D4BADB102251250A1452DC7BD6A57D4CD69B'},
 @{R=$sourceEffect;F='wind1.OZJ';H='8D6F8BAA8408E33D23FB1F96218CB38980F671A644B2C55E42D8841A48171C4F'},
 @{R=$sourceSound;F='Crash.wav';H='1DAEB80C8316A8A888126E9FDCDCAC901DF61D7A0C6FE11FB2C00FBBCEE4DBC0'}
)
foreach($x in $copies){Copy-Pinned (Join-Path $x.R $x.F) $x.H (Join-Path $resolvedOutput $x.F)}
'Status=PASS';Get-ChildItem -LiteralPath $resolvedOutput -File|Sort-Object Name|%{$h=(Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash;"Asset=$($_.Name)|Size=$($_.Length)|Sha256=$h"}

