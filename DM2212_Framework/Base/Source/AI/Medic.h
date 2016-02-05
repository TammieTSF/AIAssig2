#pragma once

#include "../GameObject.h"
class CMedic
{
public:
	enum MEDIC
	{
		IDLE,
		HEAL,
	};

	float medicPos, assistantPos, safehousePos, safehouseRadius;
};

