param(
    [string]$BetaRoot = 'C:\Users\diego\OneDrive\Desktop\MinecraftDev\MinecraftConsoles\MinecraftConsoles-Beta-Worlds\betaAssets'
)

$ErrorActionPreference = 'Stop'

$workspaceRoot = Split-Path -Parent $PSScriptRoot
$betaTextureRoot = Join-Path $BetaRoot 'assets\minecraft\textures'
$sourceResRoot = Join-Path $workspaceRoot 'Minecraft.Client\Common\res\TitleUpdate\res'

$targetRoots = [System.Collections.Generic.List[string]]::new()
$targetRoots.Add($sourceResRoot)

$buildRoot = Join-Path $workspaceRoot 'build\windows64\Minecraft.Client'
if (Test-Path $buildRoot) {
    Get-ChildItem -Path $buildRoot -Directory | ForEach-Object {
        $candidate = Join-Path $_.FullName 'Common\res\TitleUpdate\res'
        if (Test-Path $candidate) {
            $targetRoots.Add($candidate)
        }
    }
}

$targetRoots = $targetRoots | Select-Object -Unique

function Copy-BetaTexture {
    param(
        [Parameter(Mandatory = $true)][string]$SourceRelativePath,
        [Parameter(Mandatory = $true)][string[]]$TargetRelativePaths
    )

    $sourcePath = Join-Path $betaTextureRoot $SourceRelativePath
    if (-not (Test-Path $sourcePath)) {
        Write-Warning "Missing beta source: $SourceRelativePath"
        return
    }

    foreach ($targetRoot in $targetRoots) {
        foreach ($targetRelativePath in $TargetRelativePaths) {
            $targetPath = Join-Path $targetRoot $targetRelativePath
            $targetDir = Split-Path -Parent $targetPath
            if (-not (Test-Path $targetDir)) {
                New-Item -ItemType Directory -Path $targetDir -Force | Out-Null
            }

            Copy-Item -LiteralPath $sourcePath -Destination $targetPath -Force
        }
    }
}

