#pragma once

class CGarlic
{
public:
	// Bullet
	enum GARLIC
	{
		INACTIVE = 0,
		FIRED,
		MOVING,
		HIT,
	};

	int garlicLeft, garlicGiven;
	float garlicStartPos;
};

