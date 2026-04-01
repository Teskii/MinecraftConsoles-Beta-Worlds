param(
    [string]$WorkspaceRoot = (Split-Path -Parent $PSScriptRoot)
)

$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName System.Drawing

function Get-Codepoints {
    param(
        [Parameter(Mandatory = $true)][string]$UIFontDataPath
    )

    $content = Get-Content -LiteralPath $UIFontDataPath -Raw
    $match = [regex]::Match(
        $content,
        'unsigned short SFontData::Codepoints\[FONTSIZE\]\s*=\s*\{(?<body>.*?)\};',
        [System.Text.RegularExpressions.RegexOptions]::Singleline
    )

    if (-not $match.Success) {
        throw "Unable to find SFontData::Codepoints in $UIFontDataPath"
    }

    $values = [regex]::Matches($match.Groups['body'].Value, '0x[0-9A-Fa-f]+')
    if ($values.Count -eq 0) {
        throw "No codepoints found in $UIFontDataPath"
    }

    $codepoints = New-Object System.Collections.Generic.List[int]
    foreach ($value in $values) {
        $codepoints.Add([Convert]::ToInt32($value.Value, 16))
    }

    return $codepoints
}

function Copy-GlyphCell {
    param(
        [Parameter(Mandatory = $true)][System.Drawing.Bitmap]$SourceBitmap,
        [Parameter(Mandatory = $true)][System.Drawing.Bitmap]$TargetBitmap,
        [Parameter(Mandatory = $true)][int]$SourceIndex,
        [Parameter(Mandatory = $true)][int]$TargetIndex,
        [Parameter(Mandatory = $true)][int]$SourceColumns,
        [Parameter(Mandatory = $true)][int]$TargetColumns,
        [int]$CellWidth = 8,
        [int]$CellHeight = 8
    )

    $srcX = ($SourceIndex % $SourceColumns) * $CellWidth
    $srcY = [Math]::Floor($SourceIndex / $SourceColumns) * $CellHeight
    $dstX = ($TargetIndex % $TargetColumns) * $CellWidth
    $dstY = [Math]::Floor($TargetIndex / $TargetColumns) * $CellHeight

    for ($y = 0; $y -lt $CellHeight; $y++) {
        for ($x = 0; $x -lt $CellWidth; $x++) {
            $pixel = $SourceBitmap.GetPixel($srcX + $x, $srcY + $y)
            $TargetBitmap.SetPixel($dstX + $x, $dstY + $y, $pixel)
        }
    }
}

$uiFontDataPath = Join-Path $WorkspaceRoot 'Minecraft.Client\Common\UI\UIFontData.cpp'
$backupFontPath = Join-Path $WorkspaceRoot 'Minecraft.Client\Common\res\TitleUpdate\res_backup_pre_tu4_default\font\Default.png'
$tu4FontPath = Join-Path $WorkspaceRoot 'tools\extracted_stfs\TU4_CONTENT_PACKAGE\res\font\Default.png'
$sourceOutputPath = Join-Path $WorkspaceRoot 'Minecraft.Client\Common\res\TitleUpdate\res\font\Default.png'

if (-not (Test-Path $backupFontPath)) {
    throw "Missing backup font: $backupFontPath"
}

if (-not (Test-Path $tu4FontPath)) {
    throw "Missing TU4 font: $tu4FontPath"
}

$targetOutputPaths = New-Object System.Collections.Generic.List[string]
$targetOutputPaths.Add($sourceOutputPath)

$buildRoot = Join-Path $WorkspaceRoot 'build\windows64\Minecraft.Client'
if (Test-Path $buildRoot) {
    Get-ChildItem -Path $buildRoot -Directory | ForEach-Object {
        $candidate = Join-Path $_.FullName 'Common\res\TitleUpdate\res\font\Default.png'
        if (Test-Path (Split-Path -Parent $candidate)) {
            $targetOutputPaths.Add($candidate)
        }
    }
}

$targetOutputPaths = $targetOutputPaths | Select-Object -Unique

$codepoints = Get-Codepoints -UIFontDataPath $uiFontDataPath
$codepointToIndex = @{}
for ($i = 0; $i -lt $codepoints.Count; $i++) {
    if (-not $codepointToIndex.ContainsKey($codepoints[$i])) {
        $codepointToIndex[$codepoints[$i]] = $i
    }
}

$baseBitmap = [System.Drawing.Bitmap]::FromFile((Resolve-Path $backupFontPath))
$tu4Bitmap = [System.Drawing.Bitmap]::FromFile((Resolve-Path $tu4FontPath))

try {
    if ($baseBitmap.Width -ne 184 -or $baseBitmap.Height -ne 160) {
        throw "Unexpected base font size: $($baseBitmap.Width)x$($baseBitmap.Height)"
    }

    if ($tu4Bitmap.Width -ne 128 -or $tu4Bitmap.Height -ne 128) {
        throw "Unexpected TU4 font size: $($tu4Bitmap.Width)x$($tu4Bitmap.Height)"
    }

    $resultBitmap = New-Object System.Drawing.Bitmap $baseBitmap.Width, $baseBitmap.Height, ([System.Drawing.Imaging.PixelFormat]::Format32bppArgb)
    $graphics = [System.Drawing.Graphics]::FromImage($resultBitmap)
    try {
        $graphics.DrawImage($baseBitmap, 0, 0, $baseBitmap.Width, $baseBitmap.Height)
    }
    finally {
        $graphics.Dispose()
    }

    for ($codepoint = 0; $codepoint -lt 256; $codepoint++) {
        if ($codepointToIndex.ContainsKey($codepoint)) {
            Copy-GlyphCell -SourceBitmap $tu4Bitmap -TargetBitmap $resultBitmap -SourceIndex $codepoint -TargetIndex $codepointToIndex[$codepoint] -SourceColumns 16 -TargetColumns 23
        }
    }

    foreach ($targetOutputPath in $targetOutputPaths) {
        $targetDir = Split-Path -Parent $targetOutputPath
        if (-not (Test-Path $targetDir)) {
            New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
        }

        $backupPath = Join-Path $targetDir 'Default.pre_beta_port.png'
        if ((Test-Path $targetOutputPath) -and -not (Test-Path $backupPath)) {
            Copy-Item -LiteralPath $targetOutputPath -Destination $backupPath -Force
        }

        $resultBitmap.Save($targetOutputPath, [System.Drawing.Imaging.ImageFormat]::Png)
        Write-Output "Wrote $targetOutputPath"
    }
}
finally {
    $baseBitmap.Dispose()
    $tu4Bitmap.Dispose()
    if ($resultBitmap) {
        $resultBitmap.Dispose()
    }
}
