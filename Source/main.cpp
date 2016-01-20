// Program : Collaborative AI Graphic

#include <stdlib.h>
#include <windows.h>
#include <math.h>
#include <string>
#include <ostream>
#include <sstream>
#include <GL/glew.h>
#include <GLFw/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "Garlic.h"
#include "MessageBoard.h"
#include "GarlicShooter.h"
#include "Medic.h"
#include "Vampire.h"
#include "Grocer.h"
#include "MyObject.h"

using namespace std;

#pragma comment(linker,"/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
void RenderObjects();
void Render(GLFWwindow* window);

// long integer to string
string itos(const long value)
{
	ostringstream buffer;
	buffer << value;
	return buffer.str();
}


float GetDistance(float x1, float y1, float x2, float y2) { return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)); } // OK

// Within range
bool Detect(MyVector pos1, MyVector pos2, float radius1, float radius2)
{
	bool detect = false;
	float totalRadius = radius1 + radius2;
	float distance = GetDistance(pos1.x, pos1.y, pos2.x, pos2.y);
	if (distance <= totalRadius) detect = true;
	return detect;
}

void RenderCircle(GLfloat x, GLfloat y, GLfloat radius, GLfloat r, GLfloat g, GLfloat b)
{
	int n = 360;
	glColor3f(r, g, b);
	glBegin(GL_POINTS);
	for (int i = 0; i <= n; i++)
	{
		float angle = (float)(i * (2.0 * 3.14159 / n));
		glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
	}
	glEnd();
}

void RenderFillCircle(GLfloat x, GLfloat y, GLfloat radius, GLfloat r, GLfloat g, GLfloat b)
{
	int n = 360;
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (int i = 0; i <= n; i++)
	{
		float angle = (float)(i * (2.0 * 3.14159 / n));
		glVertex2f(x + radius * cos(angle), y + radius * sin(angle));
	}
	glEnd();
}

// SHADERS //////////////////////////////////////////////////////////////////
const char *VERTEX_SHADER = ""
"#version 410 core\n"
"in vec4 in_Position;\n"
"out vec2 texCoords;\n"
"void main(void) {\n"
"    gl_Position = vec4(in_Position.xy, 0, 1);\n"
"    texCoords = in_Position.zw;\n"
"}\n";

const char *FRAGMENT_SHADER = ""
"#version 410 core\n"
"precision highp float;\n"
"uniform sampler2D tex;\n"
"uniform vec4 color;\n"
"in vec2 texCoords;\n"
"out vec4 fragColor;\n"
"void main(void) {\n"
"    fragColor = vec4(1, 1, 1, texture(tex, texCoords).r) * color;\n"
"}\n";

GLuint texUniform, colorUniform;
GLuint texture{ 0 }, sampler{ 0 };
GLuint vbo{ 0 }, vao{ 0 };
GLuint vs{ 0 }, fs{ 0 }, program{ 0 };

///////////////////////////////////////////////////////////////////

static void ErrorCallBack(int error, const char*description)
{
	fputs(description, stderr);
}

float width, height;
GLFWwindow* window;
void Render(GLFWwindow* window);

static void ResizeCallBack(GLFWwindow *window, int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)w / (float)h, 0, 100);
	glMatrixMode(GL_MODELVIEW);
}

// Free Type //////////////////////////////////////////////////////
FT_Library ft_lib{ nullptr };
FT_Face face{ nullptr };

