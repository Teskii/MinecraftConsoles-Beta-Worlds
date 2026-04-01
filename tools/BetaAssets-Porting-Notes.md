# Beta Assets Porting Notes

Source pack:

- `C:\Users\diego\OneDrive\Desktop\MinecraftDev\MinecraftConsoles\MinecraftConsoles-Beta-Worlds\betaAssets`

What is in it:

- Java resource-pack layout under `assets\minecraft`
- `textures\blocks` with 58 files
- `textures\items` with 29 files
- `textures\entity` with 9 files
- `textures\environment`, `textures\gui`, `sounds`

Most useful folders for this project:

- `assets\minecraft\textures\blocks`
- `assets\minecraft\textures\items`
- `assets\minecraft\textures\entity`
- `assets\minecraft\textures\environment`
- `assets\minecraft\textures\gui`
- `assets\minecraft\sounds`

Folders that are likely Java-only or not directly portable:

- `assets\minecraft\models`
- `assets\minecraft\optifine`
- `assets\minecraft\mcpatcher`

Important porting detail:

- This console project does not use the Java pack layout directly for most block and item art.
- World block art is mostly driven by the console `terrain.png` atlas and its mipmaps in `Minecraft.Client\Common\res\TitleUpdate\res`.
- Item art is mostly driven by `gui\items.png`.
- Some folder-pack support exists for loose `res\textures\blocks` and `res\textures\items`, but that is best for staged testing, not final default-asset replacement.

Recommended workflow:

1. Use `betaAssets` as the source of truth for beta visuals.
2. For blocks and items, map Java loose textures to the console atlas slots in `PreStitchedTextureMap.cpp`.
3. For entity, environment, and gui files, compare the Java path against the project's loose `res` layout and copy only the matching files.
4. For sounds, treat them as a separate pass after visuals, since the current project audio path is less direct than the texture path.

Immediate candidates to port from this source:

- block textures that still look too modern
- item textures that are still post-beta
- sky, clouds, panorama, and gui textures
- classic/beta sound replacements
