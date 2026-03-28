#include "stdafx.h"

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaPumpkinFeature.h"
#include "BetaLegacyPlacement.h"

BetaPumpkinFeature::BetaPumpkinFeature(bool doUpdate) : Feature(doUpdate)
{
}

bool BetaPumpkinFeature::place(Level *level, Random *random, int x, int y, int z)
{
    for (int i = 0; i < 64; ++i)
    {
        int xx = x + random->nextInt(8) - random->nextInt(8);
        int yy = y + random->nextInt(4) - random->nextInt(4);
        int zz = z + random->nextInt(8) - random->nextInt(8);

        if (!level->isEmptyTile(xx, yy, zz) || level->getTile(xx, yy - 1, zz) != Tile::grass_Id)
        {
            continue;
        }

        if (BetaLegacyPlacement::CanPlacePumpkin(level, xx, yy, zz))
        {
            level->setTileAndData(xx, yy, zz, Tile::pumpkin_Id, random->nextInt(4), Tile::UPDATE_INVISIBLE_NO_LIGHT);
        }
    }

    return true;
}
