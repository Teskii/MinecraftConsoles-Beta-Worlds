#include "stdafx.h"
#include "BetaOldVillageFeature.h"
#include "net.minecraft.world.level.h"
#include "LevelData.h"
#include "net.minecraft.world.level.biome.h"
#include "net.minecraft.world.entity.ai.village.h"
#include "net.minecraft.world.level.material.h"
#include "net.minecraft.world.level.tile.h"
#include "net.minecraft.world.level.tile.entity.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.entity.animal.h"
#include "net.minecraft.world.entity.monster.h"
#include "HouseFeature.h"
#include "HugeMushroomFeature.h"
#include "SandStoneTile.h"
#include "WeighedTreasure.h"
#include "Sapling.h"
#include "DyePowderItem.h"

namespace
{
	enum HouseMaterialPalette
	{
		HouseMaterialPalette_Normal = 0,
		HouseMaterialPalette_Desert = 1,
		HouseMaterialPalette_Tundra = 2,
	};

	struct HouseLayout
	{
		int x0;
		int y0;
		int z0;
		int w;
		int h;
		int d;
		int doorSide;
	};

	enum HouseDecorationType
	{
		HouseDecorationType_LootChest = 0,
		HouseDecorationType_Furnace = 1,
		HouseDecorationType_Workbench = 2,
		HouseDecorationType_Cauldron = 3,
		HouseDecorationType_VerticalBookshelfPair = 4,
	};

	enum OppositeWallSetType
	{
		OppositeWallSetType_HorizontalBookshelfLine = 0,
		OppositeWallSetType_StairFenceStair = 1,
	};

	static int GetChunkCoordinate(int blockPos)
	{
		return blockPos >= 0 ? (blockPos / 16) : ((blockPos - 15) / 16);
	}

	static int GetFiniteWorldMinBlock(Level *level)
	{
		return -((level->getLevelData()->getXZSize() * 16) / 2);
	}

	static int GetFiniteWorldMaxBlock(Level *level)
	{
		return ((level->getLevelData()->getXZSize() * 16) / 2) - 1;
	}

	static bool IsInsideFiniteWorld(Level *level, int x, int z)
	{
		const int minBlock = GetFiniteWorldMinBlock(level);
		const int maxBlock = GetFiniteWorldMaxBlock(level);
		return x >= minBlock && x <= maxBlock && z >= minBlock && z <= maxBlock;
	}

	static bool IsInsideWorld(int iXZSize, BoundingBox *box)
	{
		if (box == nullptr)
		{
			return false;
		}

		const int halfWorld = (iXZSize * 16) / 2;
		return box->x0 >= -halfWorld &&
			box->z0 >= -halfWorld &&
			box->x1 <= halfWorld - 1 &&
			box->z1 <= halfWorld - 1;
	}

	static bool TryAppendPiece(list<StructurePiece *> &pieces, StructurePiece *piece, int iXZSize)
	{
		if (piece == nullptr || piece->getBoundingBox() == nullptr)
		{
			delete piece;
			return false;
		}

		if (!IsInsideWorld(iXZSize, piece->getBoundingBox()) ||
			StructurePiece::findCollisionPiece(&pieces, piece->getBoundingBox()) != nullptr)
		{
			delete piece;
			return false;
		}

		pieces.push_back(piece);
		return true;
	}

	static bool TryAppendDecorPiece(list<StructurePiece *> &pieces, StructurePiece *piece, int iXZSize)
	{
		if (piece == nullptr || piece->getBoundingBox() == nullptr)
		{
			delete piece;
			return false;
		}

		if (!IsInsideWorld(iXZSize, piece->getBoundingBox()))
		{
			delete piece;
			return false;
		}

		pieces.push_back(piece);
		return true;
	}

	static bool PrepareHouseLayout(Level *level, Random *random, int x, int y, int z, HouseLayout &layout)
	{
		while (y > 0 && !level->getMaterial(x, y - 1, z)->blocksMotion())
		{
			--y;
		}

		if (y <= 0)
		{
			return false;
		}

		int w = random->nextInt(7) + 7;
		int h = 4 + random->nextInt(3) / 2;
		int d = random->nextInt(7) + 7;

		const int x0 = x - w / 2;
		const int z0 = z - d / 2;
		const int doorSide = random->nextInt(4);

		if (doorSide < 2)
		{
			d += 2;
		}
		else
		{
			w += 2;
		}

		for (int xx = x0; xx < x0 + w; ++xx)
		{
			for (int zz = z0; zz < z0 + d; ++zz)
			{
				Material *m = level->getMaterial(xx, y - 1, zz);
				if (m == nullptr || !m->blocksMotion() || m == Material::ice)
				{
					return false;
				}

				bool isDoorApproach = false;
				if (doorSide == 0 && xx < x0 + 2) isDoorApproach = true;
				if (doorSide == 1 && xx > x0 + w - 1 - 2) isDoorApproach = true;
				if (doorSide == 2 && zz < z0 + 2) isDoorApproach = true;
				if (doorSide == 3 && zz > z0 + d - 1 - 2) isDoorApproach = true;

				const int tile = level->getTile(xx, y, zz);
				if (isDoorApproach)
				{
					if (tile != 0)
					{
						return false;
					}
				}
				else if (tile == Tile::cobblestone_Id || tile == Tile::mossyCobblestone_Id)
				{
					return false;
				}
			}
		}

		layout.x0 = x0;
		layout.y0 = y;
		layout.z0 = z0;
		layout.w = w;
		layout.h = h;
		layout.d = d;
		layout.doorSide = doorSide;
		return true;
	}

	static void FinalizeHouseLayout(HouseLayout &layout)
	{
		if (layout.doorSide == 0)
		{
			layout.x0++;
			layout.w--;
		}
		else if (layout.doorSide == 1)
		{
			layout.w--;
		}
		else if (layout.doorSide == 2)
		{
			layout.z0++;
			layout.d--;
		}
		else if (layout.doorSide == 3)
		{
			layout.d--;
		}
	}

