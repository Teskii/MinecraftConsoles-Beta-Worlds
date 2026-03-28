#include "stdafx.h"

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.material.h"
#include "net.minecraft.world.level.tile.h"
#include "Mth.h"
#include "BetaClayFeature.h"

BetaClayFeature::BetaClayFeature(int count)
{
    this->tile = Tile::clay_Id;
    this->count = count;
}

bool BetaClayFeature::place(Level *level, Random *random, int x, int y, int z)
{
    if (level->getMaterial(x, y, z) != Material::water)
    {
        return false;
    }

    float angle = random->nextFloat() * PI;
    double x0 = static_cast<double>(static_cast<float>(x + 8) + Mth::sin(angle) * static_cast<float>(count) / 8.0f);
    double x1 = static_cast<double>(static_cast<float>(x + 8) - Mth::sin(angle) * static_cast<float>(count) / 8.0f);
    double z0 = static_cast<double>(static_cast<float>(z + 8) + Mth::cos(angle) * static_cast<float>(count) / 8.0f);
    double z1 = static_cast<double>(static_cast<float>(z + 8) - Mth::cos(angle) * static_cast<float>(count) / 8.0f);
    double y0 = static_cast<double>(y + random->nextInt(3) + 2);
    double y1 = static_cast<double>(y + random->nextInt(3) + 2);

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

        for (int clayX = minX; clayX <= maxX; ++clayX)
        {
            double normX = (static_cast<double>(clayX) + 0.5 - centerX) / (radiusXZ / 2.0);
            if (normX * normX >= 1.0)
            {
                continue;
            }

            for (int clayY = minY; clayY <= maxY; ++clayY)
            {
                double normY = (static_cast<double>(clayY) + 0.5 - centerY) / (radiusY / 2.0);
                if (normX * normX + normY * normY >= 1.0)
                {
                    continue;
                }

                for (int clayZ = minZ; clayZ <= maxZ; ++clayZ)
                {
                    double normZ = (static_cast<double>(clayZ) + 0.5 - centerZ) / (radiusXZ / 2.0);
                    if (normX * normX + normY * normY + normZ * normZ >= 1.0)
                    {
                        continue;
                    }

                    if (level->getTile(clayX, clayY, clayZ) == Tile::sand_Id)
                    {
                        level->setTileAndData(clayX, clayY, clayZ, tile, 0, Tile::UPDATE_INVISIBLE_NO_LIGHT);
                    }
                }
            }
        }
    }

    return true;
}
