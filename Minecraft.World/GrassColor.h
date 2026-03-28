#pragma once

#include "ArrayWithLength.h"

class GrassColor
{
private:
	static intArray pixels;

public:
	static void init(intArray pixels);
	static int get(double temp, double rain);
};
