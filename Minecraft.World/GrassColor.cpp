#include "stdafx.h"
#include "..\Minecraft.Client\Minecraft.h"
#include "GrassColor.h"

intArray GrassColor::pixels;

void GrassColor::init(intArray pixels)
{
	int *oldData = GrassColor::pixels.data;
	GrassColor::pixels = pixels;
	delete [] oldData;
}

int GrassColor::get(double temp, double rain)
{
	if (pixels.data == nullptr || pixels.length < (256 * 256))
	{
		return Minecraft::GetInstance()->getColourTable()->getColor(eMinecraftColour_Grass_Common);
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