	static int GetInteriorMinX(const HouseLayout &layout)
	{
		return layout.doorSide >= 2 ? (layout.x0 + 1) : layout.x0;
	}

	static int GetInteriorMaxX(const HouseLayout &layout)
	{
		return layout.doorSide >= 2 ? (layout.x0 + layout.w - 2) : (layout.x0 + layout.w - 1);
	}

	static int GetInteriorMinZ(const HouseLayout &layout)
	{
		return layout.doorSide < 2 ? (layout.z0 + 1) : layout.z0;
	}

	static int GetInteriorMaxZ(const HouseLayout &layout)
	{
		return layout.doorSide < 2 ? (layout.z0 + layout.d - 2) : (layout.z0 + layout.d - 1);
	}

	static WeighedTreasureArray GetVillageHouseTreasure()
	{
		static bool initialized = false;
		static WeighedTreasureArray treasure;
		if (!initialized)
		{
			treasure = WeighedTreasureArray(16);
			treasure[0] = new WeighedTreasure(Item::bread_Id, 0, 1, 3, 15);
			treasure[1] = new WeighedTreasure(Item::apple_Id, 0, 1, 3, 15);
			treasure[2] = new WeighedTreasure(Item::coal_Id, 0, 2, 6, 10);
			treasure[3] = new WeighedTreasure(Item::ironIngot_Id, 0, 1, 3, 8);
			treasure[4] = new WeighedTreasure(Item::goldIngot_Id, 0, 1, 2, 4);
			treasure[5] = new WeighedTreasure(Item::stick_Id, 0, 2, 6, 10);
			treasure[6] = new WeighedTreasure(Item::wheat_Id, 0, 1, 4, 8);
			treasure[7] = new WeighedTreasure(Item::redStone_Id, 0, 1, 4, 4);
			treasure[8] = new WeighedTreasure(Item::nameTag_Id, 0, 1, 1, 2);
			treasure[9] = new WeighedTreasure(Tile::mycel_Id, 0, 1, 3, 4);
			treasure[10] = new WeighedTreasure(Tile::sapling_Id, Sapling::TYPE_JUNGLE, 1, 3, 6);
			treasure[11] = new WeighedTreasure(Item::dye_powder_Id, DyePowderItem::BROWN, 2, 5, 6);
			treasure[12] = new WeighedTreasure(Tile::waterLily_Id, 0, 1, 4, 5);
			treasure[13] = new WeighedTreasure(Tile::vine_Id, 0, 2, 6, 5);
			treasure[14] = new WeighedTreasure(Item::carrots_Id, 0, 1, 4, 7);
			treasure[15] = new WeighedTreasure(Item::potato_Id, 0, 1, 4, 7);
			initialized = true;
		}

		return treasure;
	}

	static bool CanPlaceDecorationBlock(Level *level, int x, int y, int z)
	{
		return level->getTile(x, y, z) == 0 &&
			level->getMaterial(x, y - 1, z) != nullptr &&
			level->getMaterial(x, y - 1, z)->blocksMotion();
	}

	static bool CanPlaceDecorationColumn(Level *level, int x, int y, int z, int height)
	{
		if (level->getMaterial(x, y - 1, z) == nullptr || !level->getMaterial(x, y - 1, z)->blocksMotion())
		{
			return false;
		}

		for (int dy = 0; dy < height; ++dy)
		{
			if (level->getTile(x, y + dy, z) != 0)
			{
				return false;
			}
		}

		return true;
	}

	static bool CanPlaceDecorationLine(Level *level, int x, int y, int z, int dx, int dz, int length)
	{
		for (int i = 0; i < length; ++i)
		{
			const int xx = x + dx * i;
			const int zz = z + dz * i;
			if (!CanPlaceDecorationBlock(level, xx, y, zz))
			{
				return false;
			}
		}

		return true;
	}

	static int GetOppositeWall(int wall)
	{
		switch (wall)
		{
		case 0: return 1;
		case 1: return 0;
		case 2: return 3;
		case 3: return 2;
		default: return wall;
		}
	}

	static void ShuffleIntArray(Random *random, int *values, int count)
	{
		for (int i = count - 1; i > 0; --i)
		{
			const int swapIndex = random->nextInt(i + 1);
			const int temp = values[i];
			values[i] = values[swapIndex];
			values[swapIndex] = temp;
		}
	}

	static void ShuffleSlots(Random *random, int *xs, int *zs, int count)
	{
		for (int i = count - 1; i > 0; --i)
		{
			const int swapIndex = random->nextInt(i + 1);
			const int tempX = xs[i];
			const int tempZ = zs[i];
			xs[i] = xs[swapIndex];
			zs[i] = zs[swapIndex];
			xs[swapIndex] = tempX;
			zs[swapIndex] = tempZ;
		}
	}

	static void AppendUniqueSlot(int x, int z, int *xs, int *zs, int *count)
	{
		for (int i = 0; i < *count; ++i)
		{
			if (xs[i] == x && zs[i] == z)
			{
				return;
			}
		}

		xs[*count] = x;
		zs[*count] = z;
		++(*count);
	}

	static void GatherPrimaryWalls(const HouseLayout &layout, int *walls, int *wallCount)
	{
		int count = 0;
		for (int wall = 0; wall < 4; ++wall)
		{
			if (wall != layout.doorSide && GetOppositeWall(wall) != layout.doorSide)
			{
				walls[count++] = wall;
			}
		}

		*wallCount = count;
	}

