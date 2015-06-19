#include "DestructionTest.h"


static World* g_world;
static std::vector<Entity*>* g_entities;
static uint32 g_tick;

class Ship;
struct Brick;
void destroyBrick(Brick* brick);


struct Stud
{
	int x;
	int z;
	int y;
};

struct StudConnection
{
	Brick* brick;
	StudConnection* other;
	int numStuds = 0;
	Stud studs[2 * 4];
};


struct Brick
{
	int numConnections = 0;
	StudConnection connections[2 * 4];
	Collider* collider;
	Ship* ship;
	int lastVisited = -1;
};



class Ship : public Entity
{
private:
	Brick* m_base;
	World* m_world;

	friend void destroyBrick(Brick* brick);
public:

	Ship(World* world, Body* body, vec3 color, Brick* base)
		: Entity(body, color),
		m_world(world),
		m_base(base)

	{
	}

	void buildNew()
	{
		ShapeDescription sDescr;
		sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		sDescr.hullFromBox.c = vec3(0, 0, 0);
		sDescr.hullFromBox.e = vec3(2, 0.5, 1);

		ColliderDescription cDescr;
		cDescr.isSensor = false;
		cDescr.material = m_world->createMaterial({ 1, 1, 1 });
		cDescr.shape = m_world->createShape(sDescr);
		cDescr.transform.p = vec3(0, 0, 0);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

		Collider* collider = m_world->createCollider(cDescr);

		m_body->addCollider(collider);

		m_base->collider = collider;
		m_base->collider->setUserData(m_base);
		m_base->ship = this;

		cDescr.transform.p = vec3(2, 1, 0);
		collider = m_world->createCollider(cDescr);
		m_body->addCollider(collider);

		Brick* brick = new Brick;
		brick->collider = collider;
		brick->collider->setUserData(brick);
		brick->ship = this;

		StudConnection* a = &m_base->connections[m_base->numConnections++];
		StudConnection* b = &brick->connections[brick->numConnections++];

		a->brick = m_base;
		a->other = b;
		a->numStuds = 4;
		a->studs[0] = { 0, 0, 0};
		a->studs[1] = { 0, 1, 0};
		a->studs[2] = { 1, 0, 0};
		a->studs[3] = { 1, 1, 0};

		b->brick = brick;
		b->other = a;
		b->numStuds = 4;
		b->studs[0] = { 2, 0, 1};
		b->studs[1] = { 2, 1, 1};
		b->studs[2] = { 3, 0, 1};
		b->studs[3] = { 3, 1, 1};


		cDescr.transform.p = vec3(4, 0, 0);
		collider = m_world->createCollider(cDescr);
		m_body->addCollider(collider);

		Brick* brick2 = new Brick;
		brick2->collider = collider;
		brick2->collider->setUserData(brick2);
		brick2->ship = this;

		a = &brick->connections[brick->numConnections++];
		b = &brick2->connections[brick2->numConnections++];


		a->brick = brick;
		a->other = b;
		a->numStuds = 4;
		a->studs[0] = { 2, 0, 0 };
		a->studs[1] = { 2, 1, 0 };
		a->studs[2] = { 3, 0, 0 };
		a->studs[3] = { 3, 1, 0 };

		b->brick = brick2;
		b->other = a;
		b->numStuds = 4;
		b->studs[0] = { 0, 0, 1 };
		b->studs[1] = { 0, 1, 1 };
		b->studs[2] = { 1, 0, 1 };
		b->studs[3] = { 1, 1, 1 };


		cDescr.transform.p = vec3(6, 1, 0);
		collider = m_world->createCollider(cDescr);
		m_body->addCollider(collider);

		Brick* brick3 = new Brick;
		brick3->collider = collider;
		brick3->collider->setUserData(brick3);
		brick3->ship = this;

		a = &brick2->connections[brick2->numConnections++];
		b = &brick3->connections[brick3->numConnections++];


		a->brick = brick2;
		a->other = b;
		a->numStuds = 4;
		a->studs[0] = { 2, 0, 1 };
		a->studs[1] = { 2, 1, 1 };
		a->studs[2] = { 3, 0, 1 };
		a->studs[3] = { 3, 1, 1 };

		b->brick = brick3;
		b->other = a;
		b->numStuds = 4;
		b->studs[0] = { 0, 0, 0 };
		b->studs[1] = { 0, 1, 0 };
		b->studs[2] = { 1, 0, 0 };
		b->studs[3] = { 1, 1, 0 };

	}

