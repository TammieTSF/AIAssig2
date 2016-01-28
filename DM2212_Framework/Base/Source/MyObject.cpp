#include "MyObject.h"

MyObject::MyObject()
	:radius(1.0f)
	, name("Nil")
	, speed(0.1f)
	, role("Nil")
	, r(1.0f)
	, g(1.0f)
	, b(1.0f)
	, position(0.0f, 0.0f)
	, state(0)
{

}

void MyObject::SetName(string _name)
{
	name = _name;
}

void MyObject::SetState(int _state)
{
	state = _state;
}

void MyObject::SetHealth(int _health)
{
	health = _health;
}

void MyObject::SetColor(float _r, float _g, float _b)
{
	r = _r;
	g = _g;
	b = _b;
}

void MyObject::SetRole(string _role)
{
	role = _role;
}

void MyObject::SetSpeed(float _speed)
{
	speed = _speed;
}

void MyObject::SetRadius(float _radius)
{
	radius = _radius;
}

void MyObject::SetPosition(float x, float y)
{
	position = Vector3(x, y);
}

void MyObject::SetPosition(Vector3 _position)
{
	position = _position;
}

string MyObject::GetName()
{
	return name;
}

string MyObject::GetRole()
{
	return role;
}

int MyObject::GetState()
{
	return state;
}

float MyObject::GetSpeed()
{
	return speed;
}
float MyObject::GetRadius()
{
	return radius;
}

Vector3 MyObject::GetPosition()
{
	return position;
}

float MyObject::GetX()
{
	return position.x;
}

float MyObject::GetY()
{
	return position.y;
}

float MyObject::GetR()
{
	return r;
}

float MyObject::GetG()
{
	return g;
}

float MyObject::GetB()
{
	return b;
}

int MyObject::MinusHealth()
{
	health -= 1;
	return health;
}