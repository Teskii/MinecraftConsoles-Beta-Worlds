#ifndef JAVA_RANDOM_HEADER
#define JAVA_RANDOM_HEADER

#include <stdint.h>

typedef uint64_t JavaRand;

#define RANDOM_MULTIPLIER 0x5DEECE66DULL
#define RANDOM_ADDEND 0xBULL
#define RANDOM_MASK ((1ULL << 48u) - 1)
#define RANDOM_SCALE 0x1.0p-53
#define get_java_random(seed) ((JavaRand)((seed ^ RANDOM_MULTIPLIER) & RANDOM_MASK))

static inline int32_t random_next(JavaRand *rnd, int bits)
{
    *rnd = (*rnd * 25214903917ULL + 11ULL) & ((1ULL << 48) - 1);
    return (int32_t)(*rnd >> (48 - bits));
}

static inline int32_t random_next_int(JavaRand *rnd, int32_t bound)
{
    if (bound <= 0)
        return 0;

    if ((bound & -bound) == bound)
        return (int32_t)((bound * (int64_t)random_next(rnd, 31)) >> 31);

    int32_t bits, val;
    do
    {
        bits = random_next(rnd, 31);
        val = bits % bound;
    } while (bits - val + (bound - 1) < 0);

    return val;
}

static inline double next_double(JavaRand *random) {
    return (double)((((uint64_t)((uint32_t)random_next(random, 26)) << 27u)) + random_next(random, 27)) * RANDOM_SCALE;
}

static inline uint64_t random_next_long(JavaRand *random) {
    return (((uint64_t)random_next(random, 32)) << 32u) + (int32_t)random_next(random, 32);
}


static inline void advance2(JavaRand *random) {
    *random = (*random * 0xBB20B4600A69ULL + 0x40942DE6BAULL) & RANDOM_MASK;
}

#endif