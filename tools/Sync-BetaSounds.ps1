param(
    [string]$BetaRoot = 'C:\Users\diego\OneDrive\Desktop\MinecraftDev\MinecraftConsoles\MinecraftConsoles-Beta-Worlds\betaAssets'
)

$ErrorActionPreference = 'Stop'

$workspaceRoot = Split-Path -Parent $PSScriptRoot
$betaSoundRoot = Join-Path $BetaRoot 'assets\minecraft\sounds'

if (-not (Test-Path $betaSoundRoot)) {
    throw "Beta sound source not found: $betaSoundRoot"
}

$targetRoots = [System.Collections.Generic.List[string]]::new()
$targetRoots.Add((Join-Path $workspaceRoot 'Minecraft.Client\Windows64Media\Sound\Minecraft'))

$buildRoot = Join-Path $workspaceRoot 'build\windows64\Minecraft.Client'
if (Test-Path $buildRoot) {
    Get-ChildItem -Path $buildRoot -Directory | ForEach-Object {
        $candidate = Join-Path $_.FullName 'Windows64Media\Sound\Minecraft'
        if (Test-Path $candidate) {
            $targetRoots.Add($candidate)
        }
    }
}

$targetRoots = $targetRoots | Select-Object -Unique

function Copy-SoundRelative {
    param(
        [Parameter(Mandatory = $true)][string]$SourcePath,
        [Parameter(Mandatory = $true)][string]$RelativeTargetPath
    )

    foreach ($targetRoot in $targetRoots) {
        $targetPath = Join-Path $targetRoot $RelativeTargetPath
        $targetDir = Split-Path -Parent $targetPath
        if (-not (Test-Path $targetDir)) {
            New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
        }

        Copy-Item -LiteralPath $SourcePath -Destination $targetPath -Force
    }
}

$copiedCount = 0

Get-ChildItem -Path $betaSoundRoot -Recurse -File -Filter '*.ogg' | ForEach-Object {
    $relativePath = $_.FullName.Substring($betaSoundRoot.Length + 1)
    Copy-SoundRelative -SourcePath $_.FullName -RelativeTargetPath $relativePath
    $script:copiedCount++
}

# Beta Java packs only expose the older footstep set under dig/*. On Win64 the game still
# looks for step.* for footstep playback, so mirror those clips into step/* to keep the
# beta set active there too while leaving sounds with no beta equivalent untouched.
$stepAliases = @(
    'cloth',
    'grass',
    'gravel',
    'sand',
    'snow',
    'stone',
    'wood'
)

$aliasedCount = 0

foreach ($material in $stepAliases) {
    $sourceDir = Join-Path $betaSoundRoot 'dig'
    if (-not (Test-Path $sourceDir)) {
        continue
    }

    Get-ChildItem -Path $sourceDir -File -Filter ($material + '*.ogg') | ForEach-Object {
        $targetRelative = "step\$material$($_.BaseName.Substring($material.Length))$($_.Extension)"
        Copy-SoundRelative -SourcePath $_.FullName -RelativeTargetPath $targetRelative
        $script:aliasedCount++
    }
}

# This codebase plays flint-and-steel and other legacy fizz actions through
# random.fizz, but the beta Java pack only ships fire/ignite. Mirror that clip
# over so beta ignition audio still plays on Win64.
$betaRandomFizz = Join-Path $betaSoundRoot 'random\fizz.ogg'
$betaFireIgnite = Join-Path $betaSoundRoot 'fire\ignite.ogg'
if (-not (Test-Path $betaRandomFizz) -and (Test-Path $betaFireIgnite)) {
    Copy-SoundRelative -SourcePath $betaFireIgnite -RelativeTargetPath 'random\fizz.ogg'
    $aliasedCount++
}

# Win64 flint-and-steel and fire charge use fire.new_ignite, which resolves to
# fire\new_ignite.ogg. The beta pack only has fire\ignite.ogg, so mirror it.
if (Test-Path $betaFireIgnite) {
    Copy-SoundRelative -SourcePath $betaFireIgnite -RelativeTargetPath 'fire\new_ignite.ogg'
    $aliasedCount++
}

# The beta pack does not supply lava ambient clips. Keep the existing Win64 lava
# sounds rather than overwriting them from some unrelated beta file.

Write-Host "Synced $copiedCount beta sound files and $aliasedCount sound aliases into $($targetRoots.Count) target root(s)."
