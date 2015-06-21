#include "DestructionTest.h"
#include <vector>

static World* g_world;
static std::vector<Entity*>* g_entities;
static uint32 g_tick;

class Ship;
struct Brick;
void destroyBrick(Brick* brick);
Ship* createShip(Brick* base, Transform transform, BodyType::Type type);


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
					minX = 0, maxX = 3 + dx;

				int minZ = 0, maxZ = 0;
				if (dz >= 0 && dz < 2)
					minZ = dz, maxZ = 1;
				else
					minZ = 0, maxZ = 1 + dz;

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
							a->type = StudConnection::UP;
							b->type = StudConnection::DOWN;
							brick->block[StudConnection::UP]++;
							brick2->block[StudConnection::DOWN]++;
						}
						else if (dy == -1)
						{
							y = 0;
							a->type = StudConnection::DOWN;
							b->type = StudConnection::UP;
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
			a->type = StudConnection::LEFT;
			b->type = StudConnection::RIGHT;

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
			a->type = StudConnection::RIGHT;
			b->type = StudConnection::LEFT;
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
			a->type = StudConnection::FRONT;
			b->type = StudConnection::BACK;
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
			a->type = StudConnection::BACK;
			b->type = StudConnection::FRONT;
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

		//Transform t = invTransformTransform(invTransform(m_body->getTransform()), brick->collider->getTransform);
		Transform t = invTransform(m_body->getTransform());
		vec3 _pos = transformVec3(pos, t);
		vec3 _impulse = rotate(impulse, t.q);

		std::vector<Brick*> selection;
		std::vector<StudConnection*> front;
		int tick = g_tick++;

		int frontSize = 0;

		selection.push_back(brick);
		brick->lastVisited = tick;

		for (int i = 0; i < brick->numConnections; ++i)
		{
			if (brick->connections[i].numStuds > 0)
			{
				front.push_back(&brick->connections[i]);
				frontSize += brick->connections[i].numStuds;
			}
		}

		

		while (true)
		{
			if (front.size() == 0)
				break;

			// check if selection is past center of mass
			vec3 cm = vec3(0, 0, 0);
			float m = 0.0f;
			for (int i = 0; i < selection.size(); ++i)
			{
				cm += selection[i]->collider->getMassData().m * transformVec3(selection[i]->collider->getMassData().cm, selection[i]->collider->getTransform());
				m += selection[i]->collider->getMassData().m;
			}
			cm = 1.0f / m * cm;

			Plane plane = { normalize(_pos - m_body->getLocalCenter()), dot(normalize(_pos - m_body->getLocalCenter()), m_body->getLocalCenter()) };
			if (distPointFatPlane(cm, plane, FLT_EPSILON) <= 0.0f)
			{
				break;
			}


			// check for destruction
			// pure force
			float studForce = 0.0f;

			for (int i = 0; i < front.size(); ++i)
			{
				if ((_impulse.y > 0 && front[i]->type == StudConnection::UP) ||
					(_impulse.y < 0 && front[i]->type == StudConnection::DOWN))
				{
					studForce = FLT_MAX;
					break;
				}
				else
				{
					for (int j = 0; j < front[i]->numStuds; ++j)
					{
						studForce += STUD_STRENGTH;
					}
				}
			}

			if (abs(_impulse.y) > studForce)
			{
				destroy = true;
				break;
			}

			// expand the selection with the front with the smallest increase in size
			int expansion = -1;
			float minSize = FLT_MAX;
			for (int i = 0; i < front.size(); ++i)
			{
				int newSize = frontSize - front[i]->numStuds;
				Brick* newBrick = front[i]->other->brick;
				for (int j = 0; j < brick->numConnections; ++j)
				{
					if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
					{
						newSize += newBrick->connections[j].numStuds;
					}
				}
				if (newSize < minSize)
				{
					minSize = newSize;
					expansion = i;
				}
			}

			assert(expansion != -1);
			Brick* newBrick = front[expansion]->other->brick;
			front[expansion] = front.back();
			front.pop_back();
			frontSize = minSize;

			assert(newBrick->lastVisited != tick);
			newBrick->lastVisited = tick;
			selection.push_back(newBrick);

			for (int j = 0; j < newBrick->numConnections; ++j)
			{
				if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
				{
					front.push_back(&newBrick->connections[j]);
				}
			}

			// check for outdated connection
			for (int i = 0; i < front.size(); ++i)
			{
				if (front[i]->other->brick->lastVisited == tick)
				{
					front[i] = front.back();
					front.pop_back();
					--i;
				}
			}

		}

		if (destroy)
		{
			//destroyBrick(brick);

			// dissassembly front
			for (int i = 0; i < front.size(); ++i)
			{
				//todo blocking neighbours
				StudConnection* a = front[i];
				StudConnection* b = front[i]->other;
				*a = a->brick->connections[--a->brick->numConnections];
				*b = b->brick->connections[--b->brick->numConnections];
			}

			// seperate selection
			Ship* newShip = createShip(brick, m_body->getTransform(), BodyType::Dynamic);

			// todo new base
			for (int i = 0; i < selection.size(); ++i)
			{
				m_body->removeCollider(selection[i]->collider);
				selection[i]->ship = newShip;
				newShip->m_body->addCollider(selection[i]->collider);
			}


			newShip->getBody()->applyImpulse(impulse, pos);
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
	ship->addBrick(4, 0, 0);
	ship->addBrick(-2, 1, 0);
	ship->addBrick(2, 1, 0);
	ship->addBrick(7, 1, 0);
	ship->addBrick(0, 2, 0);
	ship->addBrick(5, 2, 0);
	ship->addBrick(9, 2, 0);
	ship->addBrick(3, 3, 0);
	ship->addBrick(7, 3, 0);
	ship->addBrick(5, 4, 0);

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
		m_force = 0.0f;
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
	if (m_click && SDL_GetMouseState(&x, &y) & SDL_BUTTON(SDL_BUTTON_LEFT))
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
	RayQueryResult result = { 0 };
	if (m_world->queryRay(m_eye.p, dir, &result))
	{
		Brick* brick = (Brick*)result.collider->getUserData();
		if (brick->ship->addImpulse(brick, result.point, m_force * dir))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return -1;
	}
}