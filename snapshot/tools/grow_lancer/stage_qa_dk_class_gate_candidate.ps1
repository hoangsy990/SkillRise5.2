param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-dk-gate-8F544F13.exe'
$oldHash = '8F544F139D46AD446427A7E0D3FC6322BB6C80CC191362572199D426BC625C67'
$newHash = 'AAE8FDE7EA235B6D5B4113B17674504FF1EA6784CFF96F4550362CA11A4C0DB0'
foreach ($path in @($candidate,$staged,$backup)) {
    $absolute = [IO.Path]::GetFullPath($path)
    if (!$absolute.StartsWith($qaRoot + '\',[StringComparison]::OrdinalIgnoreCase)) { throw 'QA executable path escaped worktree' }
    if (Test-Path -LiteralPath $path) {
        $item = Get-Item -LiteralPath $path -Force
        if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Refusing reparse executable: $path" }
    }
}
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count) { throw 'QA Engine is running; stage refused' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $candidate).Hash -ne $newHash) { throw 'Candidate hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $oldHash) { throw 'Staged hash mismatch' }
if (Test-Path -LiteralPath $backup) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) { throw 'Rollback backup hash conflict' }
}
Write-Output "PASS dry-run: staged=$oldHash candidate=$newHash"
if (!$Apply) { return }
if (!(Test-Path -LiteralPath $backup)) { Copy-Item -LiteralPath $staged -Destination $backup }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) { throw 'Rollback backup verification failed' }
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count) { throw 'QA Engine started during stage; refused' }
Copy-Item -LiteralPath $candidate -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash) {
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'QA stage verification failed; previous executable restored'
}
Write-Output "PASS: isolated QA DK gate staged $newHash; rollback retained at $backup"
