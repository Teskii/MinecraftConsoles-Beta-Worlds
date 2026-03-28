#pragma once

#include "ArrayWithLength.h"

class FoliageColor
{
private:
	static intArray pixels;

public:
	static void init(intArray pixels);
	static int get(double temp, double rain);

public:
	static int getEvergreenColor();
	static int getBirchColor();
	static int getDefaultColor();
};
