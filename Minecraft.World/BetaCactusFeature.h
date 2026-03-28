#pragma once

#include "Feature.h"

class Level;

class BetaCactusFeature : public Feature
{
public:
    BetaCactusFeature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
