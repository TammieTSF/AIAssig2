#include <string>
#include "MyVector.h"

using namespace std;

class MyObject
{
public:
	string name, role;
	float speed, radius;
	MyVector position;
	int state;
	float r, g, b;
	MyObject() :radius(1.0f), name("Nil"), speed(0.1f), role("Nil"), r(1.0f), g(1.0f), b(1.0f), position(0.0f, 0.0f), state(0){}
	void SetName(string _name) { name = _name; }
	void SetState(int _state){ state = _state; }
	void SetColor(float _r, float _g, float _b){ r = _r; g = _g; b = _b; }
	void SetRole(string _role) { role = _role; }
	void SetSpeed(float _speed) { speed = _speed; }
	void SetRadius(float _radius) { radius = _radius; }
	void SetPosition(float x, float y){ position = MyVector(x, y); }
	void SetPosition(MyVector _position){ position = _position; }
	string GetName() { return name; }
	string GetRole() { return role; }
	int GetState(){ return state; }
	float GetSpeed() { return speed; }
	float GetRadius() { return radius; }
	MyVector GetPosition(){ return position; }
	float GetX(){ return position.x; }
	float GetY(){ return position.y; }
	float GetR(){ return r; }
	float GetG(){ return g; }
	float GetB(){ return b; }
};
