#include "Destruction.h"


enum
{
	X = 0,
	Z = 1,
	Y = 2
};


static ShapePtr g_brickShape = {};
static Material g_material = { 1, 0, 1 };

static int g_numJoints;
static JointData g_joints[128];

static int g_numBricks;
static Brick g_bricks[32];

static int g_numJointTableEntries;
static Joint* g_jointTable[256];

static int g_tick = 0;



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
	


	connection->joints[X][0].data = connection2->joints[X][0].data = jointX0;
	connection->joints[X][1].data = connection2->joints[X][1].data = jointX1;
	connection->joints[Z][0].data = connection2->joints[Z][0].data = jointZ0;
	connection->joints[Z][1].data = connection2->joints[Z][1].data = jointZ1;


	for (int i = X; i <= Z; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			// set connection
			connection->joints[i][j].connection = connection;
			connection2->joints[i][j].connection = connection2;

			// set up jointable
			Joint** jointEntry = g_jointTable + g_numJointTableEntries++;
			Joint** jointEntry2 = g_jointTable + g_numJointTableEntries++;
			
			*jointEntry = connection->joints[i] + j;
			*jointEntry2 = connection2->joints[i] + j;

			connection->joints[i][j].tableEntry = jointEntry;
			connection2->joints[i][j].tableEntry = jointEntry2;

			// set twin
			connection->joints[i][j].twin = connection2->joints[i][j].tableEntry;
			connection2->joints[i][j].twin = connection->joints[i][j].tableEntry;
		}
	}
	

	connection->verticalJoint.connection = connection;
	connection->verticalJoint.tableEntry = g_jointTable + g_numJointTableEntries++;
	*connection->verticalJoint.tableEntry = &connection->verticalJoint;
	
	connection2->verticalJoint.connection = connection2;
	connection2->verticalJoint.tableEntry = g_jointTable + g_numJointTableEntries++;
	*connection2->verticalJoint.tableEntry = &connection2->verticalJoint;

	connection->verticalJoint.twin = connection2->verticalJoint.tableEntry;
	connection2->verticalJoint.twin = connection->verticalJoint.tableEntry;

	connection->verticalJoint.capacity = connection2->verticalJoint.capacity = numStuds * STUD_STRENGTH;


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
				brick2->blocking[brick2->numBlocking++] = brick->connections[i].joints[X][!j].tableEntry;
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
				brick2->blocking[brick2->numBlocking++] = brick->connections[i].joints[Z][!j].tableEntry;
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

	ColliderCallbacks callbacks;
	callbacks.postSolve = collisionCallback;

	collider->setCallbacks(callbacks);

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
		if (axis == Y)
		{
			Connection* connection = brick->connections + i;
			Joint* joint = &brick->connections[i].verticalJoint;
			if (connection->other->tick != tick &&
				(connection->dir > 0 ? joint->capacity : 0) - joint->flow > 0)
			{
				path->push_back(joint);
				if (findAugmentedPath(connection->other, sink, axis, minCapacity, path, tick))
				{
					if (joint->capacity - joint->flow < *minCapacity)
						*minCapacity = joint->capacity - joint->flow;
					return true;
				}
				else
				{
					path->pop_back();
				}
			}
		}
		else
		{
			for (int j = 0; j < 2; ++j)
			{
				Connection* connection = brick->connections + i;
				Joint* joint = brick->connections[i].joints[axis] + j;
				if (connection->other->tick != tick &&
					joint->capacity - joint->flow > 0)
				{
					path->push_back(joint);
					if (findAugmentedPath(connection->other, sink, axis, minCapacity, path, tick))
					{
						if (joint->capacity - joint->flow < *minCapacity)
							*minCapacity = joint->capacity - joint->flow;
						return true;
					}
					else
					{
						path->pop_back();
					}
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
			(*augmentedPath[i]->twin)->flow -= minCapacity;
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
		if ((*brick->blocking[i])->connection->brick->tick == selectionTick ||
			(*brick->blocking[i])->connection->other->tick == selectionTick)
		{
			(*brick->blocking[i])->data->blocked--;
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


bool checkShip(Brick* brick, int tick, Ship* ship)
{
	brick->tick = tick;

	assert(brick->ship == ship);
	if (brick->ship != ship)
		return false;
	for (int i = 0; i < brick->numConnections; ++i)
	{
		if (brick->connections[i].other->tick != tick)
			if (!checkShip(brick->connections[i].other, tick, ship))
				return false;
	}
	return true;
}


struct Trace
{
	int numBricks = 0;
	Brick bricks[32];
	int numJoints = 0;
	JointData joints[128];
	int numJointTabelEntries = 0;
	Joint* jointTable[256];

	std::vector<Joint**> front;
	std::vector<Brick*> selection;
};

std::vector<Trace> g_traces;

void makeTrace(std::vector<Joint**>& front, std::vector<Brick*>& selection)
{

	g_traces.push_back(Trace());

	Trace& trace = g_traces.back();
	trace.numBricks = g_numBricks;
	trace.numJoints = g_numJoints;
	trace.numJointTabelEntries = g_numJointTableEntries;

	memcpy(trace.bricks, g_bricks, sizeof(g_bricks));
	memcpy(trace.joints, g_joints, sizeof(g_joints));
	memcpy(trace.jointTable, g_jointTable, sizeof(g_joints));

	for (int i = 0; i < trace.numBricks; ++i)
	{ 
		Brick* brickSrc = g_bricks + i;
		Brick* brickDst = trace.bricks + i;

		for (int j = 0; j < g_bricks[i].numConnections; ++j)
		{
			Connection* connectionSrc = brickSrc->connections + j;
			Connection* connectionDst = brickDst->connections + j;

			connectionDst->brick = (connectionSrc->brick - g_bricks) + trace.bricks;
			connectionDst->other = (connectionSrc->other - g_bricks) + trace.bricks;
			for (int d = X; d < Z; ++d)
			{
				for (int k = 0; k < 2; ++k)
				{
					Joint* jointSrc = connectionSrc->joints[d] + k;
					Joint* jointDst = connectionDst->joints[d] + k;

					jointDst->data = (jointSrc->data - g_joints) + trace.joints;
					jointDst->connection = connectionDst;
					
					jointDst->tableEntry = (jointSrc->tableEntry - g_jointTable) + trace.jointTable;
					*jointDst->tableEntry = jointDst;

					jointDst->twin = (jointSrc->twin - g_jointTable) + trace.jointTable;
				}
			}

		}
	}

	for (int i = 0; i < front.size(); ++i)
	{
		trace.front.push_back(front[i] - g_jointTable + trace.jointTable);
	}

	for (int i = 0; i < selection.size(); ++i)
	{
		Brick* brick = (selection[i] - g_bricks) + trace.bricks;
		trace.selection.push_back(brick);
	}
}

void Ship::destroy(Brick* brick, std::vector<Brick*>* selection, std::vector<Joint**>* front, vec3 impulse, vec3 pos, int axis, int tick)
{
	makeTrace(*front, *selection);

	for (int i = 0; i < selection->size(); ++i)
	{
		selection->at(i)->tick = tick;
	}


	//break front

	vec3 minFulcrum = vec3(0,0,0);
	float minCapacity = FLT_MAX;

	for (int i = 0; i < front->size(); ++i)
	{
		Connection* a = (*(*front)[i])->connection;
		Connection* b = (*(*(*front)[i])->twin)->connection;

 		if ((*(*front)[i])->capacity < minCapacity && axis != Y)
		{
			Joint* opposite = a->joints[axis][0].tableEntry == (*front)[i] ? a->joints[axis] + 1 : a->joints[axis];
			minFulcrum.y = opposite->data->y;
			if (axis == X)
				minFulcrum.x = opposite->data->fulcrum;
			else if (axis == Z)
				minFulcrum.z = opposite->data->fulcrum;

			minCapacity = (*(*front)[i])->capacity;
		}

		if (a != a->brick->connections + a->brick->numConnections - 1)
		{
			*a = a->brick->connections[--a->brick->numConnections];

			for (int d = X; d <= Z; ++d)
			{
				for (int i = 0; i < 2; ++i)
				{
					a->joints[d][i].connection = a;
					*a->joints[d][i].tableEntry = a->joints[d] + i;
				}
			}
		}
		else
		{
			--a->brick->numConnections;
		}


		if (b != b->brick->connections + b->brick->numConnections - 1)
		{
			*b = b->brick->connections[--b->brick->numConnections];

			for (int d = X; d <= Z; ++d)
			{
				for (int i = 0; i < 2; ++i)
				{
					b->joints[d][i].connection = b;
					*b->joints[d][i].tableEntry = b->joints[d] + i;
				}
			}
		}
		else
		{
			--b->brick->numConnections;
		}
			
	}


	//
	for (int i = 0; i < g_numBricks; ++i)
	{
		for (int j = 0; j < g_bricks[i].numConnections; ++j)
		{
			Connection* connection = g_bricks[i].connections +j;

			for (int d = 0; d < 2; ++d)
			{
				for (int k = 0; k < 2; ++k)
				{
					assert(connection->joints[d][k].connection == connection);
					assert(connection->joints[d][k].data == (*connection->joints[d][k].twin)->data);
				}
			}
		}
	}
	//

	vec3 angularMomentum = vec3(0, 0, 0);


	vec3 fulcrum = transformVec3(minFulcrum, m_body->getTransform());
	vec3 r(0, 0, 0);
	r.y = pos.y - fulcrum.y;
	if (axis == X)
		r.x = pos.x - fulcrum.x;
	else if (axis == Z)
		r.z = pos.z - fulcrum.z;
	angularMomentum = cross(r, impulse);


	//BodyDescription descr;
	//descr.transform = m_body->getTransform();
	//descr.type = BodyType::Dynamic;
	//descr.linearMomentum = impulse + m_body->getLinearMomentum();
	//descr.angularMomentum = angularMomentum + m_body->getAngularMomentum();

	//////
	//descr.linearMomentum = vec3(0, 0, 0);
	//descr.angularMomentum = vec3(0, 0, 0);

	//Ship* newShip = createShip(m_world, m_entities, descr, vec3(0,0,1));
	
	int id = m_impulses.size();
	
	Impulse newImpulse;
	newImpulse.brick = brick;
	newImpulse.impulse = impulse;
	newImpulse.pos = pos;
	newImpulse.angular = angularMomentum;

	m_impulses.push_back(newImpulse);

	// seperate selection
	for (int i = 0; i < selection->size(); ++i)
	{
		Brick* brick = selection->at(i);

		// free blocks
		for (int i = 0; i < brick->numBlocking; ++i)
		{
			if ((*brick->blocking[i])->connection->brick->tick != tick)
			{
				(*brick->blocking[i])->data->blocked--;
				brick->blocking[i] = brick->blocking[--brick->numBlocking];
			}
		}
		//m_body->removeCollider(brick->collider);
		//brick->ship = newShip;
		//newShip->m_body->addCollider(brick->collider);

	

	}
	
	//

	//newShip->calcBase();
	//calcBase();

	//assert(checkShip(newShip->m_base, g_tick++, newShip));
	//assert(checkShip(m_base, g_tick++, this));

	checkForInvalidBlock(m_base, tick , g_tick++);
}

bool breakGraph(Brick* brick,Brick* base, float maxFlow, int axis, std::vector<Brick*>* selection, std::vector<Joint**>* front, int tick)
{
	brick->tick = tick;
	selection->push_back(brick);
	for (int i = 0; i < brick->numConnections; ++i)
	{
		Connection* connection = brick->connections + i;
		if (connection->other->tick != tick)
		{
			int valid = 0;
			for (int j = 0; j < 2; ++j)
			{
				if (connection->joints[axis][j].capacity > 0)
				{
					if (connection->joints[axis][j].capacity >= connection->joints[axis][j].flow / maxFlow)
					{
						if (!breakGraph(connection->other, base, maxFlow, axis, selection, front, tick))
							return false;
					}
					else
					{
						front->push_back(connection->joints[axis][j].tableEntry);
					}
					break;
				}
				else
				{
					valid++;
				}
			}

			if (valid >= 2)
				return false;
		}
	}

	return true;
}

bool breakVerticalGraph(Brick* brick, float maxFlow, float impulse, std::vector<Brick*>* selection, std::vector<Joint*>* front, int tick)
{
	brick->tick = tick;
	selection->push_back(brick);
	for (int i = 0; i < brick->numConnections; ++i)
	{
		Connection* connection = brick->connections + i;
		if (connection->other->tick != tick)
		{
			int valid = 0;
			
			Joint* joint = &connection->verticalJoint;
			if (joint->capacity >= impulse*(joint->flow/maxFlow))
			{
				breakVerticalGraph(connection->other, maxFlow, impulse, selection, front, tick);
			}
			else
			{
				front->push_back(joint);
			}

		}
	}

	return true;
}

void clearFlowNetwork(Brick* brick, vec3 impulse, vec3 pos, int tick)
{
	brick->tick = tick;
	for (int i = 0; i < brick->numConnections; ++i)
	{

		for (int d = X; d <= Z; ++d)
		{
			for (int j = 0; j < 2; ++j)
			{
				Joint* joint = brick->connections[i].joints[d]+ j;
				joint->flow = 0;


				if (d== X)
					joint->capacity =
					brick->connections[i].dir * joint->data->baseCapacity / ((pos.x - joint->data->fulcrum) * impulse.y - (pos.y - joint->data->y) *impulse.x);
				else if (d == Z)
					joint->capacity =
					-brick->connections[i].dir * joint->data->baseCapacity / ((pos.y - joint->data->y) * impulse.z - (pos.z - joint->data->fulcrum) * impulse.y);

				if (joint->data->blocked && joint->capacity > 0)
				{
					joint->capacity = 1.0f;
				}
				else
				{
					joint->capacity = ong_MIN(1.0f, ong_MAX(0.0f, joint->capacity));
				}
			}
		}

		brick->connections[i].verticalJoint.flow = 0.0f;

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
		std::vector<Joint**> front;

		int selectionTick = g_tick++;
		if (!breakGraph(brick, m_base, max, axis, &selection, &front, selectionTick))
			return false;

		for (int i = 0; i < front.size(); ++i)
		{
			{
				Brick* brick = (*front[i])->connection->brick;
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

			{
				Brick* brick = (*front[i])->connection->other;
				int numUp = 0;
				int numDown = 0;
				for (int j = 0; j < brick->numConnections; ++j)
				{
					Brick* other = brick->connections[j].other;
					if (other->tick == selectionTick)
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
		}


		destroy(brick, &selection, &front, impulse, pos, axis, g_tick++);
		return true;
	}
	return false;
}

//bool Ship::checkVertical(Brick* brick, float verticalImpulse, vec3 impulse, vec3 pos)
//{
//	float max = maxFlow(brick, m_base, Y);
//
//	if (max > 0 && max < verticalImpulse)
//	{
//		std::vector<Brick*> selection;
//		std::vector<Joint**> front;
//
//		int selectionTick = g_tick++;
//		if (!breakVerticalGraph(brick, max, verticalImpulse, &selection, &front, selectionTick))
//			return false;
//
//		for (int i = 0; i < front.size(); ++i)
//		{
//			Brick* brick = (*front[i])->connection->brick;
//			int numUp = 0;
//			int numDown = 0;
//			for (int j = 0; j < brick->numConnections; ++j)
//			{
//				Brick* other = brick->connections[j].other;
//				if (other->tick != selectionTick)
//				{
//					if (other->pos[1] > brick->pos[1])
//						numUp++;
//					else if (other->pos[1] < brick->pos[1])
//						numDown++;
//				}
//				if (numUp > 0 && numDown > 0)
//					return false;
//			}
//
//
//		}
//
//
//		destroy(&selection, &front, impulse, pos, Y, g_tick++);
//	}
//
//	return false;
//}

bool Ship::addImpulse(Brick* brick, vec3 pos, vec3 impulse)
{
	if (brick == m_base)
		return false;

	Transform t = invTransform(m_body->getTransform());
	vec3 _pos = transformVec3(pos, t);
	vec3 _impulse = rotate(impulse, t.q);

	m_minAxis = abs(_impulse.x) > abs(_impulse.z) ? X : Z;

	// clear flow network
	clearFlowNetwork(brick, _impulse, _pos, g_tick++);

	//bool destroy = false;

	bool destroy = checkAxis(brick, impulse, pos, X);

	if (!destroy)
		destroy = checkAxis(brick, impulse, pos, Z);

	//if (!destroy)
	//	destroy = checkVertical(brick, abs(_impulse.y), impulse, pos);

	//if (destroy)
	//{
	//	m_body->applyImpulse(-impulse, pos);
	//}

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

void Ship::renderBrick(Brick* brick, Brick* base, GLuint colorLocation, int tick)
{
	brick->tick = tick;

	for (int j = 0; j < brick->numConnections; ++j)
	{

		Connection* connection = brick->connections + j;
		//for (int axis = Z; false; ++axis)
		{
			int axis = m_minAxis;


			for (int i = 0; i < 2; ++i)
			{
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
					glVertex3f((*brick->blocking[i])->connection->brick->pos[0], (*brick->blocking[i])->connection->brick->pos[1], (*brick->blocking[i])->connection->brick->pos[2]);
					glEnd();
				}

				if (joint->capacity > 0)
				{
					glLineWidth(joint->capacity * 5.0f);
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

			}
		}
		if (connection->other->tick != tick)
			renderBrick(connection->other, base, colorLocation, tick);
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


void setShip(Brick* brick, Ship* newShip, int tick)
{
	brick->tick = tick;
	brick->lastBroken = g_lastBroken;
	brick->ship->getBody()->removeCollider(brick->collider);
	brick->ship = newShip;
	newShip->getBody()->addCollider(brick->collider);

	for (int i = 0; i < brick->numConnections; ++i)
	{
		if (brick->connections[i].other->tick != tick)
		{
			setShip(brick->connections[i].other, newShip, tick);
		}
	}
}



void Ship::update(float dt)
{
	for (int i = 0; i < m_impulses.size(); ++i)
	{
		BodyDescription descr;
		descr.transform = m_body->getTransform();
		descr.type = BodyType::Dynamic;
		descr.linearMomentum = m_body->getLinearMomentum();
		descr.angularMomentum = m_body->getAngularMomentum();

		//////
		//descr.linearMomentum = vec3(0, 0, 0);
		//descr.angularMomentum = vec3(0, 0, 0);

		Ship* newShip = createShip(m_world, m_entities, descr, vec3(0,0,1));

		setShip(m_impulses[i].brick, newShip, g_tick++);
		newShip->getBody()->applyImpulse(m_impulses[i].impulse);
		newShip->getBody()->applyAngularImpulse(m_impulses[i].angular);
		newShip->calcBase();

	}
	calcBase();
	m_impulses.clear();
}


void initDestruction(World* world)
{
	ShapeDescription sDescr;
	sDescr.constructionType = ShapeConstruction::HULL_FROM_BOX;
	sDescr.hullFromBox.c = vec3(0, 0, 0);
	sDescr.hullFromBox.e = vec3(2, 0.5, 1);
	g_brickShape = world->createShape(sDescr);

	g_numJoints = 0;
	g_numBricks = 0;
	g_numJointTableEntries = 0;

	memset(g_bricks, 0, sizeof(g_bricks));
	memset(g_joints, 0, sizeof(g_joints));
	memset(g_jointTable, 0, sizeof(g_jointTable));

	g_traces.clear();
	g_traces.reserve(64);
}

void updateDestruction()
{
	g_lastBroken++;
}