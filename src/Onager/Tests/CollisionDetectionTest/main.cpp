#define _CRT_SECURE_NO_WARNINGS


#include "SDL.h"
#include "GL\glew.h"
#include "SDL_opengl.h"
#include <stdio.h>
#include <GL\GLU.h>
#include <Shapes.h>
#include <SAT.h>
#include <QuickHull.h>
#include "Narrowphase.h"

#undef main

SDL_Window* gWindow;

SDL_GLContext gContext;

GLuint gProgramID = 0;
GLuint gColorLocation = 0;


using namespace ong;

void initGL()
{
	gProgramID = glCreateProgram();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);


	{
		char source[1024] = { 0 };

		FILE* vsFile = fopen("shader.vs", "r");
		fread(source, sizeof(char), 1024, vsFile);
		fclose(vsFile);

		GLchar *vertexShaderSource[] = { source };

		glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
		glCompileShader(vertexShader);

		GLint vShaderCompiled = GL_FALSE;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vShaderCompiled);
		if (vShaderCompiled != GL_TRUE)
		{
			printf("Unable to compile vertex shader %d!\n", vertexShader);

			//Shader log length
			int infoLogLength = 0;
			int maxLength = infoLogLength;

			//Get info string length
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			//Allocate string
			char* infoLog = new char[maxLength];

			//Get info log
			glGetShaderInfoLog(vertexShader, maxLength, &infoLogLength, infoLog);
			if (infoLogLength > 0)
			{
				//Print Log
				printf("%s\n", infoLog);
			}

			//Deallocate string
			delete[] infoLog;
		}


		glAttachShader(gProgramID, vertexShader);
	}

	{
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		char source[1024] = { 0 };

		FILE* fsFile = fopen("shader.ps", "r");
		fread(source, sizeof(char), 1025, fsFile);
		fclose(fsFile);

		GLchar* fragmentShaderSource[] = { source };

		glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);

		glCompileShader(fragmentShader);


		GLint fShaderCompiled = GL_FALSE;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fShaderCompiled);
		if (fShaderCompiled != GL_TRUE)
		{
			printf("Unable to compile fragment shader %d!\n", vertexShader);
		}

		glAttachShader(gProgramID, fragmentShader);
	}

	glLinkProgram(gProgramID);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(gProgramID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("Error linking program %d!\n", gProgramID);
	}

	glUseProgram(gProgramID);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error binding shader! %s\n", gluErrorString(error));
	}

	gColorLocation = glGetUniformLocation(gProgramID, "color");
	if (gColorLocation == -1)
	{
		printf("%s is not a valid glsl program variable!\n", "color");
	}
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.0f, 800.0f / 600.0f, 0.1, 1000.0f);
	//glOrtho(-5.0f, 5.0f, -5.0f * 600.0f / 800.0f, 5.0f* 600.0f / 800.0f, 0.1, 1000.0f);

	glEnableClientState(GL_VERTEX_ARRAY);

}

void init()
{

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	gWindow = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	gContext = SDL_GL_CreateContext(gWindow);

	glewExperimental = GL_TRUE;

	glewInit();

	initGL();


}



