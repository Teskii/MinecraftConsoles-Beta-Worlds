#pragma once

#include "Feature.h"

class BetaTreeFeature : public Feature
{
public:
    BetaTreeFeature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
