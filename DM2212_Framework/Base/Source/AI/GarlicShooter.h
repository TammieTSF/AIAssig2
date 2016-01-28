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

		HEALING,
		HEALED,
		RETURNING,
		RETURNED,
	};

	bool Lane1Empty;
	bool Lane2Empty;
	bool Lane3Empty;
};

