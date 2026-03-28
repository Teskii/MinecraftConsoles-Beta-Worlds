#pragma once

#include "StructureFeature.h"
#include "StructurePiece.h"
#include "StructureStart.h"

class Biome;

class BetaOldVillageFeature : public StructureFeature
{
public:
	static void staticCtor();
	static vector<Biome *> allowedBiomes;

	BetaOldVillageFeature(int iXZSize);
	virtual wstring getFeatureName();

protected:
	virtual bool isFeatureChunk(int x, int z, bool bIsSuperflat = false);
	virtual StructureStart *createStructureStart(int x, int z);

private:
	int m_iXZSize;
	int m_spacing;
	int m_minSeparation;

public:
	class OldVillageStart : public StructureStart
	{
	public:
		static StructureStart *Create() { return new OldVillageStart(); }
		virtual EStructureStart GetType() { return eStructureStart_BetaOldVillageStart; }

		OldVillageStart();
		OldVillageStart(Level *level, Random *random, int chunkX, int chunkZ, int iXZSize);

		virtual bool isValid();
		virtual void addAdditonalSaveData(CompoundTag *tag);
		virtual void readAdditonalSaveData(CompoundTag *tag);

	private:
		bool valid;
		int m_iXZSize;
	};

	class OldHousePiece : public StructurePiece
	{
	public:
		static StructurePiece *Create() { return new OldHousePiece(); }
		virtual EStructurePiece GetType() { return eStructurePiece_BetaOldHousePiece; }

		OldHousePiece();
		OldHousePiece(int64_t houseSeed, int x, int y, int z, int materialPalette);

		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

	protected:
		virtual void addAdditonalSaveData(CompoundTag *tag);
		virtual void readAdditonalSaveData(CompoundTag *tag);

	private:
		bool generated;
		int64_t houseSeed;
		int anchorX;
		int anchorY;
		int anchorZ;
		int materialPalette;
	};

	class OldMushroomPiece : public StructurePiece
	{
	public:
		static StructurePiece *Create() { return new OldMushroomPiece(); }
		virtual EStructurePiece GetType() { return eStructurePiece_BetaOldMushroomPiece; }

		OldMushroomPiece();
		OldMushroomPiece(Random *random, int x, int y, int z, int mushroomType);

		virtual bool postProcess(Level *level, Random *random, BoundingBox *chunkBB);

	protected:
		virtual void addAdditonalSaveData(CompoundTag *tag);
		virtual void readAdditonalSaveData(CompoundTag *tag);

	private:
		bool generated;
		int64_t featureSeed;
		int anchorX;
		int anchorY;
		int anchorZ;
		int mushroomType;
	};
};
