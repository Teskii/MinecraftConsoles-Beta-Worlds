#pragma once

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"

// Mirrors LegacyUtil173.Block_o(...) closely for Beta tree generators so their
// canopy replacement rules match the Java Beta source instead of LCE defaults.
inline bool IsBetaTreeOpaque(Level *level, int x, int y, int z)
{
    int tileId = level->getTile(x, y, z);
    if (tileId == 0)
    {
        return false;
    }

    if (tileId == Tile::leaves_Id)
    {
        return false;
    }

    switch (tileId)
    {
    case Tile::bed_Id:
    case Tile::button_stone_Id:
    case Tile::button_wood_Id:
    case Tile::cactus_Id:
    case Tile::cake_Id:
    case Tile::diode_off_Id:
    case Tile::diode_on_Id:
    case Tile::door_wood_Id:
    case Tile::door_iron_Id:
    case Tile::fence_Id:
    case Tile::fire_Id:
    case Tile::flower_Id:
    case Tile::rose_Id:
    case Tile::wheat_Id:
    case Tile::deadBush_Id:
    case Tile::tallgrass_Id:
    case Tile::mushroom_red_Id:
    case Tile::mushroom_brown_Id:
    case Tile::water_Id:
    case Tile::calmWater_Id:
    case Tile::lava_Id:
    case Tile::calmLava_Id:
    case Tile::ladder_Id:
    case Tile::lever_Id:
    case Tile::rail_Id:
    case Tile::goldenRail_Id:
    case Tile::detectorRail_Id:
    case Tile::activatorRail_Id:
    case Tile::pistonBase_Id:
    case Tile::pistonStickyBase_Id:
    case Tile::pistonExtensionPiece_Id:
    case Tile::pistonMovingPiece_Id:
    case Tile::portalTile_Id:
    case Tile::pressurePlate_stone_Id:
    case Tile::pressurePlate_wood_Id:
    case Tile::weightedPlate_light_Id:
    case Tile::weightedPlate_heavy_Id:
    case Tile::redStoneDust_Id:
    case Tile::reeds_Id:
    case Tile::sign_Id:
    case Tile::wallSign_Id:
    case Tile::topSnow_Id:
    case Tile::farmland_Id:
    case Tile::stairs_wood_Id:
    case Tile::stairs_sprucewood_Id:
    case Tile::stairs_birchwood_Id:
    case Tile::stairs_junglewood_Id:
    case Tile::torch_Id:
    case Tile::redstoneTorch_off_Id:
    case Tile::redstoneTorch_on_Id:
    case Tile::trapdoor_Id:
    case Tile::web_Id:
    case Tile::glass_Id:
    case Tile::stained_glass_Id:
    case Tile::thinGlass_Id:
    case Tile::stained_glass_pane_Id:
    case Tile::ice_Id:
    case Tile::mobSpawner_Id:
        return false;
    }

    Tile *tile = Tile::tiles[tileId];
    if (tile == nullptr)
    {
        return false;
    }

    return tile->material->blocksMotion();
}

inline void PlaceBetaWorldgenBlock(Level *level, int x, int y, int z, int tile, int data)
{
    level->setTileAndData(x, y, z, tile, data, Tile::UPDATE_INVISIBLE_NO_LIGHT);
}
