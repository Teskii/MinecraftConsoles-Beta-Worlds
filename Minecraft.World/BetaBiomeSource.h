#pragma once
#include "BiomeSource.h"
#include "biomeGen.h"

class Level;

class BetaBiomeSource : public BiomeSource
{
private:
    Level *m_level;

    static Biome *MapBetaBiomeToEngine(Biomes betaBiome);
    static byte MapBetaBiomeToIndex(Biomes betaBiome);

public:
    BetaBiomeSource(Level *level);
    virtual ~BetaBiomeSource();

    virtual Biome *getBiome(int x, int z);
    virtual bool containsOnly(int x, int z, int r, vector<Biome *> allowed);
    virtual bool containsOnly(int x, int z, int r, Biome *allowed);
    virtual TilePos *findBiome(int x, int z, int r, Biome *toFind, Random *random);
    virtual TilePos *findBiome(int x, int z, int r, vector<Biome *> allowed, Random *random);
    virtual float getDownfall(int x, int z) const;
    virtual floatArray getDownfallBlock(int x, int z, int w, int h) const;
    virtual void getDownfallBlock(floatArray &downfalls, int x, int z, int w, int h) const;

    virtual float getTemperature(int x, int y, int z) const;
    virtual floatArray getTemperatureBlock(int x, int z, int w, int h) const;
    virtual void getTemperatureBlock(floatArray &temperatures, int x, int z, int w, int h) const;

    virtual BiomeArray getRawBiomeBlock(int x, int z, int w, int h) const;
    virtual void getRawBiomeBlock(BiomeArray &biomes, int x, int z, int w, int h) const;
    virtual void getRawBiomeIndices(intArray &biomes, int x, int z, int w, int h) const;

    virtual BiomeArray getBiomeBlock(int x, int z, int w, int h) const;
    virtual void getBiomeBlock(BiomeArray &biomes, int x, int z, int w, int h, bool useCache) const;

    virtual byteArray getBiomeIndexBlock(int x, int z, int w, int h) const;
    virtual void getBiomeIndexBlock(byteArray &biomeIndices, int x, int z, int w, int h, bool useCache) const;
};
