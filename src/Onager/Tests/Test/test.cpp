#define _CRT_SECURE_NO_WARNINGS

#include "test.h"

#include "World.h"

#include "SDL.h"
#include "GL\glew.h"
#include "SDL_opengl.h"
#include <stdio.h>
#include <GL\GLU.h>
#include "Narrowphase.h"
#include "Entity.h"
#include "Player.h"

#undef main


ShapePtr gBox = ShapePtr();
ShapePtr gSlope = ShapePtr();
ShapePtr gFloor = ShapePtr();
ShapePtr gSphere = ShapePtr();
ShapePtr gCapsule = ShapePtr();


void Test::initGL()
{
	m_programID = glCreateProgram();

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
		

		glAttachShader(m_programID, vertexShader);
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

		glAttachShader(m_programID, fragmentShader);
	}

	glLinkProgram(m_programID);

	GLint programSuccess = GL_TRUE;
	glGetProgramiv(m_programID, GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE)
	{
		printf("Error linking program %d!\n", m_programID);
	}

	glUseProgram(m_programID);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error binding shader! %s\n", gluErrorString(error));
	}



	m_colorLocation = glGetUniformLocation(m_programID, "color");
	if (m_colorLocation == -1)
	{
		printf("%s is not a valid glsl program variable!\n", "color");
	}

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 800.0f / 600.0f, 0.1, 1000.0f);
	//glOrtho(-5.0f, 5.0f, -5.0f * 600.0f / 800.0f, 5.0f* 600.0f / 800.0f, 0.1, 1000.0f);
	
	glEnableClientState(GL_VERTEX_ARRAY);

}

void Test::initSDL()
{

	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	m_window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	//m_window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1920, 1080, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);

	m_context = SDL_GL_CreateContext(m_window);

	glewExperimental = GL_TRUE;

	glewInit();

	initGL();

#ifndef _DEBUG
	//SDL_GL_SetSwapInterval(0);
#endif
	

}


Entity* Test::addBox(World* world, BodyDescription descr, Material* material)
{
	if (!gBox)
	{
		ShapeDescription descr;
		descr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		descr.hullFromBox.c = vec3(0, 0, 0);
		descr.hullFromBox.e = vec3(1, 1, 1);

		gBox = world->createShape(descr);
	}

	ColliderDescription colliderDescr;
	colliderDescr.material = material;
	colliderDescr.transform.p = vec3(0.0f, 0.0f, 0.0f);
	colliderDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	colliderDescr.shape = gBox;
	colliderDescr.isSensor = false;

	Collider* collider = world->createCollider(colliderDescr);

	//BodyDescription bodyDescr;
	//bodyDescr.type = BodyType::Dynamic;
	//bodyDescr.transform = transform;
	//bodyDescr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	//bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

	Body* body = world->createBody(descr);

	body->addCollider(collider);

	return new Entity(body, vec3(0,0,1));
}

Entity* Test::addSlope(World* world, Transform t, Material* material)
{
	if (!gSlope)
	{
		vec3 slope[6] =
		{
			vec3(-8, -1, -4),
			vec3(-8, 1, -4),
			vec3(8, -1, -4),
			vec3(8, 1, -4),

			vec3(-8, -1, 4),
			vec3(8, -1, 4),
		};

		ShapeDescription descr;
		descr.constructionType = ShapeConstruction::HULL_FROM_POINTS;
		descr.hullFromPoints.points = slope;
		descr.hullFromPoints.numPoints = 6;

		gSlope = world->createShape(descr);

	}

	ColliderDescription colliderDescr;
	colliderDescr.material = material;
	colliderDescr.transform.p = vec3(0.0f, 0.0f, 0.0f);
	colliderDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	colliderDescr.shape = gSlope;
	colliderDescr.isSensor = false;

	Collider* collider = world->createCollider(colliderDescr);

	BodyDescription bodyDescr;
	bodyDescr.type = BodyType::Static;
	bodyDescr.transform = t;
	bodyDescr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

	Body* body = world->createBody(bodyDescr);

	body->addCollider(collider);

	return new Entity(body, vec3(1,0,0));

}

Entity* Test::addFloor(World* world, Material* material, vec3 pos)
{

	if (!gFloor)
	{
		ShapeDescription descr;
		descr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		descr.hullFromBox.c = vec3(0, 0, 0);
		descr.hullFromBox.e = vec3(30, 1, 30);
		gFloor = world->createShape(descr);
	}

	ColliderDescription colliderDescr;
	colliderDescr.material = material;
	colliderDescr.transform.p = vec3(0.0f, 0.0f, 0.0f);
	colliderDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	colliderDescr.shape = gFloor;
	colliderDescr.isSensor = false;

	Collider* collider = world->createCollider(colliderDescr);

	BodyDescription bodyDescr;
	bodyDescr.type = BodyType::Static;
	bodyDescr.transform.p= vec3(0.0f, -2.0f, 0.0f) + pos;
	bodyDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	bodyDescr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

	Body* body = world->createBody(bodyDescr);

	body->addCollider(collider);

	return new Entity(body, vec3(1,0,0));
}


Player* Test::addPlayer(World* world, const Transform& transform)
{

	Material m;
	m.friction = 0.1f;
	m.density = 200.0f;
	m.restitution = 0.5f;

	Material* material = world->createMaterial(m);

	vec3 jetHull[8] =
	{
		vec3(	-0.25f,	-0.25f,  0.5f),
		vec3(	-0.25f, -0.25f, -0.5f),
		vec3(	 0.25f,	-0.25f, -0.5f),
		vec3(	 0.25f,	-0.25f,  0.5f),
			
		vec3(	-0.25f,	0.25f,	 0.5f),
		vec3(	-0.25f,	0.25f,	-0.5f),
		vec3(	 0.25f,	0.25f,	-0.5f),
		vec3(	 0.25f,	0.25f,	 0.5f)
	};


	ShapeDescription shapeDescr;
	shapeDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
	shapeDescr.hullFromBox.c = vec3(0, 0, 0);
	shapeDescr.hullFromBox.e = vec3(0.25f, 0.25f, 0.5f);
	ShapePtr shape = world->createShape(shapeDescr);

	ColliderDescription colliderDescr;
	colliderDescr.material = material;
	colliderDescr.transform.p = vec3(0.75f, 0.0f, 0.0f);
	colliderDescr.transform.q = QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f);
	colliderDescr.shape = shape;
	colliderDescr.isSensor = false;

	Collider* jet1 = world->createCollider(colliderDescr);

	colliderDescr.transform.p = vec3(-0.75f, 0.0f, 0.0f);
	Collider* jet2 = world->createCollider(colliderDescr);
	
	
	shapeDescr.hullFromBox.e = vec3(0.5, 0.5, 1.0f);
	shape = world->createShape(shapeDescr);

	colliderDescr.transform.p = vec3(0.0f, 0.0f, 1.0f);
	colliderDescr.shape = shape;

	Collider* bodyC = world->createCollider(colliderDescr);

	BodyDescription bodyDescr;
	bodyDescr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);
	bodyDescr.transform = transform;
	bodyDescr.type = BodyType::Dynamic;

	Body* body = world->createBody(bodyDescr);

	body->addCollider(jet1);
	body->addCollider(jet2);
	body->addCollider(bodyC);
	
	return new Player(body, vec3(0.0f, 1.0f, 0.0f), m_window, &m_toAdd);
}

void getViewMat(const Transform& t, float* viewMat)
{
	
	mat3x3 rot = transpose(toRotMat(t.q));

	vec3 l = -rot[2];
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


Test::Test()
{
	initSDL();

	m_eye = Transform(vec3(20, 0, 0), QuatFromAxisAngle(vec3(0, 1, 0), 0.0f * ong_PI / 2.0f));

	
	m_stepping = true;
	//m_playerSpec = true;
}

void Test::init()
{
	m_world = new World(vec3(0.0f, -10.0f, 0.0f));
	//m_world = new World(vec3(0.0f, 0.0f, 0.0f));


	gBox = ShapePtr();
	gCapsule = ShapePtr();
	gFloor = ShapePtr();
	gSlope = ShapePtr();
	gSphere = ShapePtr();

	Material m;
	m.density = 10.0f;
	m.friction = 1.0f;
	m.restitution = 0.0f;

	Material* material = m_world->createMaterial(m);

	BodyDescription descr;
	descr.type = BodyType::Dynamic;
	descr.transform = Transform(vec3(0.0f, 5.0f, -4.0f), QuatFromAxisAngle(vec3(1.0f, 2.0f, 3.0f), 1.0f));
	descr.linearMomentum = vec3(0.0f, 0.0f, 0.0f);
	descr.angularMomentum = vec3(0.0f, 0.0f, 0.0f);

	m_entities.push_back(addBox(m_world, descr, material));

	ShapeDescription shapeDescr;
	shapeDescr.shapeType = ShapeType::CAPSULE;
	shapeDescr.capsule.r = 0.5f;
	shapeDescr.capsule.c1 = vec3(0.0f, 0.5f, 0.0f);
	shapeDescr.capsule.c2 = vec3(0.0f, -0.5f, 0.0f);

	gCapsule = m_world->createShape(shapeDescr);

	ColliderDescription colliderDescr;
	colliderDescr.material = material;
	colliderDescr.transform = Transform(vec3(0, 0, 0), QuatFromAxisAngle(vec3(1, 0, 0), 0));
	colliderDescr.shape = gCapsule;
	colliderDescr.isSensor = false;

	descr.transform.p.x += 3.0f;

	Collider* c = m_world->createCollider(colliderDescr);
	Body* b = m_world->createBody(descr);
	b->addCollider(c);
	m_entities.push_back(new Entity(b, vec3(0, 0, 1)));



	// spherestack
	{

		shapeDescr.shapeType = ShapeType::SPHERE;
		shapeDescr.sphere.r = 1.0f;
		shapeDescr.sphere.c = vec3(0, 0, 0);
		gSphere = m_world->createShape(shapeDescr);

		colliderDescr.shape = gSphere;

		descr.transform.p.x += 3.0f;
		Collider* c = m_world->createCollider(colliderDescr);
		Body* b = m_world->createBody(descr);
		b->addCollider(c);
		m_entities.push_back(new Entity(b, vec3(0, 0, 1)));




		descr.transform = Transform(vec3(-10.0f, 3.0f, -10.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));

		for (int i = 0; i < 4; ++i)
		{
			Collider* c = m_world->createCollider(colliderDescr);

			Body* b = m_world->createBody(descr);
			b->addCollider(c);
			m_entities.push_back(new Entity(b, vec3(0, 0, 1)));

			descr.transform.p.y += 3.0f;
		}
	}

	colliderDescr.shape = gCapsule;
	descr.transform = Transform(vec3(-10.0f, 3.0f, 10.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));

	for (int i = 0; i < 4; ++i)
	{
		Collider* c = m_world->createCollider(colliderDescr);

		Body* b = m_world->createBody(descr);
		b->addCollider(c);
		m_entities.push_back(new Entity(b, vec3(0, 0, 1)));

		descr.transform.p.y += 3.0f;
	}

	
	//pyramid
	descr.transform = Transform(vec3(10.0f, 0.0f, 0.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform = Transform(vec3(10.0f, 0.0f, -2.5f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform = Transform(vec3(10.0f, 0.0f, +2.5f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));

	descr.transform = Transform(vec3(10.0f, 2.125f, -1.25f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform = Transform(vec3(10.0f, 2.125f, +1.25f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));

	descr.transform = Transform(vec3(10.0f, 4.25f, 0.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));






	descr.transform = Transform(vec3(-10.0f, 3.0f, 0.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform.p.y += 3.0;
	m_entities.push_back(addBox(m_world, descr, material));

	
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			m_entities.push_back(addFloor(m_world, material, vec3(i *60, 0, j*60)));

		}
	}
	m_entities.push_back(addSlope(m_world, Transform(vec3(0, 0, -4), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f)), material));
	m_entities.push_back(addSlope(m_world, Transform(vec3(0, 0, 4), QuatFromAxisAngle(vec3(0.0f, 1.0f, 0.0f), ong_PI)), material));
	
	descr.transform = Transform(vec3(-3.0f, 5.0f, 5.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m_entities.push_back(addBox(m_world, descr, material));
	m.friction /= 2.0f;
	material = m_world->createMaterial(m);
	descr.transform = Transform(vec3(0.0f, 5.0f, 5.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m.friction /= 2.0f;
	material = m_world->createMaterial(m);
	m_entities.push_back(addBox(m_world, descr, material));
	descr.transform = Transform(vec3(3.0f, 5.0f, 5.0f), QuatFromAxisAngle(vec3(1.0f, 0.0f, 0.0f), 0.0f));
	m.friction /= 2.0f;
	material = m_world->createMaterial(m);
	m_entities.push_back(addBox(m_world, descr, material));

	
	//player
	Transform t = Transform(vec3(0.0f, 8.0f, -10.0f), QuatFromAxisAngle(vec3(1, 0, 0), 0));
	m_player = addPlayer(m_world, t);
	m_entities.push_back(m_player);



	static int gNumSteps = 0;
	int numSteps = 0;
	for (int i = 0; i < numSteps; ++i)
	{
		printf("%d\n", gNumSteps);
		m_world->step(1.0f / 60.0f);
		++gNumSteps;
	}


}



void Test::stepPhysics(float dt)
{
	m_physicsTimer += dt;
	while (m_physicsTimer >= 1.0f / 60.0f)
	{
		m_world->step(1.0f / 60.0f);
		m_physicsTimer -= 1.0f / 60.0f;
	}
}



void Test::run()
{
	
	bool running = true;
	int lastTime = SDL_GetTicks();

	m_physicsTimer = 0.0f;
	float fps = 0.0f;
	int fpsCount = 1;
	while (running)
	{
		int thisTime = SDL_GetTicks();
		float dt = (thisTime - lastTime) / 1000.0f;
		lastTime = thisTime;

		if (dt >= 1.0f)
			continue;


		fps += dt;
		fpsCount++;
		if (fps >= 1.0f)
		{
			fps -= 1.0f;
			printf("FPS: %d\n", fpsCount);
			fpsCount = 0;
		}
		

		bool step = false;

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (procEvent(event))
				continue;


			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_K)
					step = true;
				else if ((event.key.keysym.scancode == SDL_SCANCODE_SPACE))
					m_stepping = !m_stepping;
				else if ((event.key.keysym.scancode == SDL_SCANCODE_R))
				{
					m_entities.clear();
					delete m_world;
					gBox = ShapePtr();
					gCapsule = ShapePtr();
					gFloor = ShapePtr();
					gSlope = ShapePtr();
					gSphere = ShapePtr();
					init();
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_V)
				{
					m_playerSpec = !m_playerSpec;
				}
				break;
			case SDL_MOUSEWHEEL:
				m_eye.p += rotate(vec3(0, 0, event.wheel.y), m_eye.q);
				break;
			case SDL_MOUSEMOTION:


				if (SDL_GetMouseState(0, 0) & SDL_BUTTON(SDL_BUTTON_LEFT))
				{
					m_eye.q = QuatFromAxisAngle(vec3(0, 1, 0), event.motion.xrel / 4.0f * 3.14 / 180.0f) * m_eye.q;
					m_eye.q = QuatFromAxisAngle(rotate(vec3(1, 0, 0), m_eye.q), event.motion.yrel / 4.0f * 3.14 / 180.0f) * m_eye.q;
				}

				if (SDL_GetMouseState(0, 0) & SDL_BUTTON(SDL_BUTTON_MIDDLE))
				{
					m_eye.p += rotate(vec3(-event.motion.xrel / 80.0f, event.motion.yrel / 80.0f, 0.0f), m_eye.q);
				}
			}

		}




		if (m_stepping)
		{
			if (step)
			{
				int numSteps = 1;
				for (int i = 0; i < numSteps; ++i)
				{
					printf("%d\n", m_numSteps);
					m_world->step(1.0f / 60.0f);
					++m_numSteps;
				}
			}
		}
		else
		{
			stepPhysics(dt);
		}
		
		std::vector<Entity*> deadEntities;

		for (int i = 0; i < m_entities.size(); ++i)
		{
			m_entities[i]->update(dt);
			if (m_entities[i]->isDead())
			{
				if (m_entities[i] == m_player)
					m_player = 0;

				//m_entities[i]->destroy(m_toAdd);

				//delete m_entities[i];
				m_entities[i] = m_entities.back();
				m_entities.pop_back();
				--i;
			}
		}

		for (Entity* entity : m_toAdd)
		{
			m_entities.push_back(entity);
		}
		m_toAdd.clear();


		update(dt);
		// render


		//
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60.0f, 800.0f / 600.0f, 0.1, 1000.0f);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();


		//gluLookAt(eye.x, eye.y, eye.z,
		//	0.0f, 0.0f, 0.0f,
		//	0.0f, 1.0f, 0.0f);
		float view[16];
		if (m_playerSpec && m_player)
			getViewMat(m_player->getView(), view);
		else
			getViewMat(m_eye, view);
		glMultMatrixf(view);



		for (Entity* e : m_entities)
		{
			
			e->render(m_colorLocation);
		}

		render();

		SDL_GL_SwapWindow(m_window);


	}
}