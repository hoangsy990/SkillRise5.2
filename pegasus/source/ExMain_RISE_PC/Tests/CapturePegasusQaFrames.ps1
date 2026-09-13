param(
    [Parameter(Mandatory=$true)]
    [int]$ProcessId,
    [int]$FrameCount = 18,
    [int]$IntervalSeconds = 4,
    [string]$OutputDirectory = (Join-Path $PSScriptRoot 'PegasusBuild\Captures')
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
if (-not ('PegasusQaWindowCapture' -as [type])) {
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class PegasusQaWindowCapture {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }
    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
}
'@
}

New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
$absoluteOutput = (Resolve-Path -LiteralPath $OutputDirectory).Path
for ($frame = 0; $frame -lt $FrameCount; ++$frame) {
    $process = Get-Process -Id $ProcessId -ErrorAction Stop
    if ($process.MainWindowHandle -eq [IntPtr]::Zero) {
        Start-Sleep -Seconds 1
        continue
    }
    $rect = New-Object PegasusQaWindowCapture+RECT
    if (-not [PegasusQaWindowCapture]::GetWindowRect(
        $process.MainWindowHandle, [ref]$rect)) {
        throw "GetWindowRect failed for PID $ProcessId"
    }
    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    $bitmap = New-Object System.Drawing.Bitmap $width, $height
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    try {
        $graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0, $bitmap.Size)
        $timestamp = Get-Date -Format 'yyyyMMdd-HHmmss-fff'
        $path = Join-Path $absoluteOutput ("qa-{0:D2}-{1}.png" -f $frame, $timestamp)
        $bitmap.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Output $path
    }
    finally {
        $graphics.Dispose()
        $bitmap.Dispose()
    }
    if ($frame + 1 -lt $FrameCount) {
        Start-Sleep -Seconds $IntervalSeconds
    }
}