	static void GatherPrimaryDecorationSlots(const HouseLayout &layout, int wall, int *xs, int *zs, int *slotCount)
	{
		const int minX = GetInteriorMinX(layout);
		const int maxX = GetInteriorMaxX(layout);
		const int minZ = GetInteriorMinZ(layout);
		const int maxZ = GetInteriorMaxZ(layout);
		int count = 0;

		if (wall == 0 || wall == 1)
		{
			const int x = wall == 0 ? (minX + 1) : (maxX - 1);
			const int zCandidates[3] = { (minZ + maxZ) / 2, minZ + 1, maxZ - 1 };
			for (int i = 0; i < 3; ++i)
			{
				AppendUniqueSlot(x, zCandidates[i], xs, zs, &count);
			}
		}
		else
		{
			const int z = wall == 2 ? (minZ + 1) : (maxZ - 1);
			const int xCandidates[3] = { (minX + maxX) / 2, minX + 1, maxX - 1 };
			for (int i = 0; i < 3; ++i)
			{
				AppendUniqueSlot(xCandidates[i], z, xs, zs, &count);
			}
		}

		*slotCount = count;
	}

	static void GatherOppositeWallSetSlots(const HouseLayout &layout, int wall, int *xs, int *zs, int *slotCount)
	{
		const int minX = GetInteriorMinX(layout);
		const int maxX = GetInteriorMaxX(layout);
		const int minZ = GetInteriorMinZ(layout);
		const int maxZ = GetInteriorMaxZ(layout);
		int count = 0;

		if (wall == 0 || wall == 1)
		{
			const int x = wall == 0 ? (minX + 1) : (maxX - 1);
			AppendUniqueSlot(x, (minZ + maxZ) / 2, xs, zs, &count);
			if (maxZ - minZ >= 4)
			{
				AppendUniqueSlot(x, minZ + 2, xs, zs, &count);
				AppendUniqueSlot(x, maxZ - 2, xs, zs, &count);
			}
		}
		else
		{
			const int z = wall == 2 ? (minZ + 1) : (maxZ - 1);
			AppendUniqueSlot((minX + maxX) / 2, z, xs, zs, &count);
			if (maxX - minX >= 4)
			{
				AppendUniqueSlot(minX + 2, z, xs, zs, &count);
				AppendUniqueSlot(maxX - 2, z, xs, zs, &count);
			}
		}

		*slotCount = count;
	}

	static bool FindHouseDoor(const HouseLayout &layout, Level *level, int *outDoorX, int *outDoorZ)
	{
		for (int xx = layout.x0; xx < layout.x0 + layout.w; ++xx)
		{
			for (int zz = layout.z0; zz < layout.z0 + layout.d; ++zz)
			{
				const bool isPerimeter = xx == layout.x0 || xx == (layout.x0 + layout.w - 1) || zz == layout.z0 || zz == (layout.z0 + layout.d - 1);
				if (!isPerimeter)
				{
					continue;
				}

				if (level->getTile(xx, layout.y0, zz) == Tile::door_wood_Id)
				{
					*outDoorX = xx;
					*outDoorZ = zz;
					return true;
				}
			}
		}

		return false;
	}

	static bool IsInDoorApproach(const HouseLayout &layout, int doorX, int doorZ, int x, int z)
	{
		switch (layout.doorSide)
		{
		case 0:
			return z == doorZ && (x == doorX + 1 || x == doorX + 2);
		case 1:
			return z == doorZ && (x == doorX - 1 || x == doorX - 2);
		case 2:
			return x == doorX && (z == doorZ + 1 || z == doorZ + 2);
		case 3:
			return x == doorX && (z == doorZ - 1 || z == doorZ - 2);
		default:
			return false;
		}
	}

	static void FilterDoorApproachSlots(const HouseLayout &layout, Level *level, int *xs, int *zs, int *count)
	{
		int doorX = 0;
		int doorZ = 0;
		if (!FindHouseDoor(layout, level, &doorX, &doorZ))
		{
			return;
		}

		int writeIndex = 0;
		for (int i = 0; i < *count; ++i)
		{
			if (IsInDoorApproach(layout, doorX, doorZ, xs[i], zs[i]))
			{
				continue;
			}

			xs[writeIndex] = xs[i];
			zs[writeIndex] = zs[i];
			++writeIndex;
		}

		*count = writeIndex;
	}

	static bool PlaceLootChest(Level *level, Random *random, int x, int y, int z)
	{
		if (!CanPlaceDecorationBlock(level, x, y, z))
		{
			return false;
		}

		level->setTileAndData(x, y, z, Tile::chest_Id, 0, Tile::UPDATE_CLIENTS);
		shared_ptr<ChestTileEntity> chest = dynamic_pointer_cast<ChestTileEntity>(level->getTileEntity(x, y, z));
		if (chest != nullptr)
		{
			WeighedTreasure::addChestItems(random, GetVillageHouseTreasure(), chest, 1 + random->nextInt(3));
		}
		return true;
	}

	static bool PlaceSingleDecoration(Level *level, int tile, int data, int x, int y, int z)
	{
		if (!CanPlaceDecorationBlock(level, x, y, z))
		{
			return false;
		}

		level->setTileAndData(x, y, z, tile, data, Tile::UPDATE_CLIENTS);
		return true;
	}

	static bool PlaceVerticalBookshelfPair(Level *level, int x, int y, int z)
	{
		if (!CanPlaceDecorationColumn(level, x, y, z, 2))
		{
			return false;
		}

		level->setTileAndData(x, y, z, Tile::bookshelf_Id, 0, Tile::UPDATE_CLIENTS);
		level->setTileAndData(x, y + 1, z, Tile::bookshelf_Id, 0, Tile::UPDATE_CLIENTS);
		return true;
	}

	static bool PlaceHorizontalBookshelfLine(Level *level, int x, int y, int z, int wall)
	{
		if (wall == 0 || wall == 1)
		{
			if (!CanPlaceDecorationLine(level, x, y, z - 1, 0, 1, 3))
			{
				return false;
			}

			for (int dz = -1; dz <= 1; ++dz)
			{
				level->setTileAndData(x, y, z + dz, Tile::bookshelf_Id, 0, Tile::UPDATE_CLIENTS);
			}
			return true;
		}

		if (!CanPlaceDecorationLine(level, x - 1, y, z, 1, 0, 3))
		{
			return false;
		}

		for (int dx = -1; dx <= 1; ++dx)
		{
			level->setTileAndData(x + dx, y, z, Tile::bookshelf_Id, 0, Tile::UPDATE_CLIENTS);
		}
		return true;
	}

