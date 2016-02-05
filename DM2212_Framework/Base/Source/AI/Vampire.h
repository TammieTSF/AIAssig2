#pragma once
#include "../GameObject.h"


class CVampire : public GameObject
{
public:
	enum VAMPIRE
	{
		MOVE,
		DESTROY,
		HIT,
		DIE,
	};
};

