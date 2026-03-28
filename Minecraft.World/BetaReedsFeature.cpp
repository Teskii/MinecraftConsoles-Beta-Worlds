#include "stdafx.h"

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.material.h"
#include "net.minecraft.world.level.tile.h"
#include "BetaReedsFeature.h"
#include "BetaLegacyPlacement.h"

BetaReedsFeature::BetaReedsFeature(bool doUpdate) : Feature(doUpdate)
{
}

bool BetaReedsFeature::place(Level *level, Random *random, int x, int y, int z)
{
    for (int i = 0; i < 20; ++i)
    {
        int xx = x + random->nextInt(4) - random->nextInt(4);
        int yy = y;
        int zz = z + random->nextInt(4) - random->nextInt(4);

        if (!level->isEmptyTile(xx, yy, zz))
        {
            continue;
        }

        if (level->getMaterial(xx - 1, yy - 1, zz) != Material::water &&
            level->getMaterial(xx + 1, yy - 1, zz) != Material::water &&
            level->getMaterial(xx, yy - 1, zz - 1) != Material::water &&
            level->getMaterial(xx, yy - 1, zz + 1) != Material::water)
        {
            continue;
        }

        int height = 2 + random->nextInt(random->nextInt(3) + 1);
        for (int offsetY = 0; offsetY < height; ++offsetY)
        {
            if (BetaLegacyPlacement::CanPlaceReeds(level, xx, yy + offsetY, zz))
            {
                level->setTileAndData(xx, yy + offsetY, zz, Tile::reeds_Id, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
            }
        }
    }

    return true;
}
