#include "SceneAI.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneAI::SceneAI()
{
}

SceneAI::~SceneAI()
{
}

int SceneAI::RandomInteger(int lowerLimit, int upperLimit)
{
	return rand() % (upperLimit - lowerLimit + 1) + lowerLimit;
}

// long integer to string
string SceneAI::itos(const long value)
{
	std::ostringstream buffer;
	buffer << value;
	return buffer.str();
}

float SceneAI::GetDistance(float x1, float y1, float x2, float y2)
{
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

bool SceneAI::Detect(Vector3 pos1, Vector3 pos2, float radius1, float radius2)
{
	bool detect = false;
	float totalRadius = radius1 + radius2;
	float distance = GetDistance(pos1.x, pos1.y, pos2.x, pos2.y);
	if (distance <= totalRadius) detect = true;
	return detect;
}

void SceneAI::Init()
{
	SceneBase::Init();

	m_speed = 1.f;

	Math::InitRNG();

	//init empty string
	//customerState = "";

	//Intialise variables
	objectcount = 1; // Cashier is included. Total amount of objects on screen
	srand((unsigned)time(NULL));

	//Probabilities
	//Gprobability = 50.0f; //gender probability

	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Construct Shooter
	/*
	m_cashier = new GameObject(GameObject::GO_CASHIER);
	m_cashier->active = true;
	m_cashier->scale.Set(6, 6, 6);
	m_cashier->pos.Set(m_worldWidth * 0.5f, m_worldHeight * .71f, m_cashier->pos.z);

	//Construct Medic
	Customer* customer = new Customer();
	customer->setToFemale();
	customer->SetEnterState();
	m_goList.push_back(customer);*/
}

GameObject* SceneAI::FetchGO()
{
	//Fetch game objects
	float z = 0; //Set z as zero
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it, ++z) //post-increment z
	{
		GameObject *go = (GameObject *)*it;
		if (go->active == false)
		{
			go->pos.Set(go->pos.x, go->pos.y, z / 100);//Each time the loop runs, the z axis is increasing.
			go->active = true;
			return go;
		}
	}
	//Restock the list when m_goList runs out of object
	for (int a = 0; a < 10; a++)
	{
		//m_goList.push_back(new GameObject(GameObject::GO_CUSTOMER)); // vampire
	}

	GameObject *go = m_goList.back();
	go->active = true;
	return go;
}


/*Customer* SceneAI::FetchCustomers()
{
	float z = 0; // initialise the z variable
	for (GameObject* GO : m_goList)
	{
		Customer* customer = dynamic_cast<Customer*>(GO);
		if (customer != NULL)
		{
			z++;
			if (customer->active == false)
			{
				RandomIndex = RandomInteger(1, 100);
				if (RandomIndex <= Gprobability) // 1- 50 = female
				{
					Females++;
					customer->setToFemale(); // it will crash if there is no if(customer != NULL)
				}
				else
				{
					Males++;
					customer->setToMale(); //51 - 100 male
				}

			}
			customer->pos.Set(customer->pos.x, customer->pos.y, z / 100);//Each time the loop runs, the z axis is increasing.
			customer->active = true;
			return customer; // BEST case scenario
		}
	}

	for (int a = 0; a < 10; a++)
	{
		m_goList.push_back(new GameObject(GameObject::GO_CUSTOMER));
	}

	Customer* customer = dynamic_cast<Customer*>(m_goList.back());
	if (customer != NULL)
	{
		customer->active = true;
		return customer; // BEST case scenario
	}
	return NULL; // worst case scenario. :D
}*/

void SceneAI::Update(double dt)
{
	SceneBase::Update(dt);
	static float LimitCustomers = 0;

	//do update for customer/supplier movement here.
	dt *= m_speed;
	m_force.SetZero();
}
void SceneAI::RenderGO(GameObject *go)
{
	//switch (go->type)
	//{
	//case GameObject::GO_CASHIER:
	//	// Render Cashier
	//	modelStack.PushMatrix();
	//	modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
	//	modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
	//	RenderMesh(meshList[GEO_CASHIER], false);
	//	modelStack.PopMatrix();
	//	break;
	//}
}

void SceneAI::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);

	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	//render the background image
	modelStack.PushMatrix();
	modelStack.Translate(m_worldWidth*0.5f, m_worldHeight*0.425f, -5);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	//RenderMesh(meshList[GEO_BG], false);
	modelStack.PopMatrix();

	

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	//Render cashier
	//RenderGO(m_cashier);

	//chip
	//std::ostringstream N_Chip;
	//N_Chip << "Chip: " << chip.no_item << " /10";
	//RenderTextOnScreen(meshList[GEO_TEXT], N_Chip.str(), Color(1, 1, 1), 2, m_worldWidth * 0.02f, m_worldHeight * 0.01f);

	//FPS
	std::ostringstream T_fps;
	T_fps.precision(5);
	T_fps << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], T_fps.str(), Color(1, 1, 1), 2, m_worldWidth * .44f, m_worldHeight * 0.57f);


}

void SceneAI::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	/*if (m_cashier)
	{
		delete m_cashier;
		m_cashier = NULL;
	}*/
}
