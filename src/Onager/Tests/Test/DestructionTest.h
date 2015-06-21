#pragma once


#include "test.h"


class DestructionTest : public Test
{
public:
	DestructionTest();
	void init();
	void update(float dt);
	bool procEvent(SDL_Event event) override;
private:
	int impulse();
	bool m_click;
	float m_force;
};