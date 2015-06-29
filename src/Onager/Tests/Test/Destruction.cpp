#include "Destruction.h"


enum
{
	X = 0,
	Z = 1
};


static ShapePtr g_brickShape = {};
static Material g_material = { 1, 1, 1 };
static int g_numJoints;
static JointData g_joints[256];
static int g_numBricks;
static Brick g_bricks[256];
static int g_tick = 0;

void initDestruction(World* world)
{
	ShapeDescription sDescr;
	sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
	sDescr.hullFromBox.c = vec3(0, 0, 0);
	sDescr.hullFromBox.e = vec3(2, 0.5, 1);
	g_brickShape = world->createShape(sDescr);

	g_numJoints = 0;
	g_numBricks = 0;

	memset(g_bricks, 0, sizeof(g_bricks));
	memset(g_joints, 0, sizeof(g_joints));

}

Ship* createShip(World* world, std::vector<Entity*>* entities, BodyDescription descr, vec3 color)
{
	Ship* ship = new Ship(entities, world, world->createBody(descr), color);
	entities->push_back(ship);
	return ship;
}

Ship::Ship(std::vector<Entity*>* entities, World* world, Body* body, vec3 color)
	: Entity(body, color),
	m_world(world),
	m_entities(entities)
{  
}

bool studCallback(Collider* other, void* userData)
{
	Brick* brick = (Brick*)userData;
	Brick* brick2 = (Brick*)other->getUserData();
	
	if (brick == brick2 || brick > brick2)
		return true;

	int dx = abs(brick2->pos[0] - brick->pos[0]);
	int dz = abs(brick2->pos[2] - brick->pos[2]);

	JointData* jointX0 = g_joints + g_numJoints++;
	JointData* jointX1 = g_joints + g_numJoints++;
	JointData* jointZ0 = g_joints + g_numJoints++;
	JointData* jointZ1 = g_joints + g_numJoints++;

	int numStuds = 0;
	vec3 studs[8];

	for (int x = ong_MIN(brick->pos[0], brick2->pos[0]) - 2 + dx; x < ong_MAX(brick->pos[0], brick2->pos[0]) +2 - dx; ++x)
	{
		for (int z = ong_MIN(brick->pos[2], brick2->pos[2]) - 1 + dz; z < ong_MAX(brick->pos[2], brick2->pos[2]) +1 - dz; ++z)
		{
			studs[numStuds++] = { x + 0.5f, (float)ong_MIN(brick->pos[1], brick2->pos[1]) + 0.5f, z + 0.5f };
		}
	}

	jointX0->fulcrum = ong_MIN(brick->pos[0], brick2->pos[0]) - 2 + dx;
	jointX1->fulcrum = ong_MAX(brick->pos[0], brick2->pos[0]) + 2 - dx;
	jointZ0->fulcrum = ong_MIN(brick->pos[2], brick2->pos[2]) - 1 + dz;
	jointZ1->fulcrum = ong_MAX(brick->pos[2], brick2->pos[2]) + 1 - dz;

	jointX0->y = jointX1->y = jointZ0->y = jointZ1->y = (float)ong_MIN(brick->pos[1], brick2->pos[1]) + 0.5f;
	
	for (int i = 0; i < numStuds; ++i)
	{
		jointX0->baseCapacity += (studs[i].x - jointX0->fulcrum) * -STUD_STRENGTH;
		jointX1->baseCapacity += (studs[i].x - jointX1->fulcrum) * -STUD_STRENGTH;
		jointZ0->baseCapacity -= (studs[i].z - jointZ0->fulcrum) * -STUD_STRENGTH;
		jointZ1->baseCapacity -= (studs[i].z - jointZ1->fulcrum) * -STUD_STRENGTH;
	}
	
	Connection* connection = brick->connections + brick->numConnections++;
	Connection* connection2 = brick2->connections + brick2->numConnections++;
	
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			connection->joints[i][j].connection = connection;
			connection2->joints[i][j].connection = connection2;

			connection->joints[i][j].twin = connection2->joints[i] + j;
			connection2->joints[i][j].twin = connection->joints[i] + j;
		}
	}

	connection->joints[X][0].data = connection2->joints[X][0].data = jointX0;
	connection->joints[X][1].data = connection2->joints[X][1].data = jointX1;
	connection->joints[Z][0].data = connection2->joints[Z][0].data = jointZ0;
	connection->joints[Z][1].data = connection2->joints[Z][1].data = jointZ1;

	if (brick->pos[1] > brick2->pos[1])
	{
		connection->dir = 1;
		connection2->dir = -1;
	}
	else
	{
		connection->dir = -1;
		connection2->dir = 1;
	}
	
	connection->brick = brick;
	connection2->brick = brick2;
	
	connection->other = brick2;
	connection2->other = brick;

	return true;
}

