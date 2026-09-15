param([switch]$Apply)

$ErrorActionPreference = 'Stop'
$qaRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\RuntimeQA'
$candidate = Join-Path $qaRoot 'Bin\Engine-Port S21.exe'
$staged = Join-Path $qaRoot 'Client\Engine-Port S21.exe'
$backup = Join-Path $qaRoot 'Bin\Engine-Port S21.before-spin-cross-gameplay-query-73602038.exe'
$assetSource = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\CircleShield\firehik_mono01.OZJ'
$assetTarget = Join-Path $qaRoot 'Client\Data\RISE\GrowLancer\CircleShield\firehik_mono01.OZJ'
$spinAssetSource = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExMain_RISE_PC\Tests\GrowLancerBuild\Client\Data\RISE\GrowLancer\SpinStep\motion_blur.OZJ'
$spinAssetTarget = Join-Path $qaRoot 'Client\Data\RISE\GrowLancer\SpinStep\motion_blur.OZJ'
$masterAssetSource = 'D:\MU FICA Season 21\Data\Interface\new_Master_Icon.OZJ'
$masterAssetTarget = Join-Path $qaRoot 'Client\Data\RISE\GrowLancer\Master\new_Master_Icon.OZJ'
$masterDisabledSource = 'D:\MU FICA Season 21\Data\Interface\new_Master_non_Icon.OZJ'
$masterDisabledTarget = Join-Path $qaRoot 'Client\Data\RISE\GrowLancer\Master\new_Master_non_Icon.OZJ'
$oldHash = '73602038BDAC5255BA6951E5B106225CBA26E588E4E3980CC7BB6F2F8A0C4C5B'
$newHash = '95736241063643B58F8ECE167A2B9CFBADB6EAB1FCDBE59CBF06F2CBEDC14643'
$assetHash = 'C3E18C474BA5AE9F06B9E7159AFC0DAB34869B78EAB425CB5538B94FE0E7138F'
$spinAssetHash = '934716E4C09EBCA25B43E62BC1F6B69B26663FA54306F5054FA12A354B6DC5AF'
$masterAssetHash = 'DF3D1F863741E720EFC7B9ECC90117BB1BE9CA49852A86420FA8CA3E396D9F74'
$masterDisabledHash = '9FBD4B584C260A97B39E7C5FB93473879ECF96B8646396288BC4542DE36B9D5A'

