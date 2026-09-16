param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-clash-fixture-3E9DFCD6.exe'
$oldHash = '3E9DFCD6DF9BCC8E0B68C83F320B7A73C7889CAE4E6CBD94F94C52363EB942B0'
$newHash = '9F150003CC9ABDF49CBAAEDD69CFF4557487B5A5FC664190F13E0444FB0A7591'

foreach ($path in @($candidate, $staged, $backup)) {
    $absolute = [IO.Path]::GetFullPath($path)
    if (!$absolute.StartsWith($qaRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing path outside isolated RuntimeQA: $absolute"
    }
}
foreach ($path in @($candidate, $staged)) {
    $item = Get-Item -LiteralPath $path -ErrorAction Stop
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) {
        throw "Refusing reparse executable: $path"
    }
}
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count -ne 0) {
    throw 'Isolated QA executable is running; stage refused'
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $candidate).Hash -ne $newHash) {
    throw 'Candidate hash mismatch'
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $oldHash) {
    throw 'Staged executable hash mismatch'
}
if (Test-Path -LiteralPath $backup) {
    $item = Get-Item -LiteralPath $backup -Force
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) {
        throw 'Existing rollback backup conflict'
    }
}
Write-Output "PASS dry-run: staged=$oldHash candidate=$newHash rollback=$backup"
if (!$Apply) { return }

if (!(Test-Path -LiteralPath $backup)) {
    Copy-Item -LiteralPath $staged -Destination $backup
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) {
    throw 'Rollback backup hash mismatch'
}
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count -ne 0) {
    throw 'Isolated QA executable started during preflight; stage refused'
}
Copy-Item -LiteralPath $candidate -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash) {
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'Candidate stage hash mismatch; prior QA executable restored'
}
Write-Output "PASS staged private Clash fixture $newHash; rollback $backup"
Write-Output 'No client was launched and no production executable was modified.'
