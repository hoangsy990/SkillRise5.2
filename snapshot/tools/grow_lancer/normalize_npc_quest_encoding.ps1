param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$worktree = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$source = Join-Path $worktree 'ExMain_RISE_PC\Main5.2_RISE\NewUINPCQuest.cpp'
$backup = Join-Path $worktree 'ExMain_RISE_PC\Tests\GrowLancerBuild\EncodingBackup\NewUINPCQuest.cpp.cp949'
$expected = '68424FE0E58558DF3B5559BB2009292492685431633971A8F52847938930A476'
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash -ne $expected) {
    throw 'NPC quest source changed; encoding conversion refused'
}
if (Test-Path -LiteralPath $backup) { throw 'NPC quest encoding backup already exists' }
if ((Get-Item -LiteralPath $source).Attributes -band [IO.FileAttributes]::ReparsePoint) {
    throw 'NPC quest source is reparse point'
}
$bytes = [IO.File]::ReadAllBytes($source)
$cp949 = [Text.Encoding]::GetEncoding(949,
    [Text.EncoderFallback]::ExceptionFallback,
    [Text.DecoderFallback]::ExceptionFallback)
$content = $cp949.GetString($bytes)
if ([Convert]::ToBase64String($bytes) -cne
    [Convert]::ToBase64String($cp949.GetBytes($content))) {
    throw 'NPC quest CP949 bytes do not roundtrip'
}
$nonAscii = @($content -split "`n" | Where-Object { $_ -match '[^\x00-\x7F]' })
if ($nonAscii.Count -ne 1 -or $nonAscii[0] -notmatch '^\s*// ') {
    throw 'Unexpected non-ASCII source or literal; refused'
}
Write-Output "PASS dry-run: original=$expected nonAsciiComments=1 backup=$backup"
if (!$Apply) { return }
New-Item -ItemType Directory -Path (Split-Path -Parent $backup) -Force | Out-Null
Copy-Item -LiteralPath $source -Destination $backup
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $expected) {
    throw 'NPC quest source backup hash mismatch'
}
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText($source, $content, $utf8)
if ([IO.File]::ReadAllText($source, $utf8) -cne $content) {
    throw 'NPC quest UTF-8 roundtrip mismatch; original backup retained'
}
Write-Output 'PASS NPC quest CP949 comment normalized to UTF-8; code bytes decoded without changes'
