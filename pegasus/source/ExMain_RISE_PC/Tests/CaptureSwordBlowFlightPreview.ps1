param(
    [string]$RuntimeDirectory = (Join-Path $PSScriptRoot 'PegasusBuild\RuntimeClient'),
    [string]$OutputDirectory = (Join-Path $PSScriptRoot 'PegasusBuild\Captures\sword-blow-fly'),
    [int]$TimeoutSeconds = 45,
    [int]$FrameCount = 18,
    [int]$FrameDelayMs = 70
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
if (-not ('PegasusSwordBlowPreviewCapture' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class PegasusSwordBlowPreviewCapture {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }
    [DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
    [DllImport("user32.dll")] public static extern bool ShowWindowAsync(IntPtr hWnd, int command);
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
}
'@
}

function Save-GameFrame([System.Diagnostics.Process]$Process, [string]$Path) {
    $Process.Refresh()
    $handle = $Process.MainWindowHandle
    if ($handle -eq [IntPtr]::Zero) { throw "PID $($Process.Id) has no game window" }
    [PegasusSwordBlowPreviewCapture]::ShowWindowAsync($handle, 9) | Out-Null
    [PegasusSwordBlowPreviewCapture]::SetForegroundWindow($handle) | Out-Null
    $rect = New-Object PegasusSwordBlowPreviewCapture+RECT
    if (-not [PegasusSwordBlowPreviewCapture]::GetWindowRect($handle, [ref]$rect)) {
        throw 'GetWindowRect failed'
    }
    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    $bitmap = New-Object System.Drawing.Bitmap $width, $height
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

$runtime = (Resolve-Path -LiteralPath $RuntimeDirectory).Path
$engine = Join-Path $runtime 'Engine-Pegasus.exe'
$log = Join-Path $runtime 'PegasusRuntimeQA.log'
New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$output = (Resolve-Path -LiteralPath $OutputDirectory).Path
$startLine = if (Test-Path -LiteralPath $log) { (Get-Content -LiteralPath $log).Count } else { 0 }
$process = Start-Process -FilePath $engine -WorkingDirectory $runtime -PassThru
Write-Output "LAUNCHED $($process.Id)"

$deadline = (Get-Date).AddSeconds($TimeoutSeconds)
$found = $false
while ((Get-Date) -lt $deadline -and -not $process.HasExited) {
    if (Test-Path -LiteralPath $log) {
        $lines = @(Get-Content -LiteralPath $log | Select-Object -Skip $startLine)
        if ($lines -match 'SWORD_BLOW_FLY .*result=START') {
            $found = $true
            break
        }
    }
    Start-Sleep -Milliseconds 40
    $process.Refresh()
}
if (-not $found) { throw 'Timed out waiting for SWORD_BLOW_FLY START' }

for ($index = 0; $index -lt $FrameCount; ++$index) {
    $path = Join-Path $output ('frame-{0:D2}.png' -f $index)
    Save-GameFrame $process $path
    Start-Sleep -Milliseconds $FrameDelayMs
}
Write-Output "CAPTURED $FrameCount frames in $output"
Write-Output "PROCESS_ID $($process.Id)"
