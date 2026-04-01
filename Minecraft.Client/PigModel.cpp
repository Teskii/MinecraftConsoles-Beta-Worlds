#include "stdafx.h"
#include "PigModel.h"
#include "ModelPart.h"

PigModel::PigModel() : QuadrupedModel(6, 0)
{
	yHeadOffs = 4;

	head->compile(1.0f/16.0f);
}

PigModel::PigModel(float grow) : QuadrupedModel(6, grow)
{
	yHeadOffs = 4;

	head->compile(1.0f/16.0f);
}