bool blockCallback(Collider* other, void* userData)
{
	Brick* brick = (Brick*)userData;
	Brick* brick2 = (Brick*)other->getUserData();

	if (brick == brick2)
		return true;

	for (int i = 0; i < brick->numConnections; ++i)
	{	
		for (int j = 0; j < 2; ++j)
		{
			if ((brick2->pos[1] - 0.5f == brick->connections[i].joints[X][j].data->y ||
				brick2->pos[1] + 0.5f == brick->connections[i].joints[X][j].data->y) &&
				(brick2->pos[0] - 2.0f == brick->connections[i].joints[X][j].data->fulcrum ||
				brick2->pos[0] + 2.0f == brick->connections[i].joints[X][j].data->fulcrum))
			{
				++brick->connections[i].joints[X][!j].data->blocked;
				brick2->blocking[brick2->numBlocking++] = brick->connections[i].joints[X] + !j;
			}
		}
	}
	

	for (int i = 0; i < brick->numConnections; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			if ((brick2->pos[1] - 0.5f == brick->connections[i].joints[X][j].data->y ||
				brick2->pos[1] + 0.5f == brick->connections[i].joints[X][j].data->y) &&
				(brick2->pos[2] - 2.0f == brick->connections[i].joints[Z][j].data->fulcrum ||
				brick2->pos[2] + 2.0f == brick->connections[i].joints[Z][j].data->fulcrum))
			{
				++brick->connections[i].joints[Z][!j].data->blocked;
				brick2->blocking[brick2->numBlocking++] = brick->connections[i].joints[Z] + !j;
			}
		}
	}

	//for (int i = 0; i < brick2->numConnections; ++i)
	//{
	//	for (int j = 0; j < 2; ++j)
	//	{
	//		if (brick->pos[1] - 0.5f == brick2->connections[i].joints[X][j].data->y &&
	//			(brick->pos[0] - 2.0f == brick2->connections[i].joints[X][j].data->fulcrum ||
	//			brick->pos[0] + 2.0f == brick2->connections[i].joints[X][j].data->fulcrum))
	//		{
	//			++brick2->connections[i].joints[X][!j].data->blocked;
	//			brick->blocking[brick->numBlocking++] = brick2->connections[i].joints[X] + !j;
	//		}
	//	}
	//}


	//for (int i = 0; i < brick2->numConnections; ++i)
	//{
	//	for (int j = 0; j < 2; ++j)
	//	{
	//		if (brick->pos[1] - 0.5f == brick2->connections[i].joints[Z][j].data->y &&
	//			(brick->pos[2] - 2.0f == brick2->connections[i].joints[Z][j].data->fulcrum ||
	//			brick->pos[2] + 2.0f == brick2->connections[i].joints[Z][j].data->fulcrum))
	//		{
	//			++brick2->connections[i].joints[Z][!j].data->blocked;
	//			brick->blocking[brick->numBlocking++] = brick2->connections[i].joints[Z] + !j;
	//		}
	//	}
	//}

	return true;
}

void Ship::addBrick(int x, int y, int z)
{
	if (m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x, y, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform())))
	{
		return;
	}

	ColliderDescription cDescr;
	cDescr.isSensor = false;
	cDescr.material = &g_material;
	cDescr.shape = g_brickShape;
	cDescr.transform.p = vec3(x, y, z);
	cDescr.transform.q = Quaternion(vec3(0, 0, 0), 1);

	Collider* collider = m_world->createCollider(cDescr);

	Brick* brick = g_bricks + g_numBricks++;
	brick->pos[0] = x;
	brick->pos[1] = y;
	brick->pos[2] = z;

	brick->collider = collider;
	brick->collider->setUserData(brick);
	brick->ship = this;

	m_body->addCollider(collider);

	
}

