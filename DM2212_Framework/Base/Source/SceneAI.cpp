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

	//Initialise Lane Booleans
	Shooter.Lane1Empty = false;
	Shooter.Lane2Empty = false;
	Shooter.Lane3Empty = false;

	bShooter.Lane1Empty = false;
	bShooter.Lane2Empty = false;
	bShooter.Lane3Empty = false;

	//Probabilities
	//Gprobability = 50.0f; //gender probability

	// Init Backup Shooter
	

	// Init Garlics
	/*garlic = new GameObject(GameObject::GO_BULLET);
	garlic->type = GameObject::GO_BULLET;
	garlic->SetState(CGarlic::INACTIVE);
	garlic->SetName("Bullet");
	garlic->SetColor(1.0f, 1.0f, 1.0f);
	garlic->scale.Set(2, 2, 1);
	garlic->pos.Set(shooter->pos.x, shooter->pos.y, shooter->pos.z);*/



	bShooter.AmmoRadius = 1.0f;
	bShooter.AmmoLimit = 3;
	bShooter.AmmoCount = bShooter.AmmoLimit;
	bShooter.AmmoPos = -4.5f;

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
	vampire_obj->SetState(CVampire::STAY);
	vampire_obj->scale.Set(13, 13, 1);
	vampire_obj->pos.Set(200, 10, 0);
	vampire_obj->SetHealth(3);
	vampire_obj->SetName("Vampire");
	//vampire_obj->SetRole(""); Vampire don't need a role
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

}

void SceneAI::Update(double dt)
{

	SceneBase::Update(dt);

	float distance;
	Vector3 direction, sPos;

	static float limita = 0;

	//shooter states
	if (GameObject::GO_SHOOTER)
	{
		if (limita < 3)
		{
			shooter = FetchGO();
			shooter->type = GameObject::GO_SHOOTER;
			shooter->SetState(CGShooter::ALIVE);
			shooter->SetName("Garlic Shooter");
			shooter->SetRole("Main");
			shooter->SetHealth(2);
			shooter->SetColor(0.0f, 0.0f, 0.1f);
			shooter->scale.Set(13, 13, 1);
			shooter->pos.Set(85, 10 + limita * 20, 0);


			//shooter state
			switch (shooter->GetState())
			{
			case CGShooter::ALIVE:
			{
				if (mb.GetMessage() == "Injured")
				{
					shooter->SetState(CGShooter::INJURED);
					shooter->SetRole("Patient");

					if (shooterbackup->GetState() != CGBackup::REPLACE) // if the backup is not already replacing someone
					{
						mb.SetMessage("Cover up my lane!");
						mb.SetFromLabel("Main");
						mb.SetToLabel("Refiller");

						shooterbackup->SetRole("Backup");
						shooterbackup->SetState(CGBackup::REPLACE);
					}
				}

				if (Garlics.garlicLeft == 0)
				{
					mb.SetMessage("I need more garlics");
					mb.SetFromLabel("Main");
					mb.SetToLabel("Refiller");
				}
				break;
			}

			case CGShooter::INJURED:
			{
									   // move towards the medic

									   // if two vampires attack them at the same time, announce as dead

									   /*if (mb.GetMessage() == "Killed")
									   {
									   shooter->SetState(CGShooter::DIED);
									   if(shooterbackup->GetState() != CGBackup::REPLACE)
									   shooterbackup->SetRole("Backup");
									   shooterbackup->SetState(CGBackup::REPLACE);
									   }*/
									   break;
			}
			case CGShooter::DIED:
			{
									shooter->active = false;

									break;
			}
			}

			limita++;
		}
	}

	static float limitB = 0;
	if (GameObject::GO_BSHOOTER)
	{
		if (limitB < 1)
		{
			shooterbackup = new GameObject(GameObject::GO_BSHOOTER);
			shooterbackup->type = GameObject::GO_BSHOOTER;
			shooterbackup->SetState(CGBackup::ALIVE);
			shooterbackup->SetName("Garlic Refiller");
			shooterbackup->SetRole("Refiller");
			shooterbackup->SetHealth(2);
			shooterbackup->SetColor(0.0f, 0.0f, 1.0f);
			shooterbackup->scale.Set(6, 6, 1);

			switch (shooterbackup->GetState())
			{
			case CGBackup::IDLE:
			{
				if (mb.GetMessage() == "I need more garlics")
				{
					shooterbackup->SetState(CGBackup::REFILL);
					bShooter.AmmoCount = 0;
				}
				else
				{
					if (bShooter.AmmoCount == 0)
					{
						mb.SetMessage("I need restock");
						mb.SetFromLabel("Refiller");
						mb.SetToLabel("Supplier");
					}
				}
				break;
			}
			}

			limitB++;
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
	RenderTextOnScreen(meshList[GEO_TEXT], nameString, Color(0, 0, 0), 2, medic_obj->pos.x - 22, medic_obj->pos.y - 42);

	//Render Assistant Medic Name
	nameString = amedic_obj->GetName();
	RenderTextOnScreen(meshList[GEO_TEXT], nameString, Color(0, 0, 0), 2, amedic_obj->pos.x - 37, amedic_obj->pos.y - 42);

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
	/*if (m_cashier)
	{
	delete m_cashier;
	m_cashier = NULL;
	}*/
}
