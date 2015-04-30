#pragma once

#include "Entity.h"

class Bullet : public Entity
{
public:
	Bullet::Bullet(Body* body, vec3 color)
		: Entity(body, color),
		m_lifeTime(3.0f)
	{
	}


	void update(float dt)
	{
		if (m_lifeTime > 0)
			m_lifeTime -= dt;
		
		if (m_lifeTime < 0)
		{
			World* world = m_body->getWorld();
			world->destroyBody(m_body);
			m_body = nullptr;
			m_lifeTime = 0.0f;
		}
	}

private:

	float m_lifeTime;

};