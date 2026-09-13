$ErrorActionPreference = 'Stop'

$buildRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot 'PegasusBuild')).Path
$clientRoot = (Resolve-Path -LiteralPath (Join-Path $buildRoot 'Client')).Path
$sourceExe = (Resolve-Path -LiteralPath (Join-Path $clientRoot 'Engine-Pegasus.exe')).Path
$skillIds = @(241, 242, 243, 288, 289, 1500, 1501, 2012, 2013, 2014, 2015, 2016, 2023, 2024)

foreach ($name in @('RuntimeClient', 'Delivery')) {
    $targetRoot = (Resolve-Path -LiteralPath (Join-Path $buildRoot $name)).Path
    if (-not $targetRoot.StartsWith($buildRoot, [System.StringComparison]::OrdinalIgnoreCase)) {
        throw "Target escaped isolated build root: $targetRoot"
    }

    $targetExe = Join-Path $targetRoot 'Engine-Pegasus.exe'
    Copy-Item -LiteralPath $sourceExe -Destination $targetExe -Force
    $obsoleteExe = Join-Path $targetRoot 'Engine.exe'
    if (Test-Path -LiteralPath $obsoleteExe) {
        Remove-Item -LiteralPath $obsoleteExe -Force
    }
    # StagePegasusAssets.ps1 hash-guards every recovered source file. Deploy
    # that complete isolated tree so newly proven child assets cannot be
    # present in the build output but silently absent from runtime QA.
    $pegasusDataSource = Join-Path $clientRoot 'Data\RISE\Pegasus'
    $pegasusDataTarget = Join-Path $targetRoot 'Data\RISE\Pegasus'
    New-Item -ItemType Directory -Path $pegasusDataTarget -Force | Out-Null
    Copy-Item -Path (Join-Path $pegasusDataSource '*') `
        -Destination $pegasusDataTarget -Recurse -Force
    $playerActionSource = Join-Path $clientRoot 'Data\RISE\Pegasus\Recovered\Data\Player\player.bmd'
    $playerActionTarget = Join-Path $targetRoot 'Data\RISE\Pegasus\Recovered\Data\Player\player.bmd'
    New-Item -ItemType Directory -Path ([IO.Path]::GetDirectoryName($playerActionTarget)) -Force | Out-Null
    Copy-Item -LiteralPath $playerActionSource -Destination $playerActionTarget -Force
    $targetIcons = Join-Path $targetRoot 'Data\RISE\Pegasus\UI\Skill'
    New-Item -ItemType Directory -Path $targetIcons -Force | Out-Null
    foreach ($skillId in $skillIds) {
        Copy-Item -LiteralPath (Join-Path $clientRoot "Data\RISE\Pegasus\UI\Skill\$skillId.OZJ") `
            -Destination (Join-Path $targetIcons "$skillId.OZJ") -Force
        foreach ($extension in @('jpg', 'tga')) {
            $obsolete = Join-Path $targetIcons "$skillId.$extension"
            if (Test-Path -LiteralPath $obsolete) {
                Remove-Item -LiteralPath $obsolete -Force
            }
        }
    }

    $hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $targetExe).Hash
    $files = Get-ChildItem -LiteralPath $targetIcons -File
    $ozjCount = @($files | Where-Object Extension -eq '.OZJ').Count
    $tgaCount = @($files | Where-Object Extension -eq '.tga').Count
    $jpgCount = @($files | Where-Object Extension -eq '.jpg').Count
    Write-Output "DEPLOY=$name|SHA256=$hash|OZJ=$ozjCount|JPG=$jpgCount|TGA=$tgaCount"
}