	static bool PlaceHorizontalStairFenceStairSet(Level *level, int x, int y, int z, int wall)
	{
		if (level->getTile(x, y + 1, z) != 0)
		{
			return false;
		}

		if (wall == 0 || wall == 1)
		{
			if (!CanPlaceDecorationBlock(level, x, y, z - 1) ||
				!CanPlaceDecorationBlock(level, x, y, z) ||
				!CanPlaceDecorationBlock(level, x, y, z + 1))
			{
				return false;
			}

			level->setTileAndData(x, y, z - 1, Tile::stairs_wood_Id, 3, Tile::UPDATE_CLIENTS);
			level->setTileAndData(x, y, z, Tile::fence_Id, 0, Tile::UPDATE_CLIENTS);
			level->setTileAndData(x, y, z + 1, Tile::stairs_wood_Id, 2, Tile::UPDATE_CLIENTS);
		}
		else
		{
			if (!CanPlaceDecorationBlock(level, x - 1, y, z) ||
				!CanPlaceDecorationBlock(level, x, y, z) ||
				!CanPlaceDecorationBlock(level, x + 1, y, z))
			{
				return false;
			}

			level->setTileAndData(x - 1, y, z, Tile::stairs_wood_Id, 1, Tile::UPDATE_CLIENTS);
			level->setTileAndData(x, y, z, Tile::fence_Id, 0, Tile::UPDATE_CLIENTS);
			level->setTileAndData(x + 1, y, z, Tile::stairs_wood_Id, 0, Tile::UPDATE_CLIENTS);
		}

		level->setTileAndData(x, y + 1, z, Tile::pressurePlate_wood_Id, 0, Tile::UPDATE_CLIENTS);
		return true;
	}

	static bool PlaceOppositeWallSet(Level *level, int x, int y, int z, int wall, OppositeWallSetType setType)
	{
		switch (setType)
		{
		case OppositeWallSetType_HorizontalBookshelfLine:
			return PlaceHorizontalBookshelfLine(level, x, y, z, wall);
		case OppositeWallSetType_StairFenceStair:
			return PlaceHorizontalStairFenceStairSet(level, x, y, z, wall);
		default:
			return false;
		}
	}

	static bool PlacePrimaryDecoration(Level *level, Random *random, int x, int y, int z, HouseDecorationType decorationType)
	{
		switch (decorationType)
		{
		case HouseDecorationType_LootChest:
			return PlaceLootChest(level, random, x, y, z);
		case HouseDecorationType_Furnace:
			return PlaceSingleDecoration(level, Tile::furnace_Id, 0, x, y, z);
		case HouseDecorationType_Workbench:
			return PlaceSingleDecoration(level, Tile::workBench_Id, 0, x, y, z);
		case HouseDecorationType_Cauldron:
			return PlaceSingleDecoration(level, Tile::cauldron_Id, 0, x, y, z);
		case HouseDecorationType_VerticalBookshelfPair:
			return PlaceVerticalBookshelfPair(level, x, y, z);
		default:
			return false;
		}
	}

	static void DecorateHouseInterior(Level *level, int64_t houseSeed, const HouseLayout &layout)
	{
		Random decorRandom;
		decorRandom.setSeed(houseSeed ^ 0x3C6EF372FE94F82ALL);

		int primaryWalls[2] = { 0, 0 };
		int primaryWallCount = 0;
		GatherPrimaryWalls(layout, primaryWalls, &primaryWallCount);
		if (primaryWallCount <= 0)
		{
			return;
		}

		const int primaryWall = primaryWalls[decorRandom.nextInt(primaryWallCount)];
		const int oppositeWall = GetOppositeWall(primaryWall);

		int primarySlotXs[3] = { 0, 0, 0 };
		int primarySlotZs[3] = { 0, 0, 0 };
		int primarySlotCount = 0;
		GatherPrimaryDecorationSlots(layout, primaryWall, primarySlotXs, primarySlotZs, &primarySlotCount);
		FilterDoorApproachSlots(layout, level, primarySlotXs, primarySlotZs, &primarySlotCount);
		ShuffleSlots(&decorRandom, primarySlotXs, primarySlotZs, primarySlotCount);

		int decorations[5] =
		{
			HouseDecorationType_LootChest,
			HouseDecorationType_Furnace,
			HouseDecorationType_Workbench,
			HouseDecorationType_Cauldron,
			HouseDecorationType_VerticalBookshelfPair
		};
		ShuffleIntArray(&decorRandom, decorations, 5);

		const int requestedPrimaryCount = decorRandom.nextInt(4);
		const int targetPrimaryCount = requestedPrimaryCount < primarySlotCount ? requestedPrimaryCount : primarySlotCount;
		int placedPrimaryCount = 0;
		for (int decorationIndex = 0; decorationIndex < 5 && placedPrimaryCount < targetPrimaryCount; ++decorationIndex)
		{
			if (PlacePrimaryDecoration(level, &decorRandom, primarySlotXs[placedPrimaryCount], layout.y0, primarySlotZs[placedPrimaryCount], static_cast<HouseDecorationType>(decorations[decorationIndex])))
			{
				++placedPrimaryCount;
			}
		}

		int oppositeSlotXs[3] = { 0, 0, 0 };
		int oppositeSlotZs[3] = { 0, 0, 0 };
		int oppositeSlotCount = 0;
		GatherOppositeWallSetSlots(layout, oppositeWall, oppositeSlotXs, oppositeSlotZs, &oppositeSlotCount);
		FilterDoorApproachSlots(layout, level, oppositeSlotXs, oppositeSlotZs, &oppositeSlotCount);
		ShuffleSlots(&decorRandom, oppositeSlotXs, oppositeSlotZs, oppositeSlotCount);

		int oppositeSets[2] =
		{
			OppositeWallSetType_HorizontalBookshelfLine,
			OppositeWallSetType_StairFenceStair
		};
		ShuffleIntArray(&decorRandom, oppositeSets, 2);

		const int requestedOppositeSetCount = decorRandom.nextInt(3);
		const int targetOppositeSetCount = requestedOppositeSetCount < oppositeSlotCount ? requestedOppositeSetCount : oppositeSlotCount;
		int placedOppositeSetCount = 0;
		for (int setIndex = 0; setIndex < 2 && placedOppositeSetCount < targetOppositeSetCount; ++setIndex)
		{
			if (PlaceOppositeWallSet(level, oppositeSlotXs[placedOppositeSetCount], layout.y0, oppositeSlotZs[placedOppositeSetCount], oppositeWall, static_cast<OppositeWallSetType>(oppositeSets[setIndex])))
			{
				++placedOppositeSetCount;
			}
		}
	}