$blockMap = @(
    @{ Source = 'blocks\brick.png'; Target = 'textures\blocks\brick.png' },
    @{ Source = 'blocks\cobblestone.png'; Target = 'textures\blocks\cobblestone.png' },
    @{ Source = 'blocks\cobblestone_mossy.png'; Target = 'textures\blocks\cobblestone_mossy.png' },
    @{ Source = 'blocks\diamond_block.png'; Target = 'textures\blocks\diamond_block.png' },
    @{ Source = 'blocks\dirt.png'; Target = 'textures\blocks\dirt.png' },
    @{ Source = 'blocks\dispenser_front_horizontal.png'; Target = 'textures\blocks\dispenser_front.png' },
    @{ Source = 'blocks\enchanting_table_bottom.png'; Target = 'textures\blocks\enchanting_table_bottom.png' },
    @{ Source = 'blocks\enchanting_table_side.png'; Target = 'textures\blocks\enchanting_table_side.png' },
    @{ Source = 'blocks\endframe_side.png'; Target = 'textures\blocks\endframe_side.png' },
    @{ Source = 'blocks\end_stone.png'; Target = 'textures\blocks\end_stone.png' },
    @{ Source = 'blocks\flower_rose.png'; Target = 'textures\blocks\flower_rose.png' },
    @{ Source = 'blocks\glowstone.png'; Target = 'textures\blocks\glowstone.png' },
    @{ Source = 'blocks\gold_block.png'; Target = 'textures\blocks\gold_block.png' },
    @{ Source = 'blocks\grass_side.png'; Target = 'textures\blocks\grass_side.png' },
    @{ Source = 'blocks\grass_top.png'; Target = 'textures\blocks\grass_top.png' },
    @{ Source = 'blocks\gravel.png'; Target = 'textures\blocks\gravel.png' },
    @{ Source = 'blocks\iron_block.png'; Target = 'textures\blocks\iron_block.png' },
    @{ Source = 'blocks\lapis_block.png'; Target = 'textures\blocks\lapis_block.png' },
    @{ Source = 'blocks\lava_flow.png'; Target = 'textures\blocks\lava_flow.png' },
    @{ Source = 'blocks\lava_still.png'; Target = 'textures\blocks\lava.png' },
    @{ Source = 'blocks\log_birch_top.png'; Target = 'textures\blocks\log_birch_top.png' },
    @{ Source = 'blocks\log_jungle_top.png'; Target = 'textures\blocks\log_jungle_top.png' },
    @{ Source = 'blocks\log_oak_top.png'; Target = 'textures\blocks\log_oak_top.png' },
    @{ Source = 'blocks\log_spruce_top.png'; Target = 'textures\blocks\log_spruce_top.png' },
    @{ Source = 'blocks\mob_spawner.png'; Target = 'textures\blocks\mob_spawner.png' },
    @{ Source = 'blocks\netherrack.png'; Target = 'textures\blocks\netherrack.png' },
    @{ Source = 'blocks\planks_oak.png'; Target = 'textures\blocks\planks_oak.png' },
    @{ Source = 'blocks\reeds.png'; Target = 'textures\blocks\reeds.png' },
    @{ Source = 'blocks\repeater_off.png'; Target = 'textures\blocks\repeater_off.png' },
    @{ Source = 'blocks\repeater_on.png'; Target = 'textures\blocks\repeater_on.png' },
    @{ Source = 'blocks\sand.png'; Target = 'textures\blocks\sand.png' },
    @{ Source = 'blocks\sapling_oak.png'; Target = 'textures\blocks\sapling.png' },
    @{ Source = 'blocks\sponge.png'; Target = 'textures\blocks\sponge.png' },
    @{ Source = 'blocks\sponge_wet.png'; Target = 'textures\blocks\sponge_wet.png' },
    @{ Source = 'blocks\stone.png'; Target = 'textures\blocks\stone.png' },
    @{ Source = 'blocks\water_flow.png'; Target = 'textures\blocks\water_flow.png' },
    @{ Source = 'blocks\water_still.png'; Target = 'textures\blocks\water.png' },
    @{ Source = 'blocks\wool_colored_black.png'; Target = 'textures\blocks\wool_colored_black.png' },
    @{ Source = 'blocks\wool_colored_blue.png'; Target = 'textures\blocks\wool_colored_blue.png' },
    @{ Source = 'blocks\wool_colored_brown.png'; Target = 'textures\blocks\wool_colored_brown.png' },
    @{ Source = 'blocks\wool_colored_cyan.png'; Target = 'textures\blocks\wool_colored_cyan.png' },
    @{ Source = 'blocks\wool_colored_gray.png'; Target = 'textures\blocks\wool_colored_gray.png' },
    @{ Source = 'blocks\wool_colored_green.png'; Target = 'textures\blocks\wool_colored_green.png' },
    @{ Source = 'blocks\wool_colored_light_blue.png'; Target = 'textures\blocks\wool_colored_light_blue.png' },
    @{ Source = 'blocks\wool_colored_lime.png'; Target = 'textures\blocks\wool_colored_lime.png' },
    @{ Source = 'blocks\wool_colored_magenta.png'; Target = 'textures\blocks\wool_colored_magenta.png' },
    @{ Source = 'blocks\wool_colored_orange.png'; Target = 'textures\blocks\wool_colored_orange.png' },
    @{ Source = 'blocks\wool_colored_pink.png'; Target = 'textures\blocks\wool_colored_pink.png' },
    @{ Source = 'blocks\wool_colored_purple.png'; Target = 'textures\blocks\wool_colored_purple.png' },
    @{ Source = 'blocks\wool_colored_red.png'; Target = 'textures\blocks\wool_colored_red.png' },
    @{ Source = 'blocks\wool_colored_silver.png'; Target = 'textures\blocks\wool_colored_silver.png' },
    @{ Source = 'blocks\wool_colored_yellow.png'; Target = 'textures\blocks\wool_colored_yellow.png' }
)