void Ship::build()
{
	Collider* c = m_body->getCollider();
	while (c)
	{
		Brick* brick = (Brick*)c->getUserData();

		m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(brick->pos[0], brick->pos[1] + 0.1, brick->pos[2]), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), studCallback, brick);
		m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(brick->pos[0], brick->pos[1] - 0.1, brick->pos[2]), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), studCallback, brick);

		c = c->getNext();
	}


	c = m_body->getCollider();
	while (c)
	{
		Brick* brick = (Brick*)c->getUserData();

		m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(brick->pos[0] - 0.1, brick->pos[1], brick->pos[2]), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);
		m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(brick->pos[0] + 0.1, brick->pos[1], brick->pos[2]), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);
		m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(brick->pos[0], brick->pos[1], brick->pos[2] + 0.1), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);
		m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(brick->pos[0], brick->pos[1], brick->pos[2] - 0.1), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);

		c = c->getNext();
	}



	calcBase();
}

bool findAugmentedPath(Brick* brick, Brick* sink, int axis, float* minCapacity, std::vector<Joint*>* path, int tick)
{
	if (brick == sink)
		return true;

	brick->tick = tick;
	for (int i = 0; i < brick->numConnections; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			Connection* connection = brick->connections + i;
			Joint* joint = brick->connections[i].joints[axis] + j;
			if (connection->other->tick != tick &&
				joint->data->capacity - joint->flow > 0)
			{
				path->push_back(joint);
				if (findAugmentedPath(connection->other, sink, axis, minCapacity, path, tick))
				{
					if (joint->data->capacity - joint->flow < *minCapacity)
						*minCapacity = joint->data->capacity - joint->flow;
					return true;
				}
				else
				{
					path->pop_back();
				}
			}
		}
	}
	return false;
}
float maxFlow(Brick* source, Brick* sink, int axis)
{
	std::vector<Joint*> augmentedPath;
	while (true)
	{
		float minCapacity = FLT_MAX;
		if (!findAugmentedPath(source, sink, axis, &minCapacity, &augmentedPath, g_tick++))
			break;
		for (int i = 0; i < augmentedPath.size(); ++i)
		{
			augmentedPath[i]->flow += minCapacity;
			augmentedPath[i]->twin->flow -= minCapacity;
		}
		augmentedPath.clear();
	}

	float maxFlow = 0;
	for (int i = 0; i < source->numConnections; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			maxFlow += source->connections[i].joints[axis][j].flow;
		}
	}

	return maxFlow;
}



void checkForInvalidBlock(Brick* brick, int selectionTick, int tick)
{
	brick->tick = tick;

	for (int i = 0; i < brick->numBlocking; ++i)
	{
		if (brick->blocking[i]->connection->brick->tick == selectionTick ||
			brick->blocking[i]->connection->other->tick == selectionTick)
		{
			brick->blocking[i]->data->blocked--;
			brick->blocking[i] = brick->blocking[--brick->numBlocking];
		}
	}

	for (int i = 0; i < brick->numConnections; ++i)
	{
		if (brick->connections[i].other->tick != tick)
		{
			checkForInvalidBlock(brick->connections[i].other, selectionTick, tick);
		}
	}

}

bool checkForConnectivity(Brick* brick, Brick* base, int selectionTick, int tick)
{
	brick->tick = tick;
	for (int i = 0; i < brick->numConnections; ++i)
	{
		if (brick->connections[i].other == base)
			return true;

		if (brick->connections[i].other->tick != selectionTick && brick->connections[i].other->tick != tick)
		{
			if (checkForConnectivity(brick->connections[i].other, base, selectionTick, tick))
				return true;
		}
	}

	return false;
}




void Ship::destroy(std::vector<Brick*>* selection, std::vector<Joint*>* front, vec3 impulse, vec3 pos, int axis, int tick)
{

	for (int i = 0; i < selection->size(); ++i)
	{
		selection->at(i)->tick = tick;
	}


	//break front
	for (int i = 0; i < front->size(); ++i)
	{
		Connection* a = (*front)[i]->connection;
		Connection* b = (*front)[i]->twin->connection;
		assert(a->brick->numConnections > 0 && b->brick->numConnections > 0);
		*a = a->brick->connections[--a->brick->numConnections];
		
		for (int d = X; d <= Z; ++d)
		{
			for (int i = 0; i < 2; ++i)
			{
				a->joints[d][i].connection = a;
				a->joints[d][i].twin->twin = a->joints[d] + i;
			}
		}


		*b = b->brick->connections[--b->brick->numConnections];

		for (int d = X; d <= Z; ++d)
		{
			for (int i = 0; i < 2; ++i)
			{
				b->joints[d][i].connection = b;
				b->joints[d][i].twin->twin = b->joints[d] + i;
			}
		}

		//if (checkForConnectivity(a->other, m_base, tick, g_tick++))
		//{
		//	*a = a->brick->connections[--a->brick->numConnections];
		//	*b = b->brick->connections[--b->brick->numConnections];
		//}
		//else
		//{
		//	a->other->tick = tick;
		//	selection->push_back(a->other);
		//}


	}

	BodyDescription descr;
	descr.transform = m_body->getTransform();
	descr.type = BodyType::Dynamic;
	descr.linearMomentum = impulse;
	descr.angularMomentum = vec3(0, 0, 0);

	Ship* newShip = createShip(m_world, m_entities, descr, vec3(0,0,1));
	

	

	// seperate selection
	for (int i = 0; i < selection->size(); ++i)
	{
		Brick* brick = selection->at(i);
		// free blocks
		for (int i = 0; i < brick->numBlocking; ++i)
		{
			if (brick->blocking[i]->connection->brick->tick != tick)
			{
				brick->blocking[i]->data->blocked--;
				brick->blocking[i] = brick->blocking[--brick->numBlocking];
			}
		}
		m_body->removeCollider(brick->collider);
		brick->ship = newShip;
		newShip->m_body->addCollider(brick->collider);
	}
	
	newShip->calcBase();
	calcBase();

	checkForInvalidBlock(m_base, tick , g_tick++);
}

