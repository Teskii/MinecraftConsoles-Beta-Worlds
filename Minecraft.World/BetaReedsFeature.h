#pragma once

#include "Feature.h"

class Level;

class BetaReedsFeature : public Feature
{
public:
    BetaReedsFeature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
