$ErrorActionPreference = 'Stop'
$root = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$qa = Join-Path $root 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Client'
$bin = Join-Path $root 'ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA\Bin'
$new = Join-Path $bin 'Engine-Port S21.exe'
$target = Join-Path $qa 'Engine-Port S21.exe'
$oldHash = '6F879C6EBF68E11CA79642E373F425C6F4AF72E3A524216E2976F603F008B2F3'
$newHash = 'EF1595C1C8A3CF3638C4B3C37043D8B6854BDE041F97256A2C50999152E5FB0D'
foreach ($path in @($qa,$bin,(Join-Path $qa 'Data'),(Join-Path $qa 'Data\RISE'),(Join-Path $qa 'Data\RISE\GrowLancer'),(Join-Path $qa 'Data\RISE\GrowLancer\Wrath'),$target)) {
    $item = Get-Item -LiteralPath $path
    if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) { throw "Reparse target: $path" }
}
if (Get-CimInstance Win32_Process | Where-Object { $_.ExecutablePath -and $_.ExecutablePath.Equals($target,[StringComparison]::OrdinalIgnoreCase) }) { throw 'QA executable is running' }
if ((Get-FileHash -LiteralPath $new).Hash -ne $newHash) { throw 'Candidate changed' }
$current = (Get-FileHash -LiteralPath $target).Hash
if ($current -ne $oldHash -and $current -ne $newHash) { throw 'Unexpected staged executable' }
$backup = Join-Path $bin 'Engine-Port S21.before-spin-outcome-6F879C.exe'
if (Test-Path -LiteralPath $backup) {
    if ((Get-FileHash -LiteralPath $backup).Hash -ne $oldHash) { throw 'Backup conflict' }
} elseif ($current -eq $oldHash) {
    Copy-Item -LiteralPath $target -Destination $backup
} else { throw 'Missing rollback backup' }
if ((Get-FileHash -LiteralPath $backup).Hash -ne $oldHash) { throw 'Backup hash failed' }
$source = Join-Path $root 'Client\Data\RISE\GrowLancer\Wrath'
$files = @('firehik_mono01.OZJ','firehik_mono02.OZJ','firehik_mono03.OZJ','flare01.OZJ','flareBlue.OZJ','lighting_mega01.OZJ','lighting_mega02.OZJ','lighting_mega03.OZJ','lightmarks.OZJ','Shockwave2.OZJ','WrathBuffAtlas.OZT')
foreach ($name in $files) {
    $src = Join-Path $source $name
    $dst = Join-Path $qa "Data\RISE\GrowLancer\Wrath\$name"
    if (!(Test-Path -LiteralPath $dst)) { Copy-Item -LiteralPath $src -Destination $dst }
    if ((Get-FileHash -LiteralPath $src).Hash -ne (Get-FileHash -LiteralPath $dst).Hash) { throw "Asset conflict: $name" }
}
Copy-Item -LiteralPath $new -Destination $target
if ((Get-FileHash -LiteralPath $target).Hash -ne $newHash) { throw 'Staged hash failed' }
Write-Output "PASS staged $newHash; rollback $backup; no client launched"
