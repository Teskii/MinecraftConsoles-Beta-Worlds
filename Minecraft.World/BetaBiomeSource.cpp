#include "stdafx.h"
#include "net.minecraft.world.level.h"
#include "Biome.h"
#include "BetaBiomeSource.h"
#include "biomeGen.h"

namespace
{
    static inline int GetBetaLocalIndex(int worldX, int worldZ)
    {
        return (worldX & 15) * 16 + (worldZ & 15);
    }

    template <typename THandler>
    static bool ForEachBetaCoarseSample(Level *level, int sampleX0, int sampleZ0, int sampleW, int sampleH, THandler handler)
    {
        uint64_t seed = static_cast<uint64_t>(level->getSeed());
        int cachedChunkX = 0;
        int cachedChunkZ = 0;
        bool hasCachedChunk = false;
        BiomeResult *result = nullptr;

        for (int sampleZ = sampleZ0; sampleZ < sampleZ0 + sampleH; ++sampleZ)
        {
            for (int sampleX = sampleX0; sampleX < sampleX0 + sampleW; ++sampleX)
            {
                int worldX = sampleX << 2;
                int worldZ = sampleZ << 2;
                int chunkX = worldX >> 4;
                int chunkZ = worldZ >> 4;

                if (!hasCachedChunk || chunkX != cachedChunkX || chunkZ != cachedChunkZ)
                {
                    if (result != nullptr)
                    {
                        delete_biome_result(result);
                    }

                    cachedChunkX = chunkX;
                    cachedChunkZ = chunkZ;
                    hasCachedChunk = true;
                    result = BiomeWrapper(seed, chunkX, chunkZ);
                }

                if (!handler(worldX, worldZ, result->biomes[GetBetaLocalIndex(worldX, worldZ)]))
                {
                    if (result != nullptr)
                    {
                        delete_biome_result(result);
                    }

                    return false;
                }
            }
        }

        if (result != nullptr)
        {
            delete_biome_result(result);
        }

        return true;
    }

    template <typename TWriter>
    static void ForEachBetaChunkArea(Level *level, int x, int z, int w, int h, TWriter writer)
    {
        uint64_t seed = static_cast<uint64_t>(level->getSeed());
        int chunkX0 = x >> 4;
        int chunkZ0 = z >> 4;
        int chunkX1 = (x + w - 1) >> 4;
        int chunkZ1 = (z + h - 1) >> 4;

        for (int chunkZ = chunkZ0; chunkZ <= chunkZ1; ++chunkZ)
        {
            for (int chunkX = chunkX0; chunkX <= chunkX1; ++chunkX)
            {
                BiomeResult *result = BiomeWrapper(seed, chunkX, chunkZ);

                int startX = x > (chunkX << 4) ? x : (chunkX << 4);
                int startZ = z > (chunkZ << 4) ? z : (chunkZ << 4);
                int endX = (x + w) < ((chunkX + 1) << 4) ? (x + w) : ((chunkX + 1) << 4);
                int endZ = (z + h) < ((chunkZ + 1) << 4) ? (z + h) : ((chunkZ + 1) << 4);

                for (int worldZ = startZ; worldZ < endZ; ++worldZ)
                {
                    for (int worldX = startX; worldX < endX; ++worldX)
                    {
                        int dstIndex = (worldX - x) + (worldZ - z) * w;
                        int srcIndex = GetBetaLocalIndex(worldX, worldZ);
                        writer(dstIndex, srcIndex, result);
                    }
                }

                delete_biome_result(result);
            }
        }
    }
}

BetaBiomeSource::BetaBiomeSource(Level *level)
    : BiomeSource(level), m_level(level)
{
}

BetaBiomeSource::~BetaBiomeSource()
{
}

Biome *BetaBiomeSource::MapBetaBiomeToEngine(Biomes betaBiome)
{
    switch (betaBiome)
    {
    case Rainforest:      return Biome::betaRainforest;
    case Swampland:       return Biome::betaSwampland;
    case Seasonal_forest: return Biome::betaSeasonalForest;
    case Forest:          return Biome::betaForest;
    case Savanna:         return Biome::betaSavanna;
    case Shrubland:       return Biome::betaShrubland;
    case Taiga:           return Biome::betaTaiga;
    case Desert:          return Biome::betaDesert;
    case Plains:          return Biome::betaPlains;
    case IceDesert:       return Biome::betaIceDesert;
    case Tundra:          return Biome::betaTundra;
    default:              return Biome::betaPlains;
    }
}

