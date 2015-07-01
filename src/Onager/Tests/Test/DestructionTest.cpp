#include "DestructionTest.h"
#include <vector>
#include "Destruction.h"


DestructionTest::DestructionTest()
{
	m_eye.p = vec3(0, 0, 0);
	m_eye.q = Quaternion(vec3(0, 0, 0), 1);
	m_force = 0.0f;
	

}
void DestructionTest::init()
{
	m_world = new World(vec3(0, -0, 0));
	
	initDestruction(m_world);

	BodyDescription descr;
	descr.transform.p = vec3(0, 10, 0);
	descr.transform.q = QuatFromAxisAngle(normalize(vec3(rand(), rand(), rand())), rand());
	descr.transform.q = Quaternion(vec3(0.0, 0, 0), 1);
	descr.linearMomentum = vec3(0, 0, 0);
	descr.angularMomentum = vec3(0, 0, 0);
	descr.type = BodyType::Dynamic;
	
	Ship* ship = createShip(m_world, &m_entities, descr, vec3(1,0,0));
	ship->addBrick(2, -1, 0);
	ship->addBrick(-4, 0, 0);
	ship->addBrick(0, 0, 0);
	ship->addBrick(4, 0, 0);
	ship->addBrick(-6, 1, 0);
	ship->addBrick(-2, 1, 0);
	ship->addBrick(2, 1, 0);
	ship->addBrick(7, 1, 0);
	ship->addBrick(0, 2, 0);
	ship->addBrick(5, 2, 0);
	ship->addBrick(9, 2, 0);
	ship->addBrick(3, 3, 0);
	ship->addBrick(7, 3, 0);
	ship->addBrick(5, 4, 0);

	ship->build();

	m_entities.push_back(addFloor(m_world, m_world->createMaterial({ 1, 0, 1 }), vec3(0, -10, 0)));

	m_stepping = false;
}


bool DestructionTest::procEvent(SDL_Event event)
{
	if (event.type == SDL_MOUSEWHEEL && ((SDL_GetModState()& KMOD_LCTRL) != 0))
	{
		m_force += event.wheel.y * 10.0f;
		printf("force: %f\n", m_force);
		return true;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
	{
		m_click = true;
	}
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
	{
		impulse();
	}

	return false;
}

void DestructionTest::update(float dt)
{
	int x, y;
	if (false && m_click && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_force += 30.0f * dt;
		printf("force: %f\n", m_force);
		int i = impulse();
		if (i == -1 || i == 1)
		{
			
			m_click = false;
		}
	}
}

int DestructionTest::impulse()
{
	int x, y;
	SDL_GetMouseState(&x, &y);
	int w, h;
	SDL_GetWindowSize(m_window, &w, &h);

	float _x = x / (float)w - 0.5f;
	float _y = y / (float)h - 0.5f;

	vec3 dir;

	float aspect = 800.0f / 600.0f;

	float fovY = 60 * ong_PI / 180.0f;
	float fovX = fovY * aspect;


	dir.z = 0.1f;
	dir.y = -tan(_y*fovY) * 0.1f;
	dir.x = tan(_x*fovX) * 0.1f;

	dir = normalize(rotate(dir, m_eye.q));

	BodyDescription descr;
	descr.type = BodyType::Dynamic;
	descr.transform = m_eye;
	descr.linearMomentum = 2000.0f * dir;
	descr.angularMomentum = vec3(0, 0, 0);
	m_entities.push_back(addBox(m_world, descr, m_world->createMaterial({ 10, 0, 0 })));


	//RayQueryResult result = { 0 };
	//if (m_world->queryRay(m_eye.p, dir, &result) && result.collider != 0)
	//{
	//	Brick* brick = (Brick*)result.collider->getUserData();
	//	if (brick->ship->addImpulse(brick, result.point, m_force * dir))
	//	{
	//		return 1;
	//	}
	//	else
	//	{
	//		return 0;
	//	}
	//}
	//else
	//{
	//	return -1;
	//}
	return -1;
}

void DestructionTest::render()
{
}


void DestructionTest::stepPhysics(float dt)
{
	m_physicsTimer += dt;
	while (m_physicsTimer >= 1.0f / 60.0f)
	{
		m_world->step(1.0f / 60.0f);
		m_physicsTimer -= 1.0f / 60.0f;

		updateDestruction();
	}
}

