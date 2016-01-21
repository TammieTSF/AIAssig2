#pragma once
class CMedic
{
public:
	enum MEDIC
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