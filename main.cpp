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

#include "Bullet.h"
#include "MessageBoard.h"
#include "Soldier.h"
#include "Storeman.h"
#include "Supplier.h"
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

MyObject *soldier, *storeman, *supplier, *enemy, *bullet;
CBullet Bullet;
CSupplier Supplier;
CStoreman Storeman;
CSoldier Soldier;
CMessageBoard mb;

void SimulationInit()
{
	soldier = new MyObject();
	soldier->SetRadius(0.25f);
	soldier->SetColor(0.0f, 0.0f, 1.0f);
	soldier->SetPosition(-2.5f, 0.0f);
	soldier->SetName("Soldier");
	soldier->SetState(CSoldier::ALIVE);
	soldier->SetRole("Shooter");

	enemy = new MyObject();
	enemy->SetName("Enemy");
	enemy->SetRadius(0.15f);
	enemy->SetColor(1.0f, 0.0f, 0.0f);
	enemy->SetPosition(5.0f, 0.0f);

	bullet = new MyObject();
	bullet->SetSpeed(0.1f);
	bullet->SetName("Bullet");
	bullet->SetState(CBullet::INACTIVE);
	bullet->SetRadius(0.05f), bullet->SetColor(1.0f, 1.0f, 1.0f);
	bullet->SetPosition(soldier->GetX() + soldier->GetRadius(), 0.0f);
	Bullet.bulletGiven = 3;
	Bullet.bulletLeft = Bullet.bulletGiven;
	Bullet.bulletStartPos = soldier->GetX() + soldier->GetRadius();

	storeman = new MyObject();
	storeman->SetName("Storeman");
	storeman->SetSpeed(0.025f);
	storeman->SetRadius(0.15f), storeman->SetPosition(-4.5f, 0.0f);
	storeman->SetColor(0.0f, 1.0f, 0.0f);
	storeman->SetState(CStoreman::STANDBY);
	storeman->SetRole("Replenisher");

	supplier = new MyObject();
	supplier->SetRadius(0.2f), supplier->SetPosition(-4.5f, 2.0f);
	supplier->SetColor(1.0f, 1.0f, 0.0f);
	supplier->SetSpeed(0.05f);
	supplier->SetName("Supplier"); supplier->SetRole("Supplier");
	supplier->SetState(CSupplier::STANDBY);
	Supplier.supplierPos = 2.0;

	Supplier.cycle = 1;
	Storeman.storeRadius = 1.0f;
	Storeman.storeLimit = 3;
	Storeman.storeCount = Storeman.storeLimit;
	Storeman.storePos = -4.5f;
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

	// Shooting
	// Soldier Alive(Blue) or Killed (Gray)
	if (soldier->GetState() == CSoldier::ALIVE)
		RenderFillCircle(soldier->GetX(), soldier->GetY(),
		soldier->GetRadius(), soldier->GetR(), soldier->GetG(),
		soldier->GetB());
	else
		RenderFillCircle(soldier->GetX(), soldier->GetY(),
		soldier->GetRadius(), 0.25f, 0.25f, 0.25f);

	// Bullet
	RenderFillCircle(bullet->GetX(), bullet->GetY(),
		bullet->GetRadius(), bullet->GetR(), bullet->GetG(),
		bullet->GetB());

	// Enemy
	RenderFillCircle(enemy->GetX(), enemy->GetY(), enemy->GetRadius(),
		enemy->GetR(), enemy->GetG(), enemy->GetB());

	// Store
	RenderFillCircle(Storeman.storePos, 0.0f, Storeman.storeRadius,
		0.0f, 0.5f, 0.5f);

	// Storeman
	RenderFillCircle(storeman->GetX(), storeman->GetY(),
		storeman->GetRadius(), storeman->GetR(), storeman->GetG(),
		storeman->GetB());

	// Supplier
	RenderFillCircle(supplier->GetX(), supplier->GetY(),
		supplier->GetRadius(), supplier->GetR(), supplier->GetG(),
		supplier->GetB());

	glPopMatrix();
}

