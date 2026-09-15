param([switch]$Launch)

$ErrorActionPreference = 'Stop'
$taskRoot = 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer'
$stackRoot = Join-Path $taskRoot 'ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke'
$stackRoot = [IO.Path]::GetFullPath($stackRoot)
if ($stackRoot -ne 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\ExGameServer\Tests\GrowLancerBuild\ServerStackSmoke') {
    throw 'Refusing non-isolated stack path'
}

& python (Join-Path $taskRoot 'tools\grow_lancer\verify_isolated_server_stack.py')
if ($LASTEXITCODE -ne 0) { throw 'Isolated stack/config hash verification failed' }

$dsn = @(Get-OdbcDsn -Platform '32-bit' -ErrorAction Stop | Where-Object { $_.Name -eq 'GrowLancer_QA' })
if ($dsn.Count -ne 1 -or $dsn[0].Attribute.Database -ne 'GrowLancer_QA' -or
    $dsn[0].Attribute.Server -ne '(local)' -or $dsn[0].Attribute.Trusted_Connection -ne 'Yes') {
    throw 'Current-user QA DSN is missing or points outside GrowLancer_QA'
}
$ps32 = 'C:\Windows\SysWOW64\WindowsPowerShell\v1.0\powershell.exe'
$dbName = & $ps32 -NoProfile -Command '$c=New-Object System.Data.Odbc.OdbcConnection("DSN=GrowLancer_QA;Trusted_Connection=Yes"); $c.Open(); $q=$c.CreateCommand(); $q.CommandText="SELECT DB_NAME()"; $q.ExecuteScalar(); $c.Close()'
if ($LASTEXITCODE -ne 0 -or $dbName -ne 'GrowLancer_QA') { throw '32-bit QA ODBC connection failed' }

$components = @(
    @{ Name='ConnectServer'; File='1.ConnectServer\ConnectServer.exe'; Dir='1.ConnectServer'; Port=44412; Protocol='TCP' },
    @{ Name='DataServer'; File='2.DataServer\DataServer.exe'; Dir='2.DataServer'; Port=55964; Protocol='TCP' },
    @{ Name='JoinServer'; File='3.JoinServer\JoinServer.exe'; Dir='3.JoinServer'; Port=55974; Protocol='TCP' },
    @{ Name='GameServer'; File='4.MuServer\Sub-1\GameServer\GameServer-GrowLancer-S21-QA.exe'; Dir='4.MuServer\Sub-1\GameServer'; Port=55910; Protocol='TCP' }
)

function Get-QAListener([int]$port, [string]$protocol) {
    if ($protocol -eq 'UDP') { return @(Get-NetUDPEndpoint -LocalPort $port -ErrorAction SilentlyContinue) }
    return @(Get-NetTCPConnection -LocalPort $port -State Listen -ErrorAction SilentlyContinue)
}

foreach ($port in @(44412,55561,55964,55974,55910)) {
    if ((Get-QAListener $port 'TCP').Count -or (Get-QAListener $port 'UDP').Count) {
        throw "QA port is already bound: $port"
    }
}
$existing = @(Get-CimInstance Win32_Process | Where-Object {
    $_.ExecutablePath -and $_.ExecutablePath.StartsWith($stackRoot + '\', [StringComparison]::OrdinalIgnoreCase) -and
    ($_.Name -in @('ConnectServer.exe','DataServer.exe','JoinServer.exe') -or $_.Name -like 'GameServer*.exe')
})
if ($existing.Count) { throw 'Grow Lancer stack has existing processes; refusing duplicate or partial launch' }
Write-Output 'PASS: current-user 32-bit QA DSN connects to GrowLancer_QA; five isolated ports are free'
if (!$Launch) { Write-Output 'DRY RUN: use -Launch to start four Grow Lancer-only components'; return }

$started = @()
try {
    foreach ($component in $components) {
        $exe = [IO.Path]::GetFullPath((Join-Path $stackRoot $component.File))
        $workingDirectory = [IO.Path]::GetFullPath((Join-Path $stackRoot $component.Dir))
        if (!$exe.StartsWith($stackRoot + '\', [StringComparison]::OrdinalIgnoreCase) -or
            !$workingDirectory.StartsWith($stackRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
            throw 'Component path escaped the Grow Lancer stack'
        }
        $process = Start-Process -FilePath $exe -WorkingDirectory $workingDirectory -WindowStyle Hidden -PassThru
        $started += $process.Id
        $listenerOwned = $false
        for ($attempt=0; $attempt -lt 30; $attempt++) {
            Start-Sleep -Milliseconds 500
            if (!(Get-Process -Id $process.Id -ErrorAction SilentlyContinue)) { break }
            $listenerOwned = @((Get-QAListener $component.Port $component.Protocol) | Where-Object { $_.OwningProcess -eq $process.Id }).Count -gt 0
            if ($listenerOwned) { break }
        }
        if (!$listenerOwned) { throw "QA $($component.Name) did not bind its isolated port $($component.Port)" }
        if ($component.Name -eq 'ConnectServer') {
            $udpOwned = @((Get-QAListener 55561 'UDP') | Where-Object { $_.OwningProcess -eq $process.Id }).Count -gt 0
            if (!$udpOwned) { throw 'QA ConnectServer did not bind isolated UDP 55561' }
        }
        Write-Output "PASS: $($component.Name) PID=$($process.Id) port=$($component.Port)"
    }
} catch {
    foreach ($processId in $started) {
        $owned = Get-CimInstance Win32_Process -Filter "ProcessId=$processId" -ErrorAction SilentlyContinue
        if ($owned -and $owned.ExecutablePath -and
            $owned.ExecutablePath.StartsWith($stackRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
            Stop-Process -Id $processId -ErrorAction SilentlyContinue
        }
    }
    throw
}
Write-Output ('PASS: local Grow Lancer-only stack running; PIDs=' + ($started -join ','))
Write-Output 'Scope: transport/DB smoke only; Grow Lancer server skill handlers and owner visual QA remain IN_PROCESS'
