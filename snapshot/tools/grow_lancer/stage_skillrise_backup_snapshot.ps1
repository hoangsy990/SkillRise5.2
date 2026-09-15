param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$sourceRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$backupRoot = 'D:\RISE-CrossPlatform\GrowLancerSkillriseBackup20260915'
$snapshotRoot = Join-Path $backupRoot 'snapshot'
if (!(Test-Path -LiteralPath (Join-Path $sourceRoot 'GROW_LANCER_STATE.md') -PathType Leaf) -or
    !(Test-Path -LiteralPath (Join-Path $backupRoot '.git')) -or
    !(Test-Path -LiteralPath $snapshotRoot -PathType Container)) {
    throw 'Grow Lancer source or isolated backup worktree missing'
}
if ((& git -C $backupRoot branch --show-current) -ne 'backup/grow-lancer-s21-20260915') {
    throw 'Backup target is not the dedicated Grow Lancer branch'
}

$allowed = @('*.md','*.json','*.py','*.ps1','*.h','*.cpp','*.cmd',
    '*.sql','*.lua','*.csv','*.txt','*.yml','*.yaml')
$files = @('GROW_LANCER_STATE.md')
Push-Location -LiteralPath $sourceRoot
try {
    $docsAndTools = @(& rg --files 'GrowLancer' 'tools\grow_lancer' @($allowed | ForEach-Object { '-g'; $_ }))
    $clientPackage = @(& rg --files 'ExMain_RISE_PC\Main5.2_RISE\RISE' -g 'GrowLancer*.cpp' -g 'GrowLancer*.h')
    $serverPackage = @(& rg --files 'ExGameServer' -g '*GrowLancer*.cpp' -g '*GrowLancer*.h')
    if ($LASTEXITCODE -gt 1) { throw 'Scoped rg file discovery failed' }
    $files += $docsAndTools + $clientPackage + $serverPackage
} finally { Pop-Location }
$files = @($files | Where-Object { $_ -and $_ -notmatch '(^|[\\/])__pycache__([\\/]|$)' } |
    Sort-Object -Unique)
if ($files.Count -lt 300 -or $files.Count -gt 500) {
    throw "Unexpected Grow Lancer snapshot file count: $($files.Count)"
}

$changed = 0
$bytes = [long]0
foreach ($relative in $files) {
    $source = [IO.Path]::GetFullPath((Join-Path $sourceRoot $relative))
    $target = [IO.Path]::GetFullPath((Join-Path $snapshotRoot $relative))
    if (!$source.StartsWith($sourceRoot + '\', [StringComparison]::OrdinalIgnoreCase) -or
        !$target.StartsWith($snapshotRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw "Snapshot path escaped isolated roots: $relative"
    }
    $sourceItem = Get-Item -LiteralPath $source -Force
    if ($sourceItem.Attributes -band [IO.FileAttributes]::ReparsePoint) {
        throw "Refusing source reparse file: $relative"
    }
    if (Test-Path -LiteralPath $target) {
        $targetItem = Get-Item -LiteralPath $target -Force
        if ($targetItem.Attributes -band [IO.FileAttributes]::ReparsePoint) {
            throw "Refusing target reparse file: $relative"
        }
    }
    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash
    $targetHash = if (Test-Path -LiteralPath $target) {
        (Get-FileHash -Algorithm SHA256 -LiteralPath $target).Hash
    } else { '' }
    if ($sourceHash -eq $targetHash) { continue }
    ++$changed
    $bytes += $sourceItem.Length
    if ($Apply) {
        New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
        Copy-Item -LiteralPath $source -Destination $target -Force
        if ((Get-FileHash -Algorithm SHA256 -LiteralPath $target).Hash -ne $sourceHash) {
            throw "Snapshot post-copy hash mismatch: $relative"
        }
    }
}
Write-Output "PASS: scoped Grow Lancer files=$($files.Count) different=$changed bytes=$bytes applied=$([int][bool]$Apply)"
Write-Output 'Scope: state, descriptor/docs, GrowLancer-named native code and tests/tools; no Data, EXE, dump, equipment or quest files'
