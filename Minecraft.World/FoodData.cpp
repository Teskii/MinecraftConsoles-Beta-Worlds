#include "stdafx.h"
#include "net.minecraft.world.item.h"
#include "net.minecraft.world.entity.player.h"
#include "net.minecraft.world.level.h"
#include "net.minecraft.world.h"
#include "net.minecraft.world.damagesource.h"
#include "com.mojang.nbt.h"
#include "FoodConstants.h"
#include "FoodData.h"

FoodData::FoodData()
{
	exhaustionLevel = 0;
	tickTimer = 0;

	foodLevel = FoodConstants::MAX_FOOD;
	lastFoodLevel = FoodConstants::MAX_FOOD;
	saturationLevel = FoodConstants::START_SATURATION;
}

void FoodData::eat(int food, float saturationModifier)
{
	foodLevel = FoodConstants::MAX_FOOD;
	lastFoodLevel = FoodConstants::MAX_FOOD;
	saturationLevel = FoodConstants::START_SATURATION;
	exhaustionLevel = 0.0f;
}

void FoodData::eat(FoodItem *item)
{
	eat(item->getNutrition(), item->getSaturationModifier());
}

void FoodData::tick(shared_ptr<Player> player)
{
	foodLevel = FoodConstants::MAX_FOOD;
	lastFoodLevel = FoodConstants::MAX_FOOD;
	saturationLevel = FoodConstants::START_SATURATION;
	exhaustionLevel = 0.0f;
	tickTimer = 0;
}

void FoodData::readAdditionalSaveData(CompoundTag *entityTag)
{

	if (entityTag->contains(L"foodLevel"))
	{
		foodLevel = entityTag->getInt(L"foodLevel");
		tickTimer = entityTag->getInt(L"foodTickTimer");
		saturationLevel = entityTag->getFloat(L"foodSaturationLevel");
		exhaustionLevel = entityTag->getFloat(L"foodExhaustionLevel");
	}
}

void FoodData::addAdditonalSaveData(CompoundTag *entityTag)
{
	entityTag->putInt(L"foodLevel", foodLevel);
	entityTag->putInt(L"foodTickTimer", tickTimer);
	entityTag->putFloat(L"foodSaturationLevel", saturationLevel);
	entityTag->putFloat(L"foodExhaustionLevel", exhaustionLevel);
}

int FoodData::getFoodLevel()
{
	return foodLevel;
}

int FoodData::getLastFoodLevel()
{
	return lastFoodLevel;
}

bool FoodData::needsFood()
{
	return foodLevel < FoodConstants::MAX_FOOD;
}

void FoodData::addExhaustion(float amount)
{
	exhaustionLevel = 0.0f;
}

float FoodData::getExhaustionLevel()
{
	return exhaustionLevel;
}

float FoodData::getSaturationLevel()
{
	return saturationLevel;
}

void FoodData::setFoodLevel(int food)
{
	foodLevel = food;
}

void FoodData::setSaturation(float saturation)
{
	saturationLevel = saturation;
}

void FoodData::setExhaustion(float exhaustion)
{
	exhaustionLevel = exhaustion;
}
