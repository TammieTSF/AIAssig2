#ifndef SCENE_SCENEAI_H
#define SCENE_SCENEAI_H

#include "GameObject.h"
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include <string>
#include <ostream>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFw/glfw3.h>
#include "SceneBase.h"
#include "AI\Medic.h"


using std::string;


class SceneAI : public SceneBase
{
public:
	SceneAI();
	~SceneAI();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	GameObject* FetchGO();
//	Customer* FetchCustomers();
//	Supplier* FetchSupplier();

	//Randomizer
	int RandomInteger(int lowerLimit, int upperLimit);
	std::string itos(const long value);

protected:

	//Physics
	std::vector<GameObject *> m_goList;

	float m_speed;
	float m_worldWidth;
	float m_worldHeight;

	//string to print state
	//string customerState;

	//GameObject *m_cashier;


	Vector3 m_force;

	int m_objectCount;
	int objectcount;

	
	//Probability
	//float Gprobability; // Gender probability
};

#endif