void RenderText(const string str, FT_Face face, float x, float y, float _sx, float _sy)
{
	float sx = _sx / width;
	float sy = _sy / height;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	const FT_GlyphSlot glyph = face->glyph;
	for (auto c : str) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
			continue;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
			glyph->bitmap.width, glyph->bitmap.rows,
			0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

		const float vx = x + glyph->bitmap_left * sx;
		const float vy = y + glyph->bitmap_top * sy;
		const float w = glyph->bitmap.width * sx;
		const float h = glyph->bitmap.rows * sy;

		struct {
			float x, y, s, t;
		} data[6] = {
				{ vx, vy, 0, 0 },
				{ vx, vy - h, 0, 1 },
				{ vx + w, vy, 1, 0 },
				{ vx + w, vy, 1, 0 },
				{ vx, vy - h, 0, 1 },
				{ vx + w, vy - h, 1, 1 }
		};
		glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (glyph->advance.x >> 6) * sx;
		y += (glyph->advance.y >> 6) * sy;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
///////////////////////////////////////////////////////////////////

void Cleanup()
{
	FT_Done_Face(face);
	FT_Done_FreeType(ft_lib);
	glDeleteTextures(1, &texture);
	glDeleteSamplers(1, &sampler);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
}

void DoExit()
{
	Cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

MyObject *shooter, *shooterbackup, *medic, *medicassistant, *grocer, *vampire, *garlic;
CGarlic Garlics;
CMedic Medic;
CVampire Vampire;
CGShooter Shooter;
CGrocer Grocer;
CMessageBoard mb;

void SimulationInit()
{
	// can use for loop, this is for testing only.
	/*for (int a = 0; a < 3; a++)
	{
	shooter = new MyObject();
	shooter->SetRadius(0.25f);
	shooter->SetColor(0.0f, 0.0f, 1.0f);
	shooter->SetPosition(-2.5f, 0.0f);
	shooter->SetName("Garlic Shooter " + a + 1);
	shooter->SetState(CGShooter::SHOOT);
	shooter->SetRole("Garlic Shooter " + a + 1 );
	}*/

	shooter = new MyObject();
	shooter->SetRadius(0.25f);
	shooter->SetColor(0.0f, 0.0f, 1.0f);
	shooter->SetPosition(-2.5f, 0.0f);
	shooter->SetName("Garlic Shooter");
	shooter->SetState(CGShooter::ALIVE);
	shooter->SetHealth(3);
	shooter->SetRole("Main");

	shooterbackup = new MyObject();
	shooterbackup->SetRadius(0.25f);
	shooterbackup->SetColor(0.0f, 0.0f, 1.0f);
	shooterbackup->SetPosition(-1.5f, 0.0f);
	shooterbackup->SetName("Garlic Refiller");
	shooterbackup->SetState(CGShooter::IDLE);
	shooterbackup->SetHealth(3);
	shooterbackup->SetRole("Backup");

	// vampires, use for loop also
	vampire = new MyObject();
	vampire->SetName("Vampires");
	vampire->SetRadius(0.15f);
	vampire->SetColor(1.0f, 0.0f, 0.0f);
	vampire->SetPosition(5.0f, 0.0f);
	vampire->SetHealth(3);
	vampire->SetState(CVampire::MOVE);

	// garlic for shooter 1, use for loop
	/*for (int a = 0; a < 3; a++)
	{
	garlic = new MyObject();
	garlic->SetSpeed(0.1f);
	garlic->SetName("Bullet");
	garlic->SetState(CGarlic::INACTIVE);
	garlic->SetRadius(0.05f), garlic->SetColor(1.0f, 1.0f, 1.0f);
	garlic->SetPosition(shooter->GetX() + shooter->GetRadius(), 0.0f);
	Garlics.garlicGiven = 3;
	Garlics.garlicLeft = Garlics.garlicGiven;
	Garlics.garlicStartPos = shooter->GetX() + shooter->GetRadius();
	}*/

	garlic = new MyObject();
	garlic->SetSpeed(0.1f);
	garlic->SetName("Bullet");
	garlic->SetState(CGarlic::INACTIVE);
	garlic->SetRadius(0.05f), garlic->SetColor(1.0f, 1.0f, 1.0f);
	garlic->SetPosition(shooter->GetX() + shooter->GetRadius(), 0.0f);
	Garlics.garlicGiven = 3;
	Garlics.garlicLeft = Garlics.garlicGiven;
	Garlics.garlicStartPos = shooter->GetX() + shooter->GetRadius();

	grocer = new MyObject();
	grocer->SetName("Grocer");
	grocer->SetSpeed(0.025f);
	grocer->SetRadius(0.15f), grocer->SetPosition(-4.5f, 0.0f);
	grocer->SetColor(0.0f, 1.0f, 0.0f);
	grocer->SetState(CGrocer::IDLE);
	grocer->SetRole("Supplier");
	Grocer.grocerPos = 2.0;

	medic = new MyObject();
	medic->SetRadius(0.2f), medic->SetPosition(-4.5f, 2.0f);
	medic->SetColor(1.0f, 1.0f, 0.0f);
	medic->SetSpeed(0.05f);
	medic->SetName("Medic");
	medic->SetRole("Main Healer");
	medic->SetState(CMedic::IDLE);
	Medic.medicPos = 5.0;

	medicassistant = new MyObject();
	medicassistant->SetRadius(0.2f), medic->SetPosition(-4.5f, 2.0f);
	medicassistant->SetColor(1.0f, 1.0f, 0.0f);
	medicassistant->SetSpeed(0.05f);
	medicassistant->SetName("Assistant Medic");
	medicassistant->SetRole("Backup Healer");
	medicassistant->SetState(CMedic::IDLE);
	Medic.assistantPos = 5.0;

	Grocer.cycle = 1;
	Grocer.storeRadius = 1.0f;
	Grocer.storeLimit = 3;
	Grocer.storeCount = Grocer.storeLimit;
	Grocer.storePos = -4.5f;
}

static void KeyCallBack(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
	// INIT ///////////////////////////////////////////////////////////////
	char *title = "Collaborative AI Graphic";
	glfwSetErrorCallback(ErrorCallBack);
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	width = (float)mode->width;
	height = (float)mode->height;
	GLFWwindow* window = glfwCreateWindow((int)width, (int)height, title, NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Failed to create GLFW windows.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, KeyCallBack);
	glfwSetWindowSizeCallback(window, ResizeCallBack);

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		fprintf(stderr, "Error : %s\n", glewGetErrorString(error));
		exit(EXIT_FAILURE);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, width / height, 0, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Initialize and load our freetype face
	if (FT_Init_FreeType(&ft_lib) != 0)
	{
		fprintf(stderr, "Couldn't initialize FreeType library\n");
		Cleanup();
		exit(EXIT_FAILURE);
	}

	if (FT_New_Face(ft_lib, "arial.ttf", 0, &face) != 0)
	{
		fprintf(stderr, "Unable to load arial.ttf\n");
		Cleanup();
		exit(EXIT_FAILURE);
	}
	// Initialize our texture and VBOs
	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);
	glGenTextures(1, &texture);
	glGenSamplers(1, &sampler);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Initialize shader
	vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &VERTEX_SHADER, 0);
	glCompileShader(vs);

	fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &FRAGMENT_SHADER, 0);
	glCompileShader(fs);

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	// Initialize GL state
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Get shader uniforms
	glUseProgram(program);
	glBindAttribLocation(program, 0, "in_Position");
	texUniform = glGetUniformLocation(program, "tex");
	colorUniform = glGetUniformLocation(program, "color");

	SimulationInit();
	Render(window);
	DoExit();
}

