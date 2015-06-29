#pragma once

#include "Entity.h"

class Ship;
struct Brick;
struct Connection;

static const int STUD_STRENGTH = 50;

void initDestruction(World* world);

struct JointData
{
	float fulcrum;
	float y;
	float baseCapacity;
	int blocked;
};

struct Joint
{
	JointData* data;
	Connection* connection;
	Joint* twin;
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
	Joint* blocking[MAX_CONNECTIONS];

	Collider* collider;
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
	bool addImpulse(Brick* brick, vec3 impulse, vec3 pos);

	void update(float dt) override;

	void render(GLuint colorLocation) override;

private:
	friend void collisionCallback(Collider* collider, Contact* contact);

	struct Impulse
	{
		Brick* brick;
		vec3 pos;
		vec3 impulse;
	};

	bool checkAxis(Brick * brick, vec3 impulse, vec3 pos, int axis);
	bool checkVertical(Brick* brick,float verticalImpulse, vec3 impulse, vec3 pos);
	void destroy(std::vector<Brick*>* selection, std::vector<Joint*>* front,vec3 impulse, vec3 pos, int axis, int tick);

	void calcBase();
	void renderBrick(Brick* brick, Brick* base, GLuint colorLocation, int tick);

	int m_minAxis;

	Brick* m_base;
	World* m_world;
	std::vector<Entity*>* m_entities;
	std::vector<Impulse> m_impulses;

};

inline void collisionCallback(Collider* collider, Contact* contact)
{
	Brick* brick = (Brick*)collider->getUserData();
	int d = contact->colliderA == collider ? -1 : 1;
	
	vec3 pos = vec3(0,0,0);
	vec3 impulse = vec3(0,0,0);
	for (int i = 0; i < contact->manifold.numPoints; ++i)
	{
		pos += contact->manifold.points[i].position;
		impulse += d * contact->accImpulseN[i] * contact->manifold.normal;
	}

	brick->ship->m_impulses.push_back({ brick, 1.0f / contact->manifold.numPoints * pos, 1.0f / contact->manifold.numPoints * impulse });
};