#pragma once


#include "test.h"




class StressTest : public Test
{
public:
	void init();
	//void update(float dt);
	//bool procEvent(SDL_Event event);
	//void render();
private:
	Material m_Material;
};