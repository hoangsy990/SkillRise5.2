$ErrorActionPreference = 'Stop'
$taskRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$taskBuild = Join-Path $taskRoot 'ExGameServer\Tests\GrowLancerBuild'
$vcPath = 'C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\14.51.36231'
$sdkPath = 'C:\Program Files (x86)\Windows Kits\10'
foreach ($objectName in @('MemScript.obj','stdafx.obj')) {
    if (!(Test-Path -LiteralPath (Join-Path $taskBuild "Obj\$objectName"))) {
        throw 'Build isolated Ex603 first; real tokenizer objects are required'
    }
}
Push-Location $taskRoot
try {
    & "$vcPath\bin\Hostx64\x86\cl.exe" /nologo /std:c++17 /EHsc /MT /W4 /WX `
      "/I$vcPath\include" "/I$sdkPath\Include\10.0.26100.0\ucrt" `
      "/I$sdkPath\Include\10.0.26100.0\um" "/I$sdkPath\Include\10.0.26100.0\shared" `
      ExGameServer/Tests/GrowLancerMemScriptColumnsTest.cpp `
      "$taskBuild\Obj\MemScript.obj" "$taskBuild\Obj\stdafx.obj" `
      "/Fo$taskBuild\GrowLancerMemScriptColumnsTest.obj" "/Fe$taskBuild\GrowLancerMemScriptColumnsTest.exe" `
      /link /LTCG "/LIBPATH:$vcPath\lib\x86" "/LIBPATH:$vcPath\atlmfc\lib\x86" `
      "/LIBPATH:$sdkPath\Lib\10.0.26100.0\ucrt\x86" "/LIBPATH:$sdkPath\Lib\10.0.26100.0\um\x86" kernel32.lib
    if ($LASTEXITCODE) { throw 'Tokenizer harness build failed' }
    & "$taskBuild\GrowLancerMemScriptColumnsTest.exe" ExGameServer/Tests/fixtures/legacy_class_columns.txt
    if ($LASTEXITCODE) { throw 'Minimal tokenizer regression failed' }
    & "$taskBuild\GrowLancerMemScriptColumnsTest.exe" ExGameServer/Tests/fixtures/legacy_full_skill_rows.txt full-skill
    if ($LASTEXITCODE) { throw 'Full skill row regression failed' }
} finally { Pop-Location }