byte BetaBiomeSource::MapBetaBiomeToIndex(Biomes betaBiome)
{
    return MapBetaBiomeToEngine(betaBiome)->id;
}

Biome *BetaBiomeSource::getBiome(int x, int z)
{
    return BiomeSource::getBiome(x, z);
}

bool BetaBiomeSource::containsOnly(int x, int z, int r, vector<Biome *> allowed)
{
    int x0 = (x - r) >> 2;
    int z0 = (z - r) >> 2;
    int x1 = (x + r) >> 2;
    int z1 = (z + r) >> 2;
    int w = x1 - x0 + 1;
    int h = z1 - z0 + 1;

    return ForEachBetaCoarseSample(m_level, x0, z0, w, h, [this, &allowed](int, int, Biomes betaBiome)
    {
        Biome *biome = MapBetaBiomeToEngine(betaBiome);
        return find(allowed.begin(), allowed.end(), biome) != allowed.end();
    });
}

bool BetaBiomeSource::containsOnly(int x, int z, int r, Biome *allowed)
{
    int x0 = (x - r) >> 2;
    int z0 = (z - r) >> 2;
    int x1 = (x + r) >> 2;
    int z1 = (z + r) >> 2;
    int w = x1 - x0 + 1;
    int h = z1 - z0 + 1;

    return ForEachBetaCoarseSample(m_level, x0, z0, w, h, [this, allowed](int, int, Biomes betaBiome)
    {
        return MapBetaBiomeToEngine(betaBiome) == allowed;
    });
}

TilePos *BetaBiomeSource::findBiome(int x, int z, int r, Biome *toFind, Random *random)
{
    vector<Biome *> allowed;
    allowed.push_back(toFind);
    return findBiome(x, z, r, allowed, random);
}

TilePos *BetaBiomeSource::findBiome(int x, int z, int r, vector<Biome *> allowed, Random *random)
{
    int x0 = (x - r) >> 2;
    int z0 = (z - r) >> 2;
    int x1 = (x + r) >> 2;
    int z1 = (z + r) >> 2;
    int w = x1 - x0 + 1;
    int h = z1 - z0 + 1;

    TilePos *result = nullptr;
    int found = 0;

    ForEachBetaCoarseSample(m_level, x0, z0, w, h, [this, &allowed, random, &result, &found](int worldX, int worldZ, Biomes betaBiome)
    {
        Biome *biome = MapBetaBiomeToEngine(betaBiome);
        if (find(allowed.begin(), allowed.end(), biome) != allowed.end())
        {
            if (result == nullptr || random->nextInt(found + 1) == 0)
            {
                delete result;
                result = new TilePos(worldX, 0, worldZ);
            }

            ++found;
        }

        return true;
    });

    return result;
}

float BetaBiomeSource::getDownfall(int x, int z) const
{
    uint64_t seed = static_cast<uint64_t>(m_level->getSeed());
    int chunkX = x >> 4;
    int chunkZ = z >> 4;

    BiomeResult *result = BiomeWrapper(seed, chunkX, chunkZ);

    int localX = x & 15;
    int localZ = z & 15;
    float out = static_cast<float>(result->humidity[localX * 16 + localZ]);

    delete_biome_result(result);
    return out;
}

floatArray BetaBiomeSource::getDownfallBlock(int x, int z, int w, int h) const
{
    floatArray out;
    getDownfallBlock(out, x, z, w, h);
    return out;
}

void BetaBiomeSource::getDownfallBlock(floatArray &downfalls, int x, int z, int w, int h) const
{
    if (downfalls.data == nullptr || downfalls.length < w * h)
    {
        if (downfalls.data != nullptr) delete [] downfalls.data;
        downfalls = floatArray(w * h);
    }

    ForEachBetaChunkArea(m_level, x, z, w, h, [&downfalls](int dstIndex, int srcIndex, const BiomeResult *result)
    {
        downfalls[dstIndex] = static_cast<float>(result->humidity[srcIndex]);
    });
}

