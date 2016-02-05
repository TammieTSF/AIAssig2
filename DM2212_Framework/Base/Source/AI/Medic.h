#pragma once

#include "../GameObject.h"
class CMedic
{
public:
	enum MEDIC
	{
		IDLE,
		HEAL,

		HEALING,
		HEALED,
	};

	float medicPos, assistantPos, safehousePos, safehouseRadius;
};

