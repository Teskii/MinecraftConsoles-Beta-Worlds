#pragma once

#include "Feature.h"

class Level;

class BetaSpringFeature : public Feature
{
private:
    int tile;

public:
    BetaSpringFeature(int tile, bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
