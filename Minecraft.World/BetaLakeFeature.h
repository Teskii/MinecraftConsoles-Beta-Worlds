#pragma once

#include "Feature.h"

class BetaLakeFeature : public Feature
{
private:
    int tile;

public:
    BetaLakeFeature(int tile);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
