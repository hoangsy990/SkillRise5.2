$ErrorActionPreference = 'Stop'
$scriptPath = Join-Path $PSScriptRoot 'AnalyzePegasusRuntimeQa.ps1'
$tokens = $null
$parseErrors = $null
$ast = [System.Management.Automation.Language.Parser]::ParseFile($scriptPath, [ref]$tokens, [ref]$parseErrors)
if ($parseErrors.Count) { throw 'Analyzer parse errors' }
$function = $ast.Find({ param($node)
    $node -is [System.Management.Automation.Language.FunctionDefinitionAst] -and
    $node.Name -eq 'Read-SharedQaLog'
}, $true)
if (-not $function) { throw 'Missing shared reader' }
. ([scriptblock]::Create($function.Extent.Text))
$fixture = Join-Path (Join-Path $PSScriptRoot 'PegasusBuild') ('qa-sharing-' + [guid]::NewGuid() + '.log')
try {
    [IO.File]::WriteAllText($fixture, "first`r`nsecond`r`n")
    $state = @{ Appended = $false }
    $rows = @(Read-SharedQaLog $fixture | ForEach-Object {
        if (-not $state.Appended) {
            # This happens while the reader is still open. The old ReadLines
            # implementation throws a sharing violation here.
            $writer = [IO.File]::Open($fixture, [IO.FileMode]::Append,
                [IO.FileAccess]::Write, [IO.FileShare]::ReadWrite)
            try {
                $bytes = [Text.Encoding]::UTF8.GetBytes("third`r`n")
                $writer.Write($bytes, 0, $bytes.Length)
            } finally { $writer.Dispose() }
            $state.Appended = $true
        }
        $_
    })
    if (-not $state.Appended -or $rows.Count -ne 3 -or $rows[2] -ne 'third') {
        throw 'Concurrent append was not preserved by the actual analyzer reader'
    }
    Write-Output 'PASS: actual QA analyzer reader permits concurrent append and preserves rows'
} finally {
    if (Test-Path -LiteralPath $fixture) { Remove-Item -LiteralPath $fixture }
}
