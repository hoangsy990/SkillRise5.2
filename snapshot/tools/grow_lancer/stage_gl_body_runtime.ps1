param([switch]$Apply)
$ErrorActionPreference = 'Stop'
$taskRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$buildRoot = Join-Path $taskRoot 'ExMain_RISE_PC\Tests\GrowLancerBuild'
$source = Join-Path $buildRoot 'BodyCandidates\Data\RISE\GrowLancer\ClassBody'
$destination = Join-Path $buildRoot 'RuntimeQA\Client\Data\RISE\GrowLancer\ClassBody'
$qaExe = Join-Path $buildRoot 'RuntimeQA\Client\Engine-Port S21.exe'
$files = @{
 'HelmClass08.bmd' = 'FBDDF9E7469DAC5898DF384566ABA2A3B9E0E1CAD08CC4DE8F774CF054786C23'
 'ArmorClass08.bmd' = '50DC2AC8EAE234C56612A03AC1199772F131CDF49CFF28562192AC2CBC515ED9'
 'PantClass08.bmd' = 'C7E2991E76F3DE618149C7E451A84BDCE5E550238A85DBE474DD6980A9DE1E78'
 'GloveClass08.bmd' = '71DFCD226842E6A7B87DD8C3BDCC344B315D33D7976B8C97F7C0A38CCED02F8C'
 'BootClass08.bmd' = 'EA08439D618E3AD6E8B565AA2C7C70BA666C53CF3888B903ECF513CD42582ABC'
 'hair_Class108.OZT' = 'E13026B066E69982CF8CDDC0F1A8D9558FDA0673B2A52B82F56F920614EA4A7B'
 'skin_Class108.OZJ' = '37D230AB5E35C76220B54DF28ABB52F619E1E86F9A4CB9A96C77B8C54C344A89'
}
function Check-Private([string]$path) {
 $cursor = [IO.Path]::GetFullPath($path)
 if (!$cursor.StartsWith($taskRoot + '\', [StringComparison]::OrdinalIgnoreCase)) { throw 'Outside worktree' }
 while ($cursor.Length -ge $taskRoot.Length) {
  if ((Test-Path -LiteralPath $cursor) -and ((Get-Item -LiteralPath $cursor).Attributes -band [IO.FileAttributes]::ReparsePoint)) { throw "Reparse: $cursor" }
  $cursor = [IO.Path]::GetDirectoryName($cursor)
 }
}
foreach ($name in $files.Keys) {
 $from = Join-Path $source $name; $to = Join-Path $destination $name
 Check-Private $from; Check-Private $to
 if ((Get-FileHash -LiteralPath $from).Hash -ne $files[$name]) { throw "Candidate changed: $name" }
 if ((Test-Path -LiteralPath $to) -and (Get-FileHash -LiteralPath $to).Hash -ne $files[$name]) { throw "Existing output differs: $name" }
}
if (!$Apply) { Write-Output 'DRY RUN: 7 hash-pinned base-body files; no writes'; return }
if (Get-CimInstance Win32_Process | Where-Object { $_.ExecutablePath -and $_.ExecutablePath.Equals($qaExe,[StringComparison]::OrdinalIgnoreCase) }) { throw 'QA client running; refusing stage' }
New-Item -ItemType Directory -Path $destination -Force | Out-Null
foreach ($name in $files.Keys) {
 $from = Join-Path $source $name; $to = Join-Path $destination $name
 Check-Private $to
 if ((Get-FileHash -LiteralPath $from).Hash -ne $files[$name]) { throw "Candidate changed: $name" }
 if (!(Test-Path -LiteralPath $to)) { Copy-Item -LiteralPath $from -Destination $to }
 if ((Get-FileHash -LiteralPath $to).Hash -ne $files[$name]) { throw "Output verification failed: $name" }
}
Write-Output 'PASS: 7 base-body files staged; existing identical files preserved; no class activation or launch'
