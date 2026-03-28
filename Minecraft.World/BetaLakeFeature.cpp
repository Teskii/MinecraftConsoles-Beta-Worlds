#include "stdafx.h"

#include "BetaLegacyPlacement.h"
#include "BetaLakeFeature.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.material.h"
#include "net.minecraft.world.level.tile.h"

BetaLakeFeature::BetaLakeFeature(int tile)
{
    this->tile = tile;
}

bool BetaLakeFeature::place(Level *level, Random *random, int x, int y, int z)
{
    x -= 8;
    z -= 8;

    while (y > 0 && level->isEmptyTile(x, y, z))
    {
        --y;
    }

    y -= 4;

    bool lakeShape[16 * 16 * 8] = { false };
    int spots = random->nextInt(4) + 4;

    for (int i = 0; i < spots; ++i)
    {
        double xr = random->nextDouble() * 6.0 + 3.0;
        double yr = random->nextDouble() * 4.0 + 2.0;
        double zr = random->nextDouble() * 6.0 + 3.0;
        double xp = random->nextDouble() * (16.0 - xr - 2.0) + 1.0 + xr / 2.0;
        double yp = random->nextDouble() * (8.0 - yr - 4.0) + 2.0 + yr / 2.0;
        double zp = random->nextDouble() * (16.0 - zr - 2.0) + 1.0 + zr / 2.0;

        for (int xx = 1; xx < 15; ++xx)
        {
            for (int zz = 1; zz < 15; ++zz)
            {
                for (int yy = 1; yy < 7; ++yy)
                {
                    double xd = (static_cast<double>(xx) - xp) / (xr / 2.0);
                    double yd = (static_cast<double>(yy) - yp) / (yr / 2.0);
                    double zd = (static_cast<double>(zz) - zp) / (zr / 2.0);
                    double dist = xd * xd + yd * yd + zd * zd;

                    if (dist < 1.0)
                    {
                        lakeShape[(xx * 16 + zz) * 8 + yy] = true;
                    }
                }
            }
        }
    }

    for (int xx = 0; xx < 16; ++xx)
    {
        for (int zz = 0; zz < 16; ++zz)
        {
            for (int yy = 0; yy < 8; ++yy)
            {
                bool edge = !lakeShape[(xx * 16 + zz) * 8 + yy] &&
                    ((xx < 15 && lakeShape[((xx + 1) * 16 + zz) * 8 + yy]) ||
                     (xx > 0 && lakeShape[((xx - 1) * 16 + zz) * 8 + yy]) ||
                     (zz < 15 && lakeShape[(xx * 16 + (zz + 1)) * 8 + yy]) ||
                     (zz > 0 && lakeShape[(xx * 16 + (zz - 1)) * 8 + yy]) ||
                     (yy < 7 && lakeShape[(xx * 16 + zz) * 8 + (yy + 1)]) ||
                     (yy > 0 && lakeShape[(xx * 16 + zz) * 8 + (yy - 1)]));

                if (!edge)
                {
                    continue;
                }

                const int tileId = level->getTile(x + xx, y + yy, z + zz);
                Material *material = level->getMaterial(x + xx, y + yy, z + zz);
                if (yy >= 4 && material->isLiquid())
                {
                    return false;
                }

                if (yy < 4 && !BetaLegacyPlacement::IsBuildable(tileId) && tileId != tile)
                {
                    return false;
                }
            }
        }
    }

    for (int xx = 0; xx < 16; ++xx)
    {
        for (int zz = 0; zz < 16; ++zz)
        {
            for (int yy = 0; yy < 8; ++yy)
            {
                if (!lakeShape[(xx * 16 + zz) * 8 + yy])
                {
                    continue;
                }

                level->setTileAndData(x + xx, y + yy, z + zz, yy >= 4 ? 0 : tile, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
            }
        }
    }

    for (int xx = 0; xx < 16; ++xx)
    {
        for (int zz = 0; zz < 16; ++zz)
        {
            for (int yy = 4; yy < 8; ++yy)
            {
                if (!lakeShape[(xx * 16 + zz) * 8 + yy])
                {
                    continue;
                }

                if (level->getTile(x + xx, y + yy - 1, z + zz) == Tile::dirt_Id &&
                    level->getBrightness(LightLayer::Sky, x + xx, y + yy, z + zz) > 0)
                {
                    level->setTileAndData(x + xx, y + yy - 1, z + zz, Tile::grass_Id, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
                }
            }
        }
    }

    if (Tile::tiles[tile]->material == Material::lava)
    {
        for (int xx = 0; xx < 16; ++xx)
        {
            for (int zz = 0; zz < 16; ++zz)
            {
                for (int yy = 0; yy < 8; ++yy)
                {
                    bool edge = !lakeShape[(xx * 16 + zz) * 8 + yy] &&
                        ((xx < 15 && lakeShape[((xx + 1) * 16 + zz) * 8 + yy]) ||
                         (xx > 0 && lakeShape[((xx - 1) * 16 + zz) * 8 + yy]) ||
                         (zz < 15 && lakeShape[(xx * 16 + (zz + 1)) * 8 + yy]) ||
                         (zz > 0 && lakeShape[(xx * 16 + (zz - 1)) * 8 + yy]) ||
                         (yy < 7 && lakeShape[(xx * 16 + zz) * 8 + (yy + 1)]) ||
                         (yy > 0 && lakeShape[(xx * 16 + zz) * 8 + (yy - 1)]));

                    if (!edge)
                    {
                        continue;
                    }

                    if ((yy < 4 || random->nextInt(2) != 0) &&
                        BetaLegacyPlacement::IsBuildable(level->getTile(x + xx, y + yy, z + zz)))
                    {
                        level->setTileAndData(x + xx, y + yy, z + zz, Tile::stone_Id, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
                    }
                }
            }
        }
    }

    return true;
}
