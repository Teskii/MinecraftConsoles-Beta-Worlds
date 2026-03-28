#pragma once

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"

namespace BetaLegacyPlacement
{
inline bool IsBuildable(int tileId)
{
    if (tileId == 0)
    {
        return false;
    }

    switch (tileId)
    {
    case Tile::fire_Id:
    case Tile::water_Id:
    case Tile::calmWater_Id:
    case Tile::lava_Id:
    case Tile::calmLava_Id:
    case Tile::portalTile_Id:
    case Tile::topSnow_Id:
    case Tile::button_stone_Id:
    case Tile::button_wood_Id:
    case Tile::diode_off_Id:
    case Tile::diode_on_Id:
    case Tile::ladder_Id:
    case Tile::lever_Id:
    case Tile::rail_Id:
    case Tile::goldenRail_Id:
    case Tile::detectorRail_Id:
    case Tile::activatorRail_Id:
    case Tile::redStoneDust_Id:
    case Tile::torch_Id:
    case Tile::redstoneTorch_off_Id:
    case Tile::redstoneTorch_on_Id:
    case Tile::reeds_Id:
    case Tile::flower_Id:
    case Tile::rose_Id:
    case Tile::mushroom_red_Id:
    case Tile::mushroom_brown_Id:
    case Tile::deadBush_Id:
    case Tile::tallgrass_Id:
    case Tile::sapling_Id:
    case Tile::wheat_Id:
        return false;
    }

    return true;
}

inline bool IsReplaceable(int tileId)
{
    switch (tileId)
    {
    case 0:
    case Tile::water_Id:
    case Tile::calmWater_Id:
    case Tile::lava_Id:
    case Tile::calmLava_Id:
    case Tile::topSnow_Id:
    case Tile::fire_Id:
        return true;
    }

    return false;
}

inline bool IsOpaqueForWorldE(int tileId)
{
    switch (tileId)
    {
    case 0:
    case Tile::cactus_Id:
    case Tile::topSnow_Id:
    case Tile::ice_Id:
    case Tile::tnt_Id:
    case Tile::glass_Id:
    case Tile::stained_glass_Id:
    case Tile::leaves_Id:
        return false;
    }

    return IsBuildable(tileId);
}

inline bool CanPlaceCactus(Level *level, int x, int y, int z)
{
    if (IsBuildable(level->getTile(x - 1, y, z))) return false;
    if (IsBuildable(level->getTile(x + 1, y, z))) return false;
    if (IsBuildable(level->getTile(x, y, z - 1))) return false;
    if (IsBuildable(level->getTile(x, y, z + 1))) return false;

    const int below = level->getTile(x, y - 1, z);
    return below == Tile::cactus_Id || below == Tile::sand_Id;
}

inline bool CanPlaceReeds(Level *level, int x, int y, int z)
{
    const int below = level->getTile(x, y - 1, z);
    if (below == Tile::reeds_Id)
    {
        return true;
    }

    if (below != Tile::grass_Id && below != Tile::dirt_Id)
    {
        return false;
    }

    return level->getMaterial(x - 1, y - 1, z) == Material::water ||
           level->getMaterial(x + 1, y - 1, z) == Material::water ||
           level->getMaterial(x, y - 1, z - 1) == Material::water ||
           level->getMaterial(x, y - 1, z + 1) == Material::water;
}

inline bool CanPlacePumpkin(Level *level, int x, int y, int z)
{
    return IsReplaceable(level->getTile(x, y, z)) && IsOpaqueForWorldE(level->getTile(x, y - 1, z));
}

inline bool IsAirOrLeaves(Level *level, int x, int y, int z)
{
    const int tileId = level->getTile(x, y, z);
    return tileId == 0 || tileId == Tile::leaves_Id;
}

inline bool CanPlaceDeadBush(Level *level, int x, int y, int z)
{
    return level->getTile(x, y - 1, z) == Tile::sand_Id;
}

inline bool CanPlaceFlower(Level *level, int x, int y, int z, int tileId)
{
    if (tileId == Tile::deadBush_Id)
    {
        return CanPlaceDeadBush(level, x, y, z);
    }

    if (tileId == Tile::mushroom_red_Id || tileId == Tile::mushroom_brown_Id)
    {
        return y >= 0 &&
               y < Level::maxBuildHeight &&
               level->getRawBrightness(x, y, z) < 13 &&
               IsOpaqueForWorldE(level->getTile(x, y - 1, z));
    }

    const int below = level->getTile(x, y - 1, z);
    return below == Tile::grass_Id || below == Tile::dirt_Id || below == Tile::farmland_Id;
}
}
