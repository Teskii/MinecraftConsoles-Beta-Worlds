#pragma once
#ifndef NOISE_HEADER
#define NOISE_HEADER

#include "javaRnd.h"
#include <stdint.h>

struct PermutationTable {
    double xo;
    double yo;
    double zo;
    uint8_t permutations[512];
};

static inline void initOctaves(PermutationTable octaves[], JavaRand *random, int nbOctaves)
{
    for (int i = 0; i < nbOctaves; ++i)
    {
        octaves[i].xo = next_double(random) * 256.0;
        octaves[i].yo = next_double(random) * 256.0;
        octaves[i].zo = next_double(random) * 256.0;

        uint8_t *permutations = octaves[i].permutations;

        for (int index = 0; index < 256; ++index)
        {
            permutations[index] = (uint8_t)index;
        }

        for (int index = 0; index < 256; ++index)
        {
            uint32_t randomIndex = (uint32_t)random_next_int(random, 256 - index) + index;

            uint8_t tmp = permutations[index];
            permutations[index] = permutations[randomIndex];
            permutations[randomIndex] = tmp;

            permutations[index + 256] = permutations[index];
        }
    }
}

#endif