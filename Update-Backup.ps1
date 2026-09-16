param(
    [Parameter(Mandatory = $true)][string]$SourceRoot,
    [switch]$DryRun
)

$ErrorActionPreference = 'Stop'
$sourceRoot = [IO.Path]::GetFullPath($SourceRoot).TrimEnd([IO.Path]::DirectorySeparatorChar)
$backupRoot = [IO.Path]::GetFullPath($PSScriptRoot).TrimEnd([IO.Path]::DirectorySeparatorChar)
$sourcePrefix = $sourceRoot + [IO.Path]::DirectorySeparatorChar
$snapshotRoot = [IO.Path]::GetFullPath((Join-Path $backupRoot 'snapshot'))
$snapshotPrefix = $snapshotRoot + [IO.Path]::DirectorySeparatorChar

if (-not (Test-Path -LiteralPath (Join-Path $sourceRoot 'GROW_LANCER_STATE.md'))) {
    throw 'Source is not the isolated Grow Lancer checkout.'
}
if ((& git -C $sourceRoot branch --show-current) -ne 'feature/grow-lancer-skills-s21') {
    throw 'Unexpected Grow Lancer source branch.'
}
if ((& git -C $backupRoot branch --show-current) -ne 'backup/grow-lancer-s21-20260915') {
    throw 'Unexpected SkillRise backup branch.'
}
if (& git -C $backupRoot status --porcelain) {
    $unexpected = @(& git -C $backupRoot status --porcelain | Where-Object { $_ -notmatch '^\?\? Update-Backup\.ps1$' })
    if ($unexpected.Count) { throw 'Backup worktree has unrelated changes; refusing snapshot update.' }
}

$manifestPath = Join-Path $backupRoot 'BACKUP_MANIFEST.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$allowed = @('.cpp','.c','.h','.hpp','.md','.py','.ps1','.cmd','.sql','.json',
    '.vcxproj','.filters','.rc','.txt','.csv')
$paths = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
foreach ($entry in $manifest.files) {
    if (-not $entry.path.StartsWith('snapshot/')) { throw "Bad manifest path: $($entry.path)" }
    [void]$paths.Add($entry.path.Substring(9))
}

