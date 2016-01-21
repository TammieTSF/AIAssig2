#pragma once
class CMedic
{
public:
	enum MEDIC
	{
		IDLE,
		HEAL,
		HELP, // FOR ASSISTANT, HELP THE MAIN MEDIC

		HEALING,
		HEALED,

		HELPING,
		HELPED,
	};

	float medicPos, assistantPos, safehousePos, safehouseRadius;
};

