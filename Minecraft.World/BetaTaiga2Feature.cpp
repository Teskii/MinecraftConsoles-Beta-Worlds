#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaTaiga2Feature.h"
#include "BetaTreeSupport.h"

BetaTaiga2Feature::BetaTaiga2Feature(bool doUpdate) : Feature(doUpdate)
{
}

bool BetaTaiga2Feature::place(Level *level, Random *random, int x, int y, int z)
{
    const int treeHeight = random->nextInt(4) + 6;
    const int trunkBaseHeight = 1 + random->nextInt(2);
    const int topHeight = treeHeight - trunkBaseHeight;
    const int maxLeafRadius = 2 + random->nextInt(2);
    bool free = true;

    if (y < 1 || y + treeHeight + 1 > Level::genDepth)
    {
        return false;
    }

    for (int yy = y; yy <= y + 1 + treeHeight && free; ++yy)
    {
        int radius = (yy - y) < trunkBaseHeight ? 0 : maxLeafRadius;

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

    int currentRadius = random->nextInt(2);
    int radiusStepLimit = 1;
    int minRadius = 0;
    for (int topOffset = 0; topOffset <= topHeight; ++topOffset)
    {
        int yy = y + treeHeight - topOffset;

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

        if (currentRadius >= radiusStepLimit)
        {
            currentRadius = minRadius;
            minRadius = 1;
            ++radiusStepLimit;
            if (radiusStepLimit > maxLeafRadius)
            {
                radiusStepLimit = maxLeafRadius;
            }
        }
        else
        {
            ++currentRadius;
        }
    }

    int trunkTrim = random->nextInt(3);
    for (int yy = 0; yy < treeHeight - trunkTrim; ++yy)
    {
        int tile = level->getTile(x, y + yy, z);
        if (tile == 0 || tile == Tile::leaves_Id)
        {
            PlaceBetaWorldgenBlock(level, x, y + yy, z, Tile::treeTrunk_Id, TreeTile::DARK_TRUNK);
        }
    }

    return true;
}
