#include "SceneAI.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

#define vampirelimit 5
#define mainMediclimit 1
#define assistantMediclimit 1

SceneAI::SceneAI():
vampCount(1)
,vampLimiter(0)
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

float SceneAI::RandomLane()
{
	int temp = RandomInteger(1, 3);

	switch (temp)
	{
	case 1:
		return 10.f;
		break;
	case 2:
		return 30.f;
		break;
	case 3:
		return 50.f;
		break;
	}
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

	// Init Grocer
	grocer = new GameObject(GameObject::GO_SUPPLIER);
	grocer->SetState(CGrocer::IDLE);
	grocer->SetName("Grocer");
	grocer->SetRole("Supplier");
	grocer->SetColor(1.f, 0.f, 0.f);
	grocer->scale.Set(6, 6, 1);

	bShooter.AmmoRadius = 1.0f;
	bShooter.AmmoLimit = 3;
	bShooter.AmmoCount = bShooter.AmmoLimit;
	bShooter.AmmoPos = -4.5f;


	//Assistant medic
	medicassistant = FetchGO();
	medicassistant = new GameObject(GameObject::GO_AMEDIC);
	medicassistant->SetState(CAssistant::IDLE);
	medicassistant->SetName("Assistant medic");
	medicassistant->SetRole("Second healer");
	medicassistant->SetColor(1.f, 1.f, 1.f);
	medicassistant->scale.Set(6, 6, 1);
	medicassistant->pos.Set(70, 50, medicassistant->pos.z);

	//medicassistant->SetColor();
	//vampire = new MyObject();
	//vampire->SetName("Vampires");
	//vampire->SetRadius(0.15f);
	//vampire->SetColor(1.0f, 0.0f, 0.0f);
	//vampire->SetPosition(5.0f, 0.0f);
	//vampire->SetHealth(3);
	//vampire->SetRole("Backup");

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
		m_goList.push_back(new GameObject(GameObject::GO_VAMPIRE));
	}

	GameObject *go = m_goList.back();
	go->active = true;
	return go;
}

//CVampire* SceneAI::FetchVampires()
//{
//	float z = 0;
//	for (GameObject *GO : m_goList)
//	{
//		CVampire * vampires = dynamic_cast<CVampire *> (GO);
//		if (vampires != NULL)
//		{
//			z++;
//			if (vampires->active == false)
//			{
//				RandomIndex = RandomInteger(1, 3);
//				if (RandomIndex == 1)
//				{
//					vampires->Lane1 = true;
//					vampires->Lane2 = false;
//					vampires->Lane3 = false;
//				}
//				else if (RandomIndex == 2)
//				{
//					vampires->Lane1 = false;
//					vampires->Lane2 = true;
//					vampires->Lane3 = false;
//				}
//				else if (RandomIndex == 3)
//				{
//					vampires->Lane1 = false;
//					vampires->Lane2 = false;
//					vampires->Lane3 = true;
//				}
//			}
//
//			vampires->pos.Set(vampires->pos.x, vampires->pos.y, vampires->pos.z);
//			vampires->active = true;
//			return vampires;
//		}
//	}
//
//	for (int a = 0; a < 10; a++)
//	{
//		m_goList.push_back(new GameObject(GameObject::GO_VAMPIRE));
//	}
//
//	CVampire* vampires = dynamic_cast<CVampire*>(m_goList.back());
//	if (vampires != NULL)
//	{
//		vampires->active = true;
//		return vampires; // BEST case scenario
//	}
//	return NULL; // worst case scenario. :D
//}

void SceneAI::Update(double dt)
{
	vampLimiter += dt;

	SceneBase::Update(dt);

	float distance;
	Vector3 direction, sPos;

	static float limita = 0;
	//vampire states
	translatevamp--;

	if (vampCount < vampirelimit && vampLimiter > 1) //60 frames per second
	{
		vampLimiter = 0;
		vampCount++;

		vampire = FetchGO();
		vampire->type = GameObject::GO_VAMPIRE;
		vampire->SetName("Vampire " + vampCount);
		vampire->SetHealth(3);
		vampire->SetRole("kill at sight");
		vampire->SetColor(0.0f, 0.0f, 0.0f);
		vampire->scale.Set(13, 13, 1);
		vampire->pos.Set(100 , RandomLane(), vampire->pos.z);

	}

	//for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	//{
	//	GameObject *go = (GameObject *)*it;
	//	if (go->type == GameObject::GO_VAMPIRE)
	//	{
	//		go->pos.Set(100 + translatevamp, RandomLane(), vampire->pos.z);
	//	}
	//}
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
	modelStack.Translate(m_worldWidth*0.5f, m_worldHeight*0.5f, -5);
	modelStack.Scale(m_worldWidth, m_worldHeight, 1);
	RenderMesh(meshList[GEO_BG], false);
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
	RenderTextOnScreen(meshList[GEO_TEXT], T_fps.str(), Color(1, 1, 1), 3, 0, 57);

	//FPS
	std::ostringstream T_vampcount;
	T_vampcount.precision(5);
	T_vampcount << "vampcount: " << vampLimiter;
	RenderTextOnScreen(meshList[GEO_TEXT], T_vampcount.str(), Color(1, 1, 1), 3, 0, 37);
	



	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->type == go->GO_VAMPIRE)
		{
			std::ostringstream T_vampHealth;
			T_vampHealth << "Health: " + go->GetHealth();
			RenderTextOnScreen(meshList[GEO_TEXT], T_vampHealth.str(), Color(1, 1, 1), 3, go->pos.x, go->pos.y - 5.f);
		}
	}
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
