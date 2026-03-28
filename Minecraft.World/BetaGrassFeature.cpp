#include "stdafx.h"

#include "BetaLegacyPlacement.h"
#include "BetaGrassFeature.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"

BetaGrassFeature::BetaGrassFeature(int data, bool doUpdate) : Feature(doUpdate)
{
    this->data = data;
}

bool BetaGrassFeature::place(Level *level, Random *random, int x, int y, int z)
{
    while (y > 0 && BetaLegacyPlacement::IsAirOrLeaves(level, x, y, z))
    {
        --y;
    }

    for (int i = 0; i < 128; ++i)
    {
        int xx = x + random->nextInt(8) - random->nextInt(8);
        int yy = y + random->nextInt(4) - random->nextInt(4);
        int zz = z + random->nextInt(8) - random->nextInt(8);

        if (level->getTile(xx, yy, zz) == 0 && BetaLegacyPlacement::CanPlaceFlower(level, xx, yy, zz, Tile::tallgrass_Id))
        {
            level->setTileAndData(xx, yy, zz, Tile::tallgrass_Id, data, Tile::UPDATE_INVISIBLE_NO_LIGHT);
        }
    }

    return true;
}
