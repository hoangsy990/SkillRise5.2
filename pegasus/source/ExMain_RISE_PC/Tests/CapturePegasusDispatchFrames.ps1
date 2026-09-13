param(
    [int]$ProcessId = 0,
    [string]$LaunchExecutable,
    [Parameter(Mandatory=$true)]
    [string]$LogPath,
    [string]$OutputDirectory = (Join-Path $PSScriptRoot 'PegasusBuild\Captures\dispatch'),
    [ValidateSet('CAST_DISPATCH', 'LOCAL_VISUAL_DISPATCH')]
    [string]$TraceEvent = 'CAST_DISPATCH',
    [int]$ExpectedDispatches = 15,
    [int]$TimeoutSeconds = 180
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
if (-not ('PegasusDispatchWindowCapture' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class PegasusDispatchWindowCapture {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }
    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
}
'@
}

function Save-WindowFrame([string]$Path) {
    $process = Get-Process -Id $ProcessId -ErrorAction Stop
    if ($process.MainWindowHandle -eq [IntPtr]::Zero) {
        throw "PID $ProcessId has no game window"
    }
    $rect = New-Object PegasusDispatchWindowCapture+RECT
    if (-not [PegasusDispatchWindowCapture]::GetWindowRect(
        $process.MainWindowHandle, [ref]$rect)) {
        throw "GetWindowRect failed for PID $ProcessId"
    }
    $bitmap = New-Object System.Drawing.Bitmap ($rect.Right - $rect.Left),
        ($rect.Bottom - $rect.Top)
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    try {
        $graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bitmap.Size)
        $bitmap.Save($Path, [System.Drawing.Imaging.ImageFormat]::Png)
    }
    finally {
        $graphics.Dispose()
        $bitmap.Dispose()
    }
}

New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$absoluteOutput = (Resolve-Path -LiteralPath $OutputDirectory).Path
$line = (Get-Content -LiteralPath $LogPath).Count
if ($ProcessId -eq 0) {
    if ([string]::IsNullOrWhiteSpace($LaunchExecutable)) {
        throw 'Provide either ProcessId or LaunchExecutable'
    }
    $absoluteExecutable = (Resolve-Path -LiteralPath $LaunchExecutable).Path
    $launched = Start-Process -FilePath $absoluteExecutable `
        -WorkingDirectory (Split-Path -Parent $absoluteExecutable) -PassThru
    $ProcessId = $launched.Id
    Write-Output "LAUNCHED $ProcessId $absoluteExecutable"
}
$deadline = (Get-Date).AddSeconds($TimeoutSeconds)
$captured = 0
$seenSkills = [System.Collections.Generic.HashSet[int]]::new()
while ($captured -lt $ExpectedDispatches -and (Get-Date) -lt $deadline) {
    $all = @(Get-Content -LiteralPath $LogPath)
    if ($all.Count -le $line) {
        Start-Sleep -Milliseconds 50
        continue
    }
    $new = @($all | Select-Object -Skip $line)
    $line = $all.Count
    foreach ($entry in $new) {
        $pattern = [regex]::Escape($TraceEvent) +
            ' value1=(?<skill>\d+) .*result=DISPATCHED'
        if ($entry -notmatch $pattern) {
            continue
        }
        $skill = [int]$Matches.skill
        # Reaction-capable skills intentionally dispatch ten native samples.
        # This auxiliary recorder remains one early/late pair per unique skill
        # instead of exhausting its quota on the first repeated QA step.
        if (-not $seenSkills.Add($skill)) {
            continue
        }
        ++$captured
        Start-Sleep -Milliseconds 200
        $early = Join-Path $absoluteOutput (
            '{0:D2}-skill-{1}-early.png' -f $captured, $skill)
        Save-WindowFrame $early
        Start-Sleep -Milliseconds 700
        $late = Join-Path $absoluteOutput (
            '{0:D2}-skill-{1}-late.png' -f $captured, $skill)
        Save-WindowFrame $late
        Write-Output "$skill $early $late"
    }
}

if ($captured -ne $ExpectedDispatches) {
    throw "Captured $captured/$ExpectedDispatches dispatches"
}
