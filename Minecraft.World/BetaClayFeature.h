#pragma once

#include "Feature.h"

class Level;

class BetaClayFeature : public Feature
{
private:
    int tile;
    int count;

public:
    BetaClayFeature(int count);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
