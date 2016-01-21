#include <string>
#include "MyVector.h"

using namespace std;

class MyObject
{
public:
	string name, role;
	float speed, radius;
	MyVector position;
	int state, health;
	float r, g, b;

	MyObject();

	void SetName(string _name);
	void SetState(int _state);
	void SetHealth(int _health);
	void SetColor(float _r, float _g, float _b);
	void SetRole(string _role);
	void SetSpeed(float _speed);
	void SetRadius(float _radius);
	void SetPosition(float x, float y);
	void SetPosition(MyVector _position);

	string GetName();
	string GetRole();
	int GetState();
	float GetSpeed();
	float GetRadius();
	MyVector GetPosition();
	float GetX();
	float GetY();
	float GetR();
	float GetG();
	float GetB();
};
