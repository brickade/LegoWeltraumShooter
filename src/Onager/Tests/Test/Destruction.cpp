#include "Destruction.h"


enum
{
	X = 0,
	Z = 1
};


static ShapePtr g_brickShape = {};
static Material g_material = { 1, 1, 1 };
static int g_numJoints;
static Joint g_joints[256];
static Brick g_bricks[256];
static int g_tick = 0;

Ship* createShip(World* world, std::vector<Entity*>* entities, BodyDescription descr)
{
	Ship* ship = new Ship(entities, world, world->createBody(descr), vec3(1, 0, 0));
	entities->push_back(ship);
	return ship;
}

Ship::Ship(std::vector<Entity*>* entities, World* world, Body* body, vec3 color)
	: Entity(body, color),
	m_world(world),
	m_entities(entities)
{  
	if (!g_brickShape)
	{
		ShapeDescription sDescr;
		sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
		sDescr.hullFromBox.c = vec3(0, 0, 0);
		sDescr.hullFromBox.e = vec3(2, 0.5, 1);
		g_brickShape = m_world->createShape(sDescr);
	}
}

bool studCallback(Collider* other, void* userData)
{
	Brick* brick = (Brick*)userData;
	Brick* brick2 = (Brick*)other->getUserData();

	int dx = abs(brick2->pos[0] - brick->pos[0]);
	int dz = abs(brick2->pos[2] - brick->pos[2]);

	Joint* jointX0 = g_joints + g_numJoints++;
	Joint* jointX1 = g_joints + g_numJoints++;
	Joint* jointZ0 = g_joints + g_numJoints++;
	Joint* jointZ1 = g_joints + g_numJoints++;

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
	jointZ0->fulcrum = ong_MIN(brick->pos[2], brick2->pos[2]) - 2 + dz;
	jointZ1->fulcrum = ong_MAX(brick->pos[2], brick2->pos[2]) + 2 - dz;

	jointX0->y = jointX1->y = jointZ0->y = jointZ1->y = (float)ong_MIN(brick->pos[1], brick2->pos[1]) + 0.5f;
	
	for (int i = 0; i < numStuds; ++i)
	{
		jointX0->baseCapacity += (studs[i].x - jointX0->fulcrum) * -STUD_STRENGTH;
		jointX1->baseCapacity += (studs[i].x - jointX1->fulcrum) * -STUD_STRENGTH;
		jointZ0->baseCapacity -= (studs[i].z - jointZ0->fulcrum) * -STUD_STRENGTH;
		jointZ1->baseCapacity -= (studs[i].z - jointZ1->fulcrum) * -STUD_STRENGTH;
	}
	
	Connection* connectionX0 = brick->connections[X] + brick->numConnections[X]++;
	Connection* connectionX1 = brick->connections[X] + brick->numConnections[X]++;
	Connection* connectionZ0 = brick->connections[Z] + brick->numConnections[Z]++;
	Connection* connectionZ1 = brick->connections[Z] + brick->numConnections[Z]++;

	Connection* connection2X0 = brick2->connections[X] + brick2->numConnections[X]++;
	Connection* connection2X1 = brick2->connections[X] + brick2->numConnections[X]++;
	Connection* connection2Z0 = brick2->connections[Z] + brick2->numConnections[Z]++;
	Connection* connection2Z1 = brick2->connections[Z] + brick2->numConnections[Z]++;

	connectionX0->joint = connection2X0->joint = jointX0;
	connectionX1->joint = connection2X1->joint = jointX1;
	connectionZ0->joint = connection2Z0->joint = jointZ0;
	connectionZ1->joint = connection2Z1->joint = jointZ1;
	
	connectionX0->brick = connectionX1->brick = connectionZ0->brick = connectionZ1->brick = brick;
	connection2X0->brick = connection2X1->brick = connection2Z0->brick = connection2Z1->brick = brick2;

	connectionX0->other = connectionX1->other = connectionZ0->other = connectionZ1->other = brick2;
	connection2X0->other = connection2X1->other = connection2Z0->other = connection2Z1->other = brick;

	connectionX0->twin = connection2X0;
	connection2X0->twin = connectionX0;
	connectionX1->twin = connection2X1;
	connection2X1->twin = connectionX1;

	connectionZ0->twin = connection2Z0;
	connection2Z0->twin = connectionZ0;
	connectionZ1->twin = connection2Z1;
	connection2Z1->twin = connectionZ1;

	connectionX0->opposite = connectionX1;
	connectionX1->opposite = connectionX0;
	connectionZ0->opposite = connectionZ1;
	connectionZ1->opposite = connectionZ0;

	connection2X0->opposite = connection2X1;
	connection2X1->opposite = connection2X0;
	connection2Z0->opposite = connection2Z1;
	connection2Z1->opposite = connection2Z0;
	return true;
}

