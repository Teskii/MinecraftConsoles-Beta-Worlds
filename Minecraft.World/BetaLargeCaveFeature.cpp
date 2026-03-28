#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.tile.h"
#include "Mth.h"
#include "BetaLargeCaveFeature.h"

namespace
{
    static const int BETA_CAVE_WORLD_HEIGHT = 128;
    static const int BETA_CAVE_MAX_Y = 120;

    static inline float BetaNextFloat(JavaRand *random)
    {
        return random_next(random, 24) / static_cast<float>(1 << 24);
    }

    static inline int64_t BetaNextLong(JavaRand *random)
    {
        return static_cast<int64_t>(random_next_long(random));
    }

    static inline int BetaChunkIndex(int x, int z, int y)
    {
        return (x * 16 + z) * BETA_CAVE_WORLD_HEIGHT + y;
    }

    static inline int64_t WrapJavaLongMulAddXor(int x, int64_t xScale, int z, int64_t zScale, int64_t worldSeed)
    {
        const uint64_t xPart = static_cast<uint64_t>(static_cast<int64_t>(x)) * static_cast<uint64_t>(xScale);
        const uint64_t zPart = static_cast<uint64_t>(static_cast<int64_t>(z)) * static_cast<uint64_t>(zScale);
        return static_cast<int64_t>((xPart + zPart) ^ static_cast<uint64_t>(worldSeed));
    }
}

void BetaLargeCaveFeature::apply(ChunkSource *chunkSource, Level *level, int xOffs, int zOffs, byteArray blocks)
{
    this->level = level;
    const int range = radius;
    const int64_t worldSeed = level->getSeed();

    m_betaRandom = get_java_random(worldSeed);
    int64_t xScale = BetaNextLong(&m_betaRandom) / 2LL * 2LL + 1LL;
    int64_t zScale = BetaNextLong(&m_betaRandom) / 2LL * 2LL + 1LL;

    for (int x = xOffs - range; x <= xOffs + range; ++x)
    {
        for (int z = zOffs - range; z <= zOffs + range; ++z)
        {
            m_betaRandom = get_java_random(WrapJavaLongMulAddXor(x, xScale, z, zScale, worldSeed));
            addFeature(level, x, z, xOffs, zOffs, blocks);
        }
    }
}

void BetaLargeCaveFeature::addRoom(int xOffs, int zOffs, byteArray blocks, double xRoom, double yRoom, double zRoom)
{
    addTunnel(xOffs, zOffs, blocks, xRoom, yRoom, zRoom, 1.0f + BetaNextFloat(&m_betaRandom) * 6.0f, 0.0f, 0.0f, -1, -1, 0.5);
}

