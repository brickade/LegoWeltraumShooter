#pragma once

#include "Entity.h"

class Ship;
struct Brick;
struct Connection;

static const int STUD_STRENGTH = 20;

void initDestruction(World* world);
void updateDestruction();

struct JointData
{
	float fulcrum;
	float y;
	float baseCapacity;
	int blocked;
};

struct Joint
{
	Joint** tableEntry;
	JointData* data;
	Connection* connection;
	Joint** twin;
	float capacity;
	float flow;
};

struct Connection
{
	Brick* brick;
	Brick* other;
	int dir;
	Joint joints[2][2];
	Joint verticalJoint;
};

static const int MAX_CONNECTIONS = 8;
struct Brick
{
	int pos[3];

	int numConnections;
	Connection connections[MAX_CONNECTIONS];

	int numBlocking;
	Joint** blocking[MAX_CONNECTIONS];

	Collider* collider;
	int lastBroken;
	Ship* ship;
	int tick;
};

Ship* createShip(World* world, std::vector<Entity*>* entities, BodyDescription descr, vec3 color);


class Ship : public Entity
{
public:
	Ship(std::vector<Entity*>* entities, World* world, Body* body, vec3 color);
	void addBrick(int x,int y, int z);
	void build();
	bool addImpulse(Brick* brick, vec3 pos, vec3 impulse);

	void update(float dt) override;

	void render(GLuint colorLocation) override;

private:
	friend void collisionCallback(Collider* collider, Contact* contact);

	struct Impulse
	{
		Brick* brick;
		vec3 pos;
		vec3 impulse;
		vec3 angular;
	};

	bool checkAxis(Brick * brick, vec3 impulse, vec3 pos, int axis);
	bool checkVertical(Brick* brick,float verticalImpulse, vec3 impulse, vec3 pos);
	void destroy(Brick* brick, std::vector<Brick*>* selection, std::vector<Joint**>* front,vec3 impulse, vec3 pos, int axis, int tick);

	void calcBase();
	void renderBrick(Brick* brick, Brick* base, GLuint colorLocation, int tick);

	int m_minAxis;

	Brick* m_base;
	World* m_world;
	std::vector<Entity*>* m_entities;
	std::vector<Impulse> m_impulses;
};

static int g_lastBroken = 0;

inline void collisionCallback(Collider* collider, Contact* contact)
{

	Brick* brick = (Brick*)collider->getUserData();
	Brick* brick2 = (Brick*)(contact->colliderA == collider ? contact->colliderB : contact->colliderA)->getUserData();

	if (brick->lastBroken > g_lastBroken - 5 || (brick2 && brick2->lastBroken > g_lastBroken - 5))
		return;

	int d = contact->colliderA == collider ? -1 : 1;
	
	vec3 pos = vec3(0,0,0);
	vec3 impulse = vec3(0,0,0);
	for (int i = 0; i < contact->manifold.numPoints; ++i)
	{
		pos += contact->manifold.points[i].position;
		impulse += d * contact->accImpulseN[i] * contact->manifold.normal;
	}
	pos = 1.0f / contact->manifold.numPoints * pos;

	//brick->ship->getBody()->applyImpulse(-impulse);
	//brick->ship->getBody()->applyAngularImpulse(cross(pos - brick->ship->getBody()->getWorldCenter(), -impulse));





	if (brick->ship->addImpulse(brick, pos, impulse))
	{
 		Collider* other = contact->colliderA == collider ? contact->colliderB : contact->colliderA;
		for (int i = 0; i < contact->manifold.numPoints; ++i)
		{
			vec3 impulse = contact->accImpulseN[i] * contact->manifold.normal + contact->accImpulseT[i] * contact->tangent + contact->accImpulseBT[i] * contact->biTangent;
			other->getBody()->applyImpulse((contact->colliderA == collider ? 1 : -1) * impulse, contact->manifold.points[i].position);
		}
		for (int i = 0; i < contact->manifold.numPoints; ++i)
		{
			printf("penettration: %f\n", contact->manifold.points[i].penetration);
			contact->accImpulseN[i] *= 0.0f;
			contact->accImpulseT[i] *= 0.0f;
			contact->accImpulseBT[i] *= 0.0f;
		}
	}

	//	brick->ship->m_impulses.push_back({ brick, 1.0f / contact->manifold.numPoints * pos, 1.0f / contact->manifold.numPoints * impulse });
};