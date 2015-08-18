#pragma once

#include "test.h"
#include "BrickLoader.h"

class BrickTest : public Test
{
public:
	virtual void init()
	{
		m_world = new World();
	
		a = loadBrick("engine_3.brick", m_world);
		b = loadBrick("engine_3.brick", m_world);

		m_entities.push_back(new Entity(a, vec3(1, 0, 0)));
		m_entities.push_back(new Entity(b, vec3(0, 1, 0)));

		b->setPosition(vec3(1, 0, 0));

		//a->removeCollider(a->getCollider()->getNext()->getNext());
		//a->removeCollider(a->getCollider()->getNext());
		//b->removeCollider(b->getCollider()->getNext()->getNext());
		//b->removeCollider(b->getCollider()->getNext());

	}

	void update(float dt) override
	{
		bool ovrl = false;
		Collider* coll = b->getCollider();
		for (int i = 0; i < b->getNumCollider(); coll = coll->getNext(), ++i)
		{
			ovrl |= a->queryCollider(coll);
		}


		printf("%s\n", ovrl ? "true" : "false");

		const uint8* keys = SDL_GetKeyboardState(0);
		
		if (keys[SDL_SCANCODE_KP_4])
			b->setPosition(b->getPosition() + vec3(-dt, 0, 0));
		if (keys[SDL_SCANCODE_KP_6])	
			b->setPosition(b->getPosition() + vec3(dt, 0, 0));
		if (keys[SDL_SCANCODE_KP_8])	
			b->setPosition(b->getPosition() + vec3(0, 0, dt));
		if (keys[SDL_SCANCODE_KP_2])	
			b->setPosition(b->getPosition() + vec3(0, 0, -dt));
		if (keys[SDL_SCANCODE_KP_7])	
			b->setPosition(b->getPosition() + vec3(0, -dt, 0));
		if (keys[SDL_SCANCODE_KP_9])	
			b->setPosition(b->getPosition() + vec3(0, dt, 0));
	}

private:
	Body* a;
	Body* b;
};