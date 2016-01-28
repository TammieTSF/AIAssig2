#pragma once
class CGBackup
{
public:
	enum BACKUP
	{
		ALIVE,
		SHOOT,
		RETREAT,
		RETURN,
		
		IDLE,
		REFILL,
		REPLACE,
		DIE,
		
		HEALING,
		HEALED,
		RETURNING,
		RETURNED,
	};

	float AmmoPos, AmmoRadius;
	int AmmoCount, AmmoLimit;

	bool Lane1Empty;
	bool Lane2Empty;
	bool Lane3Empty;
};

