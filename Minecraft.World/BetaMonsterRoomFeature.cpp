#include "stdafx.h"

#include "net.minecraft.world.level.h"
#include "net.minecraft.world.level.material.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.level.tile.entity.h"
#include "net.minecraft.world.item.h"
#include "DyePowderItem.h"
#include "BetaMonsterRoomFeature.h"

BetaMonsterRoomFeature::BetaMonsterRoomFeature(bool doUpdate) : Feature(doUpdate)
{
}

shared_ptr<ItemInstance> BetaMonsterRoomFeature::getRandomItem(Random *random)
{
    int i = random->nextInt(11);

    switch (i)
    {
    case 0:
        return std::make_shared<ItemInstance>(Item::saddle, 1);
    case 1:
        return std::make_shared<ItemInstance>(Item::ironIngot, random->nextInt(4) + 1);
    case 2:
        return std::make_shared<ItemInstance>(Item::bread, 1);
    case 3:
        return std::make_shared<ItemInstance>(Item::wheat, random->nextInt(4) + 1);
    case 4:
        return std::make_shared<ItemInstance>(Item::gunpowder, random->nextInt(4) + 1);
    case 5:
        return std::make_shared<ItemInstance>(Item::string, random->nextInt(4) + 1);
    case 6:
        return std::make_shared<ItemInstance>(Item::bucket_empty, 1);
    case 7:
        return random->nextInt(100) == 0 ? std::make_shared<ItemInstance>(Item::apple_gold, 1) : nullptr;
    case 8:
        return random->nextInt(2) == 0 ? std::make_shared<ItemInstance>(Item::redStone, random->nextInt(4) + 1) : nullptr;
    case 9:
        return random->nextInt(10) == 0 ?
            std::make_shared<ItemInstance>(random->nextInt(2) == 0 ? Item::record_01 : Item::record_02, 1) :
            nullptr;
    case 10:
        return std::make_shared<ItemInstance>(Item::dye_powder, 1, DyePowderItem::BROWN);
    default:
        return nullptr;
    }
}

wstring BetaMonsterRoomFeature::randomEntityId(Random *random)
{
    switch (random->nextInt(4))
    {
    case 0:
        return wstring(L"Skeleton");
    case 1:
    case 2:
        return wstring(L"Zombie");
    case 3:
        return wstring(L"Spider");
    default:
        return wstring(L"Pig");
    }
}

bool BetaMonsterRoomFeature::place(Level *level, Random *random, int x, int y, int z)
{
    const int height = 3;
    const int xRadius = random->nextInt(2) + 2;
    const int zRadius = random->nextInt(2) + 2;
    int openings = 0;

    for (int xx = x - xRadius - 1; xx <= x + xRadius + 1; ++xx)
    {
        for (int yy = y - 1; yy <= y + height + 1; ++yy)
        {
            for (int zz = z - zRadius - 1; zz <= z + zRadius + 1; ++zz)
            {
                Material *material = level->getMaterial(xx, yy, zz);

                if (yy == y - 1 && !material->isSolid())
                {
                    return false;
                }

                if (yy == y + height + 1 && !material->isSolid())
                {
                    return false;
                }

                if ((xx == x - xRadius - 1 || xx == x + xRadius + 1 || zz == z - zRadius - 1 || zz == z + zRadius + 1) &&
                    yy == y &&
                    level->isEmptyTile(xx, yy, zz) &&
                    level->isEmptyTile(xx, yy + 1, zz))
                {
                    ++openings;
                }
            }
        }
    }

    if (openings < 1 || openings > 5)
    {
        return false;
    }

    for (int xx = x - xRadius - 1; xx <= x + xRadius + 1; ++xx)
    {
        for (int yy = y + height; yy >= y - 1; --yy)
        {
            for (int zz = z - zRadius - 1; zz <= z + zRadius + 1; ++zz)
            {
                bool isBoundary = xx == x - xRadius - 1 || yy == y - 1 || zz == z - zRadius - 1 ||
                    xx == x + xRadius + 1 || yy == y + height + 1 || zz == z + zRadius + 1;

                if (!isBoundary)
                {
                    level->removeTile(xx, yy, zz);
                    continue;
                }

                if (yy >= 0 && !level->getMaterial(xx, yy - 1, zz)->isSolid())
                {
                    level->removeTile(xx, yy, zz);
                }
                else if (level->getMaterial(xx, yy, zz)->isSolid())
                {
                    int tile = (yy == y - 1 && random->nextInt(4) != 0) ? Tile::mossyCobblestone_Id : Tile::cobblestone_Id;
                    level->setTileAndData(xx, yy, zz, tile, 0, Tile::UPDATE_CLIENTS);
                }
            }
        }
    }

    for (int chestPass = 0; chestPass < 2; ++chestPass)
    {
        for (int attempt = 0; attempt < 3; ++attempt)
        {
            int chestX = x + random->nextInt(xRadius * 2 + 1) - xRadius;
            int chestZ = z + random->nextInt(zRadius * 2 + 1) - zRadius;

            if (!level->isEmptyTile(chestX, y, chestZ))
            {
                continue;
            }

            int solidSides = 0;
            if (level->getMaterial(chestX - 1, y, chestZ)->isSolid()) ++solidSides;
            if (level->getMaterial(chestX + 1, y, chestZ)->isSolid()) ++solidSides;
            if (level->getMaterial(chestX, y, chestZ - 1)->isSolid()) ++solidSides;
            if (level->getMaterial(chestX, y, chestZ + 1)->isSolid()) ++solidSides;

            if (solidSides != 1)
            {
                continue;
            }

            level->setTileAndData(chestX, y, chestZ, Tile::chest_Id, 0, Tile::UPDATE_CLIENTS);
            shared_ptr<ChestTileEntity> chest = dynamic_pointer_cast<ChestTileEntity>(level->getTileEntity(chestX, y, chestZ));
            if (chest != nullptr)
            {
                for (int lootAttempt = 0; lootAttempt < 8; ++lootAttempt)
                {
                    shared_ptr<ItemInstance> item = getRandomItem(random);
                    if (item != nullptr)
                    {
                        chest->setItem(random->nextInt(chest->getContainerSize()), item);
                    }
                }
            }

            break;
        }
    }

    level->setTileAndData(x, y, z, Tile::mobSpawner_Id, 0, Tile::UPDATE_CLIENTS);
    shared_ptr<MobSpawnerTileEntity> entity = dynamic_pointer_cast<MobSpawnerTileEntity>(level->getTileEntity(x, y, z));
    if (entity != nullptr)
    {
        entity->getSpawner()->setEntityId(randomEntityId(random));
    }

    return true;
}
