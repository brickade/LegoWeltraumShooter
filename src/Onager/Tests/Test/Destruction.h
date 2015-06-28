#pragma once

#include "Entity.h"

class Ship;
struct Brick;


static const int STUD_STRENGTH = 20;

struct Joint
{
	float fulcrum;
	float y;
	float baseCapacity;
	float capacity;
	int blocked;
};

struct Connection
{
	Joint* joint;
	Brick* brick;
	Brick* other;
	Connection* twin;
	Connection* opposite;
	float flow;
};

static const int MAX_CONNECTIONS = 8;
struct Brick
{
	int numConnections[2];
	Connection connections[2][MAX_CONNECTIONS];
	int numBlocking;
	Connection* blocking[MAX_CONNECTIONS];
	int pos[3];
	Collider* collider;
	Ship* ship;
	int tick;
};

Ship* createShip(World* world, std::vector<Entity*>* entities, BodyDescription descr);

class Ship : public Entity
{
public:
	Ship(std::vector<Entity*>* entities, World* world, Body* body, vec3 color);
	void addBrick(int x,int y, int z);
	bool addImpulse(Brick* brick, vec3 impulse, vec3 pos);

	void render(GLuint colorLocation) override;

private:
	bool checkAxis(Brick * brick, vec3 impulse, vec3 pos, int axis);
	void destroy(std::vector<Brick*>* selection, std::vector<Connection*>* front,vec3 impulse, vec3 pos, int axis, int tick);
	void calcBase();


	Brick* m_base;
	World* m_world;
	std::vector<Entity*>* m_entities;

	int m_numBricks = 0;
	Brick m_bricks[256];
};


