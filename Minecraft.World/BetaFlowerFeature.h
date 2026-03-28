#pragma once

#include "Feature.h"

class Level;

class BetaFlowerFeature : public Feature
{
private:
    int tile;
    int data;

public:
    BetaFlowerFeature(int tile, int data = 0, bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
