param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-cs-receive-trace-D7B3032B.exe'
$oldHash = 'D7B3032B199282B320644AAC32CDF42926543A2C4FFCBC15EB2A76A069434083'
$newHash = 'E01EC1963C567CC11AF91D6BA9E42BB89ED3F1407161C5F94BF9A094B20795C1'
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
Write-Output "PASS: isolated CS receive trace staged $newHash; rollback retained at $backup"
