#pragma once
#include "../GameObject.h"

class CGShooter : public GameObject
{
public:
	enum SHOOTER
	{
		ALIVE,
		INJURED,
		RETURN,
		DIED,
	};

	bool Lane1Empty;
	bool Lane2Empty;
	bool Lane3Empty;
};

