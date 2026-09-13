param(
    [Parameter(Mandatory = $true)]
    [int]$ProcessId,
    [Parameter(Mandatory = $true)]
    [string]$OutputDirectory,
    [int]$DurationSeconds = 90,
    [int]$IntervalMilliseconds = 150
)

$ErrorActionPreference = 'Stop'
$expectedRoot = [IO.Path]::GetFullPath(
    'D:\RISE-CrossPlatform\Source\_PC_GrowLancer\artifacts\runtime_qa\s21_reference')
$resolvedOutput = [IO.Path]::GetFullPath($OutputDirectory)
if (!$resolvedOutput.StartsWith($expectedRoot, [StringComparison]::OrdinalIgnoreCase)) {
    throw "Refusing capture outside isolated Grow Lancer evidence root: $resolvedOutput"
}

Add-Type -AssemblyName System.Drawing
Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
public static class GrowLancerWindowRect {
    [StructLayout(LayoutKind.Sequential)]
    public struct RECT { public int Left, Top, Right, Bottom; }
    [DllImport("user32.dll")]
    public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
}
'@

New-Item -ItemType Directory -Force -Path $resolvedOutput | Out-Null
$manifest = Join-Path $resolvedOutput 'frames.csv'
"frame,utc_iso,tick_ms,left,top,width,height,file" | Set-Content -LiteralPath $manifest -Encoding utf8

$jpegCodec = [Drawing.Imaging.ImageCodecInfo]::GetImageEncoders() |
    Where-Object MimeType -eq 'image/jpeg' |
    Select-Object -First 1
$encoderParameters = [Drawing.Imaging.EncoderParameters]::new(1)
$encoderParameters.Param[0] = [Drawing.Imaging.EncoderParameter]::new(
    [Drawing.Imaging.Encoder]::Quality, 88L)

$stopwatch = [Diagnostics.Stopwatch]::StartNew()
$frame = 0
while ($stopwatch.Elapsed.TotalSeconds -lt $DurationSeconds) {
    $process = Get-Process -Id $ProcessId -ErrorAction SilentlyContinue
    if (!$process) { break }
    $handle = $process.MainWindowHandle
    if ($handle -eq [IntPtr]::Zero) {
        Start-Sleep -Milliseconds $IntervalMilliseconds
        continue
    }
    $rect = [GrowLancerWindowRect+RECT]::new()
    if (![GrowLancerWindowRect]::GetWindowRect($handle, [ref]$rect)) {
        Start-Sleep -Milliseconds $IntervalMilliseconds
        continue
    }
    $width = $rect.Right - $rect.Left
    $height = $rect.Bottom - $rect.Top
    if ($width -le 0 -or $height -le 0) {
        Start-Sleep -Milliseconds $IntervalMilliseconds
        continue
    }

    $bitmap = [Drawing.Bitmap]::new($width, $height,
        [Drawing.Imaging.PixelFormat]::Format24bppRgb)
    $graphics = [Drawing.Graphics]::FromImage($bitmap)
    try {
        $graphics.CopyFromScreen($rect.Left, $rect.Top, 0, 0,
            [Drawing.Size]::new($width, $height), [Drawing.CopyPixelOperation]::SourceCopy)
        $name = 'frame_{0:D5}.jpg' -f $frame
        $path = Join-Path $resolvedOutput $name
        $bitmap.Save($path, $jpegCodec, $encoderParameters)
        $now = [DateTime]::UtcNow.ToString('o')
        "{0},{1},{2},{3},{4},{5},{6},{7}" -f $frame,$now,$stopwatch.ElapsedMilliseconds,
            $rect.Left,$rect.Top,$width,$height,$name |
            Add-Content -LiteralPath $manifest -Encoding utf8
        ++$frame
    }
    finally {
        $graphics.Dispose()
        $bitmap.Dispose()
    }
    Start-Sleep -Milliseconds $IntervalMilliseconds
}

$encoderParameters.Dispose()
"captured_frames=$frame duration_ms=$($stopwatch.ElapsedMilliseconds)" |
    Set-Content -LiteralPath (Join-Path $resolvedOutput 'capture_result.txt') -Encoding utf8