bool blockCallback(Collider* other, void* userData)
{
	Brick* brick = (Brick*)userData;
	Brick* brick2 = (Brick*)other->getUserData();


	for (int i = 0; i < brick->numConnections[X]; ++i)
	{	
		if (brick2->pos[1] - 0.5f == brick->connections[X][i].joint->y &&
			(brick2->pos[0] - 2.0f == brick->connections[X][i].joint->fulcrum ||
			brick2->pos[0] + 2.0f == brick->connections[X][i].joint->fulcrum))
		{
			++brick->connections[X][i].opposite->joint->blocked;
			brick2->blocking[brick2->numBlocking++] = brick->connections[X] + i;
		}
	}
	

	for (int i = 0; i < brick->numConnections[Z]; ++i)
	{
		if (brick2->pos[1] - 0.5f == brick->connections[Z][i].joint->y &&
			(brick2->pos[2] - 2.0f == brick->connections[Z][i].joint->fulcrum ||
			brick2->pos[2] + 2.0f == brick->connections[Z][i].joint->fulcrum))
		{
			++brick->connections[Z][i].opposite->joint->blocked;
			brick2->blocking[brick2->numBlocking++] = brick->connections[Z] + i;
		}
	}

	for (int i = 0; i < brick2->numConnections[X]; ++i)
	{
		if (brick->pos[1] - 0.5f == brick2->connections[X][i].joint->y &&
			(brick->pos[0] - 2.0f == brick2->connections[X][i].joint->fulcrum ||
			brick->pos[0] + 2.0f == brick2->connections[X][i].joint->fulcrum))
		{
			++brick2->connections[X][i].opposite->joint->blocked;
			brick->blocking[brick->numBlocking++] = brick2->connections[Z] + i;
		}
	}


	for (int i = 0; i < brick2->numConnections[Z]; ++i)
	{
		if (brick->pos[1] - 0.5f == brick2->connections[Z][i].joint->y &&
			(brick->pos[2] - 2.0f == brick2->connections[Z][i].joint->fulcrum ||
			brick->pos[2] + 2.0f == brick2->connections[Z][i].joint->fulcrum))
		{
			++brick2->connections[Z][i].opposite->joint->blocked;
			brick->blocking[brick->numBlocking++] = brick2->connections[Z]+ i;
		}
	}

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

	Brick* brick = m_bricks + m_numBricks++;
	brick->pos[0] = x;
	brick->pos[1] = y;
	brick->pos[2] = z;

	brick->collider = collider;
	brick->collider->setUserData(brick);
	brick->ship = this;

	m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x, y + 0.1, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), studCallback, brick);
	m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x, y - 0.1, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), studCallback, brick);

	m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x - 0.1, y, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);
	m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x + 0.1, y, z), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);
	m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x, y, z + 0.1), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);
	m_body->queryShape(g_brickShape, transformTransform(Transform(vec3(x, y, z - 0.1), Quaternion(vec3(0, 0, 0), 1)), m_body->getTransform()), blockCallback, brick);

	m_body->addCollider(collider);

	calcBase();
}

bool findAugmentedPath(Brick* brick, Brick* sink, int axis, float* minCapacity, std::vector<Connection*>* path, int tick)
{
	if (brick == sink)
		return true;

	brick->tick = tick;
	for (int i = 0; i < brick->numConnections[axis]; ++i)
	{
		Connection* connection = brick->connections[axis] + i;
		if (connection->other->tick != tick &&
			connection->joint->capacity - connection->flow > 0)
		{
			path->push_back(connection);
			if (findAugmentedPath(connection->other, sink, axis, minCapacity, path, tick))
			{
				if (connection->joint->capacity - connection->flow < *minCapacity)
					*minCapacity = connection->joint->capacity - connection->flow;
				return true;
			}
			else
			{
				path->pop_back();
			}
		}
	}
	return false;
}
float maxFlow(Brick* source, Brick* sink, int axis)
{
	std::vector<Connection*> augmentedPath;
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
	for (int i = 0; i < source->numConnections[axis]; ++i)
	{
		maxFlow += source->connections[axis][i].flow;
	}

	return maxFlow;
}

void breakGraph(Brick* brick, float maxFlow, int axis, std::vector<Brick*>* selection, std::vector<Connection*>* front, int tick)
{
	brick->tick = tick;
	selection->push_back(brick);
	for (int i = 0; i < brick->numConnections[axis]; ++i)
	{
		Connection* connection = brick->connections[axis] + i;
		if (connection->other->tick != tick)
		{
			if (connection->joint->capacity >= connection->flow / maxFlow)
			{
				if (connection->flow > 0)
					breakGraph(connection->other, maxFlow, axis, selection, front, tick);
			}
			else
			{
				front->push_back(connection);
			}
		}
	}
}

