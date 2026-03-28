#include "stdafx.h"

#include "BetaDeadBushFeature.h"
#include "BetaLegacyPlacement.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"

BetaDeadBushFeature::BetaDeadBushFeature(int tile, bool doUpdate) : Feature(doUpdate)
{
    this->tile = tile;
}

bool BetaDeadBushFeature::place(Level *level, Random *random, int x, int y, int z)
{
    while (y > 0 && BetaLegacyPlacement::IsAirOrLeaves(level, x, y, z))
    {
        --y;
    }

    for (int i = 0; i < 4; ++i)
    {
        int xx = x + random->nextInt(8) - random->nextInt(8);
        int yy = y + random->nextInt(4) - random->nextInt(4);
        int zz = z + random->nextInt(8) - random->nextInt(8);

        if (level->getTile(xx, yy, zz) == 0 && BetaLegacyPlacement::CanPlaceDeadBush(level, xx, yy, zz))
        {
            level->setTileAndData(xx, yy, zz, tile, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
        }
    }

    return true;
}
