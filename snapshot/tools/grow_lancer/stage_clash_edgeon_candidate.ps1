param([switch]$Apply)
$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-clash-edgeon-E43AA160.exe'
$oldHash = 'E43AA160439904123DDFECE1BFEF9F7A83093CF8A68317B33446B7FE46432085'
$newHash = '06CBE3D64304E0D43810BC448BC7E1611DB553DD7D6D40D582CC012B038E1FE5'
foreach ($path in @($candidate, $staged, $backup)) {
    $resolved = [IO.Path]::GetFullPath($path)
    if (!$resolved.StartsWith($qaRoot + '\', [StringComparison]::OrdinalIgnoreCase)) { throw "Unsafe path: $resolved" }
}
foreach ($path in @($candidate, $staged)) {
    if ((Get-Item -LiteralPath $path -ErrorAction Stop).Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Reparse executable: $path" }
}
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count -ne 0) { throw 'QA client is running' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $candidate).Hash -ne $newHash) { throw 'Candidate hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $oldHash) { throw 'Staged hash mismatch' }
if (Test-Path -LiteralPath $backup) {
    if ((Get-Item -LiteralPath $backup).Attributes -band [IO.FileAttributes]::ReparsePoint -or
        (Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) { throw 'Backup conflict' }
}
Write-Output "PASS dry-run staged=$oldHash candidate=$newHash backup=$backup"
if (!$Apply) { return }
if (!(Test-Path -LiteralPath $backup)) { Copy-Item -LiteralPath $staged -Destination $backup }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) { throw 'Backup hash mismatch' }
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count -ne 0) { throw 'QA client started' }
Copy-Item -LiteralPath $candidate -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash) {
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'Stage hash mismatch; prior QA restored'
}
Write-Output "PASS isolated Clash edge-on diagnostic staged=$newHash"
