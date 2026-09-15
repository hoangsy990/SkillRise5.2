param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$worktree = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$source = Join-Path $worktree 'ExMain_RISE_PC\Main5.2_RISE\CharacterManager.cpp'
$backup = Join-Path $worktree 'ExMain_RISE_PC\Tests\GrowLancerBuild\EncodingBackup\CharacterManager.cpp.cp949'
$expected = '0C6D473AA862C3FE77ED9D0474AB81B9B0B917A5CD769FFA0B05A996010DF76B'
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash -ne $expected) {
    throw 'CharacterManager source changed; encoding conversion refused'
}
if (Test-Path -LiteralPath $backup) { throw 'Encoding backup already exists; refusing overwrite' }
if ((Get-Item -LiteralPath $source).Attributes -band [IO.FileAttributes]::ReparsePoint) {
    throw 'Source is reparse point; refused'
}
$bytes = [IO.File]::ReadAllBytes($source)
$cp949 = [Text.Encoding]::GetEncoding(949,
    [Text.EncoderFallback]::ExceptionFallback,
    [Text.DecoderFallback]::ExceptionFallback)
$content = $cp949.GetString($bytes)
$again = $cp949.GetBytes($content)
if ([Convert]::ToBase64String($bytes) -cne [Convert]::ToBase64String($again)) {
    throw 'CP949 source is not byte-roundtrip-safe'
}
$lines = $content -split "`n"
$nonAscii = @($lines | Where-Object { $_ -match '[^\x00-\x7F]' })
if ($nonAscii.Count -ne 2 -or $nonAscii[0] -notmatch '^\s*return GlobalText\[3150\];\s*//' -or
    $nonAscii[1] -notmatch '^\s*// 3151 ') {
    throw 'Unexpected non-ASCII code or literal; refused'
}
Write-Output "PASS dry-run: original=$expected nonASCIIComments=2 backup=$backup"
if (!$Apply) { return }
New-Item -ItemType Directory -Path (Split-Path -Parent $backup) -Force | Out-Null
Copy-Item -LiteralPath $source -Destination $backup
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $expected) {
    throw 'Encoding backup hash mismatch; source untouched'
}
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText($source,$content,$utf8)
if ([IO.File]::ReadAllText($source,$utf8) -cne $content) {
    throw 'UTF-8 content roundtrip mismatch; backup retained'
}
Write-Output 'PASS: CP949 comments normalized to UTF-8; code text preserved; original backup retained'
