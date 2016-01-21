#pragma once
class CGShooter
{
public:
	enum SHOOTER
	{
		ALIVE,
		SHOOT,
		RETREAT,
		RETURN,
		DIE,
		IDLE, // BACKUP
		REFILL, // BACKUP
		REPLACE, // BACKUP
		RETURNB, // BACKUP

		HEALING,
		HEALED,
		RETURNING,
		RETURNED,
	};
};

