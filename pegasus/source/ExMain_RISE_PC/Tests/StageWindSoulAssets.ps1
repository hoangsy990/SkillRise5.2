$ErrorActionPreference = 'Stop'

$worktreeRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$manifestPath = Join-Path $worktreeRoot 'WindSoul\asset_provenance.json'
$manifest = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
$destinationRoot = [System.IO.Path]::GetFullPath(
    (Join-Path $worktreeRoot $manifest.destinationRoot))
$allowedRelative = 'ExMain_RISE_PC\Tests\PegasusBuild\Client\Data\RISE\WindSoulBase737'
$allowedDestination = [System.IO.Path]::GetFullPath(
    (Join-Path $worktreeRoot $allowedRelative))
if ($destinationRoot -ne $allowedDestination) {
    throw "Destination escaped isolated Wind Soul staging root: $destinationRoot"
}

$staged = 0
foreach ($asset in $manifest.assets) {
    $source = [System.IO.Path]::GetFullPath($asset.source)
    $destination = [System.IO.Path]::GetFullPath(
        (Join-Path $destinationRoot $asset.destination))
    if (-not $destination.StartsWith($destinationRoot,
        [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Destination escaped isolated Wind Soul root: $destination"
    }
    $sourceItem = Get-Item -LiteralPath $source
    $sourceHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash
    if ($sourceItem.Length -ne [long]$asset.length -or
        $sourceHash -ne $asset.sha256) {
        throw "Pinned source mismatch: $source"
    }
    $parent = Split-Path -Parent $destination
    New-Item -ItemType Directory -Path $parent -Force | Out-Null
    Copy-Item -LiteralPath $source -Destination $destination -Force
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $destination).Hash -ne
        $asset.sha256) {
        throw "Staged hash mismatch: $destination"
    }
    $staged++
}

[pscustomobject]@{ Status = 'PASS'; Files = $staged; Destination = $destinationRoot }
