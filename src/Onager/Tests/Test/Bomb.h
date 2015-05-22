#pragma once

#include "Entity.h"

class Bomb : public Entity
{
public:
	Bomb::Bomb(Body* body, vec3 color)
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
			
			Sphere s;
			s.c = vec3(0, 0, 0);
			s.r = 20.0f;
			
			auto callback = [](Collider* collider, void* bomb) ->bool
			{
				if (collider->getBody() != bomb)
				{
					vec3 p1 = collider->getBody()->getWorldCenter();;
					vec3 p2 = ((Body*)bomb)->getWorldCenter();
					
					float explosionStrength = 13000.0f;
					collider->getBody()->applyImpulse(explosionStrength / lengthSq(p1 - p2) * normalize(p1 - p2));
				}
				return true;
			};

			world->queryShape(ShapePtr(&s), m_body->getTransform(), callback, m_body);
			
			world->destroyBody(m_body);
			m_body = nullptr;
			m_lifeTime = 0.0f;
		}
	}

private:

	float m_lifeTime;

};