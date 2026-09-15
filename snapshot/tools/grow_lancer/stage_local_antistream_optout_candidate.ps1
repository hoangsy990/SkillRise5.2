param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-local-antistream-239C4303.exe'
$oldHash = '239C430335E4AA0ED89239FF387963F10D3FFF9ACFE4D9FFA6DE15A23106DF88'
$newHash = 'D7B3032B199282B320644AAC32CDF42926543A2C4FFCBC15EB2A76A069434083'
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
Write-Output "PASS: isolated local AntiStream opt-out QA staged $newHash; rollback retained at $backup"
