#pragma once


#include "test.h"


class DestructionTest : public Test
{
public:
	DestructionTest();
	void init();
	void update(float dt);
private:
	float m_force;
};