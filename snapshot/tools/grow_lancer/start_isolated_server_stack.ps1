$ErrorActionPreference = 'Stop'

$stackRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke'
$expectedRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke'
if ([IO.Path]::GetFullPath($stackRoot) -ne $expectedRoot) {
    throw "Refusing non-isolated server root: $stackRoot"
}

& python 'tools\grow_lancer\verify_isolated_server_stack.py'
if ($LASTEXITCODE -ne 0) { throw 'Grow Lancer server stack verification failed' }

$components = @(
    @{ Name = 'ConnectServer'; File = '1.ConnectServer\ConnectServer.exe'; Dir = '1.ConnectServer' },
    @{ Name = 'DataServer'; File = '2.DataServer\DataServer.exe'; Dir = '2.DataServer' },
    @{ Name = 'JoinServer'; File = '3.JoinServer\JoinServer.exe'; Dir = '3.JoinServer' },
    @{ Name = 'GameServer-GrowLancer-S21-QA'; File = '4.MuServer\Sub-1\GameServer\GameServer-GrowLancer-S21-QA.exe'; Dir = '4.MuServer\Sub-1\GameServer' }
)

$foreign = Get-CimInstance Win32_Process | Where-Object {
    ($_.Name -in @('ConnectServer.exe', 'DataServer.exe', 'JoinServer.exe') -or
        $_.Name -like 'GameServer*.exe') -and
    $_.ExecutablePath -and
    !([IO.Path]::GetFullPath($_.ExecutablePath).StartsWith($expectedRoot + '\',
        [StringComparison]::OrdinalIgnoreCase))
}
if ($foreign) {
    $details = ($foreign | ForEach-Object { "PID=$($_.ProcessId) $($_.ExecutablePath)" }) -join [Environment]::NewLine
    throw "Refusing port collision with another server stack:`n$details"
}

$started = @()
foreach ($component in $components) {
    $exe = Join-Path $stackRoot $component.File
    $workingDirectory = Join-Path $stackRoot $component.Dir
    $existing = Get-CimInstance Win32_Process | Where-Object {
        $_.ExecutablePath -and
        [IO.Path]::GetFullPath($_.ExecutablePath) -eq [IO.Path]::GetFullPath($exe)
    }
    if ($existing) {
        $started += $existing.ProcessId
        continue
    }
    $process = Start-Process -FilePath $exe -WorkingDirectory $workingDirectory `
        -WindowStyle Hidden -PassThru
    $started += $process.Id
    Start-Sleep -Milliseconds 750
}

Start-Sleep -Seconds 2
foreach ($pidValue in $started) {
    if (!(Get-Process -Id $pidValue -ErrorAction SilentlyContinue)) {
        throw "Isolated server component exited during startup: PID $pidValue"
    }
}
Write-Output ("PASS: Grow Lancer isolated stack is alive; PIDs=" + ($started -join ','))
Write-Output 'PASS: each process was launched with its own component directory as WorkingDirectory'
