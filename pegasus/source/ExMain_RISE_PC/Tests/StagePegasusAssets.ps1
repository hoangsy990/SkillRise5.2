$ErrorActionPreference = 'Stop'
$evidenceRoot = 'D:\RISE-CrossPlatform\Source\artifacts\pegasus_high_skills\sword_blow'
$report = Get-Content -LiteralPath (Join-Path $evidenceRoot 'evidence.json') -Raw | ConvertFrom-Json
if (-not $report.stagedCandidates -or $report.referenceSha256 -ne '02922A0C32060DADA6412182E2FF7E7273AF5A2E61BB5E86B8F585C412DEA0AA') {
    throw 'Missing or changed pinned Pegasus asset evidence'
}
$sourceRoot = [IO.Path]::GetFullPath((Join-Path $evidenceRoot 'asset_candidates\Data'))
$targetRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'PegasusBuild\Client\Data\RISE\Pegasus\SwordBlow'))
$operations = @()
foreach ($entry in $report.stagedCandidates) {
    $sourcePath = [IO.Path]::GetFullPath((Join-Path $sourceRoot $entry.relativePath))
    $targetPath = [IO.Path]::GetFullPath((Join-Path $targetRoot $entry.relativePath))
    if (-not $sourcePath.StartsWith($sourceRoot + '\', [StringComparison]::OrdinalIgnoreCase) -or
        -not $targetPath.StartsWith($targetRoot + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Asset path escapes isolated roots'
    }
    if ((Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash -ne $entry.sha256) {
        throw "Source hash mismatch: $sourcePath"
    }
    if ((Test-Path -LiteralPath $targetPath) -and
        (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash -ne $entry.sha256) {
        throw "Refusing to replace changed test asset: $targetPath"
    }
    $operations += [pscustomobject]@{ Source=$sourcePath; Target=$targetPath; Hash=$entry.sha256 }
}
$swordWrathTarget = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'PegasusBuild\Client\Data\RISE\Pegasus\SwordWrath\Effect'))
$swordWrathCandidates = @(
    @{ Source='D:\Pegasus\Data\Effect\Fire_wall_knight.bmd'; Name='Fire_wall_knight.bmd'; Hash='5E3E3642D5B5D1F6968C5BF1CE8026E2B9BA7675C95D897EA5B91243F8E1E682' },
    @{ Source='D:\Pegasus\Data\Effect\fire_15fmono.OZJ'; Name='fire_15fmono.OZJ'; Hash='260BB3BB7B86CD098C617E929D4CD4735510EC20B8D6E4CC4A80D645343DCD1A' }
)
foreach ($entry in $swordWrathCandidates) {
    $sourcePath = [IO.Path]::GetFullPath($entry.Source)
    $targetPath = [IO.Path]::GetFullPath((Join-Path $swordWrathTarget $entry.Name))
    if (-not $sourcePath.StartsWith('D:\Pegasus\Data\Effect\', [StringComparison]::OrdinalIgnoreCase) -or
        -not $targetPath.StartsWith($swordWrathTarget + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Sword Wrath asset path escapes isolated roots'
    }
    if ((Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Sword Wrath source hash mismatch: $sourcePath"
    }
    if ((Test-Path -LiteralPath $targetPath) -and
        (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Refusing to replace changed Sword Wrath test asset: $targetPath"
    }
    $operations += [pscustomobject]@{ Source=$sourcePath; Target=$targetPath; Hash=$entry.Hash }
}
$soundTarget = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'PegasusBuild\Client\Data\RISE\Pegasus\Sound'))
$soundCandidates = @(
    @{ Relative='ReaperAttack1.wav'; Hash='FFFEB2620CBCF4BBBD46E0BF17645347E5DBE7F4D75168834032630F69ACEAE3' },
    @{ Relative='ReaperAttack2.wav'; Hash='E62BFFC7A14A38B923E1B5618F093A4DDA0D8994EB25C3645FE5798325BF34D9' },
    @{ Relative='ReaperAttack3.wav'; Hash='EBA804713FEBDE41C614777F24E2CC3FB08F1370B3C77CAFBA84EAD1C664BD5C' },
    @{ Relative='ReaperAppear.wav'; Hash='CEB245E90CF924CBDCF41B0ABCC1930FFCB887E99C5077D5873E09B20D4BA369' },
    @{ Relative='Reaperdisappear.wav'; Hash='34C5F8D2CFD5F5FE5CDAA1207C67AFC070A8ACB8E22C240349C6E79F5AA6FB2A' },
    @{ Relative='Elf\RainingArrow.wav'; Hash='1F11A753E2D010869ECEE850C8478D4651125AE151829292A1143F1D5DB077EC' },
    @{ Relative='Elf\RainingArrow-Hit.wav'; Hash='CE00F53BABAA04964097149EDBA08C3208632335141D5991B8E066388ABDD46E' },
    @{ Relative='MG_Renewal_HavocSpear.wav'; Hash='5EC5E9D38D2027EC4DC106960ECAC5330610880D94EDC9818DA2B711F8AA009E' },
    @{ Relative='MG_Renewal_HavocSpear_Hit_1.wav'; Hash='50F4A4AD67C511418B3503D2566624A9236CC0D6F16913A463CD185D9EEAC554' },
    @{ Relative='MG_Renewal_HavocSpear_Hit_2.wav'; Hash='45DEA41A3AB7BB002A79A67D252E9FB48D3D70950E473ABE73A5606C6C113E93' },
    @{ Relative='MG_Renewal_HavocSpear_Explosion.wav'; Hash='8F261CDA3E543531FCA9CEDB911B0FC29CF5E8E325F89E8FA7845C878A9E1A02' },
    @{ Relative='MG_Renewal_SpiralCharge.wav'; Hash='F4A4B4941A8748B11A7E0079A94AF8220526107103E7CD11F5521E2D359D4C06' },
    @{ Relative='MG_Renewal_CrasherCharge.wav'; Hash='A850F7A08D7F1DC0288ED065BDE183EC393760E9BD55CDC4B37BAA6BB4C22A5E' },
    @{ Relative='MG_Renewal_ElementalCharge.wav'; Hash='CB13C248261B1F3B66C4A3E3CA872A95190FDE1DD473D2F098553548908517A6' },
    @{ Relative='MG_Renewal_ChaosBlade.wav'; Hash='EE3F18B30FCEFF0562851E0A5633838FB322E85D435E0D536EC1A71EA5C8DBC1' },
    @{ Relative='MG_Renewal_ChaosBlade_Hit_1.wav'; Hash='8B8AAFDCAE987812F1FEB6A2A8F7259960D550CBA2F818AD51AA17C193BFA60E' },
    @{ Relative='MG_Renewal_ChaosBlade_Hit_2.wav'; Hash='43A4B0088F8DEE28E9A887B25E09C44F17EC5FE58584BF70FB60EB006E54EC93' },
    @{ Relative='MG_Renewal_ChaosBlade_Explosion.wav'; Hash='BE39315F3D53BF746E1B86E642A074535866C687F36B37EC942A72356933CA77' },
    @{ Relative='Kundun\DragonViolent.wav'; Hash='BA5A71F66FCD4F905E2069A9AEB2144721F9782C62A6033575E42EEC5E16CE0A' },
    @{ Relative='Kundun\DragonViolent_Hit.wav'; Hash='E6AB3BCC6D63A0C2423A797A30BDFB18F878A6D4F2F3A544A22EA453B8E1B9F2' },
    @{ Relative='Kundun\ShiningBird.wav'; Hash='D33577D53D9C9C6DD7C14C4D1F6D6DEC5EC5E977E8222DA734A3372049D40106' },
    @{ Relative='Kundun\SpearStorm.wav'; Hash='3BF6CA4A67B7D27BAA254D4B89452E07D8CE19EDB1D7DD80CECBEAC09B42C8B3' },
    @{ Relative='Kundun\SpearStorm_Hit.wav'; Hash='4FEDD25C219913B4D48C198BC566DAA677A97AB5155688ADB773BC2083B83EE7' }
)
foreach ($entry in $soundCandidates) {
    $sourceRelative = $entry.Relative
    if ($sourceRelative.StartsWith('Elf\') -or $sourceRelative.StartsWith('Kundun\')) {
        $sourcePath = [IO.Path]::GetFullPath((Join-Path 'D:\Pegasus\Data\Sound' $sourceRelative))
    } else {
        $sourcePath = [IO.Path]::GetFullPath((Join-Path 'D:\Pegasus\Data\Sound' $sourceRelative))
    }
    $targetPath = [IO.Path]::GetFullPath((Join-Path $soundTarget $sourceRelative))
    if (-not $sourcePath.StartsWith('D:\Pegasus\Data\Sound\', [StringComparison]::OrdinalIgnoreCase) -or
        -not $targetPath.StartsWith($soundTarget + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Pegasus sound path escapes isolated roots'
    }
    if ((Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Pegasus sound source hash mismatch: $sourcePath"
    }
    if ((Test-Path -LiteralPath $targetPath) -and
        (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Refusing to replace changed Pegasus sound: $targetPath"
    }
    $operations += [pscustomobject]@{ Source=$sourcePath; Target=$targetPath; Hash=$entry.Hash }
}
$recoveredTarget = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'PegasusBuild\Client\Data\RISE\Pegasus\Recovered\Data'))
$recoveredCandidates = @(
    @{ Relative='Player\player.bmd'; Hash='FF75CA280C6633015F11D1A1304286FA6362D5A82BFD9CB1119E2FAD2D69B1B5' },
    @{ Relative='Skill\Reaper.bmd'; Hash='679ABB29CF3D7DF47CC8713C7CA4A659A4E6348173CA65D6904E07E4AAF83897' },
    @{ Relative='Skill\ReaperHead.bmd'; Hash='7F7E67929B84EC00640693868EC53542E6F7C9459440B8EAAAFA910918EE9C46' },
    @{ Relative='Skill\ScytheEffAni01.bmd'; Hash='27064560626BBE55181FD2711B1B5098ACFF623AD396C4ED56E7EB0BCA917411' },
    @{ Relative='Skill\ScytheEffAni02.bmd'; Hash='8366FBB39CFADD9C8EF5880B0AC6FAEDE9C58ADF0AC81EB05C56CBBDAED976BA' },
    @{ Relative='Skill\ScytheEffAni03.bmd'; Hash='C933E893D2C6A6CA159D3C54F2EC2CD79A74EC97BB10AFC29C1FC36B42307DE1' },
    @{ Relative='Skill\ebbone2.OZT'; Hash='EED307F7860BB6838415DC03BFC2ED3FDF05AABBFB513F30B019F7CBB7515994' },
    @{ Relative='Skill\ebbone.OZT'; Hash='EB862D1F27844142D162303956D806D67A0AD8E6696A241D55143067FD0EBC17' },
    @{ Relative='Skill\eyecoloreffect_R.OZJ'; Hash='87C211BDAF3EFFAF9A3CAA13C9009488123D667E4DA717F91BDF3892261C0B21' },
    @{ Relative='Skill\dead_scythe02.OZJ'; Hash='71BC134C53ED88B470B64D7776D8471F4407690563B75B2A3A3B4E7A4B8BD815' },
    @{ Relative='Skill\dead_scythe01.OZJ'; Hash='71BC134C53ED88B470B64D7776D8471F4407690563B75B2A3A3B4E7A4B8BD815' },
    @{ Relative='Skill\mini_headdead.OZJ'; Hash='BDEC7404484275BBB59A8F3AAE2E0CD9F9A8084E42D1FA2A4552473CB5071B32' },
    @{ Relative='Skill\cratered_render.OZJ'; Hash='BA80529C42BCE03F615AF03470C37DFA294636B4477EF97844BEF4AFDA7169A1' },
    @{ Relative='Skill\ground_star.OZJ'; Hash='00FD18620AF56137C1EBDBF18DECD32AB93AC2529A46FECE011A1482039C7E4C' },
    @{ Relative='Effect\damage01mono.OZJ'; Hash='4688846140838C454A0791847D43D2D3F6A607579804898E796ABB542AFBC00F' },
    @{ Relative='Effect\ground_smoke_mono.OZJ'; Hash='079794DD60FCBB6E9AE475DF85E488E728251BAD2A1D64C16644C5FE4E26B3DF' },
    @{ Relative='Effect\blur02_mono.OZJ'; Hash='CF21198A37F3E3483E5A043799B36289FBDD8108FD5C2B22E74A66306D3E73ED' },
    @{ Relative='Skill\M_sword_effect.bmd'; Hash='A9185FBEE4FB47D46F0315CE30B1F5B8D1B3E4A050CBC414AAA4BBB6E6CF657F' },
    @{ Relative='Effect\knight_plancrack_a.bmd'; Hash='8F88AA8E15504650FF1747A17DAA67187AD6455B3D6E5FF6DE95D6954BB41AC4' },
    @{ Relative='Effect\min_elf_skill_ring01.bmd'; Hash='B5DCE58544EC0637380FC91287126442102CD61AEE66086EEBE6C0BEE34993EC' },
    @{ Relative='Effect\min_elf_skill_ring02.bmd'; Hash='10E8C80B00419E4159133ECEEBB91A9494F3BD3709C615C29814B92972C7E39C' },
    @{ Relative='Effect\min_elf_skill_ring03.bmd'; Hash='82BF4BC9D4CA56F67CCC35FFDFCA071B260A341ACA8F4CBB541252C6AE621B00' },
    @{ Relative='Effect\min_elf_skill_ring04.bmd'; Hash='FD1D45C25E3917EE4F46574508ED550A31325C4F6E4F22963C1790DD2917E1E3' },
    @{ Relative='Effect\min_elf_skill_ring05.bmd'; Hash='A6676B234E62469DD10A0B98C5949D04B44FD9893F466F27E0DAE1128694960A' },
    @{ Relative='Effect\min_elf_skill_ring06.bmd'; Hash='7740381B90A9418B5C5AEA9C208DF3AAAD55D43F6CE43321A7BABC74B1F47D7D' },
    @{ Relative='Effect\min_elf_skill_ring07.bmd'; Hash='88D865438D7C49E2D28E19D536C6E56E6CFF77A93F8EB5D9D6138796FD439B3F' },
    @{ Relative='Effect\min_elf_skill_ring08.bmd'; Hash='D970ED01378F2574A52E15E9BFBC174DD1507BB3F1865C7D3ADAC4AA213D43E7' },
    @{ Relative='Effect\circle_shields.OZJ'; Hash='F5ED70BAF0746EB40CC7687A49242D2F0E6DAEB26B3B4C442C65CF9225B83A0C' },
    @{ Relative='Effect\Shockwave.OZJ'; Hash='F8152B6F1A247578AABA09B2FD55B98AB098AADCFD7048616E685E90B1246B7F' },
    @{ Relative='Effect\flare03.OZJ'; Hash='922F9FFBE6559C7176EA6767460A584A9252BF8F414ED45D57946192DDAA6A63' },
    @{ Relative='Effect\wave.OZJ'; Hash='AB0030FCE7EF3CCC7AF4145AED1A073E04EB4C23ADAFA8AA9AC3A11BC7E409F9' },
    @{ Relative='Effect\mzine_irondefence.OZJ'; Hash='637DAFE79CDA267FC484C9DB50FA1F356274F27D8627A00917A846C08C5358F5' },
    @{ Relative='Effect\ground_star.OZJ'; Hash='00FD18620AF56137C1EBDBF18DECD32AB93AC2529A46FECE011A1482039C7E4C' },
    @{ Relative='Effect\Swordeff_mono2line.OZJ'; Hash='33FA72691D4EA39F7D30F9D6D2285CEC3C91C16D4F5525999DA142A1D6E2EF4D' },
    @{ Relative='Effect\smoke01_strong.OZJ'; Hash='20D63B6AF959B008F7EE257E0D0727963AA34739DDE24FDE9E003F53B2E6C303' },
    @{ Relative='Effect\pin_star02_red.OZJ'; Hash='4E285498D44A466773224901D0910201FF97E56D49C215958D59999792BF06DA' },
    @{ Relative='Effect\monolines02.OZJ'; Hash='0CDFC3266E6ECBD037182652BF2FA79B2812F496605325636CE5496AFED02586' },
    @{ Relative='Effect\sand01.OZJ'; Hash='F3FA56466F79217E68D5296338507C738A4222C7FE11A2ED39F2A08794B59A94' },
    @{ Relative='Effect\flamestan_mono.OZJ'; Hash='17BFC1EF72743177FFCC9F2501CF2F4E07A0686CEC8C45810D60988162942B76' },
    @{ Relative='Effect\arrow_damge.OZJ'; Hash='923BE2238E33DDC7059618CC87EB7C732CD1DA265F98BEAB3DB058CFB4676546' },
    @{ Relative='Effect\empact01.OZJ'; Hash='BBBF1FB4EDD1CB2492674CD321A8B0C240CA4A68131772924FD60749046628D7' },
    @{ Relative='Effect\enemy_ring02.OZJ'; Hash='2C2E8069C5188889E52D6FED36086F56902EACB987092F545B47E21B590F753B' },
    @{ Relative='Effect\lightmarks.ozj'; Hash='DC4E83DDB76554298ED9619FEFB02DE7E0D3A210DB8424B97CB1244E62B4305F' },
    @{ Relative='Effect\Spark04.OZJ'; Hash='0DD591222B3BF927A40A1DBAE1463FD0A7A831B47B5CC72AAF455831AABCC578' },
    @{ Relative='Effect\bostar3_R.OZJ'; Hash='271C44638380457D72C2E8D5D083406CDDFEECC04C9EA3EBFD674264D80D38EB' },
    @{ Relative='Effect\cratered.OZT'; Hash='FCD69FB99BE1B70FCA71B976053CBDE8AAD3FBF84AF277B80C64FE4DC104C0C4' },
    @{ Relative='Effect\wind_foce.bmd'; Hash='36AA09EBB030D4791DB9472450A960BDB3F7591527ECDAF031F0F56E6E2F039F' },
    @{ Relative='Skill\Nova_arrow_sim02.bmd'; Hash='86BDF3E99D2BD9EEB157926D74186392027335BC831574A1E533062DFB39DE7A' },
    @{ Relative='Skill\Magum_line.bmd'; Hash='FE309BCA331CE601841A85C8292AE875C3D45E0B171AE4F148264684CE2F48B3' },
    @{ Relative='Effect\Shorkwave_gun01.bmd'; Hash='334BB0265B0C8BF76FF388C8BB9E37B588E14726247A20A99351D792054F1AE6' },
    @{ Relative='Effect\01spa_skill.bmd'; Hash='B8D0766ECB4EE7217935667C521EAB226B61C243EA7F71FCBC3C1CA5083899E6' },
    @{ Relative='Effect\flareRed.OZJ'; Hash='FC7B772D1B8685B5B89989838431616D19224E321A1492CC95F9B5369B794A48' },
    @{ Relative='Effect\Shockwave2.OZJ'; Hash='F47DC1F04D0DDCA6DC50E8EA09D44F430EB4B80D2B2C52D44BEC3BC8D846FEA2' },
    @{ Relative='Effect\Steam_01.OZJ'; Hash='FAB9B6510BD3FD242E7227C8572F9AB10BB087D3DB665E25E18DB72C8FC0FAE0' },
    @{ Relative='Effect\smokelines03.OZJ'; Hash='8CAFF0978BDC9418F5B62AEB744A8C228D95443CB04CFB9DB2371820F3C599DA' },
    @{ Relative='Effect\flare_mono.OZJ'; Hash='EAB40E848BCCD356C9A3A92BC4BFE66B148FD64A76E00168CF8B68E3A19300F5' },
    @{ Relative='Effect\Impack03.OZJ'; Hash='22F9F2481C6B7CB6CE4A4D674E72A35D72DC20D20B850674B3C06AE3BE84019D' },
    @{ Relative='Effect\lighting_mega03.OZJ'; Hash='3C4435F499AAA4877B2506FFDC871EE1482EE0D4CDDF881B3F0A49FEFB071B9F' },
    @{ Relative='Effect\Combo4.OZJ'; Hash='389780DFB08902E299DF9289F829349E58200B0B5BF7847AADFADCBA687B0FDA' },
    @{ Relative='Effect\ring_of_gradation.OZJ'; Hash='2E3FAD479009A7686C51CB2719B52CE0FED452C494A9105B946C01CB3B20C284' },
    @{ Relative='Effect\Shiny02.OZJ'; Hash='B728FB665EEBE3068B450DCACE28A70F352A126535AAC5A50CB5A168F722BBD6' },
    @{ Relative='Effect\hikorora.OZJ'; Hash='6114115D787CDC9B77CA0F7B5CA0FB4E30B356866BED5D5ED57B0A316767AAEA' },
    @{ Relative='Skill\twlighthik01.OZJ'; Hash='6BE0AE4DC839CD40C6B63AA1CC74847E33326CE441194714440EA93FB1DC7A8E' },
    @{ Relative='Skill\twlighthik02.OZJ'; Hash='9872B87E5A87A965B61A3865F217A5973C39FB58D54E12A7858F560FD9C04B69' },
    @{ Relative='Effect\Flare.OZJ'; Hash='5778EA278113B8A1E33905EB8519A782CF3D141FF78229C66219E368EC99647B' },
    @{ Relative='Effect\flare01.OZJ'; Hash='874B708AA0CF304EFC3BACCE089FEC9FD69CC934E24F378E21655124FCFD7AF8' },
    @{ Relative='Effect\02_buff_obj.bmd'; Hash='B19969D05CDF2C423ABB7787B61BA5B7CC525AF997224A1ABF9450F72DA25614' },
    @{ Relative='Effect\03_buff_obj.bmd'; Hash='42057A53DDD3F220B499FD805E6282ED7A4790AEFEA97E43C442EC14E999C9BA' },
    @{ Relative='Effect\05_buff_obj.bmd'; Hash='AFBFB0162AC6A35FF403501597BA69DD7DE5C4B38F3847BE58B3D9AFE1A7CABA' },
    @{ Relative='Skill\lightofbird_attack.bmd'; Hash='8A347D4263248FADBCA6A32FFA4475492674542AC5F828B857B0263A49FD5F41' },
    @{ Relative='Skill\dragonhead.bmd'; Hash='80694ACF42C65E5741D50F9A1B8F5F8B5E619B515438EEEF146B6D342DE9CB15' },
    @{ Relative='Effect\3cha_Kskill.bmd'; Hash='3A62098CC01196A18D89951ED65A51DCBE60347806A6585F8863B7A965432EC3' },
    @{ Relative='Skill\sharke_arrows.OZJ'; Hash='E6A67678AB7D395C2693ED415F6F6C0F8DD1047C24C87C755873E255CEC6DEA1' },
    @{ Relative='Effect\!Combo3.OZJ'; Hash='FBB0EE033D39BA7576714EF96C4A8244CA14480545809FE98147117DFEEAD984' },
    @{ Relative='Skill\grandmark2.OZJ'; Hash='D1B51C66F24899594FE66FEDFC6246A8E7B292A1773937876E37682ACBA9594C' },
    @{ Relative='Skill\lines2.OZJ'; Hash='79D2A20143B15E406344F43DCDAF6232DD131986BC28603B10BF70DA4F208F7A' },
    @{ Relative='Effect\grandmark2.OZJ'; Hash='D1B51C66F24899594FE66FEDFC6246A8E7B292A1773937876E37682ACBA9594C' },
    @{ Relative='Effect\lines2.OZJ'; Hash='79D2A20143B15E406344F43DCDAF6232DD131986BC28603B10BF70DA4F208F7A' },
    @{ Relative='Skill\effect_arrow_shot01.OZJ'; Hash='5D87F648657367EF99BE365D233A3C2CB261F0BCE58D9126361DF1BD162C7F26' },
    @{ Relative='Skill\magum_arrows.OZJ'; Hash='BA3146A69356FC1561593A2AD9058812B139B27D389868958D0A8AFE7E838222' },
    @{ Relative='Skill\blur02_mono_long.OZJ'; Hash='201E8FD9B161621AA21B9858F3EF00FA25BF4994989F8F9507F0444326E44156' },
    @{ Relative='Skill\monolines.OZJ'; Hash='7981E9EC00F0CCC30826EE52D0CBE4028159516C2C176F490E3FEB6DB776DF9F' },
    @{ Relative='Item\Wings\alpha_line2mono2.OZJ'; Hash='E5F26A2B35FAD0B77B8E2E207DCA50AF84B18115595DA81B5701586A52DCE7CE' },
    @{ Relative='Effect\SwordEff2_blue.OZJ'; Hash='0FA842ABD9CD4587C81AB5528B7DA6662AA3E0246AB8E2FAF43BA277DD25133A' },
    @{ Relative='Effect\line_of_big_R.OZJ'; Hash='784D5A9E2B88696BC03B89117956ABEA0D4D1464D8D8513B0CE5B5E910E6D511' },
    @{ Relative='Effect\sharke_arrows_blue.OZJ'; Hash='3A6967CF59A5EBAD936CD5DBB80D87A144F94269B9D2495C56C5556ED33D23B4' },
    @{ Relative='Effect\sharke_arrows_yellow.OZJ'; Hash='40B7C427E8945CE584E9F31B4789D6A3602B159AB79A9A61CCB3211D470205C4' },
    @{ Relative='Effect\sharke_arrows_red.OZJ'; Hash='A4380366C6E8E04B0262AB7BE0D563C3E2DE1205E1E649EF9119524C415D0E39' },
    @{ Relative='Effect\sharke_arrows_puple.OZJ'; Hash='CDA56AC0A70A169AB41C64F5968BDE2DC1B01B3867210819087E4436614D29B3' },
    @{ Relative='Effect\sharke_arrows_geen.OZJ'; Hash='3C852D33E1F348E8BD2FCCA6315B5C12E31AB2C0A9A98C5174F3AB5FA79C8DF4' },
    @{ Relative='Skill\sdKDAttackBird01_R.OZJ'; Hash='CC9615460CB62BF31E79FD34F44F612C77702A87A1A110833C6942D7A2249ECF' },
    @{ Relative='Skill\drhead.OZJ'; Hash='FF296B66F867EB0B70B3848E710FF713C09D3F545F095474983696852E544988' },
    @{ Relative='Effect\ConmocionLance.OZJ'; Hash='DDF68FD34B05AE5C14E6DCB3407A6B52E47B90B616F989F86C15A2B0986B71CB' }
    @{ Relative='Effect\wind01.OZJ'; Hash='1F5EE56564C85CDD4F759CA9DA3BA911F2F1CAADF5FC3B57385EC4BB5E19D0A1' }
    @{ Relative='Effect\smoke01.OZJ'; Hash='05D93565B3E9DC6ADE98A4F6C50BEC676E7E205CE1CFD464B7173359B397DA58' }
    @{ Relative='Effect\pin_star03.OZJ'; Hash='2C8D577F6FCBD71B34736DCFFB108EF485C393F427CA4A882232833BC1908B94' }
    @{ Relative='Effect\force_Pillar2.OZJ'; Hash='5525FCDC5293C9F6088DF4C9EC1943185D772C0F33CB95575F434772D688DA66' }
    @{ Relative='Effect\flareBlue.OZJ'; Hash='671351ECD8CF0C34211865EED815E1CC76114CAA0CDE6A95F705B20812B19825' }
    @{ Relative='Effect\ElementalCharge\flareBlue.OZJ'; Hash='E261B59F57FDF0C514917A388F02AC66B832C2BE9F97A3F90BC36C98D7B2FB32' }
    @{ Relative='Effect\Flare_yellow.OZJ'; Hash='5778EA278113B8A1E33905EB8519A782CF3D141FF78229C66219E368EC99647B' }
    @{ Relative='Effect\Flare_puple.OZJ'; Hash='69E416FCB3B25B59B5A56B1DE44D716A8F91E3D65CC62C127AAEE5AC9AA5D93B' }
    @{ Relative='Effect\Flare_green.OZJ'; Hash='D8B1DDDDB608954666E0FB81C47B9452C4490F9FBE19CCB02D4923F870F9BF75' }
    @{ Relative='Effect\Magic_Ground2.OZJ'; Hash='2BF23AC3EFF0B01E43FA88CE20CFFB175B082F98F4F22A2C970F18CC4FA465F1' }
    @{ Relative='Effect\Smoke_line.OZJ'; Hash='DDBF444F8D46332312225D05F0185A5CA42A3B1CE7DC03DD3E5B5896CD3D4070' }
    @{ Relative='Effect\smoke_line03.OZJ'; Hash='F45CE9FEB2278F55AE1C539AADE2738D5E5348FCE061EAB7DA00D994E49D58E1' }
    @{ Relative='Effect\blur02_long_magum.OZJ'; Hash='410DC92A60F4BEDBA437187CB57667A6E4E2D7D5D052AA5011A68015A4991097' }
    @{ Relative='Effect\blur02_mono_long_magum_black.OZJ'; Hash='9E00E165A768F8FED4F788E4622C839E9F36F874BB9D946CB7448AB415501497' }
    @{ Relative='Effect\alpha_line2mono3.OZJ'; Hash='0FEE13AF541849A3E91E276D1760BB2FDCEB38A557030A2C669F9B69CC2C54A5' }
    @{ Relative='Effect\ground_crack_light02.OZJ'; Hash='0CEB35563B48B843076BF170D8055B46F7A0848E490D080357230D51C4FA6ABD' }
    @{ Relative='Effect\somasi01mono_R.OZJ'; Hash='E1FEDD955C0C71707EC9DFEF7FC95FB86E2B0EEC16874C98E108F77D9FDDA578' }
    @{ Relative='Effect\ad001.OZJ'; Hash='A1FA23BF9E14511B78A25EEE9EA6CB7207EB8EA4A829B0D1400652F7C727D6F8' }
)
foreach ($entry in $recoveredCandidates) {
    $sourcePath = [IO.Path]::GetFullPath((Join-Path 'D:\Pegasus\Data' $entry.Relative))
    $targetPath = [IO.Path]::GetFullPath((Join-Path $recoveredTarget $entry.Relative))
    if (-not $sourcePath.StartsWith('D:\Pegasus\Data\', [StringComparison]::OrdinalIgnoreCase) -or
        -not $targetPath.StartsWith($recoveredTarget + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Recovered asset path escapes isolated roots'
    }
    if ((Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Recovered source hash mismatch: $sourcePath"
    }
    if ((Test-Path -LiteralPath $targetPath) -and
        (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Refusing to replace changed recovered asset: $targetPath"
    }
    $operations += [pscustomobject]@{ Source=$sourcePath; Target=$targetPath; Hash=$entry.Hash }
}
$embeddedModelDependencies = @(
    # Shorkwave_gun01.bmd is loaded from Recovered\Data\Effect, therefore
    # Open2 resolves this embedded texture beside that BMD.  The source file
    # happens to live under Pegasus Item\Wings; preserve the bytes, not the
    # unrelated source directory layout.
    @{ Source='D:\Pegasus\Data\Item\Wings\alpha_line2mono2.OZJ'; Relative='Effect\alpha_line2mono2.OZJ'; Hash='E5F26A2B35FAD0B77B8E2E207DCA50AF84B18115595DA81B5701586A52DCE7CE' },
    # wind_foce.bmd embeds ground_wind.jpg and is also loaded from Effect.
    @{ Source='D:\Pegasus\Data\Effect\ground_wind.OZJ'; Relative='Effect\ground_wind.OZJ'; Hash='8FB62BE821614F63C046C5BEECE31386B9A9C56D91CF71DA0D52EACEF5DDC4C8' }
)
foreach ($entry in $embeddedModelDependencies) {
    $sourcePath = [IO.Path]::GetFullPath($entry.Source)
    $targetPath = [IO.Path]::GetFullPath((Join-Path $recoveredTarget $entry.Relative))
    if (-not $sourcePath.StartsWith('D:\Pegasus\Data\', [StringComparison]::OrdinalIgnoreCase) -or
        -not $targetPath.StartsWith($recoveredTarget + '\', [StringComparison]::OrdinalIgnoreCase)) {
        throw 'Embedded model dependency escapes isolated roots'
    }
    if ((Get-FileHash -LiteralPath $sourcePath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Embedded dependency source hash mismatch: $sourcePath"
    }
    if ((Test-Path -LiteralPath $targetPath) -and
        (Get-FileHash -LiteralPath $targetPath -Algorithm SHA256).Hash -ne $entry.Hash) {
        throw "Refusing to replace changed embedded dependency: $targetPath"
    }
    $operations += [pscustomobject]@{ Source=$sourcePath; Target=$targetPath; Hash=$entry.Hash }
}
foreach ($operation in $operations) {
    New-Item -ItemType Directory -Path ([IO.Path]::GetDirectoryName($operation.Target)) -Force | Out-Null
    if (-not (Test-Path -LiteralPath $operation.Target)) {
        Copy-Item -LiteralPath $operation.Source -Destination $operation.Target
    }
    if ((Get-FileHash -LiteralPath $operation.Target -Algorithm SHA256).Hash -ne $operation.Hash) {
        throw "Copied asset verification failed: $($operation.Target)"
    }
}
[void](& py -3 (Join-Path $PSScriptRoot 'VerifyPegasusBmdDependencies.py') $recoveredTarget)
if ($LASTEXITCODE -ne 0) {
    throw 'Pegasus BMD embedded-texture dependency verification failed'
}
$skillIconTarget = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot 'PegasusBuild\Client\Data\RISE\Pegasus\UI\Skill'))
[void](& py -3 (Join-Path $PSScriptRoot 'StagePegasusSkillIcons.py') $skillIconTarget)
if ($LASTEXITCODE -ne 0) {
    throw 'Pegasus skill icon staging failed'
}
[pscustomobject]@{ Status='PASS'; Files=($operations.Count + 14); SwordBlow=$targetRoot; SwordWrath=$swordWrathTarget; Sound=$soundTarget; Recovered=$recoveredTarget; SkillIcons=$skillIconTarget }
