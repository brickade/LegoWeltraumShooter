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

	void render(GLuint colorLocation) override;

private:
	bool checkAxis(Brick * brick, vec3 impulse, vec3 pos, int axis);
	bool checkVertical(Brick* brick,float verticalImpulse, vec3 impulse, vec3 pos);
	void destroy(std::vector<Brick*>* selection, std::vector<Joint*>* front,vec3 impulse, vec3 pos, int axis, int tick);

	void calcBase();
	void renderBrick(Brick* brick, Brick* base, GLuint colorLocation, int tick);

	int m_minAxis;

	Brick* m_base;
	World* m_world;
	std::vector<Entity*>* m_entities;
};