float BetaBiomeSource::getTemperature(int x, int y, int z) const
{
    uint64_t seed = static_cast<uint64_t>(m_level->getSeed());
    int chunkX = x >> 4;
    int chunkZ = z >> 4;

    BiomeResult *result = BiomeWrapper(seed, chunkX, chunkZ);

    int localX = x & 15;
    int localZ = z & 15;
    float out = static_cast<float>(result->temperature[localX * 16 + localZ]);

    delete_biome_result(result);
    return out;
}

floatArray BetaBiomeSource::getTemperatureBlock(int x, int z, int w, int h) const
{
    floatArray out;
    getTemperatureBlock(out, x, z, w, h);
    return out;
}

void BetaBiomeSource::getTemperatureBlock(floatArray &temperatures, int x, int z, int w, int h) const
{
    if (temperatures.data == nullptr || temperatures.length < w * h)
    {
        if (temperatures.data != nullptr) delete [] temperatures.data;
        temperatures = floatArray(w * h);
    }

    ForEachBetaChunkArea(m_level, x, z, w, h, [&temperatures](int dstIndex, int srcIndex, const BiomeResult *result)
    {
        temperatures[dstIndex] = static_cast<float>(result->temperature[srcIndex]);
    });
}

BiomeArray BetaBiomeSource::getRawBiomeBlock(int x, int z, int w, int h) const
{
    BiomeArray out;
    getRawBiomeBlock(out, x, z, w, h);
    return out;
}

void BetaBiomeSource::getRawBiomeBlock(BiomeArray &biomes, int x, int z, int w, int h) const
{
    if (biomes.data == nullptr || biomes.length < w * h)
    {
        if (biomes.data != nullptr) delete [] biomes.data;
        biomes = BiomeArray(w * h);
    }

    ForEachBetaChunkArea(m_level, x, z, w, h, [this, &biomes](int dstIndex, int srcIndex, const BiomeResult *result)
    {
        biomes[dstIndex] = MapBetaBiomeToEngine(result->biomes[srcIndex]);
    });
}

void BetaBiomeSource::getRawBiomeIndices(intArray &biomes, int x, int z, int w, int h) const
{
    if (biomes.data == nullptr || biomes.length < w * h)
    {
        if (biomes.data != nullptr) delete [] biomes.data;
        biomes = intArray(w * h);
    }

    ForEachBetaChunkArea(m_level, x, z, w, h, [this, &biomes](int dstIndex, int srcIndex, const BiomeResult *result)
    {
        biomes[dstIndex] = MapBetaBiomeToIndex(result->biomes[srcIndex]);
    });
}

BiomeArray BetaBiomeSource::getBiomeBlock(int x, int z, int w, int h) const
{
    BiomeArray out;
    getBiomeBlock(out, x, z, w, h, false);
    return out;
}

void BetaBiomeSource::getBiomeBlock(BiomeArray &biomes, int x, int z, int w, int h, bool useCache) const
{
    getRawBiomeBlock(biomes, x, z, w, h);
}

byteArray BetaBiomeSource::getBiomeIndexBlock(int x, int z, int w, int h) const
{
    byteArray out;
    getBiomeIndexBlock(out, x, z, w, h, false);
    return out;
}

void BetaBiomeSource::getBiomeIndexBlock(byteArray &biomeIndices, int x, int z, int w, int h, bool useCache) const
{
    if (biomeIndices.data == nullptr || biomeIndices.length < w * h)
    {
        if (biomeIndices.data != nullptr) delete [] biomeIndices.data;
        biomeIndices = byteArray(w * h);
    }

    ForEachBetaChunkArea(m_level, x, z, w, h, [this, &biomeIndices](int dstIndex, int srcIndex, const BiomeResult *result)
    {
        biomeIndices[dstIndex] = MapBetaBiomeToIndex(result->biomes[srcIndex]);
    });
}
