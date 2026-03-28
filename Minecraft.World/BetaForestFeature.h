#pragma once

#include "Feature.h"

class BetaForestFeature : public Feature
{
public:
    BetaForestFeature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
