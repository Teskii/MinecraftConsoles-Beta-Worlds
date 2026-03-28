#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaForestFeature.h"
#include "BetaTreeSupport.h"

BetaForestFeature::BetaForestFeature(bool doUpdate) : Feature(doUpdate)
{
}

bool BetaForestFeature::place(Level *level, Random *random, int x, int y, int z)
{
    const int treeHeight = random->nextInt(3) + 5;
    bool free = true;

    if (y < 1 || y + treeHeight + 1 > Level::genDepth)
    {
        return false;
    }

    for (int yy = y; yy <= y + 1 + treeHeight && free; ++yy)
    {
        int radius = 1;
        if (yy == y)
        {
            radius = 0;
        }
        if (yy >= y + treeHeight - 1)
        {
            radius = 2;
        }

        for (int xx = x - radius; xx <= x + radius && free; ++xx)
        {
            for (int zz = z - radius; zz <= z + radius && free; ++zz)
            {
                if (yy >= 0 && yy < Level::genDepth)
                {
                    int tile = level->getTile(xx, yy, zz);
                    if (tile != 0 && tile != Tile::leaves_Id)
                    {
                        free = false;
                    }
                }
                else
                {
                    free = false;
                }
            }
        }
    }

    if (!free)
    {
        return false;
    }

    int belowTile = level->getTile(x, y - 1, z);
    if ((belowTile != Tile::grass_Id && belowTile != Tile::dirt_Id) || y >= Level::genDepth - treeHeight - 1)
    {
        return false;
    }

    PlaceBetaWorldgenBlock(level, x, y - 1, z, Tile::dirt_Id, 0);

    for (int yy = y - 3 + treeHeight; yy <= y + treeHeight; ++yy)
    {
        int canopyY = yy - (y + treeHeight);
        int radius = 1 - canopyY / 2;

        for (int xx = x - radius; xx <= x + radius; ++xx)
        {
            int dx = xx - x;
            for (int zz = z - radius; zz <= z + radius; ++zz)
            {
                int dz = zz - z;
                if ((abs(dx) != radius || abs(dz) != radius || (random->nextInt(2) != 0 && canopyY != 0)) &&
                    !IsBetaTreeOpaque(level, xx, yy, zz))
                {
                    PlaceBetaWorldgenBlock(level, xx, yy, zz, Tile::leaves_Id, LeafTile::BIRCH_LEAF);
                }
            }
        }
    }

    for (int yy = 0; yy < treeHeight; ++yy)
    {
        int tile = level->getTile(x, y + yy, z);
        if (tile == 0 || tile == Tile::leaves_Id)
        {
            PlaceBetaWorldgenBlock(level, x, y + yy, z, Tile::treeTrunk_Id, TreeTile::BIRCH_TRUNK);
        }
    }

    return true;
}