void Ship::destroy(std::vector<Brick*>* selection, std::vector<Connection*>* front, vec3 impulse, vec3 pos, int axis, int tick)
{
	//break front
	for (int i = 0; i < front->size(); ++i)
	{
		Connection* a = (*front)[i];
		Connection* b = a->twin;
		Connection* at = a->opposite;
		Connection* bt = b->opposite;

		*a = a->brick->connections[axis][--a->brick->numConnections[axis]];
		*b = b->brick->connections[axis][--b->brick->numConnections[axis]];

		*at = at->brick->connections[axis][--at->brick->numConnections[axis]];
		*bt = bt->brick->connections[axis][--bt->brick->numConnections[axis]];
	}

	BodyDescription descr;
	descr.transform = m_body->getTransform();
	descr.type = BodyType::Dynamic;
	descr.linearMomentum = impulse;
	descr.angularMomentum = vec3(0, 0, 0);

	Ship* newShip = createShip(m_world, m_entities, descr);
	

	for (int i = 0; i < selection->size(); ++i)
	{
		selection->at(i)->tick = tick;
	}

	// seperate selection
	for (int i = 0; i < selection->size(); ++i)
	{
		Brick* brick = selection->at(i);
		// free blocks
		for (int i = 0; i < brick->numBlocking; ++i)
		{
			if (brick->blocking[i]->brick->tick != tick)
			{
				brick->blocking[i]->joint->blocked--;
				brick->blocking[i] = brick->blocking[--brick->numBlocking];
			}
		}
		m_body->removeCollider(brick->collider);
		brick->ship = newShip;
		newShip->m_body->addCollider(brick->collider);
	}
	newShip->calcBase();
}


bool Ship::checkAxis(Brick* brick, vec3 impulse, vec3 pos, int axis)
{
	float max = maxFlow(brick, m_base, axis);

	if (max < 1.0f)
	{
		std::vector<Brick*> selection;
		std::vector<Connection*> front;
		breakGraph(brick, max, axis, &selection, &front, g_tick++);
		destroy(&selection, &front, impulse, pos, axis, g_tick++);
		return true;
	}
	return false;
}

bool Ship::addImpulse(Brick* brick, vec3 impulse, vec3 pos)
{
	Transform t = invTransform(m_body->getTransform());
	vec3 _pos = transformVec3(pos, t);
	vec3 _impulse = rotate(impulse, t.q);

	// clear flow network
	for (int i = 0; i < m_numBricks; ++i)
	{
		for (int axis = X; axis <= Z; ++axis)
		{
			for (int j = 0; j < m_bricks[i].numConnections[axis]; ++j)
			{
				Connection* connection = m_bricks[i].connections[axis] + j;
				//clear flow
				connection->flow = 0.0f;

				//set capacity
				if (axis == X)
					connection->joint->capacity =
					-connection->joint->baseCapacity / ((_pos.x - connection->joint->fulcrum) * _impulse.y - (_pos.y - connection->joint->y) * _impulse.x);
				else if (axis == Z)
					connection->joint->capacity =
					-connection->joint->baseCapacity / ((_pos.y - connection->joint->y) * _impulse.z - (_pos.z - connection->joint->fulcrum) * _impulse.y);

				if (connection->joint->blocked && connection->joint->capacity > 0)
				{
					connection->joint->capacity = 1.0f;
				}
				else
				{
					connection->joint->capacity = ong_MIN(1.0f, ong_MAX(0.0f, connection->joint->capacity));
				}
			}

		}
	}


	bool destroy = checkAxis(brick, impulse, pos, X);
	if (!destroy)
		destroy = checkAxis(brick, impulse, pos, Z);

	if (destroy)
		calcBase();

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

	for (int axis = X; axis <= Z; ++axis)
	{
		for (int i = 0; i < brick->numConnections[axis]; ++i)
		{
			Connection* connection = brick->connections[axis] + i;

			if (connection->flow != 0)
			{
				glLineWidth(10.0f * connection->flow);
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
				glVertex3f(brick->blocking[i]->brick->pos[0], brick->blocking[i]->brick->pos[1], brick->blocking[i]->brick->pos[2]);
				glEnd();
			}

			if (connection->joint->capacity > 0)
			{
				glLineWidth(connection->joint->capacity * 10.0f);
				glBegin(GL_LINES);
				if (axis == X)
				{
					glVertex3f(connection->joint->fulcrum, connection->joint->y, brick->pos[2] - 0.5f);
					glVertex3f(connection->joint->fulcrum, connection->joint->y, brick->pos[2] + 0.5f);
				}
				else if (axis == Z)
				{
					//glVertex3f(brick->pos[0] - 0.5f, connection->joint->y, connection->joint->fulcrum);
					//glVertex3f(brick->pos[0] + 0.5f, connection->joint->y, connection->joint->fulcrum);
				}
				glEnd();
				glLineWidth(1);
			}

			if (connection->other->tick != tick)
				renderBrick(connection->other, base, colorLocation, tick);
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