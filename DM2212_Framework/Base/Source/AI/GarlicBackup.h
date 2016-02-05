#pragma once
#include "../GameObject.h"
class CGBackup : public GameObject
{
public:
	enum BACKUP
	{
		ALIVE,
		IDLE,
		REFILL,
		REPLACE,
		REPLACING,	// REPLACING ONE SHOOTER ALREADY
		RETURNB,	// RETURN TO BEING A REFILLER
		INJURED,
		RETURNL,	// RETURN TO LANE IF INJRUED
		DIE,
	};

	float AmmoPos, AmmoRadius;
	int AmmoCount, AmmoLimit;

	bool Lane1Empty;
	bool Lane2Empty;
	bool Lane3Empty;
};

