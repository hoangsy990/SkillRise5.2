param(
    [string]$CaptureRoot = (Join-Path $PSScriptRoot 'PegasusBuild\RuntimeClient\PegasusQACaptures'),
    [string]$OutputPath = (Join-Path $PSScriptRoot 'PegasusBuild\RuntimeClient\PegasusQA-contact-sheet.jpg')
)

$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing

$skills = @(
    @{ Id=288;  Name='Deathside' },
    @{ Id=289;  Name='Darkness' },
    @{ Id=2023; Name='Raining Arrow' },
    @{ Id=2024; Name='Dex Booster' },
    @{ Id=2012; Name='Chaos Blade' },
    @{ Id=2013; Name='Havoc Spear' },
    @{ Id=2014; Name='Spiral Charge' },
    @{ Id=2015; Name='Crusher Charge' },
    @{ Id=2016; Name='Elemental Charge' },
    @{ Id=1500; Name='Sword Wrath' },
    @{ Id=1501; Name='Sword Blow' },
    @{ Id=241;  Name='Shining Bird' },
    @{ Id=242;  Name='Dragon Violent' },
    @{ Id=243;  Name='Spearstorm' },
    @{ Id=737;  Name='Wind Soul' }
)

if (-not (Test-Path -LiteralPath $CaptureRoot -PathType Container)) {
    throw "Capture directory does not exist: $CaptureRoot"
}

$cellWidth = 320
$imageHeight = 180
$labelHeight = 28
$canvas = New-Object System.Drawing.Bitmap ($cellWidth * 3), (($imageHeight + $labelHeight) * $skills.Count)
$graphics = [System.Drawing.Graphics]::FromImage($canvas)
$font = New-Object System.Drawing.Font 'Segoe UI', 10
$brush = [System.Drawing.Brushes]::White
try {
    $graphics.Clear([System.Drawing.Color]::FromArgb(24, 24, 24))
    for ($row = 0; $row -lt $skills.Count; ++$row) {
        $skill = $skills[$row]
        foreach ($phase in 0..2) {
            $capture = Get-ChildItem -LiteralPath $CaptureRoot `
                -Filter "qa_*_skill_$($skill.Id)_phase_$phase.jpg" -File |
                Sort-Object LastWriteTime -Descending | Select-Object -First 1
            if ($null -eq $capture) {
                throw "$($skill.Name): capture phase $phase is missing"
            }
            $source = [System.Drawing.Image]::FromFile($capture.FullName)
            try {
                $x = $phase * $cellWidth
                $y = $row * ($imageHeight + $labelHeight)
                $graphics.DrawImage($source, $x, $y, $cellWidth, $imageHeight)
                $graphics.DrawString("$($skill.Name) [$($skill.Id)] phase $phase",
                    $font, $brush, $x + 4, $y + $imageHeight + 5)
            }
            finally { $source.Dispose() }
        }
    }
    $outputDirectory = Split-Path -Parent $OutputPath
    if ($outputDirectory) { [void](New-Item -ItemType Directory -Force -Path $outputDirectory) }
    $canvas.Save($OutputPath, [System.Drawing.Imaging.ImageFormat]::Jpeg)
}
finally {
    $font.Dispose()
    $graphics.Dispose()
    $canvas.Dispose()
}

Write-Output "CONTACT_SHEET=$OutputPath"
