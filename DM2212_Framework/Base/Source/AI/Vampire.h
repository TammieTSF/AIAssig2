#pragma once
#include "../GameObject.h"


class CVampire : public GameObject
{
public:
	enum VAMPIRE
	{
		MOVE,
		STAY,
		CHANGELANE,
		DESTROY,
		DIE,
	};

	bool Lane1;
	bool Lane2;
	bool Lane3;
};

