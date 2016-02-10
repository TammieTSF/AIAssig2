#include "SceneAI.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

#define vampireLimit 1

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

	//Initialise Lane Booleans
	Shooter.Lane1Empty = false;
	Shooter.Lane2Empty = false;
	Shooter.Lane3Empty = false;

	bShooter.Lane1Empty = false;
	bShooter.Lane2Empty = false;
	bShooter.Lane3Empty = false;

	bShooter.AmmoRadius = 1.0f;
	bShooter.AmmoLimit = 3;
	bShooter.AmmoCount = bShooter.AmmoLimit;
	bShooter.AmmoPos = -4.5f;

	bulletleft1 = 2;
	bulletleft2 = 2;
	bulletleft3 = 2;

	//Shooter
	shooter = FetchGO();
	shooter->type = GameObject::GO_SHOOTER;
	shooter->SetState(CGShooter::ALIVE);
	shooter->SetName("Garlic Shooter");
	shooter->SetRole("Main");
	shooter->SetHealth(2);
	shooter->scale.Set(13, 13, 1);
	shooter->pos.Set(85, 10, 1);

	shooter2 = FetchGO();
	shooter2->type = GameObject::GO_SHOOTER;
	shooter2->SetState(CGShooter::ALIVE);
	shooter2->SetName("Garlic Shooter");
	shooter2->SetRole("Main");
	shooter2->SetHealth(2);
	shooter2->scale.Set(13, 13, 1);
	shooter2->pos.Set(85, 30, 1);

	shooter3 = FetchGO();
	shooter3->type = GameObject::GO_SHOOTER;
	shooter3->SetState(CGShooter::ALIVE);
	shooter3->SetName("Garlic Shooter");
	shooter3->SetRole("Main");
	shooter3->SetHealth(2);
	shooter3->scale.Set(13, 13, 1);
	shooter3->pos.Set(85, 50, 1);

	// Backup
	shooterbackup = FetchGO();
	shooterbackup->type = GameObject::GO_BSHOOTER;
	shooterbackup->SetState(CGBackup::ALIVE);
	shooterbackup->SetName("Garlic Refiller");
	shooterbackup->SetRole("Refiller");
	shooterbackup->SetHealth(2);
	shooterbackup->scale.Set(13, 13, 1);
	shooterbackup->pos.Set(55, 30, 1);

	//Grocer
	grocer = FetchGO();
	grocer->type = GameObject::GO_SUPPLIER;
	grocer->SetState(CGrocer::IDLE);
	grocer->SetName("Grocer");
	grocer->SetRole("Supplier");
	grocer->scale.Set(13, 13, 1);
	grocer->pos.Set(10, 30, 0);

	//Medic
	medic_obj = FetchGO();
	medic_obj->type = GameObject::GO_MMEDIC;
	medic_obj->SetState(CMedic::IDLE);
	medic_obj->scale.Set(13, 13, 1);
	medic_obj->pos.Set(33, 90, 0);
	medic_obj->SetName("MM");
	medic_obj->SetRole("Healer");

	//Assistant medic
	amedic_obj = FetchGO();
	amedic_obj->type = GameObject::GO_AMEDIC;
	amedic_obj->SetState(CAssistant::IDLE);
	amedic_obj->scale.Set(13, 13, 1);
	amedic_obj->pos.Set(57, 90, 0);
	amedic_obj->SetName("AM");
	amedic_obj->SetRole("Second healer");

	//Vampire
	vampire_obj = FetchGO();
	vampire_obj->type = GameObject::GO_VAMPIRE;
	vampire_obj->SetState(CVampire::MOVE);
	vampire_obj->scale.Set(13, 13, 1);
	vampire_obj->pos.Set(200, RandomLane(), 0);
	vampire_obj->SetHealth(3);
	vampire_obj->SetName("Vampire");
	//vampire_obj->SetRole(""); Vampire don't need a role
}

float SceneAI::RandomLane()
{
	int rand = RandomInteger(1,3);

	switch (rand)
	{
	case 1:
		return 10.f;
	case 2:
		return 30.f;
	case 3:
		return 50.f;
	}
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
		m_goList.push_back(new GameObject());
	}

	GameObject *go = m_goList.back();
	go->active = true;
	return go;
}

