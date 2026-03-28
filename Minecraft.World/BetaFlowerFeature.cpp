#include "stdafx.h"

#include "BetaLegacyPlacement.h"
#include "BetaFlowerFeature.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"

BetaFlowerFeature::BetaFlowerFeature(int tile, int data, bool doUpdate) : Feature(doUpdate)
{
    this->tile = tile;
    this->data = data;
}

bool BetaFlowerFeature::place(Level *level, Random *random, int x, int y, int z)
{
    for (int i = 0; i < 64; ++i)
    {
        int xx = x + random->nextInt(8) - random->nextInt(8);
        int yy = y + random->nextInt(4) - random->nextInt(4);
        int zz = z + random->nextInt(8) - random->nextInt(8);

        if (level->getTile(xx, yy, zz) == 0 && BetaLegacyPlacement::CanPlaceFlower(level, xx, yy, zz, tile))
        {
            level->setTileAndData(xx, yy, zz, tile, data, Tile::UPDATE_INVISIBLE_NO_LIGHT);
        }
    }

    return true;
}
