#pragma once

#include "Feature.h"

class Level;

class BetaGrassFeature : public Feature
{
private:
    int data;

public:
    BetaGrassFeature(int data, bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
