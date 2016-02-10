#pragma once
#include "../GameObject.h"


class CVampire : public GameObject
{
public:
	enum VAMPIRE
	{
		STAY,
		MOVE,
		DESTROY,
		HIT,
		DIE,
		STAY,
	};
};

