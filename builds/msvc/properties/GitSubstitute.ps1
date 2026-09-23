# GitSubstitute.ps1
# Substitutes @TOKEN@ placeholders in a template (.in) file using values passed from MSBuild.
# Intended to be invoked by GitSubstitution.props.

[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string] $InputPath,

    [Parameter(Mandatory = $true)]
    [string] $OutputPath,

    [Parameter(Mandatory = $true)]
    [string] $VersionMajorToken,

    [Parameter(Mandatory = $true)]
    [string] $VersionMinorToken,

    [Parameter(Mandatory = $true)]
    [string] $VersionPatchToken,

    [Parameter(Mandatory = $true)]
    [string] $GitCommitHashToken,

    [Parameter(Mandatory = $true)]
    [string] $GitOriginUrlToken,

    [Parameter(Mandatory = $true)]
    [string] $GitDirtyToken,

    [Parameter(Mandatory = $true)]
    [string] $VersionMajorValue,

    [Parameter(Mandatory = $true)]
    [string] $VersionMinorValue,

    [Parameter(Mandatory = $true)]
    [string] $VersionPatchValue,

    [Parameter(Mandatory = $true)]
    [AllowEmptyString()]
    [string] $GitCommitHash,

    [Parameter(Mandatory = $true)]
    [AllowEmptyString()]
    [string] $GitOriginUrl,

    [Parameter(Mandatory = $true)]
    [string] $GitDirty
)

$ErrorActionPreference = 'Stop'

function Escape-RegexLiteral {
    param([string] $Text)
    return [regex]::Escape($Text)
}

if (-not (Test-Path -LiteralPath $InputPath)) {
    throw "Input template not found: $InputPath"
}

$outDir = Split-Path -Parent $OutputPath
if ($outDir -and -not (Test-Path -LiteralPath $outDir)) {
    New-Item -ItemType Directory -Path $outDir -Force | Out-Null
}

$content = Get-Content -LiteralPath $InputPath -Raw

# -replace treats the pattern as regex; escape tokens so '@...@' is matched literally.
$replacements = @(
    @{ Token = $VersionMajorToken;   Value = $VersionMajorValue }
    @{ Token = $VersionMinorToken;   Value = $VersionMinorValue }
    @{ Token = $VersionPatchToken;   Value = $VersionPatchValue }
    @{ Token = $GitCommitHashToken;  Value = $GitCommitHash }
    @{ Token = $GitOriginUrlToken;   Value = $GitOriginUrl }
    @{ Token = $GitDirtyToken;       Value = $GitDirty }
)

foreach ($r in $replacements) {
    $pattern = '@' + (Escape-RegexLiteral $r.Token) + '@'
    # Replacement string: use MatchEvaluator so $ and other specials in values stay literal.
    $content = [regex]::Replace($content, $pattern, { param($m) $r.Value })
}

# -NoNewline matches the previous inline script behavior.
Set-Content -LiteralPath $OutputPath -Value $content -NoNewline -Encoding utf8

Write-Host "Generated: $OutputPath"
