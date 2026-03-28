#pragma once

#include "Feature.h"

class BetaTaiga1Feature : public Feature
{
public:
    BetaTaiga1Feature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