foreach ($path in @(& git -C $sourceRoot -c core.quotePath=false diff --name-only)) {
    if ($path) { [void]$paths.Add($path.Replace('\','/')) }
}
foreach ($path in @('GROW_LANCER_STATE.md','GROW_LANCER_RECOVERY_STATE.md')) {
    if (Test-Path -LiteralPath (Join-Path $sourceRoot $path)) { [void]$paths.Add($path) }
}

$scoped = @(
    @{ root='GrowLancer'; extensions=@('.md','.h','.json'); pattern='*'; recurse=$true },
    @{ root='tools/grow_lancer'; extensions=@('.py','.ps1','.cpp','.cmd','.sql'); pattern='*'; recurse=$true },
    @{ root='ExGameServer/GameServer/RISE'; extensions=@('.cpp','.h'); pattern='GrowLancer*'; recurse=$false },
    @{ root='ExMain_RISE_PC/Main5.2_RISE/RISE'; extensions=@('.cpp','.h'); pattern='GrowLancer*'; recurse=$false },
    @{ root='ExGameServer/Tests'; extensions=@('.cpp','.cmd'); pattern='*GrowLancer*'; recurse=$false },
    @{ root='ExMain_RISE_PC/Tests'; extensions=@('.cpp'); pattern='GrowLancer*'; recurse=$false },
    @{ root='Shared'; extensions=@('.h'); pattern='*GrowLancer*'; recurse=$false }
)
foreach ($rule in $scoped) {
    $directory = Join-Path $sourceRoot $rule.root
    if (-not (Test-Path -LiteralPath $directory)) { continue }
    $items = if ($rule.recurse) {
        Get-ChildItem -LiteralPath $directory -Recurse -File
    } else {
        Get-ChildItem -LiteralPath $directory -File
    }
    foreach ($item in $items) {
        if ($item.Name -like $rule.pattern -and $item.Extension -in $rule.extensions) {
            $relative = $item.FullName.Substring($sourcePrefix.Length).Replace('\','/')
            [void]$paths.Add($relative)
        }
    }
}
if (Test-Path -LiteralPath (Join-Path $sourceRoot 'Shared/LegacySkillClassColumns.h')) {
    [void]$paths.Add('Shared/LegacySkillClassColumns.h')
}

$changed = 0
$new = 0
$missing = 0
foreach ($relative in @($paths | Sort-Object)) {
    if ($relative.Contains('..') -or $relative.StartsWith('/') -or
        [IO.Path]::GetExtension($relative) -notin $allowed) {
        throw "Out-of-scope snapshot path: $relative"
    }
    $source = [IO.Path]::GetFullPath((Join-Path $sourceRoot $relative))
    $destination = [IO.Path]::GetFullPath((Join-Path $snapshotRoot $relative))
    if (-not $source.StartsWith($sourcePrefix,[StringComparison]::OrdinalIgnoreCase) -or
        -not $destination.StartsWith($snapshotPrefix,[StringComparison]::OrdinalIgnoreCase)) {
        throw "Path escaped expected roots: $relative"
    }
    if (-not (Test-Path -LiteralPath $source)) {
        if (-not (Test-Path -LiteralPath $destination)) { throw "Missing source and backup: $relative" }
        $missing++
        continue
    }
    $item = Get-Item -LiteralPath $source
    if ($item.Length -gt 2MB) { throw "Oversized source-only candidate: $relative" }
    $bytes = [IO.File]::ReadAllBytes($source)
    if ([Array]::IndexOf($bytes,[byte]0) -ge 0) {
        throw "NUL byte in source-only candidate: $relative"
    }
    $sourceHash = (Get-FileHash -LiteralPath $source -Algorithm SHA256).Hash
    $destinationHash = if (Test-Path -LiteralPath $destination) {
        (Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash
    } else { '' }
    if ($sourceHash -eq $destinationHash) { continue }
    if ($destinationHash) { $changed++ } else { $new++ }
    if (-not $DryRun) {
        $parent = Split-Path -Parent $destination
        if (-not (Test-Path -LiteralPath $parent)) {
            [void](New-Item -ItemType Directory -Path $parent -Force)
        }
        Copy-Item -LiteralPath $source -Destination $destination -Force
        if ((Get-FileHash -LiteralPath $destination -Algorithm SHA256).Hash -ne $sourceHash) {
            throw "Copy/hash mismatch: $relative"
        }
    }
}
Write-Output "PLAN source-only paths=$($paths.Count) changed=$changed new=$new old-only=$missing dryRun=$DryRun"
if ($DryRun) { return }

$entries = @()
foreach ($item in @(Get-ChildItem -LiteralPath $snapshotRoot -Recurse -File | Sort-Object FullName)) {
    $relative = 'snapshot/' + $item.FullName.Substring($snapshotPrefix.Length).Replace('\','/')
    $entries += [pscustomobject]@{
        path = $relative
        bytes = $item.Length
        sha256 = (Get-FileHash -LiteralPath $item.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    }
}
$manifest.files = $entries
$manifest.scope = 'Grow Lancer changed source, descriptors, docs, focused tests and tools only; not a standalone RISE checkout.'
$manifest.omitted = 'Git history; S21 memory dumps; EXE/DLL/build output; client/data assets; production config; runtime captures.'
$updated = (Get-Date).ToUniversalTime().ToString('yyyy-MM-ddTHH:mm:ssZ')
$manifest | Add-Member -NotePropertyName backupUpdatedAtUtc -NotePropertyValue $updated -Force
$json = $manifest | ConvertTo-Json -Depth 12
[IO.File]::WriteAllText($manifestPath,$json + "`n",[Text.UTF8Encoding]::new($false))
& (Join-Path $backupRoot 'Verify-Backup.ps1')
if ($LASTEXITCODE) { throw 'Backup verifier failed.' }