bool breakGraph(Brick* brick,Brick* base, float maxFlow, int axis, std::vector<Brick*>* selection, std::vector<Joint*>* front, int tick)
{
	if (brick == base)
		return true;

	brick->tick = tick;
	selection->push_back(brick);
	for (int i = 0; i < brick->numConnections; ++i)
	{
		Connection* connection = brick->connections + i;
		if (connection->other->tick != tick)
		{
			for (int j = 0; j < 2; ++j)
			{
				if (connection->joints[axis][j].data->capacity > 0)
				{
					if (connection->joints[axis][j].data->capacity >= connection->joints[axis][j].flow / maxFlow)
					{
						if (breakGraph(connection->other, base, maxFlow, axis, selection, front, tick))
							return true;

					}
					else
					{
						assert(connection->joints[axis][j].data != nullptr);
						front->push_back(connection->joints[axis] + j);
					}
				}
			}
		}
	}

	return false;
}


void clearFlowNetwork(Brick* brick, vec3 impulse, vec3 pos, int tick)
{
	brick->tick = tick;
	for (int i = 0; i < brick->numConnections; ++i)
	{
		//if (brick->connections[i].other->tick == tick)
		//	continue;

		for (int d = X; d <= Z; ++d)
		{
			for (int j = 0; j < 2; ++j)
			{
				Joint* joint = brick->connections[i].joints[d]+ j;
				joint->flow = 0;


				if (d== X)
					joint->data->capacity =
					brick->connections[i].dir * joint->data->baseCapacity / ((pos.x - joint->data->fulcrum) * impulse.y - (pos.y - joint->data->y) *impulse.x);
				else if (d == Z)
					joint->data->capacity =
					brick->connections[i].dir * joint->data->baseCapacity / ((pos.y - joint->data->y) * impulse.z - (pos.z - joint->data->fulcrum) * impulse.y);

				if (joint->data->blocked && joint->data->capacity > 0)
				{
					joint->data->capacity = 1.0f;
				}
				else
				{
					joint->data->capacity = ong_MIN(1.0f, ong_MAX(0.0f, joint->data->capacity));
				}
			}
		}
		if (brick->connections[i].other->tick != tick)
		{
			clearFlowNetwork(brick->connections[i].other, impulse, pos, tick);
		}
	}
}


bool Ship::checkAxis(Brick* brick, vec3 impulse, vec3 pos, int axis)
{
	float max = maxFlow(brick, m_base, axis);

	if (max > 0 && max < 1.0f)
	{
		std::vector<Brick*> selection;
		std::vector<Joint*> front;

		int selectionTick = g_tick++;
		breakGraph(brick, m_base, max, axis, &selection, &front, selectionTick);

		for (int i = 0; i < front.size(); ++i)
		{
			Brick* brick = front[i]->connection->brick;
			int numUp = 0;
			int numDown = 0;
			for (int j = 0; j < brick->numConnections; ++j)
			{
				Brick* other = brick->connections[j].other;
				if (other->tick != selectionTick)
				{
					if (other->pos[1] > brick->pos[1])
						numUp++;
					else if (other->pos[1] < brick->pos[1])
						numDown++;
				}
				if (numUp > 0 && numDown > 0)
					return false;

			}

		}


		destroy(&selection, &front, impulse, pos, axis, g_tick++);
		return true;
	}
	return false;
}