	static bool FindHousePlacement(Level *level, int64_t houseSeed, int anchorX, int anchorY, int anchorZ, int *outX, int *outY, int *outZ)
	{
		const int chunkX = GetChunkCoordinate(anchorX);
		const int chunkZ = GetChunkCoordinate(anchorZ);

		Random searchRandom;
		searchRandom.setSeed(houseSeed ^ 0x6A09E667F3BCC909LL);

		for (int attempt = 0; attempt < 96; ++attempt)
		{
			const int x = (chunkX << 4) + 2 + searchRandom.nextInt(12);
			const int z = (chunkZ << 4) + 2 + searchRandom.nextInt(12);

			int y = level->getHeightmap(x, z);
			if (y <= 0)
			{
				y = anchorY;
			}

			Random layoutRandom;
			layoutRandom.setSeed(houseSeed);
			HouseLayout layout;
			if (PrepareHouseLayout(level, &layoutRandom, x, y, z, layout))
			{
				*outX = x;
				*outY = y;
				*outZ = z;
				return true;
			}
		}

		Random layoutRandom;
		layoutRandom.setSeed(houseSeed);
		HouseLayout layout;
		if (PrepareHouseLayout(level, &layoutRandom, anchorX, anchorY, anchorZ, layout))
		{
			*outX = anchorX;
			*outY = anchorY;
			*outZ = anchorZ;
			return true;
		}

		return false;
	}

	static bool FindMushroomPlacement(Level *level, int x, int y, int z, int *outX, int *outY, int *outZ)
	{
		const int chunkX = GetChunkCoordinate(x);
		const int chunkZ = GetChunkCoordinate(z);
		Random searchRandom;
		searchRandom.setSeed((((int64_t)x) << 32) ^ (int64_t)z ^ 0x4F1BBCDCBFA54001LL);
		const int minBlock = GetFiniteWorldMinBlock(level);
		const int maxBlock = GetFiniteWorldMaxBlock(level);

		for (int attempt = 0; attempt < 48; ++attempt)
		{
			// Keep the center inside the chunk interior so the huge mushroom canopy
			// does not straddle a chunk boundary and get visually clipped.
			const int xx = (chunkX << 4) + 3 + searchRandom.nextInt(10);
			const int zz = (chunkZ << 4) + 3 + searchRandom.nextInt(10);
			if (xx < minBlock || xx > maxBlock || zz < minBlock || zz > maxBlock)
			{
				continue;
			}

			int yy = level->getHeightmap(xx, zz);
			if (yy <= 0)
			{
				yy = level->getTopSolidBlock(xx, zz);
			}
			if (yy <= 0)
			{
				yy = y;
			}

			const int groundY = yy - 1;
			if (groundY <= 0)
			{
				continue;
			}

			const int groundTile = level->getTile(xx, groundY, zz);
			if (groundTile != Tile::dirt_Id && groundTile != Tile::grass_Id && groundTile != Tile::mycel_Id)
			{
				continue;
			}

			Material *groundMaterial = level->getMaterial(xx, groundY, zz);
			if (groundMaterial == nullptr || !groundMaterial->blocksMotion() || groundMaterial->isLiquid())
			{
				continue;
			}

			*outX = xx;
			*outY = yy;
			*outZ = zz;
			return true;
		}

		return false;
	}

