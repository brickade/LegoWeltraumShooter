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
	enum Type
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		FRONT,
		BACK
	} type;

	Brick* brick;
	StudConnection* other;
	int numStuds = 0;
	Stud studs[2 * 4];
};


struct Brick
{
	int pos[3];
	int numConnections = 0;
	StudConnection connections[2 * 4 * 2];
	Collider* collider;
	Ship* ship;
	int lastVisited = -1;
	uint8 block[6];
};



class Ship : public Entity
{
private:
	
	Brick* m_base;
	World* m_world;

	friend void destroyBrick(Brick* brick);
	
public:
	static ShapePtr brickShape;

	Ship(World* world, Body* body, vec3 color, Brick* base)
		: Entity(body, color),
		m_world(world),
		m_base(base)

	{
		if (!brickShape)
		{
			ShapeDescription sDescr;
			sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
			sDescr.hullFromBox.c = vec3(0, 0, 0);
			sDescr.hullFromBox.e = vec3(2, 0.5, 1);
			brickShape = m_world->createShape(sDescr);
		}
	}

	void addBrick(int x, int y, int z)
	{

		if (m_body->queryShape(brickShape, transformTransform(Transform(vec3(x, y, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform())))
		{
			return;
		}

		ColliderDescription cDescr;
		cDescr.isSensor = false;
		cDescr.material = m_world->createMaterial({ 1, 1, 1 });
		cDescr.shape = brickShape;
		cDescr.transform.p = vec3(x,y,z);
		cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

		Collider* collider = m_world->createCollider(cDescr);

		
		Brick* brick = new Brick;
		for (int i = 0; i < 6; ++i)
			brick->block[i] = 0;

		if (m_base == 0)
			m_base = brick;
		
		brick->pos[0] = x;
		brick->pos[1] = y;
		brick->pos[2] = z;
		brick->collider = collider;
		brick->collider->setUserData(brick);
		brick->ship = this;

		//up
		ShapeQueryCallBack studCallback = [](Collider* other, void* userData){
			
			Brick* brick = (Brick*)userData;
			Brick* brick2 = (Brick*)other->getUserData();

		
			int dy = brick2->pos[1] - brick->pos[1];
		
			if (dy == 1 || dy == -1)
			{
				int dx = brick2->pos[0] - brick->pos[0];
				int dz = brick2->pos[2] - brick->pos[2];

				int minX = 0, maxX = 0;
				if (dx >= 0 && dx < 4)
					minX = dx, maxX = 3;
				else
					minX = 0, maxX = 4 + dx;

				int minZ = 0, maxZ = 0;
				if (dz >= 0 && dz < 2)
					minZ = dz, maxZ = 1;
				else
					minZ = 0, maxZ = 2 + dz;

				StudConnection* a = &brick->connections[brick->numConnections++];
				StudConnection* b = &brick2->connections[brick2->numConnections++];
				a->brick = brick;
				a->other = b;
				b->brick = brick2;
				b->other = a;
				for (int x = minX; x <= maxX; ++x)
				{
					for (int z = minZ; z <= maxZ; ++z)
					{
						int y;
						if (dy == 1)
						{
							y = 1;
							a->type = StudConnection::DOWN;
							b->type = StudConnection::UP;
							brick->block[StudConnection::UP]++;
							brick2->block[StudConnection::DOWN]++;
						}
						else if (dy == -1)
						{
							y = 0;
							a->type = StudConnection::UP;
							b->type = StudConnection::DOWN;
							brick->block[StudConnection::DOWN]++;
							brick2->block[StudConnection::UP]++;
						}


						int i = a->numStuds++;
						int j = b->numStuds++;

						a->studs[i] = { x, z, y};
						b->studs[j] = { x - dx, 1-y};
					}
				}

			}


			return true;
		};

		ShapeQueryCallBack leftBlockCallback = [](Collider* other, void* userData){
			Brick* brick = (Brick*)userData;
			Brick* brick2 = (Brick*)other->getUserData();

			brick->block[StudConnection::LEFT]++;
			brick2->block[StudConnection::RIGHT]++;

			StudConnection* a = &brick->connections[brick->numConnections++];
			StudConnection* b = &brick2->connections[brick2->numConnections++];
			a->other = b;
			a->brick = brick;
			b->other = a;
			b->brick = brick2;
			a->type = StudConnection::RIGHT;
			b->type = StudConnection::LEFT;

			return true;
		};
		ShapeQueryCallBack rightBlockCallback = [](Collider* other, void* userData){
			Brick* brick = (Brick*)userData;
			Brick* brick2 = (Brick*)other->getUserData();

			brick->block[StudConnection::RIGHT]++;
			brick2->block[StudConnection::LEFT]++;

			StudConnection* a = &brick->connections[brick->numConnections++];
			StudConnection* b = &brick2->connections[brick2->numConnections++];
			a->other = b;
			a->brick = brick;
			b->other = a;
			b->brick = brick2;
			a->type = StudConnection::LEFT;
			b->type = StudConnection::RIGHT;
			return true;
		};
		ShapeQueryCallBack frontBlockCallback = [](Collider* other, void* userData){
			Brick* brick = (Brick*)userData;
			Brick* brick2 = (Brick*)other->getUserData();

			brick->block[StudConnection::FRONT]++;
			brick2->block[StudConnection::BACK]++;
			
			StudConnection* a = &brick->connections[brick->numConnections++];
			StudConnection* b = &brick2->connections[brick2->numConnections++];
			a->other = b;
			a->brick = brick;
			b->other = a;
			b->brick = brick2;
			a->type = StudConnection::BACK;
			b->type = StudConnection::FRONT;
			return true;		
		};
		ShapeQueryCallBack backBlockCallback = [](Collider* other, void* userData){
			Brick* brick = (Brick*)userData;
			Brick* brick2 = (Brick*)other->getUserData();

			brick->block[StudConnection::BACK]++;
			brick2->block[StudConnection::FRONT]++;

			StudConnection* a = &brick->connections[brick->numConnections++];
			StudConnection* b = &brick2->connections[brick2->numConnections++];
			a->other = b;
			a->brick = brick;
			b->other = a;
			b->brick = brick2;
			a->type = StudConnection::FRONT;
			b->type = StudConnection::BACK;
			return true;
		};

		//up
		//down
		m_body->queryShape(brickShape, transformTransform(Transform(vec3(x, y + 0.1, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), studCallback, brick);
		m_body->queryShape(brickShape, transformTransform(Transform(vec3(x, y - 0.1, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), studCallback, brick);

		//left
		m_body->queryShape(brickShape, transformTransform(Transform(vec3(x-0.1, y, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), leftBlockCallback, brick);
		//right
		m_body->queryShape(brickShape, transformTransform(Transform(vec3(x + 0.1, y, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), rightBlockCallback, brick);
		//front
		m_body->queryShape(brickShape, transformTransform(Transform(vec3(x, y, z+0.1), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), frontBlockCallback, brick);
		//back
		m_body->queryShape(brickShape, transformTransform(Transform(vec3(x, y, z-0.1), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), backBlockCallback, brick);



		m_body->addCollider(collider);
	}


	bool addImpulse(Brick* brick, vec3 pos, vec3 impulse)
	{
		static const float STUD_STRENGTH = 7.0f;
		bool destroy = false;

		float up = dot(vec3(0, 1, 0), impulse);
		if (brick->block[StudConnection::UP] == 0 && up > 0)
		{
			
			{// pure up
				float studs = 0.0f;
				for (int i = 0; i < brick->numConnections; ++i)
				{
					for (int j = 0; j < brick->connections[i].numStuds; ++j)
					{
						if (brick->connections[i].studs[j].y == 0)
							studs += STUD_STRENGTH;
					}

				}

				if (up > studs)
					destroy = true;
			}
			{// joints
				float studs;

			}


		
			// search for movable bricks
			// get cheapest one
			// reverse build tree to original brick (all connections on which the accumulated force is weak enough)
			
			// end condition???
			//	as soon as force point (com of query or fulcrum point) is behind plane com-pointOfImpact
		}

		float down = dot(vec3(0, -1, 0), impulse);
		if (brick->block[StudConnection::DOWN] == 0 && down > 0)
		{
			float studs = 0.0f;
			for (int i = 0; i < brick->numConnections; ++i)
			{
				for (int j = 0; j < brick->connections[i].numStuds; ++j)
				{
					if (brick->connections[i].studs[j].y == 1)
						studs += STUD_STRENGTH;
				}

				if (down > studs)
					destroy = true;

			}

		}


		if (destroy)
		{
			destroyBrick(brick);
			brick->ship->getBody()->applyImpulse(impulse, pos);
		}
		else
		{
			m_body->applyImpulse(impulse, pos);
		}

		return destroy;
	}

};
ShapePtr Ship::brickShape;



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
		if (brick->connections[i].numStuds > 0)
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
		if (brick->connections[i].numStuds > 0)
		{
			Brick* other = brick->connections[i].other->brick;

			if (other->lastVisited != tick)
			{
				setShip(other, ship, tick);
			}
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
		other->brick->block[brick->connections[i].type]--;

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
	for (int i = 0; i < 6; ++i)
		brick->block[i] = 0;
	Ship* newShip = createShip(brick, brick->ship->getBody()->getTransform(), BodyType::Dynamic);
	brick->ship->getBody()->removeCollider(brick->collider);
	brick->ship = newShip;
	brick->ship->getBody()->addCollider(brick->collider);
}



DestructionTest::DestructionTest()
{
	m_eye.p = vec3(0, 0, 0);
	m_eye.q = Quaternion(vec3(0, 0, 0), 1);
	m_force = 0.0f;
	

}
void DestructionTest::init()
{
	m_world = new World();
	
	Ship::brickShape = ShapePtr();

	g_world = m_world;
	g_entities = &m_entities;
	
	Ship* ship = createShip(0, Transform(vec3(0, 0, 15), Quaternion(vec3(0,0,0),1)), BodyType::Static);
	ship->addBrick(0, 0, 0);
	ship->addBrick(2, 1, 0);
	ship->addBrick(4, 0, 0);
	ship->addBrick(6, 1, 0);

	m_stepping = false;
}


bool DestructionTest::procEvent(SDL_Event event)
{
	if (event.type == SDL_MOUSEWHEEL && ((SDL_GetModState()& KMOD_LCTRL) != 0))
	{
		m_force += event.wheel.y;
		printf("force: %f\n", m_force);
		return true;
	}

	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_click = true;

	}

	return false;
}

void DestructionTest::update(float dt)
{
	int x, y;
	if (m_click && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
	{
		m_force += 30.0f * dt;
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
			if (brick->ship->addImpulse(brick, result.point, m_force * dir))
			{
				m_click = false;
			}
		}
		else
		{
			m_click = false;
		}
	}
	else
	{
		m_force = 0.0f;
	}
}