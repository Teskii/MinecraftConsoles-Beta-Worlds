#pragma once

#include "Feature.h"

class BetaTaiga2Feature : public Feature
{
public:
    BetaTaiga2Feature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