void RenderObjects() // OK
{
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -10.0f);

	// Main Shooter
	// Shooter Alive(Blue) or Killed (Gray)
	if (shooter->GetState() == CGShooter::ALIVE)
		RenderFillCircle(shooter->GetX(), shooter->GetY(),
		shooter->GetRadius(), shooter->GetR(), shooter->GetG(),
		shooter->GetB());
	else
		RenderFillCircle(shooter->GetX(), shooter->GetY(),
		shooter->GetRadius(), 0.25f, 0.25f, 0.25f);

	// Shooter Backup
	if (shooterbackup->GetState() == CGShooter::ALIVE)
		RenderFillCircle(shooterbackup->GetX(), shooterbackup->GetY(),
		shooterbackup->GetRadius(), shooterbackup->GetR(), shooterbackup->GetG(),
		shooterbackup->GetB());
	else
		RenderFillCircle(shooterbackup->GetX(), shooterbackup->GetY(),
		shooterbackup->GetRadius(), 0.25f, 0.25f, 0.25f);

	// Garlic
	RenderFillCircle(garlic->GetX(), garlic->GetY(),
		garlic->GetRadius(), garlic->GetR(), garlic->GetG(),
		garlic->GetB());

	// Vampire
	RenderFillCircle(vampire->GetX(), vampire->GetY(), vampire->GetRadius(),
		vampire->GetR(), vampire->GetG(), vampire->GetB());

	// Ammo Box
	RenderFillCircle(Grocer.storePos, 0.0f, Grocer.storeRadius,
		0.0f, 0.5f, 0.5f);

	// Grocer
	RenderFillCircle(grocer->GetX(), grocer->GetY(),
		grocer->GetRadius(), grocer->GetR(), grocer->GetG(),
		grocer->GetB());

	// Medic
	RenderFillCircle(medic->GetX(), medic->GetY(),
		medic->GetRadius(), medic->GetR(), medic->GetG(),
		medic->GetB());

	// Medic Assistant
	RenderFillCircle(medicassistant->GetX(), medicassistant->GetY(),
		medicassistant->GetRadius(), medicassistant->GetR(), medicassistant->GetG(),
		medicassistant->GetB());

	// Safehouse
	RenderFillCircle(Medic.safehousePos, 0.0f, Medic.safehouseRadius,
		0.0f, 0.5f, 0.5f);


	glPopMatrix();
}

void RunFSM()
{
	float distance;
	MyVector direction, sPos;

	//Soldier
	switch (shooter->GetState())
	{
	case CGShooter::ALIVE:
		if (mb.GetMessage() == "Killed")
		{
			shooter->SetState(CGShooter::DIE);
			shooter->SetRole(" Killed ");
			shooterbackup->SetRole("Main Shooter");
			shooter->SetState(CGShooter::REPLACE);
		}
		if (Garlics.garlicLeft == 0)
		{
			mb.SetMessage("Ammo needed");
			mb.SetFromLabel("Shooter");
			mb.SetToLabel("Refiller");
		}
		break;

	case CGShooter::DIE: if (shooterbackup->GetState() == CGShooter::IDLE)
		shooterbackup->SetState(CGShooter::REPLACE);
		break;
	}

	//Storeman
	switch (shooterbackup->GetState())
	{
	case CGShooter::IDLE:
		if (mb.GetMessage() == "Ammo needed")
		{
			shooterbackup->SetState(CGShooter::REFILL);
			Grocer.storeCount = 0;
		}
		else
		{
			if (Grocer.storeCount == 0)
			{
				mb.SetMessage("Need more garlics");
				mb.SetFromLabel("Refiller");
				mb.SetToLabel("Grocer");
			}
		}
		break;

	case CStoreman::REPLENISHING: direction = (shooterbackup->GetPosition() - shooter->GetPosition()).Normalize();
		distance = GetDistance(shooterbackup->GetX(), shooterbackup->GetY(), shooter->GetX(), shooter->GetY());
		if (distance < shooterbackup->GetSpeed())
		{
			shooterbackup->SetPosition(shooter->GetPosition());
			shooterbackup->SetState(CStoreman::REPLENISHED);
			Bullet.bulletLeft = Bullet.bulletGiven;
			bullet->SetState(CBullet::INACTIVE);
		}
		else
			shooterbackup->SetPosition(shooterbackup->GetPosition() + direction * shooterbackup->GetSpeed());
		break;

	case CStoreman::REPLENISHED: mb.Reset();
		sPos = MyVector(Storeman.storePos, 0.0f);
		direction = (shooterbackup->GetPosition() - sPos).Normalize();
		distance = GetDistance(sPos.GetX(), sPos.GetY(), shooterbackup->GetX(), shooterbackup->GetY());
		if (distance < shooterbackup->GetSpeed())
		{
			shooterbackup->SetPosition(sPos);
			shooterbackup->SetState(CStoreman::STANDBY);
		}
		else
			shooterbackup->SetPosition(shooterbackup->GetPosition() + direction * shooterbackup->GetSpeed());
		break;

	case CStoreman::REPLACING: direction = (shooterbackup->GetPosition() - shooter->GetPosition()).Normalize();
		distance = GetDistance(shooterbackup->GetX(), shooterbackup->GetY(), shooter->GetX(), shooter->GetY());
		if (distance < shooterbackup->GetSpeed())
		{
			shooterbackup->SetPosition(shooter->GetPosition());
			mb.Reset();
			shooterbackup->SetState(CStoreman::ACTING);
			Bullet.bulletLeft += Bullet.bulletGiven;
			bullet->SetState(CBullet::INACTIVE);
		}
		else
			shooterbackup->SetPosition(shooterbackup->GetPosition() + direction * shooterbackup->GetSpeed());
		break;

	case CStoreman::ACTING: break;
	}

	//Supplier
	switch (supplier->GetState())
	{
	case CSupplier::STANDBY: if (mb.GetMessage() == "Need restock")
		supplier->SetState(CSupplier::DELIVERING);
		break;
	case CSupplier::DELIVERING: if (shooterbackup->GetState() == CSupplier::STANDBY)
	{
		direction = (supplier->GetPosition() - shooterbackup->GetPosition()).Normalize();
		distance = GetDistance(shooterbackup->GetX(), shooterbackup->GetY(), supplier->GetX(), supplier->GetY());
		if (distance < supplier->GetSpeed())
		{
			supplier->SetPosition(shooterbackup->GetPosition());
			shooterbackup->SetState(CStoreman::STANDBY);
			Storeman.storeCount = Storeman.storeLimit;
			supplier->SetState(CSupplier::DELIVERED);
			if (Supplier.cycle < Supplier.cycleLimit)
				Supplier.cycle++;
			else
			{
				mb.SetMessage("Killed");
				mb.SetFromLabel("Soldier");
				mb.SetToLabel("Storeman");
			}
		}
		else
			supplier->SetPosition(supplier->GetPosition() + direction * supplier->GetSpeed());
	}
								break;
	case CSupplier::DELIVERED: sPos = MyVector(Storeman.storePos, Supplier.supplierPos);
		direction = (supplier->GetPosition() - sPos).Normalize();
		distance = GetDistance(sPos.GetX(), sPos.GetY(), supplier->GetX(), supplier->GetY());
		if (distance < supplier->GetSpeed())
		{
			supplier->SetPosition(sPos);
			supplier->SetState(CSupplier::STANDBY);
		}
		else
			supplier->SetPosition(supplier->GetPosition() + direction * supplier->GetSpeed());
		mb.Reset();
		break;
	}

	//Bullets
	switch (bullet->GetState())
	{
	case CBullet::INACTIVE:
		if (Bullet.bulletLeft > 0)
		{
			if (shooter->GetState() != CGShooter::KILLED || shooterbackup->GetState() == CStoreman::ACTING)
			{
				bullet->SetState(CBullet::FIRED);
				bullet->SetPosition(Bullet.bulletStartPos, 0.0f);
			}
		}
		break;

	case CBullet::FIRED:
		Bullet.bulletLeft--;
		bullet->SetState(CBullet::MOVING);
		break;

	case CBullet::HIT: bullet->SetState(CBullet::INACTIVE);
		break;

	case CBullet::MOVING:// Update bullet data
		MyVector direction = (bullet->GetPosition() - enemy->GetPosition()).Normalize();
		float distance = GetDistance(bullet->GetPosition().GetX(), bullet->GetPosition().GetY(), enemy->GetX(), enemy->GetY());
		if (distance < bullet->GetSpeed())
		{
			bullet->GetPosition() = enemy->GetPosition();
			bullet->SetState(CBullet::HIT);
		}
		else
			bullet->SetPosition(bullet->GetPosition() + direction * bullet->GetSpeed());
		break;
	}
}

