$ErrorActionPreference = 'Stop'
$buildRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot 'PegasusBuild')).Path
$clientRoot = Join-Path $buildRoot 'Client'
$sourceRoot = Join-Path $clientRoot 'Data\RISE\Pegasus'
$sourceExe = Join-Path $clientRoot 'Engine-Pegasus.exe'
$exeHash = (Get-FileHash -LiteralPath $sourceExe -Algorithm SHA256).Hash
$sourceFiles = @(Get-ChildItem -LiteralPath $sourceRoot -Recurse -File)
if ($sourceFiles.Count -eq 0) { throw 'No staged Pegasus assets: cannot verify an empty deployment' }

# Read-only verification. Equal staging is necessary, not visual acceptance.
$expected = @{}
foreach ($file in $sourceFiles) {
    $relative = $file.FullName.Substring($sourceRoot.Length + 1)
    $expected[$relative] = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
}
foreach ($targetName in @('RuntimeClient', 'Delivery')) {
    $targetRoot = Join-Path $buildRoot $targetName
    if ((Get-FileHash -LiteralPath (Join-Path $targetRoot 'Engine-Pegasus.exe') -Algorithm SHA256).Hash -ne $exeHash) {
        throw "Stale executable in $targetName"
    }
    $assetRoot = Join-Path $targetRoot 'Data\RISE\Pegasus'
    foreach ($relative in $expected.Keys) {
        $target = Join-Path $assetRoot $relative
        if (-not (Test-Path -LiteralPath $target -PathType Leaf)) {
            throw "Missing asset in ${targetName}: $relative"
        }
        if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $expected[$relative]) {
            throw "Stale asset in ${targetName}: $relative"
        }
    }
    Write-Output "PASS: $targetName executable and $($expected.Count) Pegasus assets match build SHA256"
}
