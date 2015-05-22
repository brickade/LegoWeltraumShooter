#pragma once

#include "World.h"
#include "Body.h"
#include <GL\glew.h>


using namespace ong;

class Entity
{
public:
	Entity(Body* body, vec3 color);
	~Entity();

	virtual void update(float dt) {};

	bool isDead() { return m_hp <= 0; };
	
	void damage(int damage)
	{
		m_hp -= damage; 
	};

	Body* getBody()
	{
		return m_body;
	}

	int getHP()
	{
		return m_hp;
	}

	virtual void render(GLuint colorLocation);

	void destroy(std::vector<Entity*>& entities);

protected:

	int m_hp;

	Body* m_body;
	vec3 m_color;
	int m_numVerts;
	GLuint m_vb;




};