void Render(GLFWwindow* window) // OK
{
	while (!glfwWindowShouldClose(window))
	{
		glUseProgram(0);
		glClear(GL_COLOR_BUFFER_BIT);

		RunFSM();
		RenderObjects();

		// Bind stuff
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindSampler(0, sampler);
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glUseProgram(program);
		glUniform4f(colorUniform, 1, 1, 1, 1);
		glUniform1i(texUniform, 0);

		FT_Set_Pixel_Sizes(face, 0, 50);

		// Render text status
		string stateString = "Soldier State : ";
		switch (shooter->GetState())
		{
		case CGShooter::ALIVE: stateString += "ALIVE";
			break;
		case CGShooter::KILLED: stateString += "KILLED";
			break;
		}
		RenderText(stateString, face, -0.95f, 0.925f, 0.5f, 0.5f);
		stateString = "Soldier Role : " + shooter->GetRole();
		RenderText(stateString, face, -0.95f, 0.825f, 0.5f, 0.5f);
		stateString = "Storeman State : ";
		switch (shooterbackup->GetState())
		{
		case CStoreman::STANDBY: stateString += "STANDBY";
			break;
		case CStoreman::REPLENISHING: stateString += "REPLENISHING";
			break;
		case CStoreman::REPLENISHED: stateString += "REPLENISHED";
			break;
		case CStoreman::REPLACING: stateString += "REPLACING";
			break;
		case CStoreman::ACTING: stateString += "ACTING";
			break;
		}
		RenderText(stateString, face, -0.4f, 0.925f, 0.5f, 0.5f);
		stateString = "Storeman Role : " + shooterbackup->GetRole();
		RenderText(stateString, face, -0.4f, 0.825f, 0.5f, 0.5f);

		stateString = "Supplier State : ";
		switch (supplier->GetState())
		{
		case CSupplier::STANDBY: stateString += "STANDBY";
			break;
		case CSupplier::DELIVERING: stateString += "DELIVERING";
			break;
		case CSupplier::DELIVERED: stateString += "DELIVERED";
			break;
		}
		RenderText(stateString, face, 0.3f, 0.925f, 0.5f, 0.5f);
		stateString = "Supplier Role : " + supplier->GetRole();
		RenderText(stateString, face, 0.3f, 0.825f, 0.5f, 0.5f);
		stateString = "Message Board : ";
		stateString += mb.GetMessageW();
		stateString += " From : " + mb.GetFromLabel() + " To: " + mb.GetToLabel();
		RenderText(stateString, face, -0.95f, -0.525f, 0.5f, 0.5f);
		// Legend
		string s = "Soldier - Blue/Grey Storeman - Green Supplier - Yellow Enemy - Red Ammunition Store - Cyan";
		RenderText(s, face, -0.95f, -0.7f, 0.55f, 0.55f);
		// Bullet level status
		stateString = itos(Bullet.bulletLeft) + "/" + itos(Bullet.bulletGiven);
		RenderText(stateString, face, -0.255f, -0.225f, 0.45f, 0.45f);
		stateString = itos(Storeman.storeCount) + "/" + itos(Storeman.storeLimit);
		RenderText(stateString, face, -0.45f, -0.225f, 0.45f, 0.45f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}