void SceneAI::RunFSM()
{
	Vector3 direction, sPos;

	//Medic FSM
	switch (medic_obj->GetState())
	{
	case CMedic::HEAL:
		if (mb.GetMessage() == "HEAL")
		{
			medic_obj->SetState(CMedic::HEAL);
			medic_obj->SetRole("Healer");
			
		}
		break;
	case CMedic::HEALING:
		break;
	case CMedic::HEALED:
		break;
	}

	//Assistant medic FSM
	switch (amedic_obj->GetState())
	{
	case CAssistant::HEAL:
		break;
	case CAssistant::HEALING:
		break;
	case CAssistant::HEALED:
		break;
	case CAssistant::HELP:
		break;
	case CAssistant::HELPING:
		break;
	case CAssistant::HELPED:
		break;
	}

	//Vampire FSM    <------vampire have no role change, keep for reference
	/*switch (vampire_obj->GetState())
	{
	case CVampire::STAY:
		break;
	case CVampire::MOVE: 
		break;
	case CVampire::DESTROY:
		break;
	case CVampire::HIT:
		break;
	case CVampire::DIE:
		break;
	}*/
}

void SceneAI::Update(double dt)
{
	SceneBase::Update(dt);

	static float timeget;
	static float timeLimit = 2.f; // 0.1f spawns 10 bullets
	timeget += dt;

	if (timeget > timeLimit)
	{
		timeget = timeLimit;
	}

	if (timeget >= timeLimit)
	{
		if (bulletleft1 > 0)
		{
			garlic = FetchGO();
			garlic->type = GameObject::GO_BULLET;
			garlic->SetState(CGarlic::INACTIVE);
			garlic->SetName("Garlic");
			garlic->scale.Set(7, 7, 1);
			garlic->pos.Set(shooter->pos.x, shooter->pos.y, 0);
			garlic->active = true;

			timeget = 0;
		}
	}

	static float timeget2;
	static float timeLimit2 = 2.f; // 0.1f spawns 10 bullets
	timeget2 += dt;

	if (timeget2 > timeLimit2)
	{
		timeget2 = timeLimit2;
	}

	if (timeget2 >= timeLimit2)
	{
		if (bulletleft2 > 0)
		{
			garlic2 = FetchGO();
			garlic2->type = GameObject::GO_BULLET;
			garlic2->SetState(CGarlic::INACTIVE);
			garlic2->SetName("Garlic");
			garlic2->scale.Set(7, 7, 1);
			garlic2->pos.Set(shooter2->pos.x, shooter2->pos.y, 0);
			garlic2->vel.Set(shooter2->pos.x, 0, 0);
			garlic2->vel.Normalize() *= BULLET_SPEED;
			garlic2->active = true;

			timeget2 = 0;
		}
	}

	static float timeget3;
	static float timeLimit3 = 2.f; // 0.1f spawns 10 bullets
	timeget3 += dt;

	if (timeget3 > timeLimit3)
	{
		timeget3 = timeLimit3;
	}

	if (timeget3 >= timeLimit3)
	{
		if (bulletleft3 > 0)
		{
			garlic3 = FetchGO();
			garlic3->type = GameObject::GO_BULLET;
			garlic3->SetState(CGarlic::INACTIVE);
			garlic3->SetName("Garlic");
			garlic3->scale.Set(7, 7, 1);
			garlic3->pos.Set(shooter3->pos.x, shooter3->pos.y, 0);
			garlic3->vel.Set(shooter3->pos.x, 0, 0);
			garlic3->vel.Normalize() *= BULLET_SPEED;
			garlic3->active = true;

			timeget3 = 0;
		}
	}

	for (std::vector<GameObject *>::iterator itr = m_goList.begin(); itr != m_goList.end(); itr++)
	{
		GameObject *go = (GameObject *)*itr;
		CGarlic* garlics1 = dynamic_cast<CGarlic*>(go);
		go->pos += go->vel * dt;

		// Garlic 1
		if (go->type == GameObject::GO_BULLET)
		{
			switch (go->GetState())
			{
			case CGarlic::INACTIVE:
			{
				if (bulletleft1 > 0)
				{
					if (shooter->GetState() != CGShooter::DIED || shooterbackup->GetState() == CGBackup::REPLACING)
					{
						garlic->SetState(CGarlic::FIRED);
					}
				}
				break;
			}
			case CGarlic::FIRED:
			{
				bulletleft1--;
				garlic->SetState(CGarlic::MOVING);
				break;
			}
			case CGarlic::MOVING:
			{
				garlic->vel.Set(shooter->pos.x, 0, 0);
				garlic->vel.Normalize() *= BULLET_SPEED;

				for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *go2 = (GameObject *)*it2;
					if (go2->type == GameObject::GO_VAMPIRE)
					{
						if ((garlic->pos - vampire_obj->pos).Length() < (garlic->scale.x + vampire_obj->scale.x))
						{
							garlic->SetState(CGarlic::HIT);
						}
					}

				}

				break;
			}
			case CGarlic::HIT:
			{
				garlic->active = false;
			}
			}

			// Garlic 2
			switch (garlic2->GetState())
			{
			case CGarlic::INACTIVE:
			{
				if (bulletleft2 > 0)
				{
					if (shooter2->GetState() != CGShooter::DIED || shooterbackup->GetState() == CGBackup::REPLACING)
					{
						garlic2->SetState(CGarlic::FIRED);
					}
				}
				break;
			}
			case CGarlic::FIRED:
			{
				bulletleft2--;
				garlic2->SetState(CGarlic::MOVING);
				break;
			}
			case CGarlic::MOVING:
			{
				garlic2->vel.Set(shooter->pos.x, 0, 0);
				garlic2->vel.Normalize() *= BULLET_SPEED;

				for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *go2 = (GameObject *)*it2;
					if (go2->type == GameObject::GO_VAMPIRE)
					{
						if ((garlic2->pos - vampire_obj->pos).Length() < (garlic2->scale.x + vampire_obj->scale.x))
						{
							garlic2->SetState(CGarlic::HIT);
						}
					}

				}

				break;
			}
			case CGarlic::HIT:
			{
				garlic2->active = false;
			}
			}

			// Garlic 3
			switch (garlic3->GetState())
			{
			case CGarlic::INACTIVE:
			{
				if (bulletleft3 > 0)
				{
					if (shooter3->GetState() != CGShooter::DIED || shooterbackup->GetState() == CGBackup::REPLACING)
					{
						garlic3->SetState(CGarlic::FIRED);
					}
				}
				break;
			}
			case CGarlic::FIRED:
			{
				bulletleft3--;
				garlic3->SetState(CGarlic::MOVING);
				break;
			}
			case CGarlic::MOVING:
			{
				garlic3->vel.Set(shooter->pos.x, 0, 0);
				garlic3->vel.Normalize() *= BULLET_SPEED;

				for (std::vector<GameObject *>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *go2 = (GameObject *)*it2;
					if (go2->type == GameObject::GO_VAMPIRE)
					{
						if ((garlic3->pos - vampire_obj->pos).Length() < (garlic3->scale.x + vampire_obj->scale.x))
						{
							garlic3->SetState(CGarlic::HIT);
						}
					}

				}

				break;
			}
			case CGarlic::HIT:
			{
				garlic3->active = false;
			}
			}

		}
		
	}


	dt *= m_speed;
	m_force.SetZero();
}
void SceneAI::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_AMEDIC:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_AMEDIC], false);
		modelStack.PopMatrix();
		break;
	}
	case GameObject::GO_BSHOOTER:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BSHOOTER], false);
		modelStack.PopMatrix();
		break;
	}
	case GameObject::GO_BULLET:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BULLET], false);
		modelStack.PopMatrix();
		break;
	}
	case GameObject::GO_MMEDIC:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_MMEDIC], false);
		modelStack.PopMatrix();
		break;
	}
	case GameObject::GO_SHOOTER:
	{
		if (shooter->GetState() != CGShooter::INJURED) // as long as the state the shooter is not injured
		{
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_SHOOTER], false);
			modelStack.PopMatrix();
		}

		else if (shooter->GetState() == CGShooter::INJURED) // if the shooter gets injured
		{
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_SSHOOTER], false);
			modelStack.PopMatrix();
		}
		break;
	}
	case GameObject::GO_SUPPLIER:
	{
		if (grocer->GetState() != CGrocer::RUN) // as long as the grocer does not enter RUN state
		{
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_SUPPLIER], false);
			modelStack.PopMatrix();
		}

		else if (grocer->GetState() == CGrocer::RUN) // if the grocer enters the RUN state
		{
			modelStack.PushMatrix();
			modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
			modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
			RenderMesh(meshList[GEO_RSUPPLIER], false);
			modelStack.PopMatrix();
		}
		break;
	}
	case GameObject::GO_VAMPIRE:
	{
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_VAMPIRE], false);
		modelStack.PopMatrix();
		break;
	}
	}
}

void SceneAI::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);

	//Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
		);

	//Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();

	//Render the Background Image
	modelStack.PushMatrix();
	modelStack.Translate(m_worldWidth*0.5f, m_worldHeight*0.5f, -5);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	RenderMesh(meshList[GEO_BG], false);
	modelStack.PopMatrix();

	//Render the background for text  (black quad)
	modelStack.PushMatrix();
	modelStack.Translate(m_worldWidth*0.67f, m_worldHeight * .8f, -4);
	modelStack.Scale(m_worldWidth * .675f, m_worldHeight * .4f, 1);
	RenderMesh(meshList[GEO_TEXTBG], false);
	modelStack.PopMatrix();

	//Render the Supply Box
	modelStack.PushMatrix();
	modelStack.Translate(30, 30, -1);
	modelStack.Scale(10, 10, 1);
	RenderMesh(meshList[GEO_SUPPLYBOX], false);
	modelStack.PopMatrix();

	//Render the gameObjects
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	//Render the FPS
	std::ostringstream T_fps;
	T_fps.precision(3);
	T_fps << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], T_fps.str(), Color(1, 1, 1), 1.5, 45, 57);

	//universal string for name 
	std::string nameString;

	//Render Main Medic Name
	nameString = medic_obj->GetName();
	RenderTextOnScreen(meshList[GEO_TEXT], nameString, Color(0, 0, 0), 3, medic_obj->pos.x - 22, medic_obj->pos.y - 42);

	//Render Assistant Medic Name
	nameString = amedic_obj->GetName();
	RenderTextOnScreen(meshList[GEO_TEXT], nameString, Color(0, 0, 0), 3, amedic_obj->pos.x - 37, amedic_obj->pos.y - 42);

	//Render Garlic Shooters Name
	nameString = shooter2->GetName();
	RenderTextOnScreen(meshList[GEO_TEXT], nameString, Color(0, 0, 0), 3, shooter2->pos.x, shooter2->pos.y);

	//universal string for state
	std::string stateString;

	//Render the medic state and role
	stateString = "M Med State: ";
	switch (medic_obj->GetState())
	{
	case CMedic::IDLE: stateString += "IDLE";
		break;
	case CMedic::HEAL: stateString += "HEAL";
		break;
	case CMedic::HEALING: stateString += "HEALIMG";
		break;
	case CMedic::HEALED: stateString += "HEALED";
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 1.5, 57, 57);
	stateString = "M Med Role: " + medic_obj->GetRole();
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 1.5, 57, 55);



	//Render the Assistant medic state and role
	stateString = "A Med State: ";

	switch (amedic_obj->GetState())
	{
	case CAssistant::IDLE: stateString += "IDLE";
		break;
	case CAssistant::HEAL: stateString += "HEAL";
		break;
	case CAssistant::HEALING: stateString += "HEALING";
		break;
	case CAssistant::HEALED: stateString += "HEALED";
		break;
	case CAssistant::HELP: stateString += "HELP";
		break;
	case CAssistant::HELPING: stateString += "HELPING";
		break;
	case CAssistant::HELPED: stateString += "HELPED";
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 1.5, 57, 51);
	stateString = "A Med Role: " + amedic_obj->GetRole();
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 1.5, 57, 49);

	// Garlic
	stateString = "Shooter One State: ";

	switch (shooter->GetState())
	{
	case CGShooter::ALIVE: stateString += "ALIVE";
		break;
	case CGShooter::INJURED: stateString += "INJURED";
		break;
	case CGShooter::RETURN: stateString += "RETURN";
		break;
	case CGShooter::DIED: stateString += "DIED";
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 1.5, 57, 45);
	stateString = "Shooter One Role: " + shooter->GetRole();
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 1.5, 57, 43);

	//Render the message Board
	stateString = "Message Board: ";
	stateString += mb.GetMessageW();
	stateString += " From : " + mb.GetFromLabel() + " To: " + mb.GetToLabel();
	RenderTextOnScreen(meshList[GEO_TEXT], stateString, Color(1, 1, 1), 2, 30, 38);

	//Run the FSM
	RunFSM();
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
}