void BetaLargeCaveFeature::addTunnel(int xOffs, int zOffs, byteArray blocks, double xCave, double yCave, double zCave,
                                     float thickness, float yRot, float xRot, int step, int dist, double yScale)
{
    const double xMid = static_cast<double>(xOffs * 16 + 8);
    const double zMid = static_cast<double>(zOffs * 16 + 8);
    float yRotAdd = 0.0f;
    float xRotAdd = 0.0f;
    JavaRand tunnelRandom = get_java_random(BetaNextLong(&m_betaRandom));

    if (dist <= 0)
    {
        int maxDistance = radius * 16 - 16;
        dist = maxDistance - random_next_int(&tunnelRandom, maxDistance / 4);
    }

    bool singleBranch = false;
    if (step == -1)
    {
        step = dist / 2;
        singleBranch = true;
    }

    const int splitPoint = random_next_int(&tunnelRandom, dist / 2) + dist / 4;
    const bool steep = random_next_int(&tunnelRandom, 6) == 0;

    for (; step < dist; ++step)
    {
        const double radiusXZ = 1.5 + static_cast<double>(Mth::sin(static_cast<float>(step) * 3.1415927f / static_cast<float>(dist)) * thickness);
        const double radiusY = radiusXZ * yScale;
        const float cosPitch = Mth::cos(xRot);
        const float sinPitch = Mth::sin(xRot);

        xCave += static_cast<double>(Mth::cos(yRot) * cosPitch);
        yCave += static_cast<double>(sinPitch);
        zCave += static_cast<double>(Mth::sin(yRot) * cosPitch);

        xRot *= steep ? 0.92f : 0.7f;
        xRot += xRotAdd * 0.1f;
        yRot += yRotAdd * 0.1f;
        xRotAdd *= 0.9f;
        yRotAdd *= 0.75f;
        xRotAdd += (BetaNextFloat(&tunnelRandom) - BetaNextFloat(&tunnelRandom)) * BetaNextFloat(&tunnelRandom) * 2.0f;
        yRotAdd += (BetaNextFloat(&tunnelRandom) - BetaNextFloat(&tunnelRandom)) * BetaNextFloat(&tunnelRandom) * 4.0f;

        if (!singleBranch && step == splitPoint && thickness > 1.0f)
        {
            addTunnel(xOffs, zOffs, blocks, xCave, yCave, zCave,
                      BetaNextFloat(&tunnelRandom) * 0.5f + 0.5f, yRot - 1.5707964f, xRot / 3.0f, step, dist, 1.0);
            addTunnel(xOffs, zOffs, blocks, xCave, yCave, zCave,
                      BetaNextFloat(&tunnelRandom) * 0.5f + 0.5f, yRot + 1.5707964f, xRot / 3.0f, step, dist, 1.0);
            return;
        }

        if (singleBranch || random_next_int(&tunnelRandom, 4) != 0)
        {
            const double xDist = xCave - xMid;
            const double zDist = zCave - zMid;
            const double remainingDistance = static_cast<double>(dist - step);
            const double maxReach = static_cast<double>(thickness + 2.0f + 16.0f);
            if (xDist * xDist + zDist * zDist - remainingDistance * remainingDistance > maxReach * maxReach)
            {
                return;
            }

            if (xCave >= xMid - 16.0 - radiusXZ * 2.0 && zCave >= zMid - 16.0 - radiusXZ * 2.0 &&
                xCave <= xMid + 16.0 + radiusXZ * 2.0 && zCave <= zMid + 16.0 + radiusXZ * 2.0)
            {
                int minX = Mth::floor(xCave - radiusXZ) - xOffs * 16 - 1;
                int maxX = Mth::floor(xCave + radiusXZ) - xOffs * 16 + 1;
                int minY = Mth::floor(yCave - radiusY) - 1;
                int maxY = Mth::floor(yCave + radiusY) + 1;
                int minZ = Mth::floor(zCave - radiusXZ) - zOffs * 16 - 1;
                int maxZ = Mth::floor(zCave + radiusXZ) - zOffs * 16 + 1;

                if (minX < 0) minX = 0;
                if (maxX > 16) maxX = 16;
                if (minY < 1) minY = 1;
                if (maxY > BETA_CAVE_MAX_Y) maxY = BETA_CAVE_MAX_Y;
                if (minZ < 0) minZ = 0;
                if (maxZ > 16) maxZ = 16;

                bool detectedWater = false;
                for (int x = minX; !detectedWater && x < maxX; ++x)
                {
                    for (int z = minZ; !detectedWater && z < maxZ; ++z)
                    {
                        for (int y = maxY + 1; !detectedWater && y >= minY - 1; --y)
                        {
                            const int blockIndex = BetaChunkIndex(x, z, y);
                            if (y >= 0 && y < BETA_CAVE_WORLD_HEIGHT)
                            {
                                const int tile = blocks[blockIndex];
                                if (tile == Tile::water_Id || tile == Tile::calmWater_Id)
                                {
                                    detectedWater = true;
                                }

                                if (y != minY - 1 && x != minX && x != maxX - 1 && z != minZ && z != maxZ - 1)
                                {
                                    y = minY;
                                }
                            }
                        }
                    }
                }

                if (!detectedWater)
                {
                    for (int x = minX; x < maxX; ++x)
                    {
                        const double xNorm = (static_cast<double>(x + xOffs * 16) + 0.5 - xCave) / radiusXZ;

                        for (int z = minZ; z < maxZ; ++z)
                        {
                            const double zNorm = (static_cast<double>(z + zOffs * 16) + 0.5 - zCave) / radiusXZ;
                            int blockIndex = BetaChunkIndex(x, z, maxY);
                            bool restoreGrass = false;

                            if (xNorm * xNorm + zNorm * zNorm < 1.0)
                            {
                                for (int y = maxY - 1; y >= minY; --y)
                                {
                                    const double yNorm = (static_cast<double>(y) + 0.5 - yCave) / radiusY;

                                    if (yNorm > -0.7 && xNorm * xNorm + yNorm * yNorm + zNorm * zNorm < 1.0)
                                    {
                                        const int tile = blocks[blockIndex];
                                        if (tile == Tile::grass_Id)
                                        {
                                            restoreGrass = true;
                                        }

                                        if (tile == Tile::stone_Id || tile == Tile::dirt_Id || tile == Tile::grass_Id)
                                        {
                                            if (y < 10)
                                            {
                                                blocks[blockIndex] = static_cast<byte>(Tile::calmLava_Id);
                                            }
                                            else
                                            {
                                                blocks[blockIndex] = 0;
                                                if (restoreGrass && blocks[blockIndex - 1] == Tile::dirt_Id)
                                                {
                                                    blocks[blockIndex - 1] = static_cast<byte>(Tile::grass_Id);
                                                }
                                            }
                                        }
                                    }

                                    --blockIndex;
                                }
                            }
                        }
                    }

                    if (singleBranch)
                    {
                        break;
                    }
                }
            }
        }
    }
}

void BetaLargeCaveFeature::addFeature(Level *level, int x, int z, int xOffs, int zOffs, byteArray blocks)
{
    int caveCount = random_next_int(&m_betaRandom, random_next_int(&m_betaRandom, random_next_int(&m_betaRandom, 40) + 1) + 1);
    if (random_next_int(&m_betaRandom, 15) != 0)
    {
        caveCount = 0;
    }

    for (int caveIndex = 0; caveIndex < caveCount; ++caveIndex)
    {
        const double xCave = static_cast<double>(x * 16 + random_next_int(&m_betaRandom, 16));
        const double yCave = static_cast<double>(random_next_int(&m_betaRandom, random_next_int(&m_betaRandom, BETA_CAVE_WORLD_HEIGHT - 8) + 8));
        const double zCave = static_cast<double>(z * 16 + random_next_int(&m_betaRandom, 16));
        int tunnelCount = 1;

        if (random_next_int(&m_betaRandom, 4) == 0)
        {
            addRoom(xOffs, zOffs, blocks, xCave, yCave, zCave);
            tunnelCount += random_next_int(&m_betaRandom, 4);
        }

        for (int tunnelIndex = 0; tunnelIndex < tunnelCount; ++tunnelIndex)
        {
            const float yRot = BetaNextFloat(&m_betaRandom) * 3.1415927f * 2.0f;
            const float xRot = (BetaNextFloat(&m_betaRandom) - 0.5f) * 2.0f / 8.0f;
            const float thickness = BetaNextFloat(&m_betaRandom) * 2.0f + BetaNextFloat(&m_betaRandom);
            addTunnel(xOffs, zOffs, blocks, xCave, yCave, zCave, thickness, yRot, xRot, 0, 0, 1.0);
        }
    }
}
