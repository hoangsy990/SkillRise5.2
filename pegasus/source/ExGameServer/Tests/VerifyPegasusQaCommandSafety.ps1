$ErrorActionPreference = 'Stop'

$sourcePath = Join-Path $PSScriptRoot '..\GameServer\CommandManager.cpp'
$source = Get-Content -LiteralPath $sourcePath -Raw
if (-not $source.Contains('QA15 SERVER bird-heading=target-v1 compiled=') -or
    -not $source.Contains('(char*)"%s", kPegasusQaServerIdentity')) {
    throw 'QA15 must announce its server build/capability declaration to the caller'
}

function Require-Text {
    param(
        [string]$Text,
        [string]$Pattern,
        [string]$Description
    )

    if ($Text.IndexOf($Pattern, [System.StringComparison]::Ordinal) -lt 0) {
        throw "Missing QA15 safety contract: $Description"
    }
}

$processorStart = $source.IndexOf('void ProcessPegasusRuntimeQa(LPOBJ lpObj)', [System.StringComparison]::Ordinal)
$processorEnd = $source.IndexOf('CCommandManager gCommandManager;', $processorStart, [System.StringComparison]::Ordinal)
if ($processorStart -lt 0 -or $processorEnd -le $processorStart) {
    throw 'Unable to isolate ProcessPegasusRuntimeQa source block.'
}
$processor = $source.Substring($processorStart, $processorEnd - $processorStart)

$commandStart = $source.IndexOf('if (_stricmp(command, "/qa15") == 0)', [System.StringComparison]::Ordinal)
$commandEnd = $source.IndexOf('if (_stricmp(command, "/testwindsoul") == 0)', $commandStart, [System.StringComparison]::Ordinal)
if ($commandStart -lt 0 -or $commandEnd -le $commandStart) {
    throw 'Unable to isolate /qa15 command source block.'
}
$command = $source.Substring($commandStart, $commandEnd - $commandStart)

Require-Text $processor 'const int addResult = AddIsolatedTestSkill(lpObj, skill);' 'each runtime step provisions only its current skill'
Require-Text $command '[PegasusQA15] START_REQUEST' 'start-request diagnostic log'
Require-Text $command 'kPegasusQa15BuildStamp' 'runtime-visible isolated server build stamp'
Require-Text $command '[PegasusQA15] PREFLIGHT result=FAIL reason=SAFE_ZONE' 'safe-zone preflight diagnostic log'
Require-Text $command 'QA15 requires a position outside the safe zone.' 'safe-zone preflight owner notice'
Require-Text $command '[PegasusQA15] TARGET_CHECK result=NONE action=SPAWN' 'missing-target diagnostic log'
Require-Text $command '[PegasusQA15] SPAWN_RESULT result=FAIL' 'spawn-failure diagnostic log'
Require-Text $command '[PegasusQA15] SPAWN_RESULT result=PASS' 'spawn-success diagnostic log'
Require-Text $command '[PegasusQA15] TARGET_CHECK result=PASS' 'existing-target diagnostic log'
Require-Text $command 'GetPegasusRuntimeQaTarget' 'QA-only target selector'
Require-Text $source 'kPegasusQaReactionExcludedMonsterClass = 275' 'native no-shock monster exclusion'
Require-Text $source 'gObj[index].Class == kPegasusQaReactionExcludedMonsterClass' 'reaction exclusion enforced during selection'
Require-Text $source 'kPegasusQaSpawnMonsterClass = 0' 'reaction-capable QA spawn class'
Require-Text $processor 'target_class=%d' 'selected target class runtime evidence'

if ($command -match 'AddIsolatedTestSkill\s*\(') {
    throw '/qa15 command start must not burst add-skill packets; provisioning belongs to the timed step processor.'
}
if ($command -match 'for\s*\([^)]*kRuntimeQaSkillCount') {
    throw '/qa15 command start must not loop across the complete QA catalog.'
}

Write-Host 'PASS: QA15 command starts safely and provisions one skill per timed sample'
