param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$worktree = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$source = Join-Path $worktree 'ExMain_RISE_PC\Main5.2_RISE\NewUIMuHelper.cpp'
$backup = Join-Path $worktree 'ExMain_RISE_PC\Tests\GrowLancerBuild\EncodingBackup\NewUIMuHelper.cpp.original'
$expected = '24C54E600BAEFC0222F826A445ADA6E11BACFA77D44CF4BB58736002C57DD9B3'
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash -ne $expected) {
    throw 'MuHelper source changed; one-byte comment normalization refused'
}
if (Test-Path -LiteralPath $backup) { throw 'MuHelper source backup already exists' }
if ((Get-Item -LiteralPath $source).Attributes -band [IO.FileAttributes]::ReparsePoint) {
    throw 'MuHelper source reparse point refused'
}
$bytes = [IO.File]::ReadAllBytes($source)
$positions = @(0..($bytes.Length-1) | Where-Object { $bytes[$_] -ge 128 })
if ($positions.Count -ne 1 -or $positions[0] -ne 21470 -or $bytes[21470] -ne 0xEA) {
    throw 'Unexpected non-ASCII MuHelper bytes; refused'
}
$context = [Text.Encoding]::ASCII.GetString($bytes[21438..21500])
if ($context -notmatch '// th.m check null') { throw 'Non-ASCII byte is not the expected comment' }
Write-Output "PASS dry-run: original=$expected onlyHighByte=21470 commentOnly=1"
if (!$Apply) { return }
New-Item -ItemType Directory -Path (Split-Path -Parent $backup) -Force | Out-Null
Copy-Item -LiteralPath $source -Destination $backup
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $expected) {
    throw 'MuHelper original backup verification failed'
}
$bytes[21470] = [byte]0x65 # exact one-byte comment-only 'e' -> // them check null
[IO.File]::WriteAllBytes($source, $bytes)
if (@([IO.File]::ReadAllBytes($source) | Where-Object {$_ -ge 128}).Count -ne 0) {
    throw 'MuHelper comment normalization did not produce ASCII; original backup retained'
}
Write-Output 'PASS one malformed comment byte normalized; executable code bytes unchanged'