foreach ($path in @($candidate,$staged,$backup,$assetTarget,$spinAssetTarget,$masterAssetTarget,$masterDisabledTarget)) {
    $absolute = [IO.Path]::GetFullPath($path)
    if (!$absolute.StartsWith($qaRoot + '\',[StringComparison]::OrdinalIgnoreCase)) {
        throw "QA path escaped isolated root: $path"
    }
    if (Test-Path -LiteralPath $path) {
        $item = Get-Item -LiteralPath $path -Force
        if ($item.Attributes -band [IO.FileAttributes]::ReparsePoint) {
            throw "Refusing reparse file: $path"
        }
    }
}
$stagedHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash
if ($stagedHash -ne $oldHash -and $stagedHash -ne $newHash) { throw 'Staged engine hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $candidate).Hash -ne $newHash) { throw 'Candidate engine hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $assetSource).Hash -ne $assetHash) { throw 'Circle source asset hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $spinAssetSource).Hash -ne $spinAssetHash) { throw 'Spin source asset hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $masterAssetSource).Hash -ne $masterAssetHash) { throw 'S21 master atlas hash mismatch' }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $masterDisabledSource).Hash -ne $masterDisabledHash) { throw 'S21 disabled master atlas hash mismatch' }
if (Test-Path -LiteralPath $assetTarget) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $assetTarget).Hash -ne $assetHash) { throw 'Circle target asset conflict' }
}
if (Test-Path -LiteralPath $spinAssetTarget) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $spinAssetTarget).Hash -ne $spinAssetHash) { throw 'Spin target asset conflict' }
}
if (Test-Path -LiteralPath $masterAssetTarget) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $masterAssetTarget).Hash -ne $masterAssetHash) { throw 'Master atlas target conflict' }
}
if (Test-Path -LiteralPath $masterDisabledTarget) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $masterDisabledTarget).Hash -ne $masterDisabledHash) { throw 'Disabled master atlas target conflict' }
}
if (Test-Path -LiteralPath $backup) {
    if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) { throw 'Rollback backup conflict' }
}
if ($stagedHash -eq $newHash) {
    if (!(Test-Path -LiteralPath $backup) -or !(Test-Path -LiteralPath $assetTarget) -or
        !(Test-Path -LiteralPath $spinAssetTarget) -or
        !(Test-Path -LiteralPath $masterAssetTarget) -or !(Test-Path -LiteralPath $masterDisabledTarget)) {
        throw 'Candidate engine staged without complete verified assets/rollback'
    }
    Write-Output 'PASS: isolated QA candidate already staged with hash-pinned assets/rollback; no writes'
    return
}
$running = @(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'")
if ($running.Count) {
    if ($Apply) { throw 'QA Engine is running; stage refused' }
    Write-Output "WAIT dry-run: hashes verified, QA Engine still running ($($running.Count)); no writes"
    return
}
Write-Output "PASS dry-run: old=$oldHash candidate=$newHash circle=$assetHash master=$masterAssetHash disabled=$masterDisabledHash"
if (!$Apply) { return }

if (!(Test-Path -LiteralPath $backup)) { Copy-Item -LiteralPath $staged -Destination $backup }
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $backup).Hash -ne $oldHash) { throw 'Rollback backup verification failed' }
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count) { throw 'QA Engine started during stage; refused' }
if (!(Test-Path -LiteralPath $assetTarget)) {
    New-Item -ItemType Directory -Path (Split-Path -Parent $assetTarget) -Force | Out-Null
    Copy-Item -LiteralPath $assetSource -Destination $assetTarget
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $assetTarget).Hash -ne $assetHash) { throw 'Circle staged asset verification failed' }
if (!(Test-Path -LiteralPath $spinAssetTarget)) {
    New-Item -ItemType Directory -Path (Split-Path -Parent $spinAssetTarget) -Force | Out-Null
    Copy-Item -LiteralPath $spinAssetSource -Destination $spinAssetTarget
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $spinAssetTarget).Hash -ne $spinAssetHash) { throw 'Spin staged asset verification failed' }
if (!(Test-Path -LiteralPath $masterAssetTarget)) {
    New-Item -ItemType Directory -Path (Split-Path -Parent $masterAssetTarget) -Force | Out-Null
    Copy-Item -LiteralPath $masterAssetSource -Destination $masterAssetTarget
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $masterAssetTarget).Hash -ne $masterAssetHash) { throw 'Master atlas staged asset verification failed' }
if (!(Test-Path -LiteralPath $masterDisabledTarget)) {
    New-Item -ItemType Directory -Path (Split-Path -Parent $masterDisabledTarget) -Force | Out-Null
    Copy-Item -LiteralPath $masterDisabledSource -Destination $masterDisabledTarget
}
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $masterDisabledTarget).Hash -ne $masterDisabledHash) { throw 'Disabled master atlas staged asset verification failed' }
if (@(Get-CimInstance Win32_Process -Filter "Name='Engine-Port S21.exe'").Count) { throw 'QA Engine started during asset stage; refused' }
Copy-Item -LiteralPath $candidate -Destination $staged -Force
if ((Get-FileHash -Algorithm SHA256 -LiteralPath $staged).Hash -ne $newHash) {
    Copy-Item -LiteralPath $backup -Destination $staged -Force
    throw 'QA engine stage verification failed; previous engine restored'
}
Write-Output "PASS: isolated Grow Lancer QA candidate staged; rollback=$backup"
