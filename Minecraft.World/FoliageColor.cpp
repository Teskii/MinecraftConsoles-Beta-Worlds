#include "stdafx.h"
#include "..\Minecraft.Client\Minecraft.h"
#include "FoliageColor.h"

intArray FoliageColor::pixels;

void FoliageColor::init(intArray pixels)
{
	int *oldData = FoliageColor::pixels.data;
	FoliageColor::pixels = pixels;
	delete[] oldData;
}

int FoliageColor::get(double temp, double rain)
{
	if (pixels.data == nullptr || pixels.length < (256 * 256))
	{
		return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Foliage_Default);
	}

	if (temp < 0.0) temp = 0.0;
	else if (temp > 1.0) temp = 1.0;

	if (rain < 0.0) rain = 0.0;
	else if (rain > 1.0) rain = 1.0;

	rain *= temp;

	int x = static_cast<int>((1.0 - temp) * 255.0);
	int y = static_cast<int>((1.0 - rain) * 255.0);

	return pixels[(y << 8) | x];
}

int FoliageColor::getEvergreenColor()
{
	return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Foliage_Evergreen);
	//return 0x619961;
}

int FoliageColor::getBirchColor()
{
	return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Foliage_Birch);
	//return 0x80a755;
}

int FoliageColor::getDefaultColor()
{
	return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Foliage_Default);
	//return 0x48b518;
}