	static bool HasNearbyVillageHouseDoor(Level *level, int x, int y, int z)
	{
		const int searchRadius = 40;
		const int minBlock = GetFiniteWorldMinBlock(level);
		const int maxBlock = GetFiniteWorldMaxBlock(level);
		const int minX = max(x - searchRadius, minBlock);
		const int maxX = min(x + searchRadius, maxBlock);
		const int minZ = max(z - searchRadius, minBlock);
		const int maxZ = min(z + searchRadius, maxBlock);
		const int minY = (y - 8) > 1 ? (y - 8) : 1;
		const int maxY = (y + 8) < (Level::genDepth - 1) ? (y + 8) : (Level::genDepth - 1);

		for (int xx = minX; xx <= maxX; ++xx)
		{
			for (int zz = minZ; zz <= maxZ; ++zz)
			{
				for (int yy = minY; yy <= maxY; ++yy)
				{
					if (level->getTile(xx, yy, zz) == Tile::door_wood_Id)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	static bool TrySpawnMooshroomNextToVillageMushroom(Level *level, Random *random, int mushroomX, int mushroomY, int mushroomZ)
	{
		if (random->nextInt(5) != 0 || !level->canCreateMore(eTYPE_MUSHROOMCOW, Level::eSpawnType_Breed))
		{
			return false;
		}

		static const int spawnOffsets[8][2] =
		{
			{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 },
			{ 1, 1 }, { 1, -1 }, { -1, 1 }, { -1, -1 }
		};

		for (int attempt = 0; attempt < 8; ++attempt)
		{
			const int index = (attempt + random->nextInt(8)) & 7;
			const int spawnX = mushroomX + spawnOffsets[index][0];
			const int spawnZ = mushroomZ + spawnOffsets[index][1];
			if (!IsInsideFiniteWorld(level, spawnX, spawnZ))
			{
				continue;
			}

			for (int groundY = mushroomY - 3; groundY <= mushroomY + 1; ++groundY)
			{
				if (groundY <= 0)
				{
					continue;
				}

				if (level->getTile(spawnX, groundY, spawnZ) != Tile::grass_Id)
				{
					continue;
				}

				if (level->getTile(spawnX, groundY + 1, spawnZ) != 0 || level->getTile(spawnX, groundY + 2, spawnZ) != 0)
				{
					continue;
				}

				shared_ptr<MushroomCow> mushroomCow = std::make_shared<MushroomCow>(level);
				mushroomCow->moveTo(spawnX + 0.5, groundY + 1, spawnZ + 0.5, random->nextFloat() * 360.0f, 0.0f);
				level->addEntity(mushroomCow);
				return true;
			}
		}

		return false;
	}

	static bool IsAllowedVillageBiome(Biome *biome)
	{
		return biome == Biome::betaSwampland ||
			biome == Biome::betaPlains ||
			biome == Biome::betaShrubland ||
			biome == Biome::betaSavanna ||
			biome == Biome::betaDesert ||
			biome == Biome::betaTundra;
	}

	static HouseMaterialPalette GetHouseMaterialPalette(Biome *biome)
	{
		if (biome == Biome::betaDesert)
		{
			return HouseMaterialPalette_Desert;
		}

		if (biome == Biome::betaTundra)
		{
			return HouseMaterialPalette_Tundra;
		}

		return HouseMaterialPalette_Normal;
	}

	static bool AllowsVillageMushrooms(HouseMaterialPalette palette)
	{
		return palette == HouseMaterialPalette_Normal;
	}

	static void ApplyHouseMaterialPalette(Level *level, BoundingBox *box, HouseMaterialPalette palette)
	{
		if (box == nullptr || palette == HouseMaterialPalette_Normal)
		{
			return;
		}

		for (int xx = box->x0; xx <= box->x1; ++xx)
		{
			for (int yy = box->y0; yy <= box->y1; ++yy)
			{
				for (int zz = box->z0; zz <= box->z1; ++zz)
				{
					const int tile = level->getTile(xx, yy, zz);
					if (tile <= 0)
					{
						continue;
					}

					if (palette == HouseMaterialPalette_Desert)
					{
						if (tile == Tile::cobblestone_Id)
						{
							level->setTileAndData(xx, yy, zz, Tile::sandStone_Id, SandStoneTile::TYPE_DEFAULT, Tile::UPDATE_CLIENTS);
						}
						else if (tile == Tile::mossyCobblestone_Id)
						{
							level->setTileAndData(xx, yy, zz, Tile::sandStone_Id, SandStoneTile::TYPE_HEIROGLYPHS, Tile::UPDATE_CLIENTS);
						}
						else if (tile == Tile::wood_Id)
						{
							level->setTileAndData(xx, yy, zz, Tile::sandStone_Id, SandStoneTile::TYPE_SMOOTHSIDE, Tile::UPDATE_CLIENTS);
						}
					}
					else if (palette == HouseMaterialPalette_Tundra)
					{
						if (tile == Tile::wood_Id)
						{
							level->setTileAndData(xx, yy, zz, Tile::wood_Id, 1, Tile::UPDATE_CLIENTS);
						}
					}
				}
			}
		}
	}

	static bool CreatePlannedHousePiece(Level *level, list<StructurePiece *> &pieces, Random *random, int iXZSize, int x, int y, int z, HouseMaterialPalette palette)
	{
		if (!IsAllowedVillageBiome(level->getBiome(x, z)))
		{
			return false;
		}

		const int64_t houseSeed = random->nextLong();
		return TryAppendPiece(pieces, new BetaOldVillageFeature::OldHousePiece(houseSeed, x, y, z, palette), iXZSize);
	}
}

vector<Biome *> BetaOldVillageFeature::allowedBiomes;

void BetaOldVillageFeature::staticCtor()
{
	if (!allowedBiomes.empty())
	{
		return;
	}

	allowedBiomes.push_back(Biome::betaPlains);
	allowedBiomes.push_back(Biome::betaSwampland);
	allowedBiomes.push_back(Biome::betaSavanna);
	allowedBiomes.push_back(Biome::betaShrubland);
	allowedBiomes.push_back(Biome::betaDesert);
	allowedBiomes.push_back(Biome::betaTundra);
}

BetaOldVillageFeature::BetaOldVillageFeature(int iXZSize)
{
	m_iXZSize = iXZSize;
	m_spacing = 16;
	m_minSeparation = 8;
}

wstring BetaOldVillageFeature::getFeatureName()
{
	return L"BetaOldVillage";
}

bool BetaOldVillageFeature::isFeatureChunk(int x, int z, bool)
{
	int xx = x;
	int zz = z;
	if (x < 0) x -= m_spacing - 1;
	if (z < 0) z -= m_spacing - 1;

	int xRegion = x / m_spacing;
	int zRegion = z / m_spacing;
	Random *r = level->getRandomFor(xRegion, zRegion, 14357619);
	xRegion *= m_spacing;
	zRegion *= m_spacing;
	xRegion += r->nextInt(m_spacing - m_minSeparation);
	zRegion += r->nextInt(m_spacing - m_minSeparation);

	if (xx != xRegion || zz != zRegion)
	{
		return false;
	}

	return level->getBiomeSource()->containsOnly(xx * 16 + 7, zz * 16 + 7, 8, allowedBiomes);
}

StructureStart *BetaOldVillageFeature::createStructureStart(int x, int z)
{
	app.AddTerrainFeaturePosition(eTerrainFeature_Village, x, z);
	return new OldVillageStart(level, random, x, z, m_iXZSize);
}

BetaOldVillageFeature::OldVillageStart::OldVillageStart()
{
	valid = false;
	m_iXZSize = 0;
}

BetaOldVillageFeature::OldVillageStart::OldVillageStart(Level *level, Random *random, int chunkX, int chunkZ, int iXZSize)
	: StructureStart(chunkX, chunkZ)
{
	valid = false;
	m_iXZSize = iXZSize;

	Biome *sourceBiome = level->getBiome((chunkX << 4) + 7, (chunkZ << 4) + 7);
	const HouseMaterialPalette materialPalette = GetHouseMaterialPalette(sourceBiome);
	const int centerY = Level::genDepth / 2;
	const int targetHouseCount = 3 + random->nextInt(9);
	int appendedHouseCount = 0;
	vector<pair<int, int>> plannedHouseAnchors;

	// Always plan one house in the source chunk so the F3 village position still
	// corresponds to a real house attempt in the start chunk.
	if (!CreatePlannedHousePiece(level, pieces, random, m_iXZSize, (chunkX << 4) + 7, centerY, (chunkZ << 4) + 7, materialPalette))
	{
		return;
	}
	++appendedHouseCount;
	plannedHouseAnchors.push_back(std::make_pair((chunkX << 4) + 7, (chunkZ << 4) + 7));

	vector<pair<int, int>> candidateOffsets;
	candidateOffsets.reserve(24);
	for (int dx = -2; dx <= 2; ++dx)
	{
		for (int dz = -2; dz <= 2; ++dz)
		{
			if (dx == 0 && dz == 0)
			{
				continue;
			}

			candidateOffsets.push_back(std::make_pair(dx, dz));
		}
	}

	for (int i = static_cast<int>(candidateOffsets.size()) - 1; i > 0; --i)
	{
		const int swapIndex = random->nextInt(i + 1);
		const pair<int, int> temp = candidateOffsets[i];
		candidateOffsets[i] = candidateOffsets[swapIndex];
		candidateOffsets[swapIndex] = temp;
	}

	for (size_t i = 0; i < candidateOffsets.size() && appendedHouseCount < targetHouseCount; ++i)
	{
		const int candidateChunkX = chunkX + candidateOffsets[i].first;
		const int candidateChunkZ = chunkZ + candidateOffsets[i].second;
		const int houseX = (candidateChunkX << 4) + 7;
		const int houseZ = (candidateChunkZ << 4) + 7;

		if (CreatePlannedHousePiece(level, pieces, random, m_iXZSize, houseX, centerY, houseZ, materialPalette))
		{
			++appendedHouseCount;
			plannedHouseAnchors.push_back(std::make_pair(houseX, houseZ));
		}
	}

	if (appendedHouseCount < 3)
	{
		return;
	}

	const int mushroomCount = (appendedHouseCount >= 3 && AllowsVillageMushrooms(materialPalette)) ? (1 + random->nextInt(3)) : 0;
	for (int i = 0; i < mushroomCount; ++i)
	{
		const pair<int, int> &anchor = plannedHouseAnchors[random->nextInt(static_cast<int>(plannedHouseAnchors.size()))];
		const int offsetX = random->nextInt(33) - 16;
		const int offsetZ = random->nextInt(33) - 16;
		int mushroomX = anchor.first + offsetX;
		int mushroomZ = anchor.second + offsetZ;
		const int mushroomChunkX = GetChunkCoordinate(mushroomX);
		const int mushroomChunkZ = GetChunkCoordinate(mushroomZ);
		mushroomX = (mushroomChunkX << 4) + 3 + random->nextInt(10);
		mushroomZ = (mushroomChunkZ << 4) + 3 + random->nextInt(10);
		const int mushroomType = random->nextInt(2);

		TryAppendDecorPiece(pieces, new OldMushroomPiece(random, mushroomX, centerY, mushroomZ, mushroomType), m_iXZSize);
	}

	if (pieces.empty())
	{
		return;
	}

	calculateBoundingBox();

	if (boundingBox != nullptr)
	{
		valid = true;
	}
}

bool BetaOldVillageFeature::OldVillageStart::isValid()
{
	return valid;
}

void BetaOldVillageFeature::OldVillageStart::addAdditonalSaveData(CompoundTag *tag)
{
	StructureStart::addAdditonalSaveData(tag);
	tag->putBoolean(L"Valid", valid);
	tag->putInt(L"XZSize", m_iXZSize);
}

void BetaOldVillageFeature::OldVillageStart::readAdditonalSaveData(CompoundTag *tag)
{
	StructureStart::readAdditonalSaveData(tag);
	valid = tag->getBoolean(L"Valid");
	if (tag->contains(L"XZSize"))
	{
		m_iXZSize = tag->getInt(L"XZSize");
	}
	if (boundingBox == nullptr && !pieces.empty())
	{
		calculateBoundingBox();
	}
}

BetaOldVillageFeature::OldHousePiece::OldHousePiece() : StructurePiece(0)
{
	generated = false;
	houseSeed = 0;
	anchorX = anchorY = anchorZ = 0;
	materialPalette = HouseMaterialPalette_Normal;
}

BetaOldVillageFeature::OldHousePiece::OldHousePiece(int64_t houseSeed, int x, int y, int z, int materialPalette) : StructurePiece(0)
{
	generated = false;
	this->houseSeed = houseSeed;
	anchorX = x;
	anchorY = y;
	anchorZ = z;
	this->materialPalette = materialPalette;

	// Keep a generous footprint around the anchor so structure caching and
	// intersection checks behave like a normal structure, while the actual
	// house generation is still delegated to the legacy one-shot feature.
	boundingBox = new BoundingBox(x - 12, y - 1, z - 12, x + 12, y + 16, z + 12);
}

bool BetaOldVillageFeature::OldHousePiece::postProcess(Level *level, Random *random, BoundingBox *chunkBB)
{
	if (generated || chunkBB == nullptr)
	{
		return true;
	}

	// This legacy house generator places the whole footprint at once and does not
	// use chunk-bounded piece placement. Only allow the source chunk to trigger
	// generation so we don't fail/remove the piece from a neighboring intersecting chunk.
	if (!chunkBB->intersects(anchorX, anchorZ, anchorX, anchorZ))
	{
		return true;
	}

	Random houseRandom;
	houseRandom.setSeed(houseSeed);

	int placeX = anchorX;
	int placeY = anchorY;
	int placeZ = anchorZ;
	if (!FindHousePlacement(level, houseSeed, anchorX, anchorY, anchorZ, &placeX, &placeY, &placeZ))
	{
		return false;
	}

	Random layoutRandom;
	layoutRandom.setSeed(houseSeed);
	HouseLayout layout;
	if (!PrepareHouseLayout(level, &layoutRandom, placeX, placeY, placeZ, layout))
	{
		return false;
	}
	FinalizeHouseLayout(layout);

	HouseFeature house;
	if (!house.place(level, &houseRandom, placeX, placeY, placeZ))
	{
		return false;
	}

	BoundingBox placedBounds(placeX - 12, placeY - 1, placeZ - 12, placeX + 12, placeY + 16, placeZ + 12);
	ApplyHouseMaterialPalette(level, &placedBounds, static_cast<HouseMaterialPalette>(materialPalette));
	DecorateHouseInterior(level, houseSeed, layout);
	if (level->villages != nullptr)
	{
		int doorX = 0;
		int doorZ = 0;
		if (FindHouseDoor(layout, level, &doorX, &doorZ))
		{
			level->villages->queryUpdateAround(doorX, layout.y0, doorZ);
		}

		level->villages->queryUpdateAround(placeX, layout.y0, placeZ);
	}
	generated = true;
	return true;
}

void BetaOldVillageFeature::OldHousePiece::addAdditonalSaveData(CompoundTag *tag)
{
	tag->putBoolean(L"Generated", generated);
	tag->putLong(L"HouseSeed", houseSeed);
	tag->putInt(L"AnchorX", anchorX);
	tag->putInt(L"AnchorY", anchorY);
	tag->putInt(L"AnchorZ", anchorZ);
	tag->putInt(L"MaterialPalette", materialPalette);
}

void BetaOldVillageFeature::OldHousePiece::readAdditonalSaveData(CompoundTag *tag)
{
	generated = tag->getBoolean(L"Generated");
	houseSeed = tag->contains(L"HouseSeed") ? tag->getLong(L"HouseSeed") : 0;
	anchorX = tag->getInt(L"AnchorX");
	anchorY = tag->contains(L"AnchorY") ? tag->getInt(L"AnchorY") : Level::genDepth / 2;
	anchorZ = tag->getInt(L"AnchorZ");
	materialPalette = tag->contains(L"MaterialPalette") ? tag->getInt(L"MaterialPalette") : HouseMaterialPalette_Normal;
	if (boundingBox == nullptr)
	{
		boundingBox = new BoundingBox(anchorX - 12, anchorY - 1, anchorZ - 12, anchorX + 12, anchorY + 16, anchorZ + 12);
	}
}

BetaOldVillageFeature::OldMushroomPiece::OldMushroomPiece() : StructurePiece(0)
{
	generated = false;
	featureSeed = 0;
	anchorX = anchorY = anchorZ = 0;
	mushroomType = 0;
}

BetaOldVillageFeature::OldMushroomPiece::OldMushroomPiece(Random *random, int x, int y, int z, int mushroomType) : StructurePiece(0)
{
	generated = false;
	featureSeed = random->nextLong();
	anchorX = x;
	anchorY = y;
	anchorZ = z;
	this->mushroomType = mushroomType;
	// Keep mushroom pieces alive across the nearby village area so they can be
	// generated from any intersecting chunk instead of only from one exact
	// anchor chunk.
	boundingBox = new BoundingBox(x - 32, y - 1, z - 32, x + 32, y + 16, z + 32);
}

bool BetaOldVillageFeature::OldMushroomPiece::postProcess(Level *level, Random *random, BoundingBox *chunkBB)
{
	if (generated || chunkBB == nullptr)
	{
		return true;
	}

	if (!HasNearbyVillageHouseDoor(level, anchorX, anchorY, anchorZ))
	{
		return true;
	}

	Random featureRandom;
	featureRandom.setSeed(featureSeed);

	int placeX = anchorX;
	int placeY = anchorY;
	int placeZ = anchorZ;
	if (!FindMushroomPlacement(level, anchorX, anchorY, anchorZ, &placeX, &placeY, &placeZ))
	{
		return true;
	}

	HugeMushroomFeature mushroom(mushroomType);
	if (!mushroom.place(level, &featureRandom, placeX, placeY, placeZ))
	{
		return true;
	}

	TrySpawnMooshroomNextToVillageMushroom(level, &featureRandom, placeX, placeY, placeZ);

	generated = true;
	return true;
}

void BetaOldVillageFeature::OldMushroomPiece::addAdditonalSaveData(CompoundTag *tag)
{
	tag->putBoolean(L"Generated", generated);
	tag->putLong(L"FeatureSeed", featureSeed);
	tag->putInt(L"AnchorX", anchorX);
	tag->putInt(L"AnchorY", anchorY);
	tag->putInt(L"AnchorZ", anchorZ);
	tag->putInt(L"MushroomType", mushroomType);
}

void BetaOldVillageFeature::OldMushroomPiece::readAdditonalSaveData(CompoundTag *tag)
{
	generated = tag->getBoolean(L"Generated");
	featureSeed = tag->contains(L"FeatureSeed") ? tag->getLong(L"FeatureSeed") : 0;
	anchorX = tag->getInt(L"AnchorX");
	anchorY = tag->contains(L"AnchorY") ? tag->getInt(L"AnchorY") : Level::genDepth / 2;
	anchorZ = tag->getInt(L"AnchorZ");
	mushroomType = tag->contains(L"MushroomType") ? tag->getInt(L"MushroomType") : 0;
	if (boundingBox == nullptr)
	{
		boundingBox = new BoundingBox(anchorX - 32, anchorY - 1, anchorZ - 32, anchorX + 32, anchorY + 16, anchorZ + 32);
	}
}
