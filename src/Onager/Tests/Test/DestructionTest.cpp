#include "DestructionTest.h"
#include <vector>

static World* g_world;
static std::vector<Entity*>* g_entities;
static uint32 g_tick;

class Ship;
struct Brick;
void destroyBrick(Brick* brick);
Ship* createShip(Brick* base, Transform transform, BodyType::Type type);
void setShip(Brick* brick, Ship* ship, int tick);
bool connected(Brick* brick, Brick* base, int tick);


vec3 g_fulcrum = vec3(0,0,0);

struct Stud
{
	float x;
	float z;
	float y;
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
	float fulcrumX;
	float fulcrumZ;
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

			StudConnection* a = &brick->connections[brick->numConnections++];
			StudConnection* b = &brick2->connections[brick2->numConnections++];
		
			int dy = brick2->pos[1] - brick->pos[1];
		
			if (dy == 1 || dy == -1)
			{
				int dx = brick2->pos[0] - brick->pos[0];
				int dz = brick2->pos[2] - brick->pos[2];

				int minX = 0, maxX = 0;
				if (dx >= 0 && dx < 4)
				{
					minX = dx, maxX = 3;
					a->fulcrumX = brick->pos[0] + maxX - 1.0f;
					b->fulcrumX = brick2->pos[0] + minX - dx - 2.0;
				}
				else
				{
					minX = 0, maxX = 3 + dx;
					a->fulcrumX = brick->pos[0] + minX - 2.0f;
					b->fulcrumX = brick2->pos[0] + maxX - dx - 1.0f;
				}

				int minZ = 0, maxZ = 0;
				if (dz >= 0 && dz < 2)
				{
					minZ = dz, maxZ = 1;
					a->fulcrumZ = brick->pos[2] + maxZ - 0.0f;
					b->fulcrumZ = brick2->pos[2] + minZ - dz - 1.0f;
				}
				else
				{
					minZ = 0, maxZ = 1 + dz;
					a->fulcrumZ = brick->pos[2] + minZ - 1.0f;
					b->fulcrumZ = brick2->pos[2] + maxZ - dz - 0.0f;
				}

				a->brick = brick;
				a->other = b;
				b->brick = brick2;
				b->other = a;
				
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

				for (int x = minX; x <= maxX; ++x)
				{
					for (int z = minZ; z <= maxZ; ++z)
					{


						int i = a->numStuds++;
						int j = b->numStuds++;

						a->studs[i] = { brick->pos[0] + x - 1.5, brick->pos[2] + z - 0.5, brick->pos[1] + y  - 0.5};
						b->studs[j] = { brick2->pos[0] + x - dx - 1.5,brick2->pos[2] + z - dz - 0.5, brick2->pos[1] +  1-y - 0.5};

						//todo fulcrum


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


	bool checkFrontForConnection(Brick* brick, Brick* base, int tick, int selectionTick)
	{
		if (brick == base)
			return true;

		brick->lastVisited = tick;

		for (int i = 0; i < brick->numConnections; ++i)
		{
			if (brick->connections[i].numStuds > 0 && brick->connections[i].other->brick->lastVisited != tick &&
				brick->connections[i].other->brick->lastVisited != selectionTick)
			{
				if (checkFrontForConnection(brick->connections[i].other->brick, base, tick, selectionTick))
				{
					return true;
				}
			}
		}

		return false;
	};


	bool checkLeverage(vec3 _impulse, vec3 _pos, Brick* brick,  int axis)
	{
		std::vector<Brick*> selection;
		std::vector<StudConnection*> front;

		int tick = g_tick++;
		Plane cmPlane = { normalize(_pos - m_body->getLocalCenter()), dot(normalize(_pos - m_body->getLocalCenter()), m_body->getLocalCenter()) };

		bool destroy = false;

		while (true)
		{
			int expansion = -2;
			float minTorque = -FLT_MAX;
			for (int f = (front.size() > 0 ? 0 : -1); f < front.size(); ++f)
			{
				Brick* newBrick = front[f]->other->brick;
				if (f == -1)
					newBrick = brick;


				if (distPointFatPlane(transformVec3(newBrick->collider->getMassData().cm, newBrick->collider->getTransform()), cmPlane, FLT_EPSILON) <= 0.0f)
					continue;


				int tick = g_tick++;

				for (int i = 0; i < selection.size(); ++i)
				{
					selection[i]->lastVisited = tick;
				}

				for (int j = 0; j < newBrick->numConnections; ++j)
				{
					if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
						front.push_back(newBrick->connections + j);
				}


				Brick* base = m_base;
				if (newBrick == m_base)
				{
					for (int i = 0; i < newBrick->numConnections; ++i)
					{
						if (newBrick->connections[i].numStuds > 0 && newBrick->connections[i].other->brick->lastVisited != tick)
						{
							base = newBrick->connections[i].other->brick;
							break;
						}
					}
				}




				for (int i = 0; i < front.size(); ++i)
				{
					if (!checkFrontForConnection(front[i]->other->brick, base, g_tick++, tick))
					{
						front[i]->other->brick->lastVisited = tick;
					}
				}


				for (int i = 0; i < front.size(); ++i)
				{

					float studTorque = 0;

					Stud* stud = nullptr;
					float fulcrum;
					bool blocked = false;
					
					if ((_impulse.y < 0 ? StudConnection::DOWN : StudConnection::UP) != front[i]->type)
					{
						fulcrum = front[i]->fulcrumX;

						if ((fulcrum > _pos.x && front[i]->brick->block[StudConnection::RIGHT])
							|| (fulcrum < _pos.x && front[i]->brick->block[StudConnection::LEFT]))
						{
							blocked = true;
						}
					}
					else
					{
						fulcrum = front[i]->other->fulcrumX;
						if ((fulcrum > _pos.x && front[i]->other->brick->block[StudConnection::LEFT]) ||
							(fulcrum < _pos.x && front[i]->other->brick->block[StudConnection::RIGHT]))
						{
							blocked = true;
						}
					}


					// check torques around frustrums
					float y = front[i]->studs[0].y;
					float torque = cross(vec3(_pos.x - fulcrum, _pos.y - y, 0), _impulse).z;

					float dx = torque > 0 ? 1 : -1;

					if (blocked)
						studTorque += -dx * 100.0f * STUD_STRENGTH;


					for (int j = 0; j < front.size(); ++j)
					{
						// front which we currently try to expand
						if (j == f || front[j]->other->brick == newBrick || front[j]->other->brick->lastVisited == tick)
							continue;

						if (front[j]->type == StudConnection::UP)
						{
							blocked = true;
							break;
						}

						for (int k = 0; k < front[j]->numStuds; ++k)
						{
							Stud* stud = front[j]->studs + k;
							float tX = cross(vec3(stud->x - fulcrum, stud->y - y, 0), vec3(0, -STUD_STRENGTH, 0)).z;
							// if torque stud and torque impact point in the same direction not valid fulcrum
							if (torque* tX > 0)
							{
								blocked = true;
								studTorque += -dx * 100.0f * STUD_STRENGTH;
								break;
							}
							else
							{
								studTorque += tX;
							}
						}
					}


					float torqueDiff = dx*(torque + studTorque);
					if (torqueDiff > 0 && !blocked)
					{
						destroy = true;
						break;
					}

					if (torqueDiff > minTorque)
					{
						expansion = f;
						minTorque = torqueDiff;
					}
				}

				for (int j = 0; j < newBrick->numConnections; ++j)
				{
					if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
						front.pop_back();
				}

				if (destroy)
					break;
			}

			if (expansion == -2)
				return false;

			Brick* newBrick;
			if (expansion == -1)
			{
				newBrick = brick;
			}
			else
			{
				newBrick = front[expansion]->other->brick;
				front[expansion] = front.back();
				front.pop_back();
			}



			if (newBrick == m_base)
			{
				for (int i = 0; i < newBrick->numConnections; ++i)
				{
					if (newBrick->connections[i].numStuds > 0 && newBrick->connections[i].other->brick->lastVisited != tick)
					{
						m_base = newBrick->connections[i].other->brick;
						break;
					}
				}
			}

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

			for (int i = 0; i < front.size(); ++i)
			{

				if (!checkFrontForConnection(front[i]->other->brick, m_base, g_tick++, tick))
				{
					front[i]->other->brick->lastVisited = tick;
					selection.push_back(front[i]->other->brick);

					front[i] = front.back();
					front.pop_back();
					--i;
				}

			}

			for (int i = 0; i < front.size(); ++i)
			{

				if (front[i]->other->brick->lastVisited == tick)
				{
					front[i] = front.back();
					front.pop_back();
					--i;
				}
			}

			if (destroy)
				return true;

		}
	}

	static const float STUD_STRENGTH = 20.0f;
	bool addImpulse(Brick* brick, vec3 pos, vec3 impulse)
	{
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

		Plane cmPlane = { normalize(_pos - m_body->getLocalCenter()), dot(normalize(_pos - m_body->getLocalCenter()), m_body->getLocalCenter()) };
		// check for destruction
		




		// check for leverage x
		while (true)
		{
			int expansion = -1;
			float minTorque = -FLT_MAX;
			for (int f = 0; f < front.size(); ++f)
			{

				Brick* newBrick = front[f]->other->brick;
				if (distPointFatPlane(transformVec3(newBrick->collider->getMassData().cm, newBrick->collider->getTransform()), cmPlane, FLT_EPSILON) <= 0.0f)
					continue;


				for (int j = 0; j < newBrick->numConnections; ++j)
				{
					if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
						front.push_back(newBrick->connections + j);
				}



				for (int i = 0; i < front.size(); ++i)
				{
					float studTorque = 0;

					Stud* stud = nullptr;
					float fulcrum;
					bool blocked = false;
					if ((impulse.y < 0 ? StudConnection::DOWN : StudConnection::UP) != front[i]->type)
					{
						fulcrum = front[i]->fulcrumX;

						if ((fulcrum > _pos.x && front[i]->brick->block[StudConnection::RIGHT])
							|| (fulcrum < _pos.x && front[i]->brick->block[StudConnection::LEFT]))
						{
							blocked = true;
						}
					}
					else
					{
						fulcrum = front[i]->other->fulcrumX;
						if ((fulcrum > _pos.x && front[i]->other->brick->block[StudConnection::RIGHT]) ||
							(fulcrum < _pos.x && front[i]->other->brick->block[StudConnection::LEFT]))
						{
							blocked = true;
						}
					}


					// check torques around frustrums
					float y = front[i]->studs[0].y;
					float torque = cross(vec3(_pos.x - fulcrum, _pos.y - y, 0), _impulse).z;

					float dx = torque > 0 ? 1 : -1;

					if (blocked)
						studTorque += -dx * 100.0f * STUD_STRENGTH;


					for (int j = 0; j < front.size(); ++j)
					{
						// front which we currently try to expand
						if (j == f || front[j]->other->brick == newBrick)
							continue;

						

						if (front[j]->type == StudConnection::UP)
						{
							blocked = true;
							break;
						}

						for (int k = 0; k < front[j]->numStuds; ++k)
						{
							Stud* stud = front[j]->studs + k;
							float tX = cross(vec3(stud->x - fulcrum, stud->y - y, 0), vec3(0, -STUD_STRENGTH, 0)).z;
							// if torque stud and torque impact point in the same direction not valid fulcrum
							if (torque* tX > 0)
							{
								blocked = true;
								studTorque += -dx * 100.0f * STUD_STRENGTH;
								break;
							}
							else
							{
								studTorque += tX;
							}
						}
					}

	
					float torqueDiff = dx*(torque + studTorque);
					if (torqueDiff > 0 && !blocked)
					{
						destroy = true;
						break;
					}

					if (torqueDiff > minTorque)
					{
						expansion = f;
						minTorque = torqueDiff;
					}

				}



				for (int j = 0; j < newBrick->numConnections; ++j)
				{
					if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
						front.pop_back();
				}

				if (destroy)
					break;

			}

			if (destroy)
				break;

			if (expansion == -1)
				break;
			
			Brick* newBrick = front[expansion]->other->brick;
			front[expansion] = front.back();
			front.pop_back();

			if (newBrick == m_base)
			{
				for (int i = 0; i < newBrick->numConnections; ++i)
				{
					if (newBrick->connections[i].numStuds > 0 && newBrick->connections[i].other->brick->lastVisited != tick)
					{
						m_base = newBrick->connections[i].other->brick;
					}
				}
			}

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
			// see if rest of front still connected to base
			for (int i = 0; i < front.size(); ++i)
			{
				if (front[i]->other->brick->lastVisited == tick)
				{
					front[i] = front.back();
					front.pop_back();
					--i;
				}
				else if (!checkFrontForConnection(front[i]->other->brick, m_base, g_tick++, tick))
				{
					front[i]->other->brick->lastVisited = tick;
					selection.push_back(front[i]->other->brick);

					front[i] = front.back();
					front.pop_back();
					--i;
				}

			}
			


			// destroy?
			{

				for (int i = 0; i < front.size(); ++i)
				{

					float studTorque = 0;
					float fulcrum;
					bool blocked = false;
					if ((impulse.y < 0 ? StudConnection::DOWN : StudConnection::UP) != front[i]->type)
					{
						fulcrum = front[i]->fulcrumX;

						if ((fulcrum > _pos.x && front[i]->brick->block[StudConnection::RIGHT])
							|| (fulcrum < _pos.x && front[i]->brick->block[StudConnection::LEFT]))
						{
							blocked = true;
						}
					}
					else
					{
						fulcrum = front[i]->other->fulcrumX;
						if ((fulcrum > _pos.x && front[i]->other->brick->block[StudConnection::RIGHT]) ||
							(fulcrum < _pos.x && front[i]->other->brick->block[StudConnection::LEFT]))
						{
							blocked = true;
						}
					}


					// check torques around frustrums
					float y = front[i]->studs[0].y;
					float torque = cross(vec3(_pos.x - fulcrum, _pos.y - y, 0), _impulse).z;

					float dx = torque > 0 ? 1 : -1;

					if (blocked)
						studTorque += -dx * 100.0f * STUD_STRENGTH;


					for (int j = 0; j < front.size(); ++j)
					{
						if (front[j]->type == StudConnection::UP)
						{
							blocked = true;
							break;
						}

						for (int k = 0; k < front[j]->numStuds; ++k)
						{
							Stud* stud = front[j]->studs + k;
							float tX = cross(vec3(stud->x - fulcrum, stud->y - y, 0), vec3(0, -STUD_STRENGTH, 0)).z;
							// if torque stud and torque impact point in the same direction not valid fulcrum
							if (torque* tX > 0)
							{
								blocked = true;
								studTorque += -dx * 100.0f * STUD_STRENGTH;
								break;
							}
							else
							{
								studTorque += tX;
							}
						}
					}


					float torqueDiff = dx*(torque + studTorque);
					if (torqueDiff > 0 && !blocked)
					{
						destroy = true;
						break;
					}

				}
			}

			if (destroy)
				break;
		}


		//while (true)
		//{
		//	if (front.size() == 0)
		//		break;

		//
		//	{
		//		// pure force
		//		float studForce = 0.0f;

		//		for (int i = 0; i < front.size(); ++i)
		//		{
		//			if ((_impulse.y > 0 && front[i]->type == StudConnection::UP) ||
		//				(_impulse.y < 0 && front[i]->type == StudConnection::DOWN))
		//			{
		//				studForce = FLT_MAX;
		//				break;
		//			}
		//			else
		//			{
		//				for (int j = 0; j < front[i]->numStuds; ++j)
		//				{
		//					studForce += STUD_STRENGTH;
		//				}
		//			}
		//		}

		//		if (abs(_impulse.y) > studForce)
		//		{
		//			destroy = true;
		//			break;
		//		}

		//		// check for leverage z
		//		for (int i = 0; i < front.size(); ++i)
		//		{
		//			for (int d = -1; d <= 1; d += 2)
		//			{

		//				Stud* stud = nullptr;
		//				float max = -FLT_MAX;
		//				for (int j = 0; j < front[i]->numStuds; ++j)
		//				{
		//					if (d * front[i]->studs[j].z > max)
		//						stud = front[i]->studs + j, max = d*front[i]->studs[j].z;
		//				}

		//				// check if frustrum is blocked
		//				if (d == 1)
		//				{
		//					if (front[i]->other->brick->pos[2] > front[i]->brick->pos[2])
		//					{
		//						if (front[i]->brick->block[StudConnection::BACK] > 0)
		//							continue;
		//					}
		//					else
		//					{
		//						if (front[i]->other->brick->block[StudConnection::BACK] > 0)
		//							continue;
		//					}
		//				}
		//				else if (d == -1)
		//				{
		//					if (front[i]->other->brick->pos[0] < front[i]->brick->pos[0])
		//					{
		//						if (front[i]->brick->block[StudConnection::FRONT] > 0)
		//							continue;
		//					}
		//					else
		//					{
		//						if (front[i]->other->brick->block[StudConnection::FRONT] > 0)
		//							continue;
		//					}
		//				}


		//				// check torques around frustrums
		//				float y = front[i]->studs[0].y;
		//				float fulcrum = stud->y + (d*0.5f);
		//				float torque = cross(vec3(0, _pos.y - y, _pos.z - fulcrum), _impulse).x;
		//				float studTorque = 0;

		//				float dz = torque > 0 ? 1 : -1;

		//				for (int j = 0; j < front.size(); ++j)
		//				{
		//					if (front[j]->type == StudConnection::UP)
		//					{
		//						studTorque = -dz * FLT_MAX;
		//						break;
		//					}

		//					for (int k = 0; k < front[j]->numStuds; ++k)
		//					{
		//						Stud* stud = front[j]->studs + k;
		//						float tz = cross(vec3(0, stud->y - y, stud->z - fulcrum), vec3(0, -STUD_STRENGTH, 0)).x;
		//						// if torque stud and torque impact point in the same direction not valid fulcrum
		//						if (torque* tz > 0)
		//						{
		//							studTorque = -dz * FLT_MAX;
		//							break;
		//						}
		//						else
		//						{
		//							studTorque += tz;
		//						}
		//					}
		//				}

		//				if (dz * (torque + studTorque) > 0)
		//				{
		//					printf("torque: %f\n", studTorque);
		//					destroy = true;
		//					break;
		//				}
		//			}
		//		}		
		//		
		//		if (destroy)
		//			break;

		//	}
		//	// expand the selection with the front with the smallest increase in size
		//	int expansion = -1;
		//	float minSize = FLT_MAX;
		//	for (int i = 0; i < front.size(); ++i)
		//	{
		//		int newSize = frontSize - front[i]->numStuds;
		//		Brick* newBrick = front[i]->other->brick;
		//		// check if new brick is past center of mass
		//		if (distPointFatPlane(transformVec3(newBrick->collider->getMassData().cm, newBrick->collider->getTransform()), cmPlane, FLT_EPSILON) <= 0.0f)
		//			continue;
		//		for (int j = 0; j < brick->numConnections; ++j)
		//		{
		//			if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
		//			{
		//				newSize += newBrick->connections[j].numStuds;
		//			}
		//		}
		//		if (newSize < minSize)
		//		{
		//			minSize = newSize;
		//			expansion = i;
		//		}
		//	}

		//	if (expansion == -1)
		//		break;
		//	
		//	Brick* newBrick = front[expansion]->other->brick;
		//	front[expansion] = front.back();
		//	front.pop_back();
		//	frontSize = minSize;

		//	assert(newBrick->lastVisited != tick);
		//	newBrick->lastVisited = tick;
		//	selection.push_back(newBrick);

		//	for (int j = 0; j < newBrick->numConnections; ++j)
		//	{
		//		if (newBrick->connections[j].numStuds > 0 && newBrick->connections[j].other->brick->lastVisited != tick)
		//		{
		//			front.push_back(&newBrick->connections[j]);
		//		}
		//	}

		//	// check for outdated connection
		//	for (int i = 0; i < front.size(); ++i)
		//	{
		//		if (front[i]->other->brick->lastVisited == tick)
		//		{
		//			front[i] = front.back();
		//			front.pop_back();
		//			--i;
		//		}
		//	}

		//}

		if (destroy)
		{
			//destroyBrick(brick);


			if (m_base->lastVisited == tick)
			{
				assert(front.size() > 0);
				m_base = front[0]->other->brick;
			}

			// dissassembly front
			for (int i = 0; i < front.size(); ++i)
			{
				//todo blocking neighbours
				StudConnection* a = front[i];
				StudConnection* b = front[i]->other;
				*a = a->brick->connections[--a->brick->numConnections];
				*b = b->brick->connections[--b->brick->numConnections];

				if (!connected(b->brick, m_base, g_tick++))
				{
					Ship* ship = createShip(b->brick, brick->ship->getBody()->getTransform(), BodyType::Dynamic);
					setShip(b->brick, ship, g_tick++);
				}
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

	void renderBrick(Brick* brick, int tick)
	{
		brick->lastVisited = tick;
		for (int i = 0; i < brick->numConnections; ++i)
		{
			for (int j = 0; j < brick->connections[i].numStuds; ++j)
			{

				Stud* stud = &brick->connections[i].studs[j];
				glBegin(GL_LINES);

				glVertex3f(stud->x, stud->y, stud->z);
				if (brick->connections[i].type == StudConnection::UP)
					glVertex3f(stud->x, stud->y + 0.1f, stud->z);
				else if (brick->connections[i].type == StudConnection::DOWN)
					glVertex3f(stud->x, stud->y - 0.1f, stud->z);

				glEnd();



			}

			glBegin(GL_LINES);
			glVertex3f(brick->connections[i].fulcrumX, brick->connections[i].studs[0].y, brick->connections[i].studs[0].z - 0.5f);
			glVertex3f(brick->connections[i].fulcrumX, brick->connections[i].studs[0].y, brick->connections[i].studs[0].z + 0.5f);
			glEnd();

			if (brick->connections[i].other->brick->lastVisited != tick)
				renderBrick(brick->connections[i].other->brick, tick);
		}
	}


	void render(GLuint colorLocation) override
	{
		Entity::render(colorLocation);

		glPushMatrix();
		vec3 p = m_body->getPosition();
		glTranslatef(p.x, p.y, p.z);

		mat3x3 q = transpose(toRotMat(m_body->getOrientation()));

		float rot[16] =
		{
			q[0][0], q[0][1], q[0][2], 0.0f,
			q[1][0], q[1][1], q[1][2], 0.0f,
			q[2][0], q[2][1], q[2][2], 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f,
		};

		glMultMatrixf(rot);

		glUniform3f(colorLocation, 0, 1, 1);
		renderBrick(m_base, g_tick++);


		glPopMatrix();
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
	if (m_world->queryRay(m_eye.p, dir, &result) && result.collider != 0)
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

void DestructionTest::render()
{
	glUniform3f(m_colorLocation, 1, 1, 1);
	glPushMatrix();
	glTranslatef(0, 0, 15);
	glBegin(GL_POINTS);
	glVertex3f(g_fulcrum.x, g_fulcrum.y, g_fulcrum.z);
	glEnd();
	glPopMatrix();
}