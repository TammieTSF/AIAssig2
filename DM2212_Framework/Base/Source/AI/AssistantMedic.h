#pragma once
#include "../GameObject.h"

class CAssistant : public GameObject
{
public:
	enum ASSISTANT
	{
		IDLE,
		HELP, // FOR ASSISTANT, HELP THE MAIN MEDIC
		HEAL,

		HEALING,
		HEALED,

		HELPING,
		HELPED,
	};
};