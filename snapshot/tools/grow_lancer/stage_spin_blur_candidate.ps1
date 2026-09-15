param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-spin-blur-F0233138.exe'
$oldHash = 'F023313844C3192B26D093942E5A6DD4003D7AC7CFF91E2A779EB3371C97C0E7'
$newHash = 'D36C749B3F4BE6418F35CF6B08049F778926AA4CD5E1CE1A10AFA1FA9669B9E7'

foreach ($path in @($candidate, $staged, $backup)) {
    $absolute = [IO.Path]::GetFullPath($path)
    if (!$absolute.StartsWith($qaRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Refusing path outside isolated RuntimeQA: $absolute"
    }
}
foreach ($path in @($candidate, $staged)) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing executable: $path" }
    $item = Get-Item -LiteralPath $path -Force
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
    throw 'Accepted staged executable hash mismatch'
}
if (Test-Path -LiteralPath $backup) {
    $item = Get-Item -LiteralPath $backup -Force
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) {
        throw 'Existing rollback backup conflict'
    }
}
Write-Output "PASS dry-run: accepted=$oldHash candidate=$newHash rollback=$backup"
if (!$Apply) { return }

if (!(Test-Path -LiteralPath $backup)) {
    Copy-Item -LiteralPath $staged -Destination $backup
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) {
    throw 'Rollback backup hash mismatch'
}
Copy-Item -LiteralPath $candidate -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash) {
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'New candidate stage hash mismatch; accepted executable restored'
}
Write-Output "PASS staged private QA candidate $newHash; accepted rollback $backup"
Write-Output 'No client was launched and no production executable was modified.'