void RunFSM()
{
	float distance;
	MyVector direction, sPos;

	//Soldier
	switch (soldier->GetState())
	{
	case CSoldier::ALIVE:
		if (mb.GetMessage() == "Killed")
		{
			soldier->SetState(CSoldier::KILLED);
			soldier->SetRole(" Killed ");
			storeman->SetRole("Shooter");
			storeman->SetState(CStoreman::REPLACING);
		}
		if (Bullet.bulletLeft == 0)
		{
			mb.SetMessage("Ammo needed");
			mb.SetFromLabel("Soldier");
			mb.SetToLabel("Storeman");
		}
		break;

	case CSoldier::KILLED: if (storeman->GetState() == CStoreman::STANDBY)
		storeman->SetState(CStoreman::REPLACING);
		break;
	}

	//Storeman
	switch (storeman->GetState())
	{
	case CStoreman::STANDBY:
		if (mb.GetMessage() == "Ammo needed")
		{
			storeman->SetState(CStoreman::REPLENISHING);
			Storeman.storeCount = 0;
		}
		else
		{
			if (Storeman.storeCount == 0)
			{
				mb.SetMessage("Need restock");
				mb.SetFromLabel("Storeman");
				mb.SetToLabel("Supplier");
			}
		}
		break;

	case CStoreman::REPLENISHING: direction = (storeman->GetPosition() - soldier->GetPosition()).Normalize();
		distance = GetDistance(storeman->GetX(), storeman->GetY(), soldier->GetX(), soldier->GetY());
		if (distance < storeman->GetSpeed())
		{
			storeman->SetPosition(soldier->GetPosition());
			storeman->SetState(CStoreman::REPLENISHED);
			Bullet.bulletLeft = Bullet.bulletGiven;
			bullet->SetState(CBullet::INACTIVE);
		}
		else
			storeman->SetPosition(storeman->GetPosition() + direction * storeman->GetSpeed());
		break;

	case CStoreman::REPLENISHED: mb.Reset();
		sPos = MyVector(Storeman.storePos, 0.0f);
		direction = (storeman->GetPosition() - sPos).Normalize();
		distance = GetDistance(sPos.GetX(), sPos.GetY(), storeman->GetX(), storeman->GetY());
		if (distance < storeman->GetSpeed())
		{
			storeman->SetPosition(sPos);
			storeman->SetState(CStoreman::STANDBY);
		}
		else
			storeman->SetPosition(storeman->GetPosition() + direction * storeman->GetSpeed());
		break;

	case CStoreman::REPLACING: direction = (storeman->GetPosition() - soldier->GetPosition()).Normalize();
		distance = GetDistance(storeman->GetX(), storeman->GetY(), soldier->GetX(), soldier->GetY());
		if (distance < storeman->GetSpeed())
		{
			storeman->SetPosition(soldier->GetPosition());
			mb.Reset();
			storeman->SetState(CStoreman::ACTING);
			Bullet.bulletLeft += Bullet.bulletGiven;
			bullet->SetState(CBullet::INACTIVE);
		}
		else
			storeman->SetPosition(storeman->GetPosition() + direction * storeman->GetSpeed());
		break;

	case CStoreman::ACTING: break;
	}

	//Supplier
	switch (supplier->GetState())
	{
	case CSupplier::STANDBY: if (mb.GetMessage() == "Need restock")
		supplier->SetState(CSupplier::DELIVERING);
		break;
	case CSupplier::DELIVERING: if (storeman->GetState() == CSupplier::STANDBY)
	{
		direction = (supplier->GetPosition() - storeman->GetPosition()).Normalize();
		distance = GetDistance(storeman->GetX(), storeman->GetY(), supplier->GetX(), supplier->GetY());
		if (distance < supplier->GetSpeed())
		{
			supplier->SetPosition(storeman->GetPosition());
			storeman->SetState(CStoreman::STANDBY);
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
			if (soldier->GetState() != CSoldier::KILLED || storeman->GetState() == CStoreman::ACTING)
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
		switch (soldier->GetState())
		{
		case CSoldier::ALIVE: stateString += "ALIVE";
			break;
		case CSoldier::KILLED: stateString += "KILLED";
			break;
		}
		RenderText(stateString, face, -0.95f, 0.925f, 0.5f, 0.5f);
		stateString = "Soldier Role : " + soldier->GetRole();
		RenderText(stateString, face, -0.95f, 0.825f, 0.5f, 0.5f);
		stateString = "Storeman State : ";
		switch (storeman->GetState())
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
		stateString = "Storeman Role : " + storeman->GetRole();
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