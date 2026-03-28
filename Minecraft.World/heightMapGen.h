#pragma once
#include "stdafx.h"
#include "perlinCommon.h"
#include "biomeGen.h" 
#include <cstdint>

#ifndef HEIGHTMAP_GEN_HEADER
#define HEIGHTMAP_GEN_HEADER

struct TerrainNoises {
    PermutationTable minLimit[16];
    PermutationTable maxLimit[16];
    PermutationTable mainLimit[8];
    PermutationTable shoresBottomComposition[4];
    PermutationTable surfaceElevation[4];
    PermutationTable scale[10];
    PermutationTable depth[16];
    PermutationTable forest[8];
};
enum blocks {
    AIR,
    STONE,
    GRASS,
    DIRT,
    BEDROCK,
    MOVING_WATER,
    SAND,
    GRAVEL,
    ICE,
    SANDSTONE,
};

struct TerrainResult {
    BiomeResult *biomeResult;
    uint8_t *chunkCache;
    uint8_t *chunkHeights;
};

struct FiniteWorldBorderConfig {
    int worldSize;
    int seaLevel;
    bool classicEdgeMoat;
    bool smallEdgeMoat;
    bool mediumEdgeMoat;
};

void delete_terrain_result(TerrainResult *terrainResult);

uint8_t *TerrainInternalWrapper(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ, BiomeResult *biomeResult, const FiniteWorldBorderConfig *borderConfig = nullptr);

TerrainResult *TerrainWrapper(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ);

uint8_t *TerrainHeights(uint64_t worldSeed, int32_t chunkX, int32_t chunkZ, BiomeResult *biomeResult);

#endif //HEIGHTMAP_GEN_HEADER