bool Ship::addImpulse(Brick* brick, vec3 pos, vec3 impulse)
{
	if (brick == m_base)
		return false;

	Transform t = invTransform(m_body->getTransform());
	vec3 _pos = transformVec3(pos, t);
	vec3 _impulse = rotate(impulse, t.q);

	// clear flow network
	clearFlowNetwork(brick, _impulse, _pos, g_tick++);


	bool destroy = checkAxis(brick, impulse, pos, X);

	if (!destroy)
		destroy = checkAxis(brick, impulse, pos, Z);


	return destroy;
}




void Ship::calcBase()
{
	Collider* c = m_body->getCollider();
	float min = FLT_MAX;
	while (c)
	{
		if (lengthSq(c->getTransform().p - m_body->getLocalCenter()) < min)
		{
			min = lengthSq(c->getTransform().p - m_body->getLocalCenter());
			m_base = (Brick*)c->getUserData();
		}

		c = c->getNext();
	}

}

void renderBrick(Brick* brick, Brick* base, GLuint colorLocation, int tick)
{
	brick->tick = tick;

	for (int j = 0; j < brick->numConnections; ++j)
	{
		for (int axis = X; axis < Z; ++axis)
		{
			for (int i = 0; i < 2; ++i)
			{
				Connection* connection = brick->connections + j;
				Joint* joint = connection->joints[axis] + i;

				if (joint->flow != 0)
				{
					glLineWidth(10.0f * joint->flow);
					glBegin(GL_LINES);
					glVertex3f(brick->pos[0], brick->pos[1], brick->pos[2]);
					glVertex3f(connection->other->pos[0], connection->other->pos[1], connection->other->pos[2]);
					glEnd();
					glLineWidth(1);
				}


				//glLineWidth(connection->joint->capacity * 10.0f);
				//glBegin(GL_LINES);
				//glVertex3f(0.5f * (brick->pos[0] + connection->other->pos[0]), 0.5f*(brick->pos[1] + connection->other->pos[1]), 0.5f * (brick->pos[2] + connection->other->pos[2]));
				//glVertex3f(0.5f * (brick->pos[0] + connection->other->pos[0]), brick->pos[1] , 0.5f * (brick->pos[2] + connection->other->pos[2]));
				//glEnd();
				//glLineWidth(1);

				for (int i = 0; i < brick->numBlocking; ++i)
				{
					glBegin(GL_LINES);
					glVertex3f(brick->pos[0], brick->pos[1], brick->pos[2]);
					glVertex3f(brick->blocking[i]->connection->brick->pos[0], brick->blocking[i]->connection->brick->pos[1], brick->blocking[i]->connection->brick->pos[2]);
					glEnd();
				}

				if (joint->data->capacity > 0)
				{
					glLineWidth(joint->data->capacity * 5.0f);
					glBegin(GL_LINES);
					if (axis == X)
					{
						int min = ong_MIN(brick->pos[2], joint->connection->other->pos[2]) - 1 + abs(brick->pos[2] - joint->connection->other->pos[2]);
						int max = ong_MAX(brick->pos[2], joint->connection->other->pos[2]) + 1 - abs(brick->pos[2] - joint->connection->other->pos[2]);

						glVertex3f(joint->data->fulcrum, joint->data->y, min);
						glVertex3f(joint->data->fulcrum, joint->data->y, max);
					}
					else if (axis == Z)
					{
						int min = ong_MIN(brick->pos[0], joint->connection->other->pos[0]) - 2 + abs(brick->pos[0] - joint->connection->other->pos[0]);
						int max = ong_MAX(brick->pos[0], joint->connection->other->pos[0]) + 2 - abs(brick->pos[0] - joint->connection->other->pos[0]);

						glVertex3f(min, joint->data->y, joint->data->fulcrum);
						glVertex3f(max, joint->data->y, joint->data->fulcrum);
					}
					glEnd();
					glLineWidth(1);
				}

				if (connection->other->tick != tick)
					renderBrick(connection->other, base, colorLocation, tick);
			}
		}
	}

	if (brick == base)
	{
		glUniform3f(colorLocation, 1, 0, 1);
		glPointSize(10.0f);
		glBegin(GL_POINTS);
		glVertex3f(brick->pos[0], brick->pos[1], brick->pos[2]);
		glEnd();
		glPointSize(1.0f);
		glUniform3f(colorLocation, 0, 1, 1);
	}

}


void Ship::render(GLuint colorLocation)
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
	renderBrick(m_base, m_base, colorLocation, g_tick++);


	glPopMatrix();
}