$ErrorActionPreference = 'Stop'
$root = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
if ($root -ne 'D:\RISE-CrossPlatform\Source\_PC_GrowLancer') { throw 'Unexpected worktree' }
$destination = Join-Path $root 'Client\Data\RISE\GrowLancer\Wrath'
$rows = @(
    @('firehik_mono01.OZJ','C3E18C474BA5AE9F06B9E7159AFC0DAB34869B78EAB425CB5538B94FE0E7138F'),
    @('firehik_mono02.OZJ','EA67813627C32A9F9EAE948FFFF2EE47A0FD44A49426CCE2879FCEF4A6B5F7D6'),
    @('firehik_mono03.OZJ','CF6BA21108507E377C40145912EC3DB7FBBD19031AC8AEAF129F5A6773FFF315'),
    @('lighting_mega01.OZJ','FDF9023E93EC2D1DA880A406A591E9BB26D3A5F53C609575886A809F7A89D8EF'),
    @('lighting_mega02.OZJ','6D00E30EE3865CF769AEA8678BDEF654EDEF9F48448C62EC1C6BAC3212A8090E'),
    @('lighting_mega03.OZJ','3C4435F499AAA4877B2506FFDC871EE1482EE0D4CDDF881B3F0A49FEFB071B9F'),
    @('lightmarks.OZJ','DC4E83DDB76554298ED9619FEFB02DE7E0D3A210DB8424B97CB1244E62B4305F'),
    @('flare01.OZJ','874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8'),
    @('flareBlue.OZJ','E261B59F57FDF0C514917A388F02AC66B832C2BE9F97A3F90BC36C98D7B2FB32'),
    @('Shockwave2.OZJ','F47DC1F04D0DDCA6DC50E8EA09D44F430EB4B80D2B2C52D44BEC3BC8D846FEA2')
)
# Validate the complete set before copying anything; never overwrite a different file.
foreach ($row in $rows) {
    $original = Join-Path 'D:\MU FICA Season 21\Data\Effect' $row[0]
    $target = Join-Path $destination $row[0]
    if ((Get-FileHash -LiteralPath $original -Algorithm SHA256).Hash -ne $row[1]) { throw "Source mismatch: $original" }
    if ((Test-Path -LiteralPath $target) -and (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $row[1]) { throw "Destination mismatch: $target" }
}
New-Item -ItemType Directory -Path $destination -Force | Out-Null
foreach ($row in $rows) {
    $target = Join-Path $destination $row[0]
    if (!(Test-Path -LiteralPath $target)) {
        Copy-Item -LiteralPath (Join-Path 'D:\MU FICA Season 21\Data\Effect' $row[0]) -Destination $target
    }
    if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne $row[1]) { throw "Copy verification failed: $target" }
    Write-Output "PASS $target"
}