$itemMap = @(
    @{ Source = 'items\apple.png'; Target = 'textures\items\apple.png' },
    @{ Source = 'items\beef_cooked.png'; Target = 'textures\items\beefCooked.png' },
    @{ Source = 'items\beef_raw.png'; Target = 'textures\items\beefRaw.png' },
    @{ Source = 'items\bread.png'; Target = 'textures\items\bread.png' },
    @{ Source = 'items\carrot.png'; Target = 'textures\items\carrots.png' },
    @{ Source = 'items\carrot_golden.png'; Target = 'textures\items\carrotGolden.png' },
    @{ Source = 'items\charcoal.png'; Target = 'textures\items\charcoal.png' },
    @{ Source = 'items\chicken_cooked.png'; Target = 'textures\items\chickenCooked.png' },
    @{ Source = 'items\chicken_raw.png'; Target = 'textures\items\chickenRaw.png' },
    @{ Source = 'items\dye_powder_brown.png'; Target = 'textures\items\dyePowder_brown.png' },
    @{ Source = 'items\fireball.png'; Target = 'textures\items\fireball.png' },
    @{ Source = 'items\fishing_rod_cast.png'; Target = 'textures\items\fishingRod_cast.png' },
    @{ Source = 'items\leather_boots.png'; Target = 'textures\items\bootsCloth.png' },
    @{ Source = 'items\leather_boots_overlay.png'; Target = 'textures\items\bootsCloth_overlay.png' },
    @{ Source = 'items\leather_chestplate.png'; Target = 'textures\items\chestplateCloth.png' },
    @{ Source = 'items\leather_chestplate_overlay.png'; Target = 'textures\items\chestplateCloth_overlay.png' },
    @{ Source = 'items\leather_helmet.png'; Target = 'textures\items\helmetCloth.png' },
    @{ Source = 'items\leather_helmet_overlay.png'; Target = 'textures\items\helmetCloth_overlay.png' },
    @{ Source = 'items\leather_leggings.png'; Target = 'textures\items\leggingsCloth.png' },
    @{ Source = 'items\leather_leggings_overlay.png'; Target = 'textures\items\leggingsCloth_overlay.png' },
    @{ Source = 'items\melon.png'; Target = 'textures\items\melon.png' },
    @{ Source = 'items\melon_speckled.png'; Target = 'textures\items\speckledMelon.png' },
    @{ Source = 'items\porkchop_cooked.png'; Target = 'textures\items\porkchopCooked.png' },
    @{ Source = 'items\porkchop_raw.png'; Target = 'textures\items\porkchopRaw.png' },
    @{ Source = 'items\potato.png'; Target = 'textures\items\potato.png' },
    @{ Source = 'items\potato_baked.png'; Target = 'textures\items\potatoBaked.png' },
    @{ Source = 'items\potato_poisonous.png'; Target = 'textures\items\potatoPoisonous.png' },
    @{ Source = 'items\pumpkin_pie.png'; Target = 'textures\items\pumpkinPie.png' }
)

$directCopies = @(
    @{
        Source = 'environment\moon.png'
        Targets = @('terrain\moon.png', '1_2_2\terrain\moon.png')
    },
    @{
        Source = 'environment\moon_phases.png'
        Targets = @('terrain\moon_phases.png', '1_2_2\terrain\moon_phases.png')
    },
    @{
        Source = 'environment\sun.png'
        Targets = @('terrain\sun.png', '1_2_2\terrain\sun.png')
    },
    @{
        Source = 'entity\explosion.png'
        Targets = @('misc\explosion.png', '1_2_2\misc\explosion.png')
    },
    @{
        Source = 'entity\pig\pig.png'
        Targets = @('mob\pig.png', '1_2_2\mob\pig.png')
    },
    @{
        Source = 'entity\chest\normal.png'
        Targets = @('1_2_2\item\chest.png')
    },
    @{
        Source = 'entity\chest\normal_double.png'
        Targets = @('1_2_2\item\largechest.png')
    },
    @{
        Source = 'entity\chest\ender.png'
        Targets = @('item\enderchest.png')
    },
    @{
        Source = 'entity\chest\trapped.png'
        Targets = @('item\trapped.png')
    },
    @{
        Source = 'entity\chest\trapped_double.png'
        Targets = @('item\trapped_double.png')
    },
    @{
        Source = 'gui\title\mojang.png'
        Targets = @('1_2_2\title\mojang.png')
    },
    @{
        Source = 'gui\title\background\panorama_0.png'
        Targets = @('1_2_2\title\bg\panorama0.png')
    },
    @{
        Source = 'gui\title\background\panorama_1.png'
        Targets = @('1_2_2\title\bg\panorama1.png')
    },
    @{
        Source = 'gui\title\background\panorama_2.png'
        Targets = @('1_2_2\title\bg\panorama2.png')
    },
    @{
        Source = 'gui\title\background\panorama_3.png'
        Targets = @('1_2_2\title\bg\panorama3.png')
    },
    @{
        Source = 'gui\title\background\panorama_4.png'
        Targets = @('1_2_2\title\bg\panorama4.png')
    },
    @{
        Source = 'gui\title\background\panorama_5.png'
        Targets = @('1_2_2\title\bg\panorama5.png')
    }
)

foreach ($entry in $blockMap) {
    Copy-BetaTexture -SourceRelativePath $entry.Source -TargetRelativePaths @($entry.Target)
}

foreach ($entry in $itemMap) {
    Copy-BetaTexture -SourceRelativePath $entry.Source -TargetRelativePaths @($entry.Target)
}

foreach ($entry in $directCopies) {
    Copy-BetaTexture -SourceRelativePath $entry.Source -TargetRelativePaths $entry.Targets
}

Write-Host "Synced beta assets into $($targetRoots.Count) target root(s)."
