$ErrorActionPreference = 'Stop'

$stackRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke'
$expectedRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke'
if ([IO.Path]::GetFullPath($stackRoot) -ne $expectedRoot) {
    throw "Refusing non-isolated server root: $stackRoot"
}

$owned = Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and
    [IO.Path]::GetFullPath($_.ExecutablePath).StartsWith($expectedRoot + '\',
        [StringComparison]::OrdinalIgnoreCase)
}
foreach ($process in $owned) {
    Stop-Process -Id $process.ProcessId -ErrorAction Stop
    Write-Output "Stopped isolated PID=$($process.ProcessId) $($process.ExecutablePath)"
}
if (!$owned) { Write-Output 'No Grow Lancer isolated server process is running' }
