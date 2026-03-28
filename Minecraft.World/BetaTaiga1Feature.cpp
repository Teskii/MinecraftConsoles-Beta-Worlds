#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaTaiga1Feature.h"
#include "BetaTreeSupport.h"

BetaTaiga1Feature::BetaTaiga1Feature(bool doUpdate) : Feature(doUpdate)
{
}

bool BetaTaiga1Feature::place(Level *level, Random *random, int x, int y, int z)
{
    const int treeHeight = random->nextInt(5) + 7;
    const int trunkHeight = treeHeight - random->nextInt(2) - 3;
    const int topHeight = treeHeight - trunkHeight;
    const int topRadius = 1 + random->nextInt(topHeight + 1);
    bool free = true;

    if (y < 1 || y + treeHeight + 1 > Level::genDepth)
    {
        return false;
    }

    for (int yy = y; yy <= y + 1 + treeHeight && free; ++yy)
    {
        int radius = (yy - y) < trunkHeight ? 0 : topRadius;

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

    int currentRadius = 0;
    for (int yy = y + treeHeight; yy >= y + trunkHeight; --yy)
    {
        for (int xx = x - currentRadius; xx <= x + currentRadius; ++xx)
        {
            int dx = xx - x;
            for (int zz = z - currentRadius; zz <= z + currentRadius; ++zz)
            {
                int dz = zz - z;
                if ((abs(dx) != currentRadius || abs(dz) != currentRadius || currentRadius <= 0) &&
                    !IsBetaTreeOpaque(level, xx, yy, zz))
                {
                    PlaceBetaWorldgenBlock(level, xx, yy, zz, Tile::leaves_Id, LeafTile::EVERGREEN_LEAF);
                }
            }
        }

        if (currentRadius >= 1 && yy == y + trunkHeight + 1)
        {
            --currentRadius;
        }
        else if (currentRadius < topRadius)
        {
            ++currentRadius;
        }
    }

    for (int yy = 0; yy < treeHeight - 1; ++yy)
    {
        int tile = level->getTile(x, y + yy, z);
        if (tile == 0 || tile == Tile::leaves_Id)
        {
            PlaceBetaWorldgenBlock(level, x, y + yy, z, Tile::treeTrunk_Id, TreeTile::DARK_TRUNK);
        }
    }

    return true;
}
