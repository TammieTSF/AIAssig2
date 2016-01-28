
#include "GameObject.h"

GameObject::GameObject(GAMEOBJECT_TYPE typeValue) 
	: type(typeValue)
	, scale(1, 1, 1)
	, active(false)
	, radius(1.0f)
	, name("Nil")
	, speed(0.1f)
	, role("Nil")
	, r(1.0f)
	, g(1.0f)
	, b(1.0f)
	, state(0)
{
}

GameObject::~GameObject()
{
}

void GameObject::SetName(string _name)
{
	name = _name;
}

void GameObject::SetState(int _state)
{
	state = _state;
}

void GameObject::SetHealth(int _health)
{
	health = _health;
}

void GameObject::SetColor(float _r, float _g, float _b)
{
	r = _r;
	g = _g;
	b = _b;
}

void GameObject::SetRole(string _role)
{
	role = _role;
}

void GameObject::SetSpeed(float _speed)
{
	speed = _speed;
}

void GameObject::SetRadius(float _radius)
{
	radius = _radius;
}

string GameObject::GetName()
{
	return name;
}

string GameObject::GetRole()
{
	return role;
}

int GameObject::GetState()
{
	return state;
}

float GameObject::GetSpeed()
{
	return speed;
}
float GameObject::GetRadius()
{
	return radius;
}

float GameObject::GetR()
{
	return r;
}

float GameObject::GetG()
{
	return g;
}

float GameObject::GetB()
{
	return b;
}
