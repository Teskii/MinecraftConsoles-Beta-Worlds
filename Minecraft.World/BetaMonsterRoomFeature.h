#pragma once

#include "Feature.h"

class ItemInstance;
class Level;

class BetaMonsterRoomFeature : public Feature
{
private:
    shared_ptr<ItemInstance> getRandomItem(Random *random);
    wstring randomEntityId(Random *random);

public:
    BetaMonsterRoomFeature(bool doUpdate = false);

    virtual bool place(Level *level, Random *random, int x, int y, int z);
};
