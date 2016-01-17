#pragma once

class CBullet
{
public:
	// Bullet
	enum BULLET
	{
		INACTIVE = 0,
		FIRED,
		MOVING,
		HIT,
	};

	int bulletLeft, bulletGiven;
	float bulletStartPos;
};

