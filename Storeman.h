#pragma once
class CStoreman
{
public:
	enum STOREMAN
	{
		STANDBY,
		REPLENISHING,
		REPLENISHED,
		REPLACING,
		ACTING,
	};

	float storeRadius, storePos;
	int storeCount, storeLimit;
};

