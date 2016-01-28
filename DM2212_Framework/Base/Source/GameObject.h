#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Vector3.h"
#include <string>

using namespace std;

struct GameObject
{
	enum GAMEOBJECT_TYPE
	{
		GO_NONE = 0,
		GO_BALL,
		GO_CUBE,

		GO_SHOOTER,
		GO_BSHOOTER,
		GO_MMEDIC,
		GO_AMEDIC,
		GO_VAMPIRE,
		GO_BULLET,
		GO_SUPPLIER,

		GO_TOTAL, //must be last
	};

public:
	GAMEOBJECT_TYPE type;

	Vector3 pos, vel, scale;

	string name, role;

	float speed, radius;
	float r, g, b;

	int state, health;

	bool active;

	GameObject(GAMEOBJECT_TYPE typeValue = GO_BALL);
	virtual ~GameObject(); // must be a virtual to handle all the children classes
	// Always inherit from parent classes, do not inherit from child classes.

	void SetName(string _name);
	void SetState(int _state);
	void SetHealth(int _health);
	void SetColor(float _r, float _g, float _b);
	void SetRole(string _role);
	void SetSpeed(float _speed);
	void SetRadius(float _radius);

	string GetName();
	string GetRole();
	int GetState();
	float GetSpeed();
	float GetRadius();
	float GetR();
	float GetG();
	float GetB();
};

#endif