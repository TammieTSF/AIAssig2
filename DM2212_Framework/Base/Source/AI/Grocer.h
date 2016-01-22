#pragma once
class CGrocer
{
public:
	enum GROCER
	{
		IDLE,
		SUPPLY,
		RUN,

		SUPPLYING,
		SUPPLIED,
	};

	float storeRadius, storePos;
	int storeCount, storeLimit;
	const int cycleLimit = 2;
	int cycle;
};

