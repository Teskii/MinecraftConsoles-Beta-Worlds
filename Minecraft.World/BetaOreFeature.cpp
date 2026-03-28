#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "Mth.h"
#include "BetaOreFeature.h"

void BetaOreFeature::_init(int tile, int count, int targetTile)
{
    this->tile = tile;
    this->count = count;
    this->targetTile = targetTile;
}

BetaOreFeature::BetaOreFeature(int tile, int count)
{
    _init(tile, count, Tile::stone_Id);
}

BetaOreFeature::BetaOreFeature(int tile, int count, int targetTile)
{
    _init(tile, count, targetTile);
}

bool BetaOreFeature::place(Level *level, Random *random, int x, int y, int z)
{
    float angle = random->nextFloat() * PI;
    double x0 = x + 8 + Mth::sin(angle) * static_cast<float>(count) / 8.0f;
    double x1 = x + 8 - Mth::sin(angle) * static_cast<float>(count) / 8.0f;
    double z0 = z + 8 + Mth::cos(angle) * static_cast<float>(count) / 8.0f;
    double z1 = z + 8 - Mth::cos(angle) * static_cast<float>(count) / 8.0f;
    double y0 = y + random->nextInt(3) + 2;
    double y1 = y + random->nextInt(3) + 2;

    for (int i = 0; i <= count; ++i)
    {
        double centerX = x0 + (x1 - x0) * static_cast<double>(i) / static_cast<double>(count);
        double centerY = y0 + (y1 - y0) * static_cast<double>(i) / static_cast<double>(count);
        double centerZ = z0 + (z1 - z0) * static_cast<double>(i) / static_cast<double>(count);
        double size = random->nextDouble() * static_cast<double>(count) / 16.0;
        double radiusXZ = (Mth::sin(static_cast<float>(i) * PI / static_cast<float>(count)) + 1.0f) * size + 1.0;
        double radiusY = (Mth::sin(static_cast<float>(i) * PI / static_cast<float>(count)) + 1.0f) * size + 1.0;

        int minX = Mth::floor(centerX - radiusXZ / 2.0);
        int minY = Mth::floor(centerY - radiusY / 2.0);
        int minZ = Mth::floor(centerZ - radiusXZ / 2.0);
        int maxX = Mth::floor(centerX + radiusXZ / 2.0);
        int maxY = Mth::floor(centerY + radiusY / 2.0);
        int maxZ = Mth::floor(centerZ + radiusXZ / 2.0);

        for (int oreX = minX; oreX <= maxX; ++oreX)
        {
            double normX = (static_cast<double>(oreX) + 0.5 - centerX) / (radiusXZ / 2.0);
            if (normX * normX >= 1.0)
            {
                continue;
            }

            for (int oreY = minY; oreY <= maxY; ++oreY)
            {
                double normY = (static_cast<double>(oreY) + 0.5 - centerY) / (radiusY / 2.0);
                if (normX * normX + normY * normY >= 1.0)
                {
                    continue;
                }

                for (int oreZ = minZ; oreZ <= maxZ; ++oreZ)
                {
                    double normZ = (static_cast<double>(oreZ) + 0.5 - centerZ) / (radiusXZ / 2.0);
                    if (normX * normX + normY * normY + normZ * normZ >= 1.0)
                    {
                        continue;
                    }

                    if (level->getTile(oreX, oreY, oreZ) == targetTile)
                    {
                        level->setTileAndData(oreX, oreY, oreZ, tile, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
                    }
                }
            }
        }
    }

    return true;
}