void getRotMat(const Quaternion& _q, float* rotMat)
{

	mat3x3 q = toRotMat(_q);

	float rot[16] =
	{
		q[0][0], q[0][1], q[0][2], 0.0f,
		q[1][0], q[1][1], q[1][2], 0.0f,
		q[2][0], q[2][1], q[2][2], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	memcpy(rotMat, rot, sizeof(float) * 16);
}


void getViewMat(const Transform& t, float* viewMat)
{
	//vec3 l = rotate(vec3(0, 0, 1), t.q);
	//vec3 u = rotate(vec3(0, 1, 0), t.q);
	
	mat3x3 rot = toRotMat(t.q);
	vec3 l = rot[2];
	vec3 u = rot[1];

	vec3 s = cross(l, u);

	float view[16] =
	{
		s.x, u.x, l.x, 0.0f,
		s.y, u.y, l.y, 0.0f,
		s.z, u.z, l.z, 0.0f,
		-dot(s, t.p), -dot(u, t.p), -dot(l, t.p), 1.0f
	};

	memcpy(viewMat, view, sizeof(float) * 16);
}


int main(int argc, char* args[])
{

	init();

	Hull hull;
	Hull slopeHull;
	GLuint vb = 0;
	GLuint vbSlope = 0;

	Transform t1 = Transform(vec3(0, 0, 2), QuatFromAxisAngle(vec3(1.0, 0.0f, 0.0f), 0.0f));;
	Transform t2 = Transform(vec3(0,0,0), QuatFromAxisAngle(vec3(1.0, 0.0f, 0.0f), 0.0f));
	Transform eye = Transform(vec3(00, 00, 10.0f), QuatFromAxisAngle(vec3(1.0, 0.0f, 0.0f), 0.0f));

	vec3 box[8] =
	{
		vec3(-1, -1, 1),
		vec3(-1, -1, -1),
		vec3(1, -1, -1),
		vec3(1, -1, 1),

		vec3(-1, 1, 1),
		vec3(-1, 1, -1),
		vec3(1, 1, -1),
		vec3(1, 1, 1)
	};

	vec3 slope[6] =
	{
		vec3(-2, -1, -4),
		vec3(-2, 1, -4),
		vec3(2, -1, -4),
		vec3(2, 1, -4),

		vec3(-2, -1, 4),
		vec3(2, -1, 4),
	};



	quickHull(box, 8, &hull);
	quickHull(slope, 6, &slopeHull);

	vec3 v[24];
	for (int i = 0; i < hull.numEdges; i += 2)
	{
		HalfEdge* e = hull.pEdges + i;
		HalfEdge* et = hull.pEdges + e->twin;

		v[i] = hull.pVertices[e->tail];
		v[i + 1] = hull.pVertices[et->tail];
	}
	glGenBuffers(1, &vb);
	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, hull.numEdges * sizeof(vec3), v, GL_STATIC_DRAW);




	vec3 vS[24];
	for (int i = 0; i < slopeHull.numEdges; i += 2)
	{
		HalfEdge* e = slopeHull.pEdges + i;
		HalfEdge* et = slopeHull.pEdges + e->twin;

		vS[i] = slopeHull.pVertices[e->tail];
		vS[i + 1] = slopeHull.pVertices[et->tail];
	}
	glGenBuffers(1, &vbSlope);
	glBindBuffer(GL_ARRAY_BUFFER, vbSlope);
	glBufferData(GL_ARRAY_BUFFER, slopeHull.numEdges * sizeof(vec3), vS, GL_STATIC_DRAW);




	//update

	bool running = true;
	int lastTime = SDL_GetTicks();

	float physics = 0.0f;
	float fps = 0.0f;
	int fpsCount = 1;
	while (running)
	{
		int thisTime = SDL_GetTicks();
		float dt = (thisTime - lastTime) / 1000.0f;
		lastTime = thisTime;

		if (dt >= 0.2f)
		{
			continue;
		}

		fps += dt;
		fpsCount++;
		if (fps >= 1.0f)
		{
			fps -= 1.0f;
			printf("FPS: %d\n", fpsCount);
			fpsCount = 0;
		}



		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
			{
				char* keys = 0;
				Transform* t = (SDL_GetKeyboardState(0))[SDL_SCANCODE_X] ? &t2 : &t1;

				if (event.key.keysym.mod & KMOD_CTRL)
				{

					float angle = 0.5f * ong_PI * 0.1f;

					switch (event.key.keysym.scancode)
					{
					case SDL_SCANCODE_KP_8:
						t->q = QuatFromAxisAngle(vec3(0, 0, 1), -angle) * t->q;
						break;
					case SDL_SCANCODE_KP_2:
						t->q = QuatFromAxisAngle(vec3(0, 0, 1), angle) * t->q;
						break;
					case SDL_SCANCODE_KP_4:
						t->q = QuatFromAxisAngle(vec3(1, 0, 0), -angle) * t->q;
						break;
					case SDL_SCANCODE_KP_6:
						t->q = QuatFromAxisAngle(vec3(1, 0, 0), angle) * t->q;
						break;
					case SDL_SCANCODE_KP_7:
						t->q = QuatFromAxisAngle(vec3(0, 1, 0), -angle) * t->q;
						break;
					case SDL_SCANCODE_KP_9:
						t->q = QuatFromAxisAngle(vec3(0, 1, 0), angle) * t->q;
						break;
					}
				}
				else
				{
					switch (event.key.keysym.scancode)
					{
					case SDL_SCANCODE_KP_8:
						t->p.x -= 0.1;
						break;
					case SDL_SCANCODE_KP_2:
						t->p.x += 0.1;
						break;
					case SDL_SCANCODE_KP_4:
						t->p.z += 0.1;
						break;
					case SDL_SCANCODE_KP_6:
						t->p.z -= 0.1;
						break;
					case SDL_SCANCODE_KP_7:
						t->p.y -= 0.1;
						break;
					case SDL_SCANCODE_KP_9:
						t->p.y += 0.1;
						break;

					}
				}

				break;
			}
			case SDL_MOUSEWHEEL:
				eye.p += rotate(vec3(0, 0, -event.wheel.y), eye.q);
				break;
			case SDL_MOUSEMOTION:
				

				if (SDL_GetMouseState(0,0) & SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					eye.q = QuatFromAxisAngle(vec3(0, 1, 0), event.motion.xrel/4.0f * 3.14 / 180.0f) * eye.q;
					eye.q = QuatFromAxisAngle(rotate(vec3(1, 0, 0), eye.q), -event.motion.yrel/4.0f * 3.14 / 180.0f) * eye.q;
				}
				
				if (SDL_GetMouseState(0, 0) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
				{
					eye.p += rotate(vec3(event.motion.xrel / 80.0f, event.motion.yrel / 80.0f, 0.0f), eye.q);
				}


				break;
			}
		}



		ContactManifold contact;
		SAT(&hull, &t1, &slopeHull, &t2, &contact);


		// render

		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glVertexPointer(3, GL_FLOAT, 0, 0);


		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//gluLookAt(eye.x, eye.y, eye.z,
		//	0.0f, 0.0f, 0.0f,
		//	0.0f, 1.0f, 0.0f);
		float view[16];
		getViewMat(eye, view);
		glMultMatrixf(view);

		
		{
			//AXES
			glUniform3f(gColorLocation, 0.5, 0, 0);
			glBegin(GL_LINES);
			glVertex3f(0 ,0, 0);
			glVertex3f(1, 0, 0);
			glEnd();
			
			glUniform3f(gColorLocation, 0, 0.5, 0);
			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 1, 0);
			glEnd();

			glUniform3f(gColorLocation, 0, 0, 0.5);
			glBegin(GL_LINES);
			glVertex3f(0, 0, 0);
			glVertex3f(0, 0, 1);
			glEnd();


		}
		glPushMatrix();
		{
			glBindBuffer(GL_ARRAY_BUFFER, vb);

			glTranslatef(t1.p.x, t1.p.y, t1.p.z);
			float rot[16];
			getRotMat(t1.q, rot);
			glMultTransposeMatrixf(rot);

			glUniform3f(gColorLocation, 0, 0, 1);
			glDrawArrays(GL_LINES, 0, 24);

			glBegin(GL_LINES);
			glVertex3f(0, 0, 1);
			glVertex3f(0, 0, 2);
			glEnd();


			glUniform3f(gColorLocation, 0, 1, 0);
			glBegin(GL_LINES);
			glVertex3f(0, 1, 0);
			glVertex3f(0, 2, 0);
			glEnd();


		}
		glPopMatrix();
		glPushMatrix();
		{

			glBindBuffer(GL_ARRAY_BUFFER, vbSlope);
			glVertexPointer(3, GL_FLOAT, 0, 0);

			glTranslatef(t2.p.x, t2.p.y, t2.p.z);
			float rot[16];
			getRotMat(t2.q, rot);
			glMultTransposeMatrixf(rot);


			glUniform3f(gColorLocation, 1, 0, 0);
			glDrawArrays(GL_LINES, 0, slopeHull.numEdges);


		}
		glPopMatrix();
		{
			for (int i = 0; i < contact.numPoints; ++i)
			{
				vec3 p = contact.points[i].position;
				vec3 pen = -contact.points[i].penetration * contact.normal;


				glUniform3f(gColorLocation, 0, 1, 0);
				glBegin(GL_LINES);
				glVertex3f(p.x, p.y, p.z);
				glVertex3f(p.x + contact.normal.x, p.y + contact.normal.y, p.z + contact.normal.z);
				glEnd();
				
				glUniform3f(gColorLocation, 1, 0, 1);
				glBegin(GL_LINES);
				glVertex3f(p.x, p.y, p.z);
				glVertex3f(p.x + pen.x, p.y + pen.y, p.z + pen.z);
				glEnd();
			}
		}
	


		SDL_GL_SwapWindow(gWindow);


	}



	return 0;
}