	void addImpulse(Brick* brick, vec3 pos, vec3 impulse)
	{
		if (lengthSq(impulse) >= 250.0f)
		{
			destroyBrick(brick);
			brick->ship->getBody()->applyImpulse(impulse, pos);
		}
		else
		{
			//m_body->applyImpulse(impulse, pos);
		}
	}

};




Ship* createShip(Brick* base, Transform t, BodyType::Type type)
{
	BodyDescription bDescr;
	bDescr.type = type;
	bDescr.linearMomentum = vec3(0, 0, 0);
	bDescr.angularMomentum = vec3(0, 0, 0);
	bDescr.transform = t;

	Ship* ship = new Ship(g_world, g_world->createBody(bDescr), vec3(1, 0, 0), base);
	g_entities->push_back(ship);

	return ship;
}


bool connected(Brick* brick, Brick* base, int tick)
{
	brick->lastVisited = tick;
	for (int i = 0; i < brick->numConnections; ++i)
	{
		Brick* other = brick->connections[i].other->brick;

		if (other == base)
			return true;

		if (other->lastVisited != tick)
		{
			if (connected(other, base, tick))
				return true;
		}
	}

	return false;
}

void setShip(Brick* brick, Ship* ship, int tick)
{
	brick->lastVisited = tick;
	brick->ship->getBody()->removeCollider(brick->collider);
	brick->ship = ship;
	brick->ship->getBody()->addCollider(brick->collider);

	for (int i = 0; i < brick->numConnections; ++i)
	{
		Brick* other = brick->connections[i].other->brick;

		if (other->lastVisited != tick)
		{
			setShip(other, ship, tick);
		}
	}

}

void destroyBrick(Brick* brick)
{
	if (brick->numConnections == 0)
		return;

	if (brick == brick->ship->m_base)
	{
		brick->ship->m_base = brick->connections[0].other->brick;
	}
	

	//unlink
	for (int i = 0; i < brick->numConnections; ++i)
	{
		StudConnection* other = brick->connections[i].other;
		*other = other->brick->connections[--other->brick->numConnections];
		
		// check for disconnecting
		if (other->brick != brick->ship->m_base && !connected(other->brick, brick->ship->m_base, g_tick++))
		{
			//disconnect
			Ship* ship = createShip(other->brick, brick->ship->getBody()->getTransform(), BodyType::Dynamic);
			setShip(other->brick, ship, g_tick++);
		}
	}

	brick->numConnections = 0;
	Ship* newShip = createShip(brick, brick->ship->getBody()->getTransform(), BodyType::Dynamic);
	brick->ship->getBody()->removeCollider(brick->collider);
	brick->ship = newShip;
	brick->ship->getBody()->addCollider(brick->collider);
}



DestructionTest::DestructionTest()
{
	m_eye.p = vec3(0, 0, 0);
	m_eye.q = Quaternion(vec3(0, 0, 0), 1);

	

}
void DestructionTest::init()
{
	m_world = new World();
	
	g_world = m_world;
	g_entities = &m_entities;
	
	Ship* ship = createShip(new Brick, Transform(vec3(0, 0, 15), Quaternion(vec3(0,0,0),1)), BodyType::Static);
	ship->buildNew();

	m_force = 0.0f;
	m_stepping = false;
}


void DestructionTest::update(float dt)
{
	int x, y;
	if (SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_force = ong_MIN(m_force+ 50.0f * dt, 30.0f);
		printf("force: %f\n", m_force);

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
		RayQueryResult result = { 0 };
		if (m_world->queryRay(m_eye.p, dir, &result))
		{
			Brick* brick = (Brick*)result.collider->getUserData();
			brick->ship->addImpulse(brick, result.point, m_force * dir);
		}
		


	}
	else
	{
		m_force = 0.0f;
	}
}