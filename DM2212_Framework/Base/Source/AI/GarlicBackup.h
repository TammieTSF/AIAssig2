#pragma once
#include "../GameObject.h"
class CGBackup : public GameObject
{
public:
	enum BACKUP
	{
		ALIVE,
		SHOOT,
		RETURN, // return to lane
	
		IDLE,
		REFILL,
		REPLACE,
		RETURNP, // return to being refiller
		DIE,
	};

	float AmmoPos, AmmoRadius;
	int AmmoCount, AmmoLimit;

	bool Lane1Empty;
	bool Lane2Empty;
	bool Lane3Empty;
};

