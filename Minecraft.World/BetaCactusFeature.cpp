#include "stdafx.h"

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaCactusFeature.h"
#include "BetaLegacyPlacement.h"

BetaCactusFeature::BetaCactusFeature(bool doUpdate) : Feature(doUpdate)
{
}

bool BetaCactusFeature::place(Level *level, Random *random, int x, int y, int z)
{
    for (int i = 0; i < 10; ++i)
    {
        int xx = x + random->nextInt(8) - random->nextInt(8);
        int yy = y + random->nextInt(4) - random->nextInt(4);
        int zz = z + random->nextInt(8) - random->nextInt(8);

        if (level->getTile(xx, yy, zz) != 0)
        {
            continue;
        }

        int height = 1 + random->nextInt(random->nextInt(3) + 1);
        for (int offsetY = 0; offsetY < height; ++offsetY)
        {
            if (BetaLegacyPlacement::CanPlaceCactus(level, xx, yy + offsetY, zz))
            {
                level->setTileAndData(xx, yy + offsetY, zz, Tile::cactus_Id, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
            }
        }
    }

    return true;
}
