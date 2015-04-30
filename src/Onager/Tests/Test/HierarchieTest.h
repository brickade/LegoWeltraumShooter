#pragma once


#include "test.h"




class HierarchieTest : public Test
{
public:
	void init();
	void update(float dt);
	bool procEvent(SDL_Event event);
	void render();
private:
	Body* m_body;
	Collider* m_collider;

	Material m_Material;
};