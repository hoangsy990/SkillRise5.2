$ErrorActionPreference = 'Stop'
$manifest = Get-Content -LiteralPath (Join-Path $PSScriptRoot 'BACKUP_MANIFEST.json') -Raw | ConvertFrom-Json
$snapshotRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'snapshot')) + [IO.Path]::DirectorySeparatorChar
$checked = 0
foreach ($entry in $manifest.files) {
    $filePath = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot $entry.path))
    if (-not $filePath.StartsWith($snapshotRoot, [StringComparison]::OrdinalIgnoreCase)) {
        throw "Path outside snapshot: $($entry.path)"
    }
    $item = Get-Item -LiteralPath $filePath
    if ($item.Length -ne $entry.bytes) { throw "Size mismatch: $($entry.path)" }
    $hash = (Get-FileHash -LiteralPath $filePath -Algorithm SHA256).Hash
    if ($hash -ne $entry.sha256) { throw "SHA256 mismatch: $($entry.path)" }
    $checked++
}
$actual = @(Get-ChildItem -LiteralPath $snapshotRoot -Recurse -File)
if ($actual.Count -ne $checked) { throw 'Unexpected files in snapshot' }
Write-Output "PASS: $checked snapshot files match manifest sizes and SHA-256."
