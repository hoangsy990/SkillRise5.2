param([switch]$Apply)
$ErrorActionPreference = 'Stop'
$taskRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$buildRoot = Join-Path $taskRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild'
$source = Join-Path $buildRoot 'BodyCandidates\Data\RISE\GrowLancer\ClassBody'
$destination = Join-Path $buildRoot 'BodyRollbackFixture\Data\RISE\GrowLancer\ClassBody'
$pins = @{
 'HelmClass08.bmd' = 'FBDDF9E7469DAC5898DF384566ABA2A3B9E0E1CAD08CC4DE8F774CF054786C23'
 'hair_Class108.OZT' = 'E13026B066E69982CF8CDDC0F1A8D9558FDA0673B2A52B82F56F920614EA4A7B'
}
function Check-Private([string]$path) {
 $cursor = [IO.Path]::GetFullPath($path)
 if (!$cursor.StartsWith($taskRoot + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Outside worktree' }
 while ($cursor.Length -ge $taskRoot.Length) {
  if ((Test-Path -LiteralPath $cursor) -and ((Get-Item -LiteralPath $cursor).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw "Reparse: $cursor" }
  $cursor = [IO.Path]::GetDirectoryName($cursor)
 }
}
Check-Private $destination
if (Test-Path -LiteralPath $destination) {
 foreach ($entry in Get-ChildItem -LiteralPath $destination -Force) {
  if (!$pins.ContainsKey($entry.Name) -or $entry.PSIsContainer) { throw "Unexpected fixture entry: $($entry.Name)" }
 }
}
foreach ($name in $pins.Keys) {
 $from = Join-Path $source $name; $to = Join-Path $destination $name
 Check-Private $from; Check-Private $to
 if ((Get-FileHash -LiteralPath $from).Hash -ne $pins[$name]) { throw "Source changed: $name" }
 if ((Test-Path -LiteralPath $to) -and (Get-FileHash -LiteralPath $to).Hash -ne $pins[$name]) { throw "Output differs: $name" }
}
if (!$Apply) { Write-Output 'DRY RUN: helm + first material only; second material deliberately absent'; return }
New-Item -ItemType Directory -Path $destination -Force | Out-Null
foreach ($name in $pins.Keys) {
 $from = Join-Path $source $name; $to = Join-Path $destination $name
 Check-Private $to
 if (!(Test-Path -LiteralPath $to)) { Copy-Item -LiteralPath $from -Destination $to }
 if ((Get-FileHash -LiteralPath $to).Hash -ne $pins[$name]) { throw "Output verification failed: $name" }
}
Write-Output 'PASS: isolated two-file missing-second-material fixture; no runtime assets modified'
