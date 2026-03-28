#pragma once

#include "Feature.h"

class Level;

class BetaPumpkinFeature : public Feature
{
public:
    BetaPumpkinFeature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
