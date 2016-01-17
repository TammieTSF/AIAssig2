#pragma once
class CSupplier
{
public:
	enum SUPPLIER
	{
		STANDBY,
		DELIVERING,
		DELIVERED,
	};

	float supplierPos;
	const int cycleLimit = 2;
	int cycle;
};

