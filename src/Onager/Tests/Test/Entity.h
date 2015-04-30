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

	virtual void render(GLuint colorLocation);

protected:

	Body* m_body;
	vec3 m_color;
	int m_numVerts;
	GLuint m_vb;


};

