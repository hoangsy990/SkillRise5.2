param([string]$OutputRoot="D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\Breche")
$ErrorActionPreference='Stop'
$worktreeRoot=[IO.Path]::GetFullPath('D:\RISE-CrossPlatform\Source\_PC_GrowLancer');$resolvedOutput=[IO.Path]::GetFullPath($OutputRoot)
if(-not $resolvedOutput.StartsWith($worktreeRoot+[IO.Path]::DirectorySeparatorChar,[StringComparison]::OrdinalIgnoreCase)){throw "OutputRoot must remain below $worktreeRoot"}
$sourceItem='D:\MU FICA Season 21\Data\Item\texture';$sourceSound='D:\MU FICA Season 21\Data\Sound'
New-Item -ItemType Directory -Force -Path $resolvedOutput|Out-Null
function Copy-Pinned([string]$s,[string]$h,[string]$d){if((Get-FileHash -Algorithm SHA256 -LiteralPath $s).Hash-ne$h){throw "Source hash mismatch: $s"};Copy-Item -LiteralPath $s -Destination $d -Force;if((Get-FileHash -Algorithm SHA256 -LiteralPath $d).Hash-ne$h){throw "Staged hash mismatch: $d"}}
$copies=@(
 @{R=$sourceItem;F='Item1130_L_red.OZJ';H='5F74B13D908B80D16B4FDBB9C045701C557B7F1E74E664F9A9E3A95023CA6494'},
 @{R=$sourceSound;F='Breche.wav';H='1880D944A0B2400A84E2E4D4FFE9CF1FC0B69E6701379DD239BB6A20B18398C0'},
 # Recovered receive-root children, not the retracted equipment layer above.
 @{R='D:\MU FICA Season 21\Data\Effect';F='lightmarks_red.OZJ';H='E447C3F15664B277A2278360317C08C86CCF8921DA1746FE0084E012A5D7608E'},
 @{R='D:\MU FICA Season 21\Data\Skill';F='twlighthik02.OZJ';H='9872B87E5A87A965B61A3865F217A5973C39FB58D54E12A7858F560FD9C04B69'},
 @{R='D:\MU FICA Season 21\Data\Skill';F='twlighthik01.OZJ';H='6BE0AE4DC839CD40C6B63AA1CC74847E33326CE441194714440EA93FB1DC7A8E'}
)
foreach($x in $copies){Copy-Pinned (Join-Path $x.R $x.F) $x.H (Join-Path $resolvedOutput $x.F)}
'Status=PASS';Get-ChildItem -LiteralPath $resolvedOutput -File|Sort-Object Name|%{$h=(Get-FileHash -Algorithm SHA256 -LiteralPath $_.FullName).Hash;"Asset=$($_.Name)|Size=$($_.Length)|Sha256=$h"}
