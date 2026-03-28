#pragma once

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"

namespace BetaVegetationSupport
{
inline bool CanPlace(Level *level, int x, int y, int z, int tile)
{
    if (y <= 0 || y >= Level::maxBuildHeight)
    {
        return false;
    }

    int below = level->getTile(x, y - 1, z);

    switch (tile)
    {
    case Tile::deadBush_Id:
        return below == Tile::sand_Id;
    case Tile::mushroom_red_Id:
    case Tile::mushroom_brown_Id:
        return below != 0 && Tile::solid[below] && level->getRawBrightness(x, y, z) < 13;
    default:
        return below == Tile::grass_Id || below == Tile::dirt_Id || below == Tile::farmland_Id;
    }
}

inline int FindGroundY(Level *level, int x, int y, int z)
{
    int tile = 0;
    while (((tile = level->getTile(x, y, z)) == 0 || tile == Tile::leaves_Id) && y > 0)
    {
        --y;
    }

    return y;
}

inline void Place(Level *level, int x, int y, int z, int tile, int data)
{
    level->setTileAndData(x, y, z, tile, data, Tile::UPDATE_INVISIBLE_NO_LIGHT);
}
}
