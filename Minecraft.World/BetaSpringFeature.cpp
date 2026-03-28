#include "stdafx.h"

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaSpringFeature.h"

BetaSpringFeature::BetaSpringFeature(int tile, bool doUpdate) : Feature(doUpdate)
{
    this->tile = tile;
}

bool BetaSpringFeature::place(Level *level, Random *random, int x, int y, int z)
{
    if (level->getTile(x, y + 1, z) != Tile::stone_Id) return false;
    if (level->getTile(x, y - 1, z) != Tile::stone_Id) return false;

    int currentTile = level->getTile(x, y, z);
    if (currentTile != 0 && currentTile != Tile::stone_Id) return false;

    int stoneNeighbors = 0;
    if (level->getTile(x - 1, y, z) == Tile::stone_Id) ++stoneNeighbors;
    if (level->getTile(x + 1, y, z) == Tile::stone_Id) ++stoneNeighbors;
    if (level->getTile(x, y, z - 1) == Tile::stone_Id) ++stoneNeighbors;
    if (level->getTile(x, y, z + 1) == Tile::stone_Id) ++stoneNeighbors;

    int airNeighbors = 0;
    if (level->isEmptyTile(x - 1, y, z)) ++airNeighbors;
    if (level->isEmptyTile(x + 1, y, z)) ++airNeighbors;
    if (level->isEmptyTile(x, y, z - 1)) ++airNeighbors;
    if (level->isEmptyTile(x, y, z + 1)) ++airNeighbors;

    if (stoneNeighbors == 3 && airNeighbors == 1)
    {
        level->setTileAndUpdate(x, y, z, tile);
        level->setInstaTick(true);
        Tile::tiles[tile]->tick(level, x, y, z, random);
        level->setInstaTick(false);
    }

    return true;
}
