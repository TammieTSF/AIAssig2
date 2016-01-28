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
#include "AI\AssistantMedic.h"
#include "AI\Garlic.h"
#include "AI\GarlicBackup.h"
#include "AI\GarlicShooter.h"
#include "AI\Grocer.h"
#include "AI\Medic.h"
#include "AI\Vampire.h"
#include "MyObject.h"
#include "MessageBoard.h"

using namespace std;


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

	MyObject *garlic, 
		*grocer, 
		*medic, 
		*medicassistant, 
		*shooter,
		*shooterbackup, 
		*vampire;
	
	CAssistant aMedic;	// Medic Assistant
	CGarlic Garlics;	// Garlic Bullets
	CGBackup bShooter;	// Garlic Backup
	CGShooter Shooter;	// Main Shooter
	CGrocer Grocer;		// Grocer	
	CMedic Medic;		// Main Medic
	CMessageBoard mb;	// MessageBoard
	CVampire Vampire;	// Vampire

	//Randomizer
	int RandomInteger(int lowerLimit, int upperLimit);
	string itos(const long value);

	bool Detect(Vector3 pos1, Vector3 pos2, float radius1, float radius2);
	float GetDistance(float x1, float y1, float x2, float y2);

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