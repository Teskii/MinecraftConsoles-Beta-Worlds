#pragma once

#include "Feature.h"

class Level;

class BetaOreFeature : public Feature
{
private:
    int tile;
    int count;
    int targetTile;

    void _init(int tile, int count, int targetTile);

public:
    BetaOreFeature(int tile, int count);
    BetaOreFeature(int tile, int count, int targetTile